#include <Windows.h>
#include <string.h>
#include <iostream>
#include <variant>
#include <vector>

template <typename T>class MemoryPointer;
HMODULE getBaseAddress();
void protected_read(void*, void*, int);
template <typename T>void readBytes(MemoryPointer<T>);
template <typename T>void writeBytes(MemoryPointer<T>, void*);

/**
 * @brief Stores a pointer
 */
template <typename T> class MemoryPointer {
    public:
    DWORD_PTR offset;
    DWORD_PTR address;
    void* address_pointer;
    
    std::variant<char, int, float, double, long double> value;
    void* value_pointer;

    int size;

    public:
    /**
     * @brief Constructor - Creates a pointer.
     * @param DWORD_PTR address_param The address that we work with.
     */
    MemoryPointer() {
        offset = NULL;
        address = NULL;
        address_pointer = &address;
        value = (T)0;
        value_pointer = &value;
        size = 4;
    }
    
    /**
     * @brief Constructor - Creates a pointer.
     * @param address_param DWORD_PTR - The address that we work with.
     */
    MemoryPointer( DWORD_PTR address_param ) {
        offset = address_param;
        address = (DWORD_PTR)getBaseAddress() + address_param;
        address_pointer = &address;
        value = (T)0;
        size = sizeof(T);
        value_pointer = &value;
    }

    /**
     * @brief Retrive the value inside the pointer.
     * @return Template The value returned based on the type (template) of the pointer.
     */
    T getValue() {
        readBytes( *this );
        return std::get<T>(value);
    }

    /**
     * @brief Prints the address in hexadecimal format.
     * @return void
     */
    void printAddress() {
        printf( "%llx\n", address );
    }
};

class StringFromPointer {
    public:
    std::vector<MemoryPointer<char>*> str_vector;
    int size;
    std::string value;

    public:
    StringFromPointer() {}
    StringFromPointer( DWORD_PTR address_param, int size_param ) {
        for( int w=0;w<size_param;w++ ) {
            str_vector.push_back( new MemoryPointer<char>(address_param + w) );
        }
        size = size_param;
    }

    std::string getValue() {
        value = "";
        for (auto & char_value : str_vector) {
            if (((*char_value).getValue()) == 0) {
                continue;
            }
            value += (*char_value).getValue();
        }
        return value;
    }
};

/**
 * @brief Gets the base address of the running program.
 * @return HMODULE An address of the program. Use it to sum offsets in order to get your desired address.
 */
HMODULE getBaseAddress() {
    return GetModuleHandle( NULL );
}

/**
 * @brief Reads the value in specified address, storing the value.
 * 
 * @param dest void* - The variable to store the value.
 * @param src void* - The address to be readed.
 * @param size int - How many bytes will be readed. 
 */
void protected_read( void* dest, void* src, int size ) {
    DWORD oldProtect = 0;
    VirtualProtect( dest, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(dest, src, size);
    VirtualProtect( dest, size, oldProtect, &oldProtect );
}

/**
 * @brief Reads bytes using a MemoryPointer object.
 * 
 * @tparam T - Type of value stored in MemoryPointer.
 * @param p MemoryPointer<T> - The MemoryPointer object to be used.
 */
template <typename T> void readBytes( MemoryPointer<T> p ) {
    protected_read( p.value_pointer, (void*)p.address, p.size );
}

/**
 * @brief Write bytes in memory using a MemoryPointer.
 * @param pointer MemoryPointer<T> - A MemoryPointer object with the address to write the bytes.
 * @param patch void* - The bytes to write, passed as a pointer to void (no type)
 * @return void 
 */
template <typename T> void writeBytes( MemoryPointer<T> p, void* patch ) {
    protected_read( p.address_pointer, patch, p.size);
}