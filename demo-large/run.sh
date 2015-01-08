#!/bin/sh

# use a fresh custom loader path
unset LD_LIBRARY_PATH

# shortcut
export FNCS_INSTALL="$HOME/FNCS-install"

# update LD_LIBRARY_PATH for all but MCR
if test "x$LD_LIBRARY_PATH" = x
then
    export LD_LIBRARY_PATH="$FNCS_INSTALL/lib"
else
    export LD_LIBRARY_PATH="$FNCS_INSTALL/lib:$LD_LIBRARY_PATH"
fi
# By this point our custom LD_LIBRARY_PATH exists, no need for further
# checks.

# update PATH for all but MCR
if test "x$PATH" = x
then
    export PATH="$FNCS_INSTALL/bin"
else
    export PATH="$FNCS_INSTALL/bin:$PATH"
fi

# MCR installs older libstdcxx, so we must locate the default loader
# search paths and append them to our LD_LIBRARY_PATH.
LD_DIRS=`ldconfig -v 2>/dev/null | grep ^/ | sed 's/:.*$//'`
for dir in $LD_DIRS
do
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$dir"
done

# update LD_LIBRARY_PATH for MCR
# MCR installs older libstdcxx, so append rather than prepend path.
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$FNCS_INSTALL/MATLAB/MATLAB_Compiler_Runtime/v81/runtime/glnxa64:$FNCS_INSTALL/MATLAB/MATLAB_Compiler_Runtime/v81/bin/glnxa64:$FNCS_INSTALL/MATLAB/MATLAB_Compiler_Runtime/v81/sys/os/glnxa64:$FNCS_INSTALL/MATLAB/MATLAB_Compiler_Runtime/v81/sys/java/jre/glnxa64/jre/lib/amd64/native_threads:$FNCS_INSTALL/MATLAB/MATLAB_Compiler_Runtime/v81/sys/java/jre/glnxa64/jre/lib/amd64/server:$FNCS_INSTALL/MATLAB/MATLAB_Compiler_Runtime/v81/sys/java/jre/glnxa64/jre/lib/amd64"

# Next, set the XAPPLRESDIR environment variable to the following value:
export XAPPLRESDIR="$FNCS_INSTALL/MATLAB/MATLAB_Compiler_Runtime/v81/X11/app-defaults
"
# run ns3 in separate window
xterm -e ./firstN LinkModel_CASE9_3_feeders.txt &

# run gld instance 1 in separate window
xterm -e gridlabd Run_CASE9_NS3_S1_B7_H250_2days.glm &

# run gld instance 2 in separate window
xterm -e gridlabd Run_CASE9_NS3_S2_B5_H150_2days.glm &

# run gld instance 3 in separate window
xterm -e gridlabd Run_CASE9_NS3_S3_B9_H200_2days.glm &

# run MATPOWER OPF in separate window
xterm -e start_MATPOWER case9_3subst_NS3.m real_power_demand_case9_T.txt &

# run fncsbroker in separate window
xterm -e fncsbroker 5 &

echo "running"
