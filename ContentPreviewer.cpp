#include "ContentPreviewer.h"
#include <cmath>
#include <QTimer>

ContentPreviewer::ContentPreviewer()
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&ContentPreviewer::update));
    timer->start(10);

    projectionMode2DImage = true;
}

void ContentPreviewer::initializeGL()
{
    QOpenGLWidget::initializeGL();
    initializeOpenGLFunctions();

    float vertices[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, 0.0f, 0.0f,

         1.0f, -1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,

         0.0f, 1.0f, 0.0f,
         0.0f, 0.0f, 1.0f
    };

    basicShaderProgram = new QOpenGLShaderProgram(QOpenGLContext::currentContext());

    QOpenGLShader vertex(QOpenGLShader::Vertex);
    vertex.compileSourceFile("Res/vertex.glsl");
    basicShaderProgram->addShader(&vertex);

    QOpenGLShader fragment(QOpenGLShader::Fragment);
    fragment.compileSourceFile("Res/fragment.glsl");
    basicShaderProgram->addShader(&fragment);

    basicShaderProgram->link();
    basicShaderProgram->bind();

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
}

void ContentPreviewer::resizeGL(int w, int h)
{
    projection.setToIdentity();
    double p = qreal(width())/qreal(height() > 0 ? height() : 1);
    if (projectionMode2DImage)
    {
        if (p > 1)
            projection.ortho(-p, p, -1, 1, 0.01f, 100.0);
        else
            projection.ortho(-1, 1, -(1/p), (1/p), 0.01f, 100.0);
    }
    else
        projection.perspective(45, qreal(width())/qreal(height() > 0 ? height() : 1), 0.1f, 100.0f);
}

void ContentPreviewer::paintGL()
{
    static float i = 0;
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 view;
    if (projectionMode2DImage)
        view.lookAt(QVector3D(0, 0, -2), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
    else
        view.lookAt(QVector3D(0, 0, -2), QVector3D(0, 0, 0), QVector3D(0, 1, 0));

    QMatrix4x4 model;
    model.translate(0, 0, 0);
    i+=1;

    QMatrix4x4 mvp;
    mvp = projection * view * model;

    basicShaderProgram->setUniformValue("mvp", mvp);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}
