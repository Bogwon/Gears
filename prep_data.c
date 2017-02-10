
// No calls to any gl functions

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	VERBOSE	1
#ifndef	BOOL
	typedef	unsigned char	BOOL;
#endif

#undef EXTERN				// Paranoia.
#include "structure.h"		// global data

#include "load_data.h"

#pragma warning(disable: 4244)	// 238 warnings: conversion from 'double' to 'float', possible loss of data



float compute_percentage_overlap(GEAR* g1, GEAR* g2, int axis)	// Called only by spur_gears_align
{
	float overlap;

	assert(g1->position[axis] < g2->position[axis]);

	if	(	((g1->position[axis] + g1->width) >  g2->position[axis]			)
		&&	((g1->position[axis] + g1->width) < (g2->position[axis] + g2->width))
		)
	{
		overlap = g1->position[axis] + g1->width - g2->position[axis];			// partial overlap
		return overlap / (g1->width + g2->width - overlap);
	}

	if	((g1->position[axis] + g1->width) > (g2->position[axis] + g2->width))
		return 1.0;						// g1 is wider and g2 is completely within g1
//		return g2.width / g1.width;

	return 0;
}



float spur_gears_align(GEAR* g1, GEAR* g2)	// Called only by compute_engagement
{											//            and compute_pulley_alignment
	// returns  0 if there is no overlap
	//			1 if they are they have same width and align perfectly,
	//			  or if the narrower gear is completely covered by the wider one
	//			a fraction that reflects the ratio of the parts that overlap and their sizes

	int axis;

	assert(strcmp(g1->name, g2->name));	// Make sure g1 and g2 are different gears.
	if (g1->axis != g2->axis) return 0;	// Make sure their axii are parallel
										// i.e they have the same orientation.

	axis = g1->axis;

// Deal with simple cases first

	if ((g1->position[axis]==g2->position[axis]) && (g1->width==g2->width)) return 1.0;
	if ((g1->position[axis]+g1->width) < g2->position[axis])	return 0.0;
	if ((g2->position[axis]+g2->width) < g1->position[axis])	return 0.0;

// Now we know there is some, but not complete overlap

	if (g1->position[axis] < g2->position[axis])
		return compute_percentage_overlap(g1, g2, axis);

	return compute_percentage_overlap(g2, g1, axis);
}



float spur_gears_mesh(GEAR* g1, GEAR* g2)	// Called only by compute_engagement
{										//            and compute_pulley_alignment
	// This procedure does not check alignment. 
	// It only works with gear sizes and distance between axii.
	// radius is the distance from the center to the outside edge of the teeth
	// tooth depth measures the distance in from the outside edge to the bottom of the tooth
	// We assume the axis of the gear is parallel to one of the major axii (x, y, z)

	// returns  0 if they do not engage at all
	//			1 if they mesh perfectly
	//			a fraction between 0 and 1 if they do not engage completely
	//			a value greater than one if they interfere with each other

	float	dist;
	float	engage;

	assert(strcmp(g1->name, g2->name));	// Make sure g1 and g2 are different gears.
	if (g1->axis != g2->axis) return 0;	// Make sure their axii are parallel
										// i.e they have the same orientation.
	assert(g1->tooth_depth > 0);
	assert(g2->tooth_depth > 0);
	
// Compute distance between the two axles.

	dist = _3D_DISTANCE(g1->position, g2->position);

	engage = g1->radius + g2->radius - dist;

	if (engage < 0)			// Deal with the simple case first
		return 0;

	return engage / g1->tooth_depth;
}



int find_driving_axle(void)		// Called only by assign_relative_speed
{
	int		k;
	int		driver = 0;

	for(k=0; k<number_of_axles; k++)
		if (a[k]->motored)
		{
			driver = k++;
			break;
		}

	for(   ; k<number_of_axles; k++)
		assert(!a[k]->motored);

	return driver;
}



