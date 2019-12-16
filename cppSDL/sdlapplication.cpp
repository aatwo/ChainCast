#include "sdlapplication.h"
#include <cassert>
#include "timer.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>


SDLApplication* SDLApplication::sInstance = nullptr;


SDLApplication::SDLApplication()
{
    assert( !sInstance ); // There can only be one...
    sInstance = this;
}

SDLApplication::~SDLApplication()
{
    SDL_Log( "Cleaning up SDL" );

    delete mMainWindow;
    if( mSdlWindow )
        SDL_DestroyWindow( mSdlWindow );

    Mix_Quit();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

SDLApplication* SDLApplication::Instance()
{
    return sInstance;
}

bool SDLApplication::Init()
{
    SDL_Log( "Initialising SDL" );
    if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
    {
        SDL_Log( "Unable to initialise SDL: %s", SDL_GetError() );
        return false;
    }

    SDL_Log( "Initialising SDL TTF" );
    if( TTF_Init() == -1 ) 
    {
        SDL_Log( "Unable to initialise SDL TTF: %s", TTF_GetError() );
        return false;
    }

    SDL_Log( "Initialising SDL image" );
    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) )
    {
        SDL_Log( "Unable to initialise SDL image: %s", IMG_GetError() );
        return false;
    }

    SDL_Log( "Initialising SDL mixer" );
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        SDL_Log( "Unable to initialise SDL mixer: %s", Mix_GetError() );
        return false;
    }

    // SDL_WINDOWPOS_UNDEFINED
    mSdlWindow = SDL_CreateWindow( global::applicationName.c_str(), 500, 500, mWindowWidth, mWindowHeight, SDL_WINDOW_SHOWN );

    if( !mSdlWindow ) {
        
        SDL_Log( "Unable to create SDL window: %s", SDL_GetError() );
        return false;
    }

    mMainWindow = new MainWindow( mSdlWindow, mWindowWidth, mWindowHeight );

    return true;
}

int SDLApplication::Exec()
{
    Timer timer;
    
    while( mApplicationRunning )
    {
        ProcessSdlEventQueue();

        if( mMainWindow ) {

            double elapsedS = timer.ElapsedS();
            if( elapsedS >= 0.016666667 )
            {
                mMainWindow->Update( elapsedS, timer.ElapsedMs() );
                mMainWindow->Render();
                timer.Restart();
            }
        }        
    }

    return 0;
}

void SDLApplication::ProcessSdlEventQueue()
{
    SDL_Event event;
    while( SDL_PollEvent( &event ) != 0 )
    {

        switch( event.type )
        {

            case SDL_QUIT:
            {
                mApplicationRunning = false;
                break;
            }

            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                HandleKeyboardEvent(&event.key);
                break;
            }

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            {
                HandleMouseButtonEvent(&event.button);
                break;
            }

            case SDL_MOUSEMOTION:
            {
                HandleMouseMotionEvent(&event.motion);
                break;
            }

            case SDL_MOUSEWHEEL:
            {
                HandleMouseWheelEvent(&event.wheel);
                break;
            }

            case SDL_WINDOWEVENT:
            {
                HandleWindowEvent(&event.window);
                break;
            }

            case SDL_CONTROLLERDEVICEADDED:
            {
                HandleControllerAdded(&event.cdevice);
                break;
            }

            case SDL_CONTROLLERDEVICEREMOVED:
            {
                HandleControllerRemoved(&event.cdevice);
                break;
            }

            case SDL_CONTROLLERAXISMOTION:
            {
                HandleControllerAxisMotion(&event.caxis);
                break;
            }

            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
            {
                HandleControllerButtonDown(&event.cbutton);
                break;
            }


            // TODO: handle other event types...
        }
    }
}

