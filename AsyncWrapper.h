#pragma once

#include "the_framework/HandleHolder.h"

#include <windows.h>
#include <vector>
#include <functional>
#include <string>
#include <unordered_map>
#include <memory>

class AsyncWrapper {
public:
    AsyncWrapper(const HandleHolder& handle, std::function<void(DWORD)> errorCallback);

    void readAsync(DWORD bufSizeChars, std::function<void(std::wstring, bool)> callback);
    void writeAsync(std::wstring data, std::function<void()> callback);

private:
    struct Context {
        explicit Context(AsyncWrapper* owner);

        OVERLAPPED ovl;
        AsyncWrapper* owner;
    };

    struct ReadContext : Context {
        ReadContext(AsyncWrapper* owner, DWORD bufSize, std::function<void(std::wstring, bool)> callback);

        std::vector<wchar_t> buf;
        std::function<void(std::wstring, bool)> callback;
    };
    struct WriteContext : Context {
        WriteContext(AsyncWrapper* owner, std::wstring data, std::function<void()> callback);

        std::wstring data;
        std::function<void()> callback;
    };

    const HandleHolder& handle;
    std::function<void(DWORD)> errorCallback;
    std::unordered_map<LPOVERLAPPED, std::unique_ptr<ReadContext>> readCtx;
    std::unordered_map<LPOVERLAPPED, std::unique_ptr<WriteContext>> writeCtx;
};
