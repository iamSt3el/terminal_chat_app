#include "ui.h"
#include <ncurses.h>

void UI::initUI()
{
    initscr();
    cbreak();
    //raw();
    keypad(stdscr, TRUE);
    noecho();

    start_color(); // Enable color

    // Define your color pairs
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);


    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);

    // Create windows
    msgWindow = newwin(maxY - 3, maxX, 0, 0);
    inputWindow = newwin(3, maxX, maxY - 3, 0);

    scrollok(msgWindow, TRUE);
    box(msgWindow, 0, 0);
    box(inputWindow, 0, 0);

    

    wrefresh(msgWindow);
    wrefresh(inputWindow);
}
void UI::printMessage(const std::string &message)
{
    messages.push_back(message);
    if (messages.size() > getmaxy(msgWindow) - 2) {
        messages.erase(messages.begin());
    }

    werase(msgWindow);
    box(msgWindow, 0, 0);

    for (size_t i = 0; i < messages.size(); ++i) {
        // Check if the message is a received message
        bool isReceivedMessage = messages[i].find("Recevied:") == 0;

        // Set the color based on the message type
        wattron(msgWindow, COLOR_PAIR(isReceivedMessage ? 1 : 2));

        mvwprintw(msgWindow, i + 1, 1, messages[i].c_str());

        wattroff(msgWindow, COLOR_PAIR(isReceivedMessage ? 1 : 2));
    }
    wrefresh(msgWindow);
}


std::string UI::getUserInput()
{
    cbreak();
    char buffer[256];
    echo();
    curs_set(1);
    move(1, 5);
    mvwgetnstr(inputWindow, 1, 5, buffer, sizeof(buffer) - 1);
    curs_set(0);
    noecho();

    wclear(inputWindow);
    box(inputWindow, 0, 0);
    wrefresh(inputWindow);

    return buffer;
}

void UI::handleResize(int sig)
{
    endwin();
    refresh();
    clear();

    // Recreate and redraw windows
    initUI();
    for (const auto &message : messages) {
        printMessage(message);
    }
}

void UI::closeUI()
{
    delwin(msgWindow);
    delwin(inputWindow);
    endwin();
}