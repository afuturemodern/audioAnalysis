# audioAnalysis
scripts for audio analysis using marsyas

install marsyas per instructions given in marsyas-user.pdf

if on Arch, install using package manager with indicated dependencies:

sudo pacman -S marsyas

else on linux, install marsyas source and dependencies:

cd marsyas/
mkdir build
cd build
ccmake ..  #I have swig_ruby, vamp, max/msp, matlab, oss set to OFF then configure 
make
make install
