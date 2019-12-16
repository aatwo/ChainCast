#ifndef _WALL_H_
#define _WALL_H_


#include "entity.h"
#include <SDL2/SDL.h>
#include <string>


class Wall : public Entity
{
    public:

                                                Wall( bool destrutable, SDL_Rect& rect, std::string imagePath, SDL_Renderer* renderer );
                                                ~Wall();

        virtual void 							Update( double elapsedS, unsigned long long elapsedMs, std::vector<Entity*>& entities ) override;
		virtual void							Render() override;
        virtual Type							GetType() override;

        bool                                    IsDestructable();
        bool                                    IsDestroyed();
        void                                    SetDestroyed( bool destroyed );


    private:

        SDL_Renderer*							mRenderer 	    = nullptr;
		SDL_Texture*							mTexture 	    = nullptr;
        bool                                    mDestructable   = false;
        bool                                    mDestroyed      = false;

};


#endif //_WALL_H_
