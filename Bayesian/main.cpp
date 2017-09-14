//#include "mainwindow.h"
//#include <QtWidgets/QApplication>
//
//#include "kdefog.h"
//#include "kfmrf.h"
//
//#include <fstream>
//#include <sstream>
//
//
//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    //MainWindow w;
//    //w.show();
//
//    std::ifstream infile("param.ini", std::ios::in);
//    if (infile)
//    {
//        std::string dummyRead("");
//        std::string tempKey("");
//        // &is = static_cast<std::istream&>(infile);//(infile.rdbuf());
//        while(std::getline(infile, dummyRead)){
//            dummyRead = dummyRead.substr(0, dummyRead.find_last_of("#"));
//            if(dummyRead.find_first_of(":=") == dummyRead.npos)
//                KRmrf::s_paramMap["default"]
//                   = dummyRead.substr( dummyRead.find_first_not_of(" \t") );
//            else{
//                tempKey = dummyRead.substr(0, dummyRead.find_first_of(":="));
//                KRmrf::trim(tempKey);
//                KRmrf::s_paramMap[tempKey]
//                   = dummyRead.substr( dummyRead.find_first_not_of(" \t", dummyRead.find_first_of(":=")+1) );
//            }
//        }
//    }
//    infile.close();
//
//    KRmrf::parseArguments();
//
//    //std::cout << is.str() << std::endl;
//  //  is >> temo;
//    //is >> temo >> dummyRead;
//
//
//    KDefog * pDefog = NULL;
//    try{
//        pDefog = new KDefog(KRmrf::s_imagePath.c_str());
//    }catch(...){
//        std::cerr << "error in construct defoger." << std::endl;
//    }
//
//    //pDefog->unittest_image_io("D://test_out.png", "D://inv_test_out.png");
//    //pDefog->setAirLight();
//
//    pDefog->setAirLight(0.5, 0.6, 0.7);
//    //pDefog->setAirLight(std::vector<double>(3, 0.001));
//    pDefog->deforg_routine();
//
//
//    std::cout << "any key to continue." << std::endl;
//
//    return a.exec();
//}

