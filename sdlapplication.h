#ifndef SDLAPPLICATION_H
#define SDLAPPLICATION_H


#include <map>
#include <vector>
#include <SDL2/SDL.h>
#include "global.h"
#include "mainwindow.h"


class SDLApplication
{
    public:

                                                SDLApplication();
                                                ~SDLApplication();

        static SDLApplication*                  Instance();

        bool                                    Init();
        int                                     Exec();


    private:

        void                                    ProcessSdlEventQueue();

        void                                    HandleWindowEvent( SDL_WindowEvent* event );
        void                                    HandleKeyboardEvent( SDL_KeyboardEvent* event );
        void                                    HandleMouseButtonEvent( SDL_MouseButtonEvent* event );
        void                                    HandleMouseMotionEvent( SDL_MouseMotionEvent* event );
        void                                    HandleMouseWheelEvent( SDL_MouseWheelEvent* event );

        void                                    HandleControllerAdded( SDL_ControllerDeviceEvent* event );
        void                                    HandleControllerRemoved( SDL_ControllerDeviceEvent* event );
        void                                    HandleControllerAxisMotion( SDL_ControllerAxisEvent* event );
        void                                    HandleControllerButtonDown( SDL_ControllerButtonEvent* event );


        std::map<int, SDL_GameController*>      mControllerIdToControllerMap;
        bool                                    mApplicationRunning     = true;
        MainWindow*                             mMainWindow             = nullptr;
        SDL_Window*                             mSdlWindow              = nullptr;
        int                                     mWindowWidth            = 1000;
        int                                     mWindowHeight           = 1000;
        static SDLApplication*                  sInstance;


}; // SDLApplication


#endif // SDLAPPLICATION_H
