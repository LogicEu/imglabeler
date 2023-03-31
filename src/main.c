#define SPXE_APPLICATION
#define UTOPIA_IMPLEMENTATION

#include <spxe.h>
#include <imgtool.h>
#include <utopia/vector.h>
#include <utopia/map.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define WIDTH 800
#define HEIGHT 600

static int iml_path_check(const char* path)
{
    static const char* extensions[] = {
        ".jpg", ".jpeg", ".JPG", ".JPEG", ".png", ".PNG", 
        ".gif", ".GIF", ".ppm", ".PPM", NULL
    };

    size_t i;
    const char* dot = NULL;
    for (i = 0; path[i]; ++i) {    
        if (path[i] == '.') {
            dot = path + i;
        }
    }

    if (dot) {
        for (i = 0; extensions[i]; ++i) {
            if (!strcmp(dot, extensions[i])) {
                return 1;
            }
        }
    }
    return 0;
}

static void iml_plot(Px* pixbuf, const bmp_t* bmp)
{
    const int x = WIDTH / 2 - bmp->width / 2;
    const int y = HEIGHT / 2 - bmp->height / 2;
    memset(pixbuf, 155, WIDTH * HEIGHT * sizeof(Px));
    for (unsigned int i = 0; i < bmp->height; ++i) {
        memcpy(
            pixbuf + (y + i) * WIDTH + x, 
            bmp->pixels + (bmp->height - 1 - i) * bmp->width * sizeof(Px),
            sizeof(Px) * bmp->width
        );
    }
}

static void iml_load(Px* pixbuf, const char* imgpath)
{
    bmp_t bmp = bmp_load(imgpath);
    if (!bmp.pixels) {
        fprintf(stderr, "could not open image file: %s\n", imgpath);
        return;
    }

    if (bmp.channels != 4) {
        if (bmp.channels != 3) {
            fprintf(stderr, "image file is not RGBA or RGB: %s\n", imgpath);
            return;
        }

        uint8_t* rgba = rgb_to_rgba(bmp.pixels, bmp.width, bmp.height);
        if (!rgba) {
            fprintf(
                stderr, 
                "could not transform to RGBA image: %s\n",
                imgpath)
            ;
            return;
        }

        free(bmp.pixels);
        bmp.pixels = rgba;
        bmp.channels = 4;
    }

    while (bmp.width > WIDTH | bmp.height > HEIGHT) {
        bmp_t new_bmp = bmp_reduce(&bmp);
        bmp_free(&bmp);
        bmp = new_bmp;
    }

    iml_plot(pixbuf, &bmp);
    bmp_free(&bmp);
}

int main(const int argc, const char** argv)
{
    struct vector images = vector_create(sizeof(const char*));
    for (int i = 1; i < argc; ++i) {
        if (iml_path_check(argv[i])) {
            vector_push(&images, &argv[i]);
        }
    }

    if (!images.size) {
        printf("Missing input image.");
        return EXIT_FAILURE;
    }

    size_t index = 0, changed = 0;
    const char** imagepaths = images.data;
    Px* pixbuf = spxeStart("imglabeler", 800, 600, WIDTH, HEIGHT);
    memset(pixbuf, 155, WIDTH * HEIGHT * sizeof(Px));
    iml_load(pixbuf, imagepaths[index++]);
    
    while (spxeRun(pixbuf)) {
        if (spxeKeyPressed(ESCAPE)) {
            break;
        }
        
        if (spxeKeyPressed(RIGHT)) {
            index = (index + 1) % images.size;
            ++changed;
        }
        if (spxeKeyPressed(LEFT)) {
            index = !index ? images.size - 1 : index - 1;
            ++changed;
        }

        if (changed) {
            iml_load(pixbuf, imagepaths[index]);
            --changed;
        }
    }

    vector_free(&images);
    return spxeEnd(pixbuf);
}

