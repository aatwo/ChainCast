# ChainCast

<b>What is ChainCast?</b> This was a small project undetaken as part of a game jam / learning exercise set with the theme 'chain cast'. My implementation is a local multiplayer game setting players in a destructable maze with other players and a single persuing monster. The monster always follows the closest players and it is possible to 'cast chains' at people and / or the monster to trap them momentarily. Once caught by the monster you are out of the game.
  
<b>What tech stack was used?</b> C++ and SDL2

<b>Why would you use such a low level tech stack for a game jam?</b> This was less about the game and more about the desire for me to work more with raw C++, SDL2, Makefiles and Visual Studio Codes C++ plugin. You could probably crank out something better than this using Unity in an hour but this was more about learning than anything else.

<b>How do I build and run this?</b> The build for this currently only supports Linux but due to the projects small size it would be easy to get it working on Windows. The only things it links against are SDL2, SDL2_image, SDL2_mixer and SDL2_ttf. On debian you can install the SDL2 dependencies using the following command:
sudo apt-get install libsdl2-dev libsdl2-gfx-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
