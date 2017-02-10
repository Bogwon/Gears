
#include <math.h>
#include <string.h>

// We need to include glut.h  only to compile under Windows (using Visual C)
// The windows version of the gl.h file uses some weird define that is
// defined in glut.h. Otherwise we would have to include windows.h, and that
// might cause problems under Linux. However, we know glut.h will be there
// because main.c uses it.

#include <GL/glut.h>
#include <GL/gl.h>

#ifndef min
	#define	min(a, b)	(a < b) ? a : b
#endif

#include "structure.h"		// global data
#include "normal.h"

#pragma warning(disable: 4244)	// 238 warnings: conversion from 'double' to
								// 'float', possible loss of data

#define	CONVERT_DEGREES_TO_RADIANS(p)	(p * PI/180.0)
#define	INCREMENT						5.0
#define	INCR							CONVERT_DEGREES_TO_RADIANS(INCREMENT)



void draw_circle(float face, double radius, double offset)
{
	GLdouble	angle;
	GLdouble	rad;

	glNormal3f(0.0, 0.0, face);
	glBegin(GL_TRIANGLES);
	for(angle=0.0; angle<360.0; angle+=INCREMENT)
	{
		rad = CONVERT_DEGREES_TO_RADIANS(angle);
		glVertex3f(0.0,						0.0,					 offset);
		glVertex3f(radius*cos(rad		),	radius*sin(rad		 ),	 offset);
		glVertex3f(radius*cos(rad + INCR),	radius*sin(rad + INCR),	 offset);
	}
	glEnd();
}



void axle(GLint j, GLdouble radius, GLdouble length)
{	// called only by Generate_Display_Lists
	GLdouble	angle;
	GLdouble	rad;

// draw main cylinder 

	glBegin(GL_QUAD_STRIP);
	for(angle=0.0; angle<=360.0; angle+=INCREMENT)
	{
		rad = CONVERT_DEGREES_TO_RADIANS(angle);
		glNormal3f(		  cos(rad		),		   sin(rad		 ),	0.0F);			
		glVertex3f(radius*cos(rad		),	radius*sin(rad		 ),	 length/2);
		glVertex3f(radius*cos(rad		),	radius*sin(rad		 ),	-length/2);
	}
	glEnd();

	draw_circle( 1.0, radius,  length/2);		// draw back face 
	draw_circle(-1.0, radius, -length/2);		// draw front face 
}



void spur_gear_teeth_sides(float face, GLint teeth, GLdouble r1, GLdouble r2,
													double width, double angle)
{
	GLint		i;
	GLdouble	da;

	glNormal3f(0.0,0.0, face);
	glBegin( GL_QUADS );
	da		= 0.5 * PI / teeth;
	for (i=0; i<teeth; i++)
	{
  		glVertex3f( r1*cos(angle), r1*sin(angle), width);	angle += da;
  		glVertex3f( r2*cos(angle), r2*sin(angle), width);	angle += da;
  		glVertex3f( r2*cos(angle), r2*sin(angle), width);	angle += da;
  		glVertex3f( r1*cos(angle), r1*sin(angle), width);	angle += da;
	}
	glEnd();
}



