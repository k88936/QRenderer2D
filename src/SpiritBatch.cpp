// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "../include/SpiritBatch.h"

#include <QVector2D>

#include <QOpenGLBuffer>
#include <ranges>

struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
};

//! [0.5]
SpiritBatch::SpiritBatch(): lastTextureId(-1)
{
    initializeOpenGLFunctions();
    init_shaders();
}

SpiritBatch::~SpiritBatch()
{
    for (auto [fst, snd] : textures | std::views::values)
    {
        if (fst)
        {
            fst->destroy();
            delete fst;
        }
        if (snd)
        {
            snd->destroy();
            delete snd;
        }
    }
}

void SpiritBatch::begin()
{
    bindTextureShader();
    constexpr auto camera_pos = QVector3D(0, 0, 1);
    constexpr float camera_zoom = 0.01;
    setColor(QVector4D(1, 1, 1, 1));
    setView(camera_pos, camera_zoom);
}

void SpiritBatch::end()
{
}

void SpiritBatch::setView(const QVector3D& cam_pos, const float cam_zoom)
{
    setView(cam_pos, cam_zoom, 1080, 720);
}


void SpiritBatch::draw(const Texture& texture, const QMatrix4x4& transform)
{
    if (lastTextureId != texture.id)
    {
        bindTexture(texture);
    }
    setTransform(transform);
    flush();
}

//! [0]
void SpiritBatch::init_shaders()
{
    // Compile vertex shader
    if (!shader.addShaderFromSourceFile(QOpenGLShader::Vertex, "assets/t_vshader.glsl"))
        throw std::runtime_error("vertex shader compile error");
    // Compile fragment shader
    if (!shader.addShaderFromSourceFile(QOpenGLShader::Fragment, "assets/t_fshader.glsl"))
        throw std::runtime_error("fragment shader compile error");
    // Link shader pipeline
    if (!shader.link())
        throw std::runtime_error("shader link error");
    if (!shader.bind())
        throw std::runtime_error("shader bind error");
}


void SpiritBatch::setTransform(const QMatrix4x4& matrix)
{
    shader.setUniformValue("transform", matrix);
}

void SpiritBatch::setColor(const QVector4D& color)
{
    shader.setUniformValue("color", color);
}

//! [2]
void SpiritBatch::flush()
{
    glDrawArrays(GL_QUADS, 0, 4);
}

void SpiritBatch::setView(const QMatrix4x4& view)
{
    shader.setUniformValue("mvp_matrix", view);
}

void SpiritBatch::setView(const QVector3D& cam_pos, const float cam_zoom, const int width, const int height)
{
    constexpr qreal zNear = 2.0, zFar = 8.0;
    QMatrix4x4 projection;
    projection.ortho(-width * cam_zoom / 2.0f + cam_pos.x(),
                     width * cam_zoom / 2.0f + cam_pos.x(),
                     -height * cam_zoom / 2.0f + cam_pos.y(),
                     height * cam_zoom / 2.0f + cam_pos.y(), zNear, zFar);
    QMatrix4x4 matrix;
    matrix.translate(0.0, 0.0, cam_pos.z());
    projection *= matrix;
    setView(projection);
}

inline float SpiritBatch::worldToPixel(const float v)
{
    return v / 40;
}

QVector3D SpiritBatch::worldToPixel(const QVector3D v)
{
    return {worldToPixel(v.x()), worldToPixel(v.y()), worldToPixel(v.z())};
}

