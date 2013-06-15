/*
 *  graph.h
 *  OpenGL_HW1
 *
 *  Created by Kent Huang on 2011/3/31.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <GLUT/glut.h>

// definition
#define	SCREEN_HEIGHT	600
#define	SCREEN_WIDTH	600
#define SCREEN_SIZE		600

// structure
typedef struct VECTOR {
	GLfloat x;
	GLfloat y;
	GLfloat z;
} Vector;


// Groble veriable
GLfloat *Vertex_Coordinates;
GLint	*Vertex_Indices;
GLfloat *Vertex_Normal;
GLint	Vertex_Number;
GLint	Triangle_Number;
GLfloat x_max , x_min;
GLfloat y_max , y_min;
GLfloat z_max , z_min;

GLUquadricObj   *quadratic;


int md_num;

int	MousePush;

GLint Vr_x, Vr_y, Vr_z; // viewpoint rotate
GLint Arm_left_degree, Arm_righ_degree;
GLint Leg_left_degree, Leg_righ_degree;
GLint Robot_Move_X, Robot_Move_Y, Robot_Move_Z;

GLdouble Thita;	
GLint Mouse_x, Mouse_y;
GLint year ,day; 


// function definition
int Read_Model_From_File(char *Model_Name);
int Normalization();
void Reload_Model(char *Model_Name, int Model_index);
void CalculateNormal(const GLfloat *Vert1, const GLfloat *Vert2, const GLfloat *Vert3, GLfloat *N);
void VectorAdd(GLfloat *V1, GLfloat *V2);

void Robot_Display();
void Model_Display();