void spur_gear(	GLint j,		GLdouble radius,
								GLdouble width,
				GLint teeth,	GLdouble tooth_depth)
{	// called only by gear
	GLint		i;
	GLdouble	angle;
	GLdouble	r1;		// inner radius
	GLdouble	r2;		// outer radius

// for computing normal vectors
/*	GLdouble	len;
	GLdouble	u;
	GLdouble	v;
*/
	float		a0;		// Having four separate variables for angle
	float		a1;		// keeps things clear
	float		a2;		// and saves a bunch of multiplying
	float		a3;
	float		da;		// 1/4 of tooth angle
	float		ta;		// tooth angle
	float		pa;		// phase angle: starting position

//	teeth /= 2;			// teeth don't mesh
//	tooth_depth /= 2;	Cannot distinguish teeth when they mesh.

	r1	= radius-tooth_depth;
	r2	= radius;
	ta = TWO_PI / teeth;
	pa = ta * g[j]->phase;
	da = ta / 4;					// one fourth (1/4) of space of tooth
									// this is for:
	// 1 space between teeth
	// 2 front surface
	// 3 top of tooth
	// 4 back surface

	width	/= 2;	// we draw from center
	draw_circle(		   1.0,			r1,		 width);	// draw front face 
	draw_circle	(		  -1.0,			r1,		-width);	// draw back face 

// draw front sides of teeth 

	spur_gear_teeth_sides( 1.0, teeth,	r1, r2,  width, pa);

// draw back sides of teeth 

	spur_gear_teeth_sides(-1.0, teeth,	r1, r2, -width, pa);

// draw outward faces of teeth 

// We don't need to set Normal to begin with because we start with a line.
// We do need to set Normal at the end of the loop because at the start of
// each additional loop the first two calls to glVertex3f complete a face
// AND after we exit the loop we finish the last strip, which is the bottom
// of space between teeth.

		glBegin( GL_QUAD_STRIP );
		angle = pa;
		for (i=0;i<teeth;i++)
		{
			a0 = angle;
			a1 = a0 + da;
			a2 = a1 + da;
			a3 = a2 + da;

// start at front

			glVertex3f( r1 * cos(a0),	r1 * sin(a0),  width);
	  		glVertex3f( r1 * cos(a0),	r1 * sin(a0), -width);	// go to back

// u: difference in x coordinates of two points
// v: difference in y coordinates of two points
//			u =			r2 * cos(a1)  - r1 * cos(a0);
//	  		v =			r2 * sin(a1)  - r1 * sin(a0);

// I can't tell the difference between having these three lines included or not			
// There is no difference, we just get a scaled vector, and since we only need the 
// direction, we don't need the length.
/*
			len = sqrt( u*u + v*v );		// distance between two points
	  		u /= len;						// make a ratio
	  		v /= len;
*/
// vector perpendicular to face for lighting

//			glNormal3f( v, -u, 0.0 );
			glNormal3f( sin(a0), cos(a0), 0.0 );

// finish backward facing surface

			glVertex3f( r2 * cos(a1),   r2 * sin(a1),  width);
			glVertex3f( r2 * cos(a1),   r2 * sin(a1), -width);

// perpendicular to axis

//			glNormal3f(		 cos(a0),		 sin(a0),  0.0);
			glNormal3f(		 cos(a1),		 sin(a1),  0.0);

// finish outer surface

			glVertex3f( r2 * cos(a2),	r2 * sin(a2),  width);
	  		glVertex3f( r2 * cos(a2),	r2 * sin(a2), -width);
//			u =			r1 * cos(a3) -	r2 * cos(a2);
//	  		v =			r1 * sin(a3) -	r2 * sin(a2);

// perpendicular to face

//			glNormal3f( v, -u, 0.0 );
			glNormal3f( sin(a2), cos(a2), 0.0 );

// finish forward surface

	  		glVertex3f(	r1 * cos(a3),	r1 * sin(a3),  width);
	  		glVertex3f(	r1 * cos(a3),	r1 * sin(a3), -width);

// perpendicular to axis

			glNormal3f(		 cos(a3),		 sin(a3),  0.0);
			angle += ta;
		}

// finish last strip

	glVertex3f(			r1 * cos(pa),	r1 * sin(pa),  width );
	glVertex3f(			r1 * cos(pa),	r1 * sin(pa), -width );
	glEnd();
}



