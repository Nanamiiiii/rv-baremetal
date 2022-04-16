OUTPUT_ARCH( "riscv" )

ENTRY( _reset )

MEMORY
{
    RAM(wxa!ri) : ORIGIN = 0x80000000, LENGTH = 256M
}

SECTIONS
{
    .text :
    {
        _start_text = .;
        KEEP(*(.init))
        . = ORIGIN(RAM) + 0x200;
        _start_vector = .;
        KEEP(*(isr_vector))
        *(.text*)
        *(.rodata*)
        *(.srodata*)
        # 8 or 4 ???
        . = ALIGN(8);
        _end_text = .;
    } > RAM

    _stored_data = .;
    
}