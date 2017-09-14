#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "Bayesian/kfmrf.h"
#include "bayesiandialog.h"
#include "bccrdialog.h"
#include "ui_bayesiandialog.h"
#include "Bayesian/kgrabwnd.h"
#include "KWorker.h"
#include "KRunBayesian.h"
#include "KRunDarkChannal.h"
#include "KRunBCCR.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow(){
		emit stopAllThreads();
		for(auto & w : m_worker){ w->wait(); delete w; }
		deleteUI();
		
	}
	void clear_message();
    // set the message of status bar
    void set_message(QString, int _timeout = 2000);
	void setOrgImage(const QImage&);
	void setDefogImage(const QImage&);
	void resizeWnd();
	void deleteUI();
	std::pair<double, double> getPSNR(const cv::Mat& I1, const cv::Mat& I2)
	{
		cv::Mat s1;
		absdiff(I1, I2, s1);       // |I1 - I2|
		s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
		s1 = s1.mul(s1);           // |I1 - I2|^2

		cv::Scalar s = sum(s1);         // sum elements per channel

		double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

		double minVal = std::numeric_limits<double>::max();
		double maxVal = std::numeric_limits<double>::min();
		std::vector<cv::Mat> vecImg;
		cv::split(I1, vecImg);

		for(std::vector<cv::Mat>::size_type index = 0;index < vecImg.size();++index){
			cv::Mat& img = vecImg[index];
			double tempMinVal;
			double tempMaxVal;
			cv::minMaxLoc( vecImg[index], &tempMinVal, &tempMaxVal, NULL, NULL, cv::Mat() );
			minVal = std::min(minVal, tempMinVal);
			maxVal = std::max(maxVal, tempMaxVal);
		}


		if( sse <= 1e-10) // for small values return zero
			return std::make_pair(0., 0.);
		else
		{
			double mse = sse /(double)(I1.channels() * I1.total());
			double psnr = 10.0*log10(((maxVal-minVal)*(maxVal-minVal))/mse);
			return std::make_pair(mse, psnr);
		}
	}

	double getMSSIM( const cv::Mat& i1, const cv::Mat& i2)
	{
		const double C1 = 6.5025, C2 = 58.5225;
		/***************************** INITS **********************************/
		int d     = CV_32F;

		cv::Mat I1, I2;
		i1.convertTo(I1, d);           // cannot calculate on one byte large values
		i2.convertTo(I2, d);

		cv::Mat I2_2   = I2.mul(I2);        // I2^2
		cv::Mat I1_2   = I1.mul(I1);        // I1^2
		cv::Mat I1_I2  = I1.mul(I2);        // I1 * I2

		/*************************** END INITS **********************************/

		cv::Mat mu1, mu2;   // PRELIMINARY COMPUTING
		GaussianBlur(I1, mu1, cv::Size(11, 11), 1.5);
		GaussianBlur(I2, mu2, cv::Size(11, 11), 1.5);

		cv::Mat mu1_2   =   mu1.mul(mu1);
		cv::Mat mu2_2   =   mu2.mul(mu2);
		cv::Mat mu1_mu2 =   mu1.mul(mu2);

		cv::Mat sigma1_2, sigma2_2, sigma12;

		GaussianBlur(I1_2, sigma1_2, cv::Size(11, 11), 1.5);
		sigma1_2 -= mu1_2;

		GaussianBlur(I2_2, sigma2_2, cv::Size(11, 11), 1.5);
		sigma2_2 -= mu2_2;

		GaussianBlur(I1_I2, sigma12, cv::Size(11, 11), 1.5);
		sigma12 -= mu1_mu2;

		///////////////////////////////// FORMULA ////////////////////////////////
		cv::Mat t1, t2, t3;

		t1 = 2 * mu1_mu2 + C1;
		t2 = 2 * sigma12 + C2;
		t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

		t1 = mu1_2 + mu2_2 + C1;
		t2 = sigma1_2 + sigma2_2 + C2;
		t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

		cv::Mat ssim_map;
		divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

		cv::Scalar mssim = mean( ssim_map ); // mssim = average of ssim map
		return (mssim[0] + mssim[1] + mssim[2])/3;
	}

	double getAvgGradient(const cv::Mat& i1){
		std::vector<cv::Mat> vecImg;
		cv::split(i1, vecImg);

		double gradients = 0.;
		for(std::vector<cv::Mat>::size_type index = 0;index < vecImg.size();++index){
			cv::Mat& img = vecImg[index];
			cv::Mat fMat;
			img.convertTo(fMat,CV_64FC1);

			for(int height = 0;height + 1 < fMat.rows; ++height){
				for(int width = 0;width + 1 < fMat.cols; ++width){
					double dx = fMat.at<double>(height,width+1) - fMat.at<double>(height,width);
					double dy = fMat.at<double>(height+1,width) - fMat.at<double>(height,width);
					double ds = std::sqrt((dx*dx+dy*dy)/2);
					gradients = gradients + ds;
				}
			}
		}
		return gradients/((i1.rows-1)*(i1.cols-1) * vecImg.size());
	}


