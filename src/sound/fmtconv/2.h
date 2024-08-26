

__fmtconv_(S8, i8);
__fmtconv_(U8, u8);
__fmtconv_(S16, i16);
__fmtconv_(U16, u16);
__fmtconv_(S32, i32);
__fmtconv_(U32, u32);
__fmtconv_(S64, i64);
__fmtconv_(U64, u64);
#if defined(__x86_64__) && !NO_EXTFLOAT
__fmtconv_(F16, f16);
#endif
__fmtconv_(F32, f32);
__fmtconv_(F64, f64);
