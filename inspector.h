#ifndef INSPECTOR_H
#define INSPECTOR_H

#include <QMainWindow>
#include "DDG/DDGDat.h"
#include <QTreeWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class Inspector; }
QT_END_NAMESPACE

class Inspector : public QMainWindow
{
    Q_OBJECT

public:
    Inspector(QWidget *parent = nullptr);
    ~Inspector();

    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private slots:
    void on_actionExit_triggered();
    void on_actionOpen_triggered();
    void on_ItemView_itemClicked(QTreeWidgetItem *item, int column);

    void on_actionDat_triggered();

    void on_actionTxm_triggered();

    void on_ItemView_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_actionPdb_triggered();

private:
    Ui::Inspector *ui;

    void loadNewFile();
    bool parseFile(std::string filename);
    void renderDat();
    void renderDatChildren(DDGDat *d, QTreeWidgetItem *parent);

    std::vector<std::shared_ptr<DDGContent>> dats;
    std::vector<std::string> datNames;
};
#endif // INSPECTOR_H