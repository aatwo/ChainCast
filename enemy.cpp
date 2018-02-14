#include "enemy.h"
#include "player.h"
#include "utils.h"
#include <cmath>


Enemy::Enemy( std::vector<Player*> playerList, SDL_Point& center, int diameter, SDL_Renderer* renderer, int windowWidth, int windowHeight, int gridWidth, int gridHeight )
	: 	Entity( center.x - 0.5f * diameter, center.y - 0.5f * diameter, diameter, diameter ),
		mRenderer( renderer ),
		mPlayerList( playerList ),
		mWindowWidth( windowWidth ),
		mWindowHeight( windowHeight ),
		mGridWidth( gridWidth ),
		mGridHeight( gridHeight ),
		mCellWidth( mWindowWidth / mGridWidth ),
		mCellHeight( mWindowHeight / mGridHeight ),
		mIsStuck( false, true, mStuckDurationS )
{
	mPathFinder		= std::unique_ptr<PathFinder>( new PathFinder( windowWidth, windowHeight, gridWidth, gridHeight ) );
	mTexture 		= utils::LoadTexture( mRenderer, "Resources/enemy.png" );
	mStuckTexture 	= utils::LoadTexture( mRenderer, "Resources/enemyStuck.png" );
}

void Enemy::Update( double elapsedS, unsigned long long elapsedMs, std::vector<Entity*>& entities )
{
	// If stuck increment the stuck counter and free if necessary
	mIsStuck.Update( elapsedS );
	if( mIsStuck.State() )
		return;

	// Perform path finding to nearest player
	mTimeSincePathFindingUpdate += elapsedS;
	if( mTimeSincePathFindingUpdate >= mPathFindingUpdateInterval )
	{
		mPathFinder->SetEntities( entities );
		RecomputePathToNearestPlayer();

		mTimeSincePathFindingUpdate = 0.0;
	}

	// Move towards the next target node
	RecomputeMovementForNextPathTarget();

	double translateX = elapsedS * mXPercent * mSpeed;
	double translateY = elapsedS * mYPercent * mSpeed;

	bool translatingX = false;
	bool translatingY = false;

	if( mXPercent > 0.2 || mXPercent < -0.2 )
	{
		TranslateX( translateX );
		translatingX = true;
	}

	if( mYPercent > 0.2 || mYPercent < -0.2 )
	{
		TranslateY( translateY );
		translatingY = true;
	}

	if( !translatingX && !translatingY )
		return;

	double x1 = mTopLeftX;
	double x2 = mTopLeftX + mWidth;
	double y1 = mTopLeftY;
	double y2 = mTopLeftY + mHeight;

	bool correctedLeft 	= false;
	bool correctedRight = false;
	bool correctedUp 	= false;
	bool correctedDown 	= false;


	for( Entity* entity : entities )
	{
		if( this == entity )
			continue;

		if( entity->GetType() == Entity::Type::projectile )
			continue;

		double other_x1;
		double other_x2;
		double other_y1;
		double other_y2;

		entity->GetRect( other_x1, other_y1, other_x2, other_y2 );
		other_x2 = other_x1 + other_x2;
		other_y2 = other_y1 + other_y2;

		if( utils::AreRectsIntersecting( x1, y1, x2, y2, other_x1, other_y1, other_x2, other_y2 ) )
		{

			if( entity->GetType() == Entity::Type::player )
			{
				Player* player = dynamic_cast<Player*>( entity );
				player->SetDead( true );
				continue;
			}


			if( translatingX && translatingY ) 
			{
				// They intersect so translate the least amount of distance to fix it...
				double distanceToMoveUp 	= std::abs( other_y1 - y2 );
				double distanceToMoveDown 	= std::abs( y1 - other_y2 );
				double distanceToMoveLeft 	= std::abs( other_x1 - x2 );
				double distanceToMoveRight 	= std::abs( x1 - other_x2 );

				if( utils::IsValueTheSmallest( distanceToMoveLeft, distanceToMoveUp, distanceToMoveDown, distanceToMoveRight ) )
				{
					if( !correctedLeft )
					{
						TranslateX( -distanceToMoveLeft );
						correctedLeft = true;
					}
				}

				else 
				if( utils::IsValueTheSmallest( distanceToMoveRight, distanceToMoveUp, distanceToMoveDown, distanceToMoveLeft ) )
				{
					if( !correctedRight ) 
					{					
						TranslateX( distanceToMoveRight );
						correctedRight = true;
					}
				}
				
				else
				if( utils::IsValueTheSmallest( distanceToMoveUp, distanceToMoveDown, distanceToMoveLeft, distanceToMoveRight ) )
				{
					if( !correctedUp )
					{
						TranslateY( -distanceToMoveUp );
						correctedUp = true;
					}
				}

				else
				if( utils::IsValueTheSmallest( distanceToMoveDown, distanceToMoveUp, distanceToMoveLeft, distanceToMoveRight ) )
				{
					if( !correctedDown )
					{
						TranslateY( distanceToMoveDown );
						correctedDown = true;
					}
				}
			}

			else if( translatingX && !translatingY )
			{
				double distanceToMoveLeft 	= std::abs( other_x1 - x2 );
				double distanceToMoveRight 	= std::abs( x1 - other_x2 );
				if( distanceToMoveLeft < distanceToMoveRight )
				{
					if( !correctedLeft ) 
					{
						TranslateX( -distanceToMoveLeft );
						correctedLeft = true;
					}
				}
				
				else
				{
					if( !correctedRight )
					{
						TranslateX( distanceToMoveRight );
						correctedRight = true;
					}
				}
			}

			else if( !translatingX && translatingY )
			{
				double distanceToMoveUp 	= std::abs( other_y1 - y2 );
				double distanceToMoveDown 	= std::abs( y1 - other_y2 );

				if( distanceToMoveUp < distanceToMoveDown )
				{
					if( !correctedUp )
					{
						TranslateY( -distanceToMoveUp );
						correctedUp = true;
					}
				}

				else
				{
					if( !correctedDown )
					{
						TranslateY( distanceToMoveDown );
						correctedDown = true;
					}
				}
			}
		}
	}
}

