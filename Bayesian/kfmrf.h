#pragma once

#include <map>
#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <sstream>
#include <utility>
#include <numeric>
#include <algorithm>
#include <cctype>

//#define _GLIBCXX_USE_C99_FP_MACROS_DYNAMIC  1
#include <cmath>
#include <float.h>
#include <cfloat>

#include <QtGui/QImage>

#include "../../Library/LBFGSB/lbfgsb.h"
#include "mpfit.h"

#include "../KWorker.h"

#include "lmcurve.h"
#include "lmmin.h"
//#include "lmfit/lmcurve.h"


struct f_and_g{
    f_and_g(std::vector< std::vector<double> > & I_n,
            std::vector<double> &apow,
            std::vector<double> &ascale,
            std::string &partial_type);

    double f_and_g_fun(double *x, double *g);
    unsigned long int num_val;
    std::vector< std::vector<double> > &m_image;
    std::vector<double> &m_apow;
    std::vector<double> &m_ascale;
    std::string &m_partial_type;
};

class KRmrf{
	KWorker * callBackPtr;;
public:
    KRmrf(KWorker * ptr) : callBackPtr(ptr){
    }

    std::pair<std::vector<double>, std::vector<double> > fit_epd(const std::vector< std::vector<double> > &image){
        if(!(image.size() > 0)){
            std::cerr << "fit_epd: input image cann't be empty." << std::endl;
            return std::make_pair<std::vector<double>, std::vector<double> >(std::vector<double>(), std::vector<double>());
        }

        std::vector<double> channel_sum(image[0].size(), 0.);
        std::vector< std::vector<double> > chrom(image);
        std::vector< std::vector<double> > chrom_dx(image);
        std::vector< std::vector<double> > chrom_dy(image);

		// sum up all the channal's pixel to calculate chromaticity
        for(unsigned int  dim = 0;dim < image.size();++dim){
            for(unsigned int index = 0;index < image[dim].size();++index){
                channel_sum[index] += image[dim][index];
            }
        }
		// use chromaticity to normalize each pixel
        for(unsigned int  dim = 0;dim < image.size();++dim){
            for(unsigned int index = 0;index < image[dim].size();++index){
                chrom[dim][index] = image[dim][index] / channel_sum[index];
                if(!_finite(chrom[dim][index])) chrom[dim][index] = chrom[dim][index] > 0 ? DBL_MAX : -DBL_MAX;
                if(_isnan(chrom[dim][index])) chrom[dim][index] = 0.;
            }
        }
		// gradient along x dimension
        // Sobel - 1,0 
        for(unsigned int  dim = 0;dim < chrom.size();++dim){
            for(unsigned int height = 0;height < curImageHeight;++height){
                for(unsigned int width = 0;width < curImageWidth;++width){
                    if(width == 0 || width+1 == curImageWidth){
                        chrom_dx[dim][height*curImageWidth + width] = 0.;
                    }else{
                        double sum = 0.;
                        if(height > 0){
                            sum += (chrom[dim][(height-1)*curImageWidth + width + 1]-chrom[dim][(height-1)*curImageWidth + width - 1]);
                        }
                        if(height+1 < curImageHeight){
                            sum += (chrom[dim][(height+1)*curImageWidth + width + 1]-chrom[dim][(height+1)*curImageWidth + width - 1]);
                        }
                        sum += 2 * (chrom[dim][height*curImageWidth + width + 1]-chrom[dim][height*curImageWidth + width - 1]);
                        chrom_dx[dim][height*curImageWidth + width] = sum/8.;
                    }
                }
            }
        }
		// gradient along y dimension
        // Sobel - 0,1
        for(unsigned int  dim = 0;dim < chrom.size();++dim){
            for(unsigned int height = 0;height < curImageHeight;++height){
                for(unsigned int width = 0;width < curImageWidth;++width){
                    if(height == 0 || height+1 == curImageHeight){
                        chrom_dy[dim][height*curImageWidth + width] = 0.;
                    }else{
                        double sum = 0.;
                        if(width > 0){
                            sum += (chrom[dim][(height+1)*curImageWidth + width - 1] - chrom[dim][(height-1)*curImageWidth + width - 1]);
                        }
                        if(width+1 < curImageWidth){
                            sum += (chrom[dim][(height+1)*curImageWidth + width + 1] - chrom[dim][(height-1)*curImageWidth + width + 1]);
                        }
                        sum += 2 * (chrom[dim][(height+1)*curImageWidth + width] - chrom[dim][(height-1)*curImageWidth + width]);
                        chrom_dy[dim][height*curImageWidth + width] = sum/8.;
                    }
                }
            }
        }
		// flatting all the gradient (both dx and dy) into one vector
        std::vector< std::vector<double> > x;
        for(unsigned int index = 0;index < curImageWidth * curImageHeight;++index){
            std::vector<double> tempPixel;
            for(unsigned int  dim = 0;dim < chrom_dx.size();++dim){
                tempPixel.push_back(chrom_dx[dim][index]);
            }
            x.push_back(tempPixel);
        }
        for(unsigned int index = 0;index < curImageWidth * curImageHeight;++index){
            std::vector<double> tempPixel;
            for(unsigned int  dim = 0;dim < chrom_dy.size();++dim){
                tempPixel.push_back(chrom_dy[dim][index]);
            }
            x.push_back(tempPixel);
        }

        double min_bin = -0.25;
        double max_bin = 0.25;
        const unsigned int num_bins = 128;
        std::vector<double> vecBinSep(num_bins+1, 0);
		// calculate the histogram
        for(unsigned int index = 0;index < num_bins + 1;++index){
            vecBinSep[index] = min_bin + (max_bin-min_bin)*index/num_bins;
        }

        std::vector<double> xdata(vecBinSep);
        xdata.resize(num_bins);
        for(unsigned int index = 0;index < num_bins;++index){
            xdata[index] = xdata[index] + (vecBinSep[1] - vecBinSep[0])/2.;
        }

        std::vector<double> power(chrom.size(), 0.);
        std::vector<double> scale(chrom.size(), 0.);
		// every channal are fitted to get power and scale parameter individualy
        for(unsigned int dim = 0;dim < chrom.size();++dim){
            std::vector<double> vecBins(num_bins, 0.);
            for(unsigned int index = 0;index < 2 * curImageWidth * curImageHeight;++index){
                if(x[index][dim] < min_bin || x[index][dim] > max_bin) continue;
                int temp_bin_index = (x[index][dim] - min_bin)*num_bins/(max_bin-min_bin);
                //std::cout << temp_bin_index <<" ";
                int low_bin_index = std::max(0, temp_bin_index-2);
                int high_bin_index = std::min(static_cast<const int>(num_bins), temp_bin_index+3);
                for(int binIndex = low_bin_index;binIndex < high_bin_index;++binIndex){
                    if(x[index][dim] > vecBinSep[binIndex] && x[index][dim] < vecBinSep[binIndex+1]){
                        vecBins[binIndex] += 1.;
                        continue;
                    }
                }
            }
            double bin_sum = std::accumulate(vecBins.begin(), vecBins.end(), 0);
            for(unsigned int index = 0;index < num_bins;++index){
                vecBins[index] = vecBins[index]/bin_sum;
                //std::cout << vecBins[index] <<" ";
            }//std::cout <<"bin_sum"<<bin_sum<< std::endl;
            // curve fit
			// fit a exponential power distribution to get the parameter
            std::pair<double, double> ret_pair = curve_fit2(xdata, vecBins, 1./bin_sum);
            power[dim] = ret_pair.first;
            scale[dim] = ret_pair.second;
//            power[0] = 0.46456432;
//            power[1] = 0.65330337;
//            power[2] = 0.5307647;
//            scale[0] = 0.03809825;
//            scale[1] = 0.01489683;
//            scale[2] = 0.02754777;

            //std::cout << power[dim] << " " << scale[dim] << std::endl;
        }
		//<std::vector<double>, std::vector<double> >
        return std::make_pair(power, scale);
    }

