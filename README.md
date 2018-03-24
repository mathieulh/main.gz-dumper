# main.gz-dumper
This uses either the IPL exploit or a custom IPL to grab the decrypted main.gz/part3.bin of 3.00+ fws.

Put the decrypted part2 of the ipl you want to decrypt in ms0:/zpl/ipl_load.bin
and make the necessary changes in ipl_shellcode/patch.S (look at the comments).
Copy patch.bin into ms0:/zpl/patch.bin. 


Using the IPL 0 checksum block exploit: 
Start the dec ipl loader, hold L, wait for the psp to turn off, turn it back on,
turn it off and then on again, and finally dump the data with simple_kdumper.

If you don't have the ipl exploit setup, You may craft your own IPL using iptool (https://github.com/zecoxao/ipltool) instead.
You will need to use the following parameters: 

<pre>
unsigned long int entry			= 0x040F0000;
u32 loadAddr 					= 0x040E0000;
u32 dataSize 					= 0xF50;
u32 decblkSize 					= 0xF60;
</pre>

You can change these in ipltool.c 
