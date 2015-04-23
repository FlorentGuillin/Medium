#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fs_model = new QDirModel;
    ui->fsTreeView->setModel(fs_model);
    curr_dir = new QDir("/");
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::displayDir()
{
    if(curr_dir->exists())
    {
        ui->fsTreeView->setRootIndex(fs_model->index(curr_dir->absolutePath()));
    }
}

void MainWindow::on_pushButton_clicked()
{
    QString path = ui->pathLineEdit->text();

    if(curr_dir != 0)
        delete curr_dir;

    curr_dir = new QDir(path);

    displayDir();

}
