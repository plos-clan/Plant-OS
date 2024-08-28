utillib(libc-base)
libdef(util-libc-base NO_STD=0)

utillib(misc)
tgtlib(util-misc util-libc-base)
libdef(util-misc NO_STD=0)

utillib(data-structure)
libdef(util-data-structure NO_STD=0)

utillib(font)
tgtlib(util-font util-data-structure)

utillib(pl2d)
tgtlib(util-pl2d util-misc)

utillib(plds)
tgtlib(util-plds util-pl2d)

utillib(plui)
tgtlib(util-plui util-pl2d)

utillib(plac)
tgtlib(util-plac util-misc)

utillib(sound)
tgtlib(util-sound)
