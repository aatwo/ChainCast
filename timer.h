#ifndef _TIMER_H_
#define _TIMER_H_


#include <SDL2/SDL.h>


class Timer
{
	public:

												Timer();

		void									Start();
		void									Stop();
		void									Restart();
		double									ElapsedS();
		unsigned long long 						ElapsedMs();


	private:

		bool 									mRunning 				= true;
		Uint64									mStart 					= 0;
		Uint64									mEnd 					= 0;
		Uint64									mPerformanceFrequency 	= 0;


}; // Timer


#endif // _TIMER_H_