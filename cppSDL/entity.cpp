#include "entity.h"


Entity::Entity( double topLeftX, double topLeftY, double width, double height )
{
	mTopLeftX 	= topLeftX;
	mTopLeftY 	= topLeftY;
	mWidth 		= width;
	mHeight 	= height;
}

void Entity::GetRect( double& x_OUT, double& y_OUT, double& width_OUT, double& height_OUT )
{
	x_OUT 		= mTopLeftX;
	y_OUT 		= mTopLeftY;
	width_OUT 	= mWidth;
	height_OUT 	= mHeight;
}

void Entity::GetCenter( double& x_OUT, double& y_OUT )
{
	x_OUT = mTopLeftX + ( 0.5 * mWidth );
	y_OUT = mTopLeftY + ( 0.5 * mHeight );
}

void Entity::TranslateX( double deltaX )
{
	mTopLeftX += deltaX;
}

void Entity::TranslateY( double deltaY )
{
	mTopLeftY += deltaY;
}

void Entity::GetPendingEvents( std::vector<Event*>& pendingEvents_OUT )
{
	for( Event* event : mPendingEvents )
		pendingEvents_OUT.push_back( event );
	mPendingEvents.clear();
}
