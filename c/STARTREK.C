/* This is an adaptation of an old text-based Star Trek game!
   This C program is based on a BASIC program adapted by
   L.E. Cochran 2/29/1976.  In keeping with the original
   BASIC paradigm, almost all variables and constants are
   global.

   To show how much can be done with a relatively small amount
   of code, the BASIC program was about 2-1/2 pages (150 lines),
   although almost every line included multiple statements.

   Bob Sorem -- 28 November 2000

   ported to linux in march 2002 by peter.sieg(a)gmx.de
  
*/

#include <stdio.h>
/*#include <conio.h>*/
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
/*#include <dos.h>*/
#include <time.h>

#define cprintf printf
#define getch getchar
#define delay usleep
void clrscr() {}
void nosound() {}
void sound(int i) {}

#define FALSE          0
#define TRUE           1
#define NULLC       '\0'  /* Null character */
#define F1KEY        -59  /* 'F1' key code (following NULL) */
#define BKSPCKEY       8  /* 'Backspace' key code */
#define ENTERKEY      10  /* 'Enter' key code 10=UNIX/13=DOS */
#define ESCKEY        27  /* 'ESC' key code */
#define SPC           32  /* Space character */
#define ASCHI        126  /* Maximum input character to allow */
#define CTLBKSPCKEY  127  /* 'Ctrl-backspace' key code */

char *qs = {".EKB*"};
char *ds[6] =
   {"WARP ENGINES","SHORT RANGE SENSORS","LONG RANGE SENSORS",
    "PHASERS","PHOTON TORPEDOES","GALACTIC RECORDS"};
int d[6],k1[8],k2[8],sect[8][8],quad[8][8],a,newquad,
    q1,q2,x,y,x7,y7,k,i,j,b,k0,k9,b9,s,s1,s2,t,t0,t9,p,p0;
double x1,_y1,x2,y2,x3,y3,aa,e,e0,n,rn,h,w,c,s9,k3[8];
char ans,fbuff[81],cond[7],es[16],cmdbuff[8];



void title(void)  /* Show program title */
{
   clrscr();
   cprintf("STAR TREK!!\r\n");
   cprintf("===========\r\n\n");
}  /* End title */


/*  ********************************************************************
    Function:     speaker
    Argument(s):  frequency of sound; duration of sound (ms)
    Description:  Sounds the speaker with the specified frequency for the
                  specified duration in milliseconds.
    Includes:     dos.h
*/
void speaker(int freq,int dur)
{
   sound(freq);
   delay(dur);
   nosound();
}  /* End speaker */


/*  ********************************************************************
    Function:     beep
    Argument(s):  <none>
    Description:  Sounds the speaker with the specified frequency for the
                  specified duration in milliseconds.  Calls speaker.
*/
void beep(void)
{
   speaker(880,80);
}  /* End beep */


/*  ********************************************************************
    Function:     buzz
    Argument(s):  <none>
    Description:  Sounds the speaker with the specified frequency for the
                  specified duration in milliseconds.  Calls speaker.
*/
void buzz(void)
{
   speaker(50,200);
}  /* End buzz */


/*  ********************************************************************
    Function:     clearkeyboard
    Argument(s):  none
    Description:  Clears the keyboard buffer.
    Includes:     conio.h
*/
void clearkeyboard(void)
{
   /*while (kbhit()) getch();*/
}  /* End clearkeyboard */


/*  ********************************************************************
    Function:     fgetline
    Argument(s):  buffer, max buffer length, stream pointer
    Description:  Gets a line of text from the stream and strips the
                  non-printing characters at the end of the line
    Includes:     stdio.h
*/
void fgetline(char *buff,int blen,FILE *stream)
{
int j;

   for (j=0;j<blen;j++) buff[i]=0;
   fgets(buff,blen,stream);
   for (j=strlen(buff)-1;j>=0;j--)
   {
      if (buff[j]>31) break;
      buff[j]=0;
   }
}  /* End fgetline */


