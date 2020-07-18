#include "marsyas/realvec.h"
#include "marsyas/types.h"
//#include "marsyas/marsystems/RealvecSource.h"
#include "marsyas/system/MarSystem.h"
#include "marsyas/system/MarSystemManager.h"
#include <regex>
#include "marsyas/MATLABengine.h"
#include "marsyas/common_source.h"

/* for G-House sample
Length: 0 m 4 s
Sampling frequency: 44100 Hz
Channels: 2
Byte rate: 258 kB/s
Bits per sample: 24b
Sample count: 176,400
*/

/* for Claps - Book Of Love
Length: 4 m 14 s
Sampling frequency: 44100 Hz
Channels: 2
Byte rate: 172 kB/s
Bits per sample: 16b
Sample count: 11,226,240
*/

/*
Sample Count = Fs * seconds

*/

using namespace Marsyas;
using namespace std;


//General processing
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
    //double Fs;
    //int[] wave;

    ///////////////////////////////
    /////      Octave processing
    ///////////////////////////////

    //[wave, Fs] = auload(file_in_loadpath("/media/samples/Sample Magic - G-House - WAV.REX.AIFF/WAV/SM White Label - G-House - Wav/Synth Loops/gh_syn120_vicesquad_Cm.wav"));


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
    tls->updControl("SoundFileSourceHopper/src/mrs_bool/mixToMono", false);

/*
    topLevelSeries->addMarSystem(mng.create("Windowing", "AnalWin"));     //Hanning Analysis window
    topLevelSeries->updControl("Windowing/AnalWin/mrs_string/type", "Hanning");
*/

    ////////////////////////////////////****************************************
    ////   Mid/Side Fanout            //****************************************
    ////////////////////////////////////****************************************

    MarSystem* fanoutMS = mng.create("Fanout", "fanoutMS"); //Fanout for mid .5(L + R) and side .5(L - R) outputs

    ////////////////////////////////////
    ////   Mid series                 //
    ////////////////////////////////////

    MarSystem* midSeries = mng.create("Series", "midS"); //mid series
    midS->addMarSystem(mng.create("Sum", "mid"));
    midS->updControl("Sum/mid/mrs_bool/stereo", false);
    midS->updControl("Sum/mid/mrs_real/weight", 0.5);
    midS->updControl("Sum/mid/mrs_string/mode", "orig");
    
    midS->addMarSystem(mng.create("PlotSink", "mid"));   // output mid signal
    midS->updControl("PlotSink/mid/mrs_string/filename", "mid-" + signalName);
    midS->updControl("PlotSink/mid/mrs_bool/single_file", true);
    midS->updControl("PlotSink/mid/mrs_bool/sequence", false);
    midS->updControl("PlotSink/mid/mrs_bool/no_ticks", true);
    //midSeries->updControl("PlotSink/mid-synth/mrs_bool/matlab", true);

    ////////////////////////////////////
    ////   Side series                //
    ////////////////////////////////////

    MarSystem* sideSeries = mng.create("Series", "sideSeries"); //side series
    sideSeries->addMarSystem(mng.create("Subtract", "side"));
    sideSeries->addMarSystem(mng.create("Gain", "sideGain"));
    sideSeries->updControl("Gain/sideGain/mrs_real/gain", 0.5);
    sideSeries->addMarSystem(mng.create("PlotSink", "side"));            // plotsink side signal
    sideSeries->updControl("PlotSink/side/mrs_string/filename", "side-" + signalName);
    sideSeries->updControl("PlotSink/side/mrs_bool/single_file", true);
    sideSeries->updControl("PlotSink/side/mrs_bool/sequence", false);
    sideSeries->updControl("PlotSink/side/mrs_bool/no_ticks", true);

    fanoutMS->addMarSystem(midSeries); //attach mid and series to fanout
    fanoutMS->addMarSystem(sideSeries);
    tls->addMarSystem(fanoutMS); //attach fanout to top level series


    ////////////////////////////////////****************************************
    ////   Main Fanout                //****************************************
    ////////////////////////////////////****************************************


    MarSystem* fanoutMain = mng.create("Fanout", "fanoutMain");

    ////////////////////////////////////
    ////   Energy, Difference series  //
    ////////////////////////////////////

    MarSystem* energyDiff = mng.create("Series", "energyDiff");
    fanoutMain->addMarSystem(energyDiff); //adding the series container to the main fanout
    tls->addMarSystem(fanoutMain);

    energyDiff->addMarSystem(mng.create("Rms", "rmsEnergy")); //Energy

    //MarSystem* s = mng.create("Series", "s");
    //energyDiff->addMarSystem(mng.create("Transposer", "trans")); //transpose


    MarSystem* parallelPlot = mng.create("Parallel", "parallelPlot");
    energyDiff->addMarSystem(parallelPlot);



    parallelPlot->addMarSystem(mng.create("PlotSink", "midRmsPlot"));
    parallelPlot->updControl("PlotSink/midRmsPlot/mrs_string/filename", "midEnergy-" + signalName);
    parallelPlot->updControl("PlotSink/midRmsPlot/mrs_bool/single_file", true);
    parallelPlot->updControl("PlotSink/midRmsPlot/mrs_bool/sequence", false);
    parallelPlot->updControl("PlotSink/midRmsPlot/mrs_bool/no_ticks", true);


    parallelPlot->addMarSystem(mng.create("PlotSink", "sideRmsPlot"));
    parallelPlot->updControl("PlotSink/sideRmsPlot/mrs_string/filename", "sideEnergy-" + signalName);
    parallelPlot->updControl("PlotSink/sideRmsPlot/mrs_bool/single_file", true);
    parallelPlot->updControl("PlotSink/sideRmsPlot/mrs_bool/sequence", false);
    parallelPlot->updControl("PlotSink/sideRmsPlot/mrs_bool/no_ticks", true);


    //topLevelSeries->addMarSystem(mng.create("Selector", "selector1"));
    //topLevelSeries->updControl("Selector/selector1/mrs_natural/disable", 1); //disable side

