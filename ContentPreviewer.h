#ifndef CONTENTPREVIEWER_H
#define CONTENTPREVIEWER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QSlider>

#include "DDG/DDGContent.h"
#include "DDG/DDGPdb.h"
#include "DDG/DDGDat.h"
#include "DDG/DDGMapModelLUT.h"

enum MeshDataType
{
    MODELTYPE_3F_3F,
    MODELTYPE_3F_2F,
    MODELTYPE_3F_3F_2F,
    MODELTYPE_4F,
    MODELTYPE_3F,
};

struct MeshData
{
    unsigned int vao = 0;
    unsigned int vbo = 0;

    unsigned int ebo = 0;
    bool usesEBO = false;

    unsigned int drawCount = 0;// e.g. amount of triangles or lines.
    GLenum drawType;
};

struct MeshTextured
{
    MeshData data;
    unsigned int texture = 0;
};

struct ModelData
{
    std::vector<MeshTextured> meshes;
    QMatrix4x4 transform;
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
    bool modelsMode;

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
    MeshData triangle;
    MeshData grid;
    MeshData cube;

    // image2DMode
    MeshData imageSurface;
    unsigned int imagePreviewTexture;// 0 if doesn't exist
    float imageAspectRatio;

    // pdmMode
    MeshData boundsModel;
    std::vector<MeshTextured> meshes;

    // Area mode
    MeshData areaPointsModel;
    MeshData areaLinesModel;

    // Models mode
    std::vector<ModelData> models;
public:
    DDGDat *textureLib;
    DDGDat *modelLib;
    DDGMapModelLUT *modelLUT;
private:
    //ModelData seg1Model;
    void loadModelSegment(DDGModelSegment &seg, std::vector<MeshTextured> &texturedMeshes);

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

    MeshData createModel(void *data, unsigned int dataSize, unsigned int drawCount, MeshDataType type, GLenum drawType);
    MeshData createModelIndexed(void *data, unsigned int vertexDataSize, void *indexData, unsigned int indexDataSize, unsigned int drawCount, MeshDataType type, GLenum drawType);
    void drawModel(MeshData model);
    void useModel(MeshData model);
    void deleteModel(MeshData model);
};

#endif // CONTENTPREVIEWER_H
