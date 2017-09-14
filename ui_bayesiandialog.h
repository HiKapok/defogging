/********************************************************************************
** Form generated from reading UI file 'bayesiandialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BAYESIANDIALOG_H
#define UI_BAYESIANDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BayesianDialog
{
public:
    QWidget *layoutWidget;
    QFormLayout *formLayout;
    QHBoxLayout *horizontalLayout_10;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label;
    QLineEdit *leOutIter;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_2;
    QLineEdit *leInIter;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_9;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_4;
    QLineEdit *leWeigthC;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QLineEdit *leWeigthD;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout_8;
    QHBoxLayout *horizontalLayout;
    QLabel *label_5;
    QLineEdit *leAirLight;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *btSelAirLight;
    QHBoxLayout *horizontalLayout_7;
    QSpacerItem *verticalSpacer_4;
    QHBoxLayout *horizontalLayout_6;
    QComboBox *priorType;
    QSpacerItem *horizontalSpacer_5;
    QCheckBox *cbSaveInitial;
    QSpacerItem *horizontalSpacer_6;
    QCheckBox *cbMultiScale;

    void setupUi(QDialog *BayesianDialog)
    {
        if (BayesianDialog->objectName().isEmpty())
            BayesianDialog->setObjectName(QStringLiteral("BayesianDialog"));
        BayesianDialog->setWindowModality(Qt::WindowModal);
        BayesianDialog->resize(500, 180);
        layoutWidget = new QWidget(BayesianDialog);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(20, 20, 460, 140));
        formLayout = new QFormLayout(layoutWidget);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label = new QLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_4->addWidget(label);

        leOutIter = new QLineEdit(layoutWidget);
        leOutIter->setObjectName(QStringLiteral("leOutIter"));

        horizontalLayout_4->addWidget(leOutIter);


        horizontalLayout_10->addLayout(horizontalLayout_4);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_5->addWidget(label_2);

        leInIter = new QLineEdit(layoutWidget);
        leInIter->setObjectName(QStringLiteral("leInIter"));

        horizontalLayout_5->addWidget(leInIter);


        horizontalLayout_10->addLayout(horizontalLayout_5);


        formLayout->setLayout(0, QFormLayout::LabelRole, horizontalLayout_10);

        verticalSpacer = new QSpacerItem(20, 13, QSizePolicy::Minimum, QSizePolicy::Expanding);

        formLayout->setItem(1, QFormLayout::LabelRole, verticalSpacer);

       /* horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_2->addWidget(label_4);

        leWeigthC = new QLineEdit(layoutWidget);
        leWeigthC->setObjectName(QStringLiteral("leWeigthC"));

        horizontalLayout_2->addWidget(leWeigthC);


        horizontalLayout_9->addLayout(horizontalLayout_2);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_3->addWidget(label_3);

        leWeigthD = new QLineEdit(layoutWidget);
        leWeigthD->setObjectName(QStringLiteral("leWeigthD"));

        horizontalLayout_3->addWidget(leWeigthD);


        horizontalLayout_9->addLayout(horizontalLayout_3);*/


        //formLayout->setLayout(2, QFormLayout::LabelRole, horizontalLayout_9);

        //verticalSpacer_2 = new QSpacerItem(20, 18, QSizePolicy::Minimum, QSizePolicy::Expanding);

        //formLayout->setItem(3, QFormLayout::LabelRole, verticalSpacer_2);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout->addWidget(label_5);

        leAirLight = new QLineEdit(layoutWidget);
        leAirLight->setObjectName(QStringLiteral("leAirLight"));

        horizontalLayout->addWidget(leAirLight);


        horizontalLayout_8->addLayout(horizontalLayout);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_3);

        btSelAirLight = new QPushButton(layoutWidget);
        btSelAirLight->setObjectName(QStringLiteral("btSelAirLight"));

        horizontalLayout_8->addWidget(btSelAirLight);


        formLayout->setLayout(4, QFormLayout::LabelRole, horizontalLayout_8);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));

        formLayout->setLayout(5, QFormLayout::LabelRole, horizontalLayout_7);

        verticalSpacer_4 = new QSpacerItem(20, 18, QSizePolicy::Minimum, QSizePolicy::Expanding);

        formLayout->setItem(6, QFormLayout::LabelRole, verticalSpacer_4);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        priorType = new QComboBox(layoutWidget);
        priorType->setObjectName(QStringLiteral("priorType"));

        horizontalLayout_6->addWidget(priorType);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_5);

        cbSaveInitial = new QCheckBox(layoutWidget);
        cbSaveInitial->setObjectName(QStringLiteral("cbSaveInitial"));
		cbSaveInitial->setChecked(true);

        horizontalLayout_6->addWidget(cbSaveInitial);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_6);

        cbMultiScale = new QCheckBox(layoutWidget);
        cbMultiScale->setObjectName(QStringLiteral("cbMultiScale"));
		cbMultiScale->setChecked(true);

        horizontalLayout_6->addWidget(cbMultiScale);


        formLayout->setLayout(7, QFormLayout::LabelRole, horizontalLayout_6);


        retranslateUi(BayesianDialog);

        QMetaObject::connectSlotsByName(BayesianDialog);
    } // setupUi

    void retranslateUi(QDialog *BayesianDialog)
    {
        BayesianDialog->setWindowTitle(QApplication::translate("BayesianDialog", "\350\264\235\345\217\266\346\226\257\345\216\273\351\233\276\347\256\227\346\263\225\345\217\202\346\225\260\350\256\276\347\275\256", 0));
        label->setText(QApplication::translate("BayesianDialog", "\345\244\226\345\261\202\345\276\252\347\216\257\346\254\241\346\225\260\357\274\232", 0));
        leOutIter->setText(QApplication::translate("BayesianDialog", "3", 0));
        label_2->setText(QApplication::translate("BayesianDialog", "\345\206\205\345\261\202\345\276\252\347\216\257\346\254\241\346\225\260\357\274\232", 0));
        leInIter->setText(QApplication::translate("BayesianDialog", "20", 0));
        //label_4->setText(QApplication::translate("BayesianDialog", "\345\217\215\345\260\204\345\233\276\346\235\203\351\207\215\357\274\232", 0));
        //leWeigthC->setText(QApplication::translate("BayesianDialog", "2e-6", 0));
        //label_3->setText(QApplication::translate("BayesianDialog", "\346\267\261\345\272\246\345\233\276\346\235\203\351\207\215\357\274\232", 0));
        //leWeigthD->setText(QApplication::translate("BayesianDialog", "1", 0));
        label_5->setText(QApplication::translate("BayesianDialog", "\345\244\247\346\260\224\345\205\211\345\274\272\357\274\210R,G,B\357\274\211\357\274\232", 0));
        leAirLight->setText(QApplication::translate("BayesianDialog", "0.5,0.5,0.5", 0));
        btSelAirLight->setText(QApplication::translate("BayesianDialog", "\344\272\244\344\272\222\345\274\217\351\200\211\346\213\251\345\244\247\346\260\224\345\205\211\345\274\272", 0));
        priorType->clear();
        priorType->insertItems(0, QStringList()
         << QApplication::translate("BayesianDialog", "laplace", 0)
         << QApplication::translate("BayesianDialog", "gaussian", 0)
        );
        cbSaveInitial->setText(QApplication::translate("BayesianDialog", "\344\277\235\345\255\230\346\231\257\346\267\261\347\232\204\345\210\235\345\247\213\344\274\260\350\256\241\345\233\276", 0));
        cbMultiScale->setText(QApplication::translate("BayesianDialog", "\345\244\232\345\260\272\345\272\246\344\274\260\350\256\241\346\231\257\346\267\261", 0));
    } // retranslateUi

};

namespace Ui {
    class BayesianDialog: public Ui_BayesianDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BAYESIANDIALOG_H
