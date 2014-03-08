#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressDialog>

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
    void DisplayText(const QString &strOutput);

signals:
    void EnableRenderButton(bool bEnable);
	void DebugOutChanged(const QString& str);

private slots:
    void on_openMeshButton_clicked();
    void on_StartRender_clicked();
    void slotRenderFinished();
    void slotLoadingStarted(int nMaxSteps);
    void slotLoadingFinished();

    void on_RenderBezierCheckBox_toggled(bool checked);

    void on_NormalSmoothingCheckBox_toggled(bool checked);

    void on_AutoRender_clicked(bool checked);


    void on_NumberOfSubdivisions_valueChanged(int arg1);

    void on_butRefresh_clicked();

    void on_NormalSmoothingCheckBox_clicked(bool checked);

    void on_MultiSeedCheckBox_clicked(bool checked);

    void on_UseKDTreeCheckbox_clicked(bool checked);

    void on_ShowKDTreeCheckbox_clicked(bool checked);

    void on_WireframeCheckBox_clicked(bool checked);

    void on_butResetCamera_clicked();

    void on_butLookAtCentre_clicked();

private:
    void StartSingleRender();
    void StartRender(bool bHighQuality = true);
    Ui::MainWindow *ui;
    QProgressDialog progress;

    bool m_bAutoRendering;
	bool m_bStartNormalRender;
	bool m_bShouldRefreshView;
	bool m_bRendering;
};

#endif // MAINWINDOW_H
