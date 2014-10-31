# FNCS-Tutorial One Power and One Network Simulator

Step-by-step guide for building and running FNCS with two small
simulator applications, one a power simulator and the other a network
simulator.

## Table of Contents
- [Hardware Requirements](#hardware-requirements)
- [Software and Dependencies](#software-and-dependencies)
  - [ZeroMQ Installation](#zeromq-installation)
  - [FNCS Installation](#fncs-installation)
- [Important Environment Variables](#important-environment-variables)
- [Model Description](#model-description)
- [Running the Co-Simulation](#running-the-co-simulation)

## Hardware Requirements
[back to contents](#table-of-contents)

This software was developed and tested exclusively using the Linux
operating system.  Our developers have used RedHat EL5 as well as Ubuntu
12.04.5 LTS (precise), both of which were 64-bit systems. This software
has not been built or tested on Windows. Although all of the software
packages and their dependencies are known to have Windows installers, we
have not yet developed any way to patch those official distributions
with FNCS enhancements.

## Software and Dependencies
[back to contents](#table-of-contents)

This portion of the tutorial will walk you through installing all
prerequisite software. The following software will be covered,
indicating the primary software installed and the list of dependencies:

You will need to install git in order to clone (checkout) all of our
FNCS and related software packages.

- FNCS
  - ZeroMQ (3.2.x)

It will be assumed that you will be installing all software into
$HOME/FNCS-install. Doing so will greatly simplify the steps of this
tutorial since we can set $LD_LIBRARY_PATH and $PATH accordingly, as
well as any other needed installation paths while building many of the
involved software packages. In fact, now would be a good time to set a
shortcut environment variable, like so:

```
export FNCS_INSTALL="$HOME/FNCS-install"
```

NOTE: You could, in theory, change this to point to wherever you wish to
install FNCS and all related software packages.

It is also assumed that you are using a Bourne shell; all of the
step-by-step instructions (like the one above) that appear in this
tutorial  will assume a Bourne shell. If you are using a C shell, we
hope you can adapt the steps as needed, mostly in how environment
variables are set.

### ZeroMQ Installation
[back to contents](#table-of-contents)

http://zeromq.org/

The ZeroMQ library is the only library that our FNCS library depends on.
We have extensively tested our software using version 3.2.x, however
later versions may also work but have not yet been tested.

Get the ZeroMQ software and install it using the following steps:

```
# we are doing everything from your $HOME directory
cd $HOME

# download zeromq
wget http://download.zeromq.org/zeromq-3.2.4.tar.gz
# if you do not have wget, use
# curl -O http://download.zeromq.org/zeromq-3.2.4.tar.gz

# unpack zeromq, change to its directory
tar -xzf zeromq-3.2.4.tar.gz
cd zeromq-3.2.4

# configure, make, and make install 
./configure --prefix=$FNCS_INSTALL
make
make install
```

Congratulations, you have now installed ZeroMQ. 

### FNCS Installation
[back to contents](#table-of-contents)

https://github.com/GridOPTICS/FNCS

The FNCS software will build and install the FNCS library, the various
FNCS header files, as well as the broker application. The FNCS broker
represents the central server that all other simulator clients will
connect to in order to synchronize in time and exchange messages with
other simulators. The FNCS library and header represent the needed API
for communicating with the broker using the sync and messaging function
calls.

Get the FNCS software and install it using the following steps:

```
# we are doing everything from your $HOME directory
cd $HOME

# download FNCS
git clone https://github.com/GridOPTICS/FNCS

# change to FNCS directory
cd FNCS

# configure, make, and make install 
./configure --prefix=$FNCS_INSTALL --with-zmq=$FNCS_INSTALL
make
make install
```

Congratulations, you have now installed FNCS.

## Important Environment Variables
[back to contents](#table-of-contents)

Now that all of the FNCS and related software is installed, now would be
a great time to set some important environment variables. If you have
been following the steps exactly, then you can copy-and-paste the
following into a handy shell script that you can source before running
the co-simulation. If you are reading this file from the github sources,
you will find the file [here](FNCS_env.sh).

Here is what the file recently looked like, but please refer to the
original file as linked above.

```Bash
#!/bin/sh

export FNCS_INSTALL="$HOME/FNCS-install"

# update LD_LIBRARY_PATH for all but MCR
if test "x$LD_LIBRARY_PATH" = x
then
    export LD_LIBRARY_PATH="$FNCS_INSTALL/lib"
else
    export LD_LIBRARY_PATH="$FNCS_INSTALL/lib:$LD_LIBRARY_PATH"
fi

# update PATH for all but MCR
if test "x$PATH" = x
then
    export PATH="$FNCS_INSTALL/bin"
else
    export PATH="$FNCS_INSTALL/bin:$PATH"
fi
```

## Model Description
[back to contents](#table-of-contents)

In this current tutorial directory you will find two source files,
[sim_power.cpp](sim_power.cpp) and [sim_network.cpp](sim_network.cpp),
as well as a [Makefile](Makefile) for compiling.

We have set up the co-sim to use a single "power" simulator and a single
"network" simulator. The power simulator contains two objects that wish
to send messages, "simObj1" and "simObj2". Those two objects are
shadowed by objects in the network simulator. In this demo, simObj1
sends to simObj2 only. When simObj1 sends a message to simObj2, it is
automatically sent from the power simulator to the network simulator
because by default FNCS assumes you want messages routed through a
network simulator. Once the network sim has scheduled the message to be
delivered, it is sent on to its original destination of simObj2 running
within the power sim.

We describe the network model as being a "shadow" of the power model.
Any object wishing to communicate through the network will register an
entity for the communication (described in detail in the first demo).
The network simulator will contain a "shadow" of every communicating
entity, so for instance if you register a "simObjA" in a power simulator
there must also be a "simObjA" registered within the network simulator,
as well. What acually happens when "simObjA" sends a message is that it
is automatically transferred to the network sim's "simObjA" shadow,
injected into the simulated network, read from the network at the
destination entity's shadow object, then transferred back to the actual
destination object from its shadow in the network.

This demo shows two different types of simulators, tick-based simulators
and event-based simulators. The code is similar, but when checking for
incoming messages it is sometimes convenient to use the callback
capability for event-based sims instead of polling for new messages as
in the tick-based sim.

### A Complete Breakdown of the our First Network Sim
In the last demo we examined the power simulator. This time, we look at
our first network simulator in detail. We've attempted to insert
comments step by step to make the file self-documenting as a reference.
You should start by reading the file line by line. We will highlight
important code for you in the next section of this tutorial. Here is
what the file recently looked like, but you can refer to the original
file [sim_network.cpp](sim_network.cpp), as well.

```c++
/* C++ STL */
#include <cstdlib>
#include <iostream>
#include <list>
#include <utility>

/* FNCS headers */
#include "integrator.h"
#include "message.h"
#include "objectcomminterface.h"

using namespace std; /* C++ standard namespace */
using namespace sim_comm; /* the FNCS namespace */

/* some C++ compilers have a nullptr instance,
 * otherwise we fall back to the old default of NULL */
#ifndef nullptr 
#define nullptr NULL
#endif

/* the current time of our simulator */
static TIME time_current=0;
/* time scheduled for delivery and the message to deliver */
static list<pair<TIME,Message*> > events;
/* the entitites that will communicate */
static ObjectCommInterface *simObj1 = NULL;
static ObjectCommInterface *simObj2 = NULL;

/* the callback function that FNCS can use to query the curren time */
TIME getCurrentTime() {
    return time_current;
}

/* callback when simObj1 shadow gets a message */
static void receive_message1()
{
    /* Just because FNCS will call this callback function to indicate
     * there are messages in the inbox, that doesn't mean they are
     * automatically processed. You must process all incoming messages
     * for this object. */
    while (simObj1->hasMoreMessages()) {
        Message *message = simObj1->getNextInboxMessage();
        /* Schedule when simObj2 should extract message from
         * simObj1 out of the network, 0 to 2 seconds later, randomly.
         * This is a very crude network simulator, randomly delaying
         * messages. */
        events.push_back(make_pair(time_current+rand()%2000000000, message));
        cout << "NetSim: simObj1 got message with timestamp "
            << message->getTime()
            << ", scheduled for "
            << events.back().first << endl;
    }
    /* Event-based simulators maintain a sorted event queue. */
    events.sort();
}

/* our simulator function, called by the main() function later */
static void network_simulator()
{
    TIME time_granted=0;
    TIME time_desired=0;

    /* initialize FNCS integrator with configuration file and current time */
    Integrator::initIntegrator("sim_network.json", time_current);
    /* FNCS must be able to request the current time of the simulator */
    Integrator::setTimeCallBack(CreateCallback(getCurrentTime));
    /* register an object that wishes to communicate */
    simObj1 = Integrator::getCommInterface("simObj1");
    /* register an object that wishes to communicate */
    simObj2 = Integrator::getCommInterface("simObj2");
    /* it's required to tell FNCS when all registrations are complete */
    Integrator::finalizeRegistrations();

    /* register callback function for simObj1 for incoming messages */
    simObj1->setMessageNotifier(CreateCallback(receive_message1));

    /* this network simulator will run indefinitely until FNCS detects
     * that the rest of the simulators are finished */
    do {
        /* Process any events in our event queue first.
         * That way, any sent messages will be available to the other
         * simulator after the timeStepStart call. */
        while (!events.empty() && events.front().first <= time_granted) {
            /* We have an event scheduled and it's now time to send it.
             * Pull it off the event queue and send it. */
            Message *message = events.front().second;
            time_current = events.front().first;
            cout << "NetSim: event detected at " << time_current << endl;
            events.pop_front();
            /* send it on to the actual, non-shadow object! */
            simObj2->send(message);
        }
        time_current = time_granted;

        /* Tell FNCS we are processing the current time step. */
        Integrator::timeStepStart(time_current);
        cout << "NetSim: Working. Time is " << time_current << endl;

        /* If we have more events, we can tell FNCS the time of the next
         * event (time_desired). Otherwise, we participate in the
         * synchronization call but our vote of Infinity essentially
         * doesn't affect the outcome. 'Infinity' means the simulator is
         * in a steady state. */
        if (events.empty()) {
            time_desired = Infinity;
        }
        else {
            time_desired = events.front().first;
        }

        time_granted = Integrator::getNextTime(time_current, time_desired);

        cout << "NetSim: getNextTime"
            << " current=" << time_current
            << " desired=" << time_desired
            << " granted=" << time_granted << endl;
    }
    while (!Integrator::isFinished());
    cout << "NetSim done!" << endl;

    Integrator::stopIntegrator();
}


/* a simple main that calls our simulator function;
 * it is sometimes useful to capture and report exceptions but this
 * might be overkill for your particular needs */
int main(int argc, char **argv)
{
    try {
        cout << "starting network sim" << endl;
        network_simulator();
    } catch (const std::exception &e) {
        cout << e.what() << endl;
    }
    catch (const string &e) {
        cout << e << endl;
    }
    cout << "finished network sim" << endl;
    return 0;
}
```

We've already covered the boilerplate code and how to register
communicating entities in our last demo so we won't repeat it here --
only the new stuff.

### Message Callback Function
There are two ways to process incoming messages, either by polling for
new messages or by having FNCS use a registered callback function. Here
is the callback function from above.

```c++
/* callback when simObj1 shadow gets a message */
static void receive_message1()
{
    /* Just because FNCS will call this callback function to indicate
     * there are messages in the inbox, that doesn't mean they are
     * automatically processed. You must process all incoming messages
     * for this object. */
    while (simObj1->hasMoreMessages()) {
        Message *message = simObj1->getNextInboxMessage();
        /* Schedule when simObj2 should extract message from
         * simObj1 out of the network, 0 to 2 seconds later, randomly.
         * This is a very crude network simulator, randomly delaying
         * messages. */
        events.push_back(make_pair(time_current+rand()%2000000000, message));
        cout << "NetSim: simObj1 got message with timestamp "
            << message->getTime()
            << ", scheduled for "
            << events.back().first << endl;
    }
    /* Event-based simulators maintain a sorted event queue. */
    events.sort();
}
```

And here is how we register the callback function to FNCS.

```c++
/* register callback function for simObj1 for incoming messages */
simObj1->setMessageNotifier(CreateCallback(receive_message1));
```

Any time FNCS queues a message into the inbox of simObj1, it will call
the callback function. For a tick-based simulator like our power
simulator, this isn't terribly important since we can simply query our
objects for incoming messages while processing the next time step. For
event-based simulators like our network simulator, it is convenient to
treat messages as events in this callback manner.

### Configuration JSON File
You should already be familiar with the need for JSON configuration file
from our last demo.

```c++
Integrator::initIntegrator("sim_network.json", time_current);
```

The [sim_network.json](sim_network.json) config file is used by only the
network simulator to indicate that it registers to the broker as a
network simulator. Here is what the file recently looked like, but
please refer to the original file as linked above.
```json
{
    "interface" : "ZmqNetworkInterface",
    "broker" : "localhost",
    "simulator_type" : "communication_network",
    "synchronization_algorithm" : "CommunicatorSimulatorSyncalgo",
    "simulator_time_metric" : "nanoseconds",
    "packet_lost_period" : 51000000000
}
```

The only important new piece of information here compared to the
[sim_power.json](sim_power.json) file is that the "simulator_type" is
"communication_network". There can only be one network simulator
currently and it must tell the broker that it is the network sim. The
last difference is similar to the first in that the
"synchronization_algorithm" has changed to
"CommunicatorSimulatorSyncalgo". These changes are necessary because the
network simulator uses different code internally to manage time and
communication.

## Running the Co-Simulation
[back to contents](#table-of-contents)

We need to create a working directory for our co-simulation. Each
simulator software package will generate output files, as usual, to the
current working directory. In addition, each example simulator has
diagnostic output to standard output (the terminal). The simulators are
designed to locate files from the working directory, for example, as
inputs. Please use this current tutorial directory as a working
directory for the co-simulation run.

In this directory you will find only a few files since our simulators
are rather simple.  You will find the necessary JSON files that each
simulator needs to pass information to the FNCS library during
initialization.  There is also a handy script file for starting the
co-simulation [run.sh](run.sh).  All runtime files will be described in
detail next.

Assuming you have set up your environment correctly, perhaps having
sourced the FNCS_env.sh script, you should be able to run "make" without
issue. This will compile the two source files. To run this simple
example, the file [run.sh](run.sh) has been provided which will redirect
stdout from each simulator to its own file and will also launch the
"fncsbroker" application which is required to run the co-simulation.

If you execute the [run.sh](run.sh) file, the output from each simulator
should appear in its own file appropriately named. The output is rather
uninspiring, but you should at least notice that messages weren't
received out of order. The [run.sh](run.sh) file isn't anything special,
but it does at least execute the "fncsbroker" application with the
correct number of simulators expected (2). The total number of
simulators connecting to the broker is its only parameter.
