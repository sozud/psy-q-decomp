	mkdir -p build_35
	# put toolchain together
	cp -r ./psy-q/3.3/PSYQ/* build_35
	cp -r ./psy-q/cc_2.60/* build_35
	# replace aspsx with newer one
	cp psy-q/3.5/PSYQ/ASPSX.EXE build_35
    