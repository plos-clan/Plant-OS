
#if KERNEL
#  define NO_LOG 0
#elif LOADER
#  define NO_LOG 0
#elif RING0
#  define NO_LOG 0
#elif RING3
#  define NO_LOG 0
#endif
