
#include <common/types.h>
#include <memorymanagement.h>

using namespace myos;
using namespace myos::common;

MemoryManager* MemoryManager::activeMemoryManager = 0;

MemoryManager::MemoryManager(myos::common::size_t start, myos::common::size_t size){
    activeMemoryManager = this;
    if (size < sizeof(MemoryChunk)){
        first = 0;
        return;
    }
    first = (MemoryChunk*)start;
    first -> allocated = false;
    first -> size = size - sizeof(MemoryChunk);
    first -> next = 0;
};
MemoryManager::~MemoryManager(){

};

void* MemoryManager::malloc(myos::common::size_t size){
    MemoryChunk* result = 0;

    for (MemoryChunk* chunk = first; chunk != 0 && result == 0; chunk = chunk -> next){
        if (chunk -> size > size && !chunk -> allocated){
            result = chunk;
        }
    }

    if (result == 0) return 0;

    if (result -> size >= size + sizeof(MemoryChunk) + 1){
        MemoryChunk* temp = (MemoryChunk*)((size_t)result + sizeof(MemoryChunk) + size);
        temp -> allocated = false;
        temp -> size = result -> size - size - sizeof(MemoryChunk);
        temp -> prev = result;
        temp -> next = result -> next;
        if (temp -> next != 0){
            temp -> next -> prev = temp;
        }
        result -> size = size;
        result -> next = temp;
    }

    result -> allocated = true;
    return (void*)(((size_t)result) + sizeof(MemoryChunk));
}; 
void MemoryManager::free(void* ptr){
    MemoryChunk* chunk = (MemoryChunk*)((size_t)ptr - sizeof(MemoryChunk));
    chunk -> allocated = false;

    if(chunk -> prev != 0 && !chunk -> prev -> allocated){
        chunk -> prev -> next = chunk -> next;
        chunk -> prev -> size += chunk -> size + sizeof(MemoryChunk);
        if (chunk -> next != 0){
            chunk -> next -> prev = chunk -> prev;
        }
        chunk = chunk -> prev;
    }

    if (chunk -> next != 0 && !chunk -> next -> allocated){
        chunk -> size += chunk -> next -> size + sizeof(MemoryChunk);
        chunk -> next = chunk -> next -> next;
        if (chunk -> next != 0){
            chunk -> next -> prev = chunk;
        }
    }
};

// ====================================== GLOBAL OPERATORS =======================================

void* operator new(unsigned size){
    if (myos::MemoryManager::activeMemoryManager == 0){
        return 0;
    }
    return myos::MemoryManager::activeMemoryManager->malloc(size);
};
void* operator new[](unsigned size){
    if (myos::MemoryManager::activeMemoryManager == 0){
        return 0;
    }
    return myos::MemoryManager::activeMemoryManager->malloc(size);
};

void* operator new(unsigned size, void* ptr){
    return ptr;
};
void* operator new[](unsigned size, void* ptr){
    return ptr;
};

void operator delete(void* ptr){
    if (myos::MemoryManager::activeMemoryManager != 0){
        myos::MemoryManager::activeMemoryManager->free(ptr);
    }
};
void operator delete[](void* ptr){
    if (myos::MemoryManager::activeMemoryManager != 0){
        myos::MemoryManager::activeMemoryManager->free(ptr);
    }
};