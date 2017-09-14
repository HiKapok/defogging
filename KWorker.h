#pragma once

#include <QThread>
#include <QDebug>
#include <QStringList>

#include <string>



class KWorker : public QThread
{
    Q_OBJECT

public:
    KWorker() : sBRunning(true)
    {}
	bool testRunning(){
		QMutexLocker locker(&m_mutex);
        return sBRunning;
	}
public slots:
	void stopRunning(){ 
		QMutexLocker locker(&m_mutex);
        sBRunning = false;
	}
signals:
    void sendFinishedImage(QStringList path);
private:
    QMutex m_mutex;
    bool sBRunning;
};