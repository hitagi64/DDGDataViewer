#include "ContentPreviewer.h"
#include <cmath>
#include <QTimer>
#include <QMouseEvent>
#include <iostream>

#include "DDG/DDGTxm.h"
#include "DDG/DDGPdb.h"

ContentPreviewer::ContentPreviewer(QWidget *parent) : QOpenGLWidget(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&ContentPreviewer::update));
    timer->start(10);

    image2DMode = false;
    pdmMode = false;

    cameraRotH = 0;
    cameraRotV = 0;

    distanceFromCamera = 10;

    imagePreviewTexture = -1;
}

void ContentPreviewer::displayContent(DDGContent *c)
{
    // c may not be used outside this function
    QOpenGLWidget::makeCurrent();

    image2DMode = false;
    pdmMode = false;

    DDGTxm *cI = dynamic_cast<DDGTxm*>(c);
    if (cI != nullptr)
    {
        DDGImage img = cI->convertToImage();

        if (imagePreviewTexture != -1)
            deleteTexture(imagePreviewTexture);

        imagePreviewTexture = makeTexture(img.data.get(), img.width, img.height, GL_RGBA);

        image2DMode = true;
    }
    DDGPdb *cM = dynamic_cast<DDGPdb*>(c);
    if (cM != nullptr)
    {
        std::vector<DDGVector4> bounds = cM->getBoundsVertices();

        boundsModel = createModel(bounds.data(), bounds.size()*sizeof(DDGVector4),
                bounds.size(), MODELTYPE_4F, GL_POINTS);

        pdmMode = true;
    }
}

void ContentPreviewer::initializeGL()
{
    QOpenGLWidget::initializeGL();
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);

    float vertices[] = {
        -1.0f, -1.0f, 0.0f,// Bottom left
         1.0f, 0.0f, 0.0f,

         1.0f, -1.0f, 0.0f,// Bottom right
         0.0f, 1.0f, 0.0f,

         0.0f, 1.0f, 0.0f,// Top center
         0.0f, 0.0f, 1.0f,

        /*-1.0f, 1.0f, 0.0f,// Top left
         1.0f, 0.0f, 0.0f,

         1.0f, 1.0f, 0.0f,// Top right
         0.0f, 1.0f, 0.0f,*/
    };
    unsigned int indices[] {
        0, 1, 2,
        0, 3, 2,
        1, 4, 2
    };

    basicShaderProgram = makeShaderProgram("Res/vertex.glsl", "Res/fragment.glsl");
    basicShaderProgram->link();

    imagePreviewShader = makeShaderProgram("Res/vertexUnlitTexture.glsl", "Res/fragmentUnlitTexture.glsl");
    imagePreviewShader->link();

    litShader = makeShaderProgram("Res/whiteLitVertex.glsl", "Res/whiteLitFragment.glsl");
    litShader->link();

    greenUnlitShader = makeShaderProgram("Res/greenUnlitVertex.glsl", "Res/greenUnlitFragment.glsl");
    greenUnlitShader->link();

    triangle = createModel(vertices, sizeof(vertices),
                           3, MODELTYPE_3F_3F, GL_TRIANGLES);
    //triangle = createModelIndexed(vertices, sizeof(vertices), indices, sizeof(indices), 9, MODELTYPE_VERTEX3_COLOR3, GL_TRIANGLES);

    std::vector<float> planeData = generatePlaneUV();
    imageSurface = createModel(&planeData[0], planeData.size()*sizeof(float),
            planeData.size()/5, MODELTYPE_3F_2F, GL_TRIANGLES);

    std::vector<float> gridData = generateGrid(20, 20, 1);
    grid = createModel(&gridData[0], gridData.size()*sizeof(float),
            gridData.size()/6, MODELTYPE_3F_3F, GL_LINES);

    std::vector<float> cubeData = generateCubeNormal();
    cube = createModel(&cubeData[0], cubeData.size()*sizeof(float),
           cubeData.size()/6, MODELTYPE_3F_3F, GL_TRIANGLES);
}

void ContentPreviewer::resizeGL(int w, int h)
{
    projection.setToIdentity();
    double p = qreal(width())/qreal(height() > 0 ? height() : 1);
    if (image2DMode)
    {
        if (p > 1)
            projection.ortho(-p, p, -1, 1, 0.01f, 100.0);
        else
            projection.ortho(-1, 1, -(1/p), (1/p), 0.01f, 100.0);
    }
    else
        projection.perspective(45, qreal(width())/qreal(height() > 0 ? height() : 1), 0.1f, 10000.0f);
}

