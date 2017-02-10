
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

#pragma warning(disable: 4244)	// 238 warnings: conversion from 'double' to 'float', possible loss of data

		FILE*	mainfile;
		char	Buf[4][256];

#define	Buf1	Buf[0]
#define	Buf2	Buf[1]
#define	Buf3	Buf[2]
#define	Buf4	Buf[3]

#define Clear_Buffers()	memset(Buf, 0, sizeof(Buf))

void adjust_speed			(float factor);
void assign_relative_speed	(void);
void position_gears			(void);



// Collect data from configuration file
// In all of these Buf1 should get an equal sign, which is ignored.
void LoadTriplet(TDA A)
{
	Clear_Buffers();
	fscanf(mainfile,"%s %s %s %s",Buf1,Buf2,Buf3,Buf4);
	A[0]=atof(Buf2);
	A[1]=atof(Buf3);
	A[2]=atof(Buf4);
}



void LoadReal(double *a)
{
	Clear_Buffers();
	fscanf(mainfile,"%s %s",Buf1,Buf2);
	*a=atof(Buf2);
}



void LoadInteger(int *a)
{
	Clear_Buffers();
	fscanf(mainfile,"%s %s",Buf1,Buf2);
	*a=atoi(Buf2);
}



void LoadText(char *a)
{
	Clear_Buffers();
	fscanf(mainfile,"%s %s",Buf1,Buf2);
	if (strlen(Buf2) >= NAME_SIZE)	Buf2[NAME_SIZE - 1] = 0;	// Truncate excessively long names
	strcpy(a,Buf2);
}



// Search list of axles/gears for matching name, return index
//
int axle_find(char axle_name[])
{	// Called from numerous places
	int i;

	for(i=0;i<number_of_axles;i++)
	{
		if (!(strcmp(axle_name, a[i]->name)))
			return i;
	}
	return -1;
}



int gear_find(char gear_name[])
{	// Called from numerous places
	int i;

	for(i=0;i<number_of_gears;i++)
	{
		if(!(strcmp(gear_name,g[i]->name)))
			return i;
	}
	return -1;
}



BOOL valid_belt_variable_name(void)
{
	if(!(strcmp(Buf1,"GEAR1NAME")))		return 1;
	if(!(strcmp(Buf1,"GEAR2NAME")))		return 1;
	return 0;
}


/*
#define	add_item(count, TYPE, name)
{
	TYPE**	ptr;
	int		ix;

	ptr = name;
	name = malloc(sizeof(TYPE*) * (count  + 1));
	if (name==NULL) exit(4);

	for (ix=0; ix<count; ix++)
		name[ix] = ptr[ix];

	name[ix] = malloc(sizeof(TYPE));
	if (name[ix] == NULL) exit(4);

	memset(name[ix], 0, sizeof (TYPE));
	free(ptr);
}
*/


void add_belt(int count)
{
	BELT**	ptr;
	int		ix;

	ptr = b;
	b = malloc(sizeof(BELT*) * (count  + 1));
	if (b==NULL) exit(4);

	for (ix=0; ix<count; ix++)
		b[ix] = ptr[ix];

	b[ix] = malloc(sizeof(BELT));
	if (b[ix] == NULL) exit(4);

	memset(b[ix], 0, sizeof (BELT));
	free(ptr);
}



void add_axle(int count)
{
	AXLE**	ptr;
	int		ix;

	ptr = a;
	a = malloc(sizeof(AXLE*) * (count  + 1));
	if (a==NULL) exit(4);

	for (ix=0; ix<count; ix++)
		a[ix] = ptr[ix];

	a[ix] = malloc(sizeof(AXLE));
	if (a[ix] == NULL) exit(4);

	memset(a[ix], 0, sizeof (AXLE));
	free(ptr);
}



void add_gear(int count)
{
	GEAR**	ptr;
	int		ix;

	ptr = g;
	g = malloc(sizeof(GEAR*) * (count  + 1));
	if (g==NULL) exit(4);

	for (ix=0; ix<count; ix++)
		g[ix] = ptr[ix];

	g[ix] = malloc(sizeof(GEAR));
	if (g[ix] == NULL) exit(4);

	memset(g[ix], 0, sizeof (GEAR));
	free(ptr);
}



