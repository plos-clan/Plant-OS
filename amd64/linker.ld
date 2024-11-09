OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)
SECTIONS
{
  _image_base = .;
  . = 0x1000;
  .text :
  {
    _text = .;
    *(.text)
    *(.text.*)
    *(.gnu.linkonce.t.*)
    *(.plt)
    . = ALIGN(16);
  }
  _etext = .;
  _text_size = _etext - _text;

  . = ALIGN(4096);
  .data :
  {
    _data = .;
    *(.got.plt)
    *(.got)

    *(.data*)
    *(.rodata*)
    *(.bss.*)
    *(COMMON)
    *(.rel.local)
  }

  _edata = .;
  _data_size = _edata - _data;

  . = ALIGN(4096);
  .rela.dyn :
  {
    _rela = .;
    *(.rela.text*)
    *(.rela.data*)
    *(.rela.got)
    *(.rela.stab)
    *(.rela.init_array*)
    *(.rela.fini_array*)
    *(.rela.ctors*)
    *(.rela.dtors*)
    *(.rela.plt)
    *(.rela.dyn)
  }

  . = ALIGN(4096);
  .reloc :
  {
    KEEP (*(.reloc))
  }

  _image_end = .;
}
