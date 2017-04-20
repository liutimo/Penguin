#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QMessageBox>
#include<QLabel>
#include<QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setTextEditSytel();
    void uiDesignerStyle();

private slots:
    void on_openFileSystemAction_triggered();
    void on_closeFileSystemAction_triggered();
    void on_showFileSystemAction_triggered();
    void on_concealFileSystemAction_triggered();
private:
    Ui::MainWindow *ui;
    bool isOpenFileSystem = false;
    bool isShowFileSystem = false;
};

#endif // MAINWINDOW_H
