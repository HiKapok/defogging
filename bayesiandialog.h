#ifndef BAYESIANDIALOG_H
#define BAYESIANDIALOG_H

#include <QDialog>

namespace Ui {
class BayesianDialog;
}

class BayesianDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BayesianDialog(QWidget *parent = 0);
    ~BayesianDialog();

//private:
    Ui::BayesianDialog *ui;
};

#endif // BAYESIANDIALOG_H
