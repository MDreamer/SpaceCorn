Libraries have been installed in:
   /usr/local/lib

If you ever happen to want to link against installed libraries
in a given directory, LIBDIR, you must either use libtool, and
specify the full pathname of the library, or use the `-LLIBDIR'
flag during linking and do at least one of the following:
   - add LIBDIR to the `LD_LIBRARY_PATH' environment variable
     during execution
   - add LIBDIR to the `LD_RUN_PATH' environment variable
     during linking
   - use the `-Wl,-rpath -Wl,LIBDIR' linker flag
   - have your system administrator add LIBDIR to `/etc/ld.so.conf'

See any operating system documentation about shared libraries for
more information, such as the ld(1) and ld.so(8) manual pages.
----------------------------------------------------------------------

http://www.gamedev.net/topic/646010-sdl2-mixer-no-such-audio-device-solved/

g++ -Wl,-rpath -Wl,LIBDIR test1.c -o test1 `sdl2-config --libs` -lSDL2 -lSDL2_mixer

export LD_LIBRARY_PATH="$HOME/Desktop/SDL2-2.0.3/install/lib"
export LD_RUN_PATH="$HOME/Desktop/SDL2-2.0.3/install/lib"

export LD_LIBRARY_PATH="/home/mdreamer/SpaceCorn/SDL2-2.0.4/build/.libs"
export LD_RUN_PATH="/home/mdreamer/SpaceCorn/SDL2-2.0.4/build/.libs"

ALSA lib pcm.c:7843:(snd_pcm_recover) underrun occurred - solution: aconnect -x 

g++ -Wl,-rpath -Wl,LIBDIR -I./SDL2-2.0.4/include/ -I./SDL2_mixer-2.0.1 -L./SDL2-2.0.4/build/.libs -L./SDL2_mixer-2.0.1/build/.libs/ spacecorn.cpp -o ./build/spacecorn -lrt -lpthread `sdl2-config --libs` -lSDL2 -lSDL2_mixer -lbcm2835
