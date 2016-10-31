#define TC
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#ifndef TC
#include <rand.h>
#include <gb/gb.h>
#include <gb/console.h>
#else
#include <conio.h>
#define WORD int
#define UWORD unsigned int
#define UBYTE unsigned char
#define BYTE char
#define J_RIGHT 0x01
#define J_LEFT  0x02
#define J_UP    0x04
#define J_DOWN  0x08
#define J_A     0x10
#define J_B     0x20
#define J_SELECT 0x40
#define J_START 0x80
#endif
/* für othello */
WORD i4[8] = {-1, 0, 1,1,1,0,-1,-1};
WORD j4[8] = {-1,-1,-1,0,1,1, 1, 0};
char ch;
WORD sc[3];
UBYTE a[10][10];
WORD n1,np,op,pt,s1,s2,s3,s4,s5,xl,xh,yl,yh;
BYTE done,over;

#ifdef TC
void waitpadup() {}
BYTE joypad()
{
  BYTE joy,j;
  j = getch();
  switch (j)
  {
    case 'r': joy = J_RIGHT;
	      break;
    case '2': joy = J_RIGHT;
	      break;
    case 'l': joy = J_LEFT;
	      break;
    case '4': joy = J_LEFT;
	      break;
    case 'u': joy = J_UP;
	      break;
    case '1': joy = J_UP;
	      break;
    case 'd': joy = J_DOWN;
	      break;
    case '3': joy = J_DOWN;
	      break;
    case ' ': joy = J_A;
	      break;
    case 'a': joy = J_A;
	      break;
    case '5': joy = J_A;
	      break;
    case  9 : joy = J_B;
	      break;
    case 'b': joy = J_B;
	      break;
    case '6': joy = J_B;
	      break;
    case 's': joy = J_SELECT;
	      break;
    case '8': joy = J_SELECT;
	      break;
    case 13 : joy = J_START;
	      break;
    case '7': joy = J_START;
	      break;
    default : joy = J_A;
	      break;
  }
  return joy;
}
BYTE waitpad(BYTE mask)
{
  BYTE joy;
  do
  {
    joy = joypad();
  } while ((joy & mask) != joy);
}
void initrand() { randomize(); }
#endif

void wait_start()
{
  UBYTE joy;
  printf("\nSTART druecken.");
  joy = waitpad(J_START);
  waitpadup();
}

void clr()
{
#ifndef TC
  int x,y;
  for(y=0; y != 18; y++)
  {
    for(x=0;x != 20; x++)
    {
      gotoxy(x,y);
      puts(" ");
    }
  }
  gotoxy(0,0); /*home*/
#else
  clrscr();
#endif
}


/* für othello */
void score()
{
  WORD i,j,at;
  char c;

  sc[1] = 0; /* score/pieces for player 1*/
  sc[2] = 0; /* score/pieces for player 2*/
  n1 = 0;    /* total pieces in game     */
  for (i=1;i<9;i++)
  {
    for (j=1;j<9;j++)
    {
      at = a[i][j];
      c = '_';
      if (at == 1) { c = 'X'; sc[1] += 1; }
      if (at == 2) { c = 'O'; sc[2] += 1; }
#ifdef TC
      gotoxy(2*i+1,2*j+2);
#else
      gotoxy(2*i,2*j);
#endif
      printf("%c",c);
    }
  }
  n1 = sc[1] + sc[2]; /* total pieces in game*/
  if ((sc[op]==0) || (n1==64))
  {
    clr();
    printf("Player X = %d.\n",sc[1]);
    printf("Player O = %d.\n",sc[2]);
    if (sc[1]==sc[2]) printf("It is a tie !!\n");
    else
    {
      if (sc[1]>sc[2]) printf("Player X");
      else  printf("Player O");
      printf(" won!\n");
    }
    over = 1;
    done = 1;
  }
}

void intro()
{
  clr();
  printf("\n -*-  OTHELLO  -*-");
  printf("\n   Author:Unkown ");
  printf("\n Adapted :P.Leabo");
  printf("\nEnhanced :R.Vollmer");
  printf("\nPascal V.:C.Jones");
  printf("\nOrig.for :PET 1984");
  printf("\nGB C.V.by:P.Sieg\n");
  wait_start();
}

