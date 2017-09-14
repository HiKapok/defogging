#ifndef KDEFOG_H
#define KDEFOG_H

//#include "gdal_priv.h"
#include "kgrabwnd.h"
#include "kfunutils.h"
#include "KWorker.h"

#include <QtGui/QImage>
#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtCore/QDebug>

//#include <QtConcurrent/QtConcurrentRun>

#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <functional>


struct KFileNotExist : public std::exception{
    const char * what() const throw() {
        return "file not exist!\r\n";
    }
};

struct KNotSupportFile : public std::exception{
    const char * what() const throw() {
        return "filetype is not supported!\r\n";
    }
};

class KDefog
{
public:
    KDefog(std::string imageName, bool bSaveInitialDepth = true):
        m_imageName(imageName),
        m_qImage(imageName.c_str()),
        m_bSaveInitialDepth(bSaveInitialDepth),
		callBackPtr(nullptr){
        if(m_qImage.isNull()) throw KFileNotExist();
        if(m_qImage.depth() < 24) throw KNotSupportFile();
        cvtQImg2RawImg(m_qImage, m_rawImage);
        KRmrf::s_airLight.resize(3);
		
        KRmrf::orgImageWidth = m_qImage.width();
        KRmrf::orgImageHeight = m_qImage.height();

        KRmrf::curImageWidth = m_qImage.width();
        KRmrf::curImageHeight = m_qImage.height();

        //std::cout<< m_imageName.substr(0, m_imageName.find_last_of("\\/")+1) << std::endl;
        m_imgNameNoExt = m_imageName.substr(m_imageName.find_last_of("\\/")+1);
        m_imgNameNoExt = m_imgNameNoExt.substr(0, m_imgNameNoExt.find_last_of("."));
    }

	void setTestRunFunc(KWorker * ptr){
		callBackPtr = ptr;
	}
	
    bool deforg_routine(){
        std::vector< std::vector<double> > fRawImage;
        cvtIntImage2Double(m_rawImage, fRawImage);

        if(m_bSaveInitialDepth){
            getAndSaveInitialDepth(m_imageName.substr(0, m_imageName.find_last_of("\\/")+1)+m_imgNameNoExt+"_initial_depth.bmp", fRawImage, "BMP");
        }

		if(callBackPtr) if(!(callBackPtr->testRunning())) return false;

        KRmrf rmrf(callBackPtr);

        std::pair<std::vector< std::vector<double> >, std::vector<double> > final_ret_pair;
        if(KRmrf::s_bMultiScale){
            std::vector<double> scale;
            scale.push_back(0.5);
            scale.push_back(1.);
            final_ret_pair = rmrf.factorize_multiscale(fRawImage, scale, KRmrf::s_numOuterIter, KRmrf::s_numInnerIter);
        }else{
            final_ret_pair = rmrf.factorize(fRawImage, std::vector< std::vector<double> >(), std::vector<double>(), KRmrf::s_numOuterIter, KRmrf::s_numInnerIter);
        }

		if(callBackPtr) if(!(callBackPtr->testRunning())) return false;

        std::vector< std::vector<double> > final_albedo(final_ret_pair.first);
        for(std::vector< std::vector<double> >::size_type index = 0;index < final_albedo.size();++index){
            for(std::vector<double>::size_type imageIndex = 0;imageIndex < final_albedo[index].size();++imageIndex){
                final_albedo[index][imageIndex] = std::pow(final_albedo[index][imageIndex], 1/2.);
                //final_albedo[index][imageIndex] *= 255;
                //if(final_albedo[index][imageIndex] > 255) final_albedo[index][imageIndex] = 255;
            }
        }

        double max_albedo_pixel = DBL_MIN;
        double min_albedo_pixel = DBL_MAX;
        for(std::vector< std::vector<double> >::size_type index = 0;index < final_albedo.size();++index){
            for(std::vector<double>::size_type imageIndex = 0;imageIndex < final_albedo[index].size();++imageIndex){
                if(max_albedo_pixel < final_albedo[index][imageIndex]) max_albedo_pixel = final_albedo[index][imageIndex];
                if(min_albedo_pixel > final_albedo[index][imageIndex]) min_albedo_pixel = final_albedo[index][imageIndex];
            }
        }

        std::vector<std::vector<unsigned int> > tempScaleAlbedoImg;
        tempScaleAlbedoImg.resize(final_albedo.size());
        for(std::vector< std::vector<double> >::size_type index = 0;index < tempScaleAlbedoImg.size();++index){
            tempScaleAlbedoImg[index].resize(final_albedo[index].size());
            for(std::vector<double>::size_type imageIndex = 0;imageIndex < tempScaleAlbedoImg[index].size();++imageIndex){
                //tempScaleAlbedoImg[index][imageIndex] = final_albedo[index][imageIndex];
                //tempScaleAlbedoImg[index][imageIndex] = static_cast<unsigned int>( (final_albedo[index][imageIndex]-min_albedo_pixel)*255/(max_albedo_pixel-min_albedo_pixel) );
                tempScaleAlbedoImg[index][imageIndex] = static_cast<unsigned int>( (final_albedo[index][imageIndex]-0) * 255 );
            }
        }

		if(callBackPtr) if(!(callBackPtr->testRunning())) return false;

        saveImage(KRmrf::s_albedoOutPath+".bmp", tempScaleAlbedoImg, KRmrf::curImageWidth, KRmrf::curImageHeight, "BMP");

        std::vector<double> final_depth(final_ret_pair.second);
        double max_value = DBL_EPSILON;
        for(std::vector<double>::size_type imageIndex = 0;imageIndex < final_depth.size();++imageIndex){
            if(max_value < final_depth[imageIndex]) max_value = final_depth[imageIndex];
        }
        for(std::vector<double>::size_type imageIndex = 0;imageIndex < final_depth.size();++imageIndex){
            final_depth[imageIndex] /= max_value;
            final_depth[imageIndex] = 1. - final_depth[imageIndex];
        }

		if(callBackPtr) if(!(callBackPtr->testRunning())) return false;
        // scale the image to [0,255]
        double max_pixel = DBL_MIN;
        double min_pixel = DBL_MAX;
        std::vector<unsigned int> tempScaleDepthImg;
        tempScaleDepthImg.resize(final_depth.size());
        for(std::vector<double>::size_type imageIndex = 0;imageIndex < final_depth.size();++imageIndex){
            if(max_pixel < final_depth[imageIndex]) max_pixel = final_depth[imageIndex];
            if(min_pixel > final_depth[imageIndex]) min_pixel = final_depth[imageIndex];
        }

        for(std::vector<double>::size_type imageIndex = 0;imageIndex < final_depth.size();++imageIndex){
            //tempScaleDepthImg[imageIndex] = static_cast<unsigned int>( (final_depth[imageIndex]-0)*255 );
            tempScaleDepthImg[imageIndex] = static_cast<unsigned int>( (final_depth[imageIndex]-min_pixel)*255/(max_pixel-min_pixel) );
        }

		if(callBackPtr) if(!(callBackPtr->testRunning())) return false;
        saveImage(KRmrf::s_depthOutPath+".bmp", std::vector< std::vector<unsigned int> >(1, tempScaleDepthImg), KRmrf::curImageWidth, KRmrf::curImageHeight, "BMP");


        return true;
    }

