SND_SRCS := $(wildcard src/snd/*.c)
SND_OBJS := $(patsubst src/snd/%.c, build/snd/%.obj, $(SND_SRCS))
SRC_DIR := src/snd

SPU_SRCS := $(wildcard src/spu/*.c)
SPU_OBJS := $(patsubst src/spu/%.c, build/spu/%.obj, $(SPU_SRCS))

ALL_SRCS := $(SND_SRCS) $(SPU_SRCS)
ALL_OBJS := $(SND_OBJS) $(SPU_OBJS)

all: $(ALL_OBJS)

clean:
	rm -f $(ALL_OBJS)

prep:
	mkdir -p build
	# put toolchain together
	cp -r ./psy-q/3.3/PSYQ/* build
	cp -r ./psy-q/cc_2.60/* build
	# replace aspsx with newer one
	cp psy-q/3.5/PSYQ/ASPSX.EXE build
	# build splitter
	cargo build --release --manifest-path ./tools/psy-q-splitter/splitter/Cargo.toml

format:
	clang-format -i $$(find src/snd -type f -name "*.c")
	clang-format -i $$(find src/snd -type f -name "*.h")
	clang-format -i $$(find src/spu -type f -name "*.c")
	clang-format -i $$(find src/spu -type f -name "*.h")

build/snd/%.obj: src/snd/%.c
	# make build directory if it doesn't exist
	@mkdir -p $(@D)
	@echo "Building $@ from $<"
	# subst since we need dos slashes for this to work
	dosemu -quiet -dumb -f ./dosemurc -K . -E "build.bat $(subst /,\,$<) $(subst /,\,$@)"
	# check it
	./tools/psy-q-splitter/splitter/target/release/splitter diff_obj_with_lib ./psy-q/3.5/PSX/LIB/LIBSND.LIB $@

build/spu/%.obj: src/spu/%.c
	# make build directory if it doesn't exist
	@mkdir -p $(@D)
	@echo "Building $@ from $<"
	# subst since we need dos slashes for this to work
	dosemu -quiet -dumb -f ./dosemurc -K . -E "build.bat $(subst /,\,$<) $(subst /,\,$@)"
	# check it
	./tools/psy-q-splitter/splitter/target/release/splitter diff_obj_with_lib ./psy-q/3.5/PSX/LIB/LIBSPU.LIB $@

progress:
	./tools/psy-q-splitter/splitter/target/release/splitter progress ./psy-q/3.5/PSX/LIB/LIBSND.LIB ./build/snd
	./tools/psy-q-splitter/splitter/target/release/splitter progress ./psy-q/3.5/PSX/LIB/LIBSPU.LIB ./build/spu
