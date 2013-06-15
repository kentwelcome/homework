/*
 *  Model.c
 *  OpenGL_HW1
 *
 *  Created by Kent Huang on 2011/3/31.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "graph.h"

void Reload_Model(char *Model_Name, int Model_index)
{
	char Path_Name[32];
	printf("End of running model %s.\n",Model_Name);
	free(Vertex_Coordinates);
	free(Vertex_Indices);
	free(Vertex_Normal);
	sprintf(Path_Name,"Models/%s.txt",Model_Name);
	printf("Load model %s\n",Model_Name);
    Read_Model_From_File(Path_Name);
	Normalization();
}

int Read_Model_From_File(char *Model_Name)
{
	FILE *Model;
	char title[32];
	int	 index;
	GLfloat  x, y, z;
	GLint	 p1, p2, p3;
	GLfloat *v1, *v2, *v3;
	GLfloat *N1, *N2, *N3;
	GLfloat  N[3];
	
	Model = fopen(Model_Name, "r");
	if (!Model) {
		perror("No such input file.\n");
		return -1;
	}
	// Startup to reading Model file
	//printf("Start prasing Model file.\n");
	fscanf(Model, "%s", title);
	if (strcmp(title, "Vertices") != 0) {
		perror("Input file syntax error!\n");
		return -1;
	}
	
	fscanf(Model,"%d",&Vertex_Number);
	Vertex_Coordinates = malloc( 3*Vertex_Number*sizeof(GLfloat) );
		for ( index = 0 ; index < Vertex_Number ; index++ ) {
		fscanf(Model,"%f%f%f",&x,&y,&z);
		
		// Mark the edge of the model
		if (index == 0){
			x_max = x_min = x;
			y_max = y_min = y;
			z_max = z_min = z;
		} else {
			// Edge of X 
			if (x_max < x) {
				x_max = x;
			} else if (x_min > x) {
				x_min = x;
			}
			// Edge of Y
			if (y_max < y) {
				y_max = y;
			} else if (y_min > y) {
				y_min = y;
			}
			// Edge of Z
			if (z_max < z) {
				z_max = z;
			} else if (z_min > z) {
				z_min = z;
			}
		}
		
		Vertex_Coordinates[3*index] = x;
		Vertex_Coordinates[3*index+1] = y;
		Vertex_Coordinates[3*index+2] = z;
	}
	
	// read the triangle list
	while (fscanf(Model,"%s",title) != EOF){
		if (strcmp("Triangle_list", title) == 0 || strcmp("Triangle_list_vn", title) == 0){
			fscanf(Model, "%d", &Triangle_Number);
			Vertex_Indices = malloc(3*Triangle_Number*sizeof(GLint));
			
			if (strcmp("Triangle_list_vn", title) != 0) {
				Vertex_Normal = calloc(Vertex_Number*3, sizeof(GLfloat));
			}
			
			for (index = 0; index < Triangle_Number ; index++) {
				fscanf(Model, "%d",&p1);
				fscanf(Model, "%d",&p2);
				fscanf(Model, "%d",&p3);
				
				Vertex_Indices[3*index]   = p1;
				Vertex_Indices[3*index+1] = p2;
				Vertex_Indices[3*index+2] = p3;
				
				// Each
				v1 = &Vertex_Coordinates[3*p1];
				v2 = &Vertex_Coordinates[3*p2];
				v3 = &Vertex_Coordinates[3*p3];
				
				if (strcmp("Triangle_list_vn", title) != 0) {
					// Normal 
					N1  = &Vertex_Normal[3*p1];
					N2  = &Vertex_Normal[3*p2];
					N3  = &Vertex_Normal[3*p3];
					
					// Calculate the normal of each point
					CalculateNormal(v1, v3, v2, N);
					VectorAdd(N1, N);
					VectorAdd(N2, N);
					VectorAdd(N3, N);
				}
			}
		} else if (strcmp("Normals", title) == 0) {
			fscanf(Model, "%d", &Vertex_Number);
			Vertex_Normal = calloc(Vertex_Number*3, sizeof(GLfloat));
			for (index = 0; index < Vertex_Number; index++) {
				fscanf(Model,"%f%f%f",&x,&y,&z);
				Vertex_Normal[3*index]   = x;
				Vertex_Normal[3*index+1] = y;
				Vertex_Normal[3*index+2] = z;
			}
		}
	}
	//printf("End of parsing Model file.\n");
	return 1;
}

int Normalization()
{
	GLfloat Move_X, Move_Y, Move_Z;
	GLfloat Scaling = 1;
	int		index;
	Move_X = (x_max + x_min)/2;
	Move_Y = (y_max + y_min)/2;
	//Move_Z = (z_min);
	Move_Z = (z_max + z_min)/2;
	
	// Caculate the scale when model is too large or too small
	if (x_max > SCREEN_HEIGHT/2 || 
		y_max > SCREEN_WIDTH/2 || 
		z_max > SCREEN_WIDTH/2){ 
		// reduce vertexs value
		if (x_max > y_max) {
			if (x_max > z_max) {
				Scaling = (SCREEN_HEIGHT/2) / (x_max + 10);
			} else {
				Scaling = (SCREEN_WIDTH/2) / (z_max + 10);
			}

		} else {
			if (y_max > z_max) {
				Scaling = (SCREEN_WIDTH/2) / (y_max + 10);
			} else {
				Scaling = (SCREEN_WIDTH/2) / (z_max + 10);
			}
		} 
		
	} else if (x_max < SCREEN_HEIGHT/2 && 
			   y_max < SCREEN_WIDTH/2 && 
			   z_max < SCREEN_WIDTH/2) { 
		// enlarge vertexs value
		do {
			Scaling = Scaling*1.2;
		} while (x_max*Scaling < SCREEN_HEIGHT/2 && 
				 y_max*Scaling < SCREEN_WIDTH/2 && 
				 z_max*Scaling < SCREEN_WIDTH/2);
	}
	
	// Normalize all vertexs
	for (index = 0 ; index < Vertex_Number ; index++) {
		Vertex_Coordinates[3*index] = (Vertex_Coordinates[3*index] - Move_X)*Scaling;
		Vertex_Coordinates[3*index+1] = (Vertex_Coordinates[3*index+1] - Move_Y)*Scaling;
		Vertex_Coordinates[3*index+2] = (Vertex_Coordinates[3*index+2] - Move_Z)*Scaling;
	}
	x_max = (x_max - Move_X)*Scaling; x_min = (x_min - Move_X)*Scaling;
	y_max = (y_max - Move_Y)*Scaling; y_min = (y_min - Move_Y)*Scaling;
	z_max = (z_max - Move_Z)*Scaling; z_min = (z_min - Move_Z)*Scaling;
	printf("X Max:%f\tMin:%f\n",x_max,x_min);
	printf("Y Max:%f\tMin:%f\n",y_max,y_min);
	printf("Z Max:%f\tMin:%f\n",z_max,z_min);
	return 0;
}

// 向量外積
void CalculateNormal(const GLfloat *Vert1, const GLfloat *Vert2, const GLfloat *Vert3, GLfloat *N)
{
	GLfloat Qx, Qy, Qz, Px, Py, Pz;
	GLfloat len;
	
	Qx = Vert2[0]-Vert1[0];
	Qy = Vert2[1]-Vert1[1];
	Qz = Vert2[2]-Vert1[2];
	Px = Vert3[0]-Vert1[0];
	Py = Vert3[1]-Vert1[1];
	Pz = Vert3[2]-Vert1[2];
	
	N[0] = Py*Qz - Pz*Qy;
	N[1] = Pz*Qx - Px*Qz;
	N[2] = Px*Qy - Py*Qx;

	len = sqrt(N[0]*N[0]+N[1]*N[1]+N[2]*N[2]);
	if (len != 0) {
		N[0] = N[0]/len;
		N[1] = N[1]/len;
		N[2] = N[2]/len;
	}
}

void VectorAdd(GLfloat *V1, GLfloat *V2)
{
	GLfloat N[3];
	GLfloat len;
	
	N[0] = V1[0] + V2[0];
	N[1] = V1[1] + V2[1];
	N[2] = V1[2] + V2[2];
	len = sqrt(N[0]*N[0]+N[1]*N[1]+N[2]*N[2]);
	
	if (len != 0){
		V1[0] = N[0]/len;
		V1[1] = N[1]/len;
		V1[2] = N[2]/len;
	}
}

void Model_Display()
{
	int Triangle_index, Point;
	int p;
	GLfloat v1, v2, v3;
	GLfloat N1, N2, N3;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	// Enable DEPTH_TEST
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	
	glPushMatrix();
	glRotatef((GLfloat)Vr_x, 1, 0, 0);
	glRotatef((GLfloat)Vr_y, 0, 1, 0);
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	
	
	// Read the Model
	glBegin(GL_TRIANGLES);
	
	for (Triangle_index = 0 ; Triangle_index < Triangle_Number ; Triangle_index++) {
		p = 3*Triangle_index;
		for (Point = 0; Point < 3 ; Point++) {
			p = 3*Triangle_index + Point;
			v1 = Vertex_Coordinates[3*Vertex_Indices[p]];
			v2 = Vertex_Coordinates[3*Vertex_Indices[p]+1];
			v3 = Vertex_Coordinates[3*Vertex_Indices[p]+2];
			// Normal 
			N1 = Vertex_Normal[3*Vertex_Indices[p]];
			N2 = Vertex_Normal[3*Vertex_Indices[p]+1];
			N3 = Vertex_Normal[3*Vertex_Indices[p]+2];
			// Draw the Model
			glNormal3f(N1, N2, N3);
			glVertex3f(v1, v2, v3);
		}
	}
	glEnd();
	glPopMatrix();
	
	glutSwapBuffers();
	glFlush();
	glutPostRedisplay();
}
