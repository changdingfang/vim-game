// =======================================================================
//  FileName:     vim.h
//  Author:       dingfang
//  CreateDate:   2021-03-04 19:13:48
//  ModifyAuthor: dingfang
//  ModifyDate:   2021-03-07 14:42:38
// =======================================================================

#ifndef __VIM_H__
#define __VIM_H__

#include "utils.h"

#include <unistd.h>
#include <stdio.h>
#include <termios.h>

#include <string>
#include <vector>
#include <queue>


namespace vim
{

    const std::vector<std::string> Logo = { 
        { "        _"},
        { "__   __(_) _,__ ___     __ _   __ _  _,__ ___    ___" },
        { "\\ \\ / /| ||  _ `_  \\   / _` | / _` ||  _ ` _ \\  / _ \\" },
        { " \\ V / | || | | | | | | (_| || (_| || | | | | ||  __/" },
        { "  \\_/  |_||_| |_| |_|  \\__, | \\__,_||_| |_| |_| \\___|" },
        { "                       |___/" }
    };

    const std::vector<std::string> WinLogo = {
        { "__      __  _            " },
        { "\\ \\    / / (_)    _ _    " },
        { " \\ \\/\\/ /  | |   | ' \\   " },
        { "  \\_/\\_/  _|_|_  |_||_|  " },
        { "_|\"\"\"\"\"|_|\"\"\"\"\"|_|\"\"\"\"\"| " },
        { "\"`-0-0-'\"`-0-0-'\"`-0-0-' " }
    };

    const std::string MAPS_DIR      = "./maps";
    const std::string MAP_SUFFIX    = ".map";
    const int MAX_LINE      = 80;

    const char BOUNDARY = '=';

    const char ROLE     = '#';
    const char CREEP    = '*';

    const char EXIT1    = 10;  /* \n   */
    const char EXIT2    = 13;  /* 换行 */
    const char ESC      = 27;

    const char CLEAR    = 27;  /* Esc  */

    const char WALL1    = '|';
    const char WALL2    = '-';
    const char WALL3    = '+';
    const char WALL4    = '_';
    const std::string WALL = "|-+_\\/{}()[]=";
    const std::string WALL_JUMP = "|-_\\/{}()=";

    const size_t ONE_PAGE = 7;
    const char BEGIN    = 'i';
    const char UP       = 'k';
    const char DOWN     = 'j';
    const char LEFT     = 'h';
    const char RIGHT    = 'l';
    const char EAT      = 'x';
    const char JUMP_F   = 'f';
    const char JUMP_B   = 'F';

    struct Point
    {
        size_t x;
        size_t y;
    };

    struct Terminal
    {
        int h;
        int w;
    };

    struct Objective
    {
        char                    value;
        bool                    full;
        short int               count;
        short int               maxCount;
        struct TerminalColor    tc;
    };

    class Vim
    {
    public:
        Vim();
        ~Vim() = default;

        void help();
        void beginShow();

        void show();

        char getcmd();
        bool check(char cmd);
        bool update(char &);

        size_t getx() const { return p_.x; }
        size_t gety() const { return p_.y; }

        std::string geterr() const { return errinfo_; }
        void errclear() { errinfo_.clear(); }
        bool errempty() { return errinfo_.empty(); }
        void seterr(const std::string err, char cmd) 
        { 
            errinfo_.append("\033[");
            TerminalColor tc = { ATTR_E::NORMAL, FG_E::RED, BG_E::BLACK };
            errinfo_.append(std::to_string(static_cast<int>(tc.attr)));
            errinfo_.append(";");
            errinfo_.append(std::to_string(static_cast<int>(tc.fg)));
            errinfo_.append(";");
            errinfo_.append(std::to_string(static_cast<int>(tc.bg)));
            errinfo_.append("m");
            errinfo_.append(err); 
            if (cmd >= 33 && cmd <= 126)
            {
                errinfo_.append(1, cmd); 
            }
            else
            {
                errinfo_.append(std::to_string(cmd)); 
                errinfo_.append(" acsii"); 
            }
            errinfo_.append("\033[0m");
        }

    private:
        size_t begin(size_t select);
        void loadMap(size_t mapid);

        bool checkAndUpdate(int, int);
        bool checkWall(char ch) { return WALL.find(ch) != std::string::npos; }
        bool up();
        bool down();
        bool left();
        bool right();
        bool eat();
        bool jump(char cmd);

        bool checkObjective();

        void updateTerminal();

        void resuffixAndfilter(std::vector<std::string> &maps, std::string suffix);
    private:
        std::vector<std::string> head_;
        std::vector<std::string> mpnames_;
        std::vector<std::string> mp_;
        char oldch_;
        Point p_;
        struct timeval tm_;

        std::queue<char> cmdQue_;

        Terminal tml_;
        std::string logoSpace_;
        std::string maplistSpace_;
        std::string helpSpace_;
        std::string boundary_;

        std::vector<Objective>  objVec_;
        short int               win_;

        std::string errinfo_;
    };

}; /* namespace end of vim */


#endif /* __VIM_H__ */
