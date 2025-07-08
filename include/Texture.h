//
// Created by root on 11/30/24.
//

#ifndef METAIMAGE_H
#define METAIMAGE_H
#include <qimage.h>
#include <qdebug.h>
#include <utility>

struct Texture
{
    QImage image;

    static int genId()
    {
        static int id = 0;
        return id++;
    }
    struct Config
    {
        float scale = 1;
        bool is_raw_size = false;
        int frames = 1;
    } config;

    int id = genId();
};
#endif //METAIMAGE_H
