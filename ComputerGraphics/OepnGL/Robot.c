/*
 *  Robot.c
 *  OpenGL_HW1
 *
 *  Created by Kent Huang on 2011/5/3.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "graph.h"

void Robot_Display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	
	// groble view
	glPushMatrix();
	
	
	glRotatef((GLfloat)Vr_x, 1, 0, 0);
	glRotatef((GLfloat)Vr_y, 0, 1, 0);
	glTranslatef(Robot_Move_X, Robot_Move_Y, Robot_Move_Z);
	
	
	// draw head
	glPushMatrix();
	glTranslatef(0, 100, 0);
	glScalef(1.5, 1.0, 1.0);
	glutSolidCube(100.0);
	glPopMatrix();
	// draw body
	glPushMatrix();
	//glScalef(1.5, 2.0, 1.5);
	//glutSolidCube(50);
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 0, -50);
	gluCylinder(quadratic,38,38,100,32,32);
	// draw disk
	glPushMatrix();
	glTranslatef(0,0,100);
	gluDisk(quadratic,0,38,32,32);
	glPopMatrix();
	
	glPopMatrix();
	// draw righ hand
	glPushMatrix();
	glTranslatef(55, 0, 0);
	
	
	// Rotage hand
	glTranslatef(0, 30, 0);
	glRotatef(Arm_righ_degree, 1, 0, 0);
	glTranslatef(0, -30, 0);
	glRotatef(15, 0, 0, 1);		
	
	glScalef(1, 3, 1);
	glutSolidCube(20);
	glPopMatrix();
	// draw left hand
	glPushMatrix();
	glTranslatef(-55, 0, 0);
	
	glTranslatef(0, 30, 0);
	glRotatef(Arm_left_degree, 1, 0, 0);
	glTranslatef(0, -30, 0);
	glRotatef(-15, 0, 0, 1);		
	
	glScalef(1, 3, 1);
	glutSolidCube(20);
	glPopMatrix();
	// draw righ leg
	glPushMatrix();
	glTranslatef(15, -70, 0);
	
	glTranslatef(0, 15, 0);
	glRotatef(Leg_righ_degree, 1, 0, 0);
	glTranslatef(0, -15, 0);	
	
	glScalef(1, 1.1, 1);
	glutSolidCube(25);
	glPopMatrix();
	// draw left leg
	glPushMatrix();
	glTranslatef(-15, -70, 0);
	
	glTranslatef(0, 15, 0);
	glRotatef(Leg_left_degree, 1, 0, 0);
	glTranslatef(0, -15, 0);	
	
	glScalef(1, 1.1, 1);
	glutSolidCube(25);
	glPopMatrix();
	glPopMatrix();
	glutSwapBuffers();
	glFlush();
	
	// Move of Arm
	Arm_righ_degree = 60*sin(Thita);
	Arm_left_degree = 60*sin(Thita+3.14);
	Leg_righ_degree = 30*sin(Thita+3.14);
	Leg_left_degree = 30*sin(Thita);
	
	
	// Rotate hands and legs
	Thita = Thita + 0.05;
	if (Thita >= 2*3.14) {
		Thita = 0;
	}
	
	Robot_Move_Z++;
	if (Robot_Move_Z > 400) {
		Robot_Move_Z = -400;
	}
	
	usleep(10000);
	
	glutPostRedisplay();
	
}
