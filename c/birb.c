/*
 * Randomly flip characters in the word 'bird'.
 * Copyright (C) 2022 Robert Gill
 *
 */

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define RAND1 (rand() % 2)
#define RAND_CHOICE(a, b) (RAND1 ? (a) : (b))

static const int chidx[] = {0, 3};

static struct sigaction action;
static char birb[] = "bird\r";
static volatile sig_atomic_t run = true;

static void handler(int signal);
static char flipch(char ch);

int main(int argc, char *argv[])
{
  int ch;

  action.sa_handler = handler;
  if (sigaction(SIGINT, &action, NULL)) {
      perror("sigaction");
      exit(EXIT_FAILURE);
  }

  srand(time(NULL));
  printf("%s", birb);
  fflush(stdout);
  sleep(1);

  /* Print 'birb' first. */
  birb[3] = 'b';
  printf("%s", birb);
  fflush(stdout);

  while (run) {
      sleep(1);
      ch = chidx[RAND1];
      birb[ch] = flipch(birb[ch]);
      printf("%s", birb);
      fflush(stdout);
  }

  puts("\nbirb!");
  return 0;
}

static void handler(int signal)
{
  run = false;
}

static char flipch(char ch)
{
    switch(ch) {
      case 'b': return RAND_CHOICE('d', 'q');
      case 'd': return RAND_CHOICE('b', 'p');
      case 'p': return RAND_CHOICE('d', 'q');
      case 'q': return RAND_CHOICE('b', 'p');
      default: abort();
    }
}