signals:
    void stopAllThreads();
private slots:
	void open_save_path(){
		m_outputPath = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("选择保存路径"),QString::fromLocal8Bit("C:/"));
		if(!(m_outputPath.isEmpty())){ 
			set_message(QString::fromLocal8Bit("提示：保存路径：") + m_outputPath);
		}
	}

	void open_image(){
		QString tempInput = QFileDialog::getOpenFileName(this,
            QString::fromLocal8Bit("打开图像"), ".", QString::fromLocal8Bit("Image Files (*.png *.jpg *.bmp)"));
		if(!(tempInput.isEmpty())){ 
			QImage tempImage = QImage(tempInput);
			/*this->ui->orgImage->setPixmap(QPixmap::fromImage(m_qImage));
			this->ui->defogImage->setPixmap(QPixmap::fromImage(m_qImage));*/
			if(tempImage.isNull()){
				set_message(QString::fromLocal8Bit("错误：图像打开失败！"));
				return;
			}
			emit stopAllThreads();
			m_qImage = tempImage;
			resizeWnd();
			setOrgImage(m_qImage);
			setDefogImage(m_qImage);
			
			set_message(QString::fromLocal8Bit("提示：载入图像成功！图像路径：") + tempInput);
			
			std::string tempStr = tempInput.toUtf8();
			m_imgNameNoExt = tempStr.substr(tempStr.find_last_of("\\/")+1);
            		m_imgNameNoExt = m_imgNameNoExt.substr(0, m_imgNameNoExt.find_last_of("."));
			m_inputImage = tempInput;
		}
		else set_message(QString::fromLocal8Bit("提示：未打开任何图像！"));
	}

	void run_drakChannal(){
		if(m_qImage.isNull()){
			set_message(QString::fromLocal8Bit("提示：未打开任何图像！"));
			return;
		}
		
		emit stopAllThreads();
		std::string outputFileName("");
		
		if(!(m_outputPath.isEmpty())){
			outputFileName = (m_outputPath.toLocal8Bit().data() + m_imgNameNoExt + "_out.bmp");
		}else{
			std::string tempStr = m_inputImage.toLocal8Bit().data();
            std::string imgNameNoExt = tempStr.substr(tempStr.find_last_of("\\/")+1);
            imgNameNoExt = imgNameNoExt.substr(0, imgNameNoExt.find_last_of("."));
            outputFileName = tempStr.substr(0, tempStr.find_last_of("\\/")+1)+imgNameNoExt+"_out.bmp";
		}
		set_message(QString::fromLocal8Bit("提示：正在处理....."), 0);
		m_worker.push_back(new DrakChannalThread());
		dynamic_cast<DrakChannalThread*>(m_worker.back())->setInputImage(m_inputImage.toLocal8Bit().data());
		dynamic_cast<DrakChannalThread*>(m_worker.back())->setOutputPath(outputFileName);
		connect(m_worker.back(), SIGNAL(sendFinishedImage(QStringList)), this, SLOT(getProcessResult(const QStringList&)));
		connect(this, SIGNAL(stopAllThreads()), m_worker.back(), SLOT(stopRunning()));
		
		m_worker.back()->start();
	}
	void run_RegDefog(){
		if(m_qImage.isNull()){
			set_message(QString::fromLocal8Bit("提示：未打开任何图像！"));
			return;
		}
		
		emit stopAllThreads();
		std::string outputFileName("");
		
		if(!(m_outputPath.isEmpty())){
			outputFileName = (m_outputPath.toLocal8Bit().data() + m_imgNameNoExt + "_out.bmp");
		}else{
			std::string tempStr = m_inputImage.toLocal8Bit().data();
            std::string imgNameNoExt = tempStr.substr(tempStr.find_last_of("\\/")+1);
            imgNameNoExt = imgNameNoExt.substr(0, imgNameNoExt.find_last_of("."));
            outputFileName = tempStr.substr(0, tempStr.find_last_of("\\/")+1)+imgNameNoExt+"_out.bmp";
		}

		BCCRDialog form(this);
		form.setupSLOT(this);
		form.show();
		form.exec();

		set_message(QString::fromLocal8Bit("提示：正在处理....."), 0);
		m_worker.push_back(new KRunBCCR(m_inputImage.toLocal8Bit().data(), outputFileName, airWndSize, boundWndSize, lambda));
		connect(m_worker.back(), SIGNAL(sendFinishedImage(QStringList)), this, SLOT(getProcessResult(const QStringList&)));
		connect(this, SIGNAL(stopAllThreads()), m_worker.back(), SLOT(stopRunning()));
		
		m_worker.back()->start();
	}

	void run_bayesain(){
		if(m_qImage.isNull()){
			set_message(QString::fromLocal8Bit("提示：未打开任何图像！"));
			return;
		}
		
		emit stopAllThreads();
		BayesianDialog form(this);
		connect(form.ui->leOutIter, SIGNAL(textChanged(QString)), this, SLOT(setOutIter(const QString)));
		connect(form.ui->leInIter, SIGNAL(textChanged(QString)), this, SLOT(setInIter(const QString)));
		connect(form.ui->leAirLight, SIGNAL(textChanged(QString)), this, SLOT(setAirlight(const QString)));
		//connect(form.ui->leWeigthC, SIGNAL(textChanged(QString)), this, SLOT(setPriorC(const QString)));
		//connect(form.ui->leWeigthD, SIGNAL(textChanged(QString)), this, SLOT(setPriorD(const QString)));
		connect(form.ui->priorType, SIGNAL(activated(const QString &)), this, SLOT(setDepthProir(const QString)));
		connect(form.ui->cbMultiScale, SIGNAL(toggled(bool)), this, SLOT(setMultiScale(const bool)));
		connect(form.ui->cbSaveInitial, SIGNAL(toggled(bool)), this, SLOT(setSaveInitial(const bool)));
		connect(form.ui->btSelAirLight, SIGNAL(released()), this, SLOT(setAirlight()));
		form.show();
		form.exec();
		//
		if(!(m_outputPath.isEmpty())){
			KRmrf::s_paramMap["albedo-output"] = (m_outputPath.toUtf8().data() + m_imgNameNoExt + "_albedo");
			KRmrf::s_paramMap["depth-output"] = (m_outputPath.toUtf8().data() + m_imgNameNoExt + "_depth");
		}
		KRmrf::s_paramMap["image"] = m_inputImage.toUtf8();

		set_message(QString::fromLocal8Bit("提示：正在处理....."), 0);
		m_worker.push_back(new BayesianThread());
		connect(m_worker.back(), SIGNAL(sendFinishedImage(QStringList)), this, SLOT(getProcessResult(const QStringList&)));
		connect(this, SIGNAL(stopAllThreads()), m_worker.back(), SLOT(stopRunning()));
		
		m_worker.back()->start();
    }

	

	QString runEvaluation(std::string input, std::string output){
		cv::Mat inputImg = cv::imread(input);
		cv::Mat outputImg = cv::imread(output);
		/*cv::Mat a1(5,5,CV_32FC1);
		for(int i=0;i<5;i++){
			for(int j=0;j<5;j++)
				a1.at<float>(i,j)=i+j;
		}
		cv::Mat a2(5,5,CV_32FC1);
		for(int i=0;i<5;i++){
		for(int j=0;j<5;j++)
		a2.at<float>(i,j)=2*i+j;
		}*/
		
		//std::pair<double, double> msePair = getPSNR(a1, a2);
		std::pair<double, double> msePair = getPSNR(outputImg, inputImg);
		double mssim = getMSSIM(outputImg, inputImg);
		double gradient = getAvgGradient(outputImg);
		return QString::fromLocal8Bit("MSE:%1 PSNR:%2 MSSIM:%3 AvgGradient:%4.").arg(msePair.first, 0, 'f', 3).arg(msePair.second, 0, 'f', 3).arg(mssim, 0, 'f', 3).arg(gradient, 0, 'f', 3);
	}

	void getProcessResult(const QStringList& path){
		QString baseVal = runEvaluation(path.at(0).toLocal8Bit().data(), path.at(1).toLocal8Bit().data());
		//set_message(QString::fromLocal8Bit("提示：处理完毕，保存路径：") + QString::fromUtf8(std::string(path.at(1).toUtf8().data()).c_str()) + QString::fromLocal8Bit(" ") + baseVal, 5000);
		set_message(QString::fromLocal8Bit("提示：处理完毕，评价指标：") + baseVal, 5000);
		std::cout << "处理完毕，评价指标：" + std::string(baseVal.toLocal8Bit().data()) << std::endl;
		std::cout << "处理完毕，保存路径：" + std::string(path.at(1).toLocal8Bit().data()) << std::endl;
		setDefogImage(QImage(path.at(1)));
	}

	void setAirWndSize(const QString str){
        bool no_error = true;
        double temp = str.toDouble(&no_error);
        if(!no_error)
        {
            set_message(QString::fromLocal8Bit("错误：请输入整数！"));
            return;
        }
		airWndSize = static_cast<int>(temp);
    }
	void setBoundWndSize(const QString str){
        bool no_error = true;
        double temp = str.toDouble(&no_error);
        if(!no_error)
        {
            set_message(QString::fromLocal8Bit("错误：请输入整数！"));
            return;
        }
		boundWndSize = static_cast<int>(temp);
    }
	void setRegLamda(const QString str){
        bool no_error = true;
        double temp = str.toDouble(&no_error);
        if(!no_error)
        {
            set_message(QString::fromLocal8Bit("错误：请输入浮点数！"));
            return;
        }
		lambda = temp;
    }

    void setOutIter(const QString str){
        bool no_error = true;
        double temp = str.toDouble(&no_error);
        if(!no_error)
        {
            set_message(QString::fromLocal8Bit("错误：请输入整数！"));
            return;
        }
		KRmrf::s_paramMap["n_outer_iterations"] = std::to_string(static_cast<int>(temp));
    }
	void setInIter(const QString str){
        bool no_error = true;
        double temp = str.toDouble(&no_error);
        if(!no_error)
        {
            set_message(QString::fromLocal8Bit("错误：请输入整数！"));
            return;
        }
		KRmrf::s_paramMap["n_inner_iterations"] = std::to_string(static_cast<int>(temp));
    }
	void setPriorC(const QString str){
        bool no_error = true;
        double temp = str.toDouble(&no_error);
        if(!no_error)
        {
            set_message(QString::fromLocal8Bit("错误：请输入浮点数！"));
            return;
        }
		KRmrf::s_paramMap["prior_C_weight"] = std::to_string(temp);
    }
	void setPriorD(const QString str){
        bool no_error = true;
        double temp = str.toDouble(&no_error);
        if(!no_error)
        {
            set_message(QString::fromLocal8Bit("错误：请输入浮点数！"));
            return;
        }
		KRmrf::s_paramMap["prior_D_weight"] = std::to_string(temp);
    }
	void setAirlight(const QString str){
		//set_message(str);
		KRmrf::s_paramMap["airlight"] = str.toStdString();
    }
	void setAirlight(){
		QObject *ptr = sender();
		BayesianDialog* parent =  qobject_cast<BayesianDialog*>(ptr->parent()->parent());
	   if( parent != NULL ) 
	   { 
			KGrabWindow form(parent);
			form.prepareForGrab(m_qImage);
			form.show();
			form.exec();
			//KRmrf::s_paramMap["airlight"] = form.getChooseAirLight();
			parent->ui->leAirLight->setText(QString::fromLocal8Bit(form.getChooseAirLight().c_str()));
	   } 
		//KRmrf::s_paramMap["airlight"] = str.toStdString();
    }
	//void toggled(bool checked)
	void setMultiScale(const bool sts){
		if(sts) KRmrf::s_paramMap["ms"] = "1";
		else KRmrf::s_paramMap["ms"] = "0";
	}
	//void toggled(bool checked)
	void setSaveInitial(const bool sts){
		if(sts) KRmrf::s_paramMap["id"] = "1";
		else KRmrf::s_paramMap["id"] = "0";
	}
	//void QComboBox::activated(const QString & text)
	void setDepthProir(const QString item){
		KRmrf::s_paramMap["dpt"] = item.toStdString();
	}
