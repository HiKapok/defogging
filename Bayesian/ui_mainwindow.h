/********************************************************************************
** Form generated from reading UI file 'kmainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KMAINWINDOW_H
#define UI_KMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
//#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "../klabel.h"

QT_BEGIN_NAMESPACE

class Ui_KMainWindow
{
public:
    QWidget *centralWidget;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    KLabel *labelOrgImg;
    QSpacerItem *horizontalSpacer;
    KLabel *labelImage;
    QSpacerItem *verticalSpacer;
    //QComboBox *comboBox;
    QGridLayout *gridLayout;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *KMainWindow)
    {
        if (KMainWindow->objectName().isEmpty())
            KMainWindow->setObjectName(QStringLiteral("KMainWindow"));
        KMainWindow->resize(522, 385);
        centralWidget = new QWidget(KMainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(20, 10, 481, 351));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        labelOrgImg = new KLabel(layoutWidget);
        labelOrgImg->setObjectName(QStringLiteral("labelOrgImg"));

        horizontalLayout->addWidget(labelOrgImg);

        horizontalSpacer = new QSpacerItem(5, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        labelImage = new KLabel(layoutWidget);
        labelImage->setObjectName(QStringLiteral("labelImage"));

        horizontalLayout->addWidget(labelImage);


        verticalLayout->addLayout(horizontalLayout);

        verticalSpacer = new QSpacerItem(80, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        /*comboBox = new QComboBox(layoutWidget);
        comboBox->setObjectName(QStringLiteral("comboBox"));

        verticalLayout->addWidget(comboBox);*/

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));

        verticalLayout->addLayout(gridLayout);

        KMainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(KMainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        KMainWindow->setStatusBar(statusBar);

        retranslateUi(KMainWindow);

        QMetaObject::connectSlotsByName(KMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *KMainWindow)
    {
        KMainWindow->setWindowTitle(QApplication::translate("KMainWindow", "\346\274\224\347\244\272\347\225\214\351\235\242", 0));
        labelOrgImg->setText(QApplication::translate("KMainWindow", "\345\216\237\345\247\213\345\233\276\345\203\217", 0));
		labelOrgImg->adjustSize();
        labelImage->setText(QApplication::translate("KMainWindow", "\345\216\273\351\233\276\345\233\276\345\203\217", 0));
        //comboBox->clear();
        //comboBox->insertItems(0, QStringList()
        // << QApplication::translate("KMainWindow", "gaussian", 0)
        // << QApplication::translate("KMainWindow", "laplace", 0)
        //);
    } // retranslateUi

};

namespace Ui {
    class KMainWindow: public Ui_KMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KMAINWINDOW_H
