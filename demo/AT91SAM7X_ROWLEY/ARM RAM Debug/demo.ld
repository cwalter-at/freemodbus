MEMORY
{
  UNPLACED_SECTIONS (wx) : ORIGIN = 0x100000000, LENGTH = 0
  User_Peripherals (wx) : ORIGIN = 0xf0000000, LENGTH = 0x0fff0000
  System_Controller (wx) : ORIGIN = 0xfffff000, LENGTH = 0x00000000
  SRAM (wx) : ORIGIN = 0x00200000, LENGTH = 0x00010000
  FLASH (rx) : ORIGIN = 0x00100000, LENGTH = 0x00040000
}


SECTIONS
{
  __User_Peripherals_segment_start__ = 0xf0000000;
  __User_Peripherals_segment_end__ = 0xffff0000;
  __System_Controller_segment_start__ = 0xfffff000;
  __System_Controller_segment_end__ = 0xfffff000;
  __SRAM_segment_start__ = 0x00200000;
  __SRAM_segment_end__ = 0x00210000;
  __FLASH_segment_start__ = 0x00100000;
  __FLASH_segment_end__ = 0x00140000;

  __STACKSIZE__ = 512;
  __STACKSIZE_IRQ__ = 512;
  __STACKSIZE_FIQ__ = 512;
  __STACKSIZE_SVC__ = 512;
  __STACKSIZE_ABT__ = 0;
  __STACKSIZE_UND__ = 512;
  __HEAPSIZE__ = 1024;

  __vectors_load_start__ = __SRAM_segment_start__;
  .vectors __SRAM_segment_start__ :
  {
    __vectors_start__ = .;
    *(.vectors .vectors.*)
  }
  __vectors_end__ = __vectors_start__ + SIZEOF(.vectors);

  . = ASSERT(__vectors_end__ >= __SRAM_segment_start__ && __vectors_end__ <= (__SRAM_segment_start__ + 0x00010000) , "error: .vectors is too large to fit in SRAM memory segment");

  __fast_load_start__ = ALIGN(__vectors_end__ , 4);
  .fast ALIGN(__vectors_end__ , 4) :
  {
    __fast_start__ = .;
    *(.fast .fast.*)
  }
  __fast_end__ = __fast_start__ + SIZEOF(.fast);

  . = ASSERT(__fast_end__ >= __SRAM_segment_start__ && __fast_end__ <= (__SRAM_segment_start__ + 0x00010000) , "error: .fast is too large to fit in SRAM memory segment");

  __init_load_start__ = ALIGN(__fast_end__ , 4);
  .init ALIGN(__fast_end__ , 4) :
  {
    __init_start__ = .;
    *(.init .init.*)
  }
  __init_end__ = __init_start__ + SIZEOF(.init);

  . = ASSERT(__init_end__ >= __SRAM_segment_start__ && __init_end__ <= (__SRAM_segment_start__ + 0x00010000) , "error: .init is too large to fit in SRAM memory segment");

  __text_load_start__ = ALIGN(__init_end__ , 4);
  .text ALIGN(__init_end__ , 4) :
  {
    __text_start__ = .;
    *(.text .text.* .glue_7t .glue_7 .gnu.linkonce.t.*)
  }
  __text_end__ = __text_start__ + SIZEOF(.text);

  . = ASSERT(__text_end__ >= __SRAM_segment_start__ && __text_end__ <= (__SRAM_segment_start__ + 0x00010000) , "error: .text is too large to fit in SRAM memory segment");

  __dtors_load_start__ = ALIGN(__text_end__ , 4);
  .dtors ALIGN(__text_end__ , 4) :
  {
    __dtors_start__ = .;
    KEEP (*(SORT(.dtors.*))) KEEP (*(.dtors))
  }
  __dtors_end__ = __dtors_start__ + SIZEOF(.dtors);

  . = ASSERT(__dtors_end__ >= __SRAM_segment_start__ && __dtors_end__ <= (__SRAM_segment_start__ + 0x00010000) , "error: .dtors is too large to fit in SRAM memory segment");

  __ctors_load_start__ = ALIGN(__dtors_end__ , 4);
  .ctors ALIGN(__dtors_end__ , 4) :
  {
    __ctors_start__ = .;
    KEEP (*(SORT(.ctors.*))) KEEP (*(.ctors))
  }
  __ctors_end__ = __ctors_start__ + SIZEOF(.ctors);

  . = ASSERT(__ctors_end__ >= __SRAM_segment_start__ && __ctors_end__ <= (__SRAM_segment_start__ + 0x00010000) , "error: .ctors is too large to fit in SRAM memory segment");

  __data_load_start__ = ALIGN(__ctors_end__ , 4);
  .data ALIGN(__ctors_end__ , 4) :
  {
    __data_start__ = .;
    *(.data .data.* .gnu.linkonce.d.*)
  }
  __data_end__ = __data_start__ + SIZEOF(.data);

  . = ASSERT(__data_end__ >= __SRAM_segment_start__ && __data_end__ <= (__SRAM_segment_start__ + 0x00010000) , "error: .data is too large to fit in SRAM memory segment");

  __rodata_load_start__ = ALIGN(__data_end__ , 4);
  .rodata ALIGN(__data_end__ , 4) :
  {
    __rodata_start__ = .;
    *(.rodata .rodata.* .gnu.linkonce.r.*)
  }
  __rodata_end__ = __rodata_start__ + SIZEOF(.rodata);

  . = ASSERT(__rodata_end__ >= __SRAM_segment_start__ && __rodata_end__ <= (__SRAM_segment_start__ + 0x00010000) , "error: .rodata is too large to fit in SRAM memory segment");

  __bss_load_start__ = ALIGN(__rodata_end__ , 4);
  .bss ALIGN(__rodata_end__ , 4) (NOLOAD) :
  {
    __bss_start__ = .;
    *(.bss .bss.* .gnu.linkonce.b.*) *(COMMON)
  }
  __bss_end__ = __bss_start__ + SIZEOF(.bss);

  . = ASSERT(__bss_end__ >= __SRAM_segment_start__ && __bss_end__ <= (__SRAM_segment_start__ + 0x00010000) , "error: .bss is too large to fit in SRAM memory segment");

  __non_init_load_start__ = ALIGN(__bss_end__ , 4);
  .non_init ALIGN(__bss_end__ , 4) (NOLOAD) :
  {
    __non_init_start__ = .;
    *(.non_init .non_init.*)
  }
  __non_init_end__ = __non_init_start__ + SIZEOF(.non_init);

  . = ASSERT(__non_init_end__ >= __SRAM_segment_start__ && __non_init_end__ <= (__SRAM_segment_start__ + 0x00010000) , "error: .non_init is too large to fit in SRAM memory segment");

  __heap_load_start__ = ALIGN(__non_init_end__ , 4);
  .heap ALIGN(__non_init_end__ , 4) (NOLOAD) :
  {
    __heap_start__ = .;
    *(.heap)
    . = MAX(__heap_start__ + __HEAPSIZE__ , .);
  }
  __heap_end__ = __heap_start__ + SIZEOF(.heap);

  . = ASSERT(__heap_end__ >= __SRAM_segment_start__ && __heap_end__ <= (__SRAM_segment_start__ + 0x00010000) , "error: .heap is too large to fit in SRAM memory segment");

  __stack_load_start__ = ALIGN(__heap_end__ , 4);
  .stack ALIGN(__heap_end__ , 4) (NOLOAD) :
  {
    __stack_start__ = .;
    *(.stack)
    . = MAX(__stack_start__ + __STACKSIZE__ , .);
  }
  __stack_end__ = __stack_start__ + SIZEOF(.stack);

  . = ASSERT(__stack_end__ >= __SRAM_segment_start__ && __stack_end__ <= (__SRAM_segment_start__ + 0x00010000) , "error: .stack is too large to fit in SRAM memory segment");

  __stack_irq_load_start__ = ALIGN(__stack_end__ , 4);
  .stack_irq ALIGN(__stack_end__ , 4) (NOLOAD) :
  {
    __stack_irq_start__ = .;
    *(.stack_irq)
    . = MAX(__stack_irq_start__ + __STACKSIZE_IRQ__ , .);
  }
  __stack_irq_end__ = __stack_irq_start__ + SIZEOF(.stack_irq);

  . = ASSERT(__stack_irq_end__ >= __SRAM_segment_start__ && __stack_irq_end__ <= (__SRAM_segment_start__ + 0x00010000) , "error: .stack_irq is too large to fit in SRAM memory segment");

  __stack_fiq_load_start__ = ALIGN(__stack_irq_end__ , 4);
  .stack_fiq ALIGN(__stack_irq_end__ , 4) (NOLOAD) :
  {
    __stack_fiq_start__ = .;
    *(.stack_fiq)
    . = MAX(__stack_fiq_start__ + __STACKSIZE_FIQ__ , .);
  }
  __stack_fiq_end__ = __stack_fiq_start__ + SIZEOF(.stack_fiq);

  . = ASSERT(__stack_fiq_end__ >= __SRAM_segment_start__ && __stack_fiq_end__ <= (__SRAM_segment_start__ + 0x00010000) , "error: .stack_fiq is too large to fit in SRAM memory segment");

  __stack_svc_load_start__ = ALIGN(__stack_fiq_end__ , 4);
  .stack_svc ALIGN(__stack_fiq_end__ , 4) (NOLOAD) :
  {
    __stack_svc_start__ = .;
    *(.stack_svc)
    . = MAX(__stack_svc_start__ + __STACKSIZE_SVC__ , .);
  }
  __stack_svc_end__ = __stack_svc_start__ + SIZEOF(.stack_svc);

  . = ASSERT(__stack_svc_end__ >= __SRAM_segment_start__ && __stack_svc_end__ <= (__SRAM_segment_start__ + 0x00010000) , "error: .stack_svc is too large to fit in SRAM memory segment");

  __stack_abt_load_start__ = ALIGN(__stack_svc_end__ , 4);
  .stack_abt ALIGN(__stack_svc_end__ , 4) (NOLOAD) :
  {
    __stack_abt_start__ = .;
    *(.stack_abt)
    . = MAX(__stack_abt_start__ + __STACKSIZE_ABT__ , .);
  }
  __stack_abt_end__ = __stack_abt_start__ + SIZEOF(.stack_abt);

  . = ASSERT(__stack_abt_end__ >= __SRAM_segment_start__ && __stack_abt_end__ <= (__SRAM_segment_start__ + 0x00010000) , "error: .stack_abt is too large to fit in SRAM memory segment");

  __stack_und_load_start__ = ALIGN(__stack_abt_end__ , 4);
  .stack_und ALIGN(__stack_abt_end__ , 4) (NOLOAD) :
  {
    __stack_und_start__ = .;
    *(.stack_und)
    . = MAX(__stack_und_start__ + __STACKSIZE_UND__ , .);
  }
  __stack_und_end__ = __stack_und_start__ + SIZEOF(.stack_und);

  __SRAM_segment_used_end__ = ALIGN(__stack_abt_end__ , 4) + SIZEOF(.stack_und);

  . = ASSERT(__stack_und_end__ >= __SRAM_segment_start__ && __stack_und_end__ <= (__SRAM_segment_start__ + 0x00010000) , "error: .stack_und is too large to fit in SRAM memory segment");

}

