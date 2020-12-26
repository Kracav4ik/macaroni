#pragma once

#include "BasePanel.h"
#include "ClientPipe.h"
#include "AsyncWrapper.h"

class ClientPanel : public BasePanel {
public:
    explicit ClientPanel(Screen& screen);
    ~ClientPanel();

protected:
    void connect() override;
    bool waitPipeReady(HANDLE extraObject, DWORD timeoutMs) override;
    void disconnect() override;
    AsyncWrapper& getWrapper() override;
    WORD lineEditBg() const override;
    std::wstring connStateText() const override;
    std::wstring editHint() const override;

private:
    ClientPipe clientPipe;
    AsyncWrapper wrapper;
};