void	getdata		(const char* filename)
{
	int		gear_count=0;
	int		axle_count=0;
	int		belt_count=0;
	int		i;
	int		error_count;

// Clear axle, belt & gear descriptor arrays

//	memset(a, 0, sizeof(a));
//	memset(b, 0, sizeof(b));
//	memset(g, 0, sizeof(g));

	mainfile=fopen(filename,"r");
	if(mainfile==NULL)
	{
		printf("\nConfiguration file %s not found.\n", filename);
		exit(1);
	}
	if(ferror(mainfile))
	{
		printf("\nError opening file %s!\n", filename);
		exit(1);
	}
	if (VERBOSE) printf("Opened configuration file %s\n", filename);
	while(1)
	{
		Clear_Buffers();
		fscanf(mainfile,"%s",Buf1);
		if(ferror(mainfile))
		{
			printf("\nError reading file %s!\n", filename);
			exit(2);
		}
		if (Buf1[0]==0)
			break;

		if (VERBOSE) printf(Buf1);
		switch(Buf1[0])
		{
		case 'B':	if(!(strcmp(Buf1,"BACKGROUND")))		LoadTriplet	( background);
			else	if(!(strcmp(Buf1,"BELTNAME")))		{	add_belt(belt_count);
															LoadText	( b[belt_count++]->name);
														}
			else	printf("Undefined 'B' variable: %s\n", Buf1);
			break;

		case 'A':	if (!strcmp(Buf1,"ANAME"))			{	add_axle(axle_count);
															LoadText	( a[axle_count++]->name);
														}	
			else	if (axle_count == 0)		printf("'A' variable and no axles defined\n");
			else	if(!(strcmp(Buf1,"ARADIUS")))			LoadReal	(&a[axle_count-1]->radius);
			else	if(!(strcmp(Buf1,"AAXIS")))				LoadInteger	(&a[axle_count-1]->axis);
			else	if(!(strcmp(Buf1,"ACOLOR")))			LoadTriplet	( a[axle_count-1]->color);
			else	if(!(strcmp(Buf1,"APOSITION")))			LoadTriplet	( a[axle_count-1]->position);
			else	if(!(strcmp(Buf1,"ALENGTH")))			LoadReal	(&a[axle_count-1]->length);
			else	if(!(strcmp(Buf1,"AMOTORED")))			LoadInteger	(&a[axle_count-1]->motored);
			else	if(!(strcmp(Buf1,"AANGULARVELOCITY")))	LoadReal	(&a[axle_count-1]->angular_velocity);
			else	if(!(strcmp(Buf1,"ADIRECTION")))		LoadInteger	(&a[axle_count-1]->direction);
			else	printf("Undefined 'A' variable: %s\n", Buf1);
			break;

		case 'G':	if(!(strcmp(Buf1,"GNAME")))			{	add_gear(gear_count);
															LoadText	( g[gear_count++]->name);
														}	
			else	if	((belt_count == 0) && valid_belt_variable_name())
													printf("Belt variable and no belts defined\n");
			else	if	(gear_count == 0)			printf("'G' variable and no gears defined\n");
			else
			{
					if(!(strcmp(Buf1,"GTYPE")))				LoadText	( g[gear_count-1]->type);
			else	if(!(strcmp(Buf1,"GFACE")))				LoadInteger	(&g[gear_count-1]->face);
			else	if(!(strcmp(Buf1,"GRADIUS")))			LoadReal	(&g[gear_count-1]->radius);
			else	if(!(strcmp(Buf1,"GWIDTH")))			LoadReal	(&g[gear_count-1]->width);
	//		else	if(!(strcmp(Buf1,"GTEETH")))			LoadInteger	(&g[gear_count-1]->teeth);
			else	if(!(strcmp(Buf1,"GTOOTHDEPTH")))		LoadReal	(&g[gear_count-1]->tooth_depth);
			else	if(!(strcmp(Buf1,"GCOLOR")))			LoadTriplet	( g[gear_count-1]->color);
			else	if(!(strcmp(Buf1,"GAXLE")))				LoadText	( g[gear_count-1]->axle_name);
			else	if(!(strcmp(Buf1,"GPOSITION")))			LoadInteger	(&g[gear_count-1]->relative_position);

// Pulleys For Belts
			else	if(!(strcmp(Buf1,"GEAR1NAME")))			LoadText	( b[belt_count-1]->gear1_name);
			else	if(!(strcmp(Buf1,"GEAR2NAME")))			LoadText	( b[belt_count-1]->gear2_name);
			else	printf("Undefined 'G' variable: %s\n", Buf1);
			}
			break;

		default:	printf("Invalid configuration variable name: %s\n", Buf1);
			break;		
		}
		if (VERBOSE) printf(" %s %s %s %s\n", Buf1, Buf2, Buf3, Buf4);
	}
	fclose(mainfile);

// Copy local values to global variables. axle_find & gear_find use the global variables.

	number_of_gears = gear_count;
	number_of_axles = axle_count;
	number_of_belts = belt_count;

// Validate parameter connections

	error_count = 0;
	for(i=0;i<gear_count;i++)
	{
		if(axle_find(g[i]->axle_name)==-1)
		{
			printf("\nAxle %s defined in gear %s does not exist.",
								g[i]->axle_name,		g[i]->name);

			error_count++;
		}
	}
	
	for(i=0;i<belt_count;i++)
	{
		if(gear_find(b[i]->gear1_name)==-1)
		{
			printf("\nGear %s defined in belt %s does not exist.",
								b[i]->gear1_name,	b[i]->name);

			error_count++;
		}
		if(gear_find(b[i]->gear2_name)==-1)
		{
			printf("\nGear %s defined in belt %s does not exist.",
									b[i]->gear2_name,	b[i]->name);

			error_count++;
		}
	}

// Check gear type

	for(i=0;i<gear_count;i++)
	{
		if (!strcmp(g[i]->type, "NORMAL"))	continue;
		if (!strcmp(g[i]->type, "BEVEL" ))	continue;
		printf("\nBad Gear (%s) Type: %s.", g[i]->name, g[i]->type);
		error_count++;
	}
	if (error_count) exit(error_count);

// Initialize all gears that have been defined.

	for(i=0;i<gear_count;i++)
	{
		g[i]->teeth	= (int)(15 * g[i]->radius);
		g[i]->radius = (float)g[i]->teeth / 15.0;
	}
}

