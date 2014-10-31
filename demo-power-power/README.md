# FNCS-Tutorial Two Power Simulators

Step-by-step guide for building and running FNCS with two small
power simulator applications.

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
[sim_powerA.cpp](sim_powerA.cpp) and [sim_powerB.cpp](sim_powerB.cpp),
as well as a [Makefile](Makefile) for compiling.

We have set up the co-sim to use two "power" simulators, namely "A" and
"B". Each sim contains a single object that wishes to communicate,
"simObjA" and "simObjB", respectively. "A" sends a message to "B" at
each time step. "B" receives the message and sends its own message back
to "A" in response. Each sim iterates for 10 time steps.

### A Complete Breakdown of our First Power Sim
Let's look at [sim_powerA.cpp](sim_powerA.cpp) in detail.  We've
attempted to insert comments step by step to make the file
self-documenting as a reference. You should start by reading the file
line by line. We will highlight important code for you in the next
section of this tutorial. Here is what the file recently looked like,
but please refer to the original file as linked above.

```c++
/* C++ STL */
#include <iostream>

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

/* the callback function that FNCS can use to query the curren time */
TIME getCurrentTime() {
    return time_current;
}

/* our simulator function, called by the main() function later */
static void generic_simulator()
{
    TIME time_granted=0; /* the time step FNCS has allowed us to process */
    TIME time_desired=0; /* the time step we would like to go to next */

    /* initialize FNCS integrator with configuration file and current time */
    Integrator::initIntegrator("sim_power.json", time_current);
    /* FNCS must be able to request the current time of the simulator */
    Integrator::setTimeCallBack(CreateCallback(getCurrentTime));
    /* register an object that wishes to communicate */
    ObjectCommInterface *simObjA = Integrator::getCommInterface("simObjA");
    /* it's required to tell FNCS when all registrations are complete */
    Integrator::finalizeRegistrations();

    /* unless FNCS detects another simulator terminates early using the
     * ::isFinished() function, this simulator will run from time step 0
     * to time step 9 */
    while (!Integrator::isFinished() && time_current < 10) {
        /* FNCS uses two-phase synchronization at each time step, once
         * at the beginning of the time step ("timeStepStart") and once
         * at the end ("getNextTime") */
        Integrator::timeStepStart(time_current);

        /* do useful work; check for incoming messages to the objects we
         * registered earlier */
        {
            cout << "PowerA: Working. Time is " << time_current << endl;

            /* Send empty (NULL, size 0) message from simObjA to simObjB. */
            Message *outgoing_message = new Message(
                    /* from */ "simObjA", 
                    /* to */ "simObjB",
                    /* timeStamp */ time_current,
                    /* buffer */ NULL,
                    /* buffer size */ 0,
                    /* tag value, defaults to 0 */ 0);

            /* By default, all messages are 'routed' through
             * the network simulator unless the 'delayThroughComm'
             * attribute is set to false. */
            outgoing_message->setDelayThroughComm(false);

            /* actually send the message */
            simObjA->send(outgoing_message);
            cout << "PowerA: sent message from simObjA to simObjB" << endl;

            /* Check for incoming messages. */
            while (simObjA->hasMoreMessages()) {
                Message *incoming_message = simObjA->getNextInboxMessage();
                cout << "PowerA: simObjA received message from "
                    << incoming_message->getFrom()
                    << " with timestamp "
                    << incoming_message->getTime() << endl;
                delete incoming_message;
            }
            /* Which time step do we wish to go to next? This does not
             * necessarily need to be a delta of 1; FNCS supports
             * arbitrary time deltas. */
            time_desired = time_current + 1;

            /* No need to delete outgoing message object;
             * FNCS takes ownership. */
        }

        /* Second phase of two-phase synchronization by requesting the
         * next time step. The next time could very well be smaller
         * than this simulator is able to process i.e. the network
         * simulator runs in nanoseconds while this simulator runs in
         * seconds. In such a case, the time_granted will be the same as
         * time_current. Another possibility is that the time_granted is
         * less than time_desired due to another simulator requesting
         * a smaller time delta. */
        time_granted = Integrator::getNextTime(time_current, time_desired);

        cout << "PowerA: getNextTime"
            << " current=" << time_current
            << " desired=" << time_desired
            << " granted=" << time_granted << endl;

        time_current = time_granted;
    }
    cout << "PowerA: DONE!" << endl;

    Integrator::stopIntegrator();
}


/* a simple main that calls our simulator function;
 * it is sometimes useful to capture and report exceptions but this
 * might be overkill for your particular needs */
int main(int argc, char **argv)
{
    try {
        cout << "starting generic simulator" << endl;
        generic_simulator();
    } catch (const exception &e) {
        cout << e.what() << endl;
    }
    catch (const string &e) {
        cout << e << endl;
    }
    cout << "finished generic simulator" << endl;
    return 0;
}
```

