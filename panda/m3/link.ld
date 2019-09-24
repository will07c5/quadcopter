ENTRY(main)

GROUP(libgcc.a libc.a libm.a)

MEMORY
{
	RAM (rwx) : ORIGIN = 0x00000000, LENGTH = 2M
}

SECTIONS
{
	. = 0x00000000;
	.text : {
		KEEP(*(.intr_table))
		*(.text)
		*(.rodata)
	} > RAM

	.data : {
		*(.data)
		*(.bss)
	} > RAM

}
