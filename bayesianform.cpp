#include "bayesianform.h"
#include "ui_bayesianform.h"

BayesianForm::BayesianForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BayesianForm)
{
    ui->setupUi(this);
	setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    setFixedSize(this->width(), this->height()); // ��ֹ�ı䴰�ڴ�С
}

BayesianForm::~BayesianForm()
{
    delete ui;
}
