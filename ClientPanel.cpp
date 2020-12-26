#include "ClientPanel.h"

#include "the_framework/MessagePopup.h"
#include "the_framework/utils.h"

ClientPanel::ClientPanel(Screen& screen)
    : BasePanel(screen, true)
    , clientPipe(PIPE_NAME)
    , wrapper(clientPipe.pipe(), [this](DWORD error) {
            MessagePopup::show({
                    getLastErrorText(error),
                    L"Код ошибки " + std::to_wstring(error)
            }, true);
            disconnect();
        })
{
}

ClientPanel::~ClientPanel() {
    disconnect();
}

void ClientPanel::connect() {
    if (!clientPipe.connect()) connState = ConnState::Disconnected;
}

bool ClientPanel::waitPipeReady(HANDLE extraObject, DWORD timeoutMs) {
    return true;
}

void ClientPanel::disconnect() {
    BasePanel::disconnect();
    clientPipe.disconnect();
}

AsyncWrapper& ClientPanel::getWrapper() {
    return wrapper;
}

WORD ClientPanel::lineEditBg() const {
    return BG::DARK_CYAN;
}

std::wstring ClientPanel::connStateText() const {
    if (connState == ConnState::Connected) {
        return L"Статус: подключен к серверу";
    }
    return L"Статус: канал отключен";
}

std::wstring ClientPanel::editHint() const {
    return L"Введите сообщение для сервера";
}
