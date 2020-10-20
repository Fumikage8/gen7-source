#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:          helper_Application.h
 Description: Helper file that does all HOME menu, POWER button, sleep and 
              applet library launching management.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#include <nn.h>
#include <nn/applet.h>
#include <nn/version.h>

/*---------------------------------------------------------------------------*/

#include "helper_Application.h"

/*---------------------------------------------------------------------------*/


namespace mw {
namespace mo {
namespace helper {
namespace application {

/*---------------------------------------------------------------------------*
  HOME menu management static variables.
 *---------------------------------------------------------------------------*/

static nn::os::CriticalSection s_SleepCriticalSection;
static nn::os::LightEvent      s_SleepManagementEvent;
static bool                    s_Initialized     = false;
static BeforeEventFunc         s_BeforeEventFunc = NULL;
static AfterEventFunc          s_AfterEventFunc  = NULL;
static void*                   s_pUserData       = NULL;

/*---------------------------------------------------------------------------*
  Static sleep management query callback .
 *---------------------------------------------------------------------------*/

static AppletQueryReply SleepQueryCallback(uptr arg)
{
    NN_UNUSED_VAR(arg);
    
    s_SleepManagementEvent.ClearSignal();
    
    if ( !nn::applet::IsActive() )
        return nn::applet::REPLY_ACCEPT;

    if (s_Initialized)
        return nn::applet::REPLY_LATER;
    else
        return nn::applet::REPLY_REJECT;
}

/*---------------------------------------------------------------------------*
  Static sleep management awake callback .
 *---------------------------------------------------------------------------*/

static void AwakeCallback (uptr arg)
{
    NN_UNUSED_VAR(arg);
    s_SleepManagementEvent.Signal();
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

void Initialize (void)
{
    if (s_Initialized == false)
    {
        s_SleepManagementEvent.Initialize(false);
        s_SleepCriticalSection.Initialize();
        //nn::applet::SetSleepQueryCallback(SleepQueryCallback, 0);
        //nn::applet::SetAwakeCallback(AwakeCallback, 0);

        s_Initialized = true;
    }
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

void Finalize (void)
{
    if (s_Initialized == true)
    {
        //nn::applet::SetSleepQueryCallback(NULL, 0);
        //nn::applet::SetAwakeCallback(NULL, 0);
        s_SleepManagementEvent.Finalize();
        s_SleepCriticalSection.Finalize();
        
        s_Initialized = false;
    }
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

void SetCallbacks (BeforeEventFunc beforeEventFunc, AfterEventFunc afterEventFunc)
{
    s_BeforeEventFunc = beforeEventFunc;
    s_AfterEventFunc = afterEventFunc;
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

void SetUserData (void* pUserData)
{
    s_pUserData = pUserData;
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

nn::os::CriticalSection& GetSleepCriticalSection (void)
{
    return s_SleepCriticalSection;
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

bool ProcessCloseStatus (void)
{
    /* Application close request managegment */
    if (nn::applet::IsExpectedToCloseApplication())
    {
        if (s_BeforeEventFunc != NULL)
            s_BeforeEventFunc(s_pUserData, EVENT_CLOSE);
        return true;
    }

    return false;
}

/*---------------------------------------------------------------------------*
  
 *---------------------------------------------------------------------------*/

bool ProcessStatus (void)
{
    if (s_Initialized == false)
        return false;

    /* HOME button managegment */
    if (nn::applet::IsExpectedToProcessHomeButton())
    {
        if (s_BeforeEventFunc != NULL)
            s_BeforeEventFunc(s_pUserData, EVENT_HOME_BUTTON);

        GetSleepCriticalSection().Enter();
        nn::applet::DisableSleep();

        nn::applet::ProcessHomeButton();
        AppletWakeupState appletResult = nn::applet::WaitForStarting();
        NN_UNUSED_VAR(appletResult);

        nn::applet::EnableSleep(true);
        GetSleepCriticalSection().Leave();
        
        if (nn::applet::IsExpectedToCloseApplication())
            return true;
                        
        if (s_AfterEventFunc != NULL)
            s_AfterEventFunc(s_pUserData, EVENT_HOME_BUTTON);
    }

    /* POWER button managegment */
    if (nn::applet::IsExpectedToProcessPowerButton())
    {
        if (s_BeforeEventFunc != NULL)
            s_BeforeEventFunc(s_pUserData, EVENT_POWER_BUTTON);

        GetSleepCriticalSection().Enter();
        nn::applet::DisableSleep();

        nn::applet::ProcessPowerButton();
        nn::applet::WaitForStarting();

        nn::applet::EnableSleep(true);
        GetSleepCriticalSection().Leave();
        
        if (nn::applet::IsExpectedToCloseApplication())
            return true;
        
        if (s_AfterEventFunc != NULL)
            s_AfterEventFunc(s_pUserData, EVENT_POWER_BUTTON);
    }

    /* Sleep managegment */
    if (nn::applet::IsExpectedToReplySleepQuery())
    {
        if (GetSleepCriticalSection().TryEnter())
        {
            if (s_BeforeEventFunc != NULL)
                s_BeforeEventFunc(s_pUserData, EVENT_SLEEP);

            nn::applet::ReplySleepQuery(nn::applet::REPLY_ACCEPT);
            s_SleepManagementEvent.Wait();

            GetSleepCriticalSection().Leave();

            if (s_AfterEventFunc != NULL)
                s_AfterEventFunc(s_pUserData, EVENT_SLEEP);
        }
    }

    /* Application close request managegment */
    if (ProcessCloseStatus() == true)
        return true;

    return false;
}

/*---------------------------------------------------------------------------*/

} // end of namespace application
} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/
#endif// defined(GF_PLATFORM_CTR)
