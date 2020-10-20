#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:        helper_Application.h
 Description: Helper file that does all HOME menu, POWER button, sleep and 
              applet library launching management.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#if 0
#define __HELPER_APPLICATION_H__

#include <nn.h>
#include <nn/os.h>

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*/

namespace application
{
    enum EventType
    {
        EVENT_HOME_BUTTON,
        EVENT_POWER_BUTTON,
        EVENT_SLEEP,
        EVENT_CLOSE,
        EVENT_APPLET_LIBRARY
    };

    typedef void (*BeforeEventFunc) (void* pUserData, EventType type);
    typedef void (*AfterEventFunc)  (void* pUserData, EventType type);

    void Initialize         (void);
    void Finalize           (void);

    void SetCallbacks       (BeforeEventFunc beforeHomeButtonPress, AfterEventFunc afterHomeButtonPress);
    void SetUserData        (void* pUserData);
    
    bool ProcessCloseStatus (void);
    bool ProcessStatus      (void);

    nn::os::CriticalSection&  GetSleepCriticalSection (void);
}

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif //__HELPER_APPLICATION_H__
#endif// defined(GF_PLATFORM_CTR)
