#include "kfmrf.h"
#include "kfunutils.h"

f_and_g::f_and_g(std::vector< std::vector<double> > & I_n,
        std::vector<double> &apow,
        std::vector<double> &ascale,
        std::string &partial_type)
      :num_val(0),
       m_image(I_n),
       m_apow(apow),
       m_ascale(ascale),
       m_partial_type(partial_type){

}

double f_and_g::f_and_g_fun(double *x, double *g){
    ++num_val;

    double f = 0.;
    std::vector< std::vector<double> > albedo(m_image);
    std::vector<double> depth(m_image[0]);
	// use the raw x value to reshape into the real albedo and depth
    for(std::vector< std::vector<double> >::size_type index = 0;index < albedo.size();++index){
        for(std::vector<double>::size_type imageIndex = 0;imageIndex < albedo[index].size();++imageIndex){
			//std::cout << *x << " ";
			//if(imageIndex>100) system("pause");
             albedo[index][imageIndex] = *x++;
        }
    }
    for(std::vector<double>::size_type imageIndex = 0;imageIndex < depth.size();++imageIndex){
         depth[imageIndex] = *x++;
    }

	// if we are fix depth and optimize albedo to maximum the posterior
    if(m_partial_type == "A"){
		// current energy (likehood)
        f = KFun_E()(m_image, albedo, depth, m_apow, m_ascale);
		// current energy's gradient about albedo
        std::vector< std::vector<double> > pg = KFun_dE_A()(m_image, albedo, depth, m_apow, m_ascale);

		// fill the gradient calculated above into optimize function
        for(std::vector< std::vector<double> >::size_type index = 0;index < pg.size();++index){
            for(std::vector<double>::size_type imageIndex = 0;imageIndex < pg[index].size();++imageIndex){
                 *g++ = pg[index][imageIndex];
            }
        }

    }else{ // if we are fix albedo and optimize depth to maximum the posterior
		// current energy (likehood)
        f = KFun_E()(m_image, albedo, depth, m_apow, m_ascale);
		// current energy's gradient about depth
        std::vector<double> pg = KFun_dE_D()(m_image, albedo, depth);

		// fill the gradient calculated above into optimize function
        g += m_image.size() * pg.size(); // this time is the gradient about depth, and use the history gradient about albedo
        for(std::vector<double>::size_type imageIndex = 0;imageIndex < pg.size();++imageIndex){
             *g++ = pg[imageIndex];
        }
    }

    return f;
}

bool KRmrf::parseArguments(){
    std::string imgNameNoExt("");
    {
        mapIter it = s_paramMap.find("image");
        if(it != s_paramMap.end()){
            s_imagePath = it->second;
            trim(s_imagePath);
            imgNameNoExt = s_imagePath.substr(s_imagePath.find_last_of("\\/")+1);
            imgNameNoExt = imgNameNoExt.substr(0, imgNameNoExt.find_last_of("."));
            imgNameNoExt = s_imagePath.substr(0, s_imagePath.find_last_of("\\/")+1)+imgNameNoExt;
        }else s_imagePath="";
    }
    {
        mapIter it = s_paramMap.find("albedo-output");
        if(it != s_paramMap.end()){
            s_albedoOutPath = it->second;
            trim(s_albedoOutPath);
        }else s_albedoOutPath=imgNameNoExt+"_albedo";

    }
    {
        mapIter it = s_paramMap.find("depth-output");
        if(it != s_paramMap.end()){
            s_depthOutPath = it->second;
            trim(s_depthOutPath);
        }else s_depthOutPath=imgNameNoExt+"_depth";
    }
    {
        mapIter it = s_paramMap.find("n_outer_iterations");
        if(it != s_paramMap.end()){
            std::istringstream is(it->second);
            is >> s_numOuterIter;
        }else s_numOuterIter=3;
    }
    {
        mapIter it = s_paramMap.find("n_inner_iterations");
        if(it != s_paramMap.end()){
            std::istringstream is(it->second);
            is >> s_numInnerIter;
        }else s_numInnerIter=20;
    }
    {
        mapIter it = s_paramMap.find("airlight");
        s_bAirLightUseRect = false;
        if(it != s_paramMap.end()){
            KIntArgsInterator iter(it->second);
            double tempDbl = 0.;
            while(iter.next(tempDbl) != KIntArgsInterator::NoNext && s_airLight.size() < 4){
                s_airLight.push_back(tempDbl);
            }
            while(s_airLight.size()<3) s_airLight.push_back(0.5);
        }else{
            it = s_paramMap.find("airlight-rect");
            if(it != s_paramMap.end()){
                s_bAirLightUseRect = true;
                KIntArgsInterator iter(it->second);
                double tempDbl = 0.;
                while(iter.next(tempDbl) != KIntArgsInterator::NoNext && s_airLight.size() < 5){
                    s_airLight.push_back(tempDbl);
                }
                while(s_airLight.size()<4) s_airLight.push_back(0);
            }else s_airLight = std::vector<double>(3, .5);
        }
    }

    {
        mapIter it = s_paramMap.find("prior_C_weight");
        if(it != s_paramMap.end()){
            std::istringstream is(it->second);
            is >> s_priorCWeight;
        }else s_priorCWeight = 2e-6;
    }
    {
        mapIter it = s_paramMap.find("prior_D_weight");
        if(it != s_paramMap.end()){
            std::istringstream is(it->second);
            is >> s_priorDWeight;
        }else s_priorDWeight=1;
    }
    {
        mapIter it = s_paramMap.find("dpt");
        if(it != s_paramMap.end()){
            s_priorType = it->second;
            trim(s_priorType);
        }else s_priorType="laplace";
    }
    {
        mapIter it = s_paramMap.find("ms");
        if(it != s_paramMap.end()){
            std::istringstream is(it->second);
            is >> s_bMultiScale;
        } else s_bMultiScale=true;
    }
    {
        mapIter it = s_paramMap.find("id");
        if(it != s_paramMap.end()){
            std::istringstream is(it->second);
            is >> s_bStoreInitial;
        }else s_bStoreInitial=true;
    }

    return true;
}

std::string KRmrf::s_imagePath;
std::string KRmrf::s_albedoOutPath;
std::string KRmrf::s_depthOutPath;
unsigned int KRmrf::s_numOuterIter;
unsigned int KRmrf::s_numInnerIter;
std::vector<double> KRmrf::s_airLight;
bool KRmrf::s_bAirLightUseRect;
double KRmrf::s_priorCWeight;
double KRmrf::s_priorDWeight;
std::string KRmrf::s_priorType;
bool KRmrf::s_bMultiScale;
bool KRmrf::s_bStoreInitial;
std::map<std::string,std::string> KRmrf::s_paramMap;
unsigned int KRmrf::curImageWidth;
unsigned int KRmrf::curImageHeight;
unsigned int KRmrf::orgImageWidth;
unsigned int KRmrf::orgImageHeight;