	static double f( double t, const double *p )
	{
		return 1 * std::exp(-std::pow(std::abs(t), p[0]) / p[1]);
		//return p[0] + p[1]*(t-p[2])*(t-p[2]);
	}

	std::pair<double, double> curve_fit2(std::vector<double> xdata, std::vector<double> vecBins, double errBar){

        double * t = new double[xdata.size()];
        double * y = new double[vecBins.size()];

		for(unsigned int  index = 0;index < xdata.size();++index){
            t[index] = xdata[index];
            y[index] = vecBins[index] * 1;
        }
		/* parameter vector */

		int n_par = 2; // number of parameters in model function f
		double par[2] = { 0.25, 0.25 }; // relatively bad starting value

		/* data pairs: slightly distorted standard parabola */

		int m_dat = xdata.size(); // number of data pairs

		/* auxiliary parameters */
		lm_status_struct status;
		lm_control_struct control = lm_control_double;
		control.maxcall = 100;

		control.printflags = 0; // monitor status (+1) and parameters (+2)

		/* perform the fit */
		lmcurve_fit( n_par, par, m_dat, t, y, f, &control, &status );

		/* print results */

		//printf( "\nResults:\n" );
		//printf( "status after %d function evaluations:\n  %s\n",
		//		status.nfev, lm_infmsg[status.info] );

		//printf("obtained parameters:\n");
		//for ( i = 0; i < n_par; ++i)
		//printf("  par[%i] = %12g\n", i, par[i]);
		//printf("obtained norm:\n  %12g\n", status.fnorm );

		//printf("fitting data as follows:\n");
		//for ( i = 0; i < m_dat; ++i)
		//    printf( "  t[%2d]=%12g y=%12g fit=%12g residue=%12g\n",
		 //           i, t[i], y[i], f(t[i],par), y[i] - f(t[i],par) );



       
		//<double, double>
		//std::cout << par[0] << " " << par[1] << std::endl;
        return std::make_pair(par[0], par[1]);
	}

