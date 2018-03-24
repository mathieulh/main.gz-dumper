/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * main.c - KDumper sample, demonstrates use of kernel mode.
 *
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
 * Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
 * Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
 *
 * $Id: main.c 1095 2005-09-27 21:02:16Z jim $
 */
#include <pspkernel.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <string.h>

/* Define the module info section, note the 0x1000 flag to enable start in kernel mode */
PSP_MODULE_INFO("SDKTEST", 0x1000, 1, 1);

/* Define the thread attribute as 0 so that the main thread does not get converted to user mode */
PSP_MAIN_THREAD_ATTR(0);

/* Define printf, just to make typing easier */
#define printf	pspDebugScreenPrintf

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
	sceKernelExitGame();

	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	/* Note the use of THREAD_ATTR_USER to ensure the callback thread is in user mode */
	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, THREAD_ATTR_USER, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

char g_data[0x100] __attribute__((aligned(64)));

/* Well what would you expect ? :) */
void dump_memregion(const char* file, void *addr, int len)
{
	int fd;

	fd = sceIoOpen(file, PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);
	if(fd >= 0)
	{
		printf("Writing %s\n", file);
		memcpy(g_data, addr, len);
		sceIoWrite(fd, g_data, len);
		sceIoClose(fd);
	}
}

int main(void)
{
    u32 reg;
	pspDebugScreenInit();
	SetupCallbacks();

	printf("Kernel mem dumper. TyRaNiD 2k5.\n");
	printf("Props to nem, mrbrown, adresd et al\n\n");

	dump_memregion("ms0:/safe_kmem.bin", (void*) 0x883F0000, 0x10000);

	dump_memregion("ms0:/s_kmem.bin", (void*) 0x883FFFF4, 0xC);
	asm __volatile__ (
		"cfc0 $3, $17\n"
		"sw $3, 0(%0)\n"
    :: "r"(&reg));   
    dump_memregion("ms0:/cfc0_17.bin",(void*)&reg,4); 
	printf("Done\n");
    sceKernelExitGame();

	return 0;
}
