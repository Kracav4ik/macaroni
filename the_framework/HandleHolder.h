#pragma once

#include <windows.h>

class HandleHolder {
public:
    HandleHolder();
    HandleHolder(HANDLE value);
    HandleHolder(HandleHolder&& other) noexcept;
    HandleHolder& operator=(HandleHolder&& other) noexcept;
    ~HandleHolder();

    void reset(HANDLE value = INVALID_HANDLE_VALUE);

    operator HANDLE() const;

private:
    HANDLE handle;
};
