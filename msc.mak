all: w32exec.exe

w32exec.exe: loader.obj winapi.obj dbgout.obj w32exec.obj
	cl loader.obj winapi.obj dbgout.obj w32exec.obj -Few32exec.exe

loader.obj: loader.c
	cl -c loader.c

winapi.obj: winapi.c
	cl -c winapi.c

dbgout.obj: dbgout.c
	cl -c dbgout.c

w32exec.obj: w32exec.c
	cl -c w32exec.c
