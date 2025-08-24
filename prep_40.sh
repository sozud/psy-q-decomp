mkdir -p build/4.0
curl -sSfL -o build/4.0/wibo https://github.com/decompals/wibo/releases/download/0.6.13/wibo
chmod +x build/4.0/wibo
# put toolchain together
cp -r ./psy-q/4.0/PSYQ/* build/4.0
cp -r ./psy-q/4.0/COMPILER/* build/4.0
