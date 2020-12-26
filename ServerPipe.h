#pragma once

#include "the_framework/HandleHolder.h"
#include <windows.h>
#include <string>

class ServerPipe {
public:
    ServerPipe(const std::string& name, DWORD bufSizeBytes);

    void connect();
    void disconnect();
    bool isReady() const;
    const HandleHolder& pipe() const;

    void waitReady(HANDLE extraObject, DWORD timeoutMs);

private:
    OVERLAPPED ovl;
    HandleHolder hPipe;
    HandleHolder hEvent;
    bool ready;
};
