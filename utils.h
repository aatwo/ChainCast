#ifndef _UTILS_H_
#define _UTILS_H_


#include <SDL2/SDL.h>
#include <string>


namespace utils 
{

	SDL_Texture*                                    LoadTexture( SDL_Renderer* renderer, std::string path );
	int 											GetRandomNumber( int min, int max );
	double											RadToDeg( double rad );
	double											DegToRad( double deg );
	int												RoundToNearestMultiple( int value, int multiple );
	bool											IsValueTheSmallest( double value, double otherA, double otherB, double otherC );
	bool											AreRectsIntersecting( double x1, double y1, double x2, double y2, double other_x1, double other_y1, double other_x2, double other_y2 );
	bool											AreRangesIntersecting( double a1, double a2, double b1, double b2 );
	bool											AreCirclesIntersecting( double circle1X, double circle1Y, double circle1Radius, double circle2X, double circle2Y, double circle2Radius );
	void											GetUnitVectorFromAngle( double angleDegrees, double& x_OUT, double& y_OUT );
	double											DistanceBetweenPoints( double x1, double y1, double x2, double y2 );
	int												ManhattanDistanceBetweenPoints( int x1, int y1, int x2, int y2 );
	int                                     		GridCoordinateFromScreenCoordinate( double screenX, double screenY, int cellWidth, int cellHeight, int& gridX_OUT, int& gridY_OUT );

	const double 									PI = 3.14159265;


}


#endif // _UTILS_H_
