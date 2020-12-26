#include "BasePanel.h"

#include "AsyncWrapper.h"

#include "the_framework/Screen.h"
#include "the_framework/EditableText.h"
#include "the_framework/MessagePopup.h"
#include "the_framework/utils.h"

const std::string BasePanel::PIPE_NAME = R"(\\.\pipe\Tpy6a_Tpy6uT)";
const int BasePanel::BUF_SIZE_CHARS = 16;
const int BasePanel::BUF_SIZE_BYTES = BUF_SIZE_CHARS * sizeof(wchar_t);

static const int LINES_H = 13;
static const int EDIT_SIZE = 56;

BasePanel::BasePanel(Screen& screen, bool isClient)
    : editable(screen.getEditable())
    , connState(ConnState::Disconnected)
    , isReading(false)
    , isWriting(false)
    , isClient(isClient)
    , isStepByStep(false)
    , session(0)
    , tickAnim(0)
{
    editable.setEnabledGetter([this]() {
        return canEdit() && !MessagePopup::isVisible();
    });
    editable.setTextSizeMax(EDIT_SIZE - 1);
    editable.setText(L"", EDIT_SIZE);

    updateLines();
}

void BasePanel::registerKeys(Screen& screen) {
    screen.handleKey(VK_UP, 0, [this]() {
        lines.setScrollOffset(scrollFromDelta(-1));
    });
    screen.handleKey(VK_DOWN, 0, [this]() {
        lines.setScrollOffset(scrollFromDelta(1));
    });
    screen.handleKey(VK_PRIOR, 0, [this]() {
        lines.setScrollOffset(scrollFromDelta(-LINES_H/2));
    });
    screen.handleKey(VK_NEXT, 0, [this]() {
        lines.setScrollOffset(scrollFromDelta(LINES_H/2));
    });
    screen.handleKey(VK_HOME, ANY_CTRL_PRESSED, [this]() {
        lines.setScrollOffset(0);
    });
    screen.handleKey(VK_END, ANY_CTRL_PRESSED, [this]() {
        lines.setScrollOffset(scrollFromDelta(lines.getLinesCount()));
    });
    screen.handleKey(VK_F2, 0, [this]() {
        isStepByStep = !isStepByStep;
    });
    screen.handleKey(VK_F5, 0, [this]() {
        if (connState != ConnState::Disconnected) {
            return;
        }
        connState = ConnState::WaitingConnect;
        ++session;
        updateLines();
        connect();
    });
    screen.handleKey(VK_F8, 0, [this]() {
        if (connState == ConnState::Disconnected) {
            return;
        }
        disconnect();
    });
    screen.handleKey(VK_TAB, 0, [this]() {
        if (connState != ConnState::Connected || !isStepByStep) {
            return;
        }
        singleRead();
    });
    screen.handleKey(VK_RETURN, 0, [this]() {
        if (connState != ConnState::Connected || isWriting) {
            return;
        }
        isWriting = true;
        tickAnim = 0;
        getWrapper().writeAsync(editable.getText(), [this]() {
            isWriting = false;
            messages.push_back({session, isClient, false, editable.getText()});
            updateLines();
            lines.setScrollOffset(scrollFromDelta(1));
            editable.setText(L"", EDIT_SIZE);
        });
    });
}

