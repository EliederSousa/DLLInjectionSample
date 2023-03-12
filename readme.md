## DLLInjectionSample  
This is a simple project to create an game hacking enviroment. We will use [MinHook](https://github.com/TsudaKageyu/minhook) to help hooking/detouring functions.


* Compile the injector file (_injector/injector.cpp_): 
> g++ ./injector.cpp -Os -O3 -o injector.exe  

Beware that antivirus could block execution, or delete the executable. The hooking method is very simple so we need to refine this.  
  
    

* Compile the DLL (_main.cpp_ in the root):
> g++ main.cpp -shared -Os -O3 -I.\\includes -L.\\libs -m64 -w -W -mwindows -lmingw32 -lMinHook -o inject.dll  


* Run test.exe:  
![image](https://user-images.githubusercontent.com/16262291/224552531-2d9608fe-13f5-46fe-9c90-eae0c1059499.png)


* Run injector.exe (inject.dll must be in the same dir):  
![image](https://user-images.githubusercontent.com/16262291/224552958-064a2174-2855-4f9d-a52f-f94fcb71efe1.png)



Profit
