/*-----------------------------------------------------------------/

    hxe_curses.cpp

    Created on: 19 Aug 2015
    Author: John Beard
    Copyright (C) 2015 John Beard

/-----------------------------------------------------------------*/

#include <haxeditor/haxeditor.h>

#include <ncurses.h>

#include <iostream>
#include <memory>

class HaxWindow
{
public:
    HaxWindow() :
            shown(false),
            window(nullptr)
    {
    }

    bool isShown() const
    {
        return shown;
    }

    void SetPosition(int x, int y, int w, int h)
    {
        destroy_win();
        window = newwin(h,w,x,y);
        box(window, 0, 0);
        wrefresh(window);
    }

private:

    void destroy_win()
    {
        if (!window)
            return;

        wborder(window, ' ', ' ', ' ',' ',' ',' ',' ',' ');
        wrefresh(window);
        delwin(window);
    }

    bool shown;
    WINDOW* window;
};

/*!
 * A window that displays a data buffer as hexadecimal
 */
class HaxHexWindow: public HaxWindow
{
public:
    HaxHexWindow()
{}
};

class HaxEditorCurses: public HaxEditorBase
{
private:

};

int main()
{
    HaxEditorCurses ed;

    // drop into curses mode
    initscr();

    // no line buffering - we want input ASAP
    raw();

    // we get Fn keys, etc,
    keypad(stdscr, true);

    // we'll deal with echoing ourselves
    noecho();

    // splash screen if no file
    printw("HexEditor Curses");  /* Print Hello World          */
    refresh();          /* Print it on to the real screen */

    HaxHexWindow hex;

    hex.SetPosition(10,10,10,10);

    int ch = getch();
    if(ch == KEY_F(1))
    {
        printw("F1 Key pressed");
    }
    else
    {   printw("The pressed key is ");
        attron(A_BOLD);
        printw("%c", ch);
        attroff(A_BOLD);
        hex.SetPosition(6,6,6,6);
    }

    // drop into input loop
    getch();

    // that's it, shut it down
    endwin();
    return 0;
}

