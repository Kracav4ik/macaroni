#include "ServerPanel.h"

#include "the_framework/colors.h"
#include "the_framework/Screen.h"
#include "the_framework/EditableText.h"
#include "the_framework/MessagePopup.h"
#include "the_framework/utils.h"

ServerPanel::ServerPanel(Screen& screen)
    : BasePanel(screen, false)
    , serverPipe(PIPE_NAME, BUF_SIZE_BYTES)
    , wrapper(serverPipe.pipe(), [this](DWORD error) {
        MessagePopup::show({
                getLastErrorText(error),
                L"Код ошибки " + std::to_wstring(error)
        }, true);
        disconnect();
    })
{
}

ServerPanel::~ServerPanel() {
    disconnect();
}

void ServerPanel::connect() {
    serverPipe.connect();
}

bool ServerPanel::waitPipeReady(HANDLE extraObject, DWORD timeoutMs) {
    serverPipe.waitReady(extraObject, timeoutMs);
    return serverPipe.isReady();
}

void ServerPanel::disconnect() {
    BasePanel::disconnect();
    serverPipe.disconnect();
}

AsyncWrapper& ServerPanel::getWrapper() {
    return wrapper;
}

WORD ServerPanel::lineEditBg() const {
    return BG::DARK_GREEN;
}

std::wstring ServerPanel::connStateText() const {
    if (connState == ConnState::WaitingConnect) {
        return L"Статус: ожидание клиента";
    }
    if (connState == ConnState::Connected) {
        return L"Статус: клиент подключен";
    }
    return L"Статус: канал отключен";
}

std::wstring ServerPanel::editHint() const {
    return L"Введите сообщение для клиента";
}
