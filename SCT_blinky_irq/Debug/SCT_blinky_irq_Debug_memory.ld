/*
 * GENERATED FILE - DO NOT EDIT
 * Copyright (c) 2008-2013 Code Red Technologies Ltd,
 * Copyright 2015, 2018-2019 NXP
 * (c) NXP Semiconductors 2013-2021
 * Generated linker script file for LPC812
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.30
 * MCUXpresso IDE v11.3.0 [Build 5222] [2021-01-11] on 7-lug-2021 18.18.42
 */

MEMORY
{
  /* Define each memory region */
  MFlash16 (rx) : ORIGIN = 0x0, LENGTH = 0x4000 /* 16K bytes (alias Flash) */  
  RamLoc4 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x1000 /* 4K bytes (alias RAM) */  
}

  /* Define a symbol for the top of each memory region */
  __base_MFlash16 = 0x0  ; /* MFlash16 */  
  __base_Flash = 0x0 ; /* Flash */  
  __top_MFlash16 = 0x0 + 0x4000 ; /* 16K bytes */  
  __top_Flash = 0x0 + 0x4000 ; /* 16K bytes */  
  __base_RamLoc4 = 0x10000000  ; /* RamLoc4 */  
  __base_RAM = 0x10000000 ; /* RAM */  
  __top_RamLoc4 = 0x10000000 + 0x1000 ; /* 4K bytes */  
  __top_RAM = 0x10000000 + 0x1000 ; /* 4K bytes */  
