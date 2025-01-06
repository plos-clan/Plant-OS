add_compile_flag("-ffreestanding")
mklib(vsso)
target_compile_definitions(vsso PRIVATE RING3=1)
