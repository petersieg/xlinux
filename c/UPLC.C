/* micro s5-like simulator
   (c) 1998 Peter Sieg
   usage: s5 filename [<debug_level>]
   debug_level not defined = 0;
   debug_level = 0 = no output
                 1 = only info at first aw
                 2 = info at every aw
                 3 = like 2 plus loading of awl
*/
#include <stdio.h>

#define MAXINT 32767
#define MAXAWL 100
#define MAXEAMTZ 2
#define MYPORT 0x3bc

/* used for loops etc. */
int n1,n2,debug,port;

/* hold boolean results 0 = false, 1 = true */
int b_vka,b_vkb,b_vkl,b_vku;
int _b_vka,_b_vkb,_b_vkl,_b_vku;

/* hold awl, max 100 aw's */
char s_operation[MAXAWL][5];
char s_operand[MAXAWL][5];
int i_baustein[MAXAWL];
int i_bitnr[MAXAWL];

/* holds actual anweisung and total count of anweisungen */
int anz_aw,act_aw;

/* array for E/A/M/T/Z - only 0,0-7 is used so far */
int e[MAXEAMTZ][8], a[MAXEAMTZ][8], m[MAXEAMTZ][8];

/* holds date (yy=year,yn=month,yd=day) and time (yh=hour,ym=minute,ys=second)
*/
/* yw = week of year (not implemented yet); yo day Of week 0-6 */
/* attention: year is only 2 digits! (millenium change) */
int yy,yn,yw,yd,yo,yh,ym,ys;


void get_time()
{
  int month,year,jhd;
  char buf[10];
/*dates(buf);*/
  sprintf(buf,"%s","12-26-63");
/*printf("%s\n",buf);*/
  sscanf(buf,"%d-%d-%d",&yn,&yd,&yy); /* month-day-year(2-digits-only) */
/*printf("%d=%d=%d\n",yn,yd,yy);*/
/*times(buf);*/
  sprintf(buf,"%s","01:02:03");
/*printf("%s\n",buf);*/
  sscanf(buf,"%d:%d:%d",&yh,&ym,&ys); /* hour:minute:second */
/*printf("%d-%d-%d\n",yh,ym,ys);*/

/* now calc day of week with zeller algorythmus */
  year = yy;
  month= yn;
  if (year < 98) year = year + 2000; /* millenium hack */
  else year = year + 1900;
  if (month > 2) month -= 2;
  else
  {
    month += 10;
    year--;
  }
  jhd = year / 100;
  year %= 100;
  yo = (year / 4 + jhd / 4 + (13 * month-1) / 5 + yd + year - 2 * jhd) % 7;
  if (yo < 0) yo += 7;
/* now calc week of year - not correct, but simple substitution! */
  yw = (((yn - 1) * 30 + yd) / 7) + 1; /* (--month * 30 (days) + days) / 7 + 1*/
}


/* input m[0][3-7] from and output a[0][0-7] to printer port */
int pow2(i)
int i;
{
  int n = 1;
  int j;
  for (j=0; j<i; j++)
    n = n * 2;
  return n;
}

void in_out()
{
  char c;
  int  i;
/* output */
  i = 0;
  for (n1=0; n1<=7; n1++)
  {
    i = i + (a[0][n1] * pow2(n1)); /* assume a[0][*] can only be 0 or 1! */
/*  printf("n1=%d/i=%d/pow2(n1)=%d\n",n1,i,pow2(n1)); */
  }
  c = i;
/*printf("i=%d/c=%d/c=%c\n",i,c,c); */
/*outportb(port,c);*/

/* input  */
  /*c = inportb(port+1); * >> 4);*/
  c = 0;
  for (n1=7; n1>=3; n1--)
  {
    if (c>=pow2(n1))
    {
      m[0][n1] = 1;
      c = c - pow2(n1);
    }
    else m[0][n1] = 0;
/*  printf("c=%d/pow2(n1)=%d/m[0][n1]=%d\n",c,pow2(n1),m[0][n1]); */
  }
/* debug
  for (n1=0; n1<=7; n1++)
    printf("m[0][%d]=%d\n",n1,m[0][n1]);
*/
}

void init()
{
  int i,j;
  for (i=0; i<=(MAXEAMTZ-1); i++)
    for (j=0; j<=7; j++)
    {
      e[i][j]=0; a[i][j]=0; m[i][j]=0;
    }
  b_vku = 1;
  b_vkl = 1;
  b_vka = 0;
  b_vkb = 0;
  _b_vku = 1;
  _b_vkl = 1;
  _b_vka = 0;
  _b_vkb = 0;
}

