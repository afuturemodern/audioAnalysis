# audioAnalysis
scripts for audio analysis using marsyas

install marsyas per instructions given in marsyas-user.pdf

if on Arch, install using package manager with indicated dependencies:

`sudo pacman -S marsyas`

else on linux, install marsyas source (http://marsyas.info/downloads/sources.html) and dependencies:

`cd marsyas/`

`mkdir build`

`cd build`

Before configuring the CMake modules, you'll need to install Qt5:
`sudo apt install qt5-default`

You'll also need qtdeclarative5-dev to get Qt5Quick:
`sudo apt install qtdeclarative5-dev`

`ccmake ..`   :I have swig_ruby, vamp, max/msp, matlab, oss set to OFF, then configure

If ccmake cannot find your Qt5 dirs, you'll need to point it to them. Mine were in `/usr/lib/x86_64-linux-gnu/cmake/`
If it's giving you MIDI_IO and AUDIO_IO errors, just turn those to OFF in the ccmake interface & try again.

After you're able to generate the ccmake targets without errors, run:
`make`

`sudo make install`

In FundamentalFrequency.cpp, you'll need to change `/home/sgnldsrptr/122GbBass.wav` to a locally-stored .wav file of your own.
To compile .cpp files, cd to where you cloned this repo, and run:

`make`     
`./dataAnal`

If you're getting the error "./dataAnal: error while loading shared libraries: libmarsyas.so: cannot open shared object file: No such file or directory", you'll need to run:
`sudo /sbin/ldconfig -v`
