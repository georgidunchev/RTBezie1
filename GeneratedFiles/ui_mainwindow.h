/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Sun 16. Jun 18:39:09 2013
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QToolBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "cview.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    CView *Image;
    QToolBox *toolBox;
    QWidget *page;
    QPushButton *openMeshButton;
    QPushButton *StartRender;
    QGroupBox *groupBox;
    QDoubleSpinBox *PosX;
    QDoubleSpinBox *PosY;
    QDoubleSpinBox *PosZ;
    QGroupBox *groupBox_2;
    QDoubleSpinBox *DirX;
    QDoubleSpinBox *DirY;
    QDoubleSpinBox *DirZ;
    QGroupBox *groupBox_3;
    QDoubleSpinBox *UpX;
    QDoubleSpinBox *UpY;
    QDoubleSpinBox *UpZ;
    QCheckBox *RenderBezierCheckBox;
    QCheckBox *NormalSmoothingCheckBox;
    QPushButton *AutoRender;
    QWidget *page_2;
    QMenuBar *menuBar;
    QMenu *menuFiles;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(800, 600);
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(800, 600));
        MainWindow->setMouseTracking(false);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setMaximumSize(QSize(800, 547));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        Image = new CView(centralWidget);
        Image->setObjectName(QString::fromUtf8("Image"));

        horizontalLayout->addWidget(Image);

        toolBox = new QToolBox(centralWidget);
        toolBox->setObjectName(QString::fromUtf8("toolBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(toolBox->sizePolicy().hasHeightForWidth());
        toolBox->setSizePolicy(sizePolicy1);
        toolBox->setMinimumSize(QSize(250, 0));
        page = new QWidget();
        page->setObjectName(QString::fromUtf8("page"));
        page->setGeometry(QRect(0, 0, 250, 472));
        openMeshButton = new QPushButton(page);
        openMeshButton->setObjectName(QString::fromUtf8("openMeshButton"));
        openMeshButton->setGeometry(QRect(170, 0, 75, 23));
        openMeshButton->setCheckable(false);
        StartRender = new QPushButton(page);
        StartRender->setObjectName(QString::fromUtf8("StartRender"));
        StartRender->setEnabled(true);
        StartRender->setGeometry(QRect(170, 210, 75, 23));
        groupBox = new QGroupBox(page);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(20, 20, 221, 51));
        PosX = new QDoubleSpinBox(groupBox);
        PosX->setObjectName(QString::fromUtf8("PosX"));
        PosX->setGeometry(QRect(10, 20, 62, 22));
        PosX->setMinimum(-100);
        PosX->setSingleStep(0.01);
        PosY = new QDoubleSpinBox(groupBox);
        PosY->setObjectName(QString::fromUtf8("PosY"));
        PosY->setGeometry(QRect(80, 20, 62, 22));
        PosY->setMinimum(-100);
        PosY->setSingleStep(0.01);
        PosY->setValue(0.2);
        PosZ = new QDoubleSpinBox(groupBox);
        PosZ->setObjectName(QString::fromUtf8("PosZ"));
        PosZ->setGeometry(QRect(150, 20, 62, 22));
        PosZ->setMinimum(-100);
        PosZ->setMaximum(98.99);
        PosZ->setSingleStep(0.01);
        PosZ->setValue(-0.4);
        groupBox_2 = new QGroupBox(page);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(20, 70, 221, 51));
        DirX = new QDoubleSpinBox(groupBox_2);
        DirX->setObjectName(QString::fromUtf8("DirX"));
        DirX->setGeometry(QRect(10, 20, 62, 22));
        DirX->setMinimum(-100);
        DirX->setSingleStep(0.01);
        DirY = new QDoubleSpinBox(groupBox_2);
        DirY->setObjectName(QString::fromUtf8("DirY"));
        DirY->setGeometry(QRect(80, 20, 62, 22));
        DirY->setMinimum(-100);
        DirY->setSingleStep(0.01);
        DirZ = new QDoubleSpinBox(groupBox_2);
        DirZ->setObjectName(QString::fromUtf8("DirZ"));
        DirZ->setGeometry(QRect(150, 20, 62, 22));
        DirZ->setMinimum(-100);
        DirZ->setSingleStep(0.01);
        DirZ->setValue(1);
        groupBox_3 = new QGroupBox(page);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(20, 120, 221, 51));
        UpX = new QDoubleSpinBox(groupBox_3);
        UpX->setObjectName(QString::fromUtf8("UpX"));
        UpX->setGeometry(QRect(10, 20, 62, 22));
        UpX->setMinimum(-100);
        UpX->setSingleStep(0.01);
        UpY = new QDoubleSpinBox(groupBox_3);
        UpY->setObjectName(QString::fromUtf8("UpY"));
        UpY->setGeometry(QRect(80, 20, 62, 22));
        UpY->setMinimum(-100);
        UpY->setSingleStep(0.01);
        UpY->setValue(-1);
        UpZ = new QDoubleSpinBox(groupBox_3);
        UpZ->setObjectName(QString::fromUtf8("UpZ"));
        UpZ->setGeometry(QRect(150, 20, 62, 22));
        UpZ->setMinimum(-100);
        UpZ->setSingleStep(0.01);
        UpZ->setValue(0);
        RenderBezierCheckBox = new QCheckBox(page);
        RenderBezierCheckBox->setObjectName(QString::fromUtf8("RenderBezierCheckBox"));
        RenderBezierCheckBox->setGeometry(QRect(20, 180, 71, 18));
        NormalSmoothingCheckBox = new QCheckBox(page);
        NormalSmoothingCheckBox->setObjectName(QString::fromUtf8("NormalSmoothingCheckBox"));
        NormalSmoothingCheckBox->setGeometry(QRect(20, 200, 121, 18));
        AutoRender = new QPushButton(page);
        AutoRender->setObjectName(QString::fromUtf8("AutoRender"));
        AutoRender->setEnabled(true);
        AutoRender->setGeometry(QRect(170, 180, 75, 23));
        AutoRender->setCheckable(true);
        toolBox->addItem(page, QString::fromUtf8("Page 1"));
        page_2 = new QWidget();
        page_2->setObjectName(QString::fromUtf8("page_2"));
        page_2->setGeometry(QRect(0, 0, 250, 472));
        toolBox->addItem(page_2, QString::fromUtf8("Page 2"));

        horizontalLayout->addWidget(toolBox);


        verticalLayout->addLayout(horizontalLayout);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 800, 20));
        menuFiles = new QMenu(menuBar);
        menuFiles->setObjectName(QString::fromUtf8("menuFiles"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFiles->menuAction());
        mainToolBar->addSeparator();

        retranslateUi(MainWindow);

        toolBox->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        Image->setText(QString());
        openMeshButton->setText(QApplication::translate("MainWindow", "Browse", 0, QApplication::UnicodeUTF8));
        StartRender->setText(QApplication::translate("MainWindow", "Render", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MainWindow", "Position", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Direction", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("MainWindow", "Up", 0, QApplication::UnicodeUTF8));
        RenderBezierCheckBox->setText(QApplication::translate("MainWindow", "Bezier", 0, QApplication::UnicodeUTF8));
        NormalSmoothingCheckBox->setText(QApplication::translate("MainWindow", "Normal Smoothing", 0, QApplication::UnicodeUTF8));
        AutoRender->setText(QApplication::translate("MainWindow", "Realtime", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page), QApplication::translate("MainWindow", "Page 1", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(page_2), QApplication::translate("MainWindow", "Page 2", 0, QApplication::UnicodeUTF8));
        menuFiles->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
