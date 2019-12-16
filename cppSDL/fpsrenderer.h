#ifndef _FPS_RENDERER_H_
#define _FPS_RENDERER_H_


#include "circularbuffer.h"
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


class FpsRenderer
{
	public:

												FpsRenderer();
												~FpsRenderer();

		void									Update( double elapsedS, unsigned long long elapsedMs );
		void									Render( SDL_Renderer* renderer );


	private:

        CircularBuffer                          mFpsData;
		double									mElapsedS		= 0.0;
        TTF_Font*                               mFpsFont        = nullptr;
        SDL_Color                               mFpsTextColour  = { 255, 0, 0, 255 };


}; // FpsRenderer


#endif // _FPS_RENDERER_H_