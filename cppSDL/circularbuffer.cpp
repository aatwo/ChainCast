#include "circularbuffer.h"


CircularBuffer::CircularBuffer( int size )
{
    mSize = size;
    mData = new double[ size ]{};
}

CircularBuffer::~CircularBuffer()
{
    delete [] mData;
}

void CircularBuffer::AddValue( double value )
{
    mData[ mNextIndex++ ] = value;

    if( mNextIndex >= mSize )
        mNextIndex = 0;

    if( mCurrentSize < mSize )
        mCurrentSize++;
}

double CircularBuffer::GetAverage()
{
    if( !mSize || !mCurrentSize )
        return 0.0;

    double total = 0.0;
    for( int i = 0; i < mCurrentSize; i++ )
        total += mData[ i ];

    return total / static_cast<double>( mCurrentSize );
}
