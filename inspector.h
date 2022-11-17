#ifndef INSPECTOR_H
#define INSPECTOR_H

#include <QMainWindow>
#include "DDG/DDGDat.h"
#include "DDG/DDGTxm.h"
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
    void on_actionDat_triggered();
    void on_actionTxm_triggered();
    void on_ItemView_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_actionPdb_triggered();
    void on_actionPdm_get_before_buffer_1_vals_triggered();
    void on_actionSet_DAT_as_active_texture_lib_triggered();
    void on_actionFly_Mode_triggered();
    void on_actionSet_DAT_as_active_model_lib_triggered();
    void on_actionSet_MAPMODELLUT_as_active_triggered();
    void on_actionSave_DAT_triggered();
    void on_actionReplace_triggered();
    void on_actionReplace_Keeping_Aspect_triggered();
    void on_actionReplace_Keeping_Resolution_triggered();
    void on_actionReplace_Convert_to_pow2_triggered();
    void on_actionExport_As_PNG_triggered();
    void on_actionReplace_All_Converting_To_Power_Of_2_triggered();
    void on_actionReplace_All_Keeping_Original_Resolution_triggered();

private:
    Ui::Inspector *ui;

    void loadNewFile();
    bool parseFile(std::string filename);
    void renderDat();
    void renderDatChildren(DDGDat *d, QTreeWidgetItem *parent);

    QString dialogGetFile();

    void saveDDGImageToPNG(DDGImage img);
    void ImageFileToDDGTxm(QString filename, DDGTxm *txm, int option);

    std::vector<std::shared_ptr<DDGContent>> dats;
    std::vector<std::string> datNames;

    DDGContent* selected;
    QString selectedName;
};
#endif // INSPECTOR_H
