#include "font.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#define OFF_Y 14

Font* fontLoad(const char* path, const unsigned int size)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        printf("ERROR::FREETYPE: Could not init FreeType Library\n");
        return NULL;
    } 
	
    FT_Face face;
    if (FT_New_Face(ft, path, 0, &face)) {
        printf("ERROR::FREETYPE: Failed to load font\n");
        return NULL;
    }

    FT_Set_Pixel_Sizes(face, 0, size);
    Font* font = (Font*)malloc(sizeof(Font) * 128);

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            printf("ERROR::FREETYTPE: Failed to load Glyph\n");
            continue;
        }

        font[c] = (Font){
            malloc(face->glyph->bitmap.width * face->glyph->bitmap.rows),
            {face->glyph->bitmap.width, face->glyph->bitmap.rows},
            {face->glyph->bitmap_left, face->glyph->bitmap_top},
            (unsigned int)face->glyph->advance.x
        };
        memcpy(font[c].pixmap, face->glyph->bitmap.buffer, face->glyph->bitmap.width * face->glyph->bitmap.rows);
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return font;
}

static inline float lerpf(const float min, const float max, const float t)
{
    return min + t * (max - min);
}

static inline Px pxlerp(const Px p0, const Px p1, const float t)
{
    return (Px){
        (int)lerpf(p0.r, p1.r, t),
        (int)lerpf(p0.g, p1.g, t),
        (int)lerpf(p0.b, p1.b, t),
        (int)lerpf(p0.a, p1.a, t)
    };
}

static void fontDrawChar(
    Px* pixbuf, const Font font, const int xpos, const int ypos
)
{
    const Px red = {255, 0, 0, 255};
    const int xend = xpos + font.size.x;
    const int yend = ypos + font.size.y;
    int w, h;
    spxeScreenSize(&w, &h);
    for (int y = ypos; y < yend; ++y) {
        for (int x = xpos; x < xend; ++x) {
            Px* p = pixbuf + y * w + x;
            unsigned char c = font.pixmap[(font.size.y - 1 - (y - ypos)) * font.size.x + (x - xpos)];
            *p = pxlerp(*p, red, (float)c / 255.0);
        }
    }
}

void fontDrawText(
    Px* pixbuf, const Font* font, const char* text, int x, int y
)
{
    for (int i = 0; text[i]; i++) {
        fontDrawChar(pixbuf, font[(int)text[i]], x, y);
        x += (font[(int)text[i]].advance >> 6);
    }
}

void fontFree(Font* font)
{
    for (int i = 0; i < 128; ++i) {
        free(font[i].pixmap);
    }
    free(font);
}

