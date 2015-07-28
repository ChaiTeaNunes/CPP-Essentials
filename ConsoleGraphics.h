#ifndef __CG__
#define __CG__

/**
 * Console Graphics
 * @author	Chaiyawat Nunes	chaiteanunes@gmail.com
 * @version	1.0
 */

void cg_move(long row, long col);
long cg_kbhit();
long cg_getchar();
void cg_setColor(long foreground, long background);
int cg_getApproxColor(unsigned char r, unsigned char g, unsigned char b);
void cg_sleep(long ms);
long long cg_upTimeMS();
void cg_consoleSize(long & out_rows, long & out_columns);
inline void cg_waitForAnyKey() { while (!cg_kbhit()) { cg_sleep(1); } }
inline long cg_getch() { cg_waitForAnyKey(); return cg_getchar(); }

#include <stdio.h>

#ifdef _WIN32

#define CG_KEY_UP 18656
#define CG_KEY_LEFT 19424
#define CG_KEY_RIGHT 19936
#define CG_KEY_DOWN 20704

#define CG_COLOR_INTENSITY (1 << 3)
#define CG_COLOR_RED (1 << 2)
#define CG_COLOR_GREEN (1 << 1)
#define CG_COLOR_BLUE (1 << 0)

#define NOMINMAX
#include <windows.h>
#include <conio.h>
#include <time.h>

inline HANDLE * __stdOutputHandle() {
    static HANDLE g_h = 0;
    return &oldAttributes;
}

inline WORD * __oldAttributes() {
    static WORD oldAttributes;
    return &oldAttributes;
}

inline void __cg_release() {
    if (*__stdOutputHandle() != 0) {
        cg_setColor(*__oldAttributes() & 0xf, *__oldAttributes() & 0xf0);
        *__stdOutputHandle() = 0;
    }
}

inline void __g_init() {
    if (*__stdOutputHandle() == 0) {
        *__stdOutputHandle() = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo;
        GetConsoleScreenBufferInfo(*__stdOutputHandle(), &lpConsoleScreenBufferInfo);
        *__oldAttributes() = lpConsoleScreenBufferInfo.wAttributes;
        atexit(__cg_release);
    }
}

inline void cg_move(long row, long col) {
    if (col < 0) col = 0;
    if (row < 0) row = 0;
    COORD p = { (short)col, (short)row };
    __cg_init();
    SetConsoleCursorPosition(*__stdOutputHandle(), p);
}

inline long cg_kbhit() {
    __cg_init();
    return _kbhit() != 0;
}

inline long cg_getchar() {
    long input;
    if (!cg_kbhit()) return -1;
    input = _getch();
    switch ((char)input) {
    case '\0': case '\340':
        if (_kbhit()) {
            long nextByte = _getch();
            input |= (nextByte & 0xff) << 8;
        }
    }
    return input;
}

inline void cg_setColor(long foreground, long background) {
    __cg_init();
    if (foreground < 0){ foreground = (*__oldAttributes()) & 0xf; }
    if (background < 0){ background = (*__oldAttributes() & 0xf0) >> 4; }
    SetConsoleTextAttribute(*__stdOutputHandle(), (foreground & 0xf) | ((background & 0xf) << 4));
}

inline int cg_getApproxColor(unsigned char r, unsigned char g, unsigned char b) {
    int finalColor = 0;
    for (int threshhold = 160; threshhold > 32; threshhold -= 32) {
        if (finalColor == 0) {
            if (r >= threshhold) finalColor |= FOREGROUND_RED;
            if (g >= threshhold) finalColor |= FOREGROUND_GREEN;
            if (b >= threshhold) finalColor |= FOREGROUND_BLUE;
            if (finalColor != 0 && threshhold > 128) finalColor |= FOREGROUND_INTENSITY;
        }
        if (finalColor != 0) break;
    }
    return finalColor;
}

inline void cg_sleep(long ms) {
    Sleep(ms);
}

inline long long cg_upTimeMS() {
    return clock();
}

inline void cg_consoleSize(long & out_rows, long & out_columns) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    out_columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    out_rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

#else

#define CG_KEY_UP 4283163
#define CG_KEY_DOWN 4348699
#define CG_KEY_RIGHT 4414235
#define CG_KEY_LEFT 4479771

#define CG_COLOR_INTENSITY (1 << 3)
#define CG_COLOR_RED (1 << 2)
#define CG_COLOR_GREEN (1 << 1)
#define CG_COLOR_BLUE (1 << 0)

