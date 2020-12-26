#include "HandleHolder.h"

#include <utility>

HandleHolder::HandleHolder()
    : handle(INVALID_HANDLE_VALUE)
{
}

HandleHolder::HandleHolder(HANDLE value)
    : handle(value)
{
}

HandleHolder::HandleHolder(HandleHolder&& other) noexcept
    : handle(other.handle)
{
    other.handle = INVALID_HANDLE_VALUE;
}

HandleHolder& HandleHolder::operator=(HandleHolder&& other) noexcept {
    std::swap(handle, other.handle);
    return *this;
}

HandleHolder::~HandleHolder() {
    reset();
}

void HandleHolder::reset(HANDLE value) {
    if (handle != INVALID_HANDLE_VALUE) {
        CloseHandle(handle);
    }
    handle = value;
}

HandleHolder::operator HANDLE() const {
    return handle;
}
