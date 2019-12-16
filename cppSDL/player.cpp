#include "player.h"
#include "utils.h"
#include "enemy.h"
#include "wall.h"
#include <cmath>
#include <algorithm>


Player::Player( int playerID, SDL_Point& center, int diameter, std::string texturePath, std::string deadTexturePath, std::string stuckTexturePath, SDL_Renderer* renderer )
	: 	Entity( center.x - 0.5f * diameter, center.y - 0.5f * diameter, diameter, diameter ),
		mRenderer( renderer ),
		mPlayerID( playerID ),
		mIsStuck( false, true, mStuckDurationS ),
		mCanShoot( true, false, mReloadDurationS )
{
	mTexture 		= utils::LoadTexture( renderer, texturePath );
	mDeadTexture 	= utils::LoadTexture( renderer, deadTexturePath );
	mStuckTexture	= utils::LoadTexture( renderer, stuckTexturePath );
	mAimerTexture 	= utils::LoadTexture( renderer, "Resources/aimer.png" );
}

Player::~Player()
{
	if( mTexture )
		SDL_DestroyTexture( mTexture );

	if( mDeadTexture )
		SDL_DestroyTexture( mDeadTexture );

	if( mStuckTexture )
		SDL_DestroyTexture( mStuckTexture );

	if( mAimerTexture )
		SDL_DestroyTexture( mAimerTexture );

	// TODO - free any resources
}

int	Player::GetPlayerID()
{
	return mPlayerID;
}