    bool setAirLight(const std::vector<double> & airLight){
        KRmrf::s_airLight.assign(airLight.begin(), airLight.end());
        return true;
    }

    bool setAirLight(const double r, const double g, const double b){
        KRmrf::s_airLight.resize(0);
        KRmrf::s_airLight.push_back(r);
        KRmrf::s_airLight.push_back(g);
        KRmrf::s_airLight.push_back(b);
        return true;
    }

    bool setAirLight(const QRect & rect){
        if(!m_qImage.rect().contains(rect)) std::cerr << "w: given rect is not bounded by the image." << std::endl;
        QRect interRect = m_qImage.rect().intersected(rect);
        QImage tempImage = m_qImage.copy(interRect);

        if(tempImage.isNull()) return false;
        std::vector< std::vector<unsigned int> > vecOutImage;
        cvtQImg2RawImg(tempImage, vecOutImage);

        KRmrf::s_airLight.clear();
        for(std::vector< std::vector<unsigned int> >::size_type index = 0;index < vecOutImage.size();++index){
            double sum = 0.;
            for(std::vector<unsigned int>::size_type imageIndex = 0;imageIndex < vecOutImage[index].size();++imageIndex){
                sum += vecOutImage[index][imageIndex];
            }
            KRmrf::s_airLight.push_back(sum*1./vecOutImage[index].size());
        }

        return true;
    }

    bool setAirLight(){
        static KGrabWindow grabWnd(QApplication::activeWindow());
        grabWnd.prepareForGrab(m_qImage);
		grabWnd.getChooseAirLight();
        //QFuture<QRect> future= QtConcurrent::run(&grabWnd, &KGrabWindow::getChooseArea);
        //future.waitForFinished();
        return true;
    }

