/*
 *  File:         tictactoe.c
 *  Author:       David Ljung <dxl@fc.hp.com>
 *  Copyright:    1994, Spawning Cow! Productions
 *                This software is free on the sole condition that this
 *                header file is left as part of the file, unchanged
 *  Description:  Plays a mean tic-tac-toe game, using an algorithm
 *                which is somewhat described in a long paper at the bottom
 */
#include <stdio.h>

#define X	4
#define O	1
#define SPACE	0
#define S	SPACE

#define SUM(x,y,z)	(board[x]+board[y]+board[z])

/* Global vars - I know, a bad idea :) */
int board[9];
int done=0;

#define PLAYER	X
#define COMPUTER O

char bd_ch(int num) {
  if(num==X) return 'X';
  if(num==O) return 'O';
  if(num==S) return ' ';
  return '?';
}

show_board(){
  fprintf(stdout,"\n"
                 " %c | %c | %c\n-----------\n"
                 " %c | %c | %c\n-----------\n"
                 " %c | %c | %c\n\n",
                 bd_ch(board[6]),bd_ch(board[7]),bd_ch(board[8]),
                 bd_ch(board[3]),bd_ch(board[4]),bd_ch(board[5]),
                 bd_ch(board[0]),bd_ch(board[1]),bd_ch(board[2])
                 );
}

int check_line(int x,int y,int z,int sum,int *a,int *b,int *c) {
  if(board[x]+board[y]+board[z]!=sum) return 0;
  if(SUM(x,y,z)!=sum) return 0;
  *a=x; *b=y; *c=z;
  return 1;
}
int find_line(int sum,int *a,int *b,int *c) {
  if(check_line(0,1,2,sum,a,b,c)) return 1;
  if(check_line(3,4,5,sum,a,b,c)) return 1;
  if(check_line(6,7,8,sum,a,b,c)) return 1;
  if(check_line(0,3,6,sum,a,b,c)) return 1;
  if(check_line(1,4,7,sum,a,b,c)) return 1;
  if(check_line(2,5,8,sum,a,b,c)) return 1;
  if(check_line(0,4,8,sum,a,b,c)) return 1;
  if(check_line(2,4,6,sum,a,b,c)) return 1;
  return 0;
}

check_win() {
  int a,b,c;

  if(find_line(X+X+X,&a,&b,&c)) {
    fprintf(stdout,"\nX wins at %d,%d,%d!\n\n",a+1,b+1,c+1);
    done=1;
  }
  if(find_line(O+O+O,&a,&b,&c)) {
    fprintf(stdout,"\nO wins at %d,%d,%d!\n\n",a+1,b+1,c+1);
    done=1;
  }
  for(a=0,b=0;a<9;a++) if(board[a]==S) b=1;
  if(b==0) {
    fprintf(stdout,"\nTie game\n\n");
    done=1;
  }
}

player_turn() {
  char buf[81];
  int num=-1;

  fflush(stdin);
  fprintf(stdout,"Your move: ");
  fgets(buf,80,stdin);
  if(!strncmp("quit",buf,4)) return done=1;
  if(!sscanf(buf,"%d",&num) || num<1 || num>9) {
     fprintf(stdout,"Not a valid entry: %s",buf);
     show_board();
     return player_turn();
  }
  if(board[num-1]!=S) {
     fprintf(stdout,"The space %d is already a %c\n",num,bd_ch(board[num-1]));
     show_board();
     return player_turn();
  }
  board[num-1]=PLAYER;
}

#define C COMPUTER
#define P PLAYER
go(int where) {
  if(board[where]!=S) {
    fprintf(stderr,"COMPUTER HAS MADE FATAL MOVE\n");
    exit(-1);
  }
  board[where]=C;
  return;
}
computer_turn() {
  int a,b,c;
  int i;

/*
 * first check for win spots
 * if you find one, just take it.
 * It doesn't matter if there are others - you've just won
 */
/* fprintf(stdout,"                       Looking for wins\n"); */
  if(find_line(C+C+S,&a,&b,&c)) {
    if(board[a]==S) return go(a);
    if(board[b]==S) return go(b);
    if(board[c]==S) return go(c);
  }
/*
 * then check for blocks
 * if you find one - you have to take it.
 * It doesn't matter if there are others, you'll just have to lose
 * since you don't have a win you can take right now (otherwise we
 * would have taken it above, and wouldn't be here
 */
/* fprintf(stdout,"                       Looking for blocks\n"); */
  if(find_line(P+P+S,&a,&b,&c)) {
    if(board[a]==S) return go(a);
    if(board[b]==S) return go(b);
    if(board[c]==S) return go(c);
  }
/* that was easy, no choices involved.  Now we have
 * to *choose* a place to go.  :)
 * first lets see if we have any TAS situations we should take
 * check for two lines going through empty spots with C+S+S.
 * Fill the empty and we have two lines with C+C+S, and they can only block one
 */
/* fprintf(stdout,"                       Looking for TAS\n"); */
  for(i=0;i<9;i++) {
    int tot=0;

    if(board[i]!=S) continue;

    /* check horizontal line - if C+S+S then increment total */
    if(SUM(i/3*3,i/3*3+1,i/3*3+2)==C+S+S) tot++;

    /* check vertical line */
    if(SUM(i,(i+3)%9,(i+6)%9)==C+S+S) tot++;

    /* possibly check diagonals */
    if(i==0 || i==4 || i==8) if(SUM(0,4,8)==C+S+S) tot++;
    if(i==2 || i==4 || i==6) if(SUM(2,4,6)==C+S+S) tot++;
/* if(tot>=2) fprintf(stdout,"FOUND TAS AT %d\n",i); */
    if(tot>=2) return go(i);  /* WE HAVE A TAS SITUATION! :) */
  }
  /* No TAS found, check automatic moves
   * (Actually, this should look for TWBS - oh well)
   */
  /* First move */
  if(board[6]==S) return go(6);
  /* If they take center we have to go across, or else we risk losing */
  if(board[4]==P) return go(2);
  /* Our normal TWBS is if they don't go to 0 or 3 we can go to 0
   * and TWBS because they will need to block at 3.
   * However, if they go to 0 or 3, we can do TWBS at 8.
   * (In reality, there are *lots* of TWBS at this point)
   */
  if(board[0]==P || board[3]==P) return go(8);
  if(board[0]==S) return go(0);

}

