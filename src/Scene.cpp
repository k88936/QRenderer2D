// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "../include/Scene.h"

#include <QMouseEvent>
#include <cmath>
#include <QOpenGLBuffer>
#include <qpainter.h>
#include  <QTime>
#include <QDebug>

// #include "UnitConfigs.h"
// #include "Game.h"
// #include "Unit.h"
// #include "MapConfig.h"

Scene::Scene(QWidget *parent): QOpenGLWidget(parent) {
}


Scene::~Scene() {
    makeCurrent();
    delete batch;
    doneCurrent();
}
float ths = 0;

void Scene::initializeGL() {
    initializeOpenGLFunctions(); // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //! [2]
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);
    // Enable back face culling
    glEnable(GL_CULL_FACE);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendEquation(GL_FUNC_ADD_EXT);
    batch = new SpiritBatch();
}



void Scene::flush() {
    update();
}


//! [5]
void Scene::paintGL() {
    glAlphaFunc(GL_EQUAL, 1);
    glClearColor(0, 1, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glColor3f(48.0 / 256, 246.0 / 256, 217.0 / 256);
    // glLineWidth(2);
    // glBegin(GL_LINES);
    // glVertex3f(0, 0, 0);
    // glVertex3f(1, 0, 0);
    // glEnd();


    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

    batch->begin();
    batch->setView(camera_pos, camera_zoom, this->size().width(), this->size().height());
    render(*batch);

    glAlphaFunc(GL_GREATER, 0);
    // batch_draw(game->var_transparent_image_draw_config_map);
    // batch_draw(game->ui_image_draw_config_map);

    batch->end();
}

void Scene::resizeGL(int w, int h) {
    QOpenGLWidget::resizeGL(w, h);
}

void Scene::resizeEvent(QResizeEvent *event) {
    QOpenGLWidget::resizeEvent(event);
}

QVector3D Scene::screen_to_world(const QPointF &screen_pos) const {
    const QPointF relate = screen_pos - this->pos();
    float x = relate.x() * camera_zoom + camera_pos.x() - this->size().width() * camera_zoom / 2;
    float y = -relate.y() * camera_zoom + camera_pos.y() + this->size().height() * camera_zoom / 2;
    return {x, y, 0};
}

QVector3D Scene::screen_relative_to_world_relative(const QPointF &screen_relative) const {
    return {
        static_cast<float>(screen_relative.x() * camera_zoom), static_cast<float>(-screen_relative.y() * camera_zoom), 0
    };
}
