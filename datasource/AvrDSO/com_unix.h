/**
 * Bootloader um dem Mikrocontroller Bootloader von Peter Dannegger anzusteuern
 *
 * License: GPL
 *
 * @author Andreas Butti
 */

#ifndef COM_UNIX_H_INCLUDED
#define COM_UNIX_H_INCLUDED

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

/// Prototypes

/**
 * Set flag for one-wire local echo
 *
 */
void com_localecho ();

/**
 * Opens com port
 *
 * @return true if successfull
 */
extern char com_open(const char * device, unsigned long baud);

/**
 * Close com port and restore settings
 */
void com_close();

/**
 * Sends one char
 */
void com_putc_fast(unsigned char c);
void com_putc(unsigned char c);

/**
 * Recives one char or -1 if timeout
 */
int com_getc(int timeout);


/**
 * Sending a command
 */
void com_puts(const char *s);

#endif //COM_H_INCLUDED
