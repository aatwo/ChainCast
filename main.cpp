#include <iostream>
#include "sdlapplication.h"
#include <time.h>
#include <chrono>

#undef main

int main()
{
    SDLApplication application;

    unsigned int timeSeed = static_cast<unsigned int>( std::time( nullptr ) );
    std::srand( timeSeed );

    if( !application.Init() )
    {
        std::cout << "Initialisation failed" << std::endl;
        return -1;
    }

    return application.Exec();
}
