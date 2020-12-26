#include "ServerPanel.h"
#include "the_framework/Screen.h"
#include "the_framework/MessagePopup.h"

int main() {
    Screen s(80, 25);
    s.setTitle(L"ТРУБЯЩИЙ СЕРВЕР");

    bool running = true;

    ServerPanel panel(s);

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
}
