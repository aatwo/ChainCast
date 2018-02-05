#ifndef _EVENT_MACROS_H_
#define _EVENT_MACROS_H_


#define REGISTER_EVENT( x ) x,
enum class EventType
{
	#include "eventdefs.h"
	count
};
#undef REGISTER_EVENT


#define REGISTER_EVENT( x ) #x,
static const char* EventNames[] =
{
	#include "eventdefs.h"
	"invalid"
};
#undef REGISTER_EVENT


// const char* GetEventName( EventType eventType )
// {
// 	return EventNames[ static_cast<int>( eventType ) ];
// }


#endif // _EVENT_MACROS_H_