void SpiritBatch::resisterTexture(const Texture& metaImage)
{
    const auto handle = metaImage.id;
    if (textures.contains(handle))
    {
        return;
    }
    // if (handle == "NONE") {
    //     return;
    // }
    QOpenGLVertexArrayObject* vao;
    QOpenGLTexture* texture;
    for (int i = 0; i < metaImage.config.frames; ++i)
    {
        QRect rect = QRect(i * metaImage.image.width() / metaImage.config.frames, 0,
                           metaImage.image.width() / metaImage.config.frames, metaImage.image.height());
        auto* arrayBuf = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        vao = new QOpenGLVertexArrayObject();
        vao->create();
        arrayBuf->create();
        VertexData vertices[4];
        QVector2D texCoords[4];


        texCoords[0] = QVector2D(0, 0);
        texCoords[1] = QVector2D(1, 0);
        texCoords[2] = QVector2D(1, 1);
        texCoords[3] = QVector2D(0, 1);
        if (metaImage.config.is_raw_size)
        {
            vertices[0] = {
                worldToPixel(QVector3D(-static_cast<float>(rect.height()) * metaImage.config.scale,
                                       -static_cast<float>(rect.width()) * metaImage.config.scale, 0)),
                texCoords[0]
            }; // v0
            vertices[1] = {
                worldToPixel(QVector3D(static_cast<float>(rect.height()) * metaImage.config.scale,
                                       -static_cast<float>(rect.width()) * metaImage.config.scale, 0)),
                texCoords[1]
            }; // v1
            vertices[2] = {
                worldToPixel(QVector3D(static_cast<float>(rect.height()) * metaImage.config.scale,
                                       static_cast<float>(rect.width()) * metaImage.config.scale, 0)),
                texCoords[2]
            }; // v3
            vertices[3] = {
                worldToPixel(QVector3D(-static_cast<float>(rect.height()) * metaImage.config.scale,
                                       static_cast<float>(rect.width()) * metaImage.config.scale, 0)),
                texCoords[3]
            }; // v2
        }
        else
        {
            // Vertex data for face 0
            vertices[0] = {
                QVector3D(
                    -static_cast<float>(rect.width()) / static_cast<float>(rect.height()) * 0.5f * metaImage.config.
                    scale,
                    -0.5f * metaImage.config.scale, 0),
                texCoords[0]
            }; // v0
            vertices[1] = {
                QVector3D(
                    static_cast<float>(rect.width()) / static_cast<float>(rect.height()) * 0.5f * metaImage.config.
                    scale,
                    -0.5f * metaImage.config.scale, 0),
                texCoords[1]
            }; // v1
            vertices[2] = {
                QVector3D(
                    static_cast<float>(rect.width()) / static_cast<float>(rect.height()) * 0.5f * metaImage.config.
                    scale,
                    0.5f * metaImage.config.scale, 0),
                texCoords[2]
            }; // v3
            vertices[3] = {
                QVector3D(
                    -static_cast<float>(rect.width()) / static_cast<float>(rect.height()) * 0.5f * metaImage.config.
                    scale,
                    0.5f * metaImage.config.scale, 0),
                texCoords[3]
            }; // v2
        }
        vao->bind();
        arrayBuf->bind();
        arrayBuf->allocate(vertices, 4 * sizeof(VertexData));
        // Tell OpenGL which VBOs to use
        // Offset for position
        quintptr offset = 0;

        // Tell OpenGL programmable pipeline how to locate vertex position data
        int vertexLocation = shader.attributeLocation("a_position");
        shader.enableAttributeArray(vertexLocation);
        shader.setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

        // Offset for texture coordinate
        offset += sizeof(QVector3D);

        // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
        int texcoordLocation = shader.attributeLocation("a_texcoord");
        shader.enableAttributeArray(texcoordLocation);
        shader.setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));
        arrayBuf->release();
        vao->release();

        texture = new QOpenGLTexture(metaImage.image.copy(rect).flipped());
        // Set nearest filtering mode for texture minification
        texture->setMinificationFilter(QOpenGLTexture::Linear);
        // Set bilinear filtering mode for texture magnification
        texture->setMagnificationFilter(QOpenGLTexture::Linear);
        textures[handle] = std::make_pair(texture, vao);
    }
    if (metaImage.config.frames == 1)
    {
        textures[handle] = std::make_pair(texture, vao);
    }
}


void SpiritBatch::bindTexture(const Texture& texture)
{
    const int handle = texture.id;
    if (!textures.contains(handle))
    {
        resisterTexture(texture);
    }
    textures.at(handle).first->bind();
    textures.at(handle).second->bind();
}

void SpiritBatch::bindTextureShader()
{
    this->shader.bind();
}

//! [2]
