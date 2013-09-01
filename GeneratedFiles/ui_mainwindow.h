/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Tue 27. Aug 00:35:10 2013
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
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QTextBrowser>
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
    QWidget *Controls;
    QPushButton *openMeshButton;
    QPushButton *StartRender;
    QPushButton *AutoRender;
    QTextBrowser *Output;
    QLabel *label;
    QWidget *Settings;
    QCheckBox *RenderBezierCheckBox;
    QCheckBox *NormalSmoothingCheckBox;
    QSpinBox *NumberOfThreads;
    QCheckBox *checkBox;
    QCheckBox *checkBox_2;
    QSpinBox *MaxLevelOfTree;
    QLabel *label_2;
    QSpinBox *MinNumberOfTriangleToSplit;
    QLabel *label_3;
    QSpinBox *NumberOfSplitsPerNode;
    QLabel *label_4;
    QSpinBox *NumberOfSubdivisions;
    QLabel *label_5;
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
        sizePolicy.setHeightForWidth(Image->sizePolicy().hasHeightForWidth());
        Image->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(Image);

        toolBox = new QToolBox(centralWidget);
        toolBox->setObjectName(QString::fromUtf8("toolBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(toolBox->sizePolicy().hasHeightForWidth());
        toolBox->setSizePolicy(sizePolicy1);
        toolBox->setMinimumSize(QSize(250, 0));
        toolBox->setLayoutDirection(Qt::LeftToRight);
        Controls = new QWidget();
        Controls->setObjectName(QString::fromUtf8("Controls"));
        Controls->setGeometry(QRect(0, 0, 250, 472));
        openMeshButton = new QPushButton(Controls);
        openMeshButton->setObjectName(QString::fromUtf8("openMeshButton"));
        openMeshButton->setGeometry(QRect(170, 0, 75, 23));
        openMeshButton->setCheckable(false);
        StartRender = new QPushButton(Controls);
        StartRender->setObjectName(QString::fromUtf8("StartRender"));
        StartRender->setEnabled(true);
        StartRender->setGeometry(QRect(170, 210, 75, 23));
        AutoRender = new QPushButton(Controls);
        AutoRender->setObjectName(QString::fromUtf8("AutoRender"));
        AutoRender->setEnabled(true);
        AutoRender->setGeometry(QRect(170, 180, 75, 23));
        AutoRender->setCheckable(true);
        Output = new QTextBrowser(Controls);
        Output->setObjectName(QString::fromUtf8("Output"));
        Output->setGeometry(QRect(20, 240, 221, 231));
        label = new QLabel(Controls);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 0, 141, 21));
        toolBox->addItem(Controls, QString::fromUtf8("Controls"));
        Settings = new QWidget();
        Settings->setObjectName(QString::fromUtf8("Settings"));
        Settings->setGeometry(QRect(0, 0, 250, 472));
        RenderBezierCheckBox = new QCheckBox(Settings);
        RenderBezierCheckBox->setObjectName(QString::fromUtf8("RenderBezierCheckBox"));
        RenderBezierCheckBox->setGeometry(QRect(0, 10, 211, 18));
        NormalSmoothingCheckBox = new QCheckBox(Settings);
        NormalSmoothingCheckBox->setObjectName(QString::fromUtf8("NormalSmoothingCheckBox"));
        NormalSmoothingCheckBox->setGeometry(QRect(0, 30, 131, 20));
        NormalSmoothingCheckBox->setMinimumSize(QSize(0, 0));
        NumberOfThreads = new QSpinBox(Settings);
        NumberOfThreads->setObjectName(QString::fromUtf8("NumberOfThreads"));
        NumberOfThreads->setGeometry(QRect(0, 60, 46, 22));
        checkBox = new QCheckBox(Settings);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(10, 100, 111, 18));
        checkBox_2 = new QCheckBox(Settings);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));
        checkBox_2->setGeometry(QRect(10, 130, 111, 18));
        MaxLevelOfTree = new QSpinBox(Settings);
        MaxLevelOfTree->setObjectName(QString::fromUtf8("MaxLevelOfTree"));
        MaxLevelOfTree->setGeometry(QRect(10, 170, 46, 22));
        label_2 = new QLabel(Settings);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(80, 170, 151, 16));
        MinNumberOfTriangleToSplit = new QSpinBox(Settings);
        MinNumberOfTriangleToSplit->setObjectName(QString::fromUtf8("MinNumberOfTriangleToSplit"));
        MinNumberOfTriangleToSplit->setGeometry(QRect(10, 210, 46, 22));
        label_3 = new QLabel(Settings);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(80, 210, 151, 16));
        NumberOfSplitsPerNode = new QSpinBox(Settings);
        NumberOfSplitsPerNode->setObjectName(QString::fromUtf8("NumberOfSplitsPerNode"));
        NumberOfSplitsPerNode->setGeometry(QRect(10, 240, 46, 22));
        label_4 = new QLabel(Settings);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(80, 240, 151, 16));
        NumberOfSubdivisions = new QSpinBox(Settings);
        NumberOfSubdivisions->setObjectName(QString::fromUtf8("NumberOfSubdivisions"));
        NumberOfSubdivisions->setGeometry(QRect(10, 280, 46, 22));
        label_5 = new QLabel(Settings);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(80, 280, 151, 16));
        toolBox->addItem(Settings, QString::fromUtf8("Settings"));

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

        toolBox->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        Image->setText(QString());
        openMeshButton->setText(QApplication::translate("MainWindow", "Browse", 0, QApplication::UnicodeUTF8));
        StartRender->setText(QApplication::translate("MainWindow", "Render", 0, QApplication::UnicodeUTF8));
        AutoRender->setText(QApplication::translate("MainWindow", "Realtime", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Open File", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(Controls), QApplication::translate("MainWindow", "Controls", 0, QApplication::UnicodeUTF8));
        RenderBezierCheckBox->setText(QApplication::translate("MainWindow", "Bezier", 0, QApplication::UnicodeUTF8));
        NormalSmoothingCheckBox->setText(QApplication::translate("MainWindow", "Normal Smoothing", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("MainWindow", "Use KD Tree", 0, QApplication::UnicodeUTF8));
        checkBox_2->setText(QApplication::translate("MainWindow", "Show KD Tree", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Max Level of Tree", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Min Number of Triangles to Split", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Number of Splits per Node", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "Number of Subdivisions", 0, QApplication::UnicodeUTF8));
        toolBox->setItemText(toolBox->indexOf(Settings), QApplication::translate("MainWindow", "Settings", 0, QApplication::UnicodeUTF8));
        menuFiles->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
