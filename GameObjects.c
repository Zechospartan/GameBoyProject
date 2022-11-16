#include <gb/gb.h>

//player meta sprite with 4 sprites
struct Player
{
    UBYTE spriteID[4];
    UINT8 x;
    UINT8 y;
    UINT8 width;
    UINT8 height;
} Player;