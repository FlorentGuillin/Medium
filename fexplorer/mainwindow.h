#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QDirModel>
#include <QFileSystemModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void displayDir(QString dir_path);
    QDir *getIndexPath(const QModelIndex &index);

private slots:
    void on_pushButton_clicked();

    void on_fsTreeView_doubleClicked(const QModelIndex &index);


private:
    Ui::MainWindow *ui;
    //QDirModel *fs_model;// le modele associe au systeme de fichier
    QFileSystemModel *fs_model;
    QDir * curr_dir;//le repertoire courant
};

#endif // MAINWINDOW_H