    struct vars_struct {
      double *x;
      double *y;
      double *ey;
    };
	// exponential power distribution fitting routine
    std::pair<double, double> curve_fit(std::vector<double> xdata, std::vector<double> vecBins, double errBar){
        double * x = new double[xdata.size()];
        double * y = new double[vecBins.size()];
        double * y_error = new double[vecBins.size()];

        for(unsigned int  index = 0;index < xdata.size();++index){
            x[index] = xdata[index];
            y[index] = vecBins[index];
            y_error[index] = errBar;
        }

        double param[2] = {0.25, 0.25};           /* Parameter initial conditions */
        double perror[2];                   /* Returned parameter errors */

        struct vars_struct v;

        mp_result result;

        memset(&result,0,sizeof(result));       /* Zero results structure */
        result.xerror = perror;

        v.x = x;
        v.y = y;
        v.ey = y_error;

        /* Call fitting function for 10 data points and 2 parameters */
        /*int status = */mpfit(epd_diff, xdata.size(), 2, param, 0, 0, (void *) &v, &result);

        delete [] x;
        delete [] y;
        delete [] y_error;
		//<double, double>
		std::cout << param[0] << param[1] << std::endl;
        return std::make_pair(param[0], param[1]);
    }
	// the exponential power distribution to be fitted
    static int epd_diff(int m, int n, double *p, double *dy, double **dvec, void *vars){
        int i;
        struct vars_struct *v = (struct vars_struct *) vars;
        double *x, *y, *ey;

        x = v->x;
        y = v->y;
        ey = v->ey;

        (void)n;
        (void)dvec;

        for (i=0; i<m; i++) {
            dy[i] = (y[i] - std::exp(-std::pow(std::abs(x[i]), p[0]) / p[1])) / ey[i];
        }

        return 0;
    }


//    std::pair<double, double> curve_fit2(std::vector<double> xdata, std::vector<double> vecBins){
//        double * x = new double[xdata.size()];
//        double * y = new double[vecBins.size()];
//
//        for(unsigned int  index = 0;index < xdata.size();++index){
//            x[index] = xdata[index];
//            y[index] = vecBins[index];
//        }
//
//
//        int n = 2; /* number of parameters in model function f */
//        double param[2] = { 0.25, 0.25 }; /* really bad starting value */
//
//        /* data points: a slightly distorted standard parabola */
//        int m = xdata.size();
//
//        lm_control_struct control = lm_control_double;
//        lm_status_struct status;
//        control.verbosity = 0;
//
//        //printf( "Fitting ...\n" );
//        lmcurve( n, param, m, x, y, epd_diff2, &control, &status );
//
////                printf( "Results:\n" );
////                printf( "status after %d function evaluations:\n  %s\n",
////                        status.nfev, lm_infmsg[status.outcome] );
//
////                printf("obtained parameters:\n");
////                for ( i = 0; i < n; ++i)
////                    printf("  par[%i] = %12g\n", i, param[i]);
////                printf("obtained norm:\n  %12g\n", status.fnorm );
//
////                printf("fitting data as follows:\n");
////                for ( i = 0; i < m; ++i)
////                    printf( "  t[%2d]=%4g y=%6g fit=%10g residue=%12g\n",
////                            i, t[i], y[i], f(t[i],par), y[i] - f(t[i],par) );
//
//        delete [] x;
//        delete [] y;
//
//        return std::make_pair<double, double>(param[0], param[1]);
//    }
//
//    static double epd_diff2( double t, const double *p ){
//        return std::exp(-std::pow(std::abs(t), p[0]) / p[1]);
//    }

