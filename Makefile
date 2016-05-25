all:build_corn

build_corn:
	~/rpi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-g++ ws2812-rpi/ws2812-rpi.cpp mcp3008spi/mcp3008Spi.cpp spacecorn.cpp -o ./build/spacecorn -lrt

clean :
	rm ./build/*



