#ifndef _EVENT_PARAMETER_MACROS_H_
#define _EVENT_PARAMETER_MACROS_H_


#define REGISTER_EVENT_PARAMETER( x ) x,
enum class EventParameterType
{
	#include "eventparameterdefs.h"
	count
};
#undef REGISTER_EVENT_PARAMETER


#define REGISTER_EVENT_PARAMETER( x ) #x,
static const char* EventParameterNames[] =
{
	#include "eventparameterdefs.h"
	"invalid"
};
#undef REGISTER_EVENT_PARAMETER


// const char* GetEventParameterName( EventParameterType parameterType )
// {
// 	return EventParameterNames[ static_cast<int>( parameterType ) ];
// }


#endif // _EVENT_PARAMETER_MACROS_H_