void print_info()
{
  int i;
  printf("%3d: %2s %2s %2d.%d  ",act_aw,s_operation[act_aw],s_operand[act_aw]
         ,i_baustein[act_aw],i_bitnr[act_aw]);
  printf("V%1d%1d%1d%1d  ",_b_vku,_b_vkl,_b_vka,_b_vkb);
  printf(">%1d%1d%1d%1d  ",b_vku,b_vkl,b_vka,b_vkb);

  printf("E%1d%1d%1d%1d%1d%1d%1d%1d",e[0][0],e[0][1],e[0][2],e[0][3],e[0][4],e[0][5],e[0][6],e[0][7]);
  printf("A%1d%1d%1d%1d%1d%1d%1d%1d",a[0][0],a[0][1],a[0][2],a[0][3],a[0][4],a[0][5],a[0][6],a[0][7]);
printf("M%1d%1d%1d%1d%1d%1d%1d%1d\n",m[0][0],m[0][1],m[0][2],m[0][3],m[0][4],m[0][5],m[0][6],m[0][7]);
  _b_vku = b_vku;
  _b_vkl = b_vkl;
  _b_vka = b_vka;
  _b_vkb = b_vkb;
  i = atoi(gets(stdin)); /* simple toggle for e[0][i] if i = 0-7 */
  if ((i >= 1) && (i <= 7))
  {
    if  (e[0][i] == 0) e[0][i] = 1;
    else e[0][i] = 0;
  }
}

void step_aw(i)
int i;
{
/* first update b_vka = status of e,a,m,t,z [baustein][bitnr] */
/* for yh,ym,ys:b_vka = true if yh,ym,ys if greater than baustein */
  b_vka=0;
  if (strcmp(s_operand[i],"E") == 0) b_vka=e[i_baustein[i]][i_bitnr[i]];
  if (strcmp(s_operand[i],"A") == 0) b_vka=a[i_baustein[i]][i_bitnr[i]];
  if (strcmp(s_operand[i],"M") == 0) b_vka=m[i_baustein[i]][i_bitnr[i]];
  if (strcmp(s_operand[i],"YY")== 0) if (yy > i_baustein[i]) b_vka = 1;
  if (strcmp(s_operand[i],"YN")== 0) if (yn > i_baustein[i]) b_vka = 1;
  if (strcmp(s_operand[i],"YW")== 0) if (yw > i_baustein[i]) b_vka = 1;
  if (strcmp(s_operand[i],"YD")== 0) if (yd > i_baustein[i]) b_vka = 1;
  if (strcmp(s_operand[i],"YO")== 0) if (yo > i_baustein[i]) b_vka = 1;
  if (strcmp(s_operand[i],"YH")== 0) if (yh > i_baustein[i]) b_vka = 1;
  if (strcmp(s_operand[i],"YM")== 0) if (ym > i_baustein[i]) b_vka = 1;
  if (strcmp(s_operand[i],"YS")== 0) if (ys > i_baustein[i]) b_vka = 1;

/* second update b_vkb */
  b_vkb=0;
  if (strcmp(s_operation[i],"U" ) == 0)
    if ((b_vka == 1) && ((b_vkl == 1) || (b_vku == 1))) b_vkb=1;

  if (strcmp(s_operation[i],"UN") == 0)
    if ((b_vka == 0) && ((b_vkl == 1) || (b_vku == 1))) b_vkb=1;

  if (strcmp(s_operation[i],"O" ) == 0)
    if ((b_vka == 1) || ((b_vkl == 1) || (b_vku == 1))) b_vkb=1;

  if (strcmp(s_operation[i],"ON") == 0)
    if ((b_vka == 1) || ((b_vkl == 1) || (b_vku == 1))) b_vkb=1;

/* now set operand to the value of b_vkl */
  if (strcmp(s_operation[i],"=" ) == 0)
  {
    if (strcmp(s_operand[i],"E") == 0) e[i_baustein[i]][i_bitnr[i]] = b_vkl;
    if (strcmp(s_operand[i],"A") == 0) a[i_baustein[i]][i_bitnr[i]] = b_vkl;
    if (strcmp(s_operand[i],"M") == 0) m[i_baustein[i]][i_bitnr[i]] = b_vkl;
    b_vkb = 1; /* allow b_vkl to be set to 1! */
/*  b_vkb = b_vkl; /* allow storing to next b_vkl! <-- wrong! */
  }

/* S = Set E/A/M permanent or R = Reset E/A/M permanent */
  if (b_vkl == 1)
  {
    if (strcmp(s_operation[i],"R" ) == 0)
    {
      if (strcmp(s_operand[i],"E" ) == 0)
      {
        e[i_baustein[i]][i_bitnr[i]] = 0;
      }
      if (strcmp(s_operand[i],"A" ) == 0)
      {
        a[i_baustein[i]][i_bitnr[i]] = 0;
      }
      if (strcmp(s_operand[i],"M" ) == 0)
      {
        m[i_baustein[i]][i_bitnr[i]] = 0;
      }
      b_vkb = 1; /* allow b_vkl to be set to 1! */
    }

    if (strcmp(s_operation[i],"S" ) == 0)
    {
      if (strcmp(s_operand[i],"E" ) == 0)
      {
        e[i_baustein[i]][i_bitnr[i]] = 1;
      }
      if (strcmp(s_operand[i],"A" ) == 0)
      {
        a[i_baustein[i]][i_bitnr[i]] = 1;
      }
      if (strcmp(s_operand[i],"M" ) == 0)
      {
        m[i_baustein[i]][i_bitnr[i]] = 1;
      }
      b_vkb = 1; /* allow b_vkl to be set to 1! */
    }
  }
  else b_vkb = 1;

/* now store last b_vkb in b_vkl */
  b_vkl = b_vkb;

/* set b_vku = 1 if s_operation = "U" || "UN" */
  if ((strcmp(s_operation[i],"U") == 0) || (strcmp(s_operation[i],"UN") == 0))
    b_vku = 1;
  else
    b_vku = 0;

/* if "BE" set b_vkl + b_vku = 1 */
  if (strcmp(s_operation[i],"BE") == 0)
  {
    b_vku = 1;
    b_vkl = 1;
  }
}

