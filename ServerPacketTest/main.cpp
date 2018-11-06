#undef __STRICT_ANSI__
#define no_shenanigans __attribute__((noinline)) __declspec(dllexport)
#include <windows.h>
#include <iostream>
#include "callbacks.h"
#include "packets.h"
unsigned int base;

unsigned int __stdcall no_shenanigans HandlePacket(unsigned int packet_id, SOCKET socket){

//    if (packet_id == 10){
//        printf("Packet ID: %d\n", packet_id);
//
//        unsigned int packet_id2 = 10;
//        unsigned long long entity_id = 1;
//        wchar_t msg[1024] = {0};
//        swprintf(msg, L"Packet ID: %d", packet_id);
//        unsigned int msgsize = wcslen(msg);
//
//        char buf[1024] = {0};
//        memcpy(buf, &packet_id2, 4);
//        memcpy(buf + 4, &entity_id, 8);
//        memcpy(buf + 12, &msgsize, 4);
//        memcpy(buf + 16, msg, msgsize*2);
//
//        AddPacket(socket, buf, 4+8+4+msgsize*2);
//    }



    if (packet_id == 1337){
        printf("Packet ID: %d\n", packet_id);
        printf("Socket: %X\n", socket);
        unsigned int newpacketid = 1338;
        //send(socket, (char*)&newpacketid, 4, 0);

        unsigned long long y = 65536;
        //send(socket, (char*)&y, 8, 0);

        char buf[4+8];
        memcpy(buf, &newpacketid, 4);
        memcpy(buf+4, &y, 8);
        AddPacket(socket, buf, 4+8);

        return 1;
    }

    return 0;
}




void __stdcall no_shenanigans HandleReadyToSend(SOCKET socket){
    SendQueuedPackets(socket);
}


DWORD WINAPI no_shenanigans RegisterCallbacks(){

        RegisterCallback("RegisterPacketCallback", HandlePacket);
        RegisterCallback("RegisterReadyToSendCallback", HandleReadyToSend);

        return 0;
}

extern "C" no_shenanigans bool APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    base = (UINT_PTR)GetModuleHandle(NULL);

    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            PacketQueueInit();
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RegisterCallbacks, 0, 0, NULL);
            break;
    }
    return true;
}
