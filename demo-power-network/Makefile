CPPFLAGS = -I$(FNCS_INSTALL)/include/fncs -DDEBUG=1
LDFLAGS = -L$(FNCS_INSTALL)/lib
LIBS = -lfncs -lzmq

all: sim_network sim_power

sim_network: sim_network.cpp
	$(CXX) sim_network.cpp -o sim_network $(CPPFLAGS) $(LDFLAGS) $(LIBS)

sim_power: sim_power.cpp
	$(CXX) sim_power.cpp -o sim_power $(CPPFLAGS) $(LDFLAGS) $(LIBS)

clean:
	rm -f sim_power sim_network
