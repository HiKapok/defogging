/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
//#include "klabel.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *btOpenImage;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *btSelSavePath;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *orgImage;
    QSpacerItem *horizontalSpacer;
    QLabel *defogImage;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btDarkChannal;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btBayesian;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *btRegDefog;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(380, 260);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 40, 360, 180));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        btOpenImage = new QPushButton(layoutWidget);
        btOpenImage->setObjectName(QStringLiteral("btOpenImage"));

        horizontalLayout->addWidget(btOpenImage);

        horizontalSpacer_3 = new QSpacerItem(100, 40, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        btSelSavePath = new QPushButton(layoutWidget);
        btSelSavePath->setObjectName(QStringLiteral("btSelSavePath"));

        horizontalLayout->addWidget(btSelSavePath);


        verticalLayout->addLayout(horizontalLayout);

        verticalSpacer_3 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer_3);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setSizeConstraint(QLayout::SetNoConstraint);
        orgImage = new QLabel(layoutWidget);
        orgImage->setObjectName(QStringLiteral("orgImage"));
		orgImage->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        horizontalLayout_3->addWidget(orgImage);

        horizontalSpacer = new QSpacerItem(20, 40, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        defogImage = new QLabel(layoutWidget);
        defogImage->setObjectName(QStringLiteral("defogImage"));
		defogImage->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        horizontalLayout_3->addWidget(defogImage);


        verticalLayout->addLayout(horizontalLayout_3);

        verticalSpacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        btDarkChannal = new QPushButton(layoutWidget);
        btDarkChannal->setObjectName(QStringLiteral("btDarkChannal"));

        horizontalLayout_2->addWidget(btDarkChannal);

        horizontalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        btBayesian = new QPushButton(layoutWidget);
        btBayesian->setObjectName(QStringLiteral("btBayesian"));

        horizontalLayout_2->addWidget(btBayesian);

        horizontalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);

        btRegDefog = new QPushButton(layoutWidget);
        btRegDefog->setObjectName(QStringLiteral("btRegDefog"));

        horizontalLayout_2->addWidget(btRegDefog);


        verticalLayout->addLayout(horizontalLayout_2);

        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "\350\256\241\347\256\227\346\234\272\350\247\206\350\247\211\350\257\276\347\250\213\350\256\276\350\256\241", 0));
        btOpenImage->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\345\233\276\345\203\217", 0));
        btSelSavePath->setText(QApplication::translate("MainWindow", "\344\277\235\345\255\230\350\267\257\345\276\204", 0));
        orgImage->setText(QApplication::translate("MainWindow", "\345\216\237\345\233\276\345\203\217", 0));
        defogImage->setText(QApplication::translate("MainWindow", "\345\216\273\351\233\276\345\233\276\345\203\217", 0));
        btDarkChannal->setText(QApplication::translate("MainWindow", "\346\232\227\351\200\232\351\201\223\345\216\273\351\233\276", 0));
        btBayesian->setText(QApplication::translate("MainWindow", "\350\264\235\345\217\266\346\226\257\345\216\273\351\233\276", 0));
        btRegDefog->setText(QApplication::translate("MainWindow", "\344\270\212\344\270\213\346\226\207\346\255\243\345\210\231\345\214\226\345\216\273\351\233\276", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
