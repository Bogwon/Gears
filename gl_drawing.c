
#include <assert.h>
#include <stdio.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include "structure.h"		// global data
#include "draw_gears.h"
#include "load_data.h"

#pragma warning(disable: 4244)	// 238 warnings: conversion from 'double' to 'float', possible loss of data



void Generate_Display_Lists(void)	// Called only by main
{
	int			i;
	GLfloat		light0Pos	[4]	= { 0.70F, 0.70F, 1.25F, 0.50F };
	GLfloat		mat			[4];

	glClearColor	(background[0],	background[1],	background[2], 1.0F);
	glMaterialf		(GL_FRONT_AND_BACK,	GL_SHININESS,	20.0);
	glLightfv		(GL_LIGHT0,			GL_POSITION,	light0Pos);

	for(i=0; i<number_of_gears; i++)
	{
		g[i]->id = glGenLists(1);
		glNewList (g[i]->id, GL_COMPILE);
		glColor3dv(g[i]->color);
		mat[0]	=  g[i]->color[0];	// R
		mat[1]	=  g[i]->color[1];	// G
		mat[2]	=  g[i]->color[2];	// B
		mat[3]	= 1;				// A (Alpha, Transparency)
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat);
		gear(i, g[i]->type, g[i]->radius, g[i]->width, g[i]->teeth, g[i]->tooth_depth);
		glEndList();
	}

	for(i=0; i<number_of_axles; i++)
	{
					a[i]->id = glGenLists(1);
		glNewList(	a[i]->id, GL_COMPILE);
		glColor3dv(	a[i]->color);
		mat[0]	=	a[i]->color[0];	
		mat[1]	=	a[i]->color[1];
		mat[2]	=	a[i]->color[2];
		mat[3]	= 1;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat);
		axle(i, a[i]->radius, a[i]->length);
		glEndList();
	}

	for(i=0; i<number_of_belts; i++)
	{
							b[i]->id = glGenLists(1);
		glNewList(			b[i]->id, GL_COMPILE);
		belt(g[gear_find(	b[i]->gear1_name)],
			 g[gear_find(	b[i]->gear2_name)]);
		glEndList();
	}
}



void doRedraw( void )	// Called by drawScene & update
{
	int			i;
	int			index;
	GLdouble	x;
	GLdouble	y;
	GLdouble	z;

	glPushMatrix();

	for(i=0;i<number_of_gears;i++)
	{
		glPushMatrix();
		glTranslatef(g[i]->position[0], g[i]->position[1], g[i]->position[2]);

		{

// This is a squirrely bit stuff. If we comment this section out, everything
// looks fine except the for the one bevel gear that is mounted on the
// vertical Y axis. It shows up as mounted on the horizontal Z axis, just
// like all the other gears. I believe a value of 0 for the axis indicates
// the X axis, a value of 1 indicates the Y axis, and a value of 2 indicates
// the Z axis. So by default we are drawing gears mounted on the Z axis. If
// we are drawing gears mounted on the X axis, we need to rotate them 90
// degrees around the Y axis, which moves them from the Z axis to the X axis.
// Likewise, gears that are to be shown on an axle parallel to the Y axis
// need to be rotated 90 degrees around the X axis.

			x = 0.0;
			y = 0.0;
			z = 0.0;
					if (g[i]->axis==0)	y=1.0;
			else	if (g[i]->axis==1)	x=1.0;
			else						z=1.0;

			if (z!=1.0) glRotatef(90.0, x, y, z);
		}

		g[i]->angle += g[i]->angular_velocity;
		glRotatef( g[i]->angle, 0.0, 0.0, 1.0 );		// rotate gear to its' current position.
		glCallList(g[i]->id);
		glPopMatrix();
	}

	for(i=0;i<number_of_axles;i++)
	{
		glPushMatrix();
		glTranslatef(a[i]->position[0],a[i]->position[1],a[i]->position[2]);

		x=0.0;
		y=0.0;
		z=0.0;
				if(a[i]->axis==0)	y=1.0;
		else	if(a[i]->axis==1)	x=1.0;
		else						z=1.0;

		if(z!=1.0) glRotatef(90.0,x,y,z);

		glCallList(a[i]->id);
		glPopMatrix();
	}

	for(i=0;i<number_of_belts;i++)
	{
		glPushMatrix();
		index=gear_find(b[i]->gear1_name);

		glTranslatef(g[index]->position[0],g[index]->position[1],g[index]->position[2]);

		x=0.0;
		y=0.0;
		z=0.0;
				if(g[index]->axis==0)	y=1.0;
		else	if(g[index]->axis==1)	x=1.0;
		else							z=1.0;

		if(z!=1.0) glRotatef(90.0,x,y,z);

		glCallList(b[i]->id);
		glPopMatrix();
	}

	glPopMatrix();
}



