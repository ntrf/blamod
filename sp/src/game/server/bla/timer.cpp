#include "cbase.h"

#include "timer.h"

BlaTimer * BlaTimer::timer()
{
    static BlaTimer *timer = new BlaTimer();
    return timer;
}
