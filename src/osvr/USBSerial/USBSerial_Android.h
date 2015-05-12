/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>

*/

// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/* This file contains definitions that would be found in linux/serial.h which cannot be located during build when using Crystax NDK */

#include <linux/types.h>
struct serial_struct {
 int type;
 int line;
 unsigned int port;
 int irq;
 int flags;
 int xmit_fifo_size;
 int custom_divisor;
 int baud_base;
 unsigned short close_delay;
 char io_type;
 char reserved_char[1];
 int hub6;
 unsigned short closing_wait;
 unsigned short closing_wait2;
 unsigned char *iomem_base;
 unsigned short iomem_reg_shift;
 unsigned int port_high;
 unsigned long iomap_base;
};
#define ASYNC_CLOSING_WAIT_INF 0
#define ASYNC_CLOSING_WAIT_NONE 65535
#define PORT_UNKNOWN 0
#define PORT_8250 1
#define PORT_16450 2
#define PORT_16550 3
#define PORT_16550A 4
#define PORT_CIRRUS 5
#define PORT_16650 6
#define PORT_16650V2 7
#define PORT_16750 8
#define PORT_STARTECH 9
#define PORT_16C950 10
#define PORT_16654 11
#define PORT_16850 12
#define PORT_RSA 13
#define PORT_MAX 13
#define SERIAL_IO_PORT 0
#define SERIAL_IO_HUB6 1
#define SERIAL_IO_MEM 2
#define UART_CLEAR_FIFO 0x01
#define UART_USE_FIFO 0x02
#define UART_STARTECH 0x04
#define UART_NATSEMI 0x08
struct serial_multiport_struct {
 int irq;
 int port1;
 unsigned char mask1, match1;
 int port2;
 unsigned char mask2, match2;
 int port3;
 unsigned char mask3, match3;
 int port4;
 unsigned char mask4, match4;
 int port_monitor;
 int reserved[32];
};
struct serial_icounter_struct {
 int cts, dsr, rng, dcd;
 int rx, tx;
 int frame, overrun, parity, brk;
 int buf_overrun;
 int reserved[9];
};
struct serial_rs485 {
 __u32 flags;
#define SER_RS485_ENABLED (1 << 0)
#define SER_RS485_RTS_ON_SEND (1 << 1)
#define SER_RS485_RTS_AFTER_SEND (1 << 2)
#define SER_RS485_RX_DURING_TX (1 << 4)
 __u32 delay_rts_before_send;
 __u32 delay_rts_after_send;
 __u32 padding[5];
};