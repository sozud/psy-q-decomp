rm -rf build
mkdir -p build

cp dosemurc build

# headers
cp -r ./psy-q/3.5/PSX/INCLUDE/* build

cp -r ./psy-q/3.3/PSYQ/* build
cp -r ./psy-q/cc_2.60/* build

# replace aspsx with newer one
cp psy-q/3.5/PSYQ/ASPSX.EXE build

# copy sources 
cp src/snd/* build

export HOME_DIR=$(pwd)

export BUILD_DIR=$(pwd)/build

# build
cd $BUILD_DIR && dosemu -quiet -dumb -f ./dosemurc -K . -E "ccpsx.exe -v -O2 -g0 -G0 -funsigned-char -c -I. next.c -onext.obj"

# check
cd $HOME_DIR && cargo run --release --manifest-path ./tools/psy-q-splitter/splitter/Cargo.toml diff_obj_with_lib ./psy-q/3.5/PSX/LIB/LIBSND.LIB NEXT ./build/next.obj
