#pragma once

#define NOMINMAX

#include <Windows.h>
#include <string>
#include <initializer_list>

#include "../sdk.hpp"

namespace Utils 
{
	std::vector<char> HexToBytes(const std::string& hex);

	std::string BytesToString(unsigned char* data, int len);

	std::vector<std::string> Split(const std::string& str, const char* delim);

	unsigned int FindInDataMap(datamap_t * pMap, const char * name);

	void AttachConsole();
    void DetachConsole();

    bool ConsolePrint(const char* fmt, ...);
    
    char ConsoleReadKey();

    int WaitForModules(std::int32_t timeout, const std::initializer_list<std::wstring>& modules);

    std::uint8_t* PatternScan(void* module, const char* signature);

    void SetClantag(const char* tag);
    void SetName(const char* name);
    void modern_clamp_world(float& target, float to, float step);

    template<class T>
    static T* FindHudElement(const char* name)
    {
        static auto pThis = *reinterpret_cast<DWORD**>(PatternScan(GetModuleHandleA("client.dll"), "B9 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 89") + 0x1);
        static auto pFindHudElement = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"));
        return reinterpret_cast<T*>(pFindHudElement(pThis, name));
    }
}