    bool unittest_image_io(const QString & outputFile, const std::string & invOutputFile, const std::string format = "PNG"){
       /* GDALAllRegister();
        CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

        GDALDataset * pInDataset = (GDALDataset *)GDALOpen(m_imageName.c_str(), GA_ReadOnly);
        if (pInDataset == NULL)
        {
            std::cerr << "failed to open image file!" << std::endl;
            GDALDestroyDriverManager();
            return false;
        }
        int Width = pInDataset->GetRasterXSize();
        int Height = pInDataset->GetRasterYSize();
        int Count = pInDataset->GetRasterCount();

        CPLErr err;
        std::vector< std::vector<unsigned int> > vecImage;
        vecImage.resize(Count);
        for(int index = 0;index < Count;++index){
            vecImage[index].resize(Width*Height);
            GDALRasterBand  *pInRasterBand = pInDataset->GetRasterBand(index+1);
            err = pInRasterBand->RasterIO(GF_Read, 0, 0, Width, Height, &vecImage[index][0], Width, Height, GDT_UInt32, 0, 0);

            if (err == CE_Failure)
            {
                std::cerr << "failed to read data from image file!" << std::endl;
                GDALClose(pInDataset);
                GDALDestroyDriverManager();
                return false;
            }
        }

        QImage ret;
        cvtRawImg2QImg(vecImage, Width, Height, ret);
        ret.save(outputFile, format.c_str());


        std::vector< std::vector<unsigned int> > vecOutImage;
        cvtQImg2RawImg(ret, vecOutImage);
        if(vecOutImage.size() != static_cast<std::vector< std::vector<unsigned int> >::size_type>(Count)){
            std::cerr << "test Qimage to RawImage failed!" << std::endl;
            GDALClose(pInDataset);
            GDALDestroyDriverManager();
            return false;
        }
        GDALDataset *poDstDS = pInDataset->GetDriver()->CreateCopy( invOutputFile.c_str(), pInDataset, FALSE, NULL, NULL, NULL );

        for(int bandIndex = 0;bandIndex < Count;++bandIndex)
        {
            GDALRasterBand *poBand = poDstDS->GetRasterBand(bandIndex + 1);
            poBand->RasterIO( GF_Write, 0, 0, Width, Height, &vecOutImage[bandIndex][0], Width, Height, GDT_UInt32, 0, 0 );
            poBand->FlushCache();
        }

        poDstDS->FlushCache();
        GDALClose(pInDataset);
        GDALClose(poDstDS);
        GDALDestroyDriverManager();*/
        return true;
    }

private:
    std::string m_imageName;
    QImage m_qImage;
    bool m_bSaveInitialDepth;
    std::vector< std::vector<unsigned int> > m_rawImage;
    std::string m_imgNameNoExt;
	KWorker * callBackPtr;

    void cvtQImg2RawImg(QImage & image, std::vector< std::vector<unsigned int> >& rawImage){
        rawImage.resize(0);
        rawImage.reserve(3);
        std::vector<unsigned int> tempVec;
        for(int index = 2;index >= 0;--index){
            tempVec.resize(0);
            tempVec.reserve(static_cast<unsigned int>(image.width()*image.height()));
            for(int row = 0;row < image.height();++row){
                for(int col = 0;col < image.width();++col){
                    tempVec.push_back( ((static_cast<unsigned int>(image.pixel(col, row)) >> (8*index)) & 0xff) );
                }
            }
            rawImage.push_back(tempVec);
        }
    }

    bool cvtRawImg2QImg(const std::vector< std::vector<unsigned int> >& rawImage, int width, int height, QImage & image){
        //qDebug() << rawImage.size();
        if(rawImage.size() == 1){
            image = QImage(width, height, QImage::Format_RGB32);
            for(int row = 0;row < height;++row){
                for(int col = 0;col < width;++col){
                    image.setPixel(col, row, qRgb(rawImage[0][row*width+col],rawImage[0][row*width+col],rawImage[0][row*width+col]));
                }
            }
        }else if(rawImage.size() == 3){
            image = QImage(width, height, QImage::Format_RGB888);
            for(int row = 0;row < height;++row){
                for(int col = 0;col < width;++col){
                    image.setPixel(col, row, (0xffu << 24) | ((rawImage[0][row*width+col] & 0xff) << 16) | ((rawImage[1][row*width+col] & 0xff) << 8) | (rawImage[2][row*width+col] & 0xff));
                }
            }
        }else return false;

        return true;
    }

    void cvtIntImage2Double(const std::vector< std::vector<unsigned int> > & inImage, std::vector< std::vector<double> > & outImage){
        outImage.resize(inImage.size());
        for(std::vector< std::vector<unsigned int> >::size_type index = 0;index < inImage.size();++index){
            outImage[index].resize(inImage[index].size());
            for(std::vector<unsigned int>::size_type imageIndex = 0;imageIndex < inImage[index].size();++imageIndex){
                outImage[index][imageIndex] = inImage[index][imageIndex] / 255.;
            }
        }
    }

