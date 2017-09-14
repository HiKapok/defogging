#include "bayesiandialog.h"
#include "ui_bayesiandialog.h"

BayesianDialog::BayesianDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BayesianDialog)
{
    ui->setupUi(this);
    setFixedSize(this->width(), this->height()); 
	this->move(QPoint(this->parentWidget()->x() - (this->width() - this->parentWidget()->width())/2, this->parentWidget()->y() - (this->height() - this->parentWidget()->height())/2 - 20));
}

BayesianDialog::~BayesianDialog()
{
    delete ui;
}
