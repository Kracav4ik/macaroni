#pragma once

#include "the_framework/Lines.h"
#include "the_framework/colors.h"

class EditableText;
class AsyncWrapper;

enum class ConnState {
    Disconnected,
    WaitingConnect,
    Connected,
};

class BasePanel {
public:
    BasePanel(Screen& screen, bool isClient);

    void registerKeys(Screen& screen);
    void drawOn(Screen& s);

    void waitEvents(HANDLE stdinHandle, DWORD timeoutMs);

protected:
    struct Message {
        int session;
        bool fromClient;
        bool partial;
        std::wstring text;
    };

    ~BasePanel() = default;

    void singleRead();

    virtual void connect() = 0;
    virtual bool waitPipeReady(HANDLE extraObject, DWORD timeoutMs) = 0;
    virtual void disconnect();
    virtual AsyncWrapper& getWrapper() = 0;

    bool canEdit() const;
    virtual WORD lineEditBg() const = 0;
    virtual std::wstring connStateText() const = 0;
    virtual std::wstring editHint() const = 0;
    std::wstring pauseModeText() const;
    int scrollFromDelta(int delta) const;
    void updateLines();
    std::vector<Color> getScrollbar() const;

    static const std::string PIPE_NAME;
    static const int BUF_SIZE_CHARS;
    static const int BUF_SIZE_BYTES;

    EditableText& editable;
    ConnState connState;
    bool isReading;
    bool isWriting;
    bool isClient;
    bool isStepByStep;
    int session;
    int tickAnim;
    std::vector<Message> messages;
    Lines lines;
};
