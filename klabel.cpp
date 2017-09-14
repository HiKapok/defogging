//#include "klabel.h"
//
//#include <QtGui/QPainter>
//
//
//void KLabel::paintEvent(QPaintEvent *)
//{
//    this->setWordWrap(true);
//    QPainter *paint = new QPainter();
//    paint->begin(this);
//	if(!(m_qImage.isNull())) {
//        this->setPixmap(QPixmap::fromImage(m_qImage));
//        //this->resize(this->pixmap()->size());
//        paint->drawPixmap(QPoint(0, 0),*(this->pixmap()));
//    }
//    else paint->drawText(0,0,50,50,Qt::AlignCenter, this->text());
//    paint->end();
//}
