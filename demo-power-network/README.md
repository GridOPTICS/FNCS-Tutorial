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
- [Running the Co-Simulation](#running-the-co-simulation)
- [Model Description](#model-description)

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
you will find the file [here](demo-power-network/FNCS_env.sh).

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

## Running the Co-Simulation
[back to contents](#table-of-contents)

We need to create a working directory for our co-simulation. Each
simulator software package will generate output files, as usual, to the
current working directory. In addition, each example simulator has
diagnostic output to standard output (the terminal). The simulators are
designed to locate files from the working directory, for example, as
inputs. 

Feel free to use this current tutorial directory as a working directory
for the co-simulation run.  In this current tutorial directory you will
find two source files, [sim_power.cpp](demo-power-network/sim_power.cpp) and
[sim_network.cpp](demo-power-network/sim_network.cpp), as well as a
[Makefile](demo-power-network/Makefile) for compiling.  You will also find the
necessary JSON files that each simulator needs to pass information to
the FNCS library during initialization.  There is also a handy script
file for starting the co-simulation.  All files will be described in
detail next.

Assuming you have set up your environment correctly, perhaps having
sourced the FNCS_env.sh script, you should be able to run "make" without
issue. This will compile the two source files. To run this simple
example, the file [run.sh](demo-power-network/run.sh) has been provided which
will redirect stdout from each simulator to its own file and will also
launch the "fncsbroker" application which is required to run the
co-simulation.

## Model Description
[back to contents](#table-of-contents)

We have set up the co-sim to use a single "power" simulator and a single
"network" simulator. The power simulator contains two objects that wish
to send messages, "simObj1" and "simObj2". Those two objects are
shadowed by objects in the network simulator. In this demo, simObj1
sends to simObj2 only. When simObj1 sends a message to simObj2, it is
automatically sent from the power simulator to the network simulator
because by default FNCS assumes you want messages routed through a
network simulator. The "sim_network.json" config file indicates that the
network simulator is indeed registered to the FNCS broker as a network
simulator -- currently FNCS can support only one network simulator. Once
the network sim has scheduled the message to be delivered, it is sent on
to its original destination of simObj2 running within the power sim.

This demo shows two different types of simulators, tick-based simulators
and event-based simulators. The code is similar, but when checking for
incoming messages it is sometimes convenient to use the callback
capability for event-based sims instead of polling for new messages as
in the tick-based sim.
