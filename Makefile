all:build_corn

build_corn:
	#export LD_LIBRARY_PATH="/home/mdreamer/SpaceCorn/SDL2-2.0.4/build/.libs"
	#export LD_RUN_PATH="/home/mdreamer/SpaceCorn/SDL2-2.0.4/build/.libs"
	#~/rpi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-g++ -Wl,-rpath -Wl,LIBDIR -I./SDL2-2.0.4/include/ -I./SDL2_mixer-2.0.1 -L/home/mdreamer/SpaceCorn/SDL2-2.0.4/build/.libs -L/home/mdreamer/SpaceCorn/SDL2_mixer-2.0.1/build/.libs/ ws2812-rpi/ws2812-rpi.cpp mcp3008spi/mcp3008Spi.cpp spacecorn.cpp -o ./build/spacecorn -lrt -lpthread `sdl2-config --libs` -lSDL2 -lSDL2_mixer
	rsync -rtvz /home/mdreamer/SpaceCorn/ pi@192.168.1.16:~/SpaceCorn

clean :
	rm ./build/*



