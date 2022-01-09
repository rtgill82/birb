//
// Copyright (C) 2022 Robert Gill
//

#[macro_use]
extern crate lazy_static;

use std::{io,str,thread,time};
use std::io::Write;
use std::mem::MaybeUninit;
use std::sync::Once;

use std::sync::Arc;
use std::sync::atomic::AtomicBool;
use std::sync::atomic::Ordering;

use rand::Rng;
use rand::rngs::ThreadRng;

// Bird is the word.
const B: u8 = 'b' as u8;
const I: u8 = 'i' as u8;
const R: u8 = 'r' as u8;
const D: u8 = 'd' as u8;
const RET: u8 = '\r' as u8;

// Mirrored characters.
const P: u8 = 'p' as u8;
const Q: u8 = 'q' as u8;

// Indices of characters we'll be flipping.
const CH_IDX: [usize; 2] = [0, 3];

static INIT: Once = Once::new();
static mut RNG: MaybeUninit<ThreadRng> = MaybeUninit::uninit();

lazy_static! {
    static ref RUN: Arc<AtomicBool> = Arc::new(AtomicBool::new(true));
}

// Randomly choose one of two characters provided.
macro_rules! rand_choice {
    ($a:ident, $b:ident) => {
        if rand1() == 0 { $a } else { $b }
    }
}

fn main() {
    unsafe {
        INIT.call_once(|| {
            RNG = MaybeUninit::new(rand::thread_rng());
        });
    }

    let sec = time::Duration::new(1, 0);
    let mut birb: [u8; 5] = [B, I, R, D, RET];

    let run = (*RUN).clone();
    ctrlc::set_handler(move || {
        run.swap(false, Ordering::Relaxed);
    }).unwrap();

    // Print 'bird'.
    print(&birb);
    thread::sleep(sec);

    // Then print 'birb'.
    birb[3] = B;
    print(&birb);

    // Randomize.
    while (*RUN).load(Ordering::Relaxed) {
        thread::sleep(sec);
        let ch = CH_IDX[rand1()];
        birb[ch] = flip_ch(birb[ch]);
        print(&birb);
    }

    println!("\nbirb!");
}

// Print a `u8` array as an ASCII string.
fn print(ascii: &[u8]) {
    let s = str::from_utf8(ascii).unwrap();
    print!("{}", s);
    io::stdout().flush().unwrap();
}

// Return either 0 or 1 randomly.
pub fn rand1() -> usize {
    unsafe {
        let rng = RNG.as_mut_ptr().as_mut().unwrap();
        rng.gen_range(0..2)
    }
}

// Randomly choose new character based on current character.
fn flip_ch(ch: u8) -> u8 {
    match ch {
        B => rand_choice!(D, Q),
        D => rand_choice!(B, P),
        P => rand_choice!(D, Q),
        Q => rand_choice!(B, P),
        _ => panic!()
    }
}
