#ifndef BAYESIANFORM_H
#define BAYESIANFORM_H

#include <QWidget>

namespace Ui {
class BayesianForm;
}

class BayesianForm : public QWidget
{
    Q_OBJECT

public:
    explicit BayesianForm(QWidget *parent = 0);
    ~BayesianForm();

//private:
    Ui::BayesianForm *ui;
};

#endif // BAYESIANFORM_H
