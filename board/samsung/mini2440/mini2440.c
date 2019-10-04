/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002, 2010
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <netdev.h>
#include <asm/io.h>
#include <asm/arch/s3c24x0_cpu.h>
#include <elf.h>
#include <asm/sections.h>


#include <config.h>
#include <spl.h>
#include <image.h>
#include <linux/compiler.h>

#ifndef CONFIG_SPL_DM
/* Pointer to as well as the global data structure for SPL */
DECLARE_GLOBAL_DATA_PTR;

/*
 * WARNING: This is going away very soon. Don't use it and don't submit
 * pafches that rely on it. The global_data area is set up in crt0.S.
 */
gd_t gdata __attribute__ ((section(".data")));
#endif

#define FCLK_SPEED 1

#if (FCLK_SPEED == 0)		/* Fout = 203MHz, Fin = 12MHz for Audio */
#define M_MDIV	0xC3
#define M_PDIV	0x4
#define M_SDIV	0x1
#elif (FCLK_SPEED == 1)		/* Fout = 202.8MHz */
#define M_MDIV	0xA1
#define M_PDIV	0x3
#define M_SDIV	0x1
#endif

#define USB_CLOCK 1

#if (USB_CLOCK == 0)
#define U_M_MDIV	0xA1
#define U_M_PDIV	0x3
#define U_M_SDIV	0x1
#elif (USB_CLOCK == 1)
#define U_M_MDIV	0x48
#define U_M_PDIV	0x3
#define U_M_SDIV	0x2
#endif


/*
 * Miscellaneous platform dependent initialisations
 */

int board_early_init_f(void)
{
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();

	/* set up the I/O ports */
	writel(0x007FFFFF, &gpio->gpacon);
	writel(0x00044555, &gpio->gpbcon);
	writel(0x000007FF, &gpio->gpbup);
	writel(0xAAAAAAAA, &gpio->gpccon);
	writel(0x0000FFFF, &gpio->gpcup);
	writel(0xAAAAAAAA, &gpio->gpdcon);
	writel(0x0000FFFF, &gpio->gpdup);
	writel(0xAAAAAAAA, &gpio->gpecon);
	writel(0x0000FFFF, &gpio->gpeup);
	writel(0x000055AA, &gpio->gpfcon);
	writel(0x000000FF, &gpio->gpfup);
	writel(0xFF95FFBA, &gpio->gpgcon);
	writel(0x0000FFFF, &gpio->gpgup);
	writel(0x002AFAAA, &gpio->gphcon);
	writel(0x000007FF, &gpio->gphup);

	return 0;
}

int board_init(void)
{
	/* arch number of SMDK2410-Board */
	//gd->bd->bi_arch_number = MACH_TYPE_SMDK2410;
    gd->bd->bi_arch_number = MACH_TYPE_MINI2440;
    
	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0x30000100;

	icache_enable();
	dcache_enable();

	return 0;
}

int dram_init(void)
{
	/* dram_init must store complete ramsize in gd->ram_size */
	gd->ram_size = PHYS_SDRAM_1_SIZE;
	return 0;
}

#ifdef CONFIG_CMD_NET
int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_CS8900
	rc = cs8900_initialize(0, CONFIG_CS8900_BASE);
#endif
#ifdef CONFIG_DRIVER_DM9000
    rc = dm9000_initialize(bis);
#endif
	return rc;
}
#endif

/*
 * Hardcoded flash setup:
 * Flash 0 is a non-CFI AMD AM29LV800BB flash.
 */
ulong board_flash_get_legacy(ulong base, int banknum, flash_info_t *info)
{
	info->portwidth = FLASH_CFI_16BIT;
	info->chipwidth = FLASH_CFI_BY16;
	info->interface = FLASH_CFI_X16;
	return 1;
}

#define        GPHCON                              (*(volatile unsigned long *)0x56000070)
#define        GPHDAT                              (*(volatile unsigned long *)0x56000074)
#define        GPHUP                                 (*(volatile unsigned long *)0x56000078)

#define        ULCON0                               (*(volatile unsigned long *)0x50000000)
#define        UCON0                                 (*(volatile unsigned long *)0x50000004)
#define        UFCON0                            (*(volatile unsigned long *)0x50000008)
#define        UMCON0                             (*(volatile unsigned long *)0x5000000c)
#define        UTRSTAT0                           (*(volatile unsigned long *)0x50000010)
#define        UTXH0                                (*(volatile unsigned char *)0x50000020)
#define        URXH0                               (*(volatile unsigned char *)0x50000024)
#define        UBRDIV0                             (*(volatile unsigned long *)0x50000028)

/* use userial for test */
void init_uart(void)
{
   /*初始化端口*/
   GPHCON |= (0x2 << 6 | 0x2 << 4);
   GPHUP  |= 0x3 << 2;
   
   /*波特率115200，无奇偶校验，1位停止位*/
   ULCON0 |= 0x3;
   UCON0  |= (0x1 << 2 | 0x1 << 0);
   UBRDIV0 = 26;
}
 
/*接收一个字符*/
char getchar(void)
{
  while(!(UTRSTAT0 & (0x1 << 0)));
  return URXH0;
}
 
/*发送一个字符*/
void outchar(char c)
{
  while(!(UTRSTAT0 & (0x1 << 2)));
  UTXH0 = c;
}
 
/*发送一个字符串*/
void outstring(char* str)
{
  while(*str)
  {
    outchar(*str++);
  }
}

#ifdef CONFIG_SPL_BUILD
void board_init_r(gd_t *dummy1, ulong dummy2)
{
	__attribute__((noreturn)) void (*uboot)(void);

	init_uart();

	outstring("hello !This is SPL booting");
extern int copy_code_to_sdram(unsigned char *src, unsigned char *dest, unsigned int len);
	copy_code_to_sdram((unsigned char *)(CONFIG_SYS_NAND_U_BOOT_OFFS), \
						(unsigned char *)(CONFIG_SYS_NAND_U_BOOT_DST), \
						CONFIG_SYS_NAND_U_BOOT_SIZE);

	/*
	 * Jump to U-Boot image
	 */
	uboot = (void *)CONFIG_SYS_NAND_U_BOOT_DST;
	(*uboot)();

	while(1);
}

void board_init_f(ulong dummy)
{

#ifndef CONFIG_SPL_DM
	/* TODO: Remove settings of the global data pointer here */
	gd = &gdata;
#endif
	board_init_r(NULL, 0);
}
#endif