    std::pair<std::vector< std::vector<double> >, std::vector<double> >
            factorize(const std::vector< std::vector<double> > &imageSrc,
                      std::vector< std::vector<double> > initial_albedo = std::vector< std::vector<double> >(),
                      std::vector<double> initial_depth = std::vector<double>(),
                      unsigned int n_outer_iterations = 3,
                      unsigned int n_inner_iterations = 10,
                      bool verbose = false){
		std::cout << "I: factorize..." << std::endl;
		if(callBackPtr) if(!(callBackPtr->testRunning())) return std::pair<std::vector< std::vector<double> >, std::vector<double> >();
		// the fitted parameter of scale and power prior
        std::pair<std::vector<double>, std::vector<double> > /* apow, ascale */ pair_pow_scale = fit_epd(imageSrc);
        std::vector< std::vector<double> > image(imageSrc);
		// I tilde(wave line) in the paper
        double max_value = DBL_EPSILON;
        for(std::vector< std::vector<double> >::size_type index = 0;index < image.size();++index){
            for(std::vector<double>::size_type imageIndex = 0;imageIndex < image[index].size();++imageIndex){
                image[index][imageIndex] /= KRmrf::s_airLight[index];
                if(max_value < image[index][imageIndex]) max_value = image[index][imageIndex];
            }
        }
		// scale into [0-1] to prepare for the log transform ( log(1-I~) )
        for(std::vector< std::vector<double> >::size_type index = 0;index < image.size();++index){
            for(std::vector<double>::size_type imageIndex = 0;imageIndex < image[index].size();++imageIndex){
                image[index][imageIndex] /= max_value;
            }
        }
		// use the airLight as the albedo start point, in fact random is ok for we will update this imediately
        if(initial_albedo.empty()){
            for(std::vector< std::vector<double> >::size_type index = 0;index < image.size();++index){
                initial_albedo.push_back(std::vector<double>(image[index].size(), KRmrf::s_airLight[index]));
            }
        }
		// if there are no depth prior, we will calculate one to start
        if(initial_depth.empty()){
            initial_depth = compute_initial_depth(image);
        }
		if(callBackPtr) if(!(callBackPtr->testRunning())) return std::pair<std::vector< std::vector<double> >, std::vector<double> >();

        std::pair<std::vector< std::vector<double> >, std::vector<double> > pair_A_D
			//<std::vector< std::vector<double> >, std::vector<double> >
                = std::make_pair(initial_albedo, initial_depth);

        for(unsigned int index = 0;index < n_outer_iterations;++index){
			std::cout << "I: optimize iteration " << index+1 << "." << std::endl;
			// update A to maximum the posterior
            pair_A_D = optimize(image, pair_pow_scale.first, pair_pow_scale.second, pair_A_D.first, pair_A_D.second, "A", n_inner_iterations, verbose);
			std::cout << "I: optimize albedo " << "finished." << std::endl;
			if(callBackPtr) if(!(callBackPtr->testRunning())) return std::pair<std::vector< std::vector<double> >, std::vector<double> >();
			// update D to maximum the posterior
            pair_A_D = optimize(image, pair_pow_scale.first, pair_pow_scale.second, pair_A_D.first, pair_A_D.second, "D", n_inner_iterations, verbose);
			std::cout << "I: optimize depth " << "finished." << std::endl;
			if(callBackPtr) if(!(callBackPtr->testRunning())) return std::pair<std::vector< std::vector<double> >, std::vector<double> >();
        }

        return pair_A_D;
    }