void instructions(BYTE l)
{
  clr();
  if (l==0)
  {
	/*12345678901234567890*/
  printf("Spiel auf 8x8 Feld.\n");
  printf("Ueberspringe Gegner\n");
  printf("bis zum freien Feld\n");
  printf("konvertiere Steine\n");
  printf("d. Gegners in eigene");
  printf("Wer die meisten hat\n");
  printf("gewinnt!\n");
  wait_start();
  printf("\nEing.Spalte/Zeile\n");
  printf(" 1 - oben\n");
  printf(" 2 - rechts\n");
  printf(" 3 - unten\n");
  printf(" 4 - links\n");
  printf(" 5 - A\n");
  printf(" 6 - B\n");
  printf(" 7 - Start\n");
  printf(" 8 - Select\n");
  wait_start();
  }
  else
  {
  /*english*/
	/*12345678901234567890*/
  printf("Played on 8x8 board\n");
  printf("col+rows from 1 - 8\n");
  printf("Try to place your\n");
  printf("pieces so that it\n");
  printf("outflanks your\n");
  printf("opponent, creating\n");
  printf("horiz., vertical\n");
  printf("or diagonal runs of\n");
  printf("opposing pieces,\n");
  printf("turning them into\n");
  printf("yours.");
  wait_start();
  printf("\nInp. Columns/Rows\n");
  printf(" 1 - up\n");
  printf(" 2 - right\n");
  printf(" 3 - down\n");
  printf(" 4 - left\n");
  printf(" 5 - A\n");
  printf(" 6 - B\n");
  printf(" 7 - Start\n");
  printf(" 8 - Select\n");
  wait_start();
  }
}

void initialize()
{
  UBYTE joy;
  WORD i,j,at;
  done = 0; over = 0;
  xl = 3; xh = 6;
  yl = 3; yh = 6;
  printf("\nHow many players?\n(A=1 or B=2) ");
  do
  {
    joy = joypad();
    waitpadup();
    delay(500);
  } while (joy==0);
  if (joy & J_B) np = 2;
  else
  {
    np = 1;
    s2 = 2; s4 = 1; s5 = -2;
    /* not so strong computer opponent -> s2 = 0; s4 = 0; s5 = 0; */
  }
  for (i=0;i<10;i++)
  {
    for (j=0;j<10;j++)
    {
      a[i][j] = 0;
      /*at = a[i][j];
      gotoxy(1,1); printf("%d %d %d      ",i,j,at); delay(2000);*/
    }
  }
  a[4][4] = 1; a[4][5] = 2;
  a[5][4] = 2; a[5][5] = 1;
  n1 = 4;
  sc[1] = 2;
  sc[2] = 2;
}

void draw_board()
{
    clr();
    printf("\n  1 2 3 4 5 6 7 8 \n");
    printf("\n  _ _ _ _ _ _ _ _ 1\n");
    printf("\n  _ _ _ _ _ _ _ _ 2\n");
    printf("\n  _ _ _ _ _ _ _ _ 3\n");
    printf("\n  _ _ _ X O _ _ _ 4\n");
    printf("\n  _ _ _ O X _ _ _ 5\n");
    printf("\n  _ _ _ _ _ _ _ _ 6\n");
    printf("\n  _ _ _ _ _ _ _ _ 7\n");
    printf("\n  _ _ _ _ _ _ _ _ 8\n");
}

BYTE test_move(WORD x, WORD y)
{
  WORD i,j,t;
  BYTE m;

  m = 0;

  for (i=-1;i<2;i++)
  {
    for (j=-1;j<2;j++)
    {
       t = a[x+i][y+j];
       if (t == op) m = 1;
    }
  }
  return m;
}

void count_flank(WORD x, WORD y, WORD z)
{
  WORD i5,j5,i6,j6,k,k1,at;

  s1 = 0; k = 0;
  while (k<8)
  {
    s3 = 0;
    i5 = i4[k]; j5 = j4[k]; i6 = x+i5; j6 = y+j5;
    at = a[i6][j6];
    if (at==op)
    {
      do
      {
        s3 += 1;
        i6 += i5;
        j6 += j5;
        at = a[i6][j6];
      }  while ((at!=0) && (at!=pt));
      if (at==pt)
      {
        s1 += s3;
        if (z==1)
        {
          i6 = x; j6 = y;
          for (k1=0;k1<=s3;k1++)
          {
            a[i6][j6] = pt;
            i6 += i5;
            j6 += j5;
          }
        }
      }
   }
   k += 1;
  }
}

void show_move(WORD x, WORD y) /* procedure show_move(x,y: integer); */
{
  char c;

  c = '_';
  if (pt == 1) c = 'X';
  if (pt == 2) c = 'O';
#ifdef TC
  gotoxy(2*x+1,2*y+2);
#else
  gotoxy(2*x,2*y);
#endif
  printf("%c",c);
  count_flank(x,y,1);
}

