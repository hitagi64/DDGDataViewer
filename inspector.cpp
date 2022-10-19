#include "inspector.h"
#include "./ui_inspector.h"
#include <QFileDialog>
#include "ContentTreeItem.h"
#include <QMessageBox>
#include <QGraphicsPixmapItem>
#include "DDG/DDGTxm.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QGraphicsView>
#include <algorithm>
#include "ContentPreviewer.h"
#include "DDG/DDGPdb.h"

Inspector::Inspector(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Inspector)
{
    ui->setupUi(this);
    setAcceptDrops(true);

    ui->ItemView->setColumnWidth(0, 200);

    selected = 0;
}

Inspector::~Inspector()
{
    delete ui;
}

void Inspector::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void Inspector::dropEvent(QDropEvent *e)
{
    foreach (const QUrl &url, e->mimeData()->urls()) {
        QString fileName = url.toLocalFile();
        if (parseFile(fileName.toStdString()))
            datNames.push_back(QFileInfo(fileName).fileName().toStdString());
        renderDat();
    }
}

void Inspector::on_actionExit_triggered()
{
    QApplication::exit();
}

void Inspector::on_actionOpen_triggered()
{
    loadNewFile();
}

void Inspector::loadNewFile()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setOption(dialog.DontUseNativeDialog, true);
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    if (fileNames.size() == 0)
        return;

    this->setWindowTitle(fileNames[0]);

    if (parseFile(fileNames[0].toStdString()))
        datNames.push_back(QFileInfo(fileNames[0]).fileName().toStdString());
    renderDat();
}

bool Inspector::parseFile(std::string filename)
{
    bool success = false;
    std::shared_ptr<DDGContent> dat = std::make_shared<DDGDat>();
    try {
        DDGMemoryBuffer buffer(filename);
        bool match = false;
        dat = DDGDat::findAndLoadContentFromBuffer(buffer, match);
        if (!match)
        {
            dat = std::make_shared<DDGDat>();
            dat->loadFromMemoryBuffer(buffer);
        }
        dats.push_back(dat);
        success = true;
    }  catch (std::string e) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","An error occured while loading file: " + QString::fromStdString(e));
    }
    return success;
}

void Inspector::renderDat()
{
    ui->ItemView->clear();
    for (int i = 0; i < dats.size(); i++)
    {
        QTreeWidgetItem *itm = new ContentTreeItem(dats[i].get());
        itm->setText(0, QString::fromStdString(dats[i]->getType()));
        itm->setText(1, QString::fromStdString(datNames[i]));
        itm->setIcon(0, style()->standardIcon(QStyle::SP_FileIcon));
        ui->ItemView->addTopLevelItem(itm);

        DDGDat *c = dynamic_cast<DDGDat*>(dats[i].get());
        if (c != nullptr)
        {
            renderDatChildren(c, itm);
        }
    }
}

void Inspector::renderDatChildren(DDGDat *dat, QTreeWidgetItem *parent)
{
    std::vector<std::shared_ptr<DDGContent>> childs = dat->getObjects();
    for (int i = 0; i < childs.size(); i++)
    {
        QTreeWidgetItem *itm = new ContentTreeItem(childs[i].get());
        itm->setText(0, QString::fromStdString(childs[i]->getType()));
        itm->setText(1, QString::fromStdString(std::to_string(i)));

        DDGDat *cD = dynamic_cast<DDGDat*>(childs[i].get());
        if (cD != nullptr)
        {
            itm->setIcon(0, style()->standardIcon(QStyle::SP_FileIcon));
            renderDatChildren(cD, itm);
        }

        DDGTxm *cI = dynamic_cast<DDGTxm*>(childs[i].get());
        if (cI != nullptr)
        {
            itm->setIcon(0, style()->standardIcon(QStyle::SP_DesktopIcon));
        }

        parent->addChild(itm);
    }
}

void Inspector::on_ItemView_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    ContentTreeItem *cItem = dynamic_cast<ContentTreeItem*>(current);
    if (cItem == nullptr)
        return;

    std::string txt = cItem->content->getInfoAsString();
    ui->information->setText(QString::fromStdString(txt));

    ui->previewer->displayContent(cItem->content);

    selected = cItem->content;
}

