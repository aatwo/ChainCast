#include "event.h"
#include <cassert>


Event::Event( EventType type )
{
	mType = type;
}

void Event::AddValue( EventParameterType parameterType, Variant variant )
{
	mValues.insert( std::pair<EventParameterType, Variant>( parameterType, variant ) );
}

bool Event::GetValue( EventParameterType parameterType, Variant& variant_OUT )
{
	auto it = mValues.find( parameterType );

	if( it == mValues.end() )
	{
		variant_OUT = Variant();
		return false;
	}

	variant_OUT = it->second;
	return true;
}

int Event::GetValueInt( EventParameterType parameterType )
{
	Variant variant;
	bool gotValue = GetValue( parameterType, variant );
	
	assert( gotValue );
	assert( variant.GetType() == Variant::Type::int_type );

	return variant.ToInt();
}

double Event::GetValueDouble( EventParameterType parameterType )
{
	Variant variant;
	bool gotValue = GetValue( parameterType, variant );

	assert( gotValue );
	assert( variant.GetType() == Variant::Type::double_type );

	return variant.ToDouble();
}

std::string Event::GetValueString( EventParameterType parameterType )
{
	Variant variant;
	bool gotValue = GetValue( parameterType, variant );
	
	assert( gotValue );
	assert( variant.GetType() == Variant::Type::string_type );

	return variant.ToString();
}

EventType Event::GetType()
{
	return mType;
}
