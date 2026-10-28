#include "mem_map.h"

MEMORY
{
  FLASH (rx) : ORIGIN = PKGFLXAPP_APP0_LNA, LENGTH = PKGFLXAPP_APP0_SIZE
  RAM (rwx)  : ORIGIN = PKGFLXAPP_RAM_ADDR, LENGTH = PKGFLXAPP_RAM_SIZE
}

SECTIONS
{
  .app_header : ALIGN(4)
  {
    KEEP(*(.app_header))
    KEEP(*(.iomap))
    KEEP(*(.cfgmap))
  } > FLASH

  .app_nvram : ALIGN(0x1000)
  {
    KEEP(*(.nvramcfg))
  } > FLASH

  .text :
  {
    *(.text.startup.main)
    *(.text*)
    *(.rodata*)
  } > FLASH

  /* Every registration section exists, even if no function is registered. */
  .luat_init : ALIGN(4)
  {
    . = ALIGN(4);
    __start_luat_init_hw = .;
    KEEP(*(luat_init_hw))
    __stop_luat_init_hw = .;
    . = ALIGN(4);
    __start_luat_init_drv = .;
    KEEP(*(luat_init_drv))
    __stop_luat_init_drv = .;
    . = ALIGN(4);
    __start_luat_init_task = .;
    KEEP(*(luat_init_task))
    __stop_luat_init_task = .;
  } > FLASH

  .data : ALIGN(4)
  {
    . = ALIGN(4);
    Load$$LOAD_DRAM_SHARED$$Base = LOADADDR(.data);
    Image$$LOAD_DRAM_SHARED$$Base = .;
    *(.data*)
    *(.sect_ap_psram_p0_data.*)
    *(.sect_ap_ps_psram_p0_data.*)
    . = ALIGN(4);
  } >RAM AT>FLASH

  Image$$LOAD_DRAM_SHARED$$Length = SIZEOF(.data);

  .load_dram_shared_zi (NOLOAD):
  {
    . = ALIGN(4);
    Image$$LOAD_DRAM_SHARED$$ZI$$Base = .;
    __bss_start__ = .;
    *(.bss*)
    *(.sect_*_bss* .bss*)
    *(.sect_ap_psram_p0_bss.*)
    *(.sect_ap_ps_psram_p0_bss.*)
    *(COMMON)
    *(.sect_decompress_bss.*)
    . = ALIGN(4);
    *(.stack*)
    __bss_end__ = .;
    Image$$LOAD_DRAM_SHARED$$ZI$$Limit = .;
    *(.USB_NOINIT_DATA_BUF)
  } >RAM
}