#### Boilerplate Setup and Teardown
You'll notice there is quite a bit of "boilerplate" code -- code
necessary for simply setting up the simulator and its FNCS connection.
All simulators need the following:
- A way to tell current simulator time
- A callback function that FNCS can use to query the current time
- Integrator::initIntegrator(config_file_name, current_time)
- A loop to process time steps
  - Integrator::timeStepStart(current_time)
  - Integrator::getNextTime(current_time, desired_time)
- Integrator::stopIntegrator()

Those are the necessities -- you don't even have to send Messages --
your simulator could simply keep in synch with the other simulators but
otherwise hang out on its own and never communicate (but that's not
terribly useful.)

#### Communicating Entities
Before exchanging information between simulators, you must register the
thing you want to communicate with. Meaning, there is some named entity
that will send and receive messages. This could be the name of the
simulator itself such that you only have one "thing" that communicates
(the simulator itself), or you could have many "things" that communicate
which is what we have done for this particular demo. The important lines
of code from the above program were:

```c++
/* register an object that wishes to communicate */
ObjectCommInterface *simObjA = Integrator::getCommInterface("simObjA");
/* it's required to tell FNCS when all registrations are complete */
Integrator::finalizeRegistrations();
```

You see above that we name an entity "simObjA" and get back its
communicator interface. The ObjectCommInterface contains methods to send
and receive messages. Once we have registered all of our named
communicating entities, we must tell FNCS we are finished with
registrations. We only registered a single object, but in our larger
demos we register hundreds of objects.

#### The Message Class
The Message class must be used to exchange information between
simulators.

```c++
Message(const string &from,     /* the name of the sender */
        const string &to,       /* the name of the receiver */
        TIME timeStamp,         /* the time is it currently */
        const uint8_t *data,    /* message body, serialized */
        uint32_t dataSize,      /* message body size */
        uint8_t tag=0);         /* give the message a special tag, if needed */
```
Since FNCS was designed to have a network simulator
as part of the co-sim, Message instances by default would be routed
through the network simulator. Since this demo does not contain a
network simulator, we must disable this automatic behavior by explicitly
setting an attribute of the Message to false.

```c++
outgoing_message->setDelayThroughComm(false);
```

In our next demo, we do not reset the above attribute.

#### Configuration JSON File
As if there weren't enough boilerplate code already, there used to be
more. We tried to hide the intricate setup details by using a
configuration file that is read during initialization, for example

```c++
Integrator::initIntegrator("sim_power.json", time_current);
```

The [sim_power.json](sim_power.json) config file is used by both
simulators to indicate that these simulators are registered to the
broker as power simulators. Here is what the file recently looked like,
but please refer to the original file as linked above.
```json
{
    "interface" : "ZmqNetworkInterface",
    "broker" : "localhost",
    "simulator_type" : "power_grid",
    "synchronization_algorithm" : "GracePeriodSyncAlgo",
    "sync_params" : {
        "number_of_power_grid_sims" : 2
    },
    "simulator_time_metric" : "seconds",
    "packet_lost_period" : 2300000000
}
```
One important bit to glean from this file is the location where the
"broker" will be running. In our case its running locally, but you could
start the broker on another server so long as you tell the simulators
where it is located. Another important parameter is the
"simulator_time_metric" which tells the broker which unit of time the
simulator uses. This becomes important as you add simulators with
different units of time. Also note that we are declaring this
"simulator_type" as "power_grid" -- we have either "power_grid" or
"communication_network" simulator types. The "packet_lost_period" is
only relevant when you have a network simulator as part of the co-sim.
The other parameters were customizable at one point in time but are now
mostly vestiges of this being research code and should be copied
verbatim, e.g., "interface", "sync_params", "number_of_power_grid_sims".
The "synchronization_algorithm" will be talked about in the large demo
later on.

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

## FAQ/Troubleshooting
Q: What happens when you specify 3 to the fncsbroker instead of 2?

A: The co-sim will hang, as it is expecting another simulator to connect.

Q: What happens when you specify 3 to the fncsbroker and simply execute
another instance of either sim_powerA or sim_powerB to be the third
simulator?

A: A registration error will occur. All communicator objects must be
uniquely named across the entire co-simulation.
