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