void bevel_gear(GLint j,	GLdouble radius,
							GLdouble width,
		  GLint teeth,		GLdouble tooth_depth)
{
// The inner faces of the two bevel gears
// face each other across a 90 degree angle
// The outer face faces away.
// We need four radii to draw bevel gear teeth.

	float		irb;	// inner radius back		
	float		irf;	// inner radius front
	float		orb;	// outer radius back
	float		orf;	// outer radius front

	GLint		i;
	GLdouble	angle;
	GLdouble	da;		// degrees for each face of the the tooth.
	float		face;
	float		pa;		// phase angle
	float		ta;		// tooth angle

	GLdouble	u;
	GLdouble	v;
	GLdouble	len;

	float		a0;
	float		a1;
	float		a2;
	float		a3;

	double		save;
	TDA			P1;
	TDA			P2;
	TDA			P3;
	TDA*		N;

	width /= 2;
	teeth /= 2;					// The better to see you with, my dear
	orb = radius + width;
	orf = radius - width;
	irb = orb - tooth_depth;
	irf = orf - tooth_depth;

	// There are four outward (radial) facing faces to each tooth.
	// For convenience we make them all the same width.

	ta = TWO_PI / teeth;		// One circle (2 PI)
									// divided by number of teeth
	pa = ta * g[j]->phase;
	pa *= 0.76f;						// Fudge factor.
				// Phase calculation for bevel gears is obviously not working.
	da = ta / 4;					// divided by four faces: _/-\
									// bottom, forward face, top, back face
	// 1 space between teeth
	// 2 front surface
	// 3 top of tooth
	// 4 back surface

	face = 1.0;
	if (g[j]->face)
	{
		width	= -width;
		face	= -face;
	}

// draw faces of gear wheel

	draw_circle( face, irb,  width);		// draw front face 
	draw_circle(-face, irf, -width);		// draw back face 

// draw back sides of teeth
// Outer radius is radius, inner radius is radius - tooth depth

	glNormal3f(0.0,0.0, 1.0);
	glBegin( GL_QUADS );
	angle = 0;
	angle = pa;
	for (i=0;i<teeth;i++)
	{
		save = angle;
		P1[0] = irb*cos(angle);
		P1[1] = irb*sin(angle);
		P1[2] = width;				angle += da;
		P2[0] = orb*cos(angle);
		P2[1] = orb*sin(angle);
		P2[2] = -width;				angle += da;
		P3[0] = orb*cos(angle);
		P3[1] = orb*sin(angle);
		P3[2] = -width;				angle += da;
		N = normal(P1, P2, P3);
		glNormal3f( *N[0], *N[1], *N[2]);
		angle = save;

		glVertex3f( irb*cos(angle), irb*sin(angle),  width );	angle += da;
		glVertex3f( orb*cos(angle), orb*sin(angle), -width );	angle += da;
		glVertex3f( orb*cos(angle), orb*sin(angle), -width );	angle += da;
		glVertex3f( irb*cos(angle), irb*sin(angle),  width );	angle += da;

/*		P1[0] = irb*cos(angle			);		P1[1] = irb*sin(angle			);
		P2[0] = orb*cos(angle +		 da	);		P2[1] = orb*sin(angle +		 da	);
		P3[0] = orb*cos(angle + (2 * da));		P3[1] = orb*sin(angle + (2 * da));
		P4[0] = irb*cos(angle + (3 * da));		P4[1] = irb*sin(angle + (3 * da));

		P1[2] = width;
		P2[2] = -width;
		P3[2] = -width;
		P4[2] = width;

		angle += 4 * da;
		N = normal(P1, P2, P3);
		glNormal3f( *N[0], *N[1], *N[2]);
*/
	}
	glEnd();

// draw front sides of teeth

	glNormal3f(0.0,0.0, -1.0);
	glBegin( GL_QUADS );
	angle = 0;
	angle = pa;
	for (i=0;i<teeth;i++)
	{
		glVertex3f( irf*cos(angle), irf*sin(angle),		-width );	angle += da;
		glVertex3f( orf*cos(angle), orf*sin(angle), -3 * width );	angle += da;
		glVertex3f( orf*cos(angle), orf*sin(angle), -3 * width );	angle += da;
		glVertex3f( irf*cos(angle), irf*sin(angle),		-width );	angle += da;
	}
	glEnd();

// draw outward faces of teeth

	glBegin( GL_QUAD_STRIP );
	angle = pa;
	for (i=0;i<teeth;i++)
	{
		a0 = angle;
		a1 = angle + da;
		a2 = angle + 2*da;
		a3 = angle + 3*da;

		glVertex3f(	irb * cos(a0),	irb * sin(a0),		 width	);
		glVertex3f(	irf * cos(a0),	irf * sin(a0),		-width	);

		u =			orf * cos(a1) -	irb * cos(a0);
		v =			orb * sin(a1) -	irb * sin(a0);
		len = sqrt( u*u + v*v );
		u /= len;
		v /= len;
		glNormal3f( v, -u, 0.0 );

		glVertex3f( orb * cos(a1),	orb * sin(a1),		-width	);
		glVertex3f( orf * cos(a1),	orf * sin(a1),	-3 * width	);
		glNormal3f(		  cos(a0),		  sin(a0),		face	);
		glVertex3f( orb * cos(a2),	orb * sin(a2),		-width	);
		glVertex3f( orf * cos(a2),	orf * sin(a2),	-3 * width	);

		u =			irb * cos(a3) - orb * cos(a2);
		v =			irb * sin(a3) - orb * sin(a2);
		glNormal3f( v, -u, 0.0 );

		glVertex3f(	irb * cos(a3),	irb * sin(a3),		 width	);
		glVertex3f(	irf * cos(a3),	irf * sin(a3),		-width	);
		glNormal3f(		  cos(a0),		  sin(a0),		face	);
		angle += ta;
	}
	glVertex3f(		irb * cos(pa),	irb * sin(pa),		 width	);
	glVertex3f(		irf * cos(pa),	irf * sin(pa),		-width	);
	glEnd();
}