float compute_engagement(GEAR* g1, GEAR* g2)		// Called only by assign_relative_speed
{
	// Should return a value between zero and one.
	// Returns a zero if gears do not contact each other OR gears are different types.
	// Returns a one if they align and mesh correctly.
	// Fails if they interfere with each other.

	float	result1;
	float	result2;
	int		axis;
	int		ax;
	int		bx;
	float	diff1;
	float	diff2;

	if	(strcmp(g1->type, g2->type)) return 0;	// Gears of different types do not mesh.

	if	(!strcmp(g1->type, "NORMAL") && !strcmp(g2->type, "NORMAL"))
	{
		result1 = spur_gears_align(g1, g2);
		if (result1 == 0) return 0;

		result2 = spur_gears_mesh(g1, g2);
		assert(result2 <= 1.0);
		return result1 * result2;
	}

	assert(!strcmp(g1->type, "BEVEL") && !strcmp(g2->type, "BEVEL"));

// Do axii intersect?
// All axles are aligned with one of three main axii (x, y or z).
// The axii of spur gears that mesh are parallel.
// The axii of (bevel gears that mesh) intersect and form a right angle.
// We use the process of elimination to determine the axis that would be
// perpendicular to both at their intersection.
// Then we find their position relative to that axis.
// If the offset is the same, then they intersect.

	if (g1->axis == g2->axis) return 0;		// Same axis
	axis = 3 - (g1->axis + g2->axis);			// Determine 3rd axis
	assert((axis >=0) || (axis <=2));

	if (g1->position[axis]!=g2->position[axis]) return 0;

// Axii intersect. Now to determine if gears mesh.

// This is a little tricky. We have to determine whether we are engaging the
// near edge or the far edge of the other gear. I guess the trick is to see
// whether we are are within engagement distance of either edge. If we are,
// then we are good. Otherwise no.
	
	diff1 = fabs(g1->position[g1->axis] - g2->radius - g2->position[g1->axis]);
	diff2 = fabs(g1->position[g1->axis] + g2->radius - g2->position[g1->axis]);
	if ((diff1 > 1) && (diff2 > 1)) return 0;
	diff1 = fabs(g2->position[g2->axis] - g1->radius - g1->position[g2->axis]);
	diff2 = fabs(g2->position[g2->axis] + g1->radius - g1->position[g2->axis]);
	if ((diff1 > 1) && (diff2 > 1)) return 0;

	ax = axle_find(g1->axle_name);
	bx = axle_find(g2->axle_name);

	return 1;
}



float compute_pulley_alignment(GEAR* g1, GEAR* g2)	// Called only by assign_relative_speed
{
	// Should return a value between zero and one.
	// Returns a zero if gears do not align
	// Returns a one if they align and do not contact each other.
	// Fails if they interfere with each other.

	float	result1;
	float	result2;

	result1 = spur_gears_align(g1, g2);
	if (result1 == 0) return 0;

	result2 = spur_gears_mesh(g1, g2);
	assert(result2 == 0.0);

	return result1;
}



int assign_speed(int gx, int hx, int fwd_or_rev)	// Called only by assign_relative_speed
{
	// fwd_or_rev is either 1 or -1.

	// Returns 1 if it just sets the speed of gear 2 (hx)
	// Returns 2 if it also sets the speed of the axle gear 2 is mounted on.
	int		marker;
	int		bx;

	marker				 = 1;
	g[hx]->marked		 = 1;
	if (g[gx]->face != g[hx]->face) fwd_or_rev = -fwd_or_rev;		// Special for bevels
	g[hx]->relative_speed = fwd_or_rev * g[gx]->relative_speed * g[gx]->teeth / g[hx]->teeth;
	bx = axle_find(g[hx]->axle_name);
	if (a[bx]->marked)
		assert(a[bx]->relative_speed ==  g[hx]->relative_speed);
	else
	{
		a[bx]->relative_speed =  g[hx]->relative_speed;
		a[bx]->marked		 = 1;
		marker++;
	}
	return marker;
}



void bevel_gear_phase(int gx, int hx)
{
	if (g[gx]->axis==g[hx]->axis) return; // bevel gears only

	g[hx]->phase = 1 - g[gx]->phase;
}



void set_phase(int gx, int hx)
{
	#define mod						%
	
// We know these two gears are engaged.
// We just need to determine the position and then set the phase of these two.
// We use a bunch of local variables, but it helps keep things clear.
// I expect the compiler will optimize most of them out.
// I took a guess as to how this might work.
// I selected the bx axis as the phase relative axis,
// but then I don't really know which axis was used last time.
// I also used asin to get the angle. Would acos be better? I have no idea.

	int		ax;		// gear axis
	int		bx;		// phase relative axis
	int		cx;		// used to compute distance between axii
	float	d1;		// distance between axii along bx axis
	float	d2;		// distance between axii along cx axis
	float	h;		// hypoteneuse

	float	fraction_of_circle;
	float	fraction_of_tooth;
	float	radians;
	float	teeth;

	if (g[gx]->axis!=g[hx]->axis)
	{
		g[hx]->phase = 1 - g[gx]->phase;		// bevel gears are simple?
		return;
	}

	ax	= g[gx]->axis;
	bx	= (ax + 1) mod 3;
	cx	= (bx + 1) mod 3;

	d1	= g[hx]->position[bx] - g[gx]->position[bx];
	d2	= g[hx]->position[cx] - g[gx]->position[cx];
	h	= _2D_DISTANCE(d1, d2);

	radians				= asin(d1 / h);
	fraction_of_circle	= radians / TWO_PI;
	teeth				= fraction_of_circle * g[hx]->teeth;
	fraction_of_tooth	= teeth - (int)teeth;
	if (g[hx]->relative_speed > 0)
		g[hx]->phase			= g[gx]->phase - fraction_of_tooth;
	else
		g[hx]->phase			= g[gx]->phase + fraction_of_tooth;
}


