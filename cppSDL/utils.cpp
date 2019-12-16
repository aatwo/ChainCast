#include "utils.h"
#include <SDL2/SDL_image.h>
#include <cmath>


SDL_Texture* utils::LoadTexture( SDL_Renderer* renderer, std::string path )
{
	//Load image at specified path
    SDL_Surface* surface = IMG_Load( path.c_str() );
    if( !surface )
    {
        SDL_Log( "Failed to load image: %s", SDL_GetError() );
        return nullptr;
    }

    //Convert surface to screen format
    SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer, surface );

    // Free the surface which is no longer required
    SDL_FreeSurface( surface );

    if( !texture )
        SDL_Log( "Failed to convert surface to texture: %s", SDL_GetError() );

    return texture;
}

int utils::GetRandomNumber( int min, int max )
{
    return rand() % ( max - min + 1 ) + min;
}

double utils::RadToDeg( double rad )
{
    return rad * ( 180.0 / PI );
}

double utils::DegToRad( double deg )
{
    return ( deg * PI ) / 180.0;
}

int utils::RoundToNearestMultiple( int value, int multiple )
{
    int remainder = value % multiple;
    
    if( remainder == 0 )
        return value;

    return value + multiple - remainder;
}

bool utils::IsValueTheSmallest( double value, double otherA, double otherB, double otherC )
{
    return ( value < otherA ) && ( value < otherB ) && ( value < otherC );
}

bool utils::AreRectsIntersecting( double x1, double y1, double x2, double y2, double other_x1, double other_y1, double other_x2, double other_y2 )
{
    return ( x1 < other_x2 && x2 > other_x1 && y1 < other_y2 && y2 > other_y1 );
}

bool utils::AreRangesIntersecting( double a1, double a2, double b1, double b2 )
{
    return  ( a1 > b1 && a1 < b2 ) ||
            ( a2 > b1 && a2 < b2 ) ||
            ( b1 > a1 && b1 < a2 ) ||
            ( b2 > a1 && b2 < a2 );
}

bool utils::AreCirclesIntersecting( double circle1X, double circle1Y, double circle1Radius, double circle2X, double circle2Y, double circle2Radius )
{
    double distanceBetweenCentersX = std::abs( circle1X - circle2X );
    double distanceBetweenCentersY = std::abs( circle1Y - circle2Y );

    double distanceBetweenCenters = std::sqrt( distanceBetweenCentersX * distanceBetweenCentersX + distanceBetweenCentersY * distanceBetweenCentersY );
    return distanceBetweenCenters < ( circle1Radius + circle2Radius );
}

void utils::GetUnitVectorFromAngle( double angleDegrees, double& x_OUT, double& y_OUT )
{
    double rad = DegToRad( angleDegrees );

    x_OUT = std::cos( rad );
    y_OUT = std::sin( rad );
}

double utils::DistanceBetweenPoints( double x1, double y1, double x2, double y2 )
{
    double deltaX = std::abs( x1 - x2 );
    double deltaY = std::abs( y1 - y2 );

    return std::sqrt( ( deltaX * deltaX ) + ( deltaY * deltaY ) );
}

int	utils::ManhattanDistanceBetweenPoints( int x1, int y1, int x2, int y2 )
{
    int deltaX = std::abs( x1 - x2 );
    int deltaY = std::abs( y1 - y2 );

    return deltaX + deltaY;
}

int utils::GridCoordinateFromScreenCoordinate( double screenX, double screenY, int cellWidth, int cellHeight, int& gridX_OUT, int& gridY_OUT )
{
    int remainder = static_cast<int>( screenX ) % cellWidth;
    gridX_OUT = ( static_cast<int>( screenX ) - remainder ) / cellWidth;

    remainder = static_cast<int>( screenY ) % cellHeight ;
    gridY_OUT = ( static_cast<int>( screenY ) - remainder ) / cellHeight;
}
