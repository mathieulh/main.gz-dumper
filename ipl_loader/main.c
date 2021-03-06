/*
 * Loads the code needed for the modified 1.50 ipl to run
 * ms0:/ipl/ipl_load.bin is just a dump of stage2.bin from the decrypted 1.50 ipl
 * ms0:/ipl/patch.bin is loaded to 0x40e0000, and is run right before the normal 
 * ipl code is run
*/
#include <pspkernel.h>

PSP_MODULE_INFO("IPL_Loader", 0x1000, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

char buf[1024*1024];

#define MIPS_LUI(R,IMM) 0x3c000000|(R<<16)|((unsigned int)(IMM)&0xffff)

#define MIPS_ADDI(RT,RS,IMM)  (0x20000000|(RS<<21)|(RT<<16)|((unsigned int)(IMM)&0xffff))
#define MIPS_ADDIU(RT,RS,IMM) (0x24000000|(RS<<21)|(RT<<16)|((unsigned int)(IMM)&0xffff))
#define MIPS_ANDI(RT,RS,IMM)  (0x30000000|(RS<<21)|(RT<<16)|((unsigned int)(IMM)&0xffff))
#define MIPS_ORI(RT,RS,IMM)   (0x34000000|(RS<<21)|(RT<<16)|((unsigned int)(IMM)&0xffff))
#define MIPS_NOP              0x00000000
#define MIPS_J(ADDR)          (0x08000000 + ((((unsigned int)(ADDR))&0x0ffffffc)>>2))
#define MIPS_JR(R)            (0x00000008 + ((R)<<21))
#define MIPS_JAL(ADDR)        (0x0c000000 + (((unsigned int)(ADDR)>>2)&0x03ffffff))
#define MIPS_SYSCALL(NUM)     (0x0000000C+((NUM)<<6))

#define MIPS_AND(RD,RS,RT)  (0x00000024|(RD<<11)|(RT<<16)|(RS<<21))
#define MIPS_ADD(RD,RS,RT)  (0x00000020|(RD<<11)|(RT<<16)|(RS<<21))
#define MIPS_ADDU(RD,RS,RT) (0x00000021|(RD<<11)|(RT<<16)|(RS<<21))

#define MIPS_SW(RT,BASE,OFFSET) (0xac000000|(BASE<<21)|(RT<<16)|(OFFSET&0xFFFF))
#define MIPS_SH(RT,BASE,OFFSET) (0xa4000000|(BASE<<21)|(RT<<16)|(OFFSET&0xFFFF))


int main(int argc, char *argv[])
{
    SceUID fd;
    u32 size;

    fd = sceIoOpen("ms0:/zpl/ipl_load.bin",PSP_O_RDONLY,0777);
    size = sceIoRead(fd,buf,1024*1024);
    sceIoClose(fd);

    memcpy((void*)0x44000000,buf,size);

    fd = sceIoOpen("ms0:/zpl/patch.bin",PSP_O_RDONLY,0777);
    size = sceIoRead(fd,buf,63*1024);//63kb of code max
    sceIoClose(fd);

    memcpy((void*)0x440e0000,buf,size);

    *(u32*)0x440f0000 = MIPS_JAL(0x40e0000);//overwrite 1st ipl ins with a jal to our patch
    *(u32*)0x440f0004 = MIPS_NOP;

    sceKernelDelayThread(10000);
    asm("break\n");
	return 0;
}
