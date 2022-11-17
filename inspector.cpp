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
#include <math.h>
#include "ContentPreviewer.h"
#include "DDG/DDGPdb.h"

Inspector::Inspector(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Inspector)
{
    ui->setupUi(this);
    setAcceptDrops(true);

    ui->ItemView->setColumnWidth(0, 200);
    ui->actionKeep_Loaded_Data->setChecked(true);

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
    std::shared_ptr<DDGContent> dat;
    try {
        DDGMemoryBuffer buffer(filename);
        DDGLoadingConfig config;
        config.useFixedPoint = ui->actionLoad_Fixed_Point->isChecked();
        config.keepLoadedData = ui->actionKeep_Loaded_Data->isChecked();
        bool match = false;
        dat = DDGDat::findAndCreateFromBuffer(config, buffer, match);
        if (!match)
            dat = std::make_shared<DDGDat>(config);
        dat->loadFromMemoryBuffer(buffer);
        dats.push_back(dat);
        success = true;
    }  catch (std::runtime_error e) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","An error occured while loading a file: " + QString::fromStdString(e.what()));
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
    selectedName = cItem->text(1);
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
        DDGLoadingConfig config;
        config.useFixedPoint = ui->actionLoad_Fixed_Point->isChecked();
        std::shared_ptr<DDGContent> c = std::make_shared<DDGDat>(config);
        c->loadFromMemoryBuffer(buffer);
        dats.push_back(c);
        datNames.push_back(QFileInfo(fileNames[0]).fileName().toStdString());
    }  catch (std::runtime_error e) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","An error occured while loading a file: " + QString::fromStdString(e.what()));
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
        DDGLoadingConfig config;
        config.useFixedPoint = ui->actionLoad_Fixed_Point->isChecked();
        std::shared_ptr<DDGContent> c = std::make_shared<DDGTxm>(config);
        c->loadFromMemoryBuffer(buffer);
        dats.push_back(c);
        datNames.push_back(QFileInfo(fileNames[0]).fileName().toStdString());
    }  catch (std::runtime_error e) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","An error occured while loading a file: " + QString::fromStdString(e.what()));
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
        DDGLoadingConfig config;
        config.useFixedPoint = ui->actionLoad_Fixed_Point->isChecked();
        std::shared_ptr<DDGContent> c = std::make_shared<DDGPdb>(config);
        c->loadFromMemoryBuffer(buffer);
        dats.push_back(c);
        datNames.push_back(QFileInfo(fileNames[0]).fileName().toStdString());
    }  catch (std::runtime_error e) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","An error occured while loading a file: " + QString::fromStdString(e.what()));
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


void Inspector::on_actionSave_DAT_triggered()
{
    DDGDat *cD = dynamic_cast<DDGDat*>(selected);
    if (cD != nullptr)
    {
        QString origFileName = selectedName;
        if (origFileName == "")
            origFileName = "out.dat";
        std::string saveFileName =
                QFileDialog::getSaveFileName(
                    this,
                    "Save Dat file",
                    origFileName,
                    "*.dat",
                    0,
                    QFileDialog::DontUseNativeDialog
                ).toStdString();
        try {
            cD->saveToFile(saveFileName);
        }  catch (std::runtime_error e) {
            QMessageBox messageBox;
            messageBox.critical(
                        0,
                        "Error",
                        QString::fromStdString(
                            std::string("An error occured while saving a file: ")
                            + e.what()
                            )
                        );
        }
    }
    else
    {
        QMessageBox messageBox;
        messageBox.critical(
                    0,
                    "Error",
                    "No item selected or the selected item is not a Dat."
                    );
    }
}

void Inspector::saveDDGImageToPNG(DDGImage img)
{
    QString origFileName = selectedName;
    if (origFileName == "")
        origFileName = "out";
    origFileName += ".png";
    QString saveFileName =
            QFileDialog::getSaveFileName(
                this,
                "Save Image file",
                origFileName,
                "*.png",
                0,
                QFileDialog::DontUseNativeDialog
            );
    QImage outImg(img.width, img.height, QImage::Format_RGBA8888);
    for (int y = 0; y < img.height; y++)
    {
        for (int x = 0; x < img.width; x++)
        {
            QRgb c = qRgba(
                        img.data[(y*img.width*4) + (x*4) + 0],
                        img.data[(y*img.width*4) + (x*4) + 1],
                        img.data[(y*img.width*4) + (x*4) + 2],
                        255-(img.data[(y*img.width*4) + (x*4) + 3]*2));
            outImg.setPixel(x, y, c);
        }
    }
    outImg.save(saveFileName);
}

