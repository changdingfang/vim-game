// =======================================================================
//  FileName:     vim.cpp
//  Author:       dingfang
//  CreateDate:   2021-03-04 19:23:04
//  ModifyAuthor: dingfang
//  ModifyDate:   2021-03-08 09:43:16
// =======================================================================

#include "vim.h"
#include "utils.h"

#include <stdio.h>
#include <signal.h>
#include <sys/select.h>
#include <time.h>
#include <sys/time.h>

#include <iostream>
#include <string>
#include <fstream>

using namespace std;
using namespace vim;


Vim::Vim()
{
    this->updateTerminal();

    const char *ptmp = "hello world";
    win_ = 0;
    while (*ptmp)
    {
        TerminalColor tc = { ATTR_E::NORMAL, FG_E::WHITE, BG_E::BLACK };
        Objective obj;
        obj.value = *ptmp;
        obj.tc    = tc;
        obj.count = 0;
        obj.full  = false;
        obj.maxCount = 1;
        objVec_.push_back(obj);
        if (*ptmp >= 33 && *ptmp <= 126)
        {
            ++win_;
        }

        ++ptmp;
    }

    ::gettimeofday(&tm_, nullptr);
}

void Vim::help()
{
    fprintf(stdout, "%s+==============< Help >==============+==========< Info >=========+\n", helpSpace_.c_str());
    fprintf(stdout, "%s[]  k  ==>  up    ||  i  ==>  start ||  version:  0.0.1         []\n", helpSpace_.c_str());
    fprintf(stdout, "%s[]  j  ==>  down  || <CR>==>  exit  ||  author:   changdingfang []\n", helpSpace_.c_str());
    fprintf(stdout, "%s[]  h  ==>  left  ||  x  ==>  eat   ||                          []\n", helpSpace_.c_str());
    fprintf(stdout, "%s[]  l  ==>  right || f/F ==>  jump  ||                          []\n", helpSpace_.c_str());
    fprintf(stdout, "%s+====================================+===========================+\n", helpSpace_.c_str());
}

void Vim::beginShow()
{
    char cmd;

    string dirname(MAPS_DIR);
    vector<string> maps;
    getdir(dirname, maps);
    resuffixAndfilter(maps, MAP_SUFFIX);
    mpnames_ = maps;

    size_t select = 0;
    size_t mapsNum = this->begin(select);
    while (true)
    {
        if ((cmd = this->getcmd()) == 0)
        {
            usleep(80 * 1000);
            this->begin(select);
            continue;
        }
        switch (cmd)
        {
            case UP: if (select > 0) --select;              break;
            case DOWN: if (select < mapsNum - 1) ++select;  break;
            case LEFT: break;   /* 左翻页*/
            case RIGHT: break;  /* 右翻页*/
            case BEGIN: break;
            case EXIT1: 
            case EXIT2: exit(1);  /* 退出 */
            default: break; /* 无效输入 */
        }
        this->begin(select);

        if (cmd == BEGIN)
        {
            this->loadMap(select);
            break;
        }
    }
}

size_t Vim::begin(size_t select)
{
    /* 刷屏 */
    fprintf(stdout, "\033[H\033[2J");
    for (const auto &line : Logo)
    {
        fprintf(stdout, "%s%s\n", logoSpace_.c_str(), line.c_str());
    }

    fprintf(stdout, "\n");
    this->help();

    fprintf(stdout, "\n");
    fprintf(stdout, "%sGame Map\n", maplistSpace_.c_str());

    int st = select / ONE_PAGE;
    for (size_t i = st * ONE_PAGE; i < mpnames_.size() && i < (st + 1) * ONE_PAGE; ++i)
    {
        if (select == i)
        {
            fprintf(stdout, "%s%02ld. => %s\n", maplistSpace_.c_str(), i + 1, mpnames_[i].c_str());
            continue;
        }
        fprintf(stdout, "%s%02ld.    %s\n", maplistSpace_.c_str(), i + 1, mpnames_[i].c_str());
    }

    return mpnames_.size();
}

