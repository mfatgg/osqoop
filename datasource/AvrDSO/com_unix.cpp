//! Library for serial (com) port access for unix-like systems, e.g. Linux.
//! Support for arbitrary baudrates up to 3 MBaud (e.g. for usb serial converter).
//!
//! Slightly based on implementation by @author Andreas Butti.


/// Includes
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/times.h>

#include "com_unix.h"


/// Attributes

// Device
int fd;
// Old settings
struct termios oldtio;

int sendCount = 0;


typedef struct
{
    unsigned long   baudValue;
    speed_t         baudConst;
} baudInfo_t;
baudInfo_t baudRates[] = {
    {     50,     B50 },
    {     75,     B75 },
    {    110,    B110 },
    {    134,    B134 },
    {    150,    B150 },
    {    200,    B200 },
    {    300,    B300 },
    {    600,    B600 },
    {   1200,   B1200 },
    {   1800,   B1800 },
    {   2400,   B2400 },
    {   4800,   B4800 },
    {   9600,   B9600 },
    {  19200,  B19200 },
    {  38400,  B38400 },
    {  57600,  B57600 },
    { 115200, B115200 },
    { 230400, B230400 },
    { 460800, B460800 },
    { 921600, B921600 },
    { 500000, B500000 },
    {1000000,B1000000 },
    {2000000,B2000000 },
    {3000000,B3000000 },
};


/**
 * Set flag for one-wire local echo
 *
 */
void com_localecho ()
{
    sendCount = 1;
}


/**
 * Opens com port
 *
 * @return true if successfull
 */
char com_open(const char * device, unsigned long realbaud)
{
    speed_t baud;
    unsigned int baudid, i;
    struct termios newtio;

   // Checking baudrate
    for(i = 0; i < (sizeof (baudRates) / sizeof (baudInfo_t)); i++)
    {
        if (baudRates[i].baudValue == realbaud)
        {
            baudid = i;
            break;
        }
    }
    baud = baudRates[baudid].baudConst;

    // Open the device
    fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0)
    {
        return 0;
    }

    // Save old settings
    tcgetattr(fd, &oldtio);

    // Init memory
    memset(&newtio, 0x00 , sizeof(newtio));

    // settings flags
    newtio.c_cflag = CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR | IGNBRK;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;

    cfmakeraw(&newtio);

    // Timeout in 100ms
    newtio.c_cc[VTIME] = 0;
    // read 1 character
    newtio.c_cc[VMIN] = 0;

    // Setting baudrate
    cfsetispeed(&newtio, baud);

    // Flushing buffer
    tcflush(fd, TCIOFLUSH);

    // aplying new configuration
    tcsetattr(fd, TCSANOW, &newtio);

    sendCount = 0;

    return 1;
}


/**
 * Close com port and restore settings
 */
void com_close()
{
    tcdrain(fd);

    // restore old settings
    tcsetattr(fd, TCSANOW, &oldtio);

    // close device
    close(fd);
}

/**
 * Receives one char or -1 if timeout
 * timeout in 10th of seconds
 */
int com_getc(int timeout)
{
    static long         ticks = 0;
    static struct tms   theTimes;
    char    c;
    clock_t t = times (&theTimes);

    if (ticks == 0)
        ticks = sysconf(_SC_CLK_TCK) / 10;

    do
    {
        if (read(fd, &c, 1))
        {
            if (sendCount > 1)
            {
                sendCount--;
                t = times (&theTimes);
                continue;
            }
            return (unsigned char)c;
        }
    } while ( ((times (&theTimes) - t )/ticks) < timeout );

    return -1;
}


/**
 * Sends one char
 */
void com_putc_fast(unsigned char c)
{
    if (sendCount)
    {
        if (sendCount > 1)
            com_getc(0);
        sendCount++;
    }

    write(fd, &c, 1);
}

void com_putc(unsigned char c)
{
    tcdrain(fd);
    com_putc_fast (c);
}


/**
 * Sending a command
 */
void com_puts(const char *s)
{
    const char *tmp;

    if (sendCount)
        sendCount = 1;
    tmp = s;
    while (*tmp)
    {
        com_putc(*tmp++);
        usleep (100);
    }
    tcdrain(fd);
}