void BasePanel::drawOn(Screen& s) {
    Rect titleRect = Rect{0, 1, s.w(), 1}.withPadding(2, 0);
    s.labelsFill(titleRect, {connStateText(), pauseModeText()}, FG::WHITE | BG::BLACK);

    WORD attr = canEdit() ? FG::WHITE | lineEditBg() : FG::GREY | BG::DARK_GREY;
    Rect editRect{2, 3, EDIT_SIZE, 1};
    if (canEdit() && editable.getText().empty()) {
        attr = FG::DARK_GREY | lineEditBg();
        s.textOut(editRect.getLeftTop(), editHint());
    }
    s.paintRect(editRect, attr, false);
    editable.drawOn(s, editRect.getLeftTop(), canEdit());

    s.paintRect(editRect.moved(editRect.w, 0).withW(20), FG::WHITE | BG::BLACK);
    s.textOut({60, 3}, L"<Enter> Отправить");

    if (isWriting) {
        static const std::wstring ticks = L"-\\|/";
        s.paintRect({60, 5, 20, 1}, FG::DARK_GREY | BG::BLACK);
        s.paintRect({72, 5, 1, 1}, FG::WHITE | BG::BLACK);
        s.textOut({60, 5}, L"Отправка... " + ticks.substr(tickAnim % ticks.size(), 1));
    }

    s.paintRect({2, 4, BUF_SIZE_CHARS, 2}, FG::DARK_GREY | BG::BLACK);
    s.textOut({2, 4}, L"└" + std::wstring(BUF_SIZE_CHARS - 2, L'─') + L"┘");
    s.textOut({4, 5}, L"Буфер канала");

    if (isStepByStep) {
        s.paintRect({22, 5, 34, 1}, FG::DARK_RED | BG::BLACK);
        s.textOut({22, 5}, L"<Tab> Прочитать из канала в буфер");
    }

    Rect linesFrame = Rect{0, 7, s.w(), LINES_H + 2}.withPadX(2);
    s.frame(linesFrame, false);
    Rect linesRect = linesFrame.withPadding(2, 1);
    lines.drawOn(s, linesRect);
    s.pixelMap(linesRect.moved(linesRect.w + 1, 0).withW(1), getScrollbar(), Color::Grey);

    Rect buttonsRect = Rect{0, 23, s.w(), 1}.withPadding(2, 0);
    std::wstring otherMode = isStepByStep ? L"Непрерывный" : L"Пошаговый";
    s.labelsFill(buttonsRect, {
            L"F2 " + otherMode + L" режим приема", L"F5 Подключиться", L"F8 Отключиться", L"F10 Выход"
    }, FG::WHITE | BG::BLACK);
}

void BasePanel::waitEvents(HANDLE stdinHandle, DWORD timeoutMs) {
    if (isWriting) {
        ++tickAnim;
    }
    if (!isStepByStep) {
        singleRead();
    }
    if (connState == ConnState::WaitingConnect) {
        if (waitPipeReady(stdinHandle, timeoutMs)) {
            connState = ConnState::Connected;
        }
    } else {
        WaitForSingleObjectEx(stdinHandle, timeoutMs, TRUE);
    }
}

void BasePanel::singleRead() {
    if (connState == ConnState::Connected && !isReading) {
        isReading = true;
        getWrapper().readAsync(BUF_SIZE_CHARS, [this](std::wstring data, bool partial) {
            isReading = false;
            if (!messages.empty() && messages.back().partial) {
                messages.back().text += data;
                messages.back().partial = partial;
            } else {
                messages.push_back({session, !isClient, partial, std::move(data)});
            }
            updateLines();
            lines.setScrollOffset(scrollFromDelta(1));
        });
    }
}

void BasePanel::disconnect() {
    isWriting = false;
    isReading = false;
    editable.setText(L"", EDIT_SIZE);
    connState = ConnState::Disconnected;
}

bool BasePanel::canEdit() const {
    return connState == ConnState::Connected && !isWriting;
}

std::wstring BasePanel::pauseModeText() const {
    if (isStepByStep) {
        return L"Режим приема сообщений: пошаговый";
    }
    return L"Режим приема сообщений: непрерывный";
}

int BasePanel::scrollFromDelta(int delta) const {
    return clamp(0, delta + lines.getScrollOffset(), lines.getLinesCount() - LINES_H + 1);
}

void BasePanel::updateLines() {
    std::vector<StyledText> rows;
    rows.reserve(messages.size());
    for (const auto& msg : messages) {
        Color color = msg.fromClient ? Color::Cyan : Color::Green;
        if (session != msg.session) {
            color = darken(color);
        }
        WORD attr = toFg(color) | BG::BLACK;
        std::wstring prefix = msg.fromClient ? L"КЛИЕНТ: " : L"СЕРВЕР: ";
        std::wstring suffix = msg.partial ? L"░" : L"";
        rows.push_back({prefix + msg.text + suffix, attr, attr});
    }
    lines.setLines(std::move(rows));
}

std::vector<Color> BasePanel::getScrollbar() const {
    int pixelsCount = 2*LINES_H;
    std::vector<Color> result(pixelsCount, Color::Black);
    double totalArea = lines.getLinesCount() + 1;
    int scrollBegin = roundI(pixelsCount * lines.getScrollOffset() / totalArea);
    int scrollSize = clamp(1, roundI(pixelsCount * LINES_H / totalArea), pixelsCount);
    scrollBegin = std::min(scrollBegin, pixelsCount - scrollSize);
    for (int i = scrollBegin; i < scrollBegin + scrollSize; ++i) {
        result[i] = Color::Grey;
    }
    return result;
}