void yesno(void)  /* Get Y or N from user and place result in ans */
{
   ans='X';
   while ((ans!='Y')&&(ans!='N'))
   {
      beep();
      clearkeyboard();
      ans=toupper(getch());
   }
   cprintf("%c\r\n",ans);
}  /* End yesno */


void keytocont(void)  /* Get keypress to continue */
{
   cprintf("\r\nPRESS A KEY TO CONTINUE ... ");
   clearkeyboard();
   getch();
   clearkeyboard();
   cprintf("\r\n");
}


void showinst(void)
{
FILE *ifile;
char *fn="STINSTR.TXT";

   ans='Y';
   while (ans=='Y')
   {
      if ((ifile=fopen(fn,"rt"))==NULL)
      {
         cprintf("SORRY!  CAN'T OPEN INSTRUCTION FILE: %s\r\n",fn);
         keytocont();
         ans='N';
      }
      else
      {
         i=0;
         while (i<90)
         {
            i++;
            fgetline(fbuff,80,ifile);
            cprintf("   %s\r\n",fbuff);
            if ((i==18)||(i==38)||(i==58)||(i==79))
               keytocont();
            if (feof(ifile)) i=90;
         }
         fclose(ifile);
      }
      cprintf("\r\n\nREPEAT THESE INSTRUCTIONS (Y/N)? ");
      yesno();
      clrscr();
   }
}


void fixdamage(void)  /* Repair anything that is down */
{
   for (i=0;i<6;i++) d[i]=0;
}  /* End fixdamage */


double randinit(void)  /* Initialize pseudo-random number generator */
{
struct tm t;
double r1,r2,r3,r4;
long clk;

   /*gettime(&t);
   r1=0; *t.tm_hund;*
   r2=t.tm_sec;
   r3=t.tm_min;
   r4=t.tm_hour;
   r2=floor(r2*(100.0/60.0));
   r3=floor(r3*(100.0/60.0));
   r4=floor(r4*(100.0/24.0));
   rn=r1/100.0+r2/10000.0+r3/1000000.0+r4/100000000.0;*/
   clk = clock();
   rn = clk;
   return rn;
}  /* End randinit */


double frac(double r)  /* Get fractional part of (double) real number */
{
   return r-floor(r);
}  /* End frac */


double rnd(void)  /* Generate a new pseudo-random number */
{
   rn=frac(rn*777.7);
   return rn;
}  /* End rnd */


double fnd(void)  /* Determine damage hit amount (distance-dependent) */
{
   return sqrt(pow(abs(k1[i]-s1),2.0)+pow(abs(k2[i]-s2),2.0));
}  /* End fnd */


void setrndxy(void)  /* Set a random x and y in interval [0,7] */
{
   x=rnd()*8.0;
   y=rnd()*8.0;
}  /* End setrndxy */


void findslot(void)  /* Find an unoccupied sector */
{
   while (TRUE)
   {
      setrndxy();
      if (sect[x][y]<2) break;
   }
}  /* End findslot */


int lt(double r1,double r2)  /* See if r1 is less than r2, BASIC style */
{
   if (r1<r2)
      return -1;  /* BASIC true = -1 */
   else
      return 0;  /* BASIC false = 0 */
}  /* End lt */


int gt(double r1,double r2)  /* See if r1 is greater than r2, BASIC style */
{
   if (r1>r2)
      return -1;  /* BASIC true = -1 */
   else
      return 0;  /* BASIC false = 0 */
}  /* End gt */


