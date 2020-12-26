#include "ClientPanel.h"
#include "the_framework/Screen.h"
#include "the_framework/MessagePopup.h"

int main(int argc, char* argv[]) {
    Screen s(80, 25);
    s.setTitle(L"ТРУБЯЩИЙ КЛИЕНТ");

    bool running = true;

    ClientPanel panel(s);

    // Drawing
    auto repaint = [&]() {
        s.clear(FG::GREY | BG::BLACK);

        panel.drawOn(s);
        MessagePopup::drawOn(s);

        s.flip();
    };

    // Global exit
    s.handlePriorityKey(VK_F10, 0, [&]() {
        running = false;
    });

    // Global message popup
    MessagePopup::registerKeys(s);

    panel.registerKeys(s);

    // Initial state
    repaint();

    // Main loop
    while (running) {
        panel.waitEvents(s.getStdinHandle(), 1000);
        if (s.hasEvent()) {
            s.processEvent();
        }
        repaint();
    }



    /*
    int i;
    char chBuf[BUFSIZE];
    DWORD cbRead;

    HANDLE hPipe = CreateFileA(
            PIPE_NAME.c_str(),   // pipe name
            GENERIC_READ | GENERIC_WRITE,
            0,              // no sharing
            nullptr,           // default security attributes
            OPEN_EXISTING,  // opens existing pipe
            0,              // default attributes
            nullptr);

    DWORD dwMode = PIPE_READMODE_MESSAGE;
    SetNamedPipeHandleState(
            hPipe,    // pipe handle
            &dwMode,  // new pipe mode
            nullptr,     // don't set maximum bytes
            nullptr);    // don't set maximum time

    std::cout << "wait for read 1\n";
    std::cin >> i;

    ReadFile(
            hPipe,    // pipe handle
            chBuf,    // buffer to receive reply
            BUFSIZE,  // size of buffer
            &cbRead,  // number of bytes read
            nullptr);    // not overlapped

    std::cout << "read " << cbRead << std::endl;;

    std::cout << "wait for read 2\n";
    std::cin >> i;

    ReadFile(
            hPipe,    // pipe handle
            chBuf,    // buffer to receive reply
            BUFSIZE,  // size of buffer
            &cbRead,  // number of bytes read
            nullptr);    // not overlapped

    std::cout << "read " << cbRead << std::endl;

    std::cout << "wait for write\n";
    std::cin >> i;


    std::wstring msg = L"fuck you server fuck";
    DWORD cbWritten;
    WriteFile(
            hPipe,
            msg.c_str(),
            msg.size() * sizeof(wchar_t ),
            &cbWritten,
            nullptr);
    std::cout << "written " << cbWritten  << std::endl;;

    std::cout << "end" << std::endl;;
    std::cin >> i;

    //*/
}