/*
    topLevelSeries->addMarSystem(mng.create("AudioSinkBlocking", "play"));          //listen to mid signal
    topLevelSeries->updControl("AudioSinkBlocking/play/mrs_natural/bufferSize", 2048);
    topLevelSeries->updControl("AudioSinkBlocking/play/mrs_bool/initAudio", true);
*/

/*
    energyDiff->addMarSystem(mng.create("Differentiator", "differentiator"));
    energyDiff->addMarSystem(mng.create("PlotSink", "diffPlot"));
    energyDiff->updControl("PlotSink/diffPlot/mrs_string/filename", "Difference-synth1");
    energyDiff->updControl("PlotSink/diffPlot/mrs_bool/single_file", true);
    energyDiff->updControl("PlotSink/diffPlot/mrs_bool/sequence", false);
    energyDiff->updControl("PlotSink/diffPlot/mrs_bool/no_ticks", true);
*/
    ////////////////////////////////////
    ////   Beat Histogram series      // Current algorithm implements flux for an onset detection function
    ////////////////////////////////////

    MarSystem* powerSpecSeries = mng.create("Series", "powerSpecSeries");
    fanoutMain->addMarSystem(powerSpecSeries); //attaching to fanoutMain


    powerSpecSeries->addMarSystem(mng.create("Selector", "selector2"));
    powerSpecSeries->updControl("Selector/selector2/mrs_natural/disable", 1);//disable side

    powerSpecSeries->addMarSystem(mng.create("Spectrum", "spec")); //Complex Spectrum {Re(0), Re(N/2), Re(1), Im(1),..., Re(N/2-1), Im(N/2-1)}^T

    //MarSystem* accum1 = mng.create("Accumulator", "accum1mrs_realvec");
    //accum1->updControl("mrs_string/mode", "explicitFlush");

    //powerSpecSeries->addMarSystem(accum1);

    powerSpecSeries->addMarSystem(mng.create("PowerSpectrum", "specMag")); //Magnitude of Spectrum
    powerSpecSeries->updControl("PowerSpectrum/specMag/mrs_string/spectrumType", "magnitude");


    powerSpecSeries->addMarSystem(mng.create("PlotSink", "specMagPlot"));       // Plot spectrum
    powerSpecSeries->updControl("PlotSink/specMagPlot/mrs_string/filename", "SpecMag-" + fileName);
    powerSpecSeries->updControl("PlotSink/specMagPlot/mrs_bool/single_file", true);
    powerSpecSeries->updControl("PlotSink/specMagPlot/mrs_bool/sequence", false);
    powerSpecSeries->updControl("PlotSink/specMagPlot/mrs_bool/no_ticks", false);

