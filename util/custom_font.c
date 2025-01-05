#include <stdint.h>
#include "util/oled_fonts.h"

static const uint16_t CustomFont6x8[] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // sp
    0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x0000, 0x2000, 0x0000, // !
    0x5000, 0x5000, 0x5000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // "
    0x5000, 0x5000, 0xf800, 0x5000, 0xf800, 0x5000, 0x5000, 0x0000, // #
    0x2000, 0x7800, 0xa000, 0x7000, 0x2800, 0xf000, 0x2000, 0x0000, // $
    0xc000, 0xc800, 0x1000, 0x2000, 0x4000, 0x9800, 0x1800, 0x0000, // %
    0x4000, 0xa000, 0xa000, 0x4000, 0xa800, 0x9000, 0x6800, 0x0000, // &
    0x3000, 0x3000, 0x2000, 0x4000, 0x0000, 0x0000, 0x0000, 0x0000, // '
    0x1000, 0x2000, 0x4000, 0x4000, 0x4000, 0x2000, 0x1000, 0x0000, // (
    0x4000, 0x2000, 0x1000, 0x1000, 0x1000, 0x2000, 0x4000, 0x0000, // )
    0x2000, 0xa800, 0x7000, 0xf800, 0x7000, 0xa800, 0x2000, 0x0000, // *
    0x0000, 0x2000, 0x2000, 0xf800, 0x2000, 0x2000, 0x0000, 0x0000, // +
    0x0000, 0x0000, 0x0000, 0x0000, 0x3000, 0x3000, 0x2000, 0x0000, // ,
    0x0000, 0x0000, 0x0000, 0xf800, 0x0000, 0x0000, 0x0000, 0x0000, // -
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x3000, 0x3000, 0x0000, // .
    0x0000, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000, 0x0000, 0x0000, // /
    0x7000, 0x8800, 0x9800, 0xa800, 0xc800, 0x8800, 0x7000, 0x0000, // 0
    0x2000, 0x6000, 0x2000, 0x2000, 0x2000, 0x2000, 0x7000, 0x0000, // 1
    0x7000, 0x8800, 0x0800, 0x7000, 0x8000, 0x8000, 0xf800, 0x0000, // 2
    0xf800, 0x0800, 0x1000, 0x3000, 0x0800, 0x8800, 0x7000, 0x0000, // 3
    0x1000, 0x3000, 0x5000, 0x9000, 0xf800, 0x1000, 0x1000, 0x0000, // 4
    0xf800, 0x8000, 0xf000, 0x0800, 0x0800, 0x8800, 0x7000, 0x0000, // 5
    0x3800, 0x4000, 0x8000, 0xf000, 0x8800, 0x8800, 0x7000, 0x0000, // 6
    0xf800, 0x0800, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000, 0x0000, // 7
    0x7000, 0x8800, 0x8800, 0x7000, 0x8800, 0x8800, 0x7000, 0x0000, // 8
    0x7000, 0x8800, 0x8800, 0x7800, 0x0800, 0x1000, 0xe000, 0x0000, // 9
    0x0000, 0x0000, 0x4000, 0x0000, 0x4000, 0x0000, 0x0000, 0x0000, // :
    0x0000, 0x0000, 0x2000, 0x0000, 0x2000, 0x2000, 0x4000, 0x0000, // ;
    0x0800, 0x1000, 0x2000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0000, // <
    0x0000, 0x0000, 0xf800, 0x0000, 0xf800, 0x0000, 0x0000, 0x0000, // =
    0x4000, 0x2000, 0x1000, 0x0800, 0x1000, 0x2000, 0x4000, 0x0000, // >
    0x7000, 0x8800, 0x0800, 0x3000, 0x2000, 0x0000, 0x2000, 0x0000, // ?
    0x7000, 0x8800, 0xa800, 0xb800, 0xb000, 0x8000, 0x7800, 0x0000, // @
    0x2000, 0x5000, 0x8800, 0x8800, 0xf800, 0x8800, 0x8800, 0x0000, // A
    0xf000, 0x8800, 0x8800, 0xf000, 0x8800, 0x8800, 0xf000, 0x0000, // B
    0x7000, 0x8800, 0x8000, 0x8000, 0x8000, 0x8800, 0x7000, 0x0000, // C
    0xf000, 0x8800, 0x8800, 0x8800, 0x8800, 0x8800, 0xf000, 0x0000, // D
    0xf800, 0x8000, 0x8000, 0xf000, 0x8000, 0x8000, 0xf800, 0x0000, // E
    0xf800, 0x8000, 0x8000, 0xf000, 0x8000, 0x8000, 0x8000, 0x0000, // F
    0x7800, 0x8800, 0x8000, 0x8000, 0x9800, 0x8800, 0x7800, 0x0000, // G
    0x8800, 0x8800, 0x8800, 0xf800, 0x8800, 0x8800, 0x8800, 0x0000, // H
    0x7000, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x7000, 0x0000, // I
    0x3800, 0x1000, 0x1000, 0x1000, 0x1000, 0x9000, 0x6000, 0x0000, // J
    0x8800, 0x9000, 0xa000, 0xc000, 0xa000, 0x9000, 0x8800, 0x0000, // K
    0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0xf800, 0x0000, // L
    0x8800, 0xd800, 0xa800, 0xa800, 0xa800, 0x8800, 0x8800, 0x0000, // M
    0x8800, 0x8800, 0xc800, 0xa800, 0x9800, 0x8800, 0x8800, 0x0000, // N
    0x7000, 0x8800, 0x8800, 0x8800, 0x8800, 0x8800, 0x7000, 0x0000, // O
    0xf000, 0x8800, 0x8800, 0xf000, 0x8000, 0x8000, 0x8000, 0x0000, // P
    0x7000, 0x8800, 0x8800, 0x8800, 0xa800, 0x9000, 0x6800, 0x0000, // Q
    0xf000, 0x8800, 0x8800, 0xf000, 0xa000, 0x9000, 0x8800, 0x0000, // R
    0x7000, 0x8800, 0x8000, 0x7000, 0x0800, 0x8800, 0x7000, 0x0000, // S
    0xf800, 0xa800, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x0000, // T
    0x8800, 0x8800, 0x8800, 0x8800, 0x8800, 0x8800, 0x7000, 0x0000, // U
    0x8800, 0x8800, 0x8800, 0x8800, 0x8800, 0x5000, 0x2000, 0x0000, // V
    0x8800, 0x8800, 0x8800, 0xa800, 0xa800, 0xa800, 0x5000, 0x0000, // W
    0x8800, 0x8800, 0x5000, 0x2000, 0x5000, 0x8800, 0x8800, 0x0000, // X
    0x8800, 0x8800, 0x5000, 0x2000, 0x2000, 0x2000, 0x2000, 0x0000, // Y
    0xf800, 0x0800, 0x1000, 0x7000, 0x4000, 0x8000, 0xf800, 0x0000, // Z
    0x7800, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x7800, 0x0000, // [
    0x0000, 0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0000, 0x0000, /* \ */
    0x7800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x7800, 0x0000, // ]
    0x2000, 0x5000, 0x8800, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // ^
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xf800, 0x0000, // _
    0x6000, 0x6000, 0x2000, 0x1000, 0x0000, 0x0000, 0x0000, 0x0000, // `
    0x0000, 0x0000, 0x6000, 0x1000, 0x7000, 0x9000, 0x7800, 0x0000, // a
    0x8000, 0x8000, 0xb000, 0xc800, 0x8800, 0xc800, 0xb000, 0x0000, // b
    0x0000, 0x0000, 0x7000, 0x8800, 0x8000, 0x8800, 0x7000, 0x0000, // c
    0x0800, 0x0800, 0x6800, 0x9800, 0x8800, 0x9800, 0x6800, 0x0000, // d
    0x0000, 0x0000, 0x7000, 0x8800, 0xf800, 0x8000, 0x7000, 0x0000, // e
    0x1000, 0x2800, 0x2000, 0x7000, 0x2000, 0x2000, 0x2000, 0x0000, // f
    0x0000, 0x0000, 0x7000, 0x9800, 0x9800, 0x6800, 0x0800, 0x0000, // g
    0x8000, 0x8000, 0xb000, 0xc800, 0x8800, 0x8800, 0x8800, 0x0000, // h
    0x2000, 0x0000, 0x6000, 0x2000, 0x2000, 0x2000, 0x7000, 0x0000, // i
    0x1000, 0x0000, 0x1000, 0x1000, 0x1000, 0x9000, 0x6000, 0x0000, // j
    0x8000, 0x8000, 0x9000, 0xa000, 0xc000, 0xa000, 0x9000, 0x0000, // k
    0x6000, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x7000, 0x0000, // l
    0x0000, 0x0000, 0xd000, 0xa800, 0xa800, 0xa800, 0xa800, 0x0000, // m
    0x0000, 0x0000, 0xb000, 0xc800, 0x8800, 0x8800, 0x8800, 0x0000, // n
    0x0000, 0x0000, 0x7000, 0x8800, 0x8800, 0x8800, 0x7000, 0x0000, // o
    0x0000, 0x0000, 0xb000, 0xc800, 0xc800, 0xb000, 0x8000, 0x0000, // p
    0x0000, 0x0000, 0x6800, 0x9800, 0x9800, 0x6800, 0x0800, 0x0000, // q
    0x0000, 0x0000, 0xb000, 0xc800, 0x8000, 0x8000, 0x8000, 0x0000, // r
    0x0000, 0x0000, 0x7800, 0x8000, 0x7000, 0x0800, 0xf000, 0x0000, // s
    0x2000, 0x2000, 0xf800, 0x2000, 0x2000, 0x2800, 0x1000, 0x0000, // t
    0x0000, 0x0000, 0x8800, 0x8800, 0x8800, 0x9800, 0x6800, 0x0000, // u
    0x0000, 0x0000, 0x8800, 0x8800, 0x8800, 0x5000, 0x2000, 0x0000, // v
    0x0000, 0x0000, 0x8800, 0x8800, 0xa800, 0xa800, 0x5000, 0x0000, // w
    0x0000, 0x0000, 0x8800, 0x5000, 0x2000, 0x5000, 0x8800, 0x0000, // x
    0x0000, 0x0000, 0x8800, 0x8800, 0x7800, 0x0800, 0x8800, 0x0000, // y
    0x0000, 0x0000, 0xf800, 0x1000, 0x2000, 0x4000, 0xf800, 0x0000, // z
    0x1000, 0x2000, 0x2000, 0x4000, 0x2000, 0x2000, 0x1000, 0x0000, // {
    0x2000, 0x2000, 0x2000, 0x0000, 0x2000, 0x2000, 0x2000, 0x0000, // |
    0x4000, 0x2000, 0x2000, 0x1000, 0x2000, 0x2000, 0x4000, 0x0000, // }
    0x4000, 0xa800, 0x1000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // ~
};

// Define the custom font using the FontDef structure
FontDef CustomFont_6x8 = {
    .FontWidth = 6,
    .FontHeight = 8,
    .data = CustomFont6x8,
};