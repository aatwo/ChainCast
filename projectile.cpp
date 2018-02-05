#include "projectile.h"
#include "player.h"
#include "enemy.h"
#include "wall.h"
#include "utils.h"


Projectile::Projectile( int playerID, SDL_Point& center, double xPercent, double yPercent, int diameter, std::string texturePath, SDL_Renderer* renderer )
	:	Entity( center.x - ( 0.5 * diameter ), center.y - ( 0.5 * diameter ), diameter, diameter ),
		mCollisionRadius( 0.6 * 0.5 * static_cast<double>( diameter ) ),
		mXPercent( xPercent ),
		mYPercent( yPercent ),
		mPlayerID( playerID ),
		mRenderer( renderer )
{
	mTexture = utils::LoadTexture( renderer, texturePath );
}

bool Projectile::IsDead()
{
	return mIsDead;
}

void Projectile::Update( double elapsedS, unsigned long long elapsedMs, std::vector<Entity*>& entities )
{
	double translateX = elapsedS * mXPercent * mSpeed;
	double translateY = elapsedS * mYPercent * mSpeed;

	if( mXPercent > 0.2 || mXPercent < -0.2 )
	{
		TranslateX( translateX );
	}

	if( mYPercent > 0.2 || mYPercent < -0.2 )
	{
		TranslateY( translateY );
	}

	double centerX, centerY;
	GetCenter( centerX, centerY );

	for( Entity* entity : entities )
	{
		if( entity == this )
			continue;

		// Don't collide with other projectiles
		if( entity->GetType() == Entity::Type::projectile )
			continue;

		// Don't collide with players with the same ID as us
		if( entity->GetType() == Entity::Type::player )
		{
			Player* player = dynamic_cast<Player*>( entity );
			if( mPlayerID == player->GetPlayerID() )
				continue;
		}

		double other_x;
		double other_y;
		double other_w;
		double other_h;
		entity->GetRect( other_x, other_y, other_w, other_h );

		double other_centerX 	= other_x + 0.5 * other_w;
		double other_centerY 	= other_y + 0.5 * other_h;
		double other_radius 	= 0.5 * other_w;

		if( utils::AreCirclesIntersecting( centerX, centerY, mCollisionRadius, other_centerX, other_centerY, other_radius ) )
		{
			Entity::Type type = entity->GetType();
			mIsDead = true;

			if( entity->GetType() == Entity::Type::destructable_wall )
			{
				Wall* wall = dynamic_cast<Wall*>( entity );
				wall->SetDestroyed( true );
			}

			else if( entity->GetType() == Entity::Type::player )
			{
				Player* player = dynamic_cast<Player*>( entity );
				player->SetStuck( true );
			}

			else if( entity->GetType() == Entity::Type::enemy )
			{
				Enemy* enemy = dynamic_cast<Enemy*>( entity );
				enemy->SetStuck( true );
			}
			
		}
	}
}

void Projectile::Render()
{
	SDL_Rect rect;
	rect.x = static_cast<int>( mTopLeftX );
	rect.y = static_cast<int>( mTopLeftY );
	rect.w = static_cast<int>( mWidth );
	rect.h = static_cast<int>( mHeight );
	
	SDL_RenderCopy( mRenderer, mTexture, nullptr, &rect );
}

Entity::Type Projectile::GetType()
{
	return Entity::Type::projectile;
}
