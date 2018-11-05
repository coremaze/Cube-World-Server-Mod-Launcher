#define no_shenanigans __attribute__((noinline)) __declspec(dllexport)
#include <windows.h>
#include <iostream>
#include "callbacks.h"


unsigned int base;

unsigned int __stdcall no_shenanigans HandlePacket(unsigned int packet_id, unsigned int socket_ptr){
    if (packet_id != 0){
        printf("Packet ID: %d\n", packet_id);
    }
    return 0;
}

DWORD WINAPI no_shenanigans RegisterCallbacks(){

        RegisterCallback("RegisterPacketCallback", HandlePacket);

        return 0;
}

extern "C" no_shenanigans bool APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    base = (UINT_PTR)GetModuleHandle(NULL);

    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RegisterCallbacks, 0, 0, NULL);
            break;
    }
    return true;
}
