#ifndef _PLAYER_H_
#define _PLAYER_H_


#include "entity.h"
#include "statetracker.h"
#include <string>
#include <SDL2/SDL.h>


class Enemy;
class Projectile;
class Wall;


class Player : public Entity
{
	public:

												Player( int playerID, SDL_Point& center, int diameter, std::string texturePath, std::string deadTexturePath, std::string stuckTexturePath, SDL_Renderer* renderer );
												~Player();

		int										GetPlayerID();
		virtual void 							Update( double elapsedS, unsigned long long elapsedMs, std::vector<Entity*>& entities ) override;
		virtual void							Render() override;
		virtual Type							GetType() override;
		void									SetDead( bool isDead );
		bool									IsDead();
		void									SetStuck( bool isStuck );
		void                                    KeyDownEvent( SDL_KeyboardEvent* event );
        void                                    KeyUpEvent( SDL_KeyboardEvent* event );
		void                                    ControllerAxisMotionEvent( SDL_ControllerAxisEvent* event );
        void                                    ControllerButtonDownEvent( SDL_ControllerButtonEvent* event );
        void                                    ControllerButtonUpEvent( SDL_ControllerButtonEvent* event );


	private:

		void									RecomputeAimerAngle();
		void									ShootButtonPressed();
		void									SetMovingUp( bool moving );
		void									SetMovingDown( bool moving );
		void									SetMovingLeft( bool moving );
		void									SetMovingRight( bool moving );
		void									ComputeTranslationValuesForElapsedTime( double elapsedS, bool& translatingX_OUT, double& xTranslation_OUT, bool& translatingY_OUT, double& yTranslation_OUT );
		void 									CollideWidthPLayer( Player* player );
		void 									CollideWidthEnemy( Enemy* enemy );
		void 									CollideWidthProjectile( Projectile* projectile );
		void 									CollideWidthWall( Wall* projectile );


		const double							mSpeed 				= 120.0;
		const double							mReloadDurationS	= 0.5;
		const double							mStuckDurationS		= 2.0;


		int										mPlayerID			= -1;
		SDL_Renderer*							mRenderer 			= nullptr;
		SDL_Texture*							mTexture 			= nullptr;
		SDL_Texture*							mDeadTexture 		= nullptr;
		SDL_Texture*							mStuckTexture		= nullptr;
		SDL_Texture*							mAimerTexture		= nullptr;
		
		// Aimer
		double									mAimerAngle			= 0.0;
		double									mAimerXPercent		= 0.0;
		double									mAimerYPercent		= 0.0;
		double 									mMovementXPercent	= 0.0;
		double 									mMovementYPercent	= 0.0;

		// Player states
		bool									mIsDead				= false;
		StateTracker<bool>						mIsStuck;
		StateTracker<bool>						mCanShoot;


}; // Player


#endif // _PLAYER_H_
