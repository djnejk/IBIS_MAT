#ifndef BREAKS_H
#define BREAKS_H

struct Pause {
    int startHour;
    int startMinute;
    int endHour;
    int endMinute;
};

extern Pause breaks[];
extern const int breaksCount;

bool isBreak(int hour, int minute);

#endif