void init(void)  /* Initialize */
{
   randinit();
   fixdamage();
   setrndxy();
   q1=x;
   q2=y;
   x=8;
   y=1;
   x1=0.2075;
   _y1=6.28;
   x2=3.28;
   y2=1.8;
   aa=0.96;
   c=100;
   w=10;
   k9=0;
   b9=0;
   s9=400.0;
   t9=3451;
   t0=3421;
   t=t0;
   e0=4000.0;
   e=e0;
   p0=10;
   p=p0;
   for (i=0;i<8;i++)
      for (j=0;j<8;j++)
      {
         k=0;
         n=rnd();
         if (n<x1)
         {
            n*=64.0;
            k=lt(n,_y1)-y;
            k=k+lt(n,x2)+lt(n,y2)+lt(n,0.08)+lt(n,0.03)+lt(n,0.01);
            k9-=k;
         }
         b=gt(rnd(),aa);
         b9-=b;
         quad[i][j]=k*c+b*w-floor(rnd()*x+y);
      }
   if (k9>(t9-t0)) t9=t0+k9;
   if (b9<=0)
   {
      setrndxy();
      quad[x][y]-=10;
      b9=1;
   }
   k0=k9;
   cprintf("OBJECTIVE: DESTROY %d KLINGON BATTLE CRUISERS IN %d YEARS.\r\n",
      k9,t9-t0);
   cprintf(" THE NUMBER OF STARBASES IS %d.\r\n\n",b9);
}  /* End init */


void setupquad(void)  /* Set up quadrant when entering it */
{
   a=0;
   if ((q1<0)||(q1>7)||(q2<0)||(q2>7))
   {
      n=0;
      s=0;
      k=0;
   }
   else
   {
      n=abs(quad[q1][q2]);
      quad[q1][q2]=n;
      s=n-floor(n/10.0)*10.0;
      k=floor(n/100.0);
   }
   b=floor(n/10.0-k*10);
   setrndxy();
   s1=x;
   s2=y;
   for (i=0;i<8;i++)
      for (j=0;j<8;j++)
         sect[i][j]=1;
   sect[s1][s2]=2;
   for (i=0;i<8;i++)
   {
      k3[i]=0.0;
      x=8;
      if (i<k)
      {
         findslot();
         sect[x][y]=3;
         k3[i]=s9;
      }
      k1[i]=x;
      k2[i]=y;
   }
   i=s;
   if (b>0)
   {
      findslot();
      sect[x][y]=4;
   }
   while (i>0)
   {
      findslot();
      sect[x][y]=5;
      i--;
   }
}  /* End setupquad */


void checkcond(void)  /* Check condition */
{
   for (i=s1-1;i<=s1+1;i++)
      for (j=s2-1;j<=s2+1;j++)
         if ((i>=0)&&(i<=7)&&(j>=0)&&(j<=7))
            if (sect[i][j]==4)  /* Docked at starbase */
            {
               strcpy(cond,"DOCKED");
               e=e0;
               p=p0;
               fixdamage();
               return;
            }
   if (k>0)  /* Klingons present! */
      strcpy(cond,"RED");
   else if (e<(0.1*e0))  /* Low energy */
      strcpy(cond,"YELLOW");
   else  /* A-OK! */
      strcpy(cond,"GREEN");
   return;
}  /* End checkcond */


void showhit(void)  /* Show hit on Enterprise or Klingon */
{
   cprintf("%.3f UNIT HIT ON %s SECTOR %i - %i  (%.3f LEFT)\r\n",
      h,es,k1[i]+1,k2[i]+1,n);
}  /* End showhit */


void checkforhits(void)  /* Check for hits from Klingons */
{
   if (k<1) return;  /* No Klingons here! */
   if (strcmp(cond,"DOCKED")==0)
   {
      cprintf("STARBASE PROTECTS ENTERPRISE.\r\n");
      return;
   }
   for (i=0;i<8;i++)
      if (k3[i]>0.0)
      {
         h=k3[i]*0.4*rnd();
         k3[i]-=h;
         h/=pow(fnd(),0.4);
         e-=h;
         strcpy(es,"ENTERPRISE FROM");
         n=e;
         showhit();
      }
}  /* End checkforhits */


void showestreptime(void)  /* Show estimated time for repair */
{
   cprintf("%i YEARS ESTIMATED FOR REPAIR.\r\n\n",d[i]);
}  /* End showestreptime */


