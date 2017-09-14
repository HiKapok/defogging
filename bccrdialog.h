#ifndef BCCRDIALOG_H
#define BCCRDIALOG_H

#include <QDialog>
#include <QMainWindow>

namespace Ui {
class BCCRDialog;
}

class BCCRDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BCCRDialog(QWidget *parent = 0);
    ~BCCRDialog();
	void setupSLOT(QMainWindow*);
//private:
    Ui::BCCRDialog *ui;
};

#endif // BCCRDIALOG_H
