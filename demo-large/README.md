# FNCS-Tutorial

Step-by-step guide for building and running FNCS, ns-3, GridLAB-D, and
MATPOWER.

This guide will walk you through installing and running our primary use
case: running multiple instances of GridLAB-D coupled to MATPOWER and
realistically delaying and routing inter-simulator messages through
ns-3.

## Table of Contents
- [Hardware Requirements](#hardware-requirements)
- [Software and Dependencies](#software-and-dependencies)
  - [ZeroMQ Installation](#zeromq-installation)
  - [FNCS Installation](#fncs-installation)
  - [Xerces-C++ Installation](#xerces-c-installation)
  - [GridLAB-D Installation](#gridlab-d-installation)
  - [ns-3 Installation](#ns-3-installation)
  - [MATPOWER Installation](#matpower-installation)
- [Important Environment Variables](#important-environment-variables)
- [Model Description](#model-description)
  - [GridLAB-D Models](#gridlab-d-models)
  - [ns-3 Model](#ns-3-model)
  - [MATPOWER-OPF Model](#matpower-opf-model)
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
- GridLAB-D (our version)
  - Xerces (3.1.1)
  - autoconf (2.63 or better)
  - automake (1.11 or better)
  - libtool (2.2.6b or better)
  - FNCS
- ns-3 (our version based on 3.19)
  - Python (for the waf installer)
  - FNCS
- MATPOWER (our version)
  - MATLAB Compiler Runtime (R2013a (8.1))
  - FNCS

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

## ZeroMQ Installation
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

## FNCS Installation
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

## Xerces-C++ Installation
[back to contents](#table-of-contents)

http://xerces.apache.org/xerces-c/

```
# we are doing everything from your $HOME directory
cd $HOME

# download Xerces-C++ 3.1.1 source code
wget http://apache.mirrors.pair.com//xerces/c/3/sources/xerces-c-3.1.1.tar.gz
# if you do not have wget, use
# curl -O http://apache.mirrors.pair.com//xerces/c/3/sources/xerces-c-3.1.1.tar.gz

# unpack xerces, change to its directory
tar -xzf xerces-c-3.1.1.tar.gz
cd xerces-c-3.1.1

# configure, make, and make install 
./configure --prefix=$FNCS_INSTALL
make
make install
```

Congratulations, you have now installed Xerces-C++ and are ready to
install GridLAB-D.

## GridLAB-D Installation
[back to contents](#table-of-contents)

http://www.gridlabd.org/

GridLAB-D is a power distribution system simulator and analysis tool.
Please see its website for complete details.

We branched from its trunk development at r4534 and patched in our FNCS
additions. Please use our FNCS-ified version of GridLAB-D.

Get our version of the GridLAB-D software and install it using the
following steps:

```
# we are doing everything from your $HOME directory
cd $HOME

# download our FNCS version of GridLAB-D
git clone https://github.com/GridOPTICS/FNCS-gridlab-d

# change to FNCS-gridlab-d directory
cd FNCS-gridlab-d

# run to autotools to generate the configure script and Makefile.in
# templates
# minimum required versions:
# autoconf 2.63
# automake 1.11
# libtool 2.2.6b
autoreconf -fi

# configure, make, and make install 
./configure --prefix=$FNCS_INSTALL --with-xerces=$FNCS_INSTALL --with-fncs=$FNCS_INSTALL
make
make install
```

Congratulations, you have now installed GridLAB-D, at least, our version
of it.

## ns-3 Installation
[back to contents](#table-of-contents)

http://www.nsnam.org/

ns-3 is a discrete-event network simulator for Internet systems. Please
see their website for complete details.

We added a FNCS "application" as a patch to ns-3.19. Our application
receives FNCS messages from GridLAB-D and injects them into the network,
and once through the network (if not dropped), sends the FNCS message on
to its intended recipient.

Get our version of the ns-3 software and install it using the following
steps:

```
# we are doing everything from your $HOME directory
cd $HOME

# download our FNCS version of ns-3
git clone https://github.com/GridOPTICS/FNCS-ns-allinone-3.19

# change to FNCS-ns-allinone-3.19 directory
cd FNCS-ns-allinone-3.19

# the "allinone" package contains ns-3 utilities we do not use;
# we begin our install from the ns-3.19 directory
cd ns-3.19

# the ns-3 install typically uses the compiler flag for
# warnings-as-errors which often broke our ability to build and install
# it, so we recommend the following configure of ns-3
CFLAGS="-g -O2" CXXFLAGS="-g -O2" ./waf configure --prefix=$FNCS_INSTALL --with-fncs=$FNCS_INSTALL --with-zmq=$FNCS_INSTALL --disable-python

# 'make'
./waf build

# insatll
./waf install
```

Congratulations, you have now installed ns-3, at least, our version
of it.

## MATPOWER Installation
[back to contents](#table-of-contents)

http://www.pserc.cornell.edu/matpower/

MATPOWER is a package of MATLAB M-files for solving power flow and
optimal power flow problems.

We have adapted the optimal power flow (OPF) capabilities of the
MATPOWER software for our own needs. In addition, rather than requiring
a MATLAB license for end users, we chose to compile the MATLAB M-files.
By compiling the M-files, we now require the use of the MATLAB Compiler
Runtime (MCR) which does not require a MATLAB license. Unfortunately,
you must use the MCR version specific to the version we used when
compiling the M-files.

Please follow the instructions on the the MCR website for details on
installing MCR version R2013a (8.1).

http://www.mathworks.com/products/compiler/mcr/

The steps here reflect the pattern we have already established of
installing all softare to $HOME.

```
# we are doing everything from your $HOME directory
cd $HOME

# download MCR
wget http://www.mathworks.com/supportfiles/MCR_Runtime/R2013a/MCR_R2013a_glnxa64_installer.zip
# if you do not have wget, use
# curl -O http://www.mathworks.com/supportfiles/MCR_Runtime/R2013a/MCR_R2013a_glnxa64_installer.zip

# the MCR zip file does not unzip into a parent folder, so create one
mkdir MCR_R2013a_glnxa64_installer
cd MCR_R2013a_glnxa64_installer

# unpack MCR
unzip ../MCR_R2013a_glnxa64_installer.zip

# run the Java-based GUI installer
# NOTE: We installed to $FNCS_INSTALL/MATLAB/MATLAB_Compiler_Runtime
./install
```

Congratulations, you have now installed MCR 2013a (8.1).

Please note the important message from the installer about how to set
your LD_LIBRARY_PATH environment variable. See the next section for a
recreation of the message.

Now you need to download our MATPOWER OPF sources and build the
executable:

```
# we are doing everything from your $HOME directory
cd $HOME

# download FNCS
git clone https://github.com/GridOPTICS/FNCS-MATPOWER-OPF

# change to the FNCS-MATPOWER-OPF/Sources directory
cd FNCS-MATPOWER-OPF/Sources

# a custom Makefile is used for this software
# if the build fails, you may need to edit the top of the file
# to change the MCR path or FNCS_INSTALL path, as needed
# or the install path of the software currently being built
make
make install
```

Congratulations, you have now installed our custom MATPOWER OPF binary.

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

# update LD_LIBRARY_PATH for MCR
export LD_LIBRARY_PATH="$FNCS_INSTALL/MATLAB/MATLAB_Compiler_Runtime/v81/runtime/glnxa64:$FNCS_INSTALL/MATLAB/MATLAB_Compiler_Runtime/v81/bin/glnxa64:$FNCS_INSTALL/MATLAB/MATLAB_Compiler_Runtime/v81/sys/os/glnxa64:$FNCS_INSTALL/MATLAB/MATLAB_Compiler_Runtime/v81/sys/java/jre/glnxa64/jre/lib/amd64/native_threads:$FNCS_INSTALL/MATLAB/MATLAB_Compiler_Runtime/v81/sys/java/jre/glnxa64/jre/lib/amd64/server:$FNCS_INSTALL/MATLAB/MATLAB_Compiler_Runtime/v81/sys/java/jre/glnxa64/jre/lib/amd64:$LD_LIBRARY_PATH"

# Next, set the XAPPLRESDIR environment variable to the following value:
export XAPPLRESDIR="$FNCS_INSTALL/MATLAB/MATLAB_Compiler_Runtime/v81/X11/app-defaults
"
```

## Model Description
[back to contents](#table-of-contents)

In this current directory you will find many files, the vast majority of
them are input files for GridLAB-D.

- Tutorial files
    - FNCS_env.sh -- source this file to set up your environment variables
    - README.md -- this file
    - run.sh -- helpful script to run the demo
- GridLAB-D files
    - AEP_RT_LMP.player
    - appliance_schedules.glm
    - cap_ref_obj.glm
    - ColumbusWeather2009_2a.csv
    - phase_A.player
    - phase_B.player
    - phase_C.player
    - Run_CASE9_NS3_S1_B7_H250_2days.glm
    - Run_CASE9_NS3_S2_B5_H150_2days.glm
    - Run_CASE9_NS3_S3_B9_H200_2days.glm
    - tzinfo.txt
    - unitfile.txt
    - water_and_setpoint_schedule_v3.glm
    - configgld.json -- FNCS config file for ns-3
- ns-3 files
    - compile-ns3.sh -- our helper script for compiling ns-3 model
    - firstN.cc -- the ns-3 model source file
    - LinkModel_CASE9_3_feeders.txt -- input to compiled ns-3 model
    - configns3.json -- FNCS config file for ns-3
- MATPOWER-OPF files
    - case9_3subst_NS3.m
    - real_power_demand_case9_T.txt
    - configpowerflow.json -- FNCS config file for MATPOWER-OPF

### GridLAB-D Models
[back to contents](#table-of-contents)

Our GridLAB-D models consists of 600 houses total across three separate
model files of 250, 150, and 200 houses. Some of the houses
participate in a transactive market where they send their
(price,quantity) bids to an auction house. Each GridLAB-D model has its
own disjoint auction house. At each auction, the bids are collected,
sorted, and from them a price signal is broadcast back out to the
participating homes. The bids as well as the price signal are
communicated through the single ns-3 model to realistically delay the
messages.

To get GridLAB-D to work with FNCS, we needed to modify how it processes
time. For details, see the core/exec.c and core/main.c files in our
custom distribution of GridLAB-D. In order for houses to communicate
with the auction, we created a new GridLAB-D module called "comm", the
source code for which can be found in the communications directory can
in our custom GridLAB-D distribution. Within the comm module we created
the "market network interface" as well as the "controller network
interface" classes which create, send, and receive FNCS Messages between
them.

Figuring out how to control time within GridLAB-D was challenging,
however the exchange of messages between entities within GridLAB-D is
really no more complicated than our first simple "power+power" demo in
these tutorial pages.

### ns-3 Model
[back to contents](#table-of-contents)

Our ns-3 model [firstN.cc](firstN.cc) creates 600 nodes within the
network in groups of 20. We use the CSMA model of ns-3 in order to set a
"DataRate" as well as a "Delay". ns-3 is its own feature-rich network
modeling simulator, so our simple model should not be considered the
only solution. We give each node an IP address and later map the IP
addresses to names given to each house from the GridLAB-D model. The
GridLAB-D house names follow a regular pattern which we exploit when
naming the nodes in our ns-3 network -- we only need to know the number
of houses in the GridLAB-D model and the name prefix for the houses. The
single input file
[LinkModel_CASE9_3_feeders.txt](LinkModel_CASE9_3_feeders.txt) provides
this information to our ns-3 model. The three GridLAB-D models share a
common ns-3 model, representing a shared network although the GridLAB-D
models use disjoint auctions.

To get ns-3 to work with FNCS, we created a new ns-3 FNCS "application".
Applications are associated with individual nodes. Each node holds a
list of references to its applications. In other words, a node in ns-3
on its own doesn't do anything, rather it must have one or more
applications running on the node. Our FNCS application listens for FNCS
messages. Upon receiving a FNCS message, a FNCS application running at a
node injects the message into the simulated network at its node. The
ns-3 model routes the message appropriately, realistically delaying its
transmission, and then the FNCS application instance running on the
destination node in the network reads the message once it arrives and
sends it back to the FNCS broker so it can be sent back to the
destination simulator.

It may sound complicated, but it's really not so different from our
simple network simulator from the second tutorial.

### MATPOWER-OPF Model
[back to contents](#table-of-contents)

We use the optimal power flow portion of MATPOWER to implement a
transmission simulator. Each of the three GridLAB-D instances model a
separate power distribution network, but all three of them are attached
to different buses in our tranmission simulator. Thus, when the power
demand changes in one of the GridLAB-D instances, the other GridLAB-D
instances are potentially affected once the tranmission simulator
recalculates. In this way, the seemingly disjoint GridLAB-D instances
will interact with each other indirectly.

## Running the Co-Simulation
[back to contents](#table-of-contents)

The rest of this tutorial assumes that you have installed FNCS and our
versions of GridLAB-D, ns-3, and MATPOWER i.e. all of the software
mentioned above.

We will use the current directory of the tutorial as the working
directory for our co-simulation.  Each simulator software package will
generate output files, as usual, to the current working directory. In
addition, we have may have added own diagnostic output to standard
output (the terminal). The simulators are designed to locate files from
the working directory, for example, as inputs.

If you have sourced the FNCS_env.sh file and installed the softare as we
had indicated, you should be ready to compile the ns-3 model and run the
demo. Start by compiling the ns-3 model.

```bash
./compile-ns.sh firstN.cc
```

If we didn't already have a handy script for you this time around to run
the demo, instead you would need to manually open up six terminal
windows, one for running the fncsbroker, one for each GridLAB-D, one for
ns-3, and the last for MATPOWER. If you're wanting to run the demo that
way, do the following steps.

In the first window, from this tutorial directory, run GridLAB-D and
specify the first GLM model file.

```bash
gridlabd Run_CASE9_NS3_S1_B7_H250_2days.glm
```

In the second window, from this tutorial directory, run GridLAB-D and
specify the second GLM model file.

```bash
gridlabd Run_CASE9_NS3_S2_B5_H150_2days.glm
```

In the third window, from this tutorial directory, run GridLAB-D and
specify the third GLM model file.

```bash
gridlabd Run_CASE9_NS3_S3_B9_H200_2days.glm
```

In the fourth window, from this tutorial directory, run the compiled
ns-3 model and specify its model file.

```bash
./firstN LinkModel_CASE9_3_feeders.txt
```

In the fifth window, from this tutorial directory, run the compiled
MATPOWER application which was installed earlier and specify its two
input files in the following order.

```bash
start_MATPOWER case9_3subst_NS3.m real_power_demand_case9_T.txt
```

In the sixth window, from this tutorial directory, run fncsbroker. The
command-line argument '5' indicates that five simulators will be
connecting to the broker.
```bash
fncsbroker 5
```

As mentioned above, we have a useful script for running all of the
simulators and the broker in separate windows. This assumes you have
followed the installation process exactly as documented. No need to
source the FNCS_env.sh file because it is embedded into the run.sh
script. The run.sh script will set up your environment for you and then
run 'xterm' once for each of the simulators and the broker. xterm
instances should start appearing on your desktop. If you only see a
subset of the xterm windows, chances are they are overlapping so you
will need to drag them around your desktop to reveal hidden ones.

In either the terminals you manually created or in the ones created for
you by the run.sh script, you should start seeing (in addition to the
usual output from any of the simulators like GridLAB-D) our diagnostic
messages coming from each simulator (note that fncsbroker is silent).

Once one of the simulators reaches the end of their simulated time, or
if you manually terminate any of them using Ctrl+C, all of the
simulators should stop. If you have used the run.sh script, this should
also automatically close the xterm sessions.

What you do with the output from this co-simulation is really up to you,
the modeler. You could experiment by setting longer delays to our ns-3
model, or better yet create your own ns-3 model that is more
complicated or uses a different network protocol. You could use a larger
transmission model and attach additional GridLAB-D instances, or
increase the number of houses in the GridLAB-D instances and adjust the
inputs to the ns-3 model appropriately.
