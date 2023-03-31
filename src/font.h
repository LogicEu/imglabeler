#ifndef SPXE_FONT_H
#define SPXE_FONT_H

#include <spxe.h>

typedef struct Font {
    unsigned char* pixmap;
    struct ivec2 {
        int x;
        int y;
    } size, bearing;
    unsigned int advance;
} Font;

Font* fontLoad(const char* path, const unsigned int size);
void fontFree(Font* font);
void fontDrawText(
    Px* pixbuf, const Font* font, const char* text, int x, int y
);

#endif /* SPXE_FONT_H */

