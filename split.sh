export MYDIR=$(pwd)
cd libsnd_linked && sh build.sh && cd $MYDIR && mkdir -p config && python3 ./gen.py && rm -rf asm && rm -rf src && python3 ./splat/split.py --verbose ./splat.yaml 
