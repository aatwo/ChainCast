#include "variant.h"


Variant::Variant()
	:	mType( Type::invalid_type )
{
}

Variant::Variant( int value )
	:	mValueInt( value ),
		mType( Type::int_type )
{
}

Variant::Variant( double value )
	:	mValueDouble( value ),
		mType( Type::double_type )
{
}

Variant::Variant( std::string value )
	:	mValueString( value ),
		mType( Type::string_type )
{
}

Variant::Type Variant::GetType()
{
	return mType;
}

int Variant::ToInt()
{
	if( mType != Type::int_type )
		return 0;

	return mValueInt;
}

double Variant::ToDouble()
{
	if( mType != Type::double_type )
		return 0.0;

	return mValueDouble;
}

std::string	Variant::ToString()
{
	if( mType != Type::string_type )
		return "";

	return mValueString;
}
