#include <termios.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
/*#include <sys/select.h>*/

/*Listing 2. Terminal I/O Examples*/
/* This will be used for new terminal settings */
static struct termios current;

/* This will hold the initial state so that we
 * can restore it later. */
static struct termios initial;

/* Restore the terminal settings to those saved
 * when term_init was called. */
void term_restore(void)
{
  tcsetattr(0, TCSANOW, &initial);
}

/* Clean up terminal; called on exit */
void term_exit()
{
  term_restore();
}

/* Will be called when control-Z is pressed;
 * this correctly handles the terminal. */
void term_ctrlz()
{
  signal(SIGTSTP, term_ctrlz);
  term_restore();
  signal(getpid(), SIGSTOP);
}

/* Will be called when the application is
 * continued after having been stopped. */
void term_cont()
{
  signal(SIGCONT, term_cont);
  tcsetattr(0, TCSANOW, &current);
}

/* Needs to be called to initialize the terminal */
void term_init(void)
{
  /* If stdin isn't a terminal this fails. But
   * then so does tcsetattr so it doesn't matter. */
  tcgetattr(0, &initial);
  /* Save a copy to work with later */
  current = initial;
  /* We _must_ clean up when we exit */
  signal(SIGINT, term_exit);
  signal(SIGQUIT, term_exit);
  /* Control-Z must also be handled */
  signal(SIGTSTP, term_ctrlz);
  signal(SIGCONT, term_cont);
  atexit(term_exit);
}

/* Set raw=character-by-character input mode */
void term_character(void)
{
  /* One or more characters are sufficient
   * to cause a read to return. */
  current.c_cc[VMIN] = 1;
  /* No timeout; read waits forever
   * until ready. */
  current.c_cc[VTIME] = 0;
  /* Line-by-line mode off */
  current.c_lflag &= ~ICANON;
  current.c_lflag &= ~ECHO;
  tcsetattr(0, TCSANOW, &current);
}

/* Return to cooked=line-by-line input mode */
void term_line(void)
{
  current.c_lflag |= ICANON;
  tcsetattr(0, TCSANOW, &current);
}


/*Listing 1. An Example of an Implementation of kbhit()*/
/*Terminal must be already in raw=each char mode!*/

int unix_kbhit(void)
{
  struct timeval tv;
  fd_set read_fd;

  /* Do not wait at all, not even a microsecond */
  tv.tv_sec=0;
  tv.tv_usec=0;

  /* Must be done first to initialize read_fd */
  FD_ZERO(&read_fd);

  /* Makes select() ask if input is ready:
   * 0 is the file descriptor for stdin    */
  FD_SET(0,&read_fd);

  /* The first parameter is the number of the
   * largest file descriptor to check + 1. */
  if (select(1, &read_fd,
            NULL, /*No writes*/
            NULL, /*No exceptions*/
            &tv)
     == -1)
    return 0;	/* An error occured */

  /*	read_fd now holds a bit map of files that are
   * readable. We test the entry for the standard
   * input (file 0). */
  if (FD_ISSET(0,&read_fd))
    /* Character pending on stdin */
    return 1;

  /* no characters were pending */
  return 0;
}

int unix_kbhit_wait(unsigned long usec)
{
  struct timeval tv;
  fd_set read_fd;

  /* Do not wait at all, not even a microsecond */
  tv.tv_sec=0;
  tv.tv_usec=usec;

  /* Must be done first to initialize read_fd */
  FD_ZERO(&read_fd);

  /* Makes select() ask if input is ready:
   * 0 is the file descriptor for stdin    */
  FD_SET(0,&read_fd);

  /* The first parameter is the number of the
   * largest file descriptor to check + 1. */
  if (select(1, &read_fd,
            NULL, /*No writes*/
            NULL, /*No exceptions*/
            &tv)
     == -1)
    return 0;	/* An error occured */

  /*	read_fd now holds a bit map of files that are
   * readable. We test the entry for the standard
   * input (file 0). */
  if (FD_ISSET(0,&read_fd))
    /* Character pending on stdin */
    return 1;

  /* no characters were pending */
  return 0;
}

