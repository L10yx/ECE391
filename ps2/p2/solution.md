### P2 Solution

### a

1. MTCP_BIOC_ON 
(Enable Button interrupt-on-change. MTCP_ACK is returned.)

Sent when wanting to respond to coming button events from MTC.

Enables Button interrupt-on-change, allowing system to generate interrupts upon changes of button status.

Returns MTCP_ACK, indicating successful execution.

2. MTCP_LED_SET

Sent when wanting to change the current LED display on Tux.

Set the User-set LED display values. These will be displayed on the LED displays when the LED display is in USR mode (enabled by MTCP_LED_USR).

Mapping from 7-segment to bits

 	The 7-segment display is:
		  _A
		F| |B
		  -G
		E| |C
		  -D .dp

 	The map from bits to segments is:
 
 	__7___6___5___4____3___2___1___0__
 	| A | E | F | dp | G | C | B | D | 
 	+---+---+---+----+---+---+---+---+
 
 	Arguments: >= 1 bytes
		byte 0 - Bitmask of which LED's to set:

		__7___6___5___4____3______2______1______0___
 		| X | X | X | X | LED3 | LED2 | LED1 | LED0 | 
 		----+---+---+---+------+------+------+------+

The number of bytes which should follow should be equal to the number of bits set in byte 0. The bytes should be sent in order of increasing LED number. (e.g LED0, LED2, LED3 for a bitmask of 0x0D)

Returns MTCP_ACK, indicating successful execution.

### b

1. MTCP_ACK

Response when the MTC successfully completes a command.

Indicate successful execution of the corresponding command.

2. MTCP_BIOC_EVENT (Byte 0 of MTCP_BIOC_EVT)

Generated when the Button Interrupt-on-change mode is enabled and a button is either pressed or released.

Indicate changes of button status (either pressed or released) for the corresponding command.

Packet Format:

    Byte 0 - MTCP_BIOC_EVENT  
    Byte 1     7 - 4    3   2   1     0  
            +---------+---+---+---+-------+  
            | 1 X X X | C | B | A | start |  
            +---------+---+---+---+-------+  
    Byte 2     7 - 4      3      2      1     0  
            +---------+-------+------+------+----+  
            | 1 X X X | right | down | left | up |  
            +---------+-------+------+------+----+  

3. MTCP_RESET

Generated when the devide re-initializes itself after a power-up, a RESET button press, or an MTCP_RESET_DEV command.

Indicate reset message of the device for the corresponding command that needs reset.

Packet Format:  
    Byte 0 - MTCP_RESET  
    Byte 1 - reserved  
    Byte 2 - reserved  

### c

Function tuxctl_handle_packet() is called by tuxctl_ldisc_data_callback(), and further by tuxctl_ldisc_rcv_buf() at line 182 in tuxctl-ld.c, at which a interrupt context but not locked by spin lock, so it cannot wait in order not to take up too much time in interrupt context.
