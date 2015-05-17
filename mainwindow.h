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
#include "filetreemodel.h"

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
    void displayFiles(int bookmark_id);
    QDir *getIndexPath(const QModelIndex &index);
    bool connectSQLite();
    QFileInfoList getAllFilesRecursively(QDir* dir, QString search_filter);

private slots:
    void on_changeDirButton_clicked();

    void on_fsTreeView_doubleClicked(const QModelIndex &index);


    void on_parentDirButton_clicked();

    void on_filterButton_clicked();

    void on_bookmark_listWidget_clicked(const QModelIndex &index);

    void on_deleteBookmark_pushButton_clicked();

    void on_file_treeView_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QFileSystemModel *fs_model;
    FileTreeModel *ftm;
    QDir *curr_dir;//le repertoire courant

    QSqlDatabase db; //lien vers la base SQLite
};

#endif // MAINWINDOW_H