void showdamage(void)  /* Show damaged item */
{
   cprintf("%s DAMAGED.  ",ds[i]);
   beep();
   showestreptime();
}  /* End showdamage */


void srscan(void)  /* Do short-range scan */
{
   checkcond();
   if (a==0)  /* Initial entry into quadrant */
   {
      checkforhits();
      if (e<=0.0) return;  /* Ran out of energy! */
   }
   i=1;
   if (d[i]>0)  /* Short-range scan inoperative */
   {
      showdamage();
      return;
   }
   for (i=0;i<8;i++)
   {
      for (j=0;j<8;j++)
         cprintf("%c ",qs[sect[i][j]-1]);
      cprintf("  ");
      switch (i)
      {
         case 0 : cprintf("YEARS = %i\r\n",t9-t); break;
         case 1 : cprintf("STARDATE = %i\r\n",t); break;
         case 2 : cprintf("CONDITION: %s\r\n",cond); break;
         case 3 : cprintf("QUADRANT = %i - %i\r\n",q1+1,q2+1); break;
         case 4 : cprintf("SECTOR = %i - %i\r\n",s1+1,s2+1); break;
         case 5 : cprintf("ENERGY = %.3f\r\n",e); break;
         case 6 : cprintf("%s = %i\r\n",ds[4],p); break;
         case 7 : cprintf("KLINGONS LEFT = %i\r\n",k9); break;
      }
   }
}  /* End srscan */


void qstr(void)  /* Set up string for lr scan or galactic records */
{
   sprintf(es,"%3.3i",quad[i][j]);
}  /* End qstr */


void lrscan(void)  /* Do long-range scan */
{
   i=2;
   if (d[i]>0)  /* Long-range scan inoperative */
   {
      showdamage();
      return;
   }
   cprintf("%s FOR QUADRANT %i - %i\r\n",ds[i],q1+1,q2+1);
   for (i=q1-1;i<=q1+1;i++)
   {
      for (j=q2-1;j<=q2+1;j++)
      {
         cprintf("   ");
         if ((i<0)||(i>7)||(j<0)||(j>7))
            cprintf("***");
         else
         {
            quad[i][j]=abs(quad[i][j]);
            qstr();
            cprintf("%s",es);
         }
      }
      cprintf("\r\n");
   }
}  /* End lrscan */


void galrecs(void)  /* Do galactic records */
{
   i=5;
   if (d[i]>0)  /* Galactic records inoperative */
   {
      showdamage();
      return;
   }
   cprintf("CUMULATIVE GALACTIC MAP FOR STARDATE %i\r\n",t);
   for (i=0;i<8;i++)
   {
      for (j=0;j<8;j++)
      {
         cprintf("  ");
         if (quad[i][j]<0)
            cprintf("***");
         else
         {
            qstr();
            cprintf("%s",es);
         }
      }
      cprintf("\r\n");
   }
}  /* End galrecs */


int charokay(char cc,int mode)  /* Check for valid input characters */
{
int i,ok;

   i=mode;
   if ((i<0)||(i>3)) i=1;
   switch(i)
   {
      case 0 :
         ok=((cc>=' ')&&(cc<=ASCHI));
         break;
      case 1 :
         ok=(((cc>='A')&&(cc<='Z'))||(cc=='*')||(cc==' '));
         break;
      case 2 :
         ok=(((cc>='0')&&(cc<='9'))||(cc=='.')||(cc==',')||(cc=='-'));
         break;
      case 3 :
         ok=(((cc>='A')&&(cc<='Z'))||((cc>='0')&&(cc<='9'))||(cc==' '));
         break;
   }
   return ok;
}  /* End charokay */


void ctlbkspc(int *bl,char *buff)  /* Do a Ctrl-Backspace */
{
int
   cc;

   if (*bl>0)
   {
      for (cc=*bl;cc>0;cc--)
         cprintf("%c %c",BKSPCKEY,BKSPCKEY);
      *bl=0;
      buff[*bl]=NULLC;
   }
}  /* End ctlbkspc */


