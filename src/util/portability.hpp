#pragma once

#if __has_cpp_attribute(no_unique_address)
#define NO_UNIQUE_ADDRESS [[no_unique_address]]
#else
#define NO_UNIQUE_ADDRESS
#endif

#if __clang__
#define UNUSEDPRIVATEMEMBER(x)                                       \
  _Pragma("clang diagnostic push")                                   \
      _Pragma("clang diagnostic ignored \"-Wunused-private-field\"") \
          x _Pragma("clang diagnostic pop")
#else
#define UNUSEDPRIVATEMEMBER(x) x
#endif
