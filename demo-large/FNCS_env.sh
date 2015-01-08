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
