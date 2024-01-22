This is an attempt at a standalone PSY-Q 3.5 decompilation, but currently just targets libsnd. The current attempt uses the original 3.5 toolchain and dosemu. The current method is:

Create a binary that contains all of libsnd in it by calling every function: https://github.com/sozud/psy-q-experiments/tree/master/libsnd_linked

Use a script to generate a splat yaml for the resulting exe. This script parses the map file to get all the symbol information and uses psx_psyq_signatures to find the functions. https://github.com/sozud/psy-q-experiments/blob/master/gen.py 

The idea is making it reproducible end to end so that if the executable needs to change, everything else can be regenerated. This is a workaround for the fact that addresses are all 0 in unlinked libraries and objects, making it hard to correlate data and functions.
