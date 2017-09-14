//#ifndef KGRABWND_H
//#define KGRABWND_H

//#include <QtWidgets/QApplication>
//#include <QtWidgets/QMainWindow>
//#include <QtWidgets/QWidget>
//#include <QtWidgets/QLabel>
//#include <QPixmap>
//#include <QMouseEvent>
//#include <QMutex>
//#include <QWaitCondition>

//#include <QImage>
//#include <QRect>
//#include <QString>
//#include <QDebug>

//class KGrabWindow : public QMainWindow
//{
//    Q_OBJECT
//public:
//    explicit KGrabWindow(QWidget *parent = 0):
//        QMainWindow(parent)
//    {
//        if (this->objectName().isEmpty())
//            this->setObjectName(QStringLiteral("GrabWnd"));
//        this->resize(400, 300);

//        m_uiLabel = new QLabel;
//        m_uiLabel->setObjectName(QStringLiteral("imageLabel"));
//        //m_uiLabel->setGeometry(QRect(180, 70, 54, 12));
//        m_uiLabel->setText("image here.");
//        m_uiLabel->installEventFilter(this);


//        this->setCentralWidget(m_uiLabel);
//        this->setWindowTitle(QStringLiteral("Grab one Rect"));
//    }

//    bool eventFilter(QObject *obj, QEvent *event)
//    {
//        if (qobject_cast<QLabel*>(obj) == m_uiLabel) {
//            if (event->type() == QEvent::MouseButtonPress) {
//                QMouseEvent *msEvent = static_cast<QMouseEvent*>(event);
//                qDebug() << "Press:" << msEvent->pos();
//                return true;
//            } else if (event->type() == QEvent::MouseButtonRelease) {
//                QMouseEvent *msEvent = static_cast<QMouseEvent*>(event);
//                qDebug() << "Release:" << msEvent->pos();
//                m_cndVariable.wakeOne();
//                return true;
//            } else {
//                return false;
//            }
//        } else {
//            // pass the event on to the parent class
//            return QMainWindow::eventFilter(obj, event);
//        }
//    }

//    void prepareForGrab(QImage& image){

//        this->setWindowModality(Qt::ApplicationModal);
//        this->show();

//        m_uiLabel->setPixmap(QPixmap::fromImage(image));
//        m_uiLabel->resize(image.size());
//        this->resize(m_uiLabel->size());
//        this->setFixedSize(m_uiLabel->size());
//        this->show();



//    }

//    QRect getChooseArea(){
//        m_mutex.lock();
//        m_cndVariable.wait(&m_mutex);
//        m_mutex.unlock();
//        return QRect();
//    }



//    ~KGrabWindow(){

//    }

//private:
//    QLabel *m_uiLabel;
//    bool m_bPressed;
//    QMutex m_mutex;
//    QWaitCondition m_cndVariable;

//};

//#endif // KGRABWND_H


#ifndef KGRABWND_H
#define KGRABWND_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtGui/QPixmap>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

#include <QtGui/QImage>
#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtCore/QDebug>

#include <QDialog>

#include "../klabel.h"

