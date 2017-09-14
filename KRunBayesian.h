#pragma once

#include <QThread>
#include <QDebug>

#include <functional>

#include "KWorker.h"
#include "./Bayesian/kdefog.h"



class BayesianThread : public KWorker
{
    Q_OBJECT

public:
    BayesianThread() : KWorker()
    {}

    void run()
    {
			//std::ifstream infile("param.ini", std::ios::in);
			//if (infile)
			//{
			//	std::string dummyRead("");
			//	std::string tempKey("");
			//	// &is = static_cast<std::istream&>(infile);//(infile.rdbuf());
			//	while(std::getline(infile, dummyRead)){
			//		dummyRead = dummyRead.substr(0, dummyRead.find_last_of("#"));
			//		if(dummyRead.find_first_of(":=") == dummyRead.npos)
			//			KRmrf::s_paramMap["default"]
			//				= dummyRead.substr( dummyRead.find_first_not_of(" \t") );
			//		else{
			//			tempKey = dummyRead.substr(0, dummyRead.find_first_of(":="));
			//			KRmrf::trim(tempKey);
			//			KRmrf::s_paramMap[tempKey]
			//				= dummyRead.substr( dummyRead.find_first_not_of(" \t", dummyRead.find_first_of(":=")+1) );
			//		}
			//	}
			//}
			//infile.close();

			if (!testRunning()) return;

			KRmrf::parseArguments();

			std::string input = KRmrf::s_imagePath;
			std::string output = KRmrf::s_albedoOutPath+".bmp";
			KDefog * pDefog = NULL;
			try{
				pDefog = new KDefog(input);
			}catch(...){
				std::cerr << "error in construct defoger." << std::endl;
			}
			pDefog->setTestRunFunc(this);
			//pDefog->setAirLight(0.5, 0.6, 0.7);
			//pDefog->setAirLight(std::vector<double>(3, 0.001));
			if(pDefog->deforg_routine()){
				emit sendFinishedImage(QStringList() << QString::fromUtf8(input.c_str()) << QString::fromUtf8(output.c_str()));
			}else qDebug() << "bayesian defogging canceled!";
			
    }
};