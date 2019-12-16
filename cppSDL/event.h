#ifndef _EVENT_H_
#define _EVENT_H_


#include <map>
#include "eventmacros.h"
#include "eventparametermacros.h"
#include "variant.h"


class Event
{
	public:

												Event( EventType type );

		void 									AddValue( EventParameterType parameterType, Variant variant );
		bool 									GetValue( EventParameterType parameterType, Variant& variant_OUT );
		int 									GetValueInt( EventParameterType parameterType );
		double 									GetValueDouble( EventParameterType parameterType );
		std::string 							GetValueString( EventParameterType parameterType );
		EventType 								GetType();


	private:

		EventType 								mType;
		std::map<EventParameterType, Variant> 	mValues;		


}; // Event


#endif // _EVENT_H_
