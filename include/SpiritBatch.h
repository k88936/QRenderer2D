// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef RENDER_UTILS_H
#define RENDER_UTILS_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include "Texture.h"


class SpiritBatch final : QOpenGLFunctions
{
public:
    SpiritBatch();

    ~SpiritBatch();

    void begin();
    void end();

    void draw( const Texture & texture,const QMatrix4x4& transform);

    void bindTexture(const Texture &texture);

    void setColor(const QVector4D& color);
    void setView(const QMatrix4x4& matrix4_x4);
    void setView(const QVector3D &cam_pos, float cam_zoom);
    void setView(const QVector3D &cam_pos, float cam_zoom, int width, int height);

    static  float worldToPixel(float v);
    static QVector3D worldToPixel(QVector3D  v);
private:
    void resisterTexture(const Texture& metaImage);
    void bindTextureShader();
    void flush();
    void init_shaders();
    void setTransform(const QMatrix4x4& matrix);
    std::unordered_map<int, std::pair<QOpenGLTexture*, QOpenGLVertexArrayObject*>> textures;
    QOpenGLShaderProgram shader;
    QMatrix4x4 view;
    int lastTextureId=-1;
};

#endif // RENDER_UTILS_H
