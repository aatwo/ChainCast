#include "wall.h"
#include "utils.h"


Wall::Wall( bool destrutable, SDL_Rect& rect, std::string imagePath, SDL_Renderer* renderer )
	: 	Entity( rect.x, rect.y, rect.w, rect.h ),
		mRenderer( renderer ),
		mDestructable( destrutable )
{
	mTexture = utils::LoadTexture( renderer, imagePath );
}

Wall::~Wall()
{
    if( mTexture )
		SDL_DestroyTexture( mTexture );
}

void Wall::Update( double elapsedS, unsigned long long elapsedMs, std::vector<Entity*>& entities )
{
    // No update required
}

void Wall::Render()
{
    SDL_Rect rect;
	rect.x = static_cast<int>( mTopLeftX );
	rect.y = static_cast<int>( mTopLeftY );
	rect.w = static_cast<int>( mWidth );
	rect.h = static_cast<int>( mHeight );
	SDL_RenderCopy( mRenderer, mTexture, nullptr, &rect );
}

Entity::Type Wall::GetType()
{
	if( mDestructable )
		return Type::destructable_wall;
	
	return Type::wall;
}

bool Wall::IsDestructable()
{
	return mDestructable;
}

bool Wall::IsDestroyed()
{
	return mDestroyed;
}

void Wall::SetDestroyed( bool destroyed )
{
	if( !mDestructable )
		return;

	mDestroyed = destroyed;
}
