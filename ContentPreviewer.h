#ifndef CONTENTPREVIEWER_H
#define CONTENTPREVIEWER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include "DDG/DDGContent.h"
#include "DDG/DDGPdb.h"
#include "DDG/DDGDat.h"

enum ModelDataType
{
    MODELTYPE_3F_3F,
    MODELTYPE_3F_2F,
    MODELTYPE_3F_3F_2F,
    MODELTYPE_4F,
    MODELTYPE_3F,
};

struct ModelData
{
    unsigned int vao = 0;
    unsigned int vbo = 0;

    unsigned int ebo = 0;
    bool usesEBO = false;

    unsigned int drawCount = 0;// e.g. amount of triangles or lines.
    GLenum drawType;
};

struct ModelTextured
{
    ModelData data;
    unsigned int texture = 0;
};

class ContentPreviewer : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    ContentPreviewer(QWidget *parent);

    // Preview specified DDGContent
    void displayContent(DDGContent *c);
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    QSize sizeHint() const override;
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
private:
    // Projection
    void recalculateProjection();
    QMatrix4x4 projection;

    // Shaders
    QOpenGLShaderProgram *basicShaderProgram;
    QOpenGLShaderProgram *imagePreviewShader;
    QOpenGLShaderProgram *litShader;
    QOpenGLShaderProgram *texturedLitShader;
    QOpenGLShaderProgram *greenUnlitShader;

    // Modes
    bool image2DMode;
    bool pdbMode;
    bool areaPointsMode;
    bool areaLinesMode;

    // Camera
    float cameraRotH;
    float cameraRotV;
    float distanceFromCamera;
    QPoint lastMousePos;
    QVector3D direction;
    QVector3D position;
public:
    bool fastMode;
    bool flyMode;

private:
    // Base models
    ModelData triangle;
    ModelData grid;
    ModelData cube;

    // image2DMode
    ModelData imageSurface;
    unsigned int imagePreviewTexture;// 0 if doesn't exist
    float imageAspectRatio;

    // pdmMode
    ModelData boundsModel;
    std::vector<ModelTextured> meshes;

    // Area mode
    ModelData areaPointsModel;
    ModelData areaLinesModel;
public:
    DDGDat *textureLib;
private:
    //ModelData seg1Model;
    void loadModelSegment(DDGModelSegment &seg);

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
    std::vector<float> generateCubeNormal();

    ModelData createModel(void *data, unsigned int dataSize, unsigned int drawCount, ModelDataType type, GLenum drawType);
    ModelData createModelIndexed(void *data, unsigned int vertexDataSize, void *indexData, unsigned int indexDataSize, unsigned int drawCount, ModelDataType type, GLenum drawType);
    void drawModel(ModelData model);
    void useModel(ModelData model);
    void deleteModel(ModelData model);
};

#endif // CONTENTPREVIEWER_H
