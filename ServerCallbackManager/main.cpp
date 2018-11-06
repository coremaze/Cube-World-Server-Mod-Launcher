#include <windows.h>
#include <vector>

#define no_shenanigans __attribute__((noinline)) __declspec(dllexport)

#define MakeCallback(callbackName, callbackReturnType, registrarFunctionName, vectorName)\
    typedef callbackReturnType (__stdcall *callbackName)( ... );\
    std::vector<callbackName> vectorName;\
    extern "C" void __declspec(dllexport) registrarFunctionName(callbackName func){\
    vectorName.push_back(func);\
    }

unsigned int base;

MakeCallback(PacketCallback, int, RegisterPacketCallback, packet_callbacks);
int __stdcall no_shenanigans HandlePacket(unsigned int packet_ID, unsigned int socket_ptr){
    int handled = 0;
    for (PacketCallback func : packet_callbacks){
        unsigned int handled_2 = func(packet_ID, socket_ptr);
        if (handled_2 != 0){
            handled = handled_2;
        }
    }
    return handled;
}
DWORD HandlePacket_ptr = (DWORD)&HandlePacket;


unsigned int ASMHandlePacket_Invalid_Packet_JMP;
unsigned int ASMHandlePacket_Valid_Packet_JMP;
unsigned int ASMHandlePacket_Already_Handled_JMP;
void no_shenanigans ASMHandlePacket(){
    asm("pushad");

    asm("push [ebp-0x13B0]"); //socket
    asm("push [ebp-0x13C8]"); //Packet ID
    asm("call [_HandlePacket_ptr]");


    asm("cmp eax, 0");
    asm("je 0f"); //The packet was not handled externally.

    asm("popad"); //The packet WAS handled externally.
    asm("mov bl, [ebp-0x13A5]");
    asm("jmp [_ASMHandlePacket_Already_Handled_JMP]");


    asm("0:"); //The packet was not handled externally.
    asm("popad");
    asm("mov eax, dword ptr [ebp-0x13C8]"); //original code
    asm("cmp eax, 0xC");
    asm("ja 1f");
    asm("jmp [_ASMHandlePacket_Valid_Packet_JMP]");

    asm("1:");
    asm("jmp [_ASMHandlePacket_Invalid_Packet_JMP]");
}

void WriteJMP(BYTE* location, BYTE* newFunction){
    DWORD dwOldProtection;
    VirtualProtect(location, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection);
    location[0] = 0xE9; //jmp
    *((DWORD*)(location + 1)) = (DWORD)(( (unsigned INT32)newFunction - (unsigned INT32)location ) - 5);
    VirtualProtect(location, 5, dwOldProtection, &dwOldProtection);
}

extern "C" __declspec(dllexport) bool APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    base = (unsigned int)GetModuleHandle(NULL);
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            ASMHandlePacket_Invalid_Packet_JMP = base + 0x266DD;
            ASMHandlePacket_Valid_Packet_JMP = base + 0x260D8;
            ASMHandlePacket_Already_Handled_JMP = base + 0x266E3;
            WriteJMP((BYTE*)base + 0x260C9, (BYTE*)&ASMHandlePacket);
            break;
    }
    return true;
}
