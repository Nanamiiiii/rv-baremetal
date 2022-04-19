OUTPUT_ARCH( "riscv" )

ENTRY( _reset )

MEMORY
{
    RAM(wxa!ri) : ORIGIN = 0x80000000, LENGTH = 256M
}

PHDRS
{
  text PT_LOAD;
  data PT_LOAD;
  bss PT_LOAD;
}

SECTIONS
{
    .text :
    {
        _start_text = .;
        KEEP(*(.init))
        . = ORIGIN(RAM) + 0x200;
        _start_vector = .;
        KEEP(*(.isr_vector))
        *(.text .text.*)
        *(.rodata .rodata.*)
        *(.srodata .srodata.*)
        . = ALIGN(4);
        _end_text = .;
    } > RAM AT > RAM :text

    _stored_data = .;

    .data :
    {
        . = ALIGN(4096);
        _start_data = .;
        KEEP(*(.ramcode*))
        . = ALIGN(4);
        *(.data .data.*)
        . = ALIGN(4);
        _global_pointer = . + 0x800;
        *(.sdata .sdata.*)
        . = ALIGN(4);
        _end_data = .;
    } > RAM AT > RAM :data

    .bss (NOLOAD) :
    {
        _start_bss = .;
        *(.sbss .sbss.*)
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        _end_bss = .;
        _end = .;
    } > RAM AT > RAM :bss

}

PROVIDE(_start_heap = _end);
PROVIDE(_end_stack  = ORIGIN(RAM) + (LENGTH(RAM)) );
 