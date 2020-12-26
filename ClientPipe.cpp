#include "ClientPipe.h"

#include "the_framework/utils.h"
#include "the_framework/MessagePopup.h"

ClientPipe::ClientPipe(const std::string& name)
    : name(name)
{
}

bool ClientPipe::connect() {
    hPipe = CreateFileA(
            name.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            nullptr);
    if (hPipe == INVALID_HANDLE_VALUE ) {
        auto error = GetLastError();
        MessagePopup::show({
                L"ClientPipe::connect()",
                getLastErrorText(error),
                L"Код ошибки " + std::to_wstring(error)
        }, true);
        return false;
    }
    DWORD dwMode = PIPE_READMODE_MESSAGE;
    SetNamedPipeHandleState(hPipe, &dwMode, nullptr, nullptr);
    return true;
}

void ClientPipe::disconnect() {
    hPipe.reset();
}

const HandleHolder& ClientPipe::pipe() const {
    return hPipe;
}
