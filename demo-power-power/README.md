FNCS-Tutorial Two Power Simulators
==================================

Step-by-step guide for building and running FNCS with two small
power simulator applications.

Table of Contents
-----------------
- Hardware Requirements
- Software and Dependencies
  - ZeroMQ Installation
  - Framework for Network Co-Simulation (FNCS) Installation
- Important Environment Variables
- Running the Co-Simulation

Hardware Requirements
---------------------
This software was developed and tested exclusively using the Linux
operating system.  Our developers have used RedHat EL5 as well as Ubuntu
12.04.5 LTS (precise), both of which were 64-bit systems. This software
has not been built or tested on Windows. Although all of the software
packages and their dependencies are known to have Windows installers, we
have not yet developed any way to patch those official distributions
with FNCS enhancements.

Software and Dependencies
-------------------------
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

ZeroMQ Installation
-------------------
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

Framework for Network Co-Simulation (FNCS) Installation
-------------------------------------------------------
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

Important Environment Variables
-------------------------------
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

Running the Co-Simulation
-------------------------
We need to create a working directory for our co-simulation. Each
simulator software package will generate output files, as usual, to the
current working directory. In addition, each example simulator has
diagnostic output to standard output (the terminal). The simulators are
designed to locate files from the working directory, for example, as
inputs. 

Feel free to use this current tutorial directory as a working directory
for the co-simulation run.  In this current tutorial directory you will
find two source files, [sim_powerA.cpp](sim_powerA.cpp) and
[sim_powerB.cpp](sim_powerB.cpp), as well as a [Makefile](Makefile) for
compiling.  You will also find the necessary JSON files that each
simulator needs to pass information to the FNCS library during
initialization.  There is also a handy script file for starting the
co-simulation.  All files will be described in detail next.

Assuming you have set up your environment correctly, perhaps having
sourced the FNCS_env.sh script, you should be able to run "make" without
issue. This will compile the two source files. To run this simple
example, the file [run.sh](run.sh) has been provided which will redirect
stdout from each simulator to its own file and will also launch the
"fncsbroker" application which is required to run the co-simulation.

Model Description
-----------------
We have set up the co-sim to use two "power" simulators, namely "A" and
"B". Each sim contains a single object that wishes to communicate,
"simObjA" and "simObjB", respectively. "A" sends a message to "B" at
each time step. "B" receives the message and sends its own message back
to "A" in response. Since FNCS was designed to have a network simulator
as part of the co-sim, Message instances by default would be routed
through the network simulator. Since this demo does not contain a
network simulator, we must disable this automatic behavior by explicitly
setting an attribute of the Message to false.

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

If you execute the [run.sh](run.sh) file, the output from each simulator
should appear in its own file appropriately named. The output is rather
uninspiring, but you should at least notice that messages weren't
received out of order. The [run.sh](run.sh) file isn't anything special,
but it does at least execute the "fncsbroker" application with the
correct number of simulators expected (2). The total number of
simulators connecting to the broker is its only parameter.

Q: What happens when you specify 3 to the fncsbroker instead of 2?
A: The co-sim will hang, as it is expecting another simulator to connect.

Q: What happens when you specify 3 to the fncsbroker and simply execute
another instance of either sim_powerA or sim_powerB to be the third
simulator?
A: A registration error will occur. All communicator objects must be
uniquely named across the entire co-simulation.
