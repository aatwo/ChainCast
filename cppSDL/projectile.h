#ifndef _PROJECTILE_H_
#define _PROJECTILE_H_


#include "entity.h"
#include <SDL2/SDL.h>
#include <string>


class Projectile : public Entity
{
	public:

												Projectile( int playerID, SDL_Point& center, double xPercent, double yPercent, int diameter, std::string texturePath, SDL_Renderer* renderer );

		bool									IsDead();
		virtual void							Update( double elapsedS, unsigned long long elapsedMs, std::vector<Entity*>& entities ) override;
		virtual	void							Render() override;
		virtual Type							GetType() override;


	private:

		SDL_Renderer*							mRenderer 			= nullptr;
		SDL_Texture*							mTexture 			= nullptr;
		bool									mIsDead				= false;
		int										mPlayerID			= -1;
		double									mCollisionRadius	= 0.0;
		double									mXPercent			= 0.0;
		double									mYPercent			= 0.0;
		const int								mSpeed				= 400;


}; // Projectile


#endif // _PROJECTILE_H_
