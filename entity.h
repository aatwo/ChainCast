#ifndef _ENTITY_H_
#define _ENTITY_H_


#include <SDL2/SDL.h>
#include <vector>
#include "event.h"


class Entity
{
	public:

		enum class Type
		{
			player,
			enemy,
			projectile,
			wall,
			destructable_wall,

			unknown
		};


												Entity( double topLeftX, double topLeftY, double width, double height );

		void									GetRect( double& x_OUT, double& y_OUT, double& width_OUT, double& height_OUT );
		void									GetCenter( double& x_OUT, double& y_OUT );
		void									TranslateX( double deltaX );
		void									TranslateY( double deltaY );
		void 									GetPendingEvents( std::vector<Event*>& pendingEvents_OUT );

		virtual void							Update( double elapsedS, unsigned long long elapsedMs, std::vector<Entity*>& entities ) = 0;
		virtual	void							Render() = 0;
		virtual Type							GetType() { return Type::unknown; }


	protected:

		std::vector<Event*>						mPendingEvents;

		double 									mTopLeftX	= 0.0;
		double									mTopLeftY	= 0.0;
		double 									mWidth		= 0.0;
		double									mHeight		= 0.0;


}; // Entity


#endif // _ENTITY_H_