#ifndef CONTENTPREVIEWER_H
#define CONTENTPREVIEWER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include "DDG/DDGContent.h"
#include "DDG/DDGTxm.h"

enum ModelDataType
{
    MODELTYPE_VERTEX3_COLOR3,
    MODELTYPE_VERTEX3_UV2,
};

struct ModelData
{
    unsigned int vao;
    unsigned int vbo;
    ModelDataType type;

    unsigned int ebo;
    bool usesEBO;

    unsigned int drawCount;// e.g. amount of triangles or lines.
    GLenum drawType;
};

class ContentPreviewer : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    ContentPreviewer(QWidget *parent);

    void displayContent(DDGContent *c);
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    QSize sizeHint() const override;
private:
    QMatrix4x4 projection;
    QOpenGLShaderProgram *basicShaderProgram;
    QOpenGLShaderProgram *imagePreviewShader;

    bool image2DMode;

    float cameraRotH;
    float cameraRotV;
    float distanceFromCamera;

    QPoint lastMousePos;

    ModelData triangle;
    ModelData imageSurface;
    ModelData grid;

    int imagePreviewTexture;// -1 if doesn't exist

    // Opengl
    QOpenGLShaderProgram *makeShaderProgram(QString vertexPath, QString fragmentPath);
    unsigned int makeVBO(void *data, unsigned int dataSize);
    void useVBO(unsigned int vbo);
    void deleteVBO(unsigned int vbo);

    unsigned int makeTexture(void* data, unsigned int width, unsigned int height, GLenum type);
    void useTexture(unsigned int tex);
    void deleteTexture(unsigned int tex);

    std::vector<float> generateGrid(int width, int height, float spacing);
    std::vector<float> generatePlaneUV();

    ModelData createModel(void *data, unsigned int dataSize, unsigned int drawCount, ModelDataType type, GLenum drawType);
    ModelData createModelIndexed(void *data, unsigned int vertexDataSize, void *indexData, unsigned int indexDataSize, unsigned int drawCount, ModelDataType type, GLenum drawType);
    void drawModel(ModelData model);
    void useModel(ModelData model);
    void deleteModel(ModelData model);
};

#endif // CONTENTPREVIEWER_H
