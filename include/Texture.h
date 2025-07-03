//
// Created by root on 11/30/24.
//

#ifndef METAIMAGE_H
#define METAIMAGE_H
#include <qimage.h>

#include <utility>

struct Texture
{
    QImage image;
    float scale = 1;
    bool is_raw_size = false;
    int frames = 1;
    int id;

    explicit Texture(QImage image, const int frames = 1, const bool is_raw_size = false, const float scale = 1): image(
        std::move(
            image)), scale(scale), is_raw_size(is_raw_size), frames(frames), id(genId())
    {
    }

    static int genId()
    {
        static int id = 0;
        return id++;
    }
};
#endif //METAIMAGE_H
