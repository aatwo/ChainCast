#ifndef _VARIANT_H_
#define _VARIANT_H_


#include "string"


class Variant
{
	public:

		enum class Type 
		{
			int_type,
			double_type,
			string_type,

			invalid_type
		};

												Variant();
												Variant( int value );
												Variant( double value );
												Variant( std::string value );

		Type 									GetType();
		int 									ToInt();
		double 									ToDouble();
		std::string								ToString();


	private:

		Type 									mType	= Type::invalid_type;
		int 									mValueInt;
		double 									mValueDouble;
		std::string 							mValueString;


}; // Variant


#endif // _VARIANT_H_