/*  ********************************************************************
    Function:     getinp
    Argument(s):  input buffer, maximum input length, mode
    Description:  Gets input from console, echoing to the current screen
                  position.  An attempt to enter more characters than the
                  maximum length will result in a beep at the terminal.
                  Backspace erases the most recent character.  Ctrl-
                  backspace erases the entire line.  An attempt to erase
                  non-existent characters will result in a buzz at the
                  terminal.  The mode determines what input will be
                  accepted:
                     mode = 0: all alphanumeric characters allowed
                     mode = 1: alphabetic, space, asterisk, function keys
                     mode = 2: digits, '-', ',', '.' only
                     mode = 3: alphabetic, space, and digits only
                  Note that for this program, all alphabetic charaters are
                  shifted to upper case.

                  If a mode other than 0, 1, 2 or 3 is passed, the default
                  mode is mode 0.  Function key F1 is treated like a CR only
                  in mode 2.
                  Other keys, like Alt- keys, etc., result in a beep at the
                  terminal.  Calls beep and buzz.
    Returns:      0 for successful read; 1 for CR only; -1 for ESC
    Includes:     conio.h
*/
int getinp(char *buff,int length,int mode)
{
int
   md,
   l,
   cc;

   md=mode;
   if ((mode<0)||(mode>3)) md=0;
   buff[0]=NULLC;
   l=0;
   cc=0;
   while ((cc!=ENTERKEY)&&(cc!=ESCKEY)&&(cc!=-ENTERKEY))
   {
      cc=getch();
      if (cc==NULLC)   /*  Function key hit.  */
      {
         cc=-getch();
         if (md==2)
            switch (cc)
            {
               case F1KEY:  /*  F1;  Help - same as blank CR  */
                  if (l>0)
                     ctlbkspc(&l,buff);
                  cc=ENTERKEY;
                  break;
               default : buzz();
            }
         else
            buzz();
      }
      else if ((cc<BKSPCKEY)||((cc>BKSPCKEY)&&(cc<ENTERKEY))||
         ((cc>ENTERKEY)&&(cc<ESCKEY))||((cc>ESCKEY)&&(cc<SPC))||(cc>ASCHI))
         beep();
      else
         switch(cc)
         {
         case BKSPCKEY :  /*  Perform destructive backspace.  */
            if (l>0)
            {
               cprintf("%c %c",cc,cc);
               l--;
               buff[l]=NULLC;
            }
            else
               buzz();
            break;
         case ESCKEY :   /*  Perform escape (abort input).  */
            if(l>0)
               ctlbkspc(&l,buff);
            break;
         case CTLBKSPCKEY :   /*  Perform Ctrl-backspace.  */
            if (l>0)
               ctlbkspc(&l,buff);
            else
               buzz();
            break;
         case ENTERKEY :  /*  Carriage return -- don't do anything  */
            break;
         default :  /*  Possibly valid ASCII character  */
            if (l<length)
            {
               cc=toupper(cc);
               if (charokay(cc,md))
               {
                  buff[l]=cc;
                  cprintf("%c",cc);
                  l++;
                  buff[l]=NULLC;
               }
               else
                  beep();
            }
            else
               beep();
            break;
         }
   }
   if (cc==ESCKEY) return -1;
   else if (l==0) return 1;
   else return 0;
}  /* End getinp */


void getcourse(void)  /* Gets course and places in variable c */
{
char ibuff[5];
int gb;

   cprintf("COURSE (1-8.99)? ");
   gb=getinp(ibuff,4,2);
   cprintf("\r\n");
   if (gb!=0)
      c=0.0;
   else
      c=atof(ibuff);
}  /* End getcourse */


void getwarp(void)  /* Gets warp and places in variable w */
{
char ibuff[5];
int gb;

   cprintf("WARP (0-12.0)? ");
   gb=getinp(ibuff,4,2);
   cprintf("\r\n");
   if (gb!=0)
      w=0.0;
   else
      w=atof(ibuff);
}  /* End getwarp */