    std::pair<std::vector< std::vector<double> >, std::vector<double> >
            factorize_multiscale(const std::vector< std::vector<double> > &imageSrc,
                      std::vector<double> scales,
                      unsigned int n_outer_iterations = 3,
                      unsigned int n_inner_iterations = 10,
                      bool verbose = false){

		// we first estimate a rough depth use a low scale, and use this for accurate estimation in real scale
        std::sort(scales.begin(), scales.end());
        std::vector<double> prev_d;

        std::pair<std::vector< std::vector<double> >, std::vector<double> > pair_A_D;

        unsigned int prev_depth_width = KRmrf::curImageWidth;
        unsigned int prev_depth_height = KRmrf::curImageHeight;

        unsigned int new_width = KRmrf::curImageWidth;
        unsigned int new_height = KRmrf::curImageHeight;

        for(unsigned int index = 0;index < scales.size();++index){

            new_width = KRmrf::orgImageWidth * scales[index];
            new_height = KRmrf::orgImageHeight * scales[index];
			// resize the image to get the exact scale
            std::vector< std::vector<double> > image = imresize_float(imageSrc, KRmrf::orgImageWidth, KRmrf::orgImageHeight, new_width, new_height);

//            std::cout << new_width << new_height << std::endl;
//            for(unsigned int ii=0;ii<10;ii++){
//                std::cout <<" "<< image[0][ii] <<" "<< image[1][ii] <<" "<< image[2][ii] << std::endl;
//            }
//            std::cout<<"frff"<<std::endl;
			
            if(!(prev_d.empty())){
                prev_d = imresize_float(prev_d, prev_depth_width, prev_depth_height, new_width, new_height);
            }

            KRmrf::curImageWidth = new_width;
            KRmrf::curImageHeight = new_height;
			// estimate the albedo and depth
			std::cout << "I: factorize scale " << scales[index] << "." << std::endl;
            pair_A_D = factorize(image, std::vector< std::vector<double> >(), prev_d, n_outer_iterations, n_inner_iterations, verbose);

			if(callBackPtr) if(!(callBackPtr->testRunning())) return std::pair<std::vector< std::vector<double> >, std::vector<double> >();

            prev_d = pair_A_D.second;

            prev_depth_width = new_width;
            prev_depth_height = new_height;
        }


        return pair_A_D;
    }

