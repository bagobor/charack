#ifndef __CHARACK_MAP_GENERATOR_H_
#define __CHARACK_MAP_GENERATOR_H_

#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "CharackCoastGenerator.h"
#include "CharackLineSegment.h"
#include "../tools/vector3.h"

#define BLACK 0
#define BACK 1
#define GRID 2
#define WHITE 3
#define BLUE0 4

#define MAXCOL	10
typedef int CTable[MAXCOL][3];
    
#ifndef PI
	#define PI 3.14159265358979
#endif

#ifndef DEG2RAD
	#define DEG2RAD 0.0174532918661 /* pi/180 */
#endif

/**
 * Generates the map that all Charack classes will use to create the virtual world. The generated map
 * is a small representation of all elements in the world, such as continents and oceans.
 *
 * This class was created because its very hard to define land/water portions in the virtual world
 * using only math functions. Using the small representation generated by CharackMapGenerator, the
 * other classes can "guess" what to create in a specific position in the world (at least they will
 * know if the specific position is a piece of land or water).
 *
 * This class uses the continent generation algorithm by Torben AE. Mogensen <torbenm@diku.dk>. The original code
 * was downloaded from http://www.diku.dk/hjemmesider/ansatte/torbenm/, I just removed the code Charack will not
 * need and encapsulated the rest all togheter inside a class.
 */
class CharackMapGenerator {
	private:
		int altColors;
		int BLUE1, LAND0, LAND1, LAND2, LAND4;
		int GREEN1, BROWN0, GREY0;
		int back;
		int nocols;
		int rtable[256], gtable[256], btable[256];
		int lighter; /* specifies lighter colours */
		double M;   /* initial altitude (slightly below sea level) */
		double dd1;   /* weight for altitude difference */
		double dd2;  /* weight for distance */
		double POW;  /* power for distance function */

		int Depth; /* depth of subdivisions */
		double r1,r2,r3,r4; /* seeds */
		double longi,lat,scale;
		double vgrid, hgrid;

		int latic; /* flag for latitude based colour */

		int Width;
		int Height;

		unsigned char **col;
		int **heights;
		int cl0[60][30];

		int do_outline;
		int do_bw;
		int *outx, *outy;

		double cla, sla, clo, slo;

		double rseed;
		double increment;

		int best;
		int weight[30];

		int min_dov(int x, int y);
		int max_dov(int x, int y);
		double fmin_dov(double x, double y);
		double fmax_dov(double x, double y);
		void setcolours();
		void makeoutline(int do_bw);
		void mercator();
		int planet0(double x, double y, double z);
		double planet(double a,double b,double c,double d, double as, double bs, double cs, double ds, double ax, double ay, double az, double bx, double by, double bz, double cx, double cy, double cz, double dx, double dy, double dz, double x, double y, double z, int level);
		double planet1(double x, double y, double z);
		double rand2(double p, double q);
		void printbmp(FILE *outfile);
		void printbmpBW(FILE *outfile);
		double log_2(double x);

	public:
		CharackMapGenerator();
		~CharackMapGenerator();

		// Generate the world (land and water)
		void generate();

		// Globaly check if a specific position is land or water. The test is made against the macro world information,
		// which means coast lines and similar things are not analyzed. The method isLand() will use the information
		// of globalIsLand() as a "clue" to identify the coast lines, so it can generate a highly detailed map
		// (with noised coast lines) that can be used to check if a specific position is land or water.
		int isLand(float theX, float theZ);

		// This method will find all coast lines (which are straight lines before the method call) and, for each one,
		// generate a much more real coast line, adding some noise to the lines.
		void applyCoast(int theMapX, int theMapZ, int theViewFrustum, int theSample);
};

#endif