void computer_move()
{
  WORD i,j,b1,i3,j3,at;
  UBYTE joy;

  b1 = -1; i3 = 0; j3 = 0;
  for (i=xl;i<=xh;i++)
  {
    for (j=yl;j<=yh;j++)
    {

      at = a[i][j];
      if (at == 0)
      {
        if (test_move(i,j))
        {
          count_flank(i,j,0);
          if (s1>0)
          {
            if ((i==1) || (i==8)) s1 += s2;
            if ((j==1) || (j==8)) s1 += s2;
            if ((i==2) || (i==7)) s1 += s5;
            if ((j==2) || (j==7)) s1 += s5;
            if ((i==3) || (i==6)) s1 += s4;
            if ((j==3) || (j==6)) s1 += s4;
            if (s1>=b1)
            {
              b1 = s1; i3 = i; j3 = j;
            }
          }
        }
      }
    }
  }
  if (((i3>=1) && (i3<=8)) && ((j3>=1) && (j3<=8)))
  {
    i = i3; j = j3;
    gotoxy(1,1);
    printf("b1=%d s1=%d i=%d j=%d  ",b1,s1,i,j);
#ifdef TC
    gotoxy(1,20);
#else
    gotoxy(0,17);
#endif
    printf("Press A...     ");
    joy =waitpad(J_A);
    waitpadup();
    show_move(i,j);
    if ((i<=xl) && (i != 1)) xl -= 1;
    if ((i>=xh) && (i != 8)) xh += 1;
    if ((j<=yl) && (j != 1)) yl -= 1;
    if ((j>=yh) && (j != 8)) yh += 1;
  }
  else
  {
#ifdef TC
    gotoxy(1,20);
#else
    gotoxy(0,17);
#endif
    printf("Computer passes.");
    delay(1500);
  }
}

void player_move()
{
  WORD i,j,goodmove,at;
  UBYTE joy;
  char c;
  goodmove = 0;
  do
  {
#ifdef TC
    gotoxy(1,20);
#else
    gotoxy(0,17);
#endif
    if (pt==2) c = 'O';
    else c = 'X';
    printf("Player %c:        ",c);
#ifdef TC
    gotoxy(10,20);
#else
    gotoxy(10,17);
#endif
    do
    {
      joy = joypad();
      waitpadup();
      delay(150);
    } while (joy==0);
    if (joy & J_UP)     i = 1;
    if (joy & J_RIGHT)  i = 2;
    if (joy & J_DOWN)   i = 3;
    if (joy & J_LEFT)   i = 4;
    if (joy & J_A)      i = 5;
    if (joy & J_B)      i = 6;
    if (joy & J_START)  i = 7;
    if (joy & J_SELECT) i = 8;
    printf("%d",i);

    do
    {
      joy = joypad();
      waitpadup();
      delay(150);
    } while (joy==0);
    if (joy & J_UP)     j = 1;
    if (joy & J_RIGHT)  j = 2;
    if (joy & J_DOWN)   j = 3;
    if (joy & J_LEFT)   j = 4;
    if (joy & J_A)      j = 5;
    if (joy & J_B)      j = 6;
    if (joy & J_START)  j = 7;
    if (joy & J_SELECT) j = 8;
    printf("%d",j);

    at = a[i][j];
    if (at==0)
    {
      if (test_move(i,j))
      {
        count_flank(i,j,0);
        if (s1>0)
        {
          goodmove = 1;
          show_move(i,j);
        }
	else printf(" -1"); /*Sorry, does not flank a row.*/
      }
      else printf(" -2"); /*Sorry, not next to opponents pieces*/
    }
    else printf(" -3"); /*Sorry, square occupied; try again*/
    delay(1500);
  } while (!goodmove);
}


/* ende für othello */


void f_a()
{
  char input[10];
  WORD a,b,c;

  clr();
  printf("\nZahl 1: ");
  gets(input);
  a = atoi(input);
  printf("\nZahl 2: ");
  gets(input);
  b = atoi(input);
  printf("\n+-*/? : ");
  gets(input);

  switch (input[0]) {
    case '+': c = a + b;
              break;
    case '-': c = a - b;
              break;
    case '*': c = a * b;
              break;
    case '/': c = a / b;
              break;
    default : c = 0;
  }
  printf("\n= %d\n",c);

  wait_start();
}

