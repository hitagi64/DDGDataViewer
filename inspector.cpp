#include "inspector.h"
#include "./ui_inspector.h"
#include <QFileDialog>
#include "ContentTreeItem.h"
#include <QMessageBox>
#include "DDG/DDGTxm.h"

Inspector::Inspector(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Inspector)
{
    ui->setupUi(this);


}

Inspector::~Inspector()
{
    delete ui;
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

    parseFile(fileNames[0].toStdString());
    renderDat();
}

void Inspector::parseFile(std::string filename)
{
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
    }  catch (std::string e) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","An error occured while loading file: " + QString::fromStdString(e));
    }
}

void Inspector::renderDat()
{
    ui->ItemView->clear();
    for (int i = 0; i < dats.size(); i++)
    {
        QTreeWidgetItem *itm = new ContentTreeItem(dats[i].get());
        itm->setText(0, QString::fromStdString(dats[i]->getType()));
        itm->setIcon(0, style()->standardIcon(QStyle::SP_FileIcon));
        ui->ItemView->addTopLevelItem(itm);

        DDGDat *c = dynamic_cast<DDGDat*>(dats[i].get());
        if (c != nullptr)
        {
            renderDatChildren(c, itm);
        }
    }
    //QTreeWidgetItem *itm = new QTreeWidgetItem(ui->ItemView);
    //itm->setData(0, 0, QVariant::fromValue(static_cast<void*>(&ddgDat)));
    //itm->setText(0, "root");
    //ui->ItemView->addTopLevelItem(itm);
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
}

