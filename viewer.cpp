#include "viewer.h"
#include "ui_viewer.h"
#include "videowidget.h"

Viewer::Viewer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Viewer)
{
    ui->setupUi(this);

    QDir dir("C:/Users/user/Desktop");
    dir.setNameFilters(QStringList()<<"*.mp4");
    foreach(QFileInfo var,dir.entryInfoList()){
        ui->listWidget->addItem(var.fileName());
        connect(ui->listWidget, SIGNAL(on_listWidget_itemDoubleClicked()),this,SLOT(on_listWidget_itemDoubleClicked()));
    }
}

Viewer::~Viewer()
{
    delete ui;
}
