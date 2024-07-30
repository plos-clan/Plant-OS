void _start() {
  *(char *)0xb8000 = 'H';
  for (;;)
    ;
}