void assign_relative_speed(void)		// Called only by main
{										// Called once during initialization.
	int		ax;		// primary axle index
	int		bx;		// secondary axle index
	int		dx;		// driven axle index
	int		gx;		// primary gear index
	int		hx;		// secondary gear index
	int		marker;
	int		last_marker;
	
	dx = find_driving_axle();

/*	for all gears
		if gear is on driving axle
			assign relative speed & direction
			mark gear;

	while (1)
		for all gears
			if not marked
			and engaged
				assign relative speed & direction
				mark
				do the same for the axle
				if axle is already marked, verify speed matches.			

		for all belts
			verify alignment and clearance of gears
			assign relative speed & direction
			mark

		if marks = number of axles plus number of gears
			we are done

		if marks is unchanged since last time
			we are done

Notes: 
- when assigning speed, ensure we do not have a conflict
- gears that are used with belts may not be engaged with anything else
- gears not used with belts may be engaged with multiple gears
*/

// Find all gears mounted on drive axle, mark speed.

	marker = 0;
	for (gx=0; gx<number_of_gears; gx++)
	{
		if (dx == axle_find(g[gx]->axle_name))
		{
			marker++;
			g[gx]->marked		 = 1;
			g[gx]->relative_speed = a[dx]->direction;		// 1 or -1
			a[dx]->relative_speed = a[dx]->direction;		// 1 or -1
		}				
	}	
	assert(marker);		// We need at least one gear on drive axle. There may be more than one.

// Track down which gears turn which, calculate relative speeds.

	last_marker = marker;
	while (1)
	{
		for (gx=0; gx<number_of_gears; gx++)		// Test all combinations of gears for engagement
		{
			if (g[gx]->marked)
			{
				for (hx=0; hx<number_of_gears; hx++)
				{
					if (gx==hx)			continue;
					if (g[hx]->marked)	continue;

					if (compute_engagement(g[gx], g[hx]) > 0)
					{
						marker += assign_speed(gx, hx, -1);
						set_phase(gx, hx);
					}
				}
			}
		}

		for (bx=0; bx<number_of_belts; bx++)
		{
			gx = gear_find(b[bx]->gear1_name);
			hx = gear_find(b[bx]->gear2_name);
			assert(gx!=hx);
			if ( g[gx]->marked &&  g[hx]->marked)	continue;
			if (!g[gx]->marked && !g[hx]->marked)	continue;	// at least one, but only one must be marked
			assert(compute_pulley_alignment(g[gx], g[hx]) > 0);
			if (g[gx]->marked)
				marker += assign_speed(gx, hx, 1);
			else
				marker += assign_speed(hx, gx, 1);
		}
	
		for (ax=0; ax<number_of_axles; ax++)			// For each axle
		{
			if (a[ax]->marked)							// that has been assigned speed
			{
				for (gx=0; gx<number_of_gears; gx++)		// Check all gears
				{
					if (ax == axle_find(g[gx]->axle_name))	// if gear is on this axle
					{
						if (g[gx]->marked)
							assert(g[gx]->relative_speed == a[ax]->relative_speed);	// verify speeds match
						else
						{													// & gear has not been marked yet
							g[gx]->marked		 = 1;						// then set speed
							g[gx]->relative_speed = a[ax]->relative_speed;
						}
					}
				}
			}
		}

		if (marker == last_marker						) break;	// We didn't find any more
		if (marker == number_of_gears + number_of_axles	) break;	// We tagged them all
		last_marker = marker;
	}
}



void adjust_speed(float factor)		// Called by handleKeypress in main.c
{
	int		ax;
	int		gx;
	
	for (gx=0; gx<number_of_gears; gx++)
	{
		if (g[gx]->marked)
			g[gx]->angular_velocity = g[gx]->relative_speed * factor;
	}

	for (ax=0; ax<number_of_axles; ax++)
	{
		if (a[ax]->marked)
			a[ax]->angular_velocity = a[ax]->relative_speed * factor;
	}
}



void position_gears(void)		// Called only by main
{
// Called once at initialization to locate gears on axles.
// Copy axle axis & xyz position to gear.
// Adjust gear xyz position by relative position according to axle axis.

	int		axle_index;
	int		i;
	int		j;

	for(i=0; i<number_of_gears; i++)
	{
		axle_index	= axle_find(g[i]->axle_name);
		g[i]->axis	= a[axle_index]->axis;

		assert((g[i]->axis>=0) && (g[i]->axis<=2));

		for(j=0; j<3; j++)
			g[i]->position[j]	= a[axle_index]->position[j];

		g[i]->position[g[i]->axis]	+= g[i]->relative_position;
	}
}



