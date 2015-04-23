#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QTreeWidgetItem>
#include <QDirModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void displayDir();
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QDirModel *fs_model;
    QDir * curr_dir;
};

#endif // MAINWINDOW_H