private:
    Ui::MainWindow *ui;
	QImage m_qImage;
	QString m_inputImage;
	QString m_outputPath;
	std::string m_imgNameNoExt;
	std::vector<KWorker*> m_worker;
	int desktopWidth;
	int desktopHeight;
	int airWndSize;
	int boundWndSize;
	double lambda;
};



//std::string imgNameNoExt("");
//    {
//        mapIter it = s_paramMap.find("image");
//        if(it != s_paramMap.end()){
//            s_imagePath = it->second;
//            trim(s_imagePath);
//            imgNameNoExt = s_imagePath.substr(s_imagePath.find_last_of("\\/")+1);
//            imgNameNoExt = imgNameNoExt.substr(0, imgNameNoExt.find_last_of("."));
//            imgNameNoExt = s_imagePath.substr(0, s_imagePath.find_last_of("\\/")+1)+imgNameNoExt;
//        }else s_imagePath="";
//    }
//    {
//        mapIter it = s_paramMap.find("albedo-output");
//        if(it != s_paramMap.end()){
//            s_albedoOutPath = it->second;
//            trim(s_albedoOutPath);
//        }else s_albedoOutPath=imgNameNoExt+"_albedo";
//
//    }
//    {
//        mapIter it = s_paramMap.find("depth-output");
//        if(it != s_paramMap.end()){
//            s_depthOutPath = it->second;
//            trim(s_depthOutPath);
//        }else s_depthOutPath=imgNameNoExt+"_depth";
//    }
//

#endif // MAINWINDOW_H
