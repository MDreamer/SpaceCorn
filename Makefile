all:build_corn

build_corn:
	~/rpi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-g++ ws2812-rpi/ws2812-rpi.cpp mcp3008spi/mcp3008Spi.cpp jsoncpp.cpp spacecorn.cpp -o ./build/spacecorn -lrt 
	rsync -rtvz /home/mdreamer/SpaceCorn/ pi@192.168.1.16:~/SpaceCorn

clean :
	rm ./build/*



