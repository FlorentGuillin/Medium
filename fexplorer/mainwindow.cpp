#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fs_model = new QFileSystemModel;
    fs_model->setRootPath(QDir::currentPath());
    ui->fsTreeView->setModel(fs_model);
    curr_dir = new QDir("/");
}

MainWindow::~MainWindow()
{
    delete ui;
}
/* affiche le répertoire dispo dans curr_dir et le prend comme racine */
void MainWindow::displayDir(QString dir_path)
{
    QDir *new_dir = new QDir(dir_path);
    if(new_dir->exists())
    {
        if(curr_dir != 0)
            delete curr_dir;

        curr_dir = new_dir;
    }
    ui->pathLineEdit->setText(curr_dir->absolutePath());
    ui->fsTreeView->setRootIndex(fs_model->index(curr_dir->absolutePath()));
}
/* slot pour quand on clique sur le bouton */
void MainWindow::on_pushButton_clicked()
{
    QString path = ui->pathLineEdit->text();

    displayDir(path);

}
/* slot pour quand on double-clique sur un dossier de ui->fsTreeView*/
void MainWindow::on_fsTreeView_doubleClicked(const QModelIndex &index)
{
    QString next_path = fs_model->filePath(index);

    displayDir(next_path);
}
