/* gcc -O2 onlight.c -o onlight */

#include <stdio.h>
#include "port.h"
main()
{
   if (ioperm(0x378,1,1))
   {
      fprintf(stderr,"ioperm error.\n");
      exit(1);
   }
   /* use 0x378,0 to switch off */
   port_out(0x378,255);	
   fprintf(stderr,"The Port Status : %i\n",port_in(0x378));
   exit(0); 
  }