main() {
  int i;

  fprintf(stdout,"Tic Tac Toe\n\n");
  for(i=0;i<9;i++) board[i]=S;
  fprintf(stdout,
          " 7 | 8 | 9\n-----------\n 4 | 5 | 6\n-----------\n 1 | 2 | 3\n\n");

  while(!done) {
    computer_turn();
    show_board();
    check_win();
    if(!done) {
      player_turn();
      show_board();
      check_win();
    }
  }
}


/*
 * The 'algorithm'
 * This is actually a paper I typed up for a friend of mine who was looking
 * into doing a digital version of a tic-tac-toe game.  This paper contains
 * the majority of the theory and methods I use above
 * (the 'paper' first describes a deterministic algorithm for going first,
 *  then I go on to describe methods which you could use in general to play
 *  tic tac toe whether first or second, which would give the same results)
 *

From: Dave++ Ljung <dxl@hpesdxl.fc.hp.com>
Subject: Tic Tac Toe Ideas
To: jlassy@ux4.cso.uiuc.edu (Justin Wolfgang Amadeus Mozart Bob Lassy)
Date: Wed, 2 Nov 94 21:47:33 MST

First, a Tic Tac Toe algorithm where computer goes first

Definitions:
board:

  1 | 2 | 3
 -----------
  4 | 5 | 6
 -----------
  7 | 8 | 9

TAS	They are screwed - you have two lines that they need to block.
	For example: (it's their turn)

  O |   | X
 -----------
    |   |
 -----------
  X | O | X

If they go to 5, you go to 6, if they go to 6, you go to 5.
You win no matter what.  The TAS possibilities will be labeled with
the two positions you could go to, for this example:  TAS(5,6)

TWBS	They will be screwed - it is their turn, but you have forced them
	into a position that will create a TAS.  For example:

  O |   |  
 -----------
    |   |  
 -----------
  X |   | X

They will be screwed because they need to go to 8 to block, then you 
can move to 3 and you have the same situation as the TAS above -- TAS(5,6)
TWBS will be labeled like such:  TWBS(block:next move:choice1,choice2)
Where:	block=the position they have to go to block (be prepared to take
	      this spot if they don't go there, otherwise you will look
	      stupid declaring a win you didn't get)
	next move=the next move you take after they take the block position
	          (if they don't take the block position, of course, you do)
	choice1,2=the two choices of the TAS
So the above example would be:  TWBS(8:3:5,6)
Which means that they need to go to 8 to block (or you take 8 and win) and then
you go to 3, and that gives you a TAS(5,6)

Make sense?  Good - because that makes the algorithm much easier to type up
and understand.  I did it this way because after you make your second move,
it is always a TWBS situation, *unless* they take the center as their first
move, then it gets stupid (this is the only way they can tie)

(this table *should* be correct -- try it out and see, it will help
 you understand it anyways :)

Current state		Their move (x)		Move to		Next State
-------------		--------------		-------		----------
start						9		A

A			1			3		TWBS(6:7:5,8)

A			3			1		TWBS(5:7:4,8)
A			7			1		TWBS(5:3:2,6)

A			2, 4 or 8		1		TWBS(6:5:1,7)

A			6			7		TWBS(8:5:1,3)

A			5			1		B

B			2,4,6 or 8		**(see below)

B			3			7		TAS(2,6)
B			7			3		TAS(2,6)

** NOTE:
In this situation, you cannot win.  You will need to block, then they
need to block, etc...

There are *many* alternatives to the above table that still has the
same effect.  For example:
A			7			3		TWBS(6:1:2,5)


Each time I had an alternative, I chose the one that most looked like
one of the other lines, which makes it easier to simplify.  You can
simplify further than the lines above, for example, lines A 3 and A 7 could
be combined something like this:  (Their move=x)
A			x=3 or 7		1		TWBS(5:7:...)

where ...=4,8 if x=3 or 2,6 if x=7


So - if you are going to use some method other than this table, you might
want to use some of the alternatives so that the pattern isn't as obvious
(the above table often goes in the same places and does the same things).

The quick way to program it (that would require memory) is to just do
a bunch of case statements like the table above.  So you have the case
statement for state A which is switched on their move.  For every possible
move they have, the case statement would give you your move and the next
state to go to (another case statement)

That's one way to do it.

Since you are going to need to set up some functions for the possibility
of the other player going first, you might want to use this more interesting
method (I'm pretty sure this will give just as good results as the table,
but it lets you randomize it a little - for example, when it says ANY
corner, you can randomly pick one, so the game can be different every time):

First, go to 9 (or ANY of the corners, for that matter)

Then, when they move, if they don't take the center, take ANY of the other
corners that fulfills the following conditions: (If they pick the center,
see CENTER below)
  A  They aren't already blocking it (for example, you go to 9, they
     go to 8, this rules out 7)
  B  If they do block it, it doesn't give them 2 in a row with a blank
     space that you will need to block. (In other words, if they block
     it, it won't put you in a position where you need to block)  (for
     example, if you go to 9, then they go to 8, you can't go to 1, because
     they will block at 5, and then you will be forced to block at 2 - you
     will have lost your dominance.  Try drawing this out to see)

Then, you will have two corners - which obviously have a line potential.
This is now a TWBS situation:
They will have to block it (because of condition A), and this doesn't
force you to block.  Now you can go just about anywhere, and you will
find that you have at least one position you can go to that gives you
a TAS situation.  You will have to write a function that looks at the
board for your turn and finds a place that you can go that will give you
the TAS situation, but you will probably want to write something like this
anyways for the possibility that they can go first.  (see below for
one approach)

CENTER: Now, what if they went to the center on their turn?

Then you go to the opposite corner.  I know it doesn't make sense, since
they already have that line blocked, but if you go anywhere else, it could
set up a win for *them*.  Trust me, I've worked it out. :)

Then, if they go to one of the side squares (the even ones) then you just
block, and they block, and you block, and they block, and finally you block,
and its a damn tie.  Oh well.

If they go to one of the two corners, they have lost.  The TWBS is automatic;
since they hold the center and one of the corners, you pick the other corner
to block, and suddenly TAS.

---

Here is an example of how you can look for TAS situations.  They will have
taken the block (again, if they don't, be ready to take the spot and win).
When they do, you need to find where you can go to make the TAS.
You find this by picking the empty square that gives you two possibilities
to win, because once you put an X in the square, then there will be
two lines that have two X's and an empty space (where the empty spaces
are different).  They can only block one empty space, so you take the other.

For example, if you have 3 and 9, and 1, 2 and 5 are blank, then by
taking 1 you have a TAS situation.  2 and 5 will both give you three in a row,
so they can only block one.

There is an easy way to figure out if you have that situation on any given
line(s).  This algorithm I am about to describe is useful for figuring out
the state on any given line in general.  For example, if you want to know
if a line has one X on it and two blank spaces, or two X's and one blank
space, or an X and an O or whatever..  here is how you can do it without
trying to match the line up to each possible pattern.  For example, if we
are looking at the line 1,2,3 to see if it has one X and one O and one
space in it, we could do a comparison of 1,2,3 to each possibility:

  XO_
  OX_
  X_O
  O_X
  _XO
  _OX

But this would be stupid.  A better way is through my addition algorithm.

Just add up the values of each position on the line, where the values are:
	space	0
	X	1
	O	4

(trust me, there is method to this madness)

This sum is going to be unique for each possible line, regardless of
position, which means that all lines with one X and one O and a space
are going to have the same value, which will be a different value
then a line that has two X's and one O which will be different from
a line with two X's and a space, etc.. etc..

So, to look for a line with one X and one O and one space, we just
add up the values of the three positions in the line.  If they
equal=X+O+space=4+1+0=5, then we have found it.

This can help you find block situations.  If you are X, and there are
any lines with the sum=O+O+space=4+4+0=8, then you need to block there,
unless you have somewhere you can go to win: sum=X+X+space=1+1+0=2.

Fucking cool, eh?

So, here is how I would go about looking for TAS situations.  Pick an
empty space.  Each empty space except 5 has two lines going through it
(for example, 2 has 1,2,3 and 2,5,8).  So, see if the sum of the two
lines going through the empty space=X+space+space=1+0+0=1 then you
have a TAS situation.  You can just go to that empty space, making those
two lines now have a value=X+space+X=1+0+1=2, which gives you two possible
wins (since the space in each line is different from the other space -this
isn't hard to proove).  If the two lines going through the space do not
have the sum of 1, then go on to the next empty space.
If the empty space is 5, it gets a little tricky, since you have 4 possible
lines through space 5.  In this case, just see if you have any two with
the sum of 1, and once again, a TAS situation.

 *
 *   That's it :)
 *
 */

