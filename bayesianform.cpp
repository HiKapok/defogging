#include "bayesianform.h"
#include "ui_bayesianform.h"

BayesianForm::BayesianForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BayesianForm)
{
    ui->setupUi(this);
	setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    setFixedSize(this->width(), this->height()); // 禁止改变窗口大小
}

BayesianForm::~BayesianForm()
{
    delete ui;
}
