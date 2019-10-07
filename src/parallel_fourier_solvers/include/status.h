#pragma once

enum Stat {
    OK,
    ERROR,  // error
    STOP  // without errors, cancel program
};

inline Stat concStatus(Stat a, Stat b) {
    if (a == ERROR || b == ERROR) return ERROR;
    if (a == STOP || b == STOP) return STOP;
    return OK;
}