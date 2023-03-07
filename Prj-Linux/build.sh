
mkdir "build"
cd "build"

cmake ..
make -j

ln -s ../hyperlpr3/lib/lib* .

