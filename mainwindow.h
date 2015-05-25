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
#include <QTextStream>
#include "ui_mainwindow.h"
#include "bookmarklistwidgetitem.h"
#include "imagemetadata.h"
#include "audiometadata.h"
#include "filetreewidgetitem.h"
#include "pdfmetadata.h"
#include <QDesktopServices>
#include <fileindexer.h>

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
    QFileInfoList getAllFilesRecursively(QDir* dir, QRegExp search_filter);

private slots:
    void on_changeDirButton_clicked();

    void on_fsTreeView_doubleClicked(const QModelIndex &index);


    void on_parentDirButton_clicked();

    void on_filterButton_clicked();

    void on_bookmark_listWidget_clicked(const QModelIndex &index);

    void on_deleteBookmark_pushButton_clicked();

    void on_open_filewidgetButton_clicked();

    void on_file_treeWidget_doubleClicked(const QModelIndex &index);

    void on_deleteFile_pushButton_clicked();

    void on_file_treeWidget_clicked(const QModelIndex &index);

    void on_fileAddFile_pushButton_clicked();

    void on_indexer_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *fs_model;
    QDir *curr_dir;//le repertoire courant
    QSqlDatabase db; //lien vers la base SQLite
    QRegExp buildSearchRegExp(QString search_filter);
};

#endif // MAINWINDOW_H
