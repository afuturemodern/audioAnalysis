#include "marsyas/realvec.h"
#include "marsyas/types.h"
//#include "marsyas/marsystems/RealvecSource.h"
#include "marsyas/system/MarSystem.h"
#include "marsyas/system/MarSystemManager.h"
#include <regex>
#include "marsyas/MATLABengine.h"
//#include "marsyas/marsystems/F0Analysis.h"
//#include "marsyas/common_source.h"
#include "/home/john/marsyas-master/src/marsyas/marsystems/F0Analysis.h"


using namespace Marsyas;
using namespace std;


int main(int argc, char *argv[]) {


    //const std::size_t SIZE = 64; (void) SIZE;

    realvec input; //left and right
    realvec mid;
    realvec side;
    realvec specMagMatrix;
    realvec energy;
    realvec difference;
    realvec V_t;   //current feature vector V_t
    realvec V_t_1; //previous feature vector V_t-1


    mrs_real    sampleRate; (void) sampleRate;
    mrs_natural bitDepth; (void) bitDepth; // bits per sample
    mrs_natural channels; (void) channels; // expecting 2 channels
    mrs_natural bitRate; (void) bitRate; // bits per second
    mrs_natural windowSize = 256;
    mrs_natural hopSize = 256;
    mrs_natural sampleCount = windowSize; //hops to be added for sample count

    std::string fileName = "/media/john/New Volume/Ableton Samples/Sample Magic - G-House - WAV.REX.AIFF/WAV/SM White Label - G-House - Wav/Synth Loops/gh_syn120_vicesquad_Cm.wav";
    mrs_string signalName = "gh_syn120_vicesquad_Cm";
    string specMagFile = "specMag-" + signalName;

    MarSystemManager mng;


    ////////////////////////////
    ////   Marsyas Signal Processing (No scheduling or time events have been incorporated yet)
    ////////////////////////////


    MarSystem* tls = mng.create("Series", "tls");//****Top level series module****

    ////////////////////////////////////////////////****************************
    ////   Soundfile hopper and analysis window   //****************************
    ////////////////////////////////////////////////****************************

    tls->addMarSystem(mng.create("SoundFileSourceHopper", "src")); //default window is rectangular
    tls->updControl("SoundFileSourceHopper/src/mrs_string/filename", fileName);
    tls->updControl("SoundFileSourceHopper/src/mrs_natural/windowSize", windowSize);
    tls->updControl("SoundFileSourceHopper/src/mrs_natural/hopSize", hopSize);
    tls->updControl("SoundFileSourceHopper/src/mrs_bool/mixToMono", true);


    tls->addMarSystem(mng.create("PlotSink", "signalPlot")); //outputs original signal
    tls->updControl("PlotSink/signalPlot/mrs_string/filename", "signal-" + signalName);
    tls->updControl("PlotSink/signalPlot/mrs_bool/single_file", true);
    tls->updControl("PlotSink/signalPlot/mrs_bool/sequence", false);
    tls->updControl("PlotSink/signalPlot/mrs_bool/no_ticks", true);

    tls->addMarSystem(mng.create("AubioYin", "f0")); //fundamental frequency estimator

    tls->addMarSystem(mng.create("PlotSink", "f0Plot")); //outputs fund. freq.
    tls->updControl("PlotSink/f0Plot/mrs_string/filename", "f0-" + signalName);
    tls->updControl("PlotSink/f0Plot/mrs_bool/single_file", true);
    tls->updControl("PlotSink/f0Plot/mrs_bool/sequence", false);
    tls->updControl("PlotSink/f0Plot/mrs_bool/no_ticks", true);

    /////////////////////////////////////////////////////////////////////
    // This loop runs (ticks) the system until the end of the soundfile
    /////////////////////////////////////////////////////////////////////

    while (tls->getControl("SoundFileSourceHopper/src/mrs_bool/hasData")->to<mrs_bool>())
    {
      tls->tick();



      sampleCount += hopSize;

    }
    //cout << specMagMatrix.getSize() << endl;
    //specMagMatrix.write(specMagFile);
    sampleCount -= hopSize; //extra hop, correct sample count


    mrs_real sample_rate = tls->getControl("SoundFileSourceHopper/src/SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
    cout << sample_rate << endl;

    sample_rate = tls->getControl("mrs_real/osrate")->to<mrs_real>();
    cout << sample_rate << endl;

    cout << sampleCount << endl;

    delete tls;

    return 0;
}
