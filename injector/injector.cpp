/** 
 *  injector.cpp
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
 *  @brief Injects a DLL into a process. References:
 *      https://www.youtube.com/watch?v=KCtLiBnlpk4
 *      https://cocomelonc.github.io/tutorial/2021/09/20/malware-injection-2.html
 *  
 */

#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

/**
 * @brief Returns the pID of a target process. 
 *
 * @param target char* - The name of process (something like "main.exe")
 *
 * @return int The pID of the process.
 */
int getProcId( char* target ) {
    DWORD pID = 0;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof( PROCESSENTRY32 );
    HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    do {
        // Compares the name of the process with passed argument; to get it's pID
        if ( strcmp((char*)pe32.szExeFile, target) == 0) {
            CloseHandle( hSnapshot );
            pID = pe32.th32ProcessID;
            break;
        }
    } while( Process32Next( hSnapshot, &pe32 ) );
    CloseHandle( hSnapshot );
    return pID;
}

int main( int argc, char* argv[] ) {
    char* process = "test.exe";         // The process to be injected
    int pID = getProcId( process );     // Let's take the pID of it
    
    if ( pID == 0 ) {
        std::cout << "Error: Target process is not running!" << std::endl;
        return 0;
    } else {
        std::cout << process <<  " is running with pID " << pID << std::endl;
    }

    char dll[] = "inject.dll";      // Path to the DLL to inject
    char dllPath[MAX_PATH] = {0};   // FullPath of the DLL
    GetFullPathNameA( dll, MAX_PATH, dllPath, NULL );

    HANDLE hProcess = OpenProcess( PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pID );
    if( !hProcess ) {
        std::cout << "Error when opening the process." << std::endl;
        return 0;
    }

    LPVOID pszLibFileRemote = VirtualAllocEx( hProcess, NULL, strlen( dllPath ) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );
    
    if( !pszLibFileRemote ) {
        std::cout << "Error: couldn't alloc memory to the thread." << std::endl;
        return 0;
    } else {
        std::cout << "Memory allocated at " << pszLibFileRemote << std::endl;
    }
    
    // Writing the name of DLL in the target process and creates a remote thread to be started.
    bool isWriteOK = WriteProcessMemory( hProcess, pszLibFileRemote, dllPath, strlen( dllPath ) + 1, NULL );
    if (!isWriteOK ) {
        std::cout << "Error: couldn't possible to write in process memory." << std::endl;
        return 0;
    }

    HANDLE handleThread = CreateRemoteThread( hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, pszLibFileRemote, 0, NULL);

    if ( !handleThread ) {
        std::cout << "Error: Remote thread could not be created!" << std::endl;
        std::cout << GetLastError();
        return 0;
    } else {
        std::cout << "Created thread." << std::endl;
    }

    WaitForSingleObject( handleThread, INFINITE );
    CloseHandle( handleThread );
    VirtualFreeEx( hProcess, dllPath, 0, MEM_RELEASE );
    CloseHandle( hProcess );

    return 0;
}
