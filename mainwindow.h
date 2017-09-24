#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "netzwerk.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_ButtonHostServer_clicked();

    void on_ButtonConnect_clicked();

    void on_ButtonBeginGame_clicked();

private:
    Ui::MainWindow *ui;
    Netzwerk *n;
    bool isConnected;
};

#endif // MAINWINDOW_H
