#ifndef _SCISSORPAPERROCK_USERGESTURE_
#define _SCISSORPAPERROCK_USERGESTURE_

#include <stdint.h>

enum class UserGesture: int { SCISSOR = 0, ROCK = 1, PAPER = 2, UNKNOWN = 4 };

#define CastGesture2Int(g) static_cast<std::size_t>(g)

#endif // _SCISSORPAPERROCK_USERGESTURE_
