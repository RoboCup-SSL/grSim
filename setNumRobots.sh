echo $1
sed -i '4s/.*/#define ROBOT_COUNT '$1'/' include/config.h
cd build
cmake .. && make
cd ..
./bin/grsim
