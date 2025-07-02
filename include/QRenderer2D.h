// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QOpenGLWidget>

#include "SpiritBatch.h"


class QRenderer2D : public QOpenGLWidget, QOpenGLFunctions {
public:
    using QOpenGLWidget::QOpenGLWidget;
    [[nodiscard]] QVector3D screen_to_world(const QPointF &screen_pos) const;
    [[nodiscard]] QVector3D screen_relative_to_world_relative(const QPointF &screen_relative) const;
    ~QRenderer2D() override;
    void flush();
    virtual void render(SpiritBatch & batch) =0;
    QVector3D camera_pos = QVector3D(0, 0, -5);
    float camera_zoom = 0.01;
protected:
    explicit QRenderer2D(QWidget *parent);
private:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void resizeEvent(QResizeEvent *event) override;
    QOpenGLShaderProgram program;
    SpiritBatch *batch = nullptr;
    QQuaternion rotation;
};

#endif // MAINWIDGET_H
