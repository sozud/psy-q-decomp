SND_SRCS := $(wildcard src/snd/*.c)
OBJS := $(patsubst src/snd/%.c, build/%.obj, $(SND_SRCS))

all: $(OBJS)

clean:
	rm -f $(OBJS)

prep:
	# put toolchain together
	cp -r ./psy-q/3.3/PSYQ/* build
	cp -r ./psy-q/cc_2.60/* build
	# replace aspsx with newer one
	cp psy-q/3.5/PSYQ/ASPSX.EXE build
	# build splitter
	cargo run --release --manifest-path ./tools/psy-q-splitter/splitter/Cargo.toml

build/%.obj: src/snd/%.c
	# make build directory if it doesn't exist
	@mkdir -p $(@D)
	@echo "Building $@ from $<"
	# subst since we need dos slashes for this to work
	dosemu -quiet -dumb -f ./dosemurc -K . -E "build.bat $(subst /,\,$<) $(subst /,\,$@)"
	# check it
	./tools/psy-q-splitter/splitter/target/release/splitter diff_obj_with_lib ./psy-q/3.5/PSX/LIB/LIBSND.LIB $@
