#include "ServerPipe.h"

#include "the_framework/utils.h"
#include "the_framework/MessagePopup.h"

ServerPipe::ServerPipe(const std::string& name, DWORD bufSizeBytes)
    : ready(false)
{
    hEvent = CreateEventW(nullptr, TRUE, TRUE, nullptr);

    memset(&ovl, 0, sizeof(ovl));
    ovl.hEvent = hEvent;

    hPipe = CreateNamedPipeA(
            name.c_str(),
            PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1,
            bufSizeBytes,
            bufSizeBytes,
            0,
            nullptr);
    if (hPipe == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        MessagePopup::show({
            L"ServerPipe::ServerPipe()",
            getLastErrorText(error),
            L"Код ошибки " + std::to_wstring(error)
        }, true);
    }
    // need wait for pipe connection
}

void ServerPipe::connect() {
    ConnectNamedPipe(hPipe, &ovl);
    DWORD status = GetLastError();
    if (status == ERROR_PIPE_CONNECTED) {
        // pipe connected
        SetEvent(hEvent);
        return;
    }
    if (status != ERROR_IO_PENDING) {
        MessagePopup::show({
            L"ServerPipe::connect()",
            getLastErrorText(status),
            L"Код ошибки " + std::to_wstring(status)
        }, true);
        hPipe = INVALID_HANDLE_VALUE;
    }
}

void ServerPipe::disconnect() {
    ready = false;
    DisconnectNamedPipe(hPipe);
}

bool ServerPipe::isReady() const {
    return ready;
}

const HandleHolder& ServerPipe::pipe() const {
    return hPipe;
}

void ServerPipe::waitReady(HANDLE extraObject, DWORD timeoutMs) {
    std::vector<HANDLE> objects{hEvent, extraObject};
    DWORD res = WaitForMultipleObjectsEx(objects.size(), objects.data(), FALSE, timeoutMs, TRUE);
    if (res == WAIT_OBJECT_0) {
        ready = true;
    }
    if (res == WAIT_FAILED) {
        DWORD error = GetLastError();
        MessagePopup::show({
            L"ServerPipe::waitReady()",
            getLastErrorText(error),
            L"Код ошибки " + std::to_wstring(error)
        }, true);
    }
}
