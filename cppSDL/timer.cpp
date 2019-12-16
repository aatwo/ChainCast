#include "timer.h"


Timer::Timer() {

	mPerformanceFrequency = SDL_GetPerformanceFrequency();
    Start();
}

void Timer::Start() {

	mRunning 	= true;
	mStart 		= SDL_GetPerformanceCounter();
	mEnd 		= mStart;
}

void Timer::Stop() {

	mRunning 	= false;
	mEnd 		= SDL_GetPerformanceCounter();

}

void Timer::Restart()
{
	Start();
}

double Timer::ElapsedS()
{
	if( mRunning )
		mEnd = SDL_GetPerformanceCounter();

	unsigned long long elapsedTicks = mEnd - mStart;
	double elapsedS = static_cast<double>( elapsedTicks ) / static_cast<double>( mPerformanceFrequency );
	return elapsedS;
}

unsigned long long Timer::ElapsedMs() {

	if( mRunning )	
		mEnd = SDL_GetPerformanceCounter();

	unsigned long long elapsedTicks = mEnd - mStart;
	double elapsedS = static_cast<double>( elapsedTicks ) / static_cast<double>( mPerformanceFrequency );
	return static_cast<unsigned long long>( elapsedS * 1000.0 );
}