void dopath(void)  /* Do the path for warp or torpedo */
{
int inquad,shortmove;

   _y1=s1+0.5;
   x1=s2+0.5;
   y3=(c-1)*0.785398;
   x3=cos(y3);
   y3=-sin(y3);
   inquad=TRUE;
   if (a==1)
      shortmove=TRUE;
   else
      shortmove=FALSE;
   for (i=0;i<=n;i++)
   {
      _y1+=y3;
      x1+=x3;
      y2=floor(_y1);
      x2=floor(x1);
      y7=y2;
      x7=x2;
      if ((x7<0)||(x7>7)||(y7<0)||(y7>7))
      {
         inquad=FALSE;
         shortmove=FALSE;
         break;
      }
      if (a==5)  /* Show torpedo track */
         cprintf("%i - %i  ",y7+1,x7+1);
      if (sect[y7][x7]!=1)  /* Object blocking move or hit by torpedo */
      {
         shortmove=FALSE;
         break;
      }
   }
   if (inquad)  /* Still in quadrant -- short move, block, or torpedo hit */
   {
      newquad=FALSE;
      cprintf("\r\n");
      if (!shortmove)
      {
         if (a==1) cprintf("BLOCKED BY ");
         switch (sect[y7][x7])
         {
            case 3 :  /* Klingon */
               cprintf("KLINGON");
               if (a==5)  /* Torpedo */
               {
                  for (i=0;i<8;i++)
                     if ((y7==k1[i])&&(x7==k2[i])) k3[i]=0.0;
                  k--;
                  k9--;
               }
               break;
            case 4 :  /* Starbase */
               cprintf("STARBASE");
               if (a==5) b=2;  /* Torpedo */
               break;
            case 5 :  /* Star */
               cprintf("STAR");
               if (a==5) s--;  /* Torpedo */
               break;
         }
         if (a==1)  /* Enterprise move */
         {
            cprintf(" AT SECTOR %i - %i\r\n",y7+1,x7+1);
            y2=floor(_y1-y3);
            x2=floor(x1-x3);
            y7=y2;
            x7=x2;
         }
      }
      if (a==1)
      {
         s1=y2;
         s2=x2;
         sect[s1][s2]=2;
         a=2;  /* Flag to show we stayed within quadrant */
      }
      if (a==5)  /* Torpedo */
      {
         cprintf(" DESTROYED!");
         if (b==2)
         {
            b=0;
            cprintf(" . . . GOOD WORK!");
         }
         cprintf("\r\n");
         sect[y7][x7]=1;
         quad[q1][q2]=k*100+b*10+s;
      }
   }
   else  /* Out of quadrant -- move to new quadrant or torpedo miss */
   {
      if (a==1)  /* Move */
      {
         newquad=TRUE;
         q1=floor(q1+w*y3+(s1+0.5)/8);
         q2=floor(q2+w*x3+(s2+0.5)/8);
         q1=q1-lt(q1,0)+gt(q1,7);
         q2=q2-lt(q2,0)+gt(q2,7);
      }
      if (a==5)  /* Torpedo */
         cprintf("MISSED!\r\n");
   }
}  /* End dopath */


void phasers(void)  /* Fire phasers */
{
char ibuff[16];
int gb;

   x=0.0;
   i=3;
   if (d[i]>0)  /* Phasers inoperative */
   {
      showdamage();
      return;
   }
   while (TRUE)
   {
      cprintf("PHASERS READY: ENERGY UNITS TO FIRE? ");
      gb=getinp(ibuff,15,2);
      cprintf("\r\n");
      if (gb!=0)
      {
         x=0.0;
         break;
      }
      else
         x=atof(ibuff);
      if (x<=e) break;
      cprintf("ONLY GOT %.3f\r\n",e);
   }
   e-=x;
   y3=k;
   for (i=0;i<8;i++)
      if (k3[i]>0.0)
      {
         h=x/(y3*pow(fnd(),0.4));
         k3[i]-=h;
         strcpy(es,"KLINGON AT");
         n=k3[i];
         showhit();
         if (k3[i]<=0.0)
         {
            cprintf("**KLINGON DESTROYED**\r\n");
            k--;
            k9--;
            sect[k1[i]][k2[i]]=1;
            quad[q1][q2]-=100;
         }
      }
}  /* End phasers */