void gear(GLint j, char type[],	GLdouble radius,	// called only by Generate_Display_Lists
								GLdouble width,
		  GLint teeth,			GLdouble tooth_depth)
{
	if(!(strcmp(type,"BEVEL")))
		bevel_gear(j, radius, width, teeth, tooth_depth);
	else	// NORMAL
		spur_gear(j, radius, width, teeth, tooth_depth);
}



// Right now we only deal with belts that are centered on the centerline of the cog wheel.
// It could be possible to have belts that are not,
// but we would have to compute a new centerline for the belt. Maybe later.

void belt(GEAR* g1, GEAR* g2)
{						// called only by Generate_Display_Lists
// # if Toothed belt
//	#define	BELT_THICKNESS	0.1
#define	BELT_THICKNESS	0.05	// Gear teeth stick through belt of zero thickness.
	
	float		diff0;
	float		diff1;
	float		r1;
	float		r2;
	GLfloat		dt;
	GLdouble	D;
	GLdouble	alpha;
	GLdouble	phi;
	GLdouble	angle;
	GLdouble	width;

	GLfloat		Black[4] = {0.1F, 0.1F, 0.1F, 1.0F};	// F suffix avoids warning C4305
	GLfloat		Grey [4] = {0.5F, 0.5F, 0.5F, 1.0F};
	GLfloat		Red  [4] = {1.0F, 0.0F, 0.0F, 1.0F};
	GLfloat		Blue [4] = {0.0F, 0.0F, 1.0F, 1.0F};

	width	= min(g1->width, g2->width);	// Don't want a belt that overlaps the edges of the cogs
	width  *= 0.5;						// Edges are equidistant from centerline

// D is three dimensional distance from from the two gear positions
// We assume the gear is lying in the X-Y plane.
// When both axii are on the X-axis, alpha comes out to be PI. arccos of -1 is pi.
// From our sample:
//		phi comes out to be 1.449..., which is something less than 90 degrees.
//		D=11.
//		Difference in position is -11.
//		Difference in radius is 4/3
//		slope is -4/33, or roughly -1/8th
// so phi is the acos of the slope of the flat portion of the belt.

	D		= _3D_DISTANCE(g1->position, g2->position);
	diff0	= g2->position[0]-g1->position[0];
	diff1	= g2->position[1]-g1->position[1];
	alpha	= acos(			diff0			/ D);
	phi		= acos((g1->radius - g2->radius)	/ D);

	glColor3fv(Black);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Black);

// Draw outside surface of belt

