#include "inspector.h"
#include "./ui_inspector.h"
#include <QFileDialog>
#include "ContentTreeItem.h"
#include <QMessageBox>
#include <QGraphicsPixmapItem>
#include "DDG/DDGTxm.h"
#include <QDragEnterEvent>
#include <QMimeData>

Inspector::Inspector(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Inspector)
{
    ui->setupUi(this);
    setAcceptDrops(true);

    ui->ItemView->setColumnWidth(0, 300);
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

void Inspector::on_ItemView_itemClicked(QTreeWidgetItem *item, int column)
{
    ContentTreeItem *cItem = dynamic_cast<ContentTreeItem*>(item);
    if (cItem == nullptr)
        return;

    std::string txt = cItem->content->getInfoAsString();
    ui->information->setText(QString::fromStdString(txt));

    DDGTxm *cI = dynamic_cast<DDGTxm*>(cItem->content);
    if (cI != nullptr)
    {
        DDGImage ddgImage;
        try {
            ddgImage = cI->convertToImage();
        }  catch (std::string e) {
            QMessageBox messageBox;
            messageBox.critical(0,"Error","An error occured while previewing DDGImage: " + QString::fromStdString(e));
        }

        QImage img(ddgImage.width, ddgImage.height, QImage::Format_RGBA8888);
        memcpy(img.bits(), ddgImage.data.get(), ddgImage.width * ddgImage.height * 4);

        QGraphicsScene* scene = new QGraphicsScene();
        ui->preview->setScene(scene);
        QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(img));
        scene->addItem(item);
        ui->preview->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        ui->preview->show();
    }
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

