#include "viewer.h"
#include "ui_viewer.h"

Viewer::Viewer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Viewer)
{
    ui->setupUi(this);

    QDir dir("C:/Users/user/Desktop");
    dir.setNameFilters(QStringList()<<"*.mp4");
    foreach(QFileInfo var,dir.entryInfoList()){
        ui->listWidget->addItem(var.fileName());
    }

}

Viewer::~Viewer()
{
    delete ui;
}

