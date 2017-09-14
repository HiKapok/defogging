/********************************************************************************
** Form generated from reading UI file 'bccrdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BCCRDIALOG_H
#define UI_BCCRDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BCCRDialog
{
public:
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *lineEdit_2;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QLineEdit *lineEdit_3;

    void setupUi(QDialog *BCCRDialog)
    {
        if (BCCRDialog->objectName().isEmpty())
            BCCRDialog->setObjectName(QStringLiteral("BCCRDialog"));
		BCCRDialog->setWindowModality(Qt::WindowModal);
        BCCRDialog->resize(293, 135);
        widget = new QWidget(BCCRDialog);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(20, 10, 251, 111));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        lineEdit = new QLineEdit(widget);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        horizontalLayout->addWidget(lineEdit);


        verticalLayout->addLayout(horizontalLayout);

        verticalSpacer = new QSpacerItem(10, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(widget);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        lineEdit_2 = new QLineEdit(widget);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));

        horizontalLayout_2->addWidget(lineEdit_2);


        verticalLayout->addLayout(horizontalLayout_2);

        verticalSpacer_2 = new QSpacerItem(10, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_3 = new QLabel(widget);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_3->addWidget(label_3);

        lineEdit_3 = new QLineEdit(widget);
        lineEdit_3->setObjectName(QStringLiteral("lineEdit_3"));

        horizontalLayout_3->addWidget(lineEdit_3);


        verticalLayout->addLayout(horizontalLayout_3);


        retranslateUi(BCCRDialog);

        QMetaObject::connectSlotsByName(BCCRDialog);
    } // setupUi

    void retranslateUi(QDialog *BCCRDialog)
    {
        BCCRDialog->setWindowTitle(QApplication::translate("BCCRDialog", "\346\255\243\345\210\231\345\214\226\345\216\273\351\233\276\345\217\202\346\225\260\350\256\276\347\275\256", 0));
		lineEdit->setText(QApplication::translate("BCCRDialog", "15", 0));
        lineEdit_2->setText(QApplication::translate("BCCRDialog", "3", 0));
        lineEdit_3->setText(QApplication::translate("BCCRDialog", "2", 0));

        label->setText(QApplication::translate("BCCRDialog", "\345\244\247\346\260\224\345\205\211\345\274\272\344\274\260\350\256\241\347\252\227\345\244\247\345\260\217\357\274\232", 0));
        label_2->setText(QApplication::translate("BCCRDialog", "\350\276\271\347\225\214\347\272\246\346\235\237\347\252\227\345\244\247\345\260\217\357\274\232", 0));
        label_3->setText(QApplication::translate("BCCRDialog", "\351\200\217\345\260\204\347\216\207\346\255\243\345\210\231\345\214\226\345\274\272\345\272\246\357\274\232", 0));
    } // retranslateUi

};

namespace Ui {
    class BCCRDialog: public Ui_BCCRDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BCCRDIALOG_H
