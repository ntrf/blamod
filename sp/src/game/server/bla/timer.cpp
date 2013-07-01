#include "timer.h"

BlaTimer *timer()
{
    static BlaTimer *timer = new BlaTimer();
    return timer;
}
