rm -rf build
mkdir -p build
cp -r ../psy-q/PSYQ/* build
cp -r ../psy-q/PSX/INCLUDE/* build
cp -r ../psy-q/PSX/LIB/* build
cp -r ../psy-q/PSX/BIN/* build
cp ../dosemurc ./build
cp ./main.c build

unix2dos -n ./link.txt build/link.txt

export MYDIR=$(pwd)/build

cd $MYDIR && dosemu -quiet -dumb -f ./dosemurc -K . -E "ccpsx.exe -O2 -g -c -I. main.c -omain.obj"
cd $MYDIR && dosemu -quiet -dumb -f ./dosemurc -K . -E "psylink.exe /m /c /g @link.txt,main.cpe,main.sym,main.map"
cd $MYDIR && dosemu -quiet -dumb -f ./dosemurc -K . -E "cpe2x.exe main.cpe"
