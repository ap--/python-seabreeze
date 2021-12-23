/***************************************************//**
 * @file    NativeRS232POSIX.c
 * @date    February 2012
 * @author  Ocean Optics, Inc.
 *
 * This is a native implementation of the RS232 interface
 * for POSIX.  This has been tested under both Linux
 * and MacOSX.
 *
 * LICENSE:
 *
 * SeaBreeze Copyright (C) 2014, Ocean Optics Inc
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *******************************************************/

#include "common/globals.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <termios.h>
#include <sys/time.h>
#include <stdlib.h>
#include "native/rs232/NativeRS232.h"
#include "api/seabreezeapi/SeaBreezeAPIConstants.h"

/* Local struct definitions */
struct __rs232_handle {
  int fd;
  int opened;
  int verbose;
};

struct __rs232_baud_map_entry {
    unsigned int bps;
    unsigned int code;
};

/* Local prototypes */
int __rs232_get_closest_baud_index(unsigned int target_baud);
void __xdump(void *x, int length);
void __rs232_xdump(void *x, int length, int is_read);
void __rs232_desc_xfer(int length, int is_read);
void __rs232_initialize_uart(struct __rs232_handle *desc);
static int __rs232_set_baud(void *desc, unsigned int baud_bps);

/* Static private variables */
static struct __rs232_baud_map_entry __rs232_baud_table[] = {
    /* This maps actual baud rate (int) to termbits.h value */
    {0,     B0},    /* Drops DTR */
    {50,    B50},
    {75,    B75},
    {110,   B110},
    {134,   B134},
    {150,   B150},
    {200,   B200},
    {300,   B300},
    {600,   B600},
    {1200,  B1200},
    {1800,  B1800},
    {2400,  B2400},
    {4800,  B4800},
    {9600,  B9600},
    {19200, B19200},
    {38400, B38400},
    {57600, B57600},
    {115200, B115200},
    {230400, B230400},
#ifdef B460800
    {460800, B460800}
#endif
};

static int __rs232_baud_table_length =
        sizeof(__rs232_baud_table) / sizeof(struct __rs232_baud_map_entry);

#ifdef __clang__
#define ABS
#else
#define ABS abs
#endif

int __rs232_get_closest_baud_index(unsigned int target_baud) {
    int i;
    int best_delta;
    int best_index;
    int delta;

    if(__rs232_baud_table_length < 1) {
        fprintf(stderr, "Internal error: baud rate table not properly defined.\n");
        return -1;
    }

    //best_delta = ABS(target_baud - __rs232_baud_table[0].bps);

	// the compiler flags and error for abs(unsigned var1- unsigned var2)
    best_delta = target_baud - __rs232_baud_table[0].bps;
    best_delta= abs(best_delta);

    best_index = 0;
    for(i = 1; i < __rs232_baud_table_length; i++) {
        //delta = ABS(target_baud - __rs232_baud_table[i].bps);
        delta = target_baud - __rs232_baud_table[i].bps;
        delta= abs(delta);

        if(delta < best_delta) {
            best_index = i;
            best_delta = delta;
        }
    }
    return best_index;
}

void __xdump(void *x, int length) {
    int i, j;
    int written;
    char buf[256];

    for(i = 0; i < (length/16) + 1; i++) {
        written = 0;
        written += sprintf(buf + written, "[%04X]: ", (i*16 & 0x00FFFF));
        for(j = 0; j < 16 && (j + (i*16) < length); j++) {
            written += sprintf(buf + written,
                 "%02X ", ((char *)x)[j+(i*16)] & 0x00FF);
        }
        written += sprintf(buf + written, "\n");
            fprintf(stderr, "%s", buf);
        if(j < 16 && (j + i*16 >= length)) {
            break;
        }
    }
    fflush(stderr);
}

void __rs232_xdump(void *x, int length, int is_read) {
    struct timeval tv;

    gettimeofday(&tv, 0);

    fprintf(stderr, "[%ld.%ld] Transferred %d bytes %s:\n",
        (long)tv.tv_sec, (long)tv.tv_usec, length,
        is_read ? "in" : "out");

    __xdump(x, length);
}