QString Inspector::dialogGetFile()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setOption(dialog.DontUseNativeDialog, true);
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    if (fileNames.size() == 0)
        return "";

    return fileNames[0];
}

void Inspector::ImageFileToDDGTxm(QString filename, DDGTxm *txm, int option)
{
    try {


        QImage img;
        if (!img.load(filename))
            throw std::runtime_error("Failed to open image.");

        if (option == 1)
            img = img.scaled(txm->getWidth(), txm->getHeight(), Qt::KeepAspectRatioByExpanding);
        if (option == 2)
            img = img.scaled(txm->getWidth(), txm->getHeight());
        if (option == 3)
        {
            float px = std::log2((double)img.width());
            int pxInt = px;
            unsigned int newResx = std::pow(2, pxInt);

            float py = std::log2((double)img.height());
            int pyInt = py;
            unsigned int newResy = std::pow(2, pyInt);
            if (newResx < 1 || newResy < 1)
                throw std::runtime_error("Image size is 0.");
            img = img.scaled(newResx, newResy);
        }

        DDGImage ddgImage;
        ddgImage.width = img.width();
        ddgImage.height = img.height();
        for (int y = 0; y < img.height(); y++)
        {
            for (int x = 0; x < img.width(); x++)
            {
                QRgb c = img.pixel(x, y);
                ddgImage.data.push_back(qRed(c));
                ddgImage.data.push_back(qGreen(c));
                ddgImage.data.push_back(qBlue(c));
                ddgImage.data.push_back(qAlpha(c));
            }
        }
        txm->loadFromImage(ddgImage);
    }
    catch (std::runtime_error e) {
                QMessageBox messageBox;
                messageBox.critical(
                        0,
                        "Error",
                        QString::fromStdString(
                            std::string("An error occured while replacing an image: ")
                            + e.what()
                            )
                        );
            }
}

void Inspector::on_actionReplace_triggered()
{
    DDGTxm *cT = dynamic_cast<DDGTxm*>(selected);
    if (cT != nullptr)
        ImageFileToDDGTxm(dialogGetFile(), cT, 0);
}


void Inspector::on_actionReplace_Keeping_Aspect_triggered()
{
    DDGTxm *cT = dynamic_cast<DDGTxm*>(selected);
    if (cT != nullptr)
        ImageFileToDDGTxm(dialogGetFile(), cT, 1);
}


void Inspector::on_actionReplace_Keeping_Resolution_triggered()
{
    DDGTxm *cT = dynamic_cast<DDGTxm*>(selected);
    if (cT != nullptr)
        ImageFileToDDGTxm(dialogGetFile(), cT, 2);
}


void Inspector::on_actionReplace_Convert_to_pow2_triggered()
{
    DDGTxm *cT = dynamic_cast<DDGTxm*>(selected);
    if (cT != nullptr)
        ImageFileToDDGTxm(dialogGetFile(), cT, 3);
}


void Inspector::on_actionExport_As_PNG_triggered()
{
    DDGTxm *cT = dynamic_cast<DDGTxm*>(selected);
    if (cT != nullptr)
        saveDDGImageToPNG(cT->convertToImage());
}


void Inspector::on_actionReplace_All_Converting_To_Power_Of_2_triggered()
{
    DDGDat *cD = dynamic_cast<DDGDat*>(selected);
    if (cD != nullptr)
    {
        QString filename = dialogGetFile();
        auto datObjects = cD->getObjects();
        for (int i = 0; i < datObjects.size(); i++)
        {
            DDGTxm *cT = dynamic_cast<DDGTxm*>(datObjects[i].get());
            if (cT != nullptr)
                ImageFileToDDGTxm(filename, cT, 3);
        }
    }
}


void Inspector::on_actionReplace_All_Keeping_Original_Resolution_triggered()
{
    DDGDat *cD = dynamic_cast<DDGDat*>(selected);
    if (cD != nullptr)
    {
        QString filename = dialogGetFile();
        auto datObjects = cD->getObjects();
        for (int i = 0; i < datObjects.size(); i++)
        {
            DDGTxm *cT = dynamic_cast<DDGTxm*>(datObjects[i].get());
            if (cT != nullptr)
                ImageFileToDDGTxm(filename, cT, 2);
        }
    }
}

