### P1 Solution

### a
To add a non-scrolling status bar, which is to separate the screen into two distinct regions, we should refer to section CRT Controller Registers in the documentation.   

Necessary register value settings:  
Write the line number of separation to bits 7-0 of (Index 18h) Line Compare Register.  

How the VGA acts upon them:  
The Line Compare field specifies the scan line at which a horizontal division can occur, providing for split-screen operation (This field specifies bits 7-0 of the Line Compare field. Bit 9 of this field is located in the Maximum Scan Line Register, and bit 8 of this field is located in the Overflow Register.)  
When the scan line counter reaches the value in the Line Compare field, the current scan line address is reset to 0 and the Preset Row Scan is presumed to be 0. (If the Pixel Panning Mode field is set to 1 then the Pixel Shift Count and Byte Panning fields are reset to 0 for the remainder of the display cycle.)  
In this way, our lower part stay non-scrolling status bar.  

Any relevant constraints that must be obeyed when setting up the status bar:  
The start address for the lower part is programmed to be 0, except when using virtual resolutions, paging, and/or split-screen operation.  
Only the upper part pans if the Pixel Panning Mode field is set to 0 or both parts are panned if set to 1. (bit 5 of Attribute Mode Control Register (Index 10h))  
The Preset Row Scan only applies to the upper part, while the one of lower part is prsumed to be 0.  

### b
To change the VGA’s color palette, we should refer to section Color Registers in the documentation.    
To write a palette entry, first have the palette entry's index value in DAC Address Write Mode Register (at 3C8h), then load 3 colors to the DAC Data Register(at 3C9h) (Three successive I/O operations accesses three intensity values, first the red, then green, then blue intensity values).   

The sequence of register operations necessary to set a given color to a given 18-bit RGB (red, green, blue) value:  

1. Disable interrupts
2. Write the palette entry's index value into DAC Address Write Mode Register (at 3C8h)
3. Write red value into DAC Data Register (at 3C9h)
4. Write green value into DAC Data Register (at 3C9h)
5. Write blue value into DAC Data Register (at 3C9h)
6. Enable interrupts

After three I/O operations the index automatically increments to allow the next DAC entry to be read without having to reload the index. I/O operations to this port should always be performed in sets of three, otherwise the results are dependent on the DAC implementation. 
