#
# Copyright (C) 2022 Robert Gill
#

# Set delay for flipping a character (in seconds).
DELAY =

RELEASE = target/release/birb
DEBUG = target/debug/birb

birb: $(RELEASE)

debug: $(DEBUG)

run: $(RELEASE)
	cargo run --release -- $(DELAY)

$(RELEASE): src/main.rs
	cargo build --release

$(DEBUG): src/main.rs
	cargo build

clean:
	-cargo clean

.PHONY: birb clean debug run