void Player::Update( double elapsedS, unsigned long long elapsedMs, std::vector<Entity*>& entities )
{
	if( mIsDead )
		return;

	// Determine if we are still stuck or not
	mIsStuck.Update( elapsedS );
	if( mIsStuck.State() )
		return;

	// Determine if we can shoot or not
	mCanShoot.Update( elapsedS );

	// Compute how far we want to travel in each direction given how much time has elapsed
	double 	xTranslation, yTranslation;
	bool 	translatingX, translatingY;
	ComputeTranslationValuesForElapsedTime( elapsedS, translatingX, xTranslation, translatingY, yTranslation );

	if( !translatingX && !translatingY )
		return;

	// Move
	TranslateX( xTranslation );
	TranslateY( yTranslation );


	// Collision correction
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


		// TODO - Aaron: A better way of moving might be to compute the maximum distance we can move, computing our desired translation and capping the later to the former. This would work well with other entities and would require no error correction passes.


		if( utils::AreRectsIntersecting( x1, y1, x2, y2, other_x1, other_y1, other_x2, other_y2 ) )
		{

			if( entity->GetType() == Entity::Type::enemy )
			{
				Enemy* enemy = dynamic_cast<Enemy*>( entity );
				if( !enemy->IsStuck() )
				{
					mIsDead = true;
					return;
				}
			}

			else if( entity->GetType() == Entity::Type::player )
			{






				// TODO - Aaron: Add custom code for moving the other player
				if( translatingX && !translatingY )
				{
					double distanceToMoveLeft 	= std::abs( other_x1 - x2 );
					double distanceToMoveRight 	= std::abs( x1 - other_x2 );
					if( distanceToMoveLeft < distanceToMoveRight )
					{
						if( !correctedLeft ) 
						{
							entity->TranslateX( distanceToMoveLeft );
							correctedLeft = true;
						}
					}
					
					else
					{
						if( !correctedRight )
						{
							entity->TranslateX( -distanceToMoveRight );
							correctedRight = true;
						}
					}
				}






			}
			
			else
			{
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
}

void Player::Render()
{
	{ // Draw player

		SDL_Rect rect;
		rect.x = static_cast<int>( mTopLeftX );
		rect.y = static_cast<int>( mTopLeftY );
		rect.w = static_cast<int>( mWidth );
		rect.h = static_cast<int>( mHeight );
		
		if( mIsDead )
			SDL_RenderCopy( mRenderer, mDeadTexture, nullptr, &rect );
		
		else if( mIsStuck.IsActive() )
			SDL_RenderCopy( mRenderer, mStuckTexture, nullptr, &rect );

		else
			SDL_RenderCopy( mRenderer, mTexture, nullptr, &rect );
	}

	// Draw aimer
	if( !mIsDead && !mIsStuck.IsActive() )
	{

		const int aimerMargin = 5;

		SDL_Rect rect;
		rect.x = static_cast<int>( mTopLeftX ) - aimerMargin;
		rect.y = static_cast<int>( mTopLeftY ) - aimerMargin;
		rect.w = static_cast<int>( mWidth ) + ( 2 * aimerMargin );
		rect.h = static_cast<int>( mHeight ) + ( 2 * aimerMargin );

		SDL_Point center;
		center.x = ( 0.5 * rect.w );
		center.y = ( 0.5 * rect.h );

		SDL_RenderCopyEx( mRenderer, mAimerTexture, nullptr, &rect, mAimerAngle, &center, SDL_FLIP_NONE );
	}

	// TODO - do something if we are dead...
}

Entity::Type Player::GetType()
{
	return Type::player;
}

void Player::SetDead( bool isDead )
{
	mIsDead = isDead;
}

bool Player::IsDead()
{
	return mIsDead;
}

void Player::SetStuck( bool isStuck )
{
	mIsStuck.Activate();
}

void Player::KeyDownEvent( SDL_KeyboardEvent* event )
{
	bool keyPressIsAutoRepeat = ( event->repeat != 0 ); // non-zero implies it's a repeat key

	if( mIsDead || keyPressIsAutoRepeat )
		return;

	if( event->keysym.scancode == SDL_SCANCODE_UP || event->keysym.scancode == SDL_SCANCODE_W )
	{
		SetMovingUp( true );
	}

	else
	if( event->keysym.scancode == SDL_SCANCODE_DOWN || event->keysym.scancode == SDL_SCANCODE_S )
	{
		SetMovingDown( true );
	}

	else
	if( event->keysym.scancode == SDL_SCANCODE_LEFT || event->keysym.scancode == SDL_SCANCODE_A )
	{
		SetMovingLeft( true );
	}

	else
	if( event->keysym.scancode == SDL_SCANCODE_RIGHT || event->keysym.scancode == SDL_SCANCODE_D )
	{
		SetMovingRight( true );
	}

	else if( event->keysym.scancode == SDL_SCANCODE_SPACE )
	{
		ShootButtonPressed();
	}
}

void Player::KeyUpEvent( SDL_KeyboardEvent* event )
{
	bool keyPressIsAutoRepeat = ( event->repeat != 0 ); // non-zero implies it's a repeat key

	if( mIsDead || keyPressIsAutoRepeat )
		return;

	if( event->keysym.scancode == SDL_SCANCODE_UP || event->keysym.scancode == SDL_SCANCODE_W )
	{
		SetMovingUp( false );
	}

	else
	if( event->keysym.scancode == SDL_SCANCODE_DOWN || event->keysym.scancode == SDL_SCANCODE_S )
	{
		SetMovingDown( false );
	}

	else
	if( event->keysym.scancode == SDL_SCANCODE_LEFT || event->keysym.scancode == SDL_SCANCODE_A )
	{
		SetMovingLeft( false );
	}

	else
	if( event->keysym.scancode == SDL_SCANCODE_RIGHT || event->keysym.scancode == SDL_SCANCODE_D )
	{
		SetMovingRight( false );
	}
}

void Player::ControllerAxisMotionEvent( SDL_ControllerAxisEvent* event )
{
	if( mIsDead )
		return;

	switch( event->axis ) 
	{
		case SDL_CONTROLLER_AXIS_LEFTX:
		{
			if( event->value < 0 )
				mMovementXPercent = static_cast<double>( event->value ) / 32768.0;

			else if( event->value > 0 )
				mMovementXPercent = static_cast<double>( event->value ) / 32767.0;

			else
				mMovementXPercent = 0.0;

			break;
		}	

		case SDL_CONTROLLER_AXIS_LEFTY:
		{
			if( event->value < 0 )
				mMovementYPercent = static_cast<double>( event->value ) / 32768.0;

			else if( event->value > 0 )
				mMovementYPercent = static_cast<double>( event->value ) / 32767.0;

			else
				mMovementYPercent = 0.0;

			break;
		}	

		case SDL_CONTROLLER_AXIS_RIGHTX:
		{
			if( event->value < 0 )
				mAimerXPercent = static_cast<double>( event->value ) / 32768.0;

			else if( event->value > 0 )
				mAimerXPercent = static_cast<double>( event->value ) / 32767.0;

			else
				mAimerXPercent = 0.0;

			RecomputeAimerAngle();
			break;
		}	

		case SDL_CONTROLLER_AXIS_RIGHTY:
		{
			if( event->value < 0 )
				mAimerYPercent = static_cast<double>( event->value ) / 32768.0;

			else if( event->value > 0 )
				mAimerYPercent = static_cast<double>( event->value ) / 32767.0;

			else
				mAimerYPercent = 0.0;
				
			RecomputeAimerAngle();
			break;
		}	

		case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
		{
			double triggerPercent = static_cast<double>( event->value ) / 32767.0;
			if( triggerPercent > 0.2 )
				ShootButtonPressed();
				
			break;
		}

		// case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
		// {
		// 	axisDisplayName = "SDL_CONTROLLER_AXIS_TRIGGERLEFT";
		// 	break;
		// }


		default: {}
	}
}

void Player::ControllerButtonDownEvent( SDL_ControllerButtonEvent* event )
{
	if( mIsDead )
		return;

	switch( event->button )
	{
		case SDL_CONTROLLER_BUTTON_DPAD_UP:
		{
			SetMovingUp( true );
			break;
		}

		case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		{
			SetMovingDown( true );
			break;
		}

		case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		{
			SetMovingLeft( true );
			break;
		}

		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		{
			SetMovingRight( true );
			break;
		}


		default: {}
	}
}

void Player::ControllerButtonUpEvent( SDL_ControllerButtonEvent* event )
{
	if( mIsDead )
		return;

	switch( event->button )
	{
		case SDL_CONTROLLER_BUTTON_DPAD_UP:
		{
			SetMovingUp( false );
			break;
		}

		case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		{
			SetMovingDown( false );
			break;
		}

		case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		{
			SetMovingLeft( false );
			break;
		}

		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		{
			SetMovingRight( false );
			break;
		}


		default: {}
	}
}

void Player::RecomputeAimerAngle()
{
	double angle = 0.0;
	double xPercent = mAimerXPercent;
	double yPercent = mAimerYPercent;

	// Only recompute the aimer angle if the sensitivity threshold is broken
	if( std::abs( xPercent ) < 0.1 && std::abs( yPercent ) < 0.1 )
		return;

	// Top right quadrant
	if( xPercent >= 0.0 && yPercent < 0.0 )
	{
		double rads = std::atan( xPercent / std::abs( yPercent ) );
		angle = 270.0 + utils::RadToDeg( rads );
	}
	
	// Bottom right quadrant
	else if( xPercent > 0.0 && yPercent >= 0.0 )
	{
		double rads = std::atan( yPercent / xPercent );
		angle = utils::RadToDeg( rads );
	}

	// Bottom left quadrant
	else if( xPercent <= 0.0 && yPercent > 0.0 )
	{
		double rads = std::atan( std::abs( xPercent ) / yPercent );
		angle = 90.0 + utils::RadToDeg( rads );
	}

	// Top left quadrant
	else if( xPercent < 0.0 && yPercent <= 0.0 )
	{
		double rads = std::atan( std::abs( yPercent ) / std::abs( xPercent ) );
		angle = 180.0 + utils::RadToDeg( rads );
	}

	mAimerAngle = angle;
}

void Player::ShootButtonPressed()
{
	if( !mCanShoot.State() )
		return;

	double projectileX = mTopLeftX + ( 0.5 * mWidth );
	double projectileY = mTopLeftY + ( 0.5 * mHeight );

	Event* createProjectileEvent = new Event( EventType::create_player_projectile );
	
	createProjectileEvent->AddValue( EventParameterType::player_id, mPlayerID );
	createProjectileEvent->AddValue( EventParameterType::x, projectileX );
	createProjectileEvent->AddValue( EventParameterType::y, projectileY );
	createProjectileEvent->AddValue( EventParameterType::angle, mAimerAngle );

	mPendingEvents.push_back( createProjectileEvent );

	mCanShoot.Activate();
}

void Player::SetMovingUp( bool moving )
{
	if( moving )
	{
		mMovementYPercent 	+= -1.0;
		mAimerYPercent 		+= -1.0;
		RecomputeAimerAngle();
	}
	else
	{
		mMovementYPercent 	-= -1.0;
		mAimerYPercent		-= -1.0;
		RecomputeAimerAngle();
	}
}

void Player::SetMovingDown( bool moving )
{
	if( moving )
	{
		mMovementYPercent 	+= 1.0;
		mAimerYPercent 		+= 1.0;
		RecomputeAimerAngle();
	}
	else
	{
		mMovementYPercent 	-= 1.0;
		mAimerYPercent		-= 1.0;
		RecomputeAimerAngle();
	}
}

void Player::SetMovingLeft( bool moving )
{
	if( moving )
	{
		mMovementXPercent 	+= -1.0;
		mAimerXPercent 		+= -1.0;
		RecomputeAimerAngle();
	}
	else
	{
		mMovementXPercent 	-= -1.0;
		mAimerXPercent 		-= -1.0;
		RecomputeAimerAngle();
	}
}

void Player::SetMovingRight( bool moving )
{
	if( moving )
	{
		mMovementXPercent 	+= 1.0;
		mAimerXPercent 		+= 1.0;
		RecomputeAimerAngle();
	}
	else
	{
		mMovementXPercent 	-= 1.0;
		mAimerXPercent 		-= 1.0;
		RecomputeAimerAngle();
	}
}

void Player::ComputeTranslationValuesForElapsedTime( double elapsedS, bool& translatingX_OUT, double& xTranslation_OUT, bool& translatingY_OUT, double& yTranslation_OUT )
{
	xTranslation_OUT = 0.0;
	yTranslation_OUT = 0.0;
	translatingX_OUT = false;
	translatingY_OUT = false;

	if( mMovementXPercent > 0.2 || mMovementXPercent < -0.2 )
	{
		xTranslation_OUT = elapsedS * mMovementXPercent * mSpeed;
		translatingX_OUT = true;
	}

	if( mMovementYPercent > 0.2 || mMovementYPercent < -0.2 )
	{
		yTranslation_OUT = elapsedS * mMovementYPercent * mSpeed;
		translatingY_OUT = true;
	}
}

void Player::CollideWidthPLayer( Player* player )
{

}

void Player::CollideWidthEnemy( Enemy* enemy )
{

}

void Player::CollideWidthProjectile( Projectile* projectile )
{

}

void Player::CollideWidthWall( Wall* projectile )
{
	
}
