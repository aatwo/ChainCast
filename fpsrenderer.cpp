#include "fpsrenderer.h"


FpsRenderer::FpsRenderer()
	:	mFpsData( 10 )
{
    mFpsFont = TTF_OpenFont( "Resources/DejaVuSans.ttf", 24 );
    if( !mFpsFont )        
        SDL_Log( "Error loading font: %s", TTF_GetError() );
}

FpsRenderer::~FpsRenderer()
{
	TTF_CloseFont( mFpsFont );
}

void FpsRenderer::Update( double elapsedS, unsigned long long elapsedMs )
{
    mElapsedS += elapsedS;
}

void FpsRenderer::Render( SDL_Renderer* renderer )
{
    double fps = 1.0 / mElapsedS;
    mElapsedS = 0.0;
    mFpsData.AddValue( fps );

    std::string fpsString = std::to_string( static_cast<int>( mFpsData.GetAverage() ) );
    SDL_Surface* fpsSurface = TTF_RenderText_Solid( mFpsFont, fpsString.c_str(), mFpsTextColour );
    SDL_Texture* fpsTexture = SDL_CreateTextureFromSurface( renderer, fpsSurface );

    SDL_Rect fpsRect;
    fpsRect.x = 0;
    fpsRect.y = 0;
    fpsRect.w = 60;
    fpsRect.h = 30;

    SDL_RenderCopy( renderer, fpsTexture, nullptr, &fpsRect );

	// Free resources
    SDL_FreeSurface( fpsSurface );
    SDL_DestroyTexture( fpsTexture );
}
