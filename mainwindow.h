#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "sdleventdelegate.h"
#include "timer.h"
#include "circularbuffer.h"
#include "entity.h"
#include "fpsrenderer.h"
#include "pathfinder.h"
#include <string>
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>


class Player;
class Enemy;
class FpsRenderer;
class PathFinder;


class MainWindow : public SDLEventDelegate
{
    public:

                                                        MainWindow( SDL_Window* window, int windowWidth, int windowHeight );
                                                        ~MainWindow();

        void                                            Update( double elapsedSeconds, int elapsedMs );
        void                                            Render();


        virtual void                                    KeyDownEvent( SDL_KeyboardEvent* event ) override;
        virtual void                                    KeyUpEvent( SDL_KeyboardEvent* event ) override;
        virtual void                                    MouseButtonDownEvent( SDL_MouseButtonEvent* event ) override;
        virtual void                                    ControllerAddedEvent( int controllerID, SDL_ControllerDeviceEvent* event ) override;
        virtual void                                    ControllerRemovedEvent( int controllerID, SDL_ControllerDeviceEvent* event ) override;
        virtual void                                    ControllerAxisMotionEvent( int controllerID, SDL_ControllerAxisEvent* event ) override;
        virtual void                                    ControllerButtonDownEvent( int controllerID, SDL_ControllerButtonEvent* event ) override;
        virtual void                                    ControllerButtonUpEvent( int controllerID, SDL_ControllerButtonEvent* event ) override;


    private:

        void                                            DrawGrid();
        void                                            LoadLevel();
        void                                            LoadPlayers();
        void                                            LoadEnemy();
        void                                            LoadOverlay();
        void                                            CreateWall( int x, int y, int w, int h, std::string texturePath, bool destructable );
        void                                            CreatePlayerProjectile( int playerID, double x, double y, double xPercent, double yPercent );
        void                                            ProcessEvents( std::vector<Event*>& eventList );
        void                                            OnCreatePlayerProjectileEvent( Event* event );


        SDL_Renderer*                                   mRenderer;
        SDL_Window*                                     mWindow;
        int                                             mWindowWidth = 0;
        int                                             mWindowHeight = 0;

        std::vector<SDL_Texture*>                       mTextureList;
        std::vector<Mix_Music*>                         mMusicList;
        std::vector<Mix_Chunk*>                         mSoundList;
        std::vector<Entity*>                            mAllEntities;
        
        std::unique_ptr<PathFinder>                     mPathFinder     = nullptr;
        std::unique_ptr<FpsRenderer>                    mFpsRenderer    = nullptr;
        Enemy*                                          mEnemy          = nullptr;
        Player*                                         mPlayerOne      = nullptr;
        Player*                                         mPlayerTwo      = nullptr;
        Player*                                         mPlayerThree    = nullptr;
        Player*                                         mPlayerFour     = nullptr;

        bool                                            mClickToggle = false;
        int                                             mClickX1, mClickY1, mClickX2, mClickY2;

        const int                                       mWallCreationChancePercent  = 30;
        const int                                       mProjectileDiameter    = 20;
        const int                                       mGridWidth                  = 25;
        const int                                       mGridHeight                 = 25;
        const int                                       mCellWidth;
        const int                                       mCellHeight;
        const int                                       mPlayerDiameter;
        const int                                       mEnemyDiameter;


}; // MainWindow


#endif // MAINWINDOW_H