void __rs232_desc_xfer(int length, int is_read) {
    struct timeval tv;

    gettimeofday(&tv, 0);
    fprintf(stderr, "[%ld.%ld] Transferring %d bytes %s\n",
        (long)tv.tv_sec, (long)tv.tv_usec, length, is_read ? "in" : "out");
}

void __rs232_initialize_uart(struct __rs232_handle *desc) {
    struct termios options;

    tcgetattr(desc->fd, &options);

    /* Set up 8N1 format */
    options.c_cflag &= ~(CSIZE);    /* Clear word size */
    options.c_cflag |= CS8;         /* 8 data bits */
    options.c_cflag &= ~(PARENB);   /* Clear parity */
    options.c_cflag &= ~(CSTOPB);   /* Clear stop bit */

    /* Set typical control flags */
    options.c_cflag |= CLOCAL;      /* Local line */
    options.c_cflag |= CREAD;       /* Enable receiver */
    options.c_cflag &= ~(CRTSCTS);  /* Disable hardware flow control */

    /* Make the interface raw (no terminal emulation) */
    options.c_lflag &= ~(ICANON);   /* Do not wait for a newline to push */
    options.c_lflag &= ~(ECHO);     /* Do not echo to sender */
    options.c_lflag &= ~(ECHOE);    /* Do not echo erase character */
    options.c_lflag &= ~(ISIG);     /* Disable terminal signals */
    options.c_oflag &= ~(OPOST);    /* Disable processed output */
    options.c_iflag |= IGNBRK;

    /* Disable software flow control */
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    /* Disable automatic newline translation (0x0A <-> 0x0D) */
    options.c_iflag &= ~(INLCR);
    options.c_iflag &= ~(ICRNL);
    options.c_iflag &= ~(IGNCR);
    options.c_oflag &= ~(ONLCR);
    options.c_oflag &= ~(OCRNL);
    options.c_oflag &= ~(ONOCR);

    tcsetattr(desc->fd, TCSANOW, &options);
}

static int __rs232_set_baud(void *handle, unsigned int baud_bps) {
    struct __rs232_handle *desc;
    int index;
    unsigned int actual;
    struct termios options;

    desc = (struct __rs232_handle *)handle;

    index = __rs232_get_closest_baud_index(baud_bps);
    actual = __rs232_baud_table[index].bps;

    if(baud_bps != actual && 0 != desc->verbose) {
        fprintf(stderr,
            "Warning: could not match desired baud rate (wanted %d, got %d)\n",
            baud_bps, actual);
    }

    /* Get the current settings */
    tcgetattr(desc->fd, &options);
    cfsetispeed(&options, __rs232_baud_table[index].code);
    cfsetospeed(&options, __rs232_baud_table[index].code);

    tcsetattr(desc->fd, TCSANOW, &options);

    return actual;
}

void *RS232Open(char *port, int *errorCode) {
    int temp_fd;
    struct __rs232_handle *desc;

    if(NULL == port) {
        return NULL;
    }

    /* O_RDWR sets bidirectional access
     * O_NOCTTY indicates that this is not a controlling terminal
     * O_NDELAY ignores the state of the DCD pin
     */
    temp_fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
    if(temp_fd < 0) {
        SET_ERROR_CODE(NO_DEVICE_FOUND);
        return NULL;
    }

    fcntl(temp_fd, F_SETFL, 0);             /* Zero out the option flags on the port */
    fcntl(temp_fd, F_SETFL, O_NDELAY);      /* Return 0 on read if no data available */

    desc = (struct __rs232_handle *)calloc(1, sizeof(struct __rs232_handle));
    desc->fd = temp_fd;
    desc->opened = 1;
    __rs232_initialize_uart(desc);

    SET_ERROR_CODE(OPEN_OK);

    return desc;
}

int RS232Close(void *handle) {
    struct __rs232_handle *desc;

    desc = (struct __rs232_handle *)handle;

    close(desc->fd);
    desc->opened = 0;

    return 0;
}

