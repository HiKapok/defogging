#ifndef KLABEL_H
#define KLABEL_H

#include <cfloat>
#include <vector>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QtGui/QImage>
#include <QtGui/QColor>
#include <QtGui/QPaintEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtCore/QString>
#include <QtWidgets/QMainWindow>

class KLabel : public QLabel
{
    Q_OBJECT
public:
    explicit KLabel(QWidget *parent=0, Qt::WindowFlags f=0)
        :QLabel(parent, f),
        m_bPressed(true),
        m_grabRect(),
        m_penWidth(2.){
    }
    explicit KLabel(const QString &text, QWidget *parent=0, Qt::WindowFlags f=0)
        :QLabel(text, parent, f),
        m_bPressed(true),
        m_grabRect(),
        m_penWidth(2.){
    }


    void setImage(QImage & image){
        m_qImage = image;
        m_pixMap = QPixmap::fromImage(image);
        this->setPixmap(m_pixMap);
        int tempWidth = ( m_pixMap.width() + m_pixMap.height() ) / 250;
        tempWidth = std::max(tempWidth, 2);
        tempWidth = std::min(tempWidth, 5);
        m_penWidth = tempWidth;
    }

    QRect getChooseArea(){
		QRect a=this->parentWidget()->geometry();
        if(!m_grabRect.isNull()){
            QRect tuneRect;
            tuneRect.setTop(m_grabRect.height() > 0 ? m_grabRect.top() : m_grabRect.bottom());
            tuneRect.setLeft(m_grabRect.width() > 0 ? m_grabRect.left() : m_grabRect.right());
            tuneRect.setBottom(m_grabRect.height() > 0 ? m_grabRect.bottom() : m_grabRect.top());
            tuneRect.setRight(m_grabRect.width() > 0 ? m_grabRect.right() : m_grabRect.left());
            tuneRect = tuneRect.intersected(m_pixMap.rect());
            return tuneRect;
        }
        return m_grabRect;
    }

	std::string getAirLightStr(){
		QRect rect = getChooseArea();
		if(rect.isNull()) return "0.5,0.5,0.5";
		//QRect interRect = m_qImage.rect().intersected(rect);
        QImage tempImage = m_qImage.copy(rect);

        if(tempImage.isNull()) return "0.5,0.5,0.5";

		std::string ret("");
        for(int index = 2;index >= 0;--index){
			double sum = 0.;
            for(int row = 0;row < tempImage.height();++row){
                for(int col = 0;col < tempImage.width();++col){
                     sum += ( ((static_cast<unsigned int>(tempImage.pixel(col, row)) >> (8*index)) & 0xff) );
                }
            }
            ret = ret + std::to_string(sum*(1./255.)/(tempImage.width()*tempImage.height())) + ",";
		}

		return ret.substr(0, ret.length() - 1);
	}

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent * event)
    {
        QLabel::paintEvent(event);
        //if(m_bPressed){
            m_painter.begin(this);
            m_painter.drawPixmap(0, 0, m_pixMap);

            m_painter.setPen(QPen(Qt::red, m_penWidth));
            if(!m_grabRect.isNull()){
                QRect tuneRect;
                tuneRect.setTop(m_grabRect.height() > 0 ? m_grabRect.top() : m_grabRect.bottom());
                tuneRect.setLeft(m_grabRect.width() > 0 ? m_grabRect.left() : m_grabRect.right());
                tuneRect.setBottom(m_grabRect.height() > 0 ? m_grabRect.bottom() : m_grabRect.top());
                tuneRect.setRight(m_grabRect.width() > 0 ? m_grabRect.right() : m_grabRect.left());
                tuneRect = tuneRect.intersected(m_pixMap.rect());
                m_painter.drawRect(tuneRect);
                //qDebug()<< tuneRect.left() << " " << tuneRect.top() << " " << tuneRect.right()
                                                 //<< " " << tuneRect.bottom() <<" image size: "<<m_pixMap.size();
            }

            m_painter.end();
       // }

    }
    virtual void mousePressEvent(QMouseEvent * event)
    {
        m_bPressed = true;
        m_grabRect.setTopLeft(event->pos());
        QLabel::mousePressEvent(event);
    }

    virtual void mouseReleaseEvent(QMouseEvent * event)
    {
        m_bPressed = false;
        m_grabRect.setBottomRight(event->pos());
        QLabel::mouseReleaseEvent(event);
        update();
    }

    virtual void mouseMoveEvent(QMouseEvent * event)
    {
        if(event->type() == QMouseEvent::MouseMove)
        if(m_bPressed){
            m_grabRect.setBottomRight(event->pos());
        }
        QLabel::mouseMoveEvent(event);
        update();
    }

private:
    bool m_bPressed;
    QRect m_grabRect;
    QPainter m_painter;
    QPixmap m_pixMap;
    QImage m_qImage;
    qreal m_penWidth;
};
#endif // KLABEL_H
