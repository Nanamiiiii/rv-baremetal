OUTPUT_ARCH( "riscv" )

ENTRY( _reset )

MEMORY
{
    FLASH(rxai!w) : ORIGIN = 0x20000000, LENGTH = 32M
    RAM(wxa!ri) : ORIGIN = 0x80000000, LENGTH = 256M
}

SECTIONS
{
    .text :
    {
        _start_text = .;
        KEEP(*(.init))
        . = ORIGIN(FLASH) + 0x200;
        _start_vector = .;
        KEEP(*(.trap_handler))
        *(.text .text.*)
        *(.rodata .rodata.*)
        *(.srodata .srodata.*)
        . = ALIGN(4);
        _end_text = .;
    } > FLASH

    _stored_data = .;

    .data : AT(_stored_data)
    {
        . = ALIGN(4096);
        _start_data = .;
        KEEP(*(.ram*))
        . = ALIGN(4);
        *(.data .data.*)
        . = ALIGN(4);
        _global_pointer = . + 0x800;
        *(.sdata .sdata.*)
        . = ALIGN(4);
        _end_data = .;
    } > RAM

    .bss (NOLOAD) :
    {
        _start_bss = .;
        *(.sbss .sbss.*)
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        _end_bss = .;
        _end = .;
    } > RAM

}

PROVIDE(_start_heap = _end);
PROVIDE(_end_stack  = ORIGIN(RAM) + (LENGTH(RAM)) );
 