void showstardate(void)  /* Display current star date */
{
   cprintf("\r\nIT IS STARDATE %i.\r\n",t);
}  /* End showstardate */




void main(void)
{
int gamecomp;  /* 0 = in progress; 1 = won; -1 = lost; -99 = quit */
int alldone,rating,moved;
double drate;

   title();
   cprintf("DO YOU NEED INSTRUCTIONS Y/N)? ");
   yesno();
   clrscr();
   if (ans=='Y') showinst();
   alldone=FALSE;
   while (! alldone)
   {
      clrscr();
      title();
      init();
      gamecomp=FALSE;
      newquad=TRUE;
      while (!gamecomp)
      {
         if (newquad) setupquad();
         newquad=FALSE;
         moved=FALSE;
         srscan();
         if (e<=0.0)  /* Ran out of energy */
            gamecomp=-1;
         else
         {
            while (TRUE)  /* Command loop (-99 or ESC to quit) */
            {
               cprintf("COMMAND? ");
               a=getinp(cmdbuff,7,2);
               cprintf("\r\n");
               if (a==1) a=99;
               if (a==-1) a=-99;
               if (a==0) a=atoi(cmdbuff);
               if (a==-99)
               {
                  cprintf("\r\nARE YOU SURE YOU WANT TO QUIT? ");
                  yesno();
                  if (ans=='Y')
                  {
                     gamecomp=-99;
                     break;  /* Break out of command loop */
                  }
                  else
                     continue;  /* Back to top of command loop */
               }
               if ((a<1)||(a>6))
               {
                  for (i=0;i<6;i++)
                     cprintf("  %i = %s\r\n",i+1,ds[i]);
                  cprintf("  -99 OR ESC TO QUIT\r\n\n");
                  continue;  /* Back to top of command loop */
               }
               switch (a)
               {
                  case 1 :  /* Warp engines */
                     while (TRUE)
                     {
                        while (TRUE)
                        {
                           getcourse();
                           if (c<9.0) break;
                           beep();
                        }
                        if (c>=1.0)
                           while (TRUE)
                           {
                              getwarp();
                              if ((w<=0.0)||(w>12.0))
                              {
                                 c=10.0;
                                 break;
                              }
                              if ((d[0]>0)&&(w>0.2))
                              {
                                 i=0;
                                 cprintf("%s DAMAGED; MAX IS 0.2; ",ds[i]);
                                 showestreptime();
                                 beep();
                              }
                              else
                                 break;
                              beep();
                           }
                        if (c<9.0) break;
                     }
                     if (c<1.0) break;  /* Abort move */
                     checkforhits();
                     if (e<=0.0)  /* Ran out of energy */
                     {
                        gamecomp=-1;
                        break;
                     }
                     if (rnd()<=0.25)
                     {
                        x=floor(rnd()*6.0);
                        if (rnd()<=0.5)
                        {
                           beep();
                           d[x]+=floor(6.0-rnd()*5.0);
                           cprintf("**SPACE STORM, %s DAMAGED**\r\n",ds[x]);
                           i=x;
                           showestreptime();
                           d[x]++;
                           delay(100);
                           beep();
                        }
                        else
                        {
                           j=-1;
                           for (i=x;i<6;i++)
                              if (d[i]>0)
                              {
                                 j=i;
                                 break;
                              }
                           if (j<0)
                              for (i=0;i<x;i++)
                                 if (d[i]>0)
                                 {
                                    j=i;
                                    break;
                                 }
                           if (j>=0)
                           {
                              d[j]=1;
                              cprintf("**SPOCK USED A NEW REPAIR TECHNIQUE**\r\n");
                           }
                        }
                     }
                     for (i=0;i<6;i++)
                        if (d[i]!=0)
                        {
                           d[i]--;
                           if (d[i]<=0)
                           {
                              d[i]=0;
                              cprintf("%s ARE FIXED!\r\n",ds[i]);
                              beep();
                           }
                        }
                     n=floor(w*8.0);
                     e=e-n-n+0.5;
                     t++;
                     sect[s1][s2]=1;
                     if (t>t9)  /* Ran out of time! */
                     {
                        gamecomp=-1;
                        break;
                     }
                     dopath();
                     if (e<=0.0)  /* Ran out of energy */
                     {
                        gamecomp=-1;
                        break;
                     }
                     moved=TRUE;
                     break;
                  
                  case 2 :  /* Short-range scan */
                     srscan();
                     break;
                  
                  case 3 :  /* Long-range scan */
                     lrscan();
                     break;
                  
                  case 4 :  /* Phasers */
                     phasers();
                     if (x>0.0)
                     {
                        if (e<=0.0) gamecomp=-1;  /* Ran out of energy */
                        checkforhits();
                        if (e<=0.0) gamecomp=-1;  /* Ran out of energy */
                        if (k9<1) gamecomp=1;  /* All Klingons destroyed! */
                        if (!gamecomp) checkcond();
                     }
                     break;
                  
                  case 5 :  /* Photon torpedos */
                     if (d[4]>0)  /* Torpedoes damaged */
                     {
                        cprintf("SPACE CRUD BLOCKING TUBES.  ");
                        i=4;
                        showestreptime();
                        beep();
                        break;
                     }
                     n=15;
                     if (p<1)
                     {
                        cprintf("NO TORPEDOES LEFT!\r\n");
                        break;
                     }
                     c=10.0;
                     while (c>=9.0)
                     {
                        cprintf("TORPEDO ");
                        getcourse();
                     }
                     if (c<1.0) break;  /* Abort firing of torpedo */
                     p--;
                     cprintf("TRACK: ");
                     dopath();
                     if (e<=0.0) gamecomp=-1;  /* Ran out of energy */
                     checkforhits();
                     if (e<=0.0) gamecomp=-1;  /* Ran out of energy */
                     if (k9<1) gamecomp=1;  /* All Klingons destroyed! */
                     if (!gamecomp) checkcond();
                     break;
                  
                  case 6 :  /* Galactic records */
                     galrecs();
                     break;
               }
               if (gamecomp) break;
               if (moved) break;  /* Enterprise moved */
            }  /* End command loop */
         }
      }  /* Game is over! */
      showstardate();
      switch (gamecomp)
      {
         case 1 :
            drate=t-t0;
            rating=(k0/drate)*1000.0;
            cprintf("THE FEDERATION HAS BEEN SAVED!\r\n");
            cprintf("YOU ARE PROMOTED TO ADMIRAL.\r\n");
            cprintf("%i KLINGONS IN %i YEARS.  RATING = %i\r\n\n",
               k0,t-t0,rating);
            break;
         case -1 :
            if (t>t9)
               cprintf("YOU RAN OUT OF TIME!\r\n");
            if (e<=0.0)
               cprintf("YOU RAN OUT OF ENERGY!\r\n");
            cprintf("THANKS TO YOUR BUNGLING, THE FEDERATION WILL BE\r\n");
            cprintf("CONQUERED BY THE REMAINING %i KLINGON CRUISERS!\r\n",k9);
            cprintf("YOU ARE DEMOTED TO CABIN BOY!\r\n");
            break;
         case -99 :
            cprintf("OKAY, QUITTER -- NO KUDOS FOR YOU.\r\n");
            break;
      }
      cprintf("\r\nTRY AGAIN? ");
      yesno();
      if (ans=='N') alldone=1;
   }
}  /* End main */
