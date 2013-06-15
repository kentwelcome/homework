#include "graph.h"


char models[11][16] = {
	"Armadillo3K",
	"bunny",
	"dolphin",
	"cube",
	"dragon",
	"happy5K",
	"horse1K",
	"sphere",
	"teapot",
	"tiger",
	"toruses"};


GLfloat light_ambient[] = { 0.5, 0.0, 0.2, 1.0 };  /* abvient light. */
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };  /* diffuse light. */
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 }; /* specular light.*/
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 }; 
GLfloat mat_shininess[] = { 50.0 };
GLfloat lmodel_ambient[] = { 0.1, 0.1, 0.1, 1.0 };

GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};  /* Infinite light location. */
//GLUquadricObj   *quadratic;


void ChangeWindowsSize(int width , int height)
{
	GLfloat near, far;
	
	near = -SCREEN_SIZE;
	far	 = SCREEN_SIZE;
	// Prevent a divide by zero
	if (height == 0) {
		height = 1;
	}
	
	// Set Viewport to window dimensions
	glViewport(0, 0, width, height);
	
	// Reset coordinate system
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	
	// Establish clipping volume (left, right, bottom, top, near, far)
	GLfloat aspectRatio = (GLfloat)width / (GLfloat)height;
	if (width <= height) {
		glOrtho(-SCREEN_SIZE, SCREEN_SIZE, -SCREEN_SIZE/aspectRatio, SCREEN_SIZE/aspectRatio, near, far);
	} else {
		glOrtho(-SCREEN_SIZE*aspectRatio, SCREEN_SIZE*aspectRatio, -SCREEN_SIZE, SCREEN_SIZE, near, far);
	}
	//gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}

void Solar_Display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glColor3f(1.0, 1.0, 1.0);
	glPushMatrix();
	glRotatef(Vr_x, 0, 1, 0);
	glRotatef(Vr_y, 1, 0, 0);
	glutWireSphere(100, 20, 16); // draw sun 
	glRotatef(day, 0.0, 1.0, 0.0);
	glTranslatef(200.0, 0.0, 0.0);
	glRotatef(day, 0.0, 1.0, 0.0);
	glutWireSphere(20, 10, 8);
	glPopMatrix();
	glutSwapBuffers();
	
	day++;
	if (day>360){
		day = 0;
		year++;
	}
	glutPostRedisplay();
	glFlush();
}


void SetupRC()
{
	// Set clear color to black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glShadeModel(GL_SMOOTH);
	
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.5);  // Kc
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.5);	// Kl
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.5); // Kq
	
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); 
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	
	quadratic=gluNewQuadric();					 //   創建二次幾何體
	gluQuadricNormals(quadratic,   GLU_SMOOTH);	 //   使用平滑法線
	gluQuadricTexture(quadratic,   GL_TRUE);	 //   使用紋理
	
}

void keyboard(unsigned char key, int x, int y)
{
	//char model_name[32];
	switch (key) {
		case ',':
			md_num = (md_num+1)%11;
			Reload_Model(models[md_num],md_num);
			SetupRC();
			break;
		case '.':
			md_num = (md_num-1+11)%11;
			Reload_Model(models[md_num],md_num);
			SetupRC();
			break;
		// View Point Rotate Y
		case 'a':
			Vr_y = (Vr_y+360-1)%360;
			break;
		case 'd':
			Vr_y = (Vr_y+1)%360;
			break;
		// View Point Rotate X
		case 'w':
			Vr_x = (Vr_x + 360 -1)%360;
			break;
		case 's':
			Vr_x = (Vr_x + 1)%360;
			break;
		default:
			return;
			break;
	}
	
	glutPostRedisplay();
}

void MouseButton(int button, int state, int x, int y)
{
	switch (button) {
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN) {
				//printf("Mouse Down\tX:%d\tY:\t%d\n",x,y);
				MousePush = 1;
				Mouse_x = x;
				Mouse_y = y;
			} else if (state == GLUT_UP) {
				MousePush = 0;
			}
			break;
		default:
			break;
	}
}

void MouseMove(int x, int y)
{
	if (MousePush) {
		Vr_y = (Vr_y + 360 + (x - Mouse_x) )%360;
		Vr_x = (Vr_x + 360 + (y - Mouse_y) )%360;
		Mouse_x = x;
		Mouse_y = y;
	}
	glutPostRedisplay();
}

int main (int argc, char *argv[]) {
    // insert code here...
	//int Triangle_index;
	//int	Point , p;
	//GLfloat v1, v2, v3;
	int i;
	char model_name[32];
	
	for (i = 0 ;  i < 11 ; i++) {
		printf("%s\n",models[i]);
	}
	
	year = 0;
	day = 0;
	MousePush = 0;
	md_num = 0;
	
	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH); 
	glutInitWindowSize(SCREEN_HEIGHT,SCREEN_WIDTH);		//視窗長寬 
	glutInitWindowPosition(80,80);		//視窗左上角的位置 
	glutCreateWindow("OpenGL HW 1");	//建立視窗
	sprintf(model_name,"Models/%s.txt",models[md_num]);
	printf("Load model %s\n",models[md_num]);
    Read_Model_From_File(model_name);
	Normalization();
	glutDisplayFunc(Model_Display); 
	//glutDisplayFunc(Solar_Display);
	//glutDisplayFunc(Robot_Display);
	glutReshapeFunc(ChangeWindowsSize); 
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMove);
	glutKeyboardFunc(keyboard);
	SetupRC();
	
	glutMainLoop(); 
	// Free data
	printf("End of running model %s.\n",models[md_num]);
	free(Vertex_Coordinates);
	free(Vertex_Indices);
    return 0;
}


