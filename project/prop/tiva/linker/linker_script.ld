/** @file linker_script.ld
*
* @brief Linker script for tiva, adapted from tiva-ware blinky.ld 
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/


TASK_HEAP_SIZE = 0x10000;
HEAP_SIZE = 0x8000;

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x00100000
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00040000
}

SECTIONS
{
    .text :
    {
        _text = .;
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        _etext = .;
    } > FLASH

    .ARM.exidx :
    {
        *(.ARM.exidx*)
        *(.gnu.linkonce.armexidx.*)
    } > FLASH

    _begin_data = .;

    .data : AT(_begin_data)
    {
        _data = .;
        _ldata = LOADADDR (.data);
        *(vtable)
        *(.data*)
        _edata = .;
    } > SRAM

    .bss :
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > SRAM
    heap_low = .;
    . = . + HEAP_SIZE; 
    heap_top = .;
    task_heap_low = .;
    . = . + TASK_HEAP_SIZE; 
    task_heap_high = .;
}