void Vim::show()
{
    /* 刷屏 */
    fprintf(stdout, "\033[H\033[2J");

    /* Logo */
    for (const auto &line : Logo)
    {
        fprintf(stdout, "%s%s\n", logoSpace_.c_str(), line.c_str());
    }

    /* 信息栏 */
    fprintf(stdout, "%s\n", boundary_.c_str());
    printf("# x: %2ld # win: %d # %s\n", this->getx(), win_, this->geterr().c_str());
    string objsstr;
    for (const auto &obj : objVec_)
    {
        objsstr.append("\033[");
        objsstr.append(to_string(static_cast<int>(obj.tc.attr)));
        objsstr.append(";");
        objsstr.append(to_string(static_cast<int>(obj.tc.fg)));
        objsstr.append(";");
        objsstr.append(to_string(static_cast<int>(obj.tc.bg)));
        objsstr.append("m");
        objsstr.append(1, obj.value);
        objsstr.append("\033[0m");
    }
    struct timeval tmv;
    ::gettimeofday(&tmv, nullptr);
    string useTime = calTime(tm_, tmv);
    /* printf("# y: %2ld # %s # time: %s #\n", this->gety() */
    /*         , objsstr.c_str() */
    /*         , useTime.c_str()); */
    printf("# y: %2ld # %s #\n", this->gety()
            , objsstr.c_str());

    /* body */
    fprintf(stdout, "%s\n", boundary_.c_str());
    for (size_t i = 0; i < mp_.size(); ++i)
    {
        fprintf(stdout, "%2ld %s\n", i, mp_[i].c_str());
    }
    fprintf(stdout, "%s\n", boundary_.c_str());
}

char Vim::getcmd()
{
    int ch = getch();
    if (ch < 0)
    {
        cerr << "get cmd error" << endl;
        exit(-1);
    }

    return ch;
}

bool Vim::check(char cmd)
{
    /*
    if (cmd >= '0' && cmd <= '9')
    {
        cmdQue_.push(cmd);
        return true;
    }
    */

    if (!cmdQue_.empty())
    {
        switch (cmdQue_.back())
        {
            case JUMP_F:    return this->jump(cmd);
            case JUMP_B:    return this->jump(cmd);
            default: break;
        }
    }

    switch (cmd)
    {
        case UP:        return this->up();
        case DOWN:      return this->down();
        case LEFT:      return this->left();
        case RIGHT:     return this->right();
        case EAT:       return this->eat();
        case JUMP_F:    
        case JUMP_B:    cmdQue_.push(cmd); return true;
        case EXIT1: 
        case EXIT2: exit(0);
        default: this->seterr("invalid cmd! cmd : ", cmd); return false;
    }

    return true;
}

bool Vim::update(char &ch)
{
    mp_[this->gety()][this->getx()] = oldch_;
    oldch_ = ch;
    ch = ROLE;

    return true;
}



bool Vim::up()
{
    if (p_.y <= 0 || !this->checkAndUpdate(this->getx(), this->gety() - 1))
    {
        return false;
    }

    --p_.y;
    return true;
}

bool Vim::down()
{
    if (p_.y >= mp_.size() -1 || !this->checkAndUpdate(this->getx(), this->gety() + 1))
    {
        return false;
    }

    ++p_.y;
    return true;
}

bool Vim::left()
{
    if (this->getx() <= 0 || !this->checkAndUpdate(this->getx() - 1, this->gety()))
    {
        return false;
    }

    --p_.x;
    return true;
}

bool Vim::right()
{
    if (this->getx() >= MAX_LINE - 1 || !this->checkAndUpdate(this->getx() + 1, this->gety()))
    {
        return false;
    }
    ++p_.x;
    return true;
}

bool Vim::eat()
{
    if (oldch_ >= 33 && oldch_ <= 126)
    {
        this->checkObjective();
    }

    oldch_ = ' ';
    return true;
}

bool Vim::jump(char cmd)
{
    char docmd = cmdQue_.back();
    queue<char> tmp;
    cmdQue_.swap(tmp);

    string::size_type pos = 0;
    string &row = mp_[this->gety()];
    string::size_type wallPos = 0;
    switch (docmd)
    {
        case JUMP_F:
            {
                if ((pos = row.find_first_of(cmd, this->getx() + 1)) == string::npos)
                {
                    this->seterr("invalid jump f cmd! cmd : ", cmd);
                    return false;
                }

                string::size_type wallPos = row.find_first_of(WALL_JUMP, this->getx() + 1);
                if (wallPos != string::npos && wallPos <= pos)
                {
                    this->seterr("invalid jump f cmd! cmd: ", cmd);
                    return false;
                }
                break;
            }
        case JUMP_B: 
            {
                if ((pos = row.find_last_of(cmd, this->getx() - 1)) == string::npos)
                {
                    this->seterr("invalid jump b cmd! cmd: ", cmd);
                    return false;
                }

                string::size_type wallPos = row.find_last_of(WALL_JUMP, this->getx() - 1);
                if (wallPos != string::npos && wallPos >= pos)
                {
                    this->seterr("invalid jump b cmd! cmd: ", cmd);
                    return false;
                }
                break;
            }
    }

    if (!this->checkAndUpdate(pos, this->gety()))
    {
        this->seterr("invalid jump cmd! cmd: ", cmd);
        return false;
    }

    p_.x = pos;
    return true;
}

