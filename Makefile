all:build_corn

build_corn:
	#export LD_LIBRARY_PATH="/media/mdreamer/worksapce/SpaceCorn/SDL2-2.0.4/build/.libs"
	#export LD_RUN_PATH="/media/mdreamer/workspace/SpaceCorn/SDL2-2.0.4/build/.libs"
	#/media/mdreamer/workspace/rpi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-g++ -Wl,-rpath -Wl,LIBDIR -I /media/mdreamer/workspace/firmware/hardfp/opt/vc/lib-I./SDL2-2.0.4/include/ -I./SDL2_mixer-2.0.1 -L/media/mdreamer/workspace/SpaceCorn/SDL2-2.0.4/build/.libs -L /media/mdreamer/workspace/firmware/hardfp/opt/vc/lib -L/media/mdreamer/workspace/SpaceCorn/SDL2_mixer-2.0.1/build/.libs/ ws2812-rpi/ws2812-rpi.cpp mcp3008spi/mcp3008Spi.cpp spacecorn.cpp -o ./build/spacecorn -lrt -lpthread `sdl2-config --libs` -lSDL2 -lSDL2_mixer
	#g++ -Wl,-rpath -Wl,LIBDIR -I./SDL2-2.0.4/include/ -I./SDL2_mixer-2.0.1 -L./SDL2-2.0.4/build/.libs -L./SDL2_mixer-2.0.1/build/.libs/ spacecorn.cpp -o ./build/spacecorn -lrt -lpthread `sdl2-config --libs` -lSDL2 -lSDL2_mixer -lbcm2835
	# tryout with LEDs g++ -Wl,-rpath -Wl,LIBDIR -I./SDL2-2.0.4/include/ -I./SDL2_mixer-2.0.1 -L./SDL2-2.0.4/build/.libs -L./SDL2_mixer-2.0.1/build/.libs/ ws2812-rpi/ws2812-rpi.cpp spacecorn.cpp -o ./build/spacecorn -lrt -lpthread `sdl2-config --libs` -lSDL2 -lSDL2_mixer -lbcm2835
	
	#up to date - 29/06/2017
	#g++ -Wl,-rpath -Wl,LIBDIR -I./SDL2-2.0.4/include/ -I./SDL2_mixer-2.0.1 -L./SDL2-2.0.4/build/.libs -L./SDL2_mixer-2.0.1/build/.libs/ spacecorn.cpp SPI_bus.cpp  RS-232/rs232.c -o ./build/spacecorn -lrt -lpthread `sdl2-config --libs` -lSDL2 -lSDL2_mixer -lbcm2835

	#run the file as root!
	
	rsync -rtvz /media/mdreamer/workspace/SpaceCorn pi@raspberrypi:~/
	#rsync -rtvz /media/mdreamer/workspace/SpaceCorn pi@169.254.41.1:~/
	#rsync -rtvz /media/mdreamer/workspace/SpaceCorn pi@192.168.199.90:~/

clean :
	rm ./build/*



