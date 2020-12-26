#pragma once

#include "BasePanel.h"

#include "ServerPipe.h"
#include "AsyncWrapper.h"

class ServerPanel : public BasePanel {
public:
    explicit ServerPanel(Screen& screen);
    ~ServerPanel();

protected:
    void connect() override;
    bool waitPipeReady(HANDLE extraObject, DWORD timeoutMs) override;
    void disconnect() override;
    AsyncWrapper& getWrapper() override;
    WORD lineEditBg() const override;
    std::wstring connStateText() const override;
    std::wstring editHint() const override;

private:
    ServerPipe serverPipe;
    AsyncWrapper wrapper;
};