void Inspector::on_actionDat_triggered()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    if (fileNames.size() == 0)
        return;

    this->setWindowTitle(fileNames[0]);

    try {
        DDGMemoryBuffer buffer(fileNames[0].toStdString());
        std::shared_ptr<DDGContent> c = std::make_shared<DDGDat>();
        c->loadFromMemoryBuffer(buffer);
        dats.push_back(c);
        datNames.push_back(QFileInfo(fileNames[0]).fileName().toStdString());
    }  catch (std::string e) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","An error occured while loading file: " + QString::fromStdString(e));
    }

    renderDat();
}

void Inspector::on_actionTxm_triggered()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    if (fileNames.size() == 0)
        return;

    this->setWindowTitle(fileNames[0]);

    try {
        DDGMemoryBuffer buffer(fileNames[0].toStdString());
        std::shared_ptr<DDGContent> c = std::make_shared<DDGTxm>();
        c->loadFromMemoryBuffer(buffer);
        dats.push_back(c);
        datNames.push_back(QFileInfo(fileNames[0]).fileName().toStdString());
    }  catch (std::string e) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","An error occured while loading file: " + QString::fromStdString(e));
    }

    renderDat();
}

void Inspector::on_actionPdb_triggered()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    if (fileNames.size() == 0)
        return;

    this->setWindowTitle(fileNames[0]);

    try {
        DDGMemoryBuffer buffer(fileNames[0].toStdString());
        std::shared_ptr<DDGContent> c = std::make_shared<DDGPdb>();
        c->loadFromMemoryBuffer(buffer);
        dats.push_back(c);
        datNames.push_back(QFileInfo(fileNames[0]).fileName().toStdString());
    }  catch (std::string e) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","An error occured while loading file: " + QString::fromStdString(e));
    }

    renderDat();
}

void Inspector::on_actionPdm_get_before_buffer_1_vals_triggered()
{
    if (selected == 0)
        return;

    DDGPdb *cM = dynamic_cast<DDGPdb*>(selected);
    if (cM != nullptr)
    {
        std::vector<DDGModelTexture> s1 = cM->getModelSegment1().textures;
        std::vector<DDGModelTexture> s2 = cM->getModelSegment2().textures;
        std::vector<DDGModelTexture> s3 = cM->getModelSegment3().textures;

        std::string s;
        s += "Segment 1: \n";
        for (DDGModelTexture v : s1)
        {
            s += std::to_string(v.textureIndex) + ", " + std::to_string(v.b) + ", " + std::to_string(v.c) + "\n";
        }

        s += "Segment 2: \n";
        for (DDGModelTexture v : s2)
        {
            s += std::to_string(v.textureIndex) + ", " + std::to_string(v.b) + ", " + std::to_string(v.c) + "\n";
        }

        s += "Segment 3: \n";
        for (DDGModelTexture v : s3)
        {
            s += std::to_string(v.textureIndex) + ", " + std::to_string(v.b) + ", " + std::to_string(v.c) + "\n";
        }

        QMessageBox messageBox;
        messageBox.information(0,
                            "Result",
                            QString::fromStdString(s));
    }
}


void Inspector::on_actionSet_DAT_as_active_texture_lib_triggered()
{
    DDGDat *cD = dynamic_cast<DDGDat*>(selected);
    if (cD != nullptr)
    {
        ui->previewer->textureLib = cD;
    }
}

void Inspector::on_actionFly_Mode_triggered()
{
    ui->previewer->flyMode = ui->actionFly_Mode->isChecked();
}

void Inspector::on_actionSet_DAT_as_active_model_lib_triggered()
{
    DDGDat *cD = dynamic_cast<DDGDat*>(selected);
    if (cD != nullptr)
    {
        ui->previewer->modelLib = cD;
    }
}

void Inspector::on_actionSet_MAPMODELLUT_as_active_triggered()
{
    DDGMapModelLUT *cT = dynamic_cast<DDGMapModelLUT*>(selected);
    if (cT != nullptr)
    {
        ui->previewer->modelLUT = cT;
    }
}

