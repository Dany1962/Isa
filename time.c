#include <time.h>
#include <sys/timeb.h>



#include "temps.h"

int ftime_ok = FAUX;
int get_ms()
{
    struct timeb timebuffer;
    ftime(&timebuffer);
    if (timebuffer.millitm != 0)
        ftime_ok = VRAI;
    return (timebuffer.time * 1000) + timebuffer.millitm;
}

int controle_si_temps_depasse()
{
    if(get_ms() >= fin)
    {
        return VRAI;
    }

    return FAUX;
}
