ifndef ICEDIR
ICEDIR=/home/noo/ice
endif

LOPT := $(OPT)
COPT := $(OPT) -std=c++11
LIBS := -lice -ljpeg -ltiff -lpng `wx-config --libs`

ifndef MACHTYPE
MACHTYPE := some_machine
endif

COPT := $(COPT) -g -Wall -fno-strict-aliasing -Wno-sign-compare -Wno-unused-local-typedefs
#-ffast-math

INCLUDE := -I$(ICEDIR)/src `wx-config --cxxflags`
LIBRARY := -L$(ICEDIR)/lib/$(MACHTYPE)

LIBS := $(LIBS)

ifdef OPENMP
COPT := $(COPT) -fopenmp -DOPENMP
LOPT := $(LOPT) -fopenmp
endif

ifndef NOFFTW3
LIBS := $(LIBS) -lfftw3 
endif

all: v23d patterngen genCalib calibrate makeCV

v23d: v23d.o v2file.o v2analyse.o v2signal.o v2vector.o v2phases.o FileName.o $(ICEDIR)/lib/$(MACHTYPE)/libice.a
	$(CXX) $(LOPT) $(LIBRARY) -o v23d v23d.o v2file.o v2analyse.o v2signal.o v2vector.o v2phases.o FileName.o $(LIBS)

calibrate: calibrate.o calMarker.o v2trafo.o v2file.o FileName.o $(ICEDIR)/lib/$(MACHTYPE)/libice.a
	$(CXX) $(LOPT) $(LIBRARY) -o calibrate calibrate.o FileName.o calMarker.o v2trafo.o v2file.o $(LIBS)

makeCV: makeCV.o v2file.o FileName.o v2trafo.o $(ICEDIR)/lib/$(MACHTYPE)/libice.a
	$(CXX) $(LOPT) $(LIBRARY) -o makeCV makeCV.o FileName.o v2file.o v2trafo.o $(LIBS)	

testcalib: testcalib.o v2trafo.o  $(ICEDIR)/lib/$(MACHTYPE)/libice.a
	$(CXX) $(LOPT) $(LIBRARY) -o testcalib testcalib.o v2trafo.o $(LIBS)	

%.o:%.c
	$(CC) $(COPT) $(INCLUDE) -c $*.c

%.o:%.cpp
	$(CXX) $(COPT) $(INCLUDE) -c $*.cpp

%: %.o
	$(CXX) $(COPT) $(LOPT) $(LIBRARY) -o $* $*.o $(LIBS)

%:%.cpp
	$(CXX) $(COPT) $(INCLUDE) -c $*.cpp
	$(CXX) $(LIBRARY) $(LOPT) -o $* $*.o $(LIBS)

%.cpp:%.ag
	AppGen $*.ag $*.cpp
#	$(CXX) $(COPT) $(INCLUDE) -c $*.cpp
#	rm $*.cpp

clean_files:
	-rm -f *.o depend *~ *orig
	-rm -f .*.del_cache

clean:	clean_files dep

dep:
	-rm depend 
	touch depend
	$(CXX) -M $(COPT) $(INCLUDE) *.cpp >> depend	

# command for beautifier
ASTYLE := astyle
# format options for beautifier
ASTYLE_OPT = --style=gnu --unpad-paren --pad-header --pad-oper --indent-namespaces --indent=spaces=2 --convert-tabs --align-pointer=type --align-reference=type

format:
	$(ASTYLE) $(ASTYLE_OPT) *.cpp      
	$(ASTYLE) $(ASTYLE_OPT) *.h
	$(ASTYLE) $(ASTYLE_OPT) *.ag

-include depend
