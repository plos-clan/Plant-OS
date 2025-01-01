#ifndef __cplusplus

#  define new1(type)      ((type *)calloc(1, sizeof(type)))
#  define new2(type, len) ((type *)calloc((usize)(len), sizeof(type)))
#  define new3(type, len, init)                                                                    \
    ({                                                                                             \
      usize _len_ = (usize)(len);                                                                  \
      assert(_len_ * sizeof(type) / sizeof(type) == _len_);                                        \
      type *_ptr_ = malloc(_len_ * sizeof(type));                                                  \
      if (_ptr_ != null) {                                                                         \
        auto _init_ = (type)(init);                                                                \
        for (usize i = 0; i < _len_; i++) {                                                        \
          _ptr_[i] = _init_;                                                                       \
        }                                                                                          \
      }                                                                                            \
      _ptr_;                                                                                       \
    })

#  define new(...) CONCAT(new, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)

#endif