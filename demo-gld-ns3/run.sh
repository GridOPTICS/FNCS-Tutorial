#!/bin/sh

PWD=`pwd`

# load env vars
. $PWD/FNCS_env.sh

# run ns3 in separate window
xterm -e ./firstN LinkModelGLDNS3.txt &

# run gld in separate window
xterm -e gridlabd fncs_GLD_300node_Feeder_1.glm &

# run fncsbroker in separate window
xterm -e fncsbroker 2 &

echo "running"