void SDLApplication::HandleWindowEvent( SDL_WindowEvent* event )
{
    switch( event->event )
    {

        case SDL_WINDOWEVENT_FOCUS_GAINED:
        {
            mMainWindow->FocusInEvent();
            break;
        }

        case SDL_WINDOWEVENT_FOCUS_LOST:
        {
            mMainWindow->FocusOutEvent();
            break;
        }

        case SDL_WINDOWEVENT_SIZE_CHANGED:
        {
            mMainWindow->ResizeEvent( event->data1, event->data2 );
            break;
        }

        case SDL_WINDOWEVENT_SHOWN:
        {
            mMainWindow->ShowEvent();
            break;
        }

        case SDL_WINDOWEVENT_HIDDEN:
        {
            mMainWindow->HideEvent();
            break;
        }

        case SDL_WINDOWEVENT_ENTER:
        {
            mMainWindow->EnterEvent();
            break;
        }

        case SDL_WINDOWEVENT_LEAVE:
        {
            mMainWindow->LeaveEvent();
            break;
        }

        case SDL_WINDOWEVENT_MOVED:
        {
            mMainWindow->MoveEvent( event->data1, event->data2 );
            break;
        }

        case SDL_WINDOWEVENT_MINIMIZED:
        {
            mMainWindow->MinimiseEvent();
            break;
        }

        case SDL_WINDOWEVENT_MAXIMIZED:
        {
            mMainWindow->MaximiseEvent();
            break;
        }

        case SDL_WINDOWEVENT_RESTORED:
        {
            mMainWindow->RestoreEvent();
            break;
        }

        default: {}

    }

}

void SDLApplication::HandleKeyboardEvent( SDL_KeyboardEvent* event )
{
    switch( event->type )
    {
        case SDL_KEYDOWN:
        {
            mMainWindow->KeyDownEvent( event );
            break;
        }

        case SDL_KEYUP:
        {
            mMainWindow->KeyUpEvent( event );
            break;
        }

        default: {}
    }
}

void SDLApplication::HandleMouseButtonEvent( SDL_MouseButtonEvent* event )
{
    switch( event->type )
    {
        case SDL_MOUSEBUTTONDOWN:
        {
            mMainWindow->MouseButtonDownEvent( event );
            break;
        }

        case SDL_MOUSEBUTTONUP:
        {
            mMainWindow->MouseButtonUpEvent( event );
            break;
        }

        default: {}
    }
}

void SDLApplication::HandleMouseMotionEvent( SDL_MouseMotionEvent* event )
{
    mMainWindow->MouseMoveEvent( event );
}

void SDLApplication::HandleMouseWheelEvent( SDL_MouseWheelEvent* event )
{
    mMainWindow->MouseWheelEvent( event );
}

void SDLApplication::HandleControllerAdded( SDL_ControllerDeviceEvent* event )
{
    int controllerIndex = event->which;
    if( !SDL_IsGameController( controllerIndex ) )
        return;

    SDL_GameController* controller = SDL_GameControllerOpen( controllerIndex );
    if( !controller )
    {
        SDL_Log( "Failed to open controller: %s", SDL_GetError() );
        return;
    }

    SDL_Joystick* joystick = SDL_GameControllerGetJoystick( controller );
    if( !joystick )
    {
        SDL_Log( "Unable to retrieve the joystick object for the controller" );
        return;
    }

    int controllerID = SDL_JoystickInstanceID( joystick );
    mControllerIdToControllerMap.insert( std::make_pair( controllerID, controller ) );

    mMainWindow->ControllerAddedEvent( controllerID, event );

    SDL_Log( "Controller detected... ID: %i", controllerID );
}

void SDLApplication::HandleControllerRemoved( SDL_ControllerDeviceEvent* event )
{
    int controllerID = event->which;

    auto controllerIterator = mControllerIdToControllerMap.find( controllerID );

    if( controllerIterator == mControllerIdToControllerMap.end() )
        return;

    SDL_GameController* controller = controllerIterator->second;
    SDL_GameControllerClose( controller );

    mMainWindow->ControllerRemovedEvent( controllerID, event );

    SDL_Log( "Controller disconnected... ID: %i", controllerID );
}

void SDLApplication::HandleControllerAxisMotion( SDL_ControllerAxisEvent* event )
{
    mMainWindow->ControllerAxisMotionEvent( event->which, event );
}

void SDLApplication::HandleControllerButtonDown( SDL_ControllerButtonEvent* event )
{
    switch( event->type )
    {
        case SDL_CONTROLLERBUTTONDOWN:
        {
            mMainWindow->ControllerButtonDownEvent( event->which, event );
            break;
        }

        case SDL_CONTROLLERBUTTONUP:
        {
            mMainWindow->ControllerButtonUpEvent( event->which, event );
            break;
        }

        default: {}
    }
}