void run_awl()
{
  if (anz_aw >= 1)
  {
    while (1==1)
    {
      step_aw(act_aw);
      if  (debug >= 2) print_info();
      if ((debug == 1) && (act_aw == 1)) print_info();
      act_aw++;
      if (act_aw > anz_aw)
      {
        act_aw=1;
        in_out();   /* output a[0][0-7] to port and get e[0][0-7] from port+1 */
        get_time(); /* update vars yh,ym,ys with current time */
      }
    }
  }
}

main(argc,argv)
int  argc;
char *argv[];
{
  FILE *fp;
  char buf[256],s1[5],s2[5];
  int i1,i2;
  int istop;

  if (argc < 2)
  {
    puts("Missing Filename for AWL File");
    exit(1);
  }

  debug = 1;
  if (argc >= 3)
  {
    debug=atoi(argv[2]);
    if (debug >= 200)
    {
      debug = 0; /* run without output, when utilized with io interface */
      sscanf(argv[2],"%x",&port); /* debug field means port# in hex! */
      printf("Port set to 0x%x\n",port); /* show port number */
    }
    else if ((debug < 0) || (debug > 3)) debug = 1;
    if (port < 200) port = 0;
  }

  if ((fp=fopen(argv[1],"r")) == NULL)
  {
    puts("Cannot Open ");
    puts(argv[1]);
    exit(2);
  }

/* here file is open to read in awl */
  anz_aw=0;
  act_aw=1;
  istop=0;

  while ((istop == 0) && (fgets(buf,256,fp) != NULL))
  {
    anz_aw++;
    sscanf(buf,"%s %s %d %d",&s1,&s2,&i1,&i2);
/* debug only */
    if (debug == 3)
    {
      printf("%3d --> %s %s %d %d\n",anz_aw,s1,s2,i1,i2);
      gets(stdin);
    }

/* only read up to MAXAWL lines or until BE */
    if ((anz_aw >= MAXAWL) || (strcmp(s1,"BE") == 0))
    {
      istop=1;
      strcpy(s1,"BE");
    }

/* syntax check here */
    if ((i2 < 0) || (i2 > 7)) /* bitnr must be between 0 and 7 */
    {
      printf("%s\nError in AWL file %s at line %d - wrong bitnr. Must be 0-7.",buf,argv[1],anz_aw);
      exit(3);
    }
    if (i1 < 0)
    {
      printf("%s\nError in AWL file %s at line %d - baustein negative.",buf,argv[1],anz_aw);
      exit(3);
    }
    if (i1 >= MAXEAMTZ)
    {
      if ((strcmp(s2,"E") == 0) ||
          (strcmp(s2,"A") == 0) ||
          (strcmp(s2,"M") == 0))
      {
      printf("%s\nError in AWL file %s at line %d - baustein over max=%d.",buf,argv[1],anz_aw,MAXEAMTZ-1);
      exit(3);
      }

    }

/* store aw in awl array */
    strcpy(s_operation[anz_aw],s1);
    strcpy(s_operand[anz_aw],s2);
    i_baustein[anz_aw]=i1;
    i_bitnr[anz_aw]=i2;

/* debug only */
    if (debug == 3)
    {
      printf("%3d ==> %s %s %d %d\n",anz_aw,s_operation[anz_aw],s_operand[anz_aw],i_baustein[anz_aw],i_bitnr[anz_aw]);
      gets(stdin);
    }

  }
  fclose(fp);

/* debug only */
  if (debug == 3)
  {
    printf("==> anz_aw = %d\n",anz_aw);
    for (i1=1; i1<=anz_aw; i1++)
    {
      printf("%3d ==> %s %s %d %d\n",i1,s_operation[i1],s_operand[i1],i_baustein[i1],i_bitnr[i1]);
      gets(stdin);
    }
  }

/* init variable e,a,m,z,t */
  init();

/* run */
  run_awl();

/* exit program */
  exit(0);
}


