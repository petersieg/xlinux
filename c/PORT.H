/************** begin of port.h *****************/
static inline int port_in( int port )
{
   unsigned char value;
  __asm__ volatile ("inb %1,%0"
                    : "=a" (value)
                    : "d" ((unsigned short)port));
   return value;
}

static inline void port_out( unsigned short int port, unsigned char val )
{
  __asm__ volatile (
                    "outb %0,%1\n"
                    :
                    : "a" (val), "d" (port)
                    );
}
