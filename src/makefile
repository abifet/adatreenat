CXX             = g++
DEBUG           = -Wall -g 
#CXXFLAGS        = $(DEBUG) -I./ -march=x86-64 -m64 -D__64BIT__ 
CXXFLAGS        =  -O3
LDFLAGS         = 

SRC		= ListNode.cpp List.cpp Adwin.cpp AdaTreeNat.cpp
OBJ		= $(addsuffix .o, $(basename $(SRC)))

TARGET          = AdaTreeNat

all             : $(TARGET)

AdaTreeNat		:$(OBJ)
		$(CXX) $(OBJ) -o $@ $(LDFLAGS)

depend		:
		makedepend $(CXXFLAGS) -Y $(SRC)

clean           :
		rm -f core *.o *.a *~ $(TARGET)

new             :
		$(MAKE) clean
		$(MAKE)
# DO NOT DELETE

ListNode.o: ListNode.h
List.o: List.h ListNode.h
Adwin.o: Adwin.h List.h ListNode.h
AdaTreeNat.o: Adwin.h List.h ListNode.h
