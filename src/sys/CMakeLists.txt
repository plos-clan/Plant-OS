add_compile_flag("-ffreestanding")
mklib(sys)
target_compile_definitions(sys PRIVATE RING3=1)
