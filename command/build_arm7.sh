mkdir build/arm32
cd build/arm32
# export cross_compile_toolchain=/home/s4129/software/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf
 cmake -DCMAKE_SYSTEM_NAME=Linux \
	-DCMAKE_SYSTEM_VERSION=1 \
	-DCMAKE_SYSTEM_PROCESSOR=armv7 \
	-DCMAKE_C_COMPILER=$cross_compile_toolchain/bin/arm-linux-gnueabihf-gcc \
  -DCMAKE_CXX_COMPILER=$cross_compile_toolchain/bin/arm-linux-gnueabihf-g++ \
	-DBUILD_LINUX_ARM7=ON ../../

make -j4
