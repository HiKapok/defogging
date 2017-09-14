#ifndef KFUNUTILS_H
#define KFUNUTILS_H

#include "kfmrf.h"

#include <vector>

//#define _GLIBCXX_USE_C99_FP_MACROS_DYNAMIC  1
#include <cmath>

#include <iostream>

class KFuncLibs{
public:
    static double likelihood(const std::vector< std::vector<double> > &imageI,
                      const std::vector< std::vector<double> > &imageA,
                      const std::vector<double> &imageD){
        if(!(imageA.size() > 0 &&  imageI.size()>0)){
            std::cerr << "likelihood: input image can't be empty." << std::endl;
            return 0.;
        }
        if(imageA[0].size() != imageD.size()){
            std::cerr << "likelihood: input image and depth image don't have equal size." << std::endl;
            return 0.;
        }
        std::vector<double> expDepth(imageD.size());
        std::vector< std::vector<double> > imageIEst(imageA);
        double diff_sum = 0.;
        for(unsigned int index = 0;index < imageD.size();++index){
			// exp(-D)
            expDepth[index] = std::exp(-imageD[index]);
            for(unsigned int dim = 0;dim < imageIEst.size();++dim){
                // albedo * exp(-D) + (1 - exp(-D)) to get estimate defogging image
                imageIEst[dim][index] = imageIEst[dim][index] * expDepth[index] + (1. - expDepth[index]);
                // use gaussian formulation (no normalization factor) to get likehood (use exp[(I-A-D)^2])
                diff_sum += std::pow(imageIEst[dim][index] - imageI[dim][index], 2.);
            }
        }

        return diff_sum;
    }

    static std::vector< std::vector<double> > likelihood_grad_A(const std::vector< std::vector<double> > &imageI,
                      const std::vector< std::vector<double> > &imageA,
                      const std::vector<double> &imageD){
        if(!(imageA.size() > 0 &&  imageI.size()>0)){
            std::cerr << "likelihood_grad_A: input image can't be empty." << std::endl;
            return std::vector< std::vector<double> >();
        }
        if(imageA[0].size() != imageD.size()){
            std::cerr << "likelihood_grad_A: input image and depth image don't have equal size." << std::endl;
            return std::vector< std::vector<double> >();
        }
        std::vector<double> expDepth(imageD.size());
        std::vector< std::vector<double> > imageIEst(imageA);
        for(unsigned int index = 0;index < imageD.size();++index){
            // exp(-D)
            expDepth[index] = std::exp(-imageD[index]);
            for(unsigned int dim = 0;dim < imageIEst.size();++dim){
                // albedo * exp(-D) + (1 - exp(-D)) to get estimate defogging image
                imageIEst[dim][index] = imageIEst[dim][index] * expDepth[index] + (1. - expDepth[index]);
                // the gaussian function's gradient about albedo
                imageIEst[dim][index] = 2. * expDepth[index] * (imageIEst[dim][index] - imageI[dim][index]);
            }
        }

        return imageIEst;
    }

    static std::vector<double> likelihood_grad_D(
                      const std::vector< std::vector<double> > &imageI,
                      const std::vector< std::vector<double> > &imageA,
                      const std::vector<double> &imageD){
        if(!(imageA.size() > 0 &&  imageI.size()>0)){
            std::cerr << "likelihood_grad_D: input image can't be empty." << std::endl;
            return std::vector<double>();
        }
        if(imageA[0].size() != imageD.size()){
            std::cerr << "likelihood_grad_D: input image and depth image don't have equal size." << std::endl;
            return std::vector<double>();
        }
        std::vector<double> expDepth(imageD.size());
        std::vector<double> vecGradD(imageD.size());
        std::vector< std::vector<double> > imageIEst(imageA);
        for(unsigned int index = 0;index < imageD.size();++index){
            // exp(-D)
            expDepth[index] = std::exp(-imageD[index]);

            double tempSum = 0.;
            for(unsigned int dim = 0;dim < imageIEst.size();++dim){
                // albedo * exp(-D) + (1 - exp(-D)) to get estimate defogging image
                imageIEst[dim][index] = imageIEst[dim][index] * expDepth[index] + (1. - expDepth[index]);
                // the gaussian function's gradient about depth
                tempSum += -2. * (imageA[dim][index] - 1.) * expDepth[index] * (imageIEst[dim][index] - imageI[dim][index]);
            }
            vecGradD[index] = tempSum;

        }

        return vecGradD;
    }

