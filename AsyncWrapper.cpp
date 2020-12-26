#include "AsyncWrapper.h"

#include "the_framework/MessagePopup.h"
#include "the_framework/utils.h"

AsyncWrapper::AsyncWrapper(const HandleHolder& handle, std::function<void(DWORD)> errorCallback)
    : handle(handle)
    , errorCallback(std::move(errorCallback))
{
}

void AsyncWrapper::readAsync(DWORD bufSizeChars, std::function<void(std::wstring, bool)> callback) {
    auto context = std::make_unique<ReadContext>(this, bufSizeChars, std::move(callback));
    auto ovl = &context->ovl;
    auto buf = context->buf.data();
    readCtx.emplace(ovl, std::move(context));
    auto result = ReadFileEx(handle, buf, bufSizeChars * sizeof(wchar_t), ovl, [](DWORD error, DWORD bytesRead, LPOVERLAPPED ovl) {
        auto context = (ReadContext*)ovl;
        if (error != 0) {
            context->owner->errorCallback(error);
        } else {
            // MSDN recommended way to check if pipe has more data
            DWORD unused;
            WINBOOL success = GetOverlappedResult(context->owner->handle, ovl, &unused, FALSE);
            DWORD ovlStatus = GetLastError();
            int charsRead = bytesRead / sizeof(wchar_t);
            bool partial = success == 0 && ovlStatus == ERROR_MORE_DATA;

            context->callback(std::wstring(context->buf.data(), charsRead), partial);
        }
        context->owner->readCtx.erase(ovl);
    });
    if (result == 0) {
        errorCallback(GetLastError());
    }
}

void AsyncWrapper::writeAsync(std::wstring data, std::function<void()> callback) {
    auto context = std::make_unique<WriteContext>(this, std::move(data), std::move(callback));
    auto ovl = &context->ovl;
    auto buf = context->data.c_str();
    auto bufSize = context->data.size();
    writeCtx.emplace(ovl, std::move(context));
    auto result = WriteFileEx(handle, buf, bufSize * sizeof(wchar_t), ovl, [](DWORD error, DWORD bytesRead, LPOVERLAPPED ovl) {
        auto context = (WriteContext*)ovl;
        if (error != 0) {
            context->owner->errorCallback(error);
        } else {
            context->callback();
        }
        context->owner->writeCtx.erase(ovl);
    });
    if (result == 0) {
        errorCallback(GetLastError());
    }
}

AsyncWrapper::Context::Context(AsyncWrapper* owner)
    : owner(owner)
{
    memset(&ovl, 0, sizeof(ovl));
}

AsyncWrapper::ReadContext::ReadContext(AsyncWrapper* owner, DWORD bufSize, std::function<void(std::wstring, bool)> callback)
    : Context(owner)
    , buf(bufSize, L'\0')
    , callback(std::move(callback))
{
}

AsyncWrapper::WriteContext::WriteContext(AsyncWrapper* owner, std::wstring data, std::function<void()> callback)
    : Context(owner)
    , data(std::move(data))
    , callback(std::move(callback))
{
}
