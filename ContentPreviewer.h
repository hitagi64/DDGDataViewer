#ifndef CONTENTPREVIEWER_H
#define CONTENTPREVIEWER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

enum ModelType
{
    MODELTYPE_VERTEX3_COLOR3,
    MODELTYPE_VERTEX3_UV2,
};

struct ModelData
{
    unsigned int vao;
    unsigned int vbo;
    ModelType type;
};

class ContentPreviewer : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    ContentPreviewer();
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
private:
    std::vector<float> generateGrid(int width, int height, float spacing);

    ModelData createModel(void *data, unsigned int dataSize, ModelType type);
    void useModel(ModelData model);
    void deleteModel(ModelData model);

    QMatrix4x4 projection;
    QOpenGLShaderProgram *basicShaderProgram;

    bool projectionMode2DImage;

    float cameraRotH;
    float cameraRotV;
    float distanceFromCamera;

    QPoint lastMousePos;

    ModelData triangle;
    ModelData grid;

    // Opengl
    QOpenGLShaderProgram *makeShaderProgram(QString vertexPath, QString fragmentPath);
    unsigned int makeVBO(void *data, unsigned int dataSize);
    void useVBO(unsigned int vbo);
    void deleteVBO(unsigned int vbo);
    void glPrintError();
};

#endif // CONTENTPREVIEWER_H
