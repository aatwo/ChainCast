#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_


class CircularBuffer {

    public:

                                                CircularBuffer( int size );
                                                ~CircularBuffer();

        void                                    AddValue( double value );
        double                                  GetAverage();


    private:

        double*                                 mData           = nullptr;
        int                                     mNextIndex      = 0;
        int                                     mSize           = 0;
        int                                     mCurrentSize    = 0;


}; // CircularBuffer


#endif // _CIRCULAR_BUFFER_H_
