#!/bin/sh

# use a fresh custom loader path
unset LD_LIBRARY_PATH

# shortcut
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

# run ns3 in separate window
xterm -e ./firstN LinkModelGLDNS3.txt &

# run gld in separate window
xterm -e gridlabd fncs_GLD_300node_Feeder_1.glm &

# run fncsbroker in separate window
xterm -e fncsbroker 2 &

echo "running"
