#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    setFixedSize(this->width(), this->height()); // 禁止改变窗口大小
	connect(ui->btBayesian, SIGNAL(released()), this, SLOT(run_bayesain()));
	connect(ui->btDarkChannal, SIGNAL(released()), this, SLOT(run_drakChannal()));
	connect(ui->btRegDefog, SIGNAL(released()), this, SLOT(run_RegDefog()));
	connect(ui->btOpenImage, SIGNAL(released()), this, SLOT(open_image()));
	connect(ui->btSelSavePath, SIGNAL(released()), this, SLOT(open_save_path()));

	QDesktopWidget *desktop = QApplication::desktop();
    desktopWidth = desktop->width();
    desktopHeight = desktop->height();

	airWndSize = 15;
	boundWndSize = 3;
	lambda = 2.;
}

void MainWindow::deleteUI(){delete ui;}

void MainWindow::clear_message()
{
    ui->statusBar->clearMessage();
}

void MainWindow::set_message(QString mes, int time_out)
{
	/*qDebug()<<this->size();
	qDebug()<<this->ui->centralWidget->size();*/
    ui->statusBar->showMessage(mes, time_out);
}

void MainWindow::setOrgImage(const QImage& image){
	QImage tempImage = image;
	if(tempImage.width() > 2 * desktopWidth / 5){
		tempImage = tempImage.scaledToWidth(2 * desktopWidth / 5);
	}
	if(tempImage.height() > 3 * desktopHeight / 5 ){
		tempImage = tempImage.scaledToHeight(3 * desktopHeight / 5);
	}
	ui->orgImage->setPixmap(QPixmap::fromImage(tempImage));
}

void MainWindow::setDefogImage(const QImage& image){
	QImage tempImage = image;
	if(tempImage.width() > 2 * desktopWidth / 5){
		tempImage = tempImage.scaledToWidth(2 * desktopWidth / 5);
	}
	if(tempImage.height() > 3 * desktopHeight / 5 ){
		tempImage = tempImage.scaledToHeight(3 * desktopHeight / 5);
	}
	ui->defogImage->setPixmap(QPixmap::fromImage(tempImage));
}

void MainWindow::resizeWnd(){
	int minWidth = this->ui->centralWidget->size().width();
	int minHeigh = this->ui->centralWidget->size().height();

	QImage tempImage = m_qImage;
	if(tempImage.width() > 2 * desktopWidth / 5){
		tempImage = tempImage.scaledToWidth(2 * desktopWidth / 5);
	}
	if(tempImage.height() > 3 * desktopHeight / 5 ){
		tempImage = tempImage.scaledToHeight(3 * desktopHeight / 5);
	}

	if(ui->orgImage->size().width() < tempImage.width()){
		minWidth += 2*(tempImage.width() - ui->orgImage->size().width());
	}
	if(ui->orgImage->size().height() < tempImage.height()){
		minHeigh += (tempImage.height() - ui->orgImage->size().height());
	}
	ui->layoutWidget->setGeometry(QRect(10, 40, minWidth, minHeigh));//t.width()*106/50, t.height()*173/100));
    ui->layoutWidget->setFixedSize(minWidth, minHeigh);
    //ui->layoutWidget->adjustSize();
    ui->layoutWidget->updateGeometry();
    this->setFixedSize(minWidth+20, minHeigh + 80);
    //this->setMinimumSize(m_iMinWidth + 20, m_iMinHeight + 20);
    this->adjustSize();
    //this->updateGeometry();
    int desktop_width = desktopWidth;
    int desktop_height = desktopHeight;
    if(desktop_width > this->width()) desktop_width = (desktop_width - this->width()) / 2;
    else desktop_width = 0;
    if(desktop_height > this->height()) desktop_height = (desktop_height - this->height()) / 2;
    else desktop_height = 0;
    this->move(desktop_width, desktop_height);
}
