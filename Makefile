all:build_corn

build_corn:
	~/rpi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-g++ mcp3008Spi.cpp mcp3008SpiTest.cpp -o ./build/mcpTest

clean :
	rm ./build/*