void ContentPreviewer::paintGL()
{
    static float i = 0;
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 view;
    if (image2DMode)
    {
        view.lookAt(QVector3D(0, 0, -2), QVector3D(0, 0, 0), QVector3D(0, 1, 0));

        QMatrix4x4 model;
        model.translate(0, 0, 0);
        i+=1;

        QMatrix4x4 mvp;
        mvp = projection * view * model;

        imagePreviewShader->bind();
        imagePreviewShader->setUniformValue("mvp", mvp);

        useTexture(imagePreviewTexture);

        drawModel(imageSurface);
    }
    else
    {
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

        if (pdmMode)
        {
            glPointSize(10);

            greenUnlitShader->bind();
            greenUnlitShader->setUniformValue("mvp", mvp);
            drawModel(boundsModel);
        }
        else
        {
            litShader->bind();
            litShader->setUniformValue("mvp", mvp);
            drawModel(cube);

            basicShaderProgram->bind();
            basicShaderProgram->setUniformValue("mvp", mvp);
            drawModel(triangle);
        }

        basicShaderProgram->bind();
        basicShaderProgram->setUniformValue("mvp", mvp);
        drawModel(grid);
    }
}

void ContentPreviewer::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->pos();
}

void ContentPreviewer::mouseMoveEvent(QMouseEvent *event)
{
    if (!image2DMode)
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
    }

    lastMousePos = event->pos();
}

void ContentPreviewer::wheelEvent(QWheelEvent *event)
{
    if (image2DMode)
        return;
    distanceFromCamera += event->angleDelta().y()/-30.0f;
    if (distanceFromCamera < 0.2)
        distanceFromCamera = 0.2;
}

QSize ContentPreviewer::sizeHint() const
{
    return {300, 300};
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

std::vector<float> ContentPreviewer::generatePlaneUV()
{
    float vertices[] = {
        -1.0f, -1.0f, 0.0f,// Bottom left
         0.0f, 0.0f,

        -1.0f, 1.0f, 0.0f,// Top left
         0.0f, 1.0f,

         1.0f, 1.0f, 0.0f,// Top right
         1.0f, 1.0f,


        -1.0f, -1.0f, 0.0f,// Bottom left
         0.0f, 0.0f,

         1.0f, -1.0f, 0.0f,// Bottom right
         1.0f, 0.0f,

         1.0f, 1.0f, 0.0f,// Top right
         1.0f, 1.0f,
    };

    return std::vector<float>(std::begin(vertices), std::end(vertices));
}

std::vector<float> ContentPreviewer::generateCubeNormal()
{
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    return std::vector<float>(std::begin(vertices), std::end(vertices));
}

ModelData ContentPreviewer::createModel(void *data, unsigned int dataSize,
                                        unsigned int drawCount, ModelDataType type,
                                        GLenum drawType)
{
    ModelData model;
    model.drawCount = drawCount;
    model.usesEBO = false;
    model.drawType = drawType;

    glGenVertexArrays(1, &model.vao);
    glBindVertexArray(model.vao);

    model.vbo = makeVBO(data, dataSize);

    if (type == MODELTYPE_3F_3F)
    {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    else if (type == MODELTYPE_3F_2F)
    {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    else if (type == MODELTYPE_4F)
    {
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
    }
    else
    {
        std::cerr << "Unknown model type!" << std::endl;
    }

    return model;
}

ModelData ContentPreviewer::createModelIndexed(void *vertexData, unsigned int vertexDataSize,
                                               void *indexData, unsigned int indexDataSize,
                                               unsigned int drawCount, ModelDataType type,
                                               GLenum drawType)
{
    ModelData d = createModel(vertexData, vertexDataSize, drawCount, type, drawType);

    d.usesEBO = true;

    glGenBuffers(1, &d.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, indexData, GL_STATIC_DRAW);

    return d;
}

void ContentPreviewer::drawModel(ModelData model)
{
    useModel(model);
    if (model.usesEBO == false)
    {
        glDrawArrays(model.drawType, 0, model.drawCount);
    }
    else
    {
        glDrawElements(model.drawType, model.drawCount, GL_UNSIGNED_INT, 0);
    }
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
    std::cout << glGetError() << std::endl;
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    std::cout << glGetError() << std::endl;
    glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
    std::cout << glGetError() << std::endl;

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

unsigned int ContentPreviewer::makeTexture(void *data, unsigned int width, unsigned int height, GLenum type)
{
    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, type, GL_UNSIGNED_BYTE, data);
    //glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return tex;
}

void ContentPreviewer::useTexture(unsigned int tex)
{
    glBindTexture(GL_TEXTURE_2D, tex);
}

void ContentPreviewer::deleteTexture(unsigned int tex)
{
    glDeleteTextures(1, &tex);
}
