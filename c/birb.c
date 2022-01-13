/*
 * Randomly flip characters in the word 'bird'.
 * Copyright (C) 2022 Robert Gill
 *
 */

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define SECOND (1000000)
#define RAND1 (rand() % 2)
#define RAND_CHOICE(a, b) (RAND1 ? (a) : (b))

static const int chidx[] = {0, 3};

static struct sigaction action;
static char birb[] = "bird\r";
static volatile sig_atomic_t run = true;

static void handler(int signal);
static useconds_t parsedelay(const char *value);
static char flipch(char ch);

int main(int argc, char *argv[])
{
  int ch;
  useconds_t delay = SECOND;

  if (argc == 2) {
      delay = parsedelay(argv[1]);
  }

  action.sa_handler = handler;
  if (sigaction(SIGINT, &action, NULL)) {
      perror("sigaction");
      exit(EXIT_FAILURE);
  }

  srand(time(NULL));
  printf("%s", birb);
  fflush(stdout);
  usleep(delay);

  /* Print 'birb' first. */
  birb[3] = 'b';
  printf("%s", birb);
  fflush(stdout);

  while (run) {
      usleep(delay);
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

static useconds_t parsedelay(const char *value)
{
  char *endptr;
  double secf;

  errno = 0;
  secf = strtof(value, &endptr);

  if (errno != 0) {
    perror("strtof");
    exit(EXIT_FAILURE);
  }

  if ((value == endptr)) {
      fputs("strtof: failed to parse value\n", stderr);
      exit(EXIT_FAILURE);
  }

  if (secf <= 0.0) {
      fputs("DELAY cannot be zero or negative\n", stderr);
      exit (EXIT_FAILURE);
  }

  if (secf > 60.0) {
      fputs("DELAY cannot be greater than 60 seconds\n", stderr);
      exit (EXIT_FAILURE);
  }

  return (SECOND * secf);
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
