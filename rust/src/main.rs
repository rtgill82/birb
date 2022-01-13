//
// Copyright (C) 2022 Robert Gill
//

#[macro_use]
extern crate lazy_static;

use std::{env,io,str,thread,time};
use std::io::Write;
use std::mem::MaybeUninit;
use std::sync::Once;

use std::sync::Arc;
use std::sync::atomic::AtomicBool;
use std::sync::atomic::Ordering;

use rand::Rng;
use rand::rngs::ThreadRng;

mod args;
use crate::args::Args;

// 1 second defined as microseconds.
const SECOND: f32 = 1000000.0;

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

// Function pointer for character mutation.
type Func = fn(ch: u8) -> u8;

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

    let mut birb: [u8; 5] = [B, I, R, D, RET];
    let args = parse_args();
    let sec = time::Duration::from_micros(args.delay);

    let mut mutate: Func = rotate_ch;
    if args.random {
        mutate = flip_ch;
    }

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
        birb[ch] = mutate(birb[ch]);
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

// Display help message.
fn show_help() {
    println!("USAGE: birb [-hr?] [--help] [--random] [DELAY]\n");
    println!("  -h, -?, --help\tDisplay this help message");
    println!("  -r, --random\t\trandomly mutate characters\n");
}

// Parse command line arguments.
fn parse_args() -> Args {
    let mut count = 0;
    let mut args: Args = Default::default();

    for (i, arg) in env::args().enumerate() {
        if i < 1 { continue };

        if &arg[0..1] == "-" {
            if &arg[1..2] == "-" {
                // Handle long options.
                if "random" == &arg[2..] {
                    args.random = true;
                } else if "help" == &arg[2..] {
                    show_help();
                    std::process::exit(libc::EXIT_SUCCESS);
                } else {
                    show_help();
                    std::process::exit(libc::EXIT_FAILURE);
                }
            } else {
                // Handle short options.
                match &arg[1..2] {
                    "?" | "h" => {
                        show_help();
                        std::process::exit(libc::EXIT_SUCCESS);
                    },

                    "r" => args.random = true,

                    _ => {
                        show_help();
                        std::process::exit(libc::EXIT_FAILURE);
                    }
                }
            }
        } else {
            if count >= 1 {
                show_help();
                std::process::exit(libc::EXIT_FAILURE);
            }

            count += 1;
            args.delay = parse_delay(&arg);
        }
    }

    args
}

// Parse delay.
fn parse_delay(s: &str) -> u64 {
    let result = s.parse::<f32>();
    match result {
        Ok(delay) => {
            if delay <= 0.0 {
                eprintln!("DELAY cannot be zero or negative");
                std::process::exit(libc::EXIT_FAILURE);
            }

            if delay > 60.0 {
                eprintln!("DELAY cannot be greater than 60 seconds");
                std::process::exit(libc::EXIT_FAILURE);
            }

            return (SECOND * delay).round() as u64;
        }

        Err(err) => {
            eprintln!("ParseFloatError: {}", err);
            std::process::exit(libc::EXIT_FAILURE);
        }
    }
}

// Select next character in a sequence.
fn rotate_ch(ch: u8) -> u8
{
    match ch {
        B => Q,
        D => B,
        P => D,
        Q => P,
        _ => panic!()
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
