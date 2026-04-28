#include "src/main.c"

#if defined(__APPLE__)
#include "src/mac_sound.c"
#elif defined(__linux__)
#include "src/linux_sound.c"
#endif
