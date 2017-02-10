
#define PI					3.14159265
#define	TWO_PI				(2.0 * PI)
#define	NAME_SIZE			20

typedef double TDA[3];

struct	BELT
{
	char		name[NAME_SIZE];		// Same for all three
	int		id;

	char		gear1_name[NAME_SIZE];	// Link to other elements
	char		gear2_name[NAME_SIZE];
};

struct	AXLE
{
	char		name[NAME_SIZE];		// Same for all three
	int		id;

	int		axis;					// Same for axle and gear
	int		motored;
	int		direction;
	double	angular_velocity;
	double	radius;
	TDA			color;
	TDA			position;
	float		relative_speed;			// New ccp
	int			marked;					// New ccp

	double	length;					// Similar for axle and gear
};

struct	GEAR
{
	char		name[NAME_SIZE];		// Same for all three
	int		id;

	int		axis;					// Same for axle and gear
	int		motored;
	int		direction;
	double	angular_velocity;
	double	radius;
	TDA			color;
	TDA			position;
	float		relative_speed;			// New ccp
	int			marked;					// New ccp

	double	width;					// Similar for axle and gear

// Unique 

	char		type[NAME_SIZE];		// "NORMAL" or "BEVEL"
	int		face;						// only used for bevel
	int		teeth;
	int		relative_position;			// used only in load_data.cpp
	double	tooth_depth;
	double	angle;
	float	phase;						// New ccp

	char		axle_name[NAME_SIZE];	// Link to other elements
};

typedef	struct	GEAR	GEAR;
typedef	struct	AXLE	AXLE;
typedef	struct	BELT	BELT;

// EXTERN is defined to be nothing in main.c
// This causes the variables to be created.
// Everywhere else EXTERN is undefined,
// so that makes these variables extern.

#ifndef	EXTERN
#define	EXTERN	extern
#endif

EXTERN		AXLE**	a;
EXTERN		BELT**	b;
EXTERN		GEAR**	g;
EXTERN		int		number_of_gears;
EXTERN		int		number_of_axles;
EXTERN		int		number_of_belts;
EXTERN		TDA		background;

#define	_2D_DISTANCE(p1, p2)	sqrt(pow(p1, 2) + pow(p2, 2))
#define _3D_DISTANCE(P1, P2)	sqrt(	pow(P1[0] - P2[0],2)	\
									+	pow(P1[1] - P2[1],2)	\
									+	pow(P1[2] - P2[2],2)	\
									)

