#
# Copyright (C) 2022 Robert Gill
#

# Set delay for flipping a character (in seconds).
DELAY =

all:
	$(MAKE) -C c
	$(MAKE) -C rust

run:
	$(MAKE) -C rust run

clean:
	$(MAKE) -C c clean
	$(MAKE) -C rust clean

.PHONY: all clean run