    static double albedo_prior(
                      const std::vector< std::vector<double> > &imageA,
                      const std::vector<double> &power,
                      const std::vector<double> &scale){
        if(!(imageA.size() > 0)){
            std::cerr << "albedo_prior: input image can't be empty." << std::endl;
            return 0.;
        }

        std::vector<double> sumColorA(imageA[0].size(), 0.);
        std::vector<std::vector<double> > chromaticity(imageA);

        for(unsigned int index = 0;index < sumColorA.size();++index){
            // sum up pixel along channals to get chromaticity
            for(unsigned int dim = 0;dim < imageA.size();++dim){
                sumColorA[index] += imageA[dim][index];
            }
            // chromaticity normalization
            if(sumColorA[index] > 1e-8){
                for(unsigned int dim = 0;dim < chromaticity.size();++dim){
                    chromaticity[dim][index] = imageA[dim][index] / sumColorA[index];
                }
            }else{
                for(unsigned int dim = 0;dim < chromaticity.size();++dim){
                    chromaticity[dim][index] = 0.;
                }
            }
        }

        std::vector<std::vector<std::vector<double> > > gradVec = grid_diff(chromaticity);

        std::vector<std::vector<double> > vecSingleSum(gradVec.size(), std::vector<double>(chromaticity.size(), 0.));
        // use four direction's gradient (feed gradient into the exponential distribution function) to get albedo prior
        for(unsigned int diffIndex = 0; diffIndex < gradVec.size();++diffIndex){
            for(unsigned int dim = 0;dim < gradVec[diffIndex].size();++dim){
                for(unsigned int index = 0;index < gradVec[diffIndex][dim].size();++index){
                    vecSingleSum[diffIndex][dim] += std::pow(std::abs(gradVec[diffIndex][dim][index]), power[dim])/scale[dim];
                }
            }
        }
		// sum up four direction
        double diffSum = 0.;
        for(unsigned int diffIndex = 0; diffIndex < vecSingleSum.size();++diffIndex){
            for(unsigned int dim = 0;dim < vecSingleSum[diffIndex].size();++dim){
                diffSum += vecSingleSum[diffIndex][dim];
            }
        }
        return diffSum;
    }

    static std::vector<std::vector<double> > albedo_prior_grad(
                      const std::vector< std::vector<double> > &imageA,
                      const std::vector<double> &power,
                      const std::vector<double> &scale){
        if(!(imageA.size() > 0)){
            std::cerr << "albedo_prior_grad: input image can't be empty." << std::endl;
            return std::vector<std::vector<double> >();
        }

        std::vector<double> sumColorA(imageA[0].size(), 0.);
        std::vector<std::vector<double> > chromaticity(imageA);

        for(unsigned int index = 0;index < sumColorA.size();++index){
            // sum up pixel along channals to get chromaticity
            for(unsigned int dim = 0;dim < imageA.size();++dim){
                sumColorA[index] += imageA[dim][index];
            }
            // chromaticity normalization
            if(sumColorA[index] > 1e-8){
                for(unsigned int dim = 0;dim < chromaticity.size();++dim){
                    chromaticity[dim][index] = imageA[dim][index] / sumColorA[index];
                }
            }else{
                for(unsigned int dim = 0;dim < chromaticity.size();++dim){
                    chromaticity[dim][index] = 0.;
                }
            }
        }

        std::vector<std::vector<std::vector<double> > > gradVec = grid_diff(chromaticity);


		// use four direction's gradient (feed gradient into the exponential distribution function's derivative) to get albedo prior's gradient
        for(unsigned int diffIndex = 0; diffIndex < gradVec.size();++diffIndex){
            for(unsigned int dim = 0;dim < gradVec[diffIndex].size();++dim){
                for(unsigned int index = 0;index < gradVec[diffIndex][dim].size();++index){
                    if(std::abs(gradVec[diffIndex][dim][index]) > 1e-8){
                        double sign = 0.;
                        if(gradVec[diffIndex][dim][index]<-1e-8){
                            sign = -1.;
                        }else if(gradVec[diffIndex][dim][index]>1e-8){
                            sign = 1.;
                        }
                        gradVec[diffIndex][dim][index] = sign * power[dim] * std::pow(std::abs(gradVec[diffIndex][dim][index]), power[dim]-1.)/scale[dim];
                    }else{
                        gradVec[diffIndex][dim][index] = 0.;
                    }
                }
            }
        }

        std::vector<std::vector<double> > grid_grad = make_grid_grad(gradVec, imageA);

        return grid_grad;
    }