void Enemy::Render()
{
	SDL_Rect rect;

	{ // Draw current path

		rect.w = mCellWidth;
		rect.h = mCellHeight;

		SDL_SetRenderDrawColor( mRenderer, 185, 255, 112, 200 );
		for( SDL_Point& point : mCurrentPath )
		{
			rect.x = point.x * mCellWidth;
			rect.y = point.y * mCellHeight;
			SDL_RenderFillRect( mRenderer, &rect );
		}
	}

	// Draw Enemy
	rect.x = static_cast<int>( mTopLeftX );
	rect.y = static_cast<int>( mTopLeftY );
	rect.w = static_cast<int>( mWidth );
	rect.h = static_cast<int>( mHeight );

	if( mIsStuck.State() )
		SDL_RenderCopy( mRenderer, mStuckTexture, nullptr, &rect );
	else
		SDL_RenderCopy( mRenderer, mTexture, nullptr, &rect );
}

Entity::Type Enemy::GetType()
{
	return Type::enemy;
}

void Enemy::SetStuck( bool isStuck )
{
	mIsStuck.Activate();
}

bool Enemy::IsStuck()
{
	return mIsStuck.State();
}

void Enemy::RecomputeMovementForNextPathTarget()
{
	mXPercent = 0.0;
	mYPercent = 0.0;

	if( mCurrentPath.size() <= 1 )
		return;

	double x, y;
	GetCenter( x, y );

	int gridX, gridY;
	utils::GridCoordinateFromScreenCoordinate( x, y, mCellWidth, mCellHeight, gridX, gridY );

	SDL_Point targetPoint = mCurrentPath[ mCurrentPath.size() - 2 ];
	double targetCenterX = ( targetPoint.x * mCellWidth ) + ( 0.5 * mCellWidth );
	double targetCenterY = ( targetPoint.y * mCellHeight ) + ( 0.5 * mCellHeight );

	double thresholdPixels = 3.0;

	// Horizontal
	bool arrivedAtX = std::abs( targetCenterX - x ) < thresholdPixels;
	if( !arrivedAtX )
	{
		if( x > targetCenterX )
			mXPercent = -1.0;
		
		else if( x < targetCenterX )
			mXPercent = 1.0;
	}

	// Vertical
	bool arrivedAtY = std::abs( targetCenterY - y ) < thresholdPixels;
	if( !arrivedAtY )
	{
		if( y > targetCenterY )
			mYPercent = -1.0;

		else if( y < targetCenterY )
			mYPercent = 1.0;
	}

	if( arrivedAtX && arrivedAtY )
	{
		RecomputePathToNearestPlayer();
	}
}

void Enemy::RecomputePathToNearestPlayer()
{
	int 					closestPlayerDistance 	= -1;
	Player* 				closestPlayer 			= nullptr;
	std::vector<SDL_Point>	closestPath;
	
	for( Player* player : mPlayerList )
	{
		if( player->IsDead() )
			continue;

		double playerX, playerY;
		player->GetCenter( playerX, playerY );

		int playerGridX, playerGridY;
		utils::GridCoordinateFromScreenCoordinate( playerX, playerY, mCellWidth, mCellHeight, playerGridX, playerGridY );

		double centerX, centerY;
		GetCenter( centerX, centerY );

		int gridX, gridY;
		utils::GridCoordinateFromScreenCoordinate( centerX, centerY, mCellWidth, mCellHeight, gridX, gridY );

		mPathFinder->FindPath( gridX, gridY, playerGridX, playerGridY );
		int distanceToPlayer = mPathFinder->GetPathDistance();

		if( distanceToPlayer < 0 )
			continue;

		if( !closestPlayer || distanceToPlayer < closestPlayerDistance )
		{
			closestPlayer 			= player;
			closestPlayerDistance 	= distanceToPlayer;
			closestPath				= mPathFinder->GetPath();
		}
	}

	if( closestPlayer ) 
	{
		mCurrentPath = closestPath;
	}
}
