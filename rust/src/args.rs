//
// Copyright (C) 2022 Robert Gill
//

use crate::SECOND;

pub struct Args {
    pub delay: u64,
    pub random: bool
}

impl Default for Args {
    fn default() -> Self {
        Args {
            delay: SECOND as u64,
            random: false
        }
    }
}
