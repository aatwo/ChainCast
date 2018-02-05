#ifndef _ENEMY_H_
#define _ENEMY_H_


#include "entity.h"
#include "pathfinder.h"
#include "statetracker.h"
#include <string>
#include <vector>
#include <memory>


class Player;


class Enemy : public Entity
{
	public:

												Enemy( std::vector<Player*> playerList, SDL_Point& center, int diameter, SDL_Renderer* renderer, int windowWidth, int windowHeight, int gridWidth, int gridHeight );

		virtual void							Update( double elapsedS, unsigned long long elapsedMs, std::vector<Entity*>& entities ) override;
		virtual	void							Render() override;
		virtual Type							GetType() override;
		void									SetStuck( bool isStuck );
		bool									IsStuck();

		
	private:

		void									RecomputeMovementForNextPathTarget();
		void									RecomputePathToNearestPlayer();


		const double							mSpeed 						= 140.0;
		const double							mStuckDurationS				= 1.0;
		const double							mPathFindingUpdateInterval 	= 0.5;
		const int								mWindowWidth;
		const int								mWindowHeight;
		const int								mGridWidth;
		const int								mGridHeight;
		const int								mCellWidth;
		const int								mCellHeight;


		SDL_Renderer*							mRenderer 					= nullptr;
		SDL_Texture*							mTexture					= nullptr;
		SDL_Texture*							mStuckTexture				= nullptr;
		std::unique_ptr<PathFinder>				mPathFinder					= nullptr;
		double									mXPercent					= 0.0;
		double									mYPercent					= 0.0;

		StateTracker<bool>						mIsStuck;
		double									mTimeSincePathFindingUpdate	= 0.0;
		std::vector<Player*>					mPlayerList;
		std::vector<SDL_Point> 					mCurrentPath;


}; // Enemy


#endif // _ENEMY_H_
