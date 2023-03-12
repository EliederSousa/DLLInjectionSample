* Compile _injector/injector.cpp_: 
> g++ ./injector.cpp -o injector.exe


* Compile _main.cpp_:
> g++ main.cpp -shared -Os -O3 -I.\\includes -L.\\libs -m64 -w -W -mwindows -lmingw32 -lMinHook -o inject.dll


* Run test.exe:
  

* Run injector.exe (inject.dll must be in the same dir):


Profit