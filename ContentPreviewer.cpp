#include "ContentPreviewer.h"
#include <cmath>
#include <QTimer>
#include <QMouseEvent>
#include <iostream>

ContentPreviewer::ContentPreviewer()
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&ContentPreviewer::update));
    timer->start(10);

    projectionMode2DImage = false;

    cameraRotH = 0;
    cameraRotV = 0;

    distanceFromCamera = 10;
}

void ContentPreviewer::initializeGL()
{
    QOpenGLWidget::initializeGL();
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);

    float vertices[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, 0.0f, 0.0f,

         1.0f, -1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,

         0.0f, 1.0f, 0.0f,
         0.0f, 0.0f, 1.0f
    };

    basicShaderProgram = makeShaderProgram("Res/vertex.glsl", "Res/fragment.glsl");
    basicShaderProgram->link();
    basicShaderProgram->bind();

    triangle = createModel(vertices, sizeof(vertices), MODELTYPE_VERTEX3_COLOR3);

    std::vector<float> gridData = generateGrid(20, 20, 1);
    grid = createModel(&gridData[0], gridData.size()*sizeof(float), MODELTYPE_VERTEX3_COLOR3);

    glPrintError();
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
        view.lookAt(QVector3D(
                        sin(cameraRotH * (3.1415f/180))*-distanceFromCamera*cos(cameraRotV * (3.1415f/180)),
                        sin(cameraRotV * (3.1415f/180))*-distanceFromCamera,
                        cos(cameraRotH * (3.1415f/180))*-distanceFromCamera*cos(cameraRotV * (3.1415f/180))
                        ), QVector3D(0, 0, 0), QVector3D(0, 1, 0));

    QMatrix4x4 model;
    model.translate(0, 0, 0);
    i+=1;

    QMatrix4x4 mvp;
    mvp = projection * view * model;

    basicShaderProgram->setUniformValue("mvp", mvp);

    useModel(triangle);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    useModel(grid);
    glDrawArrays(GL_LINES, 0, 21*21);
}

void ContentPreviewer::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->pos();
}

void ContentPreviewer::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastMousePos.x();
    int dy = event->y() - lastMousePos.y();

    if (event->buttons() & Qt::LeftButton)
    {
        cameraRotH -= dx/2.0f;

        cameraRotV -= dy/2.0f;
        if (cameraRotV < -90)
            cameraRotV = -90;
        if (cameraRotV > 90)
            cameraRotV = 90;
    }

    lastMousePos = event->pos();
}

void ContentPreviewer::wheelEvent(QWheelEvent *event)
{
    distanceFromCamera += event->angleDelta().y()/-30.0f;
    if (distanceFromCamera < 0.2)
        distanceFromCamera = 0.2;
}

std::vector<float> ContentPreviewer::generateGrid(int width, int height, float spacing)
{
    std::vector<float> grid;

    for (int i = width/-2; i <= width/2; i++)
    {
        grid.push_back((height/2)*spacing);
        grid.push_back(0);
        grid.push_back(i*spacing);

        grid.push_back(0.5f);
        grid.push_back(0.5f);
        grid.push_back(0.5f);

        grid.push_back((height/-2)*spacing);
        grid.push_back(0);
        grid.push_back(i*spacing);

        grid.push_back(0.5f);
        grid.push_back(0.5f);
        grid.push_back(0.5f);
    }

    for (int i = height/-2; i <= height/2; i++)
    {
        grid.push_back(i*spacing);
        grid.push_back(0);
        grid.push_back((width/2)*spacing);

        grid.push_back(0.5f);
        grid.push_back(0.5f);
        grid.push_back(0.5f);

        grid.push_back(i*spacing);
        grid.push_back(0);
        grid.push_back((width/-2)*spacing);

        grid.push_back(0.5f);
        grid.push_back(0.5f);
        grid.push_back(0.5f);
    }

    return grid;
}

ModelData ContentPreviewer::createModel(void *data, unsigned int dataSize, ModelType type)
{
    ModelData model;

    glGenVertexArrays(1, &model.vao);

    model.vbo = makeVBO(data, dataSize);

    glBindVertexArray(model.vao);

    if (type == MODELTYPE_VERTEX3_COLOR3)
    {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    else if (type == MODELTYPE_VERTEX3_UV2)
    {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
    }


    return model;
}

void ContentPreviewer::useModel(ModelData model)
{
    glBindVertexArray(model.vao);
}

void ContentPreviewer::deleteModel(ModelData model)
{
    glDeleteVertexArrays(1, &model.vao);
    deleteVBO(model.vbo);
}

QOpenGLShaderProgram *ContentPreviewer::makeShaderProgram(QString vertexPath, QString fragmentPath)
{
    QOpenGLShaderProgram *p = new QOpenGLShaderProgram(QOpenGLContext::currentContext());

    QOpenGLShader vertex(QOpenGLShader::Vertex);
    vertex.compileSourceFile(vertexPath);
    p->addShader(&vertex);

    QOpenGLShader fragment(QOpenGLShader::Fragment);
    fragment.compileSourceFile(fragmentPath);
    p->addShader(&fragment);

    return p;
}

unsigned int ContentPreviewer::makeVBO(void *data, unsigned int dataSize)
{
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);

    return vbo;
}

void ContentPreviewer::useVBO(unsigned int vbo)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

void ContentPreviewer::deleteVBO(unsigned int vbo)
{
    glDeleteBuffers(1, &vbo);
}

void ContentPreviewer::glPrintError()
{
    std::cout << glGetError() << std::endl;
}
