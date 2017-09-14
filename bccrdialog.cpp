#include "bccrdialog.h"
#include "ui_bccrdialog.h"
#include "mainwindow.h"



BCCRDialog::BCCRDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BCCRDialog)
{
    ui->setupUi(this);
	setFixedSize(this->width(), this->height()); 
	this->move(QPoint(this->parentWidget()->x() - (this->width() - this->parentWidget()->width())/2, this->parentWidget()->y() - (this->height() - this->parentWidget()->height())/2 - 20));
}

BCCRDialog::~BCCRDialog()
{
    delete ui;
}

void BCCRDialog::setupSLOT(QMainWindow* dialog){
	connect(ui->lineEdit, SIGNAL(textChanged(QString)), dynamic_cast<MainWindow*>(dialog), SLOT(setAirWndSize(const QString)));
	connect(ui->lineEdit_2, SIGNAL(textChanged(QString)), dynamic_cast<MainWindow*>(dialog), SLOT(setBoundWndSize(const QString)));
	connect(ui->lineEdit_3, SIGNAL(textChanged(QString)), dynamic_cast<MainWindow*>(dialog), SLOT(setRegLamda(const QString)));
}