    static double depth_prior(const std::vector<double> & depth){
        std::vector<std::vector<double> > gradVec = grid_diff(depth);
        double sum = 0.;
		// sum up the exponential distribution function along four direction's gradient to get depth prior
        if(KRmrf::s_priorType == "gaussian"){
            for(unsigned int diffIndex = 0;diffIndex < gradVec.size();++diffIndex){
                for(unsigned int index = 0;index < gradVec[diffIndex].size();++index){
                    sum += std::pow(gradVec[diffIndex][index], 2.);
                }
            }
        }else{
            for(unsigned int diffIndex = 0;diffIndex < gradVec.size();++diffIndex){
                for(unsigned int index = 0;index < gradVec[diffIndex].size();++index){
                    sum += std::abs(gradVec[diffIndex][index]);
                }
            }
        }

        return sum;
    }

    static std::vector<double> depth_prior_grad(const std::vector<double> & depth){
        std::vector<std::vector<double> > gradVec = grid_diff(depth);
		// sum up the exponential distribution function's derivative along four direction's gradient to get depth prior
        if(KRmrf::s_priorType == "gaussian"){
            for(unsigned int diffIndex = 0;diffIndex < gradVec.size();++diffIndex){
                for(unsigned int index = 0;index < gradVec[diffIndex].size();++index){
                    gradVec[diffIndex][index] = 2. * gradVec[diffIndex][index];
                }
            }
        }else{
            for(unsigned int diffIndex = 0;diffIndex < gradVec.size();++diffIndex){
                for(unsigned int index = 0;index < gradVec[diffIndex].size();++index){
                    if(gradVec[diffIndex][index]<-1e-8){
                        gradVec[diffIndex][index] = -1.;
                    }else if(gradVec[diffIndex][index]>1e-8){
                        gradVec[diffIndex][index] = 1.;
                    }else gradVec[diffIndex][index] = 0.;
                }
            }
        }

        return make_grid_grad(gradVec, depth);
    }


private:
    // def grid_diff(grid):
    // top = grid[:-1] - grid[1:]
    // right = grid[:, :-1] - grid[:, 1:]
    // bot = grid[1:] - grid[:-1]
    // left = grid[:, 1:] - grid[:, :-1]
    // return (top, right, bot, left)

