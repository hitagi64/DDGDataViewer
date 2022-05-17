#include "inspector.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Inspector w;
    w.show();
    return a.exec();
}
