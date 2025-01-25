add_compile_flag("-ffreestanding")
mklib(vsdo)
target_compile_definitions(vsdo PRIVATE RING3=1)
