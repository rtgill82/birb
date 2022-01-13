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
#define RAND1() (rand() % 2)
#define RAND_CHOICE(a, b) (RAND1() ? (a) : (b))

typedef struct args_st {
    useconds_t delay;
    bool random;
} Args;

typedef char (*Func)(char ch);

static const int chidx[] = {0, 3};

static struct sigaction action;
static char birb[] = "bird\r";
static volatile sig_atomic_t run = true;

static void handler(int signal);
static void showhelp();
static void parseargs(int argc, char *argv[], Args *args);
static useconds_t parsedelay(const char *value);
static char rotatech(char ch);
static char flipch(char ch);

int main(int argc, char *argv[])
{
  Func mutate;
  int ch;
  Args args;

  args.delay = SECOND;
  args.random = false;
  mutate = rotatech;

  if (argc > 1) {
      parseargs(argc, argv, &args);
  }

  if (args.random == true) {
      mutate = flipch;
  }

  action.sa_handler = handler;
  if (sigaction(SIGINT, &action, NULL)) {
      perror("sigaction");
      exit(EXIT_FAILURE);
  }

  srand(time(NULL));
  printf("%s", birb);
  fflush(stdout);
  usleep(args.delay);

  /* Print 'birb' first. */
  birb[3] = 'b';
  printf("%s", birb);
  fflush(stdout);

  while (run) {
      usleep(args.delay);
      ch = chidx[RAND1()];
      birb[ch] = mutate(birb[ch]);
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

static void showhelp()
{
  puts("USAGE: birb [-hr?] [--help] [--random] [DELAY]\n");
  puts("  -h, -?, --help\tDisplay this help message");
  puts("  -r, --random\t\trandomly mutate characters\n");
}

static void parseargs(int argc, char *argv[], Args *args)
{
  int count = 0;

  for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
          if (argv[i][1] == '-') {
              /* Handle long options. */
              if (strcmp("random", &argv[i][2]) == 0) {
                  args->random = true;
              } else if (strcmp("help", &argv[i][2]) == 0) {
                  showhelp();
                  exit(EXIT_SUCCESS);
              } else {
                  showhelp();
                  exit(EXIT_FAILURE);
              }
          } else {
              /* Handle short options. */
              switch(argv[i][1]) {
                case '?':
                case 'h':
                  showhelp();
                  exit(EXIT_SUCCESS);
                case 'r':
                  args->random = true;
                  break;
                default:
                  showhelp();
                  exit(EXIT_FAILURE);
                  break;
              }
          }
      } else {
          /* Handle main argument. */
          if (count >= 1) {
              showhelp();
              exit(EXIT_FAILURE);
          }

          count += 1;
          args->delay = parsedelay(argv[i]);
      }
  }
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

static char rotatech(char ch)
{
  switch(ch) {
    case 'b': return 'q';
    case 'd': return 'b';
    case 'p': return 'd';
    case 'q': return 'p';
    default: abort();
  }
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
