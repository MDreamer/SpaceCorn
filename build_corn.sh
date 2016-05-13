echo "startign building the Space Corn..."
echo g++ -std=c++0x -pthread  -o mcp3008 mcp3008SpiTest.cpp mcp3008Spi.cpp
g++ -std=c++0x -Wall -o mcp3008 mcp3008SpiTest.cpp -lpthread mcp3008Spi.cpp -lrt

