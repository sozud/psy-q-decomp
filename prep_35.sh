mkdir -p build/3.5
# put toolchain together
cp -r ./psy-q/3.3/PSYQ/* build/3.5

wget -O https://github.com/Xeeynamo/sotn-decomp/releases/download/cc1-psx-26/cc1-psx-26.tar.gz
tar -xvf cc1-psx-26.tar.gz -C build/3.5
cp -r ./psy-q/cc_2.60/* build/3.5
# replace aspsx with newer one
cp psy-q/3.5/PSYQ/ASPSX.EXE build/3.5