bool Vim::checkObjective()
{
    for (auto &obj : objVec_)
    {
        if (oldch_ == obj.value && !obj.full)
        {
            if (++obj.count >= obj.maxCount)
            {
                obj.tc.fg = FG_E::YELLOW;
                obj.full = true;
                if (--win_ == 0)
                {
                    fprintf(stdout, "\033[H\033[2J");
                    fprintf(stdout, "\n\n\n");
                    for (const auto &row : WinLogo)
                    {
                        fprintf(stdout, "              %s\n", row.c_str());
                    }
                    fprintf(stdout, "\n\n\n");
                    exit(-1);
                }
            }
            return true;
        }
    }

    return false;
}

void Vim::loadMap(size_t mapid)
{
    string mapname(MAPS_DIR);
    mapname.append("/");
    mapname.append(mpnames_[mapid]);
    mapname.append(MAP_SUFFIX);
    ifstream ifs(mapname, ios::in);
    if (!ifs.good())
    {
        cerr << "open file error! filename: [" << mapname << "]" << endl;
        exit(-1);
    }

    string line;
    while (getline(ifs, line))
    {
        if (line.size() < MAX_LINE)
        {
            line.append(MAX_LINE - line.size(), ' ');
        }
        mp_.push_back(line);
    }

    p_.x = 1;
    p_.y = 4;
    oldch_ = ' ';

    if (mp_.size() < p_.y)
    {
        cerr << "map error!" << endl;
        exit(1);
    }
    mp_[p_.y][p_.x] = ROLE;
}

bool Vim::checkAndUpdate(int x, int y)
{
    char &ch = mp_[y][x];
    if (this->checkWall(ch))
    {
        return false;
    }
    this->update(ch);
    return true;
}

void Vim::updateTerminal()
{
    if (::isatty(STDIN_FILENO) == 0)
    {
        cerr << "get tty error" << endl;
        exit(1);
    }

    pr_winsize(STDIN_FILENO, tml_.h, tml_.w);

    logoSpace_.clear();
    logoSpace_.append((tml_.w - 60) / 2, ' ');

    maplistSpace_.clear();
    maplistSpace_.append((tml_.w - 30) / 2, ' ');

    helpSpace_.clear();
    helpSpace_.append((tml_.w - 65) / 2, ' ');

    boundary_.clear();
    boundary_.append(tml_.w, BOUNDARY);
}

void Vim::resuffixAndfilter(std::vector<std::string> &maps, std::string suffix)
{
    for (auto it = maps.begin(); it != maps.end();)
    {
        string::size_type pos = it->rfind(suffix);
        if (pos == string::npos)
        {
            it = maps.erase(it);
            continue;
        }

        *it  = it->substr(0, pos);
        ++it;
    }
}

void sigWinch(int sig)
{
    cout << "winch..." << endl;
}


int main(void)
{
    if (::signal(SIGWINCH, sigWinch) == SIG_ERR)
    {
        printf("signal failed!\n");
        exit(-1);
    }

    /* 直接读取终端输入 */
    system("stty -icanon");
    /* 关闭回显 */
    system("stty -echo");

    Vim v;
    v.beginShow();
    v.show();
    char cmd;
    while (true)
    {
        if ((cmd = v.getcmd()) == 0)
        {
            usleep(30 * 1000);
            v.show();
            continue;
        }

        v.errclear();
        if (!v.check(cmd))
        {
            if (v.errempty())
            {
                v.seterr("errinfo: invalid cmd! cmd: ", cmd);
            }
            v.show();
            continue;
        }

        v.show();
    }

    return 0;
}