    static std::vector<std::vector<std::vector<double> > > grid_diff(const std::vector<std::vector<double> > & grid){
//        KRmrf::curImageWidth
        std::vector<std::vector<std::vector<double> > > gradVec;
        std::vector<std::vector<double> > top(grid.size(), std::vector<double>(KRmrf::curImageWidth * (KRmrf::curImageHeight-1), 0.));
        std::vector<std::vector<double> > bottom(grid.size(), std::vector<double>(KRmrf::curImageWidth * (KRmrf::curImageHeight-1), 0.));
        std::vector<std::vector<double> > left(grid.size(), std::vector<double>((KRmrf::curImageWidth-1) * KRmrf::curImageHeight, 0.));
        std::vector<std::vector<double> > right(grid.size(), std::vector<double>((KRmrf::curImageWidth-1) * KRmrf::curImageHeight, 0.));
        for(unsigned int dim = 0; dim < grid.size();++dim){
            // top diff
            for(unsigned int  height = 0;height < KRmrf::curImageHeight-1;++height){
                for(unsigned int  width = 0;width < KRmrf::curImageWidth;++width){
                    top[dim][height*KRmrf::curImageWidth + width] = (grid[dim][height*KRmrf::curImageWidth + width] -
                               grid[dim][(height+1)*KRmrf::curImageWidth + width]);
                }
            }
            // bottom diff
            for(unsigned int  height = 1;height < KRmrf::curImageHeight;++height){
                for(unsigned int  width = 0;width < KRmrf::curImageWidth;++width){
                    bottom[dim][(height-1)*KRmrf::curImageWidth + width] = (grid[dim][height*KRmrf::curImageWidth + width] - grid[dim][(height-1)*KRmrf::curImageWidth + width]);
                }
            }
            // left diff
            for(unsigned int  height = 0;height < KRmrf::curImageHeight;++height){
                for(unsigned int  width = 1;width < KRmrf::curImageWidth;++width){
                    left[dim][height*(KRmrf::curImageWidth-1) + width -1] = (grid[dim][height*KRmrf::curImageWidth + width] - grid[dim][height*KRmrf::curImageWidth + width - 1]);
                }
            }
            // right diff
            for(unsigned int  height = 0;height < KRmrf::curImageHeight;++height){
                for(unsigned int  width = 0;width < KRmrf::curImageWidth - 1;++width){
                    right[dim][height*(KRmrf::curImageWidth-1) + width] = (grid[dim][height*KRmrf::curImageWidth + width] - grid[dim][height*KRmrf::curImageWidth + width + 1]);
                }
            }
        }

        gradVec.push_back(top);
        gradVec.push_back(right);
        gradVec.push_back(bottom);
        gradVec.push_back(left);

        return gradVec;
    }

    static std::vector<std::vector<double> > grid_diff(const std::vector<double> & grid){
        std::vector<std::vector<double> > vecGrid(1, grid);
        std::vector<std::vector<std::vector<double> > > tempGrad = grid_diff(vecGrid);
        std::vector<std::vector<double> > gradVec;

        gradVec.push_back(tempGrad[0][0]);
        gradVec.push_back(tempGrad[1][0]);
        gradVec.push_back(tempGrad[2][0]);
        gradVec.push_back(tempGrad[3][0]);

        return gradVec;
    }
	// sum up the four direction's gradient to get each pixel's gradient
    static std::vector<std::vector<double> > make_grid_grad(std::vector<std::vector<std::vector<double> > > &grid, const std::vector<std::vector<double> > & image){
        std::vector<std::vector<double> > grad_output(image.size(), std::vector<double>(image[0].size(), 0.));

        for(unsigned int dim = 0; dim < image.size();++dim){
            // top diff
            std::vector<std::vector<double> > &grad_image = grid[0];
            for(unsigned int  height = 0;height < KRmrf::curImageHeight-1;++height){
                for(unsigned int  width = 0;width < KRmrf::curImageWidth;++width){
                    grad_output[dim][height*KRmrf::curImageWidth + width] = grad_image[dim][height*KRmrf::curImageWidth + width];
                }
            }
            // right diff
            grad_image = grid[1];
            for(unsigned int  height = 0;height < KRmrf::curImageHeight;++height){
                for(unsigned int  width = 0;width < KRmrf::curImageWidth - 1;++width){
                    grad_output[dim][height*KRmrf::curImageWidth + width] += grad_image[dim][height*(KRmrf::curImageWidth-1) + width];
                }
            }
            // bottom diff
            grad_image = grid[2];
            for(unsigned int  height = 1;height < KRmrf::curImageHeight;++height){
                for(unsigned int  width = 0;width < KRmrf::curImageWidth;++width){
                    grad_output[dim][height*KRmrf::curImageWidth + width] += grad_image[dim][(height-1)*KRmrf::curImageWidth + width];
                }
            }
            // left diff
            grad_image = grid[3];
            for(unsigned int  height = 0;height < KRmrf::curImageHeight;++height){
                for(unsigned int  width = 1;width < KRmrf::curImageWidth;++width){
                    grad_output[dim][height*KRmrf::curImageWidth + width] += grad_image[dim][height*(KRmrf::curImageWidth-1) + width -1];
                }
            }

            // Account for less neighbors on edge pixels.
            for(unsigned int  height = 0;height < KRmrf::curImageHeight;++height){
                for(unsigned int  width = 0;width < KRmrf::curImageWidth;++width){
                    if(width == 0 || height == 0 || (height + 1 == KRmrf::curImageHeight) || (width + 1 == KRmrf::curImageWidth))
                        grad_output[dim][height*KRmrf::curImageWidth + width] *= 4./3;
                }
            }

        }

        return grad_output;
    }

