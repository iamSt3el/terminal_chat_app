#ifndef UI_H
#define UI_H
#include <ncurses.h>
#include <string>
#include <vector>
#include <csignal>

class UI{
    public:
    WINDOW *msgWindow;
    WINDOW *inputWindow;
    std::vector<std::string> messages;

    void initUI();
    void printMessage(const std::string &message);
    std::string getUserInput();
    void handleResize(int sig);
    void closeUI();

};

#endif
