#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressDialog>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    void paintEvent(QPaintEvent * pe);

signals:
    void EnableRenderButton(bool bEnable);

private slots:
    void on_openMeshButton_clicked();
    void on_StartRender_clicked();
    void slotRenderFinished();

private:
    Ui::MainWindow *ui;
    QProgressDialog * progress;
};

#endif // MAINWINDOW_H