void f_b()
{
  char input[10];
  WORD a,b,c,ci,d,o,cnt_err,cnt_ok,cnt_loop,max_loop;
  UBYTE joy;

  d = 10;
  cnt_err = 0;
  cnt_ok  = 0;
  max_loop = 10;

  clr();
  printf("\nBitte waehlen:");
  printf("\n A-leicht");
  printf("\n B-schwer");
  joy = waitpad(J_A | J_B);
  waitpadup();
  if (joy & J_B) d = 100; /* d=Schwierigkeitsgrad*/
  initrand(clock());

  for (cnt_loop=0;cnt_loop<max_loop;cnt_loop++) {
    a = rand() % d;
    b = rand() % d;
    o = rand() % 4;  /* operation; 1=+;2=-;3=*;0=/*/
    /*printf("%d]",o);*/

    switch (o) {
      case 1: printf("\n%d + %d = ",a,b);
              c = a + b;
              break;
      case 2: printf("\n%d - %d = ",a,b);
              c = a - b;
              break;
      case 3: printf("\n%d * %d = ",a,b);
              c = a * b;
              break;
      default:if (b==0)  b = 1;
              printf("\n%d / %d = ",a,b);
              c = a / b;
              break;
    }

    gets(input);
    ci = atoi(input);
    if (c != ci) { cnt_err++; printf("Falsch! Richtig:%d\n",c); }
    else { cnt_ok++; printf("Richtig!\n"); }

  } /* for */
  clr();
  printf("\nKopfrechnen\n\nErgebnisse:\n");
  printf("\nRichtig: %d=%d Proz.",cnt_ok,cnt_ok   * 100 / max_loop);
  printf("\nFalsch : %d=%d Proz.",cnt_err,cnt_err * 100 / max_loop);
  wait_start();
}

void f_se()
{
  char ch[20],my[20];
  WORD done;
  WORD i,j,count;
  WORD z0,z1,z2,z3,z4; /* 4 digits for my 4-digit char array */

  count = 1; done = 0;
  /* create random 4-digit string in char array my */
  /* strcpy(my,"1234"); */
  initrand(clock());
  z1 = (rand() % 9)+1; /* 1..9 */
  do { z2 = (rand() % 9)+1; } while (z2==z1);
  do { z3 = (rand() % 9)+1; } while ((z3==z1) || (z3==z2));
  do { z4 = (rand() % 9)+1; } while ((z4==z1) || (z4==z2) || (z4==z3));
  z0 = z1*1000+z2*100+z3*10+z4;
  sprintf(my,"%d",z0);
  my[4] = 0;     /* terminate string */

  clr();
  /* insert my to see code */
  printf("Rate meinen Code.\n");
  /*printf("(%s)(%d)",my,z0);*/
  printf("\n0 fuer Abbruch.\n");

   do {
     printf("\nVersuch %d:",count);
     gets(ch);
     if (strcmp(ch,"0")==0) done = 2; /* user abort */
     if (strcmp(my,ch)==0)  done = 1; /* success; code broken */
     /* check how near guess is and print hint */
     /* for right digit at  right position print # */
     /* for right digit but wrong position print + */
     if (done==0) {
       for (i=0;i<4;i++) {
         if (ch[i]==my[i]) printf("# ");
         else for (j=0;j<4;j++) if (ch[i]==my[j]) printf("+ ");
       }
     }
     count++;

   } while (done==0);

   if (done==1)
     printf("\n\nCode geknackt!");
   else
     printf("\n\nSchade.. :-(");

  wait_start();
}

void f_st()
{
  UBYTE joy;
  clr();
  intro();
  printf("\nAnleitung?:");
  printf("\n A-Nein/Skip");
  printf("\n B-Deutsch");
  printf("\nSe-English");
  joy = waitpad(J_A | J_B | J_SELECT);
  waitpadup();
  if (joy & J_B)      instructions(0);
  if (joy & J_SELECT) instructions(1);
  initialize();
  draw_board();
  do {
    pt = 1; op = 2;
    player_move();
    score();
    if (!over)
    {
      pt = 2; op = 1;
      if (np==2) player_move();
      else computer_move();
      score();
    }
  } while (!over);
  wait_start();
}

void main()
{
  UBYTE joy;

  while(1) {
    clr();
    printf("\nBitte waehlen:\n");
    printf("\n A-Taschenrechner");
    printf("\n B-Kopfrechnen");
    printf("\nSe-Mastermind");
    printf("\nSt-Othello\n");
    joy = waitpad(J_A | J_B | J_START | J_SELECT);
    waitpadup();
    if (joy & J_A) f_a();
    if (joy & J_B) f_b();
    if (joy & J_SELECT) f_se();
    if (joy & J_START)  f_st();
  }
}

