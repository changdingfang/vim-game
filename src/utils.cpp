// =======================================================================
//  FileName:     utils.cpp
//  Author:       dingfang
//  CreateDate:   2021-03-05 07:07:55
//  ModifyAuthor: dingfang
//  ModifyDate:   2021-03-06 14:22:10
// =======================================================================

#include "utils.h"

#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

using namespace std;

int getch(void)
{
    struct termios tm, tm_old;
    int fd = STDIN_FILENO;

    if (tcgetattr(fd, &tm) < 0)
    {
        return -1;
    }

    tm_old = tm;
    cfmakeraw(&tm);
    if (tcsetattr(fd, TCSANOW, &tm) < 0)
    {
        return -1;
    }

#if 0
    int flag;
    flag = fcntl(fd, F_GETFL, 0);
    flag |= FNDELAY;
    fcntl(fd, F_SETFL, flag);
#endif

    char ch[1] = {0};
    /* ch[0] = getchar(); */
    read(fd, ch, 1);
    if (tcsetattr(fd, TCSANOW, &tm_old) < 0)
    {
        return -1;
    }

    return ch[0];
}

void pr_winsize(int fd, int &h, int &w)
{
    struct winsize size;
    if (ioctl(fd, TIOCGWINSZ, (char *)&size) < 0)
    {
        h = -1;
        w = -1;
        return ;
    }

    h = size.ws_row;
    w = size.ws_col;

    return ;
}

std::string calTime(const struct timeval &tv1, const struct timeval &tv2)
{
    int us = (tv2.tv_usec - tv1.tv_usec) / 10000;
    int s  = tv2.tv_sec - tv1.tv_sec;

    if (us < 0)
    {
        us += 100;
        --s;
    }

    return std::to_string(s).append(".").append(std::to_string(us));
}

void getdir(const std::string &dir, std::vector<std::string> &dirVec)
{
    DIR *pdir = opendir(dir.c_str());
    if (pdir == nullptr)
    {
        return ;
    }
    struct dirent *dirp;
    while ((dirp = readdir(pdir)) != nullptr)
    {
        if (::strcmp(dirp->d_name, ".") == 0
                || ::strcmp(dirp->d_name, "..") == 0)
        {
            continue;
        }
        struct stat statbuf;
        string fullpath(dir);
        fullpath.append("/");
        fullpath.append(dirp->d_name);
        if (::lstat(fullpath.c_str(), &statbuf) < 0)
        {
            continue;
        }
        if (S_ISREG(statbuf.st_mode) != 0)
        {
            dirVec.push_back(dirp->d_name);
        }
    }
}

void resuffix(std::vector<std::string> &strVec, const char *suffix)
{
    for (auto &str : strVec)
    {
        string::size_type pos = str.rfind(suffix);
        if (pos == string::npos)
        {
            continue;
        }

        str = str.substr(0, pos);
    }

    return ;
}
