#
# 'make depend' uses makedepend to automatically generate dependencies
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#


#  ****LOAD HDD****  sudo mount -t vboxsf music /media/music

#### g++ -std=gnu++0x -I/home/jonathan/Desktop/audioDSP/marsyas/src/marsyas -I/home/jonathan/Desktop/audioDSP/marsyas/src/marsyas/system -I/usr/local/include/octave-3.6.1 -o DataAnalysis DataAnalysis.cpp -L/usr/local/lib -L/home/jonathan/Desktop/audioDSP/marsyas/lib -loctave -lAquila -lOoura_fft -lboost_iostreams -lboost_system  -lmarsyas
#### LD_LIBRARY_PATH="/usr/local/lib/" ./DataAnalysis

# define the C compiler to use
CC = g++

#g++ -o DataAnalysis  DataAnalysis.cpp -lAquila

# define any compile-time flags     -Wextra -Wunreachable-code -ggdb -O0   -std=gnu++0x -std=c++11 -std=gnu++11 -
#Wl,rpath=/usr/local/MATLAB/R2017a/bin/glnxa64/
CXXFLAGS = -std=c++17 -Wall -O0 -g
# define any directories containing header files other than /usr/include    -I/home/jonathan/Desktop/audioDSP/marsyas/src
# -I/usr/include/octave-3.6.1/ -I/home/jonathan/Desktop/audioDSP/aquila/include  -I/usr/local/include/octave-4.0.0
INCLUDES = -L/usr/local/include #-I/home/john/audioDSP/aquila/include -I/usr/include/octave-4.0.2/

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify    -L/home/jonathan/Desktop/audioDSP/aquila/lib
#   their path using -Lpath, something like:   -L/home/jonathan/Desktop/audioDSP/marsyas-0.5.0/lib -L/home/jonathan/Desktop/audioDSP/marsyas-0.5.0/lib
#-L/usr/local/lib -L/usr/lib/x86_64-linux-gnu  -L/usr/local/lib -L/home/jonathan/Desktop/audioDSP/aquila/lib   -L/usr/local/lib/octave/4.0.0
LDFLAGS = -L/usr/local/lib #-L/usr/local/MATLAB/R2017a/bin/glnxa64 -L/home/john/audioDSP/aquila/lib -L/home/john/marsyas-master/src/marsyas/marsystems


# define any libraries to link into executable: -lmwMATLAB_res
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname    -loctave -loctinterp -lAquila -lOoura_fft -lboost_system
#   option, something like (this will link in libmylib.so and libm.so: -lboost_iostreams -lboost_system -lasound
LIBS = -lmarsyas #-lAquila -loctave  -leng -lmx -lut -lmwcpp11compat -lmwresource_core -lmwi18n -lmwfl -lmat

# define the C source files
SRCS = FundamentalFrequency.cpp

MUSICDIRECTORY := /media/john/New Volume/music


# define the C object files
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#

# define the executable file
MAIN = dataAnal

#
# The following part of the makefile is generic; it can be used to
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean

all:
	rm -f *.o
	$(CC) $(CXXFLAGS) $(INCLUDES) -c -o $(MAIN).o  $(SRCS)
	$(CC) $(LDFLAGS) -o $(MAIN) $(MAIN).o $(LIBS)

run:
	./dataAnal
