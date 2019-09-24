ENTRY(_start)

MEMORY
{
	RAM (rwx) : ORIGIN = 0x9E000000, LENGTH = 1M
}

SECTIONS
{
	.text 0x9E000000 : {
		*(.text.startup)
		*(.text)
	} > RAM
	
	.data : {
		*(.data)
		*(.bss)
	} > RAM
}
