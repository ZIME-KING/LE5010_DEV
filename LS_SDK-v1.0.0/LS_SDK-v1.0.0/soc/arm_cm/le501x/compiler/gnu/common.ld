
SECTIONS
{
    __vector_lma__ = ORIGIN(FLASH);
    
    .vector :
    {
        . = ALIGN(4);
        __vector__start__ = .;
        KEEP(*(.isr_vector))
        __vector__end__ = .;
    }>RAM AT>FLASH

    __data_lma__ = __vector_lma__ + SIZEOF(.vector);
    
    .data :
    {
        . = ALIGN(4);
        __data_start__ = .;
        *(.data*)
        *(.xip_banned)
		*(vtable)

		. = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(4);
		/* init data */
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);


		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);

		KEEP(*(.jcr*))

        __data_end__ = ALIGN(4);
    }>RAM AT> FLASH   

    .text :
    {
    
        KEEP(*(.init))
        KEEP(*(.fini))

        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)


        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(.eh_frame*)
        *(.text* .rodata*)
    }>FLASH
    
	.ARM.extab : 
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > FLASH
    __exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	}>FLASH
	__exidx_end = .;

    .bss (NOLOAD):
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        *(SECTION_RTT)
        __bss_end__ = ALIGN(4);
    }>RAM

    .heap (NOLOAD):
    {
        __end__ = .;
        PROVIDE(end = .);
        *(.heap*)
    } > RAM
    reset_retain_addr = MAX(__HeapLimit , 0x6AE8);
    .reset_retain reset_retain_addr (NOLOAD):
    {
        . = ALIGN(4);
        reset_retain_start = .;
        . += 2*4;
        reset_retain_end = .;
    } > RAM
    ASSERT(reset_retain_end <= 0xb800, "Error end address for reset retain")
    .dmaram 0xa000 (NOLOAD):
    {
        *(DMAC1)
        *(DMARAM)
    }
    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy (NOLOAD):
    {
        *(.stack*)
    } > RAM

    /* Set stack top to end of RAM, and stack limit move down by
     * size of stack_dummy section */
    PROVIDE(__stack_data_start__ = ORIGIN(RAM) + LENGTH(RAM));
    PROVIDE(__mesh_stack_data_start__ = __stack_data_start__);
    __StackTop = __mesh_stack_data_start__;
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);
    
    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
    
}