int RS232Write(void *handle, char *data, int numberOfBytes) {
    struct __rs232_handle *desc;
    ssize_t written = 0;
    int offset;
    unsigned int bytesToWrite;

    desc = (struct __rs232_handle *)handle;

    if(NULL == desc) {
        return -1;
    }

    if(0 == desc->opened && 0 != desc->verbose) {
        fprintf(stderr,
            "Warning: attempting to write to unopened RS232 port.\n");
    }

    if(0 != desc->verbose) {
        __rs232_desc_xfer(numberOfBytes, 0);
    }

    offset = 0;
    while(offset < numberOfBytes) {
        bytesToWrite = numberOfBytes - offset;
        written = write(desc->fd, (void *)&(data[offset]), bytesToWrite);
        if(written < 0 && EAGAIN != errno) {
            /* Error occurred */
            if(0 != desc->verbose) {
                fprintf(stderr, "Error: failed to write to RS232 port.\n");
            }
            return written;
        } else if(0 == written || (written < 0 && EAGAIN == errno)) {
            /* This has written as much as possible for now.  Return the number
             * of bytes successfully written.  The caller can deal with this
             * either by trying again or coming back to this later.
             */
            return offset;
        } else {
            /* Some progress has been made, so advance the index */
            offset += written;
        }
    }

    if(0 != desc->verbose) {
        __rs232_xdump(data, offset, 0);
    }

    return offset;
}

int RS232Read(void *handle, char *buffer, int numberOfBytes) {
    struct __rs232_handle *desc;
    int bytesRead;
    int offset;
    unsigned int bytesToRead;

    desc = (struct __rs232_handle *)handle;

    if(NULL == desc) {
        return -1;
    }

    if(0 == desc->opened && 0 != desc->verbose) {
        fprintf(stderr,
            "Warning: attempting to read from unopened RS232 port.\n");
    }

    if(0 != desc->verbose) {
        __rs232_desc_xfer(numberOfBytes, 1);
    }

    offset = 0;
    while(offset < numberOfBytes) {
        bytesToRead = numberOfBytes - offset;
        bytesRead = read(desc->fd, (void *)&(buffer[offset]), bytesToRead);
        if(bytesRead < 0 && EAGAIN != errno) {
            /* Error occurred */
            if(0 != desc->verbose) {
                fprintf(stderr, "Error: failed to read from RS232 port.\n");
            }
            return bytesRead;
        } else if(0 == bytesRead || (bytesRead < 0 && EAGAIN == errno)) {
            /* This has written as much as possible for now.  Return the number
             * of bytes successfully written.  The caller can deal with this
             * either by trying again or coming back to this later.
             */
            return offset;
        } else {
            /* Some progress has been made, so advance the index */
            offset += bytesRead;
        }
    }

    if(0 != desc->verbose) {
        __rs232_xdump(buffer, offset, 1);
    }

    return offset;
}

int RS232SetBaudRate(void *handle, int rate) {

    if(NULL == handle) {
        return -1;
    }

    return __rs232_set_baud(handle, rate);
}

int RS232ClearInputBuffer(void *handle) {
    struct __rs232_handle *desc;

    if(NULL == handle) {
        return -1;
    }

    desc = (struct __rs232_handle *)handle;

    if(0 == desc->opened && 0 != desc->verbose) {
        fprintf(stderr, "Warning: attempting to clear unopened serial port.\n");
    }

    return tcflush(desc->fd, TCIFLUSH);
}

int RS232ClearOutputBuffer(void *handle) {

    struct __rs232_handle *desc;

    if(NULL == handle) {
        return -1;
    }

    desc = (struct __rs232_handle *)handle;

    if(0 == desc->opened && 0 != desc->verbose) {
        fprintf(stderr, "Warning: attempting to clear unopened serial port.\n");
    }

    return tcflush(desc->fd, TCOFLUSH);
}

int RS232WaitForWrite(void *handle) {
    struct __rs232_handle *desc;

    if(NULL == handle) {
        return -1;
    }

    desc = (struct __rs232_handle *)handle;

    if(0 == desc->opened && 0 != desc->verbose) {
        fprintf(stderr, "Warning: attempting to flush unopened serial port.\n");
    }

    return tcdrain(desc->fd);
}
