#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "globals.h"
#include "worker.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    Worker *Pako;

    bool HaveCams;

protected slots:

    void AtEnd(void);
    void FeedBack(const int& n);
    void TimeOut(void);

private slots:
    void on_BGo_clicked();
    void on_image_linkActivated(const QString &link);
};

#endif // MAINWINDOW_H
