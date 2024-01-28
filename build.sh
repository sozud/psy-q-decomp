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
cd $BUILD_DIR && dosemu -quiet -dumb -f ./dosemurc -K . -E "ccpsx.exe -v -O2 -g0 -G0 -funsigned-char -c -I. play.c -oplay.obj"
cd $BUILD_DIR && dosemu -quiet -dumb -f ./dosemurc -K . -E "ccpsx.exe -v -O2 -g0 -G0 -funsigned-char -c -I. scsmvol.c -oscsmvol.obj"
cd $BUILD_DIR && dosemu -quiet -dumb -f ./dosemurc -K . -E "ccpsx.exe -v -O2 -g0 -G0 -funsigned-char -c -I. scssattr.c -oscssattr.obj"
cd $BUILD_DIR && dosemu -quiet -dumb -f ./dosemurc -K . -E "ccpsx.exe -v -O2 -g0 -G0 -funsigned-char -c -I. scssvol.c -oscssvol.obj"
cd $BUILD_DIR && dosemu -quiet -dumb -f ./dosemurc -K . -E "ccpsx.exe -v -O2 -g0 -G0 -funsigned-char -c -I. sscall.c -osscall.obj"

# build splitter
cd $HOME_DIR && cargo run --release --manifest-path ./tools/psy-q-splitter/splitter/Cargo.toml

# check
cd $HOME_DIR && ./tools/psy-q-splitter/splitter/target/release/splitter diff_obj_with_lib ./psy-q/3.5/PSX/LIB/LIBSND.LIB NEXT ./build/next.obj &&
cd $HOME_DIR && ./tools/psy-q-splitter/splitter/target/release/splitter diff_obj_with_lib ./psy-q/3.5/PSX/LIB/LIBSND.LIB PLAY ./build/play.obj &&
cd $HOME_DIR && ./tools/psy-q-splitter/splitter/target/release/splitter diff_obj_with_lib ./psy-q/3.5/PSX/LIB/LIBSND.LIB SCSMVOL ./build/scsmvol.obj &&
cd $HOME_DIR && ./tools/psy-q-splitter/splitter/target/release/splitter diff_obj_with_lib ./psy-q/3.5/PSX/LIB/LIBSND.LIB SCSSATTR ./build/scssattr.obj &&
cd $HOME_DIR && ./tools/psy-q-splitter/splitter/target/release/splitter diff_obj_with_lib ./psy-q/3.5/PSX/LIB/LIBSND.LIB SCSSVOL ./build/scssvol.obj &&
cd $HOME_DIR && ./tools/psy-q-splitter/splitter/target/release/splitter diff_obj_with_lib ./psy-q/3.5/PSX/LIB/LIBSND.LIB SSCALL ./build/sscall.obj