    std::vector< std::vector<double> > imresize_float(const std::vector< std::vector<double> > &imageSrc, unsigned int inWidth, unsigned int inHeight, unsigned int outWidth, unsigned int outHeight){
        double max_pixel = DBL_MIN;
        double min_pixel = DBL_MAX;

        std::vector< std::vector<double> > image(imageSrc);

        for(std::vector< std::vector<double> >::size_type index = 0;index < image.size();++index){
            for(std::vector<double>::size_type imageIndex = 0;imageIndex < image[index].size();++imageIndex){
                if(max_pixel < image[index][imageIndex]) max_pixel = image[index][imageIndex];
                if(min_pixel > image[index][imageIndex]) min_pixel = image[index][imageIndex];
            }
        }
        // scale the image to [0,255]
        std::vector< std::vector<unsigned int> > scaled_image(imageSrc.size(), std::vector<unsigned int>());
        for(std::vector< std::vector<double> >::size_type index = 0;index < image.size();++index){
            scaled_image[index].resize(image[index].size());
            for(std::vector<double>::size_type imageIndex = 0;imageIndex < image[index].size();++imageIndex){
                scaled_image[index][imageIndex] = static_cast<unsigned int>( (image[index][imageIndex]-min_pixel)*255/(max_pixel-min_pixel) );
            }
        }
        // convert all pixels to QImage
        QImage qImage;
        if(scaled_image.size() == 1){
            qImage = QImage(inWidth, inHeight, QImage::Format_RGB32);
            for(unsigned int row = 0;row < inHeight;++row){
                for(unsigned int col = 0;col < inWidth;++col){
                    qImage.setPixel(col, row, qRgb(scaled_image[0][row*inWidth+col],scaled_image[0][row*inWidth+col],scaled_image[0][row*inWidth+col]));
                }
            }
        }else if(scaled_image.size() == 3){
            qImage = QImage(inWidth, inHeight, QImage::Format_RGB888);
            for(unsigned int row = 0;row < inHeight;++row){
                for(unsigned int col = 0;col < inWidth;++col){
                    qImage.setPixel(col, row, (0xffu << 24) | ((scaled_image[0][row*inWidth+col] & 0xff) << 16) | ((scaled_image[1][row*inWidth+col] & 0xff) << 8) | (scaled_image[2][row*inWidth+col] & 0xff));
                }
            }
        }
        // scale size of image
        QImage qScaledImage = qImage.scaled(outWidth, outHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        // convert all pixels back to vector
        image.clear();
        image.reserve(scaled_image.size());
        std::vector<double> tempVec;
        for(int index = scaled_image.size() - 1;index >= 0;--index){
            tempVec.resize(0);
            tempVec.reserve(static_cast<unsigned int>(qScaledImage.width()*qScaledImage.height()));
            for(int row = 0;row < qScaledImage.height();++row){
                for(int col = 0;col < qScaledImage.width();++col){
                    tempVec.push_back( (max_pixel-min_pixel) * ((static_cast<unsigned int>(qScaledImage.pixel(col, row)) >> (8*index)) & 0xff) / 255. + min_pixel );
                }
            }
            image.push_back(tempVec);
        }

        return image;
    }

    std::vector<double> imresize_float(const std::vector<double> &imageSrc, unsigned int inWidth, unsigned int inHeight, unsigned int outWidth, unsigned int outHeight){
        std::vector< std::vector<double> > image(1, imageSrc);
        return *(imresize_float(image, inWidth, inHeight, outWidth, outHeight).begin());
    }

    std::pair<std::vector< std::vector<double> >, std::vector<double> >
            optimize(std::vector< std::vector<double> > I_n, std::vector<double> apow, std::vector<double> ascale,
                    std::vector< std::vector<double> > initial_albedo, std::vector<double> initial_depth,
                    std::string partial_type, unsigned int iters, bool verbose){

        if(partial_type != "A" && partial_type != "D"){
            std::cerr << "Invalid partial derivative type " << partial_type << std::endl;
            return std::make_pair<std::vector< std::vector<double> >, std::vector<double> >(std::vector< std::vector<double> >(), std::vector<double>());
        }

        unsigned int num_pixels = initial_depth.size();
		
		// flattening albedo and depth into one vector to prepare for optimize
        std::vector<double> x(num_pixels*initial_albedo.size() + initial_depth.size(), 0.);
        std::vector<double>::iterator it = x.begin();
        std::vector<std::pair<double, double> > boundVec(x.size());
        std::vector<std::pair<double, double> >::iterator itBound = boundVec.begin();
		// albedo bound during optimization
        for(std::vector< std::vector<double> >::size_type index = 0;index < initial_albedo.size();++index){
            for(std::vector<double>::size_type imageIndex = 0;imageIndex < initial_albedo[index].size();++imageIndex){
                *it++ = initial_albedo[index][imageIndex];
                itBound->first = 1e-3;
                itBound->second = 1.;
                itBound++;
            }
        }
		// depth bound during optimization
        for(std::vector<double>::size_type imageIndex = 0;imageIndex < initial_depth.size();++imageIndex){
            *it++ = initial_depth[imageIndex];
            itBound->first = 1e-3;
            itBound->second = -1.;
            itBound++;
        }
		
        x = lbfgsb(f_and_g(I_n, apow, ascale, partial_type), x, boundVec, 16, iters, 15000, verbose);

		// reshape the flatten vector to real shape
        std::vector< std::vector<double> > final_albedo(initial_albedo);
        std::vector<double> final_depth(initial_depth);
        it = x.begin();
        for(std::vector< std::vector<double> >::size_type index = 0;index < final_albedo.size();++index){
            for(std::vector<double>::size_type imageIndex = 0;imageIndex < final_albedo[index].size();++imageIndex){
				//std::cout << *it << " ";
                 final_albedo[index][imageIndex] = *it++;
				 //if(imageIndex>100) system("pause");
            }
        }
        for(std::vector<double>::size_type imageIndex = 0;imageIndex < final_depth.size();++imageIndex){
            final_depth[imageIndex] = *it++;
        }
		
		//<std::vector< std::vector<double> >, std::vector<double> >
        return std::make_pair(final_albedo, final_depth);
    }


    std::vector<double> compute_initial_depth(const std::vector< std::vector<double> > &imageSrc){
        double depth_max_value = DBL_MIN;
		// I~
        std::vector< std::vector<double> > image(imageSrc);
		// use the max value along the channal of I~ to use as depth starting poiint
        for(std::vector< std::vector<double> >::size_type index = 0;index < image.size();++index){
            for(std::vector<double>::size_type imageIndex = 0;imageIndex < image[index].size();++imageIndex){
                image[index][imageIndex] = std::log(1 - image[index][imageIndex]);

                // use channal 0 to store the channale-wise maxvalue
                if(image[0][imageIndex] < image[index][imageIndex]) image[0][imageIndex] = image[index][imageIndex];
            }
        }
		// check for numerical valid
        for(std::vector<double>::size_type imageIndex = 0;imageIndex < image[0].size();++imageIndex){
            image[0][imageIndex] = -image[0][imageIndex];
            if(_finite(image[0][imageIndex]) && depth_max_value < image[0][imageIndex]) depth_max_value = image[0][imageIndex];
        }
		// clip the invalid numerical value
        for(std::vector<double>::size_type imageIndex = 0;imageIndex < image[0].size();++imageIndex){
            if(!_finite(image[0][imageIndex])) image[0][imageIndex] = depth_max_value;
            if(_isnan(image[0][imageIndex])) image[0][imageIndex] = depth_max_value;
            if(image[0][imageIndex] < 1e-3 ) image[0][imageIndex] = 1e-3;
        }

        return image[0];
    }



    std::vector<double> lbfgsb(f_and_g fun, std::vector<double> x0, std::vector<std::pair<double, double> > boundVec, int maxcor, long int max_iter, unsigned long int max_eval, bool bVerbose)
    {
        integer n = x0.size();
        integer m = maxcor;

        /* Local variables */
        double f = 0.;
        double *g = new double[n];
        double *x = new double[n];

        double *l = new double[n];
        double *u = new double[n];

        integer *nbd = new integer[n];

        for(integer index = 0; index < n;++index){
            l[index] = boundVec[index].first;
            if(boundVec[index].second < 0){ nbd[index] = 1; continue; }
            u[index] = boundVec[index].second;
            nbd[index] = 2;
        }

        double *wa = new double[2*m*n + 5*n + 11*m*m + 8*m];
        integer *iwa = new integer[3 * n];
        /*  char task[60]; */
        integer taskValue;
        integer *task = &taskValue; /* must initialize !! */
        /*  http://stackoverflow.com/a/11278093/269192 */
        double factr = 1e7;
        /*  char csave[60]; */
        integer csaveValue;
        integer *csave = &csaveValue;
        double dsave[29];
        integer isave[44];
        logical lsave[4];
        double pgtol = 1e-8;
        integer iprint = -1;

        if(bVerbose) iprint = 1;

        /* copy source x from vector to local array */
        for(long index = 0;index < n;++index){
            x[index] = x0[index];
        }

        fun.num_val = 0;
        /*     We start the iteration by initializing task. */

        *task = (integer)START;
        /*     s_copy(task, "START", (ftnlen)60, (ftnlen)5); */
        /*        ------- the beginning of the loop ---------- */
    L111:
        /*     This is the call to the L-BFGS-B code. */
        setulb(&n, &m, x, l, u, nbd, &f, g, &factr, &pgtol, wa, iwa, task, &
                iprint, csave, lsave, isave, dsave);
        /*     if (s_cmp(task, "FG", (ftnlen)2, (ftnlen)2) == 0) { */
        if ( IS_FG(*task) ) {
            /*        the minimization routine has returned to request the */
            /*        function f and gradient g values at the current x. */
            /*        Compute function value f for the sample problem. */
            f = fun.f_and_g_fun(x, g);
            /*          go back to the minimization routine. */
            goto L111;
        }

        /*     if (s_cmp(task, "NEW_X", (ftnlen)5, (ftnlen)5) == 0) { */
        if ( *task==NEW_X ) {
            /*        the minimization routine has returned with a new iterate. */
            /*        The time limit has not been reached, and we test whether */
            /*        the following two stopping tests are satisfied: */
            /*        1) Terminate if the total number of f and g evaluations */
            /*             exceeds 900. */
            if (isave[33] >= max_iter || fun.num_val > max_eval)
            //{
                *task = STOP_ITER;
            /*             s_copy(task, "STOP: TOTAL NO. of f AND g EVALUATIONS EXCEEDS LIM" */
            /*                     "IT", (ftnlen)60, (ftnlen)52); */
            //}
            /*        2) Terminate if  |proj g|/(1+|f|) < 1.0d-10. */
            //if (dsave[12] <= (fabs(f) + 1.) * 1e-10)
            //{
            //    *task = STOP_GRAD;
            /*             s_copy(task, "STOP: THE PROJECTED GRADIENT IS SUFFICIENTLY SMALL", */
            /*                     (ftnlen)60, (ftnlen)50); */
            //}
            /*        We wish to print the following information at each iteration: */
            /*          1) the current iteration number, isave(30), */
            /*          2) the total number of f and g evaluations, isave(34), */
            /*          3) the value of the objective function f, */
            /*          4) the norm of the projected gradient,  dsve(13) */

            /*        See the comments at the end of driver1 for a description */
            /*        of the variables isave and dsave. */
            //printf("Iterate %ld, nfg = %ld, f = %.2e, |proj g| = %.2e\n", isave[29], isave[33], f, dsave[12] );
            /*        If the run is to be terminated, we print also the information */
            /*        contained in task as well as the final value of x. */
            /*         if (s_cmp(task, "STOP", (ftnlen)4, (ftnlen)4) == 0) { */
            if (IS_STOP(*task)) {
                //printf("Final X = ");
                /* copy source x from vector to local array */
                for(long index = 0;index < n;++index){
                    x0[index] = x[index];
                }
            }
            goto L111;
        }
        /*        the minimization routine has returned with a new iterate, */
        /*         and we have opted to continue the iteration. */
        /*           ---------- the end of the loop ------------- */
        /*     If task is neither FG nor NEW_X we terminate execution. */
        //s_stop("", (ftnlen)0);


        delete [] g;
        delete [] l;
        delete [] u;
        delete [] x;
        delete [] wa;
        delete [] nbd;
        delete [] iwa;

        return x0;
    }

public:
    typedef std::map<std::string, std::string>::iterator mapIter;

    // trim from start (in place)
    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                std::not1(std::ptr_fun<int, int>(std::isspace))));
    }

    // trim from end (in place)
    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    }

    // trim from both ends (in place)
    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    struct KIntArgsInterator{
        typedef enum { SuccNext, NoNext } IterStatus;

        KIntArgsInterator(std::string arg, std::string sep=" ,\t"): m_argString(arg), m_argSep(sep){
            trim(m_argString);
        }
        template <typename T>
        IterStatus next(T & out){
            std::string tempStr="";
            if(m_argString.find_first_not_of(m_argSep) != m_argString.npos){
                m_argString = m_argString.substr(m_argString.find_first_not_of(m_argSep));
            }else return NoNext;

            if(m_argString.find_first_of(m_argSep) != m_argString.npos){
                tempStr = m_argString.substr(0, m_argString.find_first_of(m_argSep));
                m_argString = m_argString.substr(m_argString.find_first_of(m_argSep)+1);
                //ltrim(m_argString);
            }else{
                if(m_argString.length()>0) tempStr = m_argString;
                else return NoNext;
                m_argString = "";
            }
            std::istringstream is(tempStr);
            is >> out;
            return SuccNext;
        }

    private:
        std::string m_argString;
        std::string m_argSep;
    };

    static std::string s_imagePath;
    static std::string s_albedoOutPath;
    static std::string s_depthOutPath;
    static unsigned int s_numOuterIter;
    static unsigned int s_numInnerIter;
    static std::vector<double> s_airLight;
    static bool s_bAirLightUseRect;
    static double s_priorCWeight;
    static double s_priorDWeight;
    static std::string s_priorType;
    static bool s_bMultiScale;
    static bool s_bStoreInitial;
    static std::map<std::string, std::string> s_paramMap;

    static bool parseArguments();


    // may be changed across different scale
    static unsigned int curImageWidth;
    static unsigned int curImageHeight;

    static unsigned int orgImageWidth;
    static unsigned int orgImageHeight;
};
