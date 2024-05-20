mkdir -p build/aarch64
cd build/aarch64
# export cross_compile_toolchain=/home/s4129/software/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf
 cmake -DCMAKE_SYSTEM_NAME=Linux \
	-DCMAKE_SYSTEM_VERSION=1 \
	-DCMAKE_SYSTEM_PROCESSOR=aarch64 \
  -DCMAKE_C_COMPILER=$GCC_AARCH64_COMPILER-gcc \
  -DCMAKE_CXX_COMPILER=$GCC_AARCH64_COMPILER-g++ \
	-DBUILD_LINUX_ARM_AARCH64=ON ../../

make -j4