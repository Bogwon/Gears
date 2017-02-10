/*

Given three points, compute the vector that is normal to the plane defined by those three points.

  */

#include <math.h>
#include "structure.h"

//	#define	dimensionof(p)	((sizeof(p)/sizeof(double))
struct POINT
{
	double	x;
	double	y;
	double	z;
};

typedef	struct POINT	POINT;
TDA		N;		// TDA: Three Doubles in an Array



TDA* normal(TDA p1, TDA p2, TDA p3)
{
	TDA		diff1;
	TDA		diff2;
	double	x;
	double	y;
	double	z;
	double	h3;
	double	h2;
	double	angle1;
	double	angle2;
	double	angle3;
	double	angle4;
	int		ix;

// Difference between two points gives a vector that starts at the origin and
// is parallel to a line between the two points.

	for (ix=0; ix<3; ix++)
		diff1[ix] = p1[ix] - p2[ix];

	for (ix=0; ix<3; ix++)
		diff2[ix] = p3[ix] - p2[ix];

	h3 = _3D_DISTANCE(p1, p2);

// Compute rotation of diff1 to get Y & Z coordinates of 0.
// Rotate diff1 until it is aligned with the X axis.

	x = diff1[0];
	y = diff1[1];
	z = diff1[2];
	h2 = _2D_DISTANCE(x, z);

// sin(angle in x-z plane) = h2/z;
// cos(angle in x-z plane) = h2/x;

	angle1 = asin(z/h2);
	x = diff1[0] = h2;
	z = diff1[2] = 0;

	h2 = _2D_DISTANCE(x, y);

// Rotate vector to x axis

// sin(angle in x-y plane) = h2/y;
// cos(angle in x-y plane) = h2/x;

	angle2 = asin(y/h2);
	x = diff1[0] = h2;
	y = diff1[1] = 0;

// Compute rotation of diff2 to get X & Y coordinates of 0.
// Rotate diff2 around y axis (in x-z plane) until it is aligned with the Z axis.

	x = diff2[0];
	y = diff2[1];
	z = diff2[2];
	h2 = _2D_DISTANCE(x, z);

// sin(angle in x-z plane) = h2/x;
// cos(angle in x-z plane) = h2/z;

	angle3 = asin(x/h2);
	x = diff2[0] = 0;
	z = diff2[2] = h2;

	h2 = _2D_DISTANCE(y, z);

// Rotate vector around x axis (in the y-z plane) until it is on the z axis

// sin(angle in y-z plane) = h2/y;
// cos(angle in y-z plane) = h2/z;

	angle4 = asin(y/h2);
	z = diff2[0] = h2;
	y = diff2[1] = 0;

// So now we just have to apply the four angles of rotation to a vector with
// the coordinates of (0, 1, 0).
// The angles will need to be applied in reverse order, and to the correct
// axii.

	N[0] = 0;	// x
	N[1] = 1;	// y
	N[2] = 0;	// z

	N[2] = N[1] * sin(angle4);
	N[1] = N[1] * cos(angle4);

	N[2] = N[2] * sin(angle3);
	N[0] = N[0] * cos(angle3);

	N[0] = N[0] * sin(angle2);	// rotate ccw in x-y plane
	N[1] = N[1] * cos(angle2);

	N[2] = N[2] * sin(angle1);	// rotate ccw in x-z plane
	N[0] = N[0] * cos(angle1);


	return &N;
}

