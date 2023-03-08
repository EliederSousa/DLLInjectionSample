/** 
 *  inject.cpp
 *  
 *  Copyright (c) 2023, Elieder Sousa
 *  eliedersousa<at>gmail<dot>com
 *  
 *  Distributed under the MIT license.
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *  
 *  @date 07/03/23
 *  
 *  @brief  DLL a ser injetada no processo. Compile com:
 *          g++ inject.cpp -shared -mwindows -lmingw32 -o inject.dll
 *  
 */ 
#include <Windows.h>
#include <iostream>

void getConsoleOutput() {
    AllocConsole();
    FILE *f = new FILE();
    freopen_s(&f, "CONOUT$", "w", stdout);
    std::cout.clear();
}

DWORD WINAPI MainThread( LPVOID param ) {
    std::cout << "Oi! Sou uma dll injetada :)\n";
    return 0;
}

bool APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved ) {
    if ( dwReason == DLL_PROCESS_ATTACH ) {
        CreateThread( 0, 0, MainThread, hModule, 0, 0 );
    }
    return true;
}