#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdlib.h>

inline termios * __oldTerminalIOSettings() {
    static termios oldTerminalIOSettings;
    return &oldTerminalIOSettings;
}

inline long * __initialized() {
    static long initted = 0;
    return &initted;
}

inline timeval * __g_startTime() {
    static timeval g_startTime = { 0, 0 };
    return &g_startTime;
}

inline fd_set * __g_fds() {
    static fd_set g_fds;
    return &g_fds;
}

inline void __cg_release() {
    if (*__initialized() != 0) {
        cg_setColor(-1, -1);
        *__initialized() = 0;
    }
}

inline void __cg__init() {
        if (*__initialized() == 0) {
                *__initialized() = 1;
                FD_ZERO(__g_fds());
                gettimeofday(__g_startTime(), NULL);
                atexit(__cg_release);
        }
}

inline void __cg_doConsoleInputMode() {
        __cg__init();
        tcgetattr(STDIN_FILENO, __oldTerminalIOSettings());
        termios currentTerminalIOSettings = *__oldTerminalIOSettings();
        currentTerminalIOSettings.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &currentTerminalIOSettings);
}

inline void __cg_undoConsoleInputMode() {
        tcsetattr(STDIN_FILENO, TCSANOW, __oldTerminalIOSettings());
}

inline long __cg_kbhitCheck() {
        static timeval g_tv_zero = { 0, 0 };
        long result;
        FD_SET(STDIN_FILENO, __g_fds());
        result = select(STDIN_FILENO + 1, __g_fds(), NULL, NULL, &g_tv_zero);
        return result && (FD_ISSET(0, __g_fds()));
}

inline long cg_kbhit() {
        long result;
        __cg_doConsoleInputMode();
        result = __cg_kbhitCheck();
        __cg_undoConsoleInputMode();
        return result;
}

inline long cg_getchar() {
    long buffer = -1;
    __cg_doConsoleInputMode();
    if (__cg_kbhitCheck()) {
            buffer = 0;
            read(STDIN_FILENO, (char *)&buffer, 1);
            switch (buffer) {
            case '\033':
                read(STDIN_FILENO, ((char *)&buffer) + 1, 1);
            switch (((char *)&buffer)[1]) {
            case '[':
                read(STDIN_FILENO, ((char *)&buffer) + 2, 1);
                break;
            }
            break;
        }
    }
    __cg_undoConsoleInputMode();
    return buffer;
}

inline void cg_move(long row, long col) {
        if (col < 0) col = 0;
        if (row < 0) row = 0;
        __cg__init();
        fflush(stdout);
        printf("\033[%d;%df", (int)row + 1, (int)col + 1);
        fflush(stdout);
}

inline void cg_setColor(long foreground, long background) {
        __cg__init();
        fflush(stdout);
        // colorRGB and colorGRAY usable for TTY (unix/linux) expanded console color
        if (foreground >= 0)
                printf("\033[38;5;%dm", (int)foreground);
        else
                printf("\033[39m");// default foreground color
        if (background >= 0)
                printf("\033[48;5;%dm", (int)background);
        else
                printf("\033[49m");// default background color
        fflush(stdout);
}

inline void cg_sleep(long a_ms) {
        __cg__init();
        usleep((useconds_t)(a_ms * 1000));
}

inline long long cg_upTimeMS() {
        static timeval now;
        static time_t seconds, useconds, ms;
        __cg__init();
        gettimeofday(&now, NULL);
        seconds = now.tv_sec - __g_startTime()->tv_sec;
        useconds = now.tv_usec - __g_startTime()->tv_usec;
        ms = seconds * 1000 + useconds / 1000;
        return ms;
}

inline void cg_consoleSize(long & out_rows, long & out_columns) {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        out_columns = w.ws_col;
        out_rows = w.ws_row;
}

inline int CG_COLOR_RGB8bit(int R, int G, int B){ return (16 + (B + (G * 6) + (R * 36))); }

inline int cg_getApproxColor(unsigned char r, unsigned char g, unsigned char b) {
        int finalColor = 0;
        if(r == g && g == b) {
                int gray = (r/255.0)*23;
                finalColor = (232+gray);
        } else {
                int R = (int)((r/255.0)*5);
                int G = (int)((g/255.0)*5);
                int B = (int)((b/255.0)*5);
                finalColor = (16+(B+(G*6)+(R*36)));
        }
        return finalColor;
}

inline int CG_COLOR_GRAYSCALE24(int gray) { return (232 + gray); }

#endif

#endif
