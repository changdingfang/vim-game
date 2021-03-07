// =======================================================================
//  FileName:     utils.h
//  Author:       dingfang
//  CreateDate:   2021-03-05 07:07:17
//  ModifyAuthor: dingfang
//  ModifyDate:   2021-03-06 13:07:56
// =======================================================================

#ifndef __UTILS_H__
#define __UTILS_H__

#include <sys/time.h>

#include <string>
#include <vector>

enum class ATTR_E
{
    NORMAL = 0,
    BLOD,
    REVERSE = 7,
};

enum class FG_E
{
    BLACK = 30,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    PURPLE,
    CYAN,
    WHITE
};

enum class BG_E
{
    BLACK = 40,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    PURPLE,
    CYAN,
    WHITE,
};

struct TerminalColor
{
    ATTR_E attr;
    FG_E fg;
    BG_E bg;
};

int getch(void);

void pr_winsize(int fd, int &h, int &w);

std::string calTime(const struct timeval &tv1, const struct timeval &tv2);

void getdir(const std::string &dir, std::vector<std::string> &dirVec);
void resuffix(std::vector<std::string> &strVec, const char *suffix);


#endif /* __UTILS_H__ */
