#include "breaks.h"

// Časy přestávek
Pause breaks[] = {
    {7, 55, 8, 0},
    {8, 45, 8, 55},
    {9, 40, 10, 0},
    {10, 45, 10, 55},
    {11, 40, 11, 50},
    {12, 35, 12, 40},
    {13, 25, 13, 30},
    {14, 15, 14, 20}
};

const int breaksCount = sizeof(breaks) / sizeof(breaks[0]);

bool isBreak(int hour, int minute) {
    for (int i = 0; i < breaksCount; i++) {
        Pause p = breaks[i];
        if ((hour > p.startHour || (hour == p.startHour && minute >= p.startMinute)) &&
            (hour < p.endHour || (hour == p.endHour && minute < p.endMinute))) {
            return true;
        }
    }
    return false;
}