    void cvtDblImage2Int(const std::vector< std::vector<double> > & inImage, std::vector< std::vector<unsigned int> > & outImage, double scale, unsigned int upBound = 255){
        outImage.resize(inImage.size());
        for(std::vector< std::vector<double> >::size_type index = 0;index < inImage.size();++index){
            outImage[index].resize(inImage[index].size());
            for(std::vector<double>::size_type imageIndex = 0;imageIndex < inImage[index].size();++imageIndex){
                unsigned int tempPixel = static_cast<unsigned int>(inImage[index][imageIndex] * scale);
                outImage[index][imageIndex] = std::min(tempPixel, upBound);
            }
        }
    }

    void cvtDblImage2Int(const std::vector<double> & inImage, std::vector<unsigned int> & outImage, double scale, unsigned int upBound = 255){
        outImage.resize(inImage.size());
        for(std::vector<double>::size_type imageIndex = 0;imageIndex < inImage.size();++imageIndex){
            unsigned int tempPixel = static_cast<unsigned int>(inImage[imageIndex] * scale);
            outImage[imageIndex] = std::min(tempPixel, upBound);
        }
    }

    void saveImage(std::string fileName, const std::vector< std::vector<unsigned int> > & inImage, int width, int height, const std::string format = "PNG"){
        QImage ret;
        cvtRawImg2QImg(inImage, width, height, ret);
        ret.save(fileName.c_str(), format.c_str());
    }

    bool getAndSaveInitialDepth(const std::string fileName, std::vector< std::vector<double> > image, const std::string format = "PNG"){
        double max_value = DBL_EPSILON;
        double depth_max_value = DBL_MIN;
        for(std::vector< std::vector<double> >::size_type index = 0;index < image.size();++index){
            for(std::vector<double>::size_type imageIndex = 0;imageIndex < image[index].size();++imageIndex){
                image[index][imageIndex] /= KRmrf::s_airLight[index];
                if(max_value < image[index][imageIndex]) max_value = image[index][imageIndex];
                //std::cout << image[index][imageIndex] << std::endl;
            }
        }

        for(std::vector< std::vector<double> >::size_type index = 0;index < image.size();++index){
            for(std::vector<double>::size_type imageIndex = 0;imageIndex < image[index].size();++imageIndex){
                image[index][imageIndex] /= max_value;
                image[index][imageIndex] = std::log(1 - image[index][imageIndex]);

                // use channal 0 to store the channale-wise maxvalue
                if(image[0][imageIndex] < image[index][imageIndex]) image[0][imageIndex] = image[index][imageIndex];
            }
        }
        for(std::vector<double>::size_type imageIndex = 0;imageIndex < image[0].size();++imageIndex){
            image[0][imageIndex] = -image[0][imageIndex];
            if(_finite(image[0][imageIndex]) && depth_max_value < image[0][imageIndex]) depth_max_value = image[0][imageIndex];
        }

        for(std::vector<double>::size_type imageIndex = 0;imageIndex < image[0].size();++imageIndex){
            if(!_finite(image[0][imageIndex])) image[0][imageIndex] = depth_max_value;
            if(_isnan(image[0][imageIndex])) image[0][imageIndex] = depth_max_value;
            if(image[0][imageIndex] < 1e-3 ) image[0][imageIndex] = 1e-3;
        }

        depth_max_value = std::max(depth_max_value, 1e-3);

        double max_pixel = DBL_MIN;
        double min_pixel = DBL_MAX;
        for(std::vector<double>::size_type imageIndex = 0;imageIndex < image[0].size();++imageIndex){
            image[0][imageIndex] /= depth_max_value;
            image[0][imageIndex] = 1. - image[0][imageIndex];
            if(max_pixel < image[0][imageIndex]) max_pixel = image[0][imageIndex];
            if(min_pixel > image[0][imageIndex]) min_pixel = image[0][imageIndex];
            //std::cout << image[0][imageIndex] << std::endl;
        }
        // scale the image to [0,255]
        std::vector<unsigned int> tempScaleImg;
        tempScaleImg.resize(image[0].size());
        for(std::vector<double>::size_type imageIndex = 0;imageIndex < image[0].size();++imageIndex){
            tempScaleImg[imageIndex] = static_cast<unsigned int>( (image[0][imageIndex]-min_pixel)*255/(max_pixel-min_pixel) );
        }

        //cvtDblImage2Int(image[0], tempScaleImg, 1.);

        //std::cout << fileName << std::endl;
        saveImage(fileName, std::vector< std::vector<unsigned int> >(1, tempScaleImg), m_qImage.width(), m_qImage.height(), format);

        return true;
    }
};

#endif // KDEFOG_H