    static std::vector<double> make_grid_grad(std::vector<std::vector<double> > &grid, const std::vector<double> & image){

        std::vector<std::vector<double> > vecImage(1, image);
        std::vector<std::vector<std::vector<double> > > tempGrad;

        tempGrad.push_back(std::vector<std::vector<double> >(1, grid[0]));
        tempGrad.push_back(std::vector<std::vector<double> >(1, grid[1]));
        tempGrad.push_back(std::vector<std::vector<double> >(1, grid[2]));
        tempGrad.push_back(std::vector<std::vector<double> >(1, grid[3]));

        std::vector<std::vector<double> > tempGradOutput = make_grid_grad(tempGrad, vecImage);

        return tempGradOutput[0];
    }

};

class KFun_dE_D{
public:
    std::vector<double> operator()(const std::vector< std::vector<double> > imageI, const std::vector< std::vector<double> > imageA, const std::vector<double> imageD){
        std::vector<double> likdhood_grad = KFuncLibs::likelihood_grad_D(imageI, imageA, imageD);
        std::vector<double> depth_grad = KFuncLibs::depth_prior_grad(imageD);

        if(likdhood_grad.size() != depth_grad.size()){
            std::cerr << "KFun_dE_D: unequal dim inputs." << std::endl;
            return std::vector<double>();
        }
		// likehood fuunction's gradient about depth
        for(unsigned int index = 0;index < likdhood_grad.size();++index){
            likdhood_grad[index] = (KRmrf::s_priorDWeight * depth_grad[index] + likdhood_grad[index])/(imageD.size());
        }

        return likdhood_grad;
    }
};

class KFun_dE_A{
public:
    std::vector< std::vector<double> > operator()(const std::vector< std::vector<double> > imageI, const std::vector< std::vector<double> > imageA, const std::vector<double> imageD, const std::vector<double> apowVec, const std::vector<double> ascaleVec){
        std::vector< std::vector<double> > likdhood_grad = KFuncLibs::likelihood_grad_A(imageI, imageA, imageD);
        std::vector< std::vector<double> > albedod_grad = KFuncLibs::albedo_prior_grad(imageA, apowVec, ascaleVec);

        if(likdhood_grad.size() != albedod_grad.size()){
            std::cerr << "KFun_dE_A: unequal dim inputs." << std::endl;
            return std::vector< std::vector<double> >();
        }
		// likehood fuunction's gradient about albedod
        for(unsigned int dim = 0;dim < likdhood_grad.size();++dim){
            for(unsigned int index = 0;index < likdhood_grad[dim].size();++index){
                likdhood_grad[dim][index] = (KRmrf::s_priorCWeight * albedod_grad[dim][index] + likdhood_grad[dim][index])/(imageD.size());
            }
        }

        return likdhood_grad;
    }
};


class KFun_E{
public:
	// energy function (posterior probability)
    double operator()(const std::vector< std::vector<double> > &imageI, const std::vector< std::vector<double> > &imageA, const std::vector<double> &imageD, const std::vector<double> &apowVec, const std::vector<double> &ascaleVec){
        return (KFuncLibs::likelihood(imageI, imageA, imageD) + KRmrf::s_priorCWeight * KFuncLibs::albedo_prior(imageA, apowVec, ascaleVec) + KRmrf::s_priorDWeight * KFuncLibs::depth_prior(imageD))/(imageD.size());
    }
};




#endif // KFUNUTILS_H