/*
    beatHist->addMarSystem(mng.create("Spectrum2Mel", "melBank"));
    beatHist->updControl("Spectrum2Mel/melBank/mrs_natural/melBands", 20);
    beatHist->updControl("Spectrum2Mel/melBank/mrs_bool/htkMel", true);
    beatHist->updControl("Spectrum2Mel/melBank/mrs_bool/constAmp", false);

    beatHist->addMarSystem(mng.create("Flux", "flx"));
    beatHist->updControl("Flux/flx/mrs_string/mode", "marsyas");

    beatHist->addMarSystem(mng.create("BeatHistogram", "beatHist"));

    beatHist->addMarSystem(mng.create("PlotSink", "beatHist"));       // Plot beat histogram
    beatHist->updControl("PlotSink/beatHist/mrs_string/filename", "beatHist-" + fileName);
    beatHist->updControl("PlotSink/beatHist/mrs_bool/single_file", true);
    beatHist->updControl("PlotSink/beatHist/mrs_bool/sequence", false);
    beatHist->updControl("PlotSink/beatHist/mrs_bool/no_ticks", true);
*/

    //topLevelSeries->put_html(std::cout); //parallelPlot
/*
    beatExtraction->addMarSystem(mng.create("AutoCorrelation", "autoC"));
    beatExtraction->updControl("AutoCorrelation/autoC/mrs_bool/aliasedOutput", false);
    beatExtraction->updControl("AutoCorrelation/autoC/mrs_bool/setr0to1", true);

    beatExtraction->addMarSystem(mng.create("PlotSink", "autoPlot"));
    beatExtraction->updControl("PlotSink/autoPlot/mrs_string/filename", "autoC-synth1");
    beatExtraction->updControl("PlotSink/autoPlot/mrs_bool/single_file", true);
    beatExtraction->updControl("PlotSink/autoPlot/mrs_bool/sequence", false);
    beatExtraction->updControl("PlotSink/autoPlot/mrs_bool/no_ticks", true);

    beatExtraction->addMarSystem(mng.create("BeatHistogram", "BH"));

    beatExtraction->addMarSystem(mng.create("PlotSink", "beatPeaks"));
    beatExtraction->updControl("PlotSink/beatPeaks/mrs_string/filename", "peaks-synth1");
    beatExtraction->updControl("PlotSink/beatPeaks/mrs_bool/single_file", true);
    beatExtraction->updControl("PlotSink/beatPeaks/mrs_bool/sequence", false);
    beatExtraction->updControl("PlotSink/beatPeaks/mrs_bool/no_ticks", true);
*/

    /////////////////////////////////////////////////////////////////////
    // This loop runs (ticks) the system until the end of the soundfile
    /////////////////////////////////////////////////////////////////////
/*
    do {

        topLevelSeries->tick();

        //Marsyas::realvec SpectrumDatasample_rate
        //Marsyas::realvec processedData = series1->getControl("mrs_realvec/processedData")->to<mrs_realvec>();
        sampleCount += hopSize;
        Marsyas::realvec processedData = powerSpecSeries->getControl("PowerSpectrum/specMag/mrs_realvec/processedData")->to<mrs_realvec>();
        std::cout << processedData << std::endl;

    } while (topLevelSeries->getControl("SoundFileSourceHopper/src/mrs_bool/hasData")->to<mrs_bool>());
*/
    while (tls->getControl("SoundFileSourceHopper/src/mrs_bool/hasData")->to<mrs_bool>())
    {
      tls->tick();

      //Marsyas::realvec SpectrumDatasample_rate
      //mrs_realvec specMagVec = powerSpecSeries->getControl("PowerSpectrum/specMag/mrs_realvec/processedData")->to<mrs_realvec>();
      //std::cout << specMagVec << std::endl;
      sampleCount += hopSize;


    }
    //cout << specMagMatrix.getSize() << endl;
    //specMagMatrix.write(specMagFile);
    sampleCount -= hopSize; //extra hop, correct sample count


    mrs_real sample_rate = topLevelSeries->getControl("SoundFileSourceHopper/src/SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
    std::cout << sample_rate << std::endl;

    //mrs_natural onObs = energyDiff->getControl("mrs_natural/onObservations")->to<mrs_natural>();
    //std::cout << onObs << std::endl;

    sample_rate = topLevelSeries->getControl("mrs_real/osrate")->to<mrs_real>();
    std::cout << sample_rate << std::endl;

    std::cout << sampleCount;


    delete tls;


    return 0;
}


void printSys(){

    //topLevelSeries->put_html(std::cout);
    //std::cout << *topLevelSeries;
}
