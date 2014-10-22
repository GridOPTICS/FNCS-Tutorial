CPPFLAGS = -I$(FNCS_INSTALL)/include/fncs
LDFLAGS = -L$(FNCS_INSTALL)/lib
LIBS = -lfncs -lzmq

all: sim_powerA sim_powerB

sim_powerA: sim_powerA.cpp
	$(CXX) sim_powerA.cpp -o sim_powerA $(CPPFLAGS) $(LDFLAGS) $(LIBS)

sim_powerB: sim_powerB.cpp
	$(CXX) sim_powerB.cpp -o sim_powerB $(CPPFLAGS) $(LDFLAGS) $(LIBS)

clean:
	rm -f sim_powerA sim_powerB