//class KLabel : public QLabel
//{
//    Q_OBJECT
//public:
//    explicit KLabel(QWidget *parent=0, Qt::WindowFlags f=0)
//        :QLabel(parent, f),
//        m_bPressed(true),
//        m_grabRect(),
//        m_penWidth(2.){
//    }
//    explicit KLabel(const QString &text, QWidget *parent=0, Qt::WindowFlags f=0)
//        :QLabel(text, parent, f),
//        m_bPressed(true),
//        m_grabRect(),
//        m_penWidth(2.){
//    }
//
//
//    void setImage(QImage & image){
//        m_pixMap = QPixmap::fromImage(image);
//        this->setPixmap(m_pixMap);
//        int tempWidth = ( m_pixMap.width() + m_pixMap.height() ) / 250;
//        tempWidth = std::max(tempWidth, 2);
//        tempWidth = std::min(tempWidth, 5);
//        m_penWidth = tempWidth;
//    }
//
//    QRect getChooseArea(){
//        if(!m_grabRect.isNull()){
//            QRect tuneRect;
//            tuneRect.setTop(m_grabRect.height() > 0 ? m_grabRect.top() : m_grabRect.bottom());
//            tuneRect.setLeft(m_grabRect.width() > 0 ? m_grabRect.left() : m_grabRect.right());
//            tuneRect.setBottom(m_grabRect.height() > 0 ? m_grabRect.bottom() : m_grabRect.top());
//            tuneRect.setRight(m_grabRect.width() > 0 ? m_grabRect.right() : m_grabRect.left());
//            tuneRect = tuneRect.intersected(m_pixMap.rect());
//            return tuneRect;
//        }
//        return m_grabRect;
//    }
//
//    // QWidget interface
//protected:
//    virtual void paintEvent(QPaintEvent * event)
//    {
//        QLabel::paintEvent(event);
//        //if(m_bPressed){
//            m_painter.begin(this);
//            m_painter.drawPixmap(0, 0, m_pixMap);
//
//            m_painter.setPen(QPen(Qt::red, m_penWidth));
//            if(!m_grabRect.isNull()){
//                QRect tuneRect;
//                tuneRect.setTop(m_grabRect.height() > 0 ? m_grabRect.top() : m_grabRect.bottom());
//                tuneRect.setLeft(m_grabRect.width() > 0 ? m_grabRect.left() : m_grabRect.right());
//                tuneRect.setBottom(m_grabRect.height() > 0 ? m_grabRect.bottom() : m_grabRect.top());
//                tuneRect.setRight(m_grabRect.width() > 0 ? m_grabRect.right() : m_grabRect.left());
//                tuneRect = tuneRect.intersected(m_pixMap.rect());
//                m_painter.drawRect(tuneRect);
//                //qDebug()<< tuneRect.left() << " " << tuneRect.top() << " " << tuneRect.right()
//                                                 //<< " " << tuneRect.bottom() <<" image size: "<<m_pixMap.size();
//            }
//
//            m_painter.end();
//       // }
//
//    }
//    virtual void mousePressEvent(QMouseEvent * event)
//    {
//        m_bPressed = true;
//        m_grabRect.setTopLeft(event->pos());
//        QLabel::mousePressEvent(event);
//    }
//
//    virtual void mouseReleaseEvent(QMouseEvent * event)
//    {
//        m_bPressed = false;
//        m_grabRect.setBottomRight(event->pos());
//        QLabel::mouseReleaseEvent(event);
//        update();
//    }
//
//    virtual void mouseMoveEvent(QMouseEvent * event)
//    {
//        if(event->type() == QMouseEvent::MouseMove)
//        if(m_bPressed){
//            m_grabRect.setBottomRight(event->pos());
//        }
//        QLabel::mouseMoveEvent(event);
//        update();
//    }
//
//private:
//    bool m_bPressed;
//    QRect m_grabRect;
//    QPainter m_painter;
//    QPixmap m_pixMap;
//    qreal m_penWidth;
//};

class KGrabWindow : public QDialog
{
    Q_OBJECT
public:
    explicit KGrabWindow(QWidget *parent = 0):
        QDialog(parent)
    {
        if (this->objectName().isEmpty())
            this->setObjectName(QStringLiteral("GrabWnd"));
		this->setWindowModality(Qt::WindowModal);
        this->resize(400, 300);

        m_uiLabel = new KLabel(this);
        m_uiLabel->setObjectName(QStringLiteral("imageLabel"));
        //m_uiLabel->setGeometry(QRect(180, 70, 54, 12));
        m_uiLabel->setText("image here.");
//        m_uiLabel->installEventFilter(this);
        //this->setCentralWidget(m_uiLabel);
        this->setWindowTitle(QStringLiteral("Grab one Rect"));
    }

    void prepareForGrab(QImage& image){
        m_uiLabel->setImage(image);
        m_uiLabel->resize(image.size());
        this->resize(m_uiLabel->size());
        this->setFixedSize(m_uiLabel->size());
        this->setWindowModality(Qt::ApplicationModal);
        this->show();
    }

    std::string getChooseAirLight(){
        return m_airLightStr;
    }
	    // QDialog interface
protected:
    virtual void closeEvent(QCloseEvent * event)
    {
		m_airLightStr = m_uiLabel->getAirLightStr();
        event->accept();
    }

protected:
    virtual void keyPressEvent(QKeyEvent * event)
    {
        if(event->key() == Qt::Key_Return){  
            this->close();
        }
    }
private:
    KLabel *m_uiLabel;
    std::string m_airLightStr;
};

#endif // KGRABWND_H
