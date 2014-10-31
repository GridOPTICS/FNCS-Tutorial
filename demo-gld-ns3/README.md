# FNCS-Tutorial

Step-by-step guide for building and running FNCS, ns-3, and GridLAB-D.

This guide will walk you through installing and running a preliminary
use case: running a single instance of GridLAB-D and realistically
delaying and routing messages through ns-3.

## Table of Contents
- [Hardware Requirements](#hardware-requirements)
- [Software and Dependencies](#software-and-dependencies)
  - [ZeroMQ Installation](#zeromq-installation)
  - [FNCS Installation](#fncs-installation)
  - [Xerces-C++ Installation](#xerces-c-installation)
  - [GridLAB-D Installation](#gridlab-d-installation)
  - [ns-3 Installation](#ns-3-installation)
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
- GridLAB-D (our version)
  - Xerces (3.1.1)
  - autoconf (2.63 or better)
  - automake (1.11 or better)
  - libtool (2.2.6b or better)
  - FNCS
- ns-3 (our version based on 3.19)
  - Python (for the waf installer)
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

# update LD_LIBRARY_PATH
if test "x$LD_LIBRARY_PATH" = x
then
    export LD_LIBRARY_PATH="$FNCS_INSTALL/lib"
else
    export LD_LIBRARY_PATH="$FNCS_INSTALL/lib:$LD_LIBRARY_PATH"
fi

# update PATH
if test "x$PATH" = x
then
    export PATH="$FNCS_INSTALL/bin"
else
    export PATH="$FNCS_INSTALL/bin:$PATH"
fi
```

## Model Description
[back to contents](#table-of-contents)

TODO

## Running the Co-Simulation
[back to contents](#table-of-contents)

The rest of this tutorial assumes that you have installed FNCS and our versions
of GridLAB-D and ns-3 i.e. all of the software mentioned above.

We will use the current directory of the tutorial as the working
directory for our co-simulation.  Each simulator software package will
generate output files, as usual, to the current working directory. In
addition, we have may have added own diagnostic output to standard
output (the terminal). The simulators are designed to locate files from
the working directory, for example, as inputs.

The current directory should contain all needed files for this
co-simulation demo.

- firstN.cc, the ns-3 model
- compile-ns3.sh, a helper script for compiling firstN.cc
- FIXME.glm, GridLAB-D model file


