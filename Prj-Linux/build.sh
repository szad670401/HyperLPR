
mkdir "build"
# shellcheck disable=SC2164
cd "build"

cmake ..
make -j

ln -s ../hyperlpr3/lib/lib* .

