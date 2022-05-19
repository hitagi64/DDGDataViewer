#ifndef CONTENTPREVIEWER_H
#define CONTENTPREVIEWER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class ContentPreviewer : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    ContentPreviewer();
protected:
    void initializeGL();
    void resizeGL( int w, int h );
    void paintGL();
private:
    QMatrix4x4 projection;
    QOpenGLShaderProgram *basicShaderProgram;

    bool projectionMode2DImage;
};

#endif // CONTENTPREVIEWER_H
