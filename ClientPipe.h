#pragma once

#include "the_framework/HandleHolder.h"

#include <windows.h>
#include <string>

class ClientPipe {
public:
    explicit ClientPipe(const std::string& name);

    bool connect();
    void disconnect();
    const HandleHolder& pipe() const;
private:
    std::string name;
    HandleHolder hPipe;
};
