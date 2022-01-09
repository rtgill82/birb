#
# Copyright (C) 2022 Robert Gill
#

RELEASE = target/release/birb
DEBUG = target/debug/birb

birb: $(RELEASE)

debug: $(DEBUG)

run: clean
	cargo run --release

$(RELEASE): src/main.rs
	cargo build --release

$(DEBUG): src/main.rs
	cargo build

clean:
	-cargo clean

.PHONY: birb clean debug run
