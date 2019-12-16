#ifndef SDLEVENTDELEGATE_H
#define SDLEVENTDELEGATE_H


struct SDL_KeyboardEvent;
struct SDL_MouseButtonEvent;
struct SDL_MouseMotionEvent;
struct SDL_MouseWheelEvent;
struct SDL_ControllerDeviceEvent;
struct SDL_ControllerAxisEvent;
struct SDL_ControllerButtonEvent;


class SDLEventDelegate
{
    public:

        // Window events
        virtual void                            ShowEvent() {}
        virtual void                            HideEvent() {}
        virtual void                            FocusInEvent() {}
        virtual void                            FocusOutEvent() {}
        virtual void                            ResizeEvent( int newWidth, int newHeight ) { (void)newWidth; (void)newHeight; }
        virtual void                            EnterEvent() {}
        virtual void                            LeaveEvent() {}
        virtual void                            MoveEvent( int newX, int newY ) { (void)newX; (void)newY; }
        virtual void                            MinimiseEvent() {}
        virtual void                            MaximiseEvent() {}
        virtual void                            RestoreEvent() {}

        // Keyboard events
        virtual void                            KeyDownEvent( SDL_KeyboardEvent* event ) { (void)event; }
        virtual void                            KeyUpEvent( SDL_KeyboardEvent* event ) { (void)event; }

        // Mouse events
        virtual void                            MouseButtonDownEvent( SDL_MouseButtonEvent* event ) { (void)event; }
        virtual void                            MouseButtonUpEvent( SDL_MouseButtonEvent* event ) { (void)event; }
        virtual void                            MouseMoveEvent( SDL_MouseMotionEvent* event ) { (void)event; }
        virtual void                            MouseWheelEvent( SDL_MouseWheelEvent* event ) { (void)event; }

        // Controller events
        virtual void                            ControllerAddedEvent( int controllerID, SDL_ControllerDeviceEvent* event ) { (void)controllerID; (void)event; }
        virtual void                            ControllerRemovedEvent( int controllerID, SDL_ControllerDeviceEvent* event ) { (void)controllerID; (void)event; }
        virtual void                            ControllerAxisMotionEvent( int controllerID, SDL_ControllerAxisEvent* event ) { (void)controllerID; (void)event; }
        virtual void                            ControllerButtonDownEvent( int controllerID, SDL_ControllerButtonEvent* event ) { (void)controllerID; (void)event; }
        virtual void                            ControllerButtonUpEvent( int controllerID, SDL_ControllerButtonEvent* event ) { (void)controllerID; (void)event; }


}; // SDLEventDelegate


#endif // SDLEVENTDELEGATE_H
