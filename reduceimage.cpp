#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/chrono.hpp>
#include <ctime>

#include <sys/time.h>
#include <stdio.h>
#include <cstdlib>
#include <fstream>

#include <time.h>

#include <unistd.h>

#include "opencv2/highgui/highgui.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <pthread.h>
#include <vector>
#include <string>

#include "args.hxx"

#include "Image.cpp"


#ifdef HAVE_OPENCL
#include <opencv2/core/ocl.hpp>
#endif




std::string version="1.0.001";


double debug_start_time = (double)cv::getTickCount();
double debug_last_time = (double)cv::getTickCount();
double debug_window_offset = 0;

bool bDebugTime=false;
void debugTime(std::string str){
  if (bDebugTime){
    double time_since_start = ((double)cv::getTickCount() - debug_start_time)/cv::getTickFrequency();
    double time_since_last = ((double)cv::getTickCount() - debug_last_time)/cv::getTickFrequency();
    std::cout << str << ": " << time_since_last << "s " << "(total: " << time_since_start  << "s)" << std::endl;
  }
  debug_last_time = (double)cv::getTickCount();
}

int main( int argc, char** argv ){


  args::ArgumentParser parser("Reduce image size.", "...");
  args::HelpFlag help(parser, "help", "Display this help menu", { "help"});
  args::Flag debug(parser, "debug", "Show debug messages", {'d', "debug"});
  args::ValueFlag<std::string> filename(parser, "filename", "The filename", {'f',"file"});



  try
  {
      parser.ParseCLI(argc, argv);


      if (filename==0){
        std::cout << parser;
        return 0;
      }
  }
  catch (args::Help)
  {

      std::cout << parser;
      return 0;
  }
  catch (args::ParseError e)
  {
      std::cerr << e.what() << std::endl;
      std::cerr << parser;
      return 1;
  }
  if (debug){
    std::cout << "processing image: " << args::get(filename) << std::endl;
  }



  debugTime("Start");
  Image* im=new Image();
  im->setDebug(debug);
  im->open((args::get(filename)).c_str());
  std::string sfilename = args::get(filename);
  std::string resultfilename = boost::str( boost::format("%s") % sfilename  );
  im->save(resultfilename);

  debugTime("Stop");

  return -1;
}
