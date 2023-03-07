
BUILD_DIR=build/linux
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARE=ON -DBUILD_SAMPLES=OFF -DBUILD_TEST=OFF ../..
make -j4
make install