#define	DRAW_LINE(R, X, Y)	{								\
		glNormal3f(  cos(angle),	 sin(angle),	0.0);	\
		glVertex3f(R*cos(angle)+X, R*sin(angle)+Y,  width);	\
		glVertex3f(R*cos(angle)+X, R*sin(angle)+Y, -width);	\
	}
    
	glBegin(GL_QUAD_STRIP);
	r1		= g1->radius + BELT_THICKNESS;	// radius of outer surface of belt around gear #1.
	r2		= g2->radius + BELT_THICKNESS;	// radius of outer surface of belt around gear #2.
	dt		= TWO_PI / g1->teeth;	// Draw belt around first gear
    for(angle =		  +phi+alpha;	
		angle < TWO_PI-phi+alpha;
		angle += dt)
	{
		DRAW_LINE(r1, 0, 0);
	}
	angle = TWO_PI-phi+alpha;
	DRAW_LINE(r1, 0, 0);			// Finish arc
	DRAW_LINE(r2, diff0, diff1);	// Draw flat to second gear
    
	dt		= TWO_PI / g2->teeth;	// Draw belt around second gear
    for(angle = -phi+alpha;
		angle <  phi+alpha;
		angle +=			dt)
	{
		DRAW_LINE(r2, diff0, diff1);
	}
	angle =  phi+alpha;
	DRAW_LINE(r2, diff0, diff1);	// Finish arc
	DRAW_LINE(r1, 0, 0);			// Draw flat back to first gear
	glEnd();

/* #if Toothed belt

// Flat belt
// Draw inside surface of belt.
// Only difference between this and previous section is radii and Normal angle.

#define	DRAW_IN_LINE(R, X, Y)	{								\
		glNormal3f(  cos(angle-PI),		sin(angle-PI),	0.0);	\
		glVertex3f(R*cos(angle)+X,		R * sin(angle)+Y,	  width);	\
		glVertex3f(R*cos(angle)+X,		R * sin(angle)+Y,	 -width);	\
	}
    
	glColor3fv(Blue);
	glBegin(GL_QUAD_STRIP);
	r1		= g1.radius;	// radius of inner surface of belt around gear #1.
	r2		= g2.radius;	// radius of inner surface of belt around gear #2.
	dt		= TWO_PI / g1.teeth;	// Draw belt around first gear
    for(angle =		  +phi+alpha;	
		angle < TWO_PI-phi+alpha;
		angle += dt)
	{
		DRAW_IN_LINE(r1, 0, 0);
	}
	angle = TWO_PI-phi+alpha;
	DRAW_IN_LINE(r1, 0, 0);			// Finish arc
	DRAW_IN_LINE(r2, diff0, diff1);	// Draw flat to second gear
    
	dt		= TWO_PI / g2.teeth;	// Draw belt around second gear
    for(angle = -phi+alpha;
		angle <  phi+alpha;
		angle +=			dt)
	{
		DRAW_IN_LINE(r2, diff0, diff1);
	}
	angle =  phi+alpha;
	DRAW_IN_LINE(r2, diff0, diff1);	// Finish arc
	DRAW_IN_LINE(r1, 0, 0);			// Draw flat back to first gear
	glEnd();

#define	DRAW_BELT_EDGE(Angle, Z)	{													\
		glVertex3f( g1.radius	*cos(Angle),		g1.radius	* sin(Angle),		Z);	\
		glVertex3f( r1			*cos(Angle),		r1			* sin(Angle),		Z);	\
		glVertex3f( r2			*cos(Angle)+diff0,	r2			* sin(Angle)+diff1,	Z);	\
		glVertex3f( g2.radius	*cos(Angle)+diff0,	g2.radius	* sin(Angle)+diff1,	Z);	\
	}

// Draw edges on one side

	r1		= g1.radius + BELT_THICKNESS;
	r2		= g2.radius + BELT_THICKNESS;

	glColor3fv(Red);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Red);
	glBegin(GL_QUADS);
	glNormal3f(0.0, 0.0, -1);
	DRAW_BELT_EDGE(alpha+phi, -width);
	DRAW_BELT_EDGE(alpha-phi, -width);

// Draw edges on other side

	glNormal3f(0.0, 0.0,  1);
	DRAW_BELT_EDGE(alpha+phi, width);
	DRAW_BELT_EDGE(alpha-phi, width);
	glEnd();
*/
}



