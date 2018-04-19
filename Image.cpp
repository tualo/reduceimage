#include "Image.h"




void Image::showImage(){
  showImage(debugImage,showDebugWindowWaitTime);
}


void Image::showImage(cv::Mat& src){
  showImage(src,showDebugWindowWaitTime);
}

void Image::showImage(cv::Mat& src,int ww){
  if (showDebugWindow){
    cv::Mat rotated=src.clone();
    int x=src.cols /5;
    int y=src.rows /5;
    cv::Mat res = cv::Mat(x, y, CV_8UC1);
    cv::resize(rotated, res, cv::Size(x, y), 0, 0, 3);
    cv::namedWindow("DEBUG", CV_WINDOW_AUTOSIZE );
    cv::imshow("DEBUG", res );
    cv::waitKey(ww);
  }
}


void Image::setDebug(bool val){
  showDebug=val;
}

void Image::setDebugWindow(bool val){
  showDebugWindow=val;
}

void Image::setDebugTime(bool val){
  showDebugTime=val;
}

void Image::_debugTime(std::string str){
  if (showDebugTime){
    double time_since_last = ((double)cv::getTickCount() - debug_last_time)/cv::getTickFrequency();
    std::cout << "(ImageRecognizeEx::_debugTime)\t" << str << ": " << time_since_last << "s " << std::endl;
  }
  debug_last_time = (double)cv::getTickCount();
}



Image::Image() :
  showDebug(false),
  showDebugWindow(false),
  showDebugTime(false),
  debug_last_time((double)cv::getTickCount()) {

  showDebugWindowWaitTime=500;
  subtractMean=20;

}

Image::~Image() {
  //tess->End();
}



void Image::open(const char* filename){
  fileName = filename;
  std::cout << "opening file \"" << filename << "\"." << std::endl;
  try{
    cv::setUseOptimized(true);
    cv::Mat mat = cv::imread( filename, cv::IMREAD_GRAYSCALE );
    setImage(mat);
  } catch (cv::Exception& e) {
      std::cerr << "Error opening file \"" << filename << "\". Reason: " << e.msg << std::endl;
      exit(1);
  }
}

void Image::setImage(cv::Mat mat){
  originalImage = mat.clone();
  debugImage=originalImage.clone();
  reducedImage=originalImage.clone();
  processImage=originalImage.clone();
  showImage(originalImage);
}


bool Image::is_digits(const std::string &str){
    return std::all_of(str.begin(), str.end(), ::isdigit); // C++11
}



cv::Mat Image::largestSimpleContour(cv::Mat& src){
  _debugTime("start largestSimpleContour");

  cv::Mat thr;
  cv::threshold(src, thr,subtractMean, 255,cv::THRESH_BINARY);


  int blength = thr.cols;
  int i=blength;
  int h=0;
  int height = thr.rows;


  double lastAVG = 255;
  int currentAVG = 255;
  int divisor=0;
  int avglength = 20;
  double cAVG=0;
  char avgbuffer[avglength];
  bzero(avgbuffer,avglength);
  char cavgbuffer[avglength];
  bzero(cavgbuffer,avglength);
  for(;i>199;i--){

    divisor=0;
    currentAVG=0;
    for(h=0;h<height;h+=5){
      currentAVG += thr.at<uchar>(h,i);
      divisor++;
    }
    currentAVG /=divisor;
    /*
    if (currentAVG>lastAVG*2){
      break;
    }
    */
    //std::cout << "currentAVG at i " << currentAVG << " - " << lastAVG << std::endl;
    lastAVG=0;
    cAVG=0;
    for(h=avglength-1;h>0;h--){
      avgbuffer[h]=avgbuffer[h-1];
      lastAVG+=avgbuffer[h];
    }
    for(h=avglength-1;h>0;h--){
      cavgbuffer[h]=cavgbuffer[h-1];
      cAVG+=cavgbuffer[h];
    }
    lastAVG=lastAVG/(avglength-1);
    cAVG=(cAVG+currentAVG)/(avglength);

    //std::cout << "currentAVG at i "<< i << " current " << cAVG << " - last " << lastAVG << std::endl;
    if ((i<blength-avglength*2) && (cAVG>lastAVG)) {
      //std::cout << "rising edge " << cAVG << " vs. " << lastAVG << std::endl;
      break;
    }
    avgbuffer[0]=currentAVG;
    cavgbuffer[0]=currentAVG;
  }
  //std::cout << "largestSimpleContour at i " << i << std::endl;
  if (i<200){
    std::cerr << "this should not happen the contour is to small " << i << " use the hole image "<< std::endl;
    i=blength;
  }

  cv::Rect myROI(0, 0, i, thr.rows);
  cv::Mat result = src(myROI);
  //roiImage = roiImage(myROI);

  _debugTime("stop largestSimpleContour");
  return result;
}


void Image::save(std::string name){
  cv::Mat mat = largestSimpleContour(processImage);
  std::vector<int> params;
  params.push_back(CV_IMWRITE_JPEG_QUALITY);
  params.push_back(50);
  cv::imwrite(name,mat,params);
}
