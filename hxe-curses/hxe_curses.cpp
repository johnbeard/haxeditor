/*-----------------------------------------------------------------/

    hxe_curses.cpp

    Created on: 19 Aug 2015
    Author: John Beard
    Copyright (C) 2015 John Beard

/-----------------------------------------------------------------*/

#include <haxeditor/HaxEditor.h>
#include <ncurses.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>
#include <vector>

class HaxWindow
{
public:
    HaxWindow() :
            shown(true),
            window(nullptr),
            x(0), y(0), w(0), h(0),
            colourAttr(A_NORMAL)
    {
    }

    bool isShown() const
    {
        return shown;
    }

    void SetPosition(int x_, int y_, int w_, int h_)
    {
        x = x_;
        y = y_;
        w = w_;
        h = h_;

        // update directly
        if (shown)
            createWindow();
    }

    void setIsVisible(bool show)
    {
        if (!show && shown)
        {
            destroyWindow();
        }
        else if (show && !shown)
        {
            createWindow();
        }

        shown = show;
    }

    void printText(const std::string& txt)
    {
        wprintw(window, txt.c_str());

        wrefresh(window);
    }

    void NewLine()
    {
        wprintw(window, "\n");
        wrefresh(window);
    }

    void SetColour(int colour)
    {
        colourAttr = COLOR_PAIR(colour);
        wattron(window, colourAttr);
    }

    void UnsetColour()
    {
        wattroff(window, colourAttr);
    }

private:

    void createWindow()
    {
        if (window)
            destroyWindow();

        window = newwin(h, w, x, y);
        box(window, 0, 0);
        wrefresh(window);
    }

    void destroyWindow()
    {
        if (!window)
            return;

        wborder(window, ' ', ' ', ' ',' ',' ',' ',' ',' ');
        wrefresh(window);
        delwin(window);

        window = nullptr;
    }

    bool shown;
    WINDOW* window;
    int x,y,w,h;
    int colourAttr;
};

/*!
 * A window that displays a data buffer as hexadecimal
 */
class HaxHexWindow: public HaxWindow
{
public:
    HaxHexWindow()
    {
    }


    void SetData(std::vector<char>& data)
    {
        const int lineLen = 16;

        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0');

        for (unsigned i = 0; i < data.size(); ++i)
        {
            char byte = data[i];
            ss.str(std::string());
            ss << std::setw(2) << static_cast<unsigned char>(byte) << " ";

            if (byte == 2)
                SetColour(4);
            else
                UnsetColour();

            printText(ss.str());
        }
    }
};

class HaxEditorCurses: public HaxEditorBase
{
private:

};

static bool color_enabled;

void init_colors(void)
{
    use_default_colors();
    color_enabled = has_colors();
    if(color_enabled)
    {
        start_color();
        init_pair(1, COLOR_BLACK,   COLOR_BLACK);
        init_pair(2, COLOR_RED,     COLOR_BLACK);
        init_pair(3, COLOR_GREEN,   COLOR_BLACK);
        init_pair(4, COLOR_YELLOW,  COLOR_BLACK);
        init_pair(5, COLOR_BLUE,    COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7, COLOR_CYAN,    COLOR_BLACK);
        init_pair(8, COLOR_WHITE,   COLOR_BLACK);
    }
}

int main()
{
    // drop into curses mode
    initscr();

    // no line buffering - we want input ASAP
    raw();

    // we get Fn keys, etc,
    keypad(stdscr, true);

    // we'll deal with echoing ourselves
    noecho();

    init_colors();

    // splash screen if no file
    printw("HexEditor Curses");  /* Print Hello World          */
    refresh();          /* Print it on to the real screen */

    wxFileName filename("/home/john/Pictures/Grid-400.png");
    FAL myfile(filename, FAL::ReadOnly);

    long position = 0;
    myfile.Seek(position, wxFromStart);
    char *buffer = new char[ 1024 ];
    int readBytes = myfile.Read(buffer, 1024);

    HaxHexWindow hex;

    hex.SetPosition(10,10,16*3,10);

    std::vector<char> data;

    for (int i = 0; i < readBytes; ++i)
    {
        data.push_back(buffer[i]);
    }

    hex.SetData(data);

    // drop into input loop
    getch();

    // that's it, shut it down
    endwin();
    return 0;
}

