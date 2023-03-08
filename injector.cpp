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
 *  @date 12/01/23
 *  
 *  @brief Injeta uma DLL em um processo. Referências:
 *      https://www.youtube.com/watch?v=KCtLiBnlpk4
 *      https://cocomelonc.github.io/tutorial/2021/09/20/malware-injection-2.html
 *  
 */

#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

int getProcId( char* target ) {
    DWORD pID = 0;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof( PROCESSENTRY32 );
    HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    do {
        // Compara o nome do processo para injetar com os existentes na lista de processos
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
    char* process = "main.exe";         // Processo para injetar
    int pID = getProcId( process );     // pID do processo
    
    if ( pID == 0 ) {
        std::cout << "Erro: Processo não está rodando!";
        return 0;
    } else {
        std::cout << process <<  " está executando com o pID: " << pID << std::endl;
    }

    char dll[] = "inject.dll";      // Nome da DLL para injetar
    char dllPath[MAX_PATH] = {0};   // FullPath da DLL
    GetFullPathNameA( dll, MAX_PATH, dllPath, NULL );

    // Abrindo um processo e alocando memória para a DLL
    HANDLE hProcess = OpenProcess( PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pID );
    if( !hProcess ) {
        std::cout << "Erro ao abrir o processo.";
        return 0;
    }

    LPVOID pszLibFileRemote = VirtualAllocEx( hProcess, NULL, strlen( dllPath ) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );

    if( !pszLibFileRemote ) {
        std::cout << "Erro: não foi possível alocar memória para a thread.";
        return 0;
    } else {
        std::cout << "Memória alocada em " << pszLibFileRemote << std::endl;
    }

    // Escrevendo o nome da DLL no processo alvo e criando uma remote thread para ela ser iniciada.
    bool isWriteOK = WriteProcessMemory( hProcess, pszLibFileRemote, dllPath, strlen( dllPath ) + 1, NULL );
    if (!isWriteOK ) {
        std::cout << "Erro: não foi possível escrever na memória do processo.";
        return 0;
    }

    HANDLE handleThread = CreateRemoteThread( hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, pszLibFileRemote, 0, NULL);

    if ( !handleThread ) {
        std::cout << "Erro: Thread remota não pode ser criada!" << std::endl;
        std::cout << GetLastError();
        return 0;
    } else {
        std::cout << "Thread criada." << std::endl;
    }

    WaitForSingleObject( handleThread, INFINITE );
    CloseHandle( handleThread );
    VirtualFreeEx( hProcess, dllPath, 0, MEM_RELEASE );
    CloseHandle( hProcess );

    return 0;
}