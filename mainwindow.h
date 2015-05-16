#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QDirModel>
#include <QFileSystemModel>
#include <QFileDialog>
#include <QtSql/QtSql>
#include <QMessageBox>
#include <QStringBuilder>

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
    void displayBookmarks();
    QDir *getIndexPath(const QModelIndex &index);
    bool connectSQLite();
    QFileInfoList getAllFilesRecursively(QDir* dir);

private slots:
    void on_changeDirButton_clicked();

    void on_fsTreeView_doubleClicked(const QModelIndex &index);


    void on_parentDirButton_clicked();

    void on_filterButton_clicked();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *fs_model;
    QDir *curr_dir;//le repertoire courant

    QSqlDatabase db; //lien vers la base SQLite
};

#endif // MAINWINDOW_H
