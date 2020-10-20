#if PM_DEBUG
//==============================================================================
/**
 * @file   BattleVideoPlayerDebug.cpp
 * @date   2017/05/24 Wed. 13:03:16
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <GameSys/include/GameManager.h>
#include  <GameSys/include/GameData.h>
#include  <Debug/DebugWin/include/DebugWinSystem.h>

#include  "./BattleVideoPlayerDebug.h"


namespace NetAppLib {
namespace BattleVideoPlayer {
namespace Debug {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_BattleVideoPlayerDebug
namespace FILELOCAL {
  bool IsTrigA(void)
  {
    gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

    return (pButton->IsTrigger(gfl2::ui::BUTTON_A));
  }


  VideoPlayerDataDebug*  ToThis(void* _pThis)
  {
    return static_cast<VideoPlayerDataDebug*>(_pThis);
  }

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  Update_LocalTimeVal(void* _pThis, gfl2::debug::DebugWinItem* pWinItem)
  {
    VideoPlayerDataDebug*  pThis = ToThis(_pThis);
    
    pThis->OnUpdate_TimeVal(pWinItem, 0);
  }


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  wchar_t*  Draw_LocalTimeVal( void* _pThis, gfl2::debug::DebugWinItem* pWinItem)
  {
    VideoPlayerDataDebug*  pThis = ToThis(_pThis);
    return  pThis->OnDraw_TimeVal(pWinItem, 0);
  }
}


// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
VideoPlayerDataDebug::VideoPlayerDataDebug(void)
  : m_pGroup_Root(NULL)
  , m_pGroups()
  , m_LocalTime()
  , m_LocalTimeTarget(0)
  /*  text  */
//  , m_Text()
{
  ::Debug::DebugWin::DebugWinWork*  devWork       = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData*               pMsgData      = devWork->GetMessageData();
  gfl2::heap::HeapBase*             pHeapBase     = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
  gfl2::debug::DebugWinGroup*       pCurrentGroup = NULL;

  /*  GiftGroup  */
  m_pGroup_Root                             = gfl2::debug::DebugWin_AddGroup(pHeapBase, L"VideoPlayerDataDebug", NULL);
//  m_pGroups[GROUP_ALBUM]                    = gfl2::debug::DebugWin_AddGroup(pHeapBase, L"Album",            m_pGroup_Root);
//  m_pGroups[GROUP_GIFTFLAG_ReceiveOnce]     = gfl2::debug::DebugWin_AddGroup(pHeapBase, L"Flag:ReceiveOnce", m_pGroup_Root);
//  m_pGroups[GROUP_GIFTFLAG_RandomDeliv]     = gfl2::debug::DebugWin_AddGroup(pHeapBase, L"Flag:Random",      m_pGroup_Root);
//  m_pGroups[GROUP_GIFTFLAG_ReceiveOnceADay] = gfl2::debug::DebugWin_AddGroup(pHeapBase, L"Flag:OnceADay",    m_pGroup_Root);

  /*  win  */
#define VALDEF(_ItemID, _CaptionID, _min, _max)   do{ \
                                                    m_ViewItemInfos[(_ItemID)].pThis      = this;  \
                                                    m_ViewItemInfos[(_ItemID)].pViewItem  = gfl2::debug::DebugWin_AddItemValueU32(pCurrentGroup, pHeapBase, pMsgData, _CaptionID, &m_ViewItemInfos[(_ItemID)].value, (_min), (_max)); \
                                                    m_ViewItemInfos[(_ItemID)].value      = (_min); \
                                                  }while(false)

#define VALDEFDIRECT(_ItemID, _Caption, _min, _max)   do{ \
                                                    m_ViewItemInfos[(_ItemID)].pThis      = this;  \
                                                    m_ViewItemInfos[(_ItemID)].pViewItem  = gfl2::debug::DebugWin_AddItemValueU32(pCurrentGroup, pHeapBase, _Caption, &m_ViewItemInfos[(_ItemID)].value, (_min), (_max)); \
                                                    m_ViewItemInfos[(_ItemID)].value      = (_min); \
                                                  }while(false)

#define PROCDEF(_CaptionID, _ptrFunc)             do{ \
                                                    gfl2::debug::DebugWin_AddItemUpdateFunc(pCurrentGroup, pHeapBase, pMsgData, _CaptionID, this, _ptrFunc);  \
                                                  }while(false)

#define PROCDEFDIRECT(_Caption, _ptrFunc)       do{ \
                                                    gfl2::debug::DebugWin_AddItemUpdateFunc(pCurrentGroup, pHeapBase, _Caption, this, _ptrFunc);  \
                                                  }while(false)

  /*    */
  pCurrentGroup = m_pGroup_Root;
  {
    /*    */
    gfl2::debug::DebugWin_AddItemUpdateFunc(pCurrentGroup, pHeapBase, L" LocalTime", this, FILELOCAL::Update_LocalTimeVal,  FILELOCAL::Draw_LocalTimeVal);

    VALDEFDIRECT(VALITEM_OverwriteMetaCreateTimeByLocalTime,  L"UploadedTime <== LocalTime",  0, 1);
  }

  /*  GROUP_GIFTFLAG_ReceiveOnce  */

#undef  PROCDEFDIRECT
#undef  VALDEFDIRECT
#undef  PROCDEF
#undef  VALDEF


  /*  text  */
//  m_Text.Initialize(m_pWork->GetAppHeap());
//  m_Text.Scale(0.8f, 0.8f);
  
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
VideoPlayerDataDebug::~VideoPlayerDataDebug()
{
//  m_Text.Finalize();

  /*  groups  */
  /*  sub  */
  for(u32 index = 0; index < GFL_NELEMS(m_pGroups); ++index)
  {
//    gfl2::debug::DebugWin_RemoveGroup(m_pGroups[index]);
  }
  /*  root  */
  gfl2::debug::DebugWin_RemoveGroup(m_pGroup_Root);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  VideoPlayerDataDebug::OnUpdate(void)
{
//  m_Text.CLS();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  VideoPlayerDataDebug::OnDraw(const gfl2::gfx::CtrDisplayNo displayNo)
{
//  m_Text.Draw(displayNo);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
#if defined( GF_PLATFORM_CTR )
void  VideoPlayerDataDebug::OverwriteServerTimeByLocalTimeIfEnable(nn::nex::DateTime & rNexDateTime)
{
  if(!m_ViewItemInfos[VALITEM_OverwriteMetaCreateTimeByLocalTime].value) return;

  rNexDateTime  = nn::nex::DateTime(
                   m_LocalTime.GetYear(),
                   m_LocalTime.GetMonth(),
                   m_LocalTime.GetDay(),
                   m_LocalTime.GetHour(),
                   m_LocalTime.GetMinute(),
                   0
                  );
}
#endif


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  VideoPlayerDataDebug::OnUpdate_TimeVal(gfl2::debug::DebugWinItem* pWinItem, const u32 mode)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  switch(mode)
  {
    case  0:  /*  localtime  */
    {
      const u32 localDateTargetMax  = 5;
      s32 year    = m_LocalTime.GetYear();
      s32 month   = m_LocalTime.GetMonth();
      s32 day     = m_LocalTime.GetDay();
      s32 hour    = m_LocalTime.GetHour();
      s32 minute  = m_LocalTime.GetMinute();
      
      if(pButton->IsTrigger(gfl2::ui::BUTTON_R))        ++m_LocalTimeTarget;
      else if(pButton->IsTrigger(gfl2::ui::BUTTON_L))   m_LocalTimeTarget += 4;

      m_LocalTimeTarget %= localDateTargetMax;

      if(pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
      {
        switch(m_LocalTimeTarget)
        {
          case  0:
            ++year;
            break;

          case  1:
            ++month;
            if(12 < month)  month = 1;
            break;

          case  2:
            ++day;
            if(31 < day)  day = 1;
            break;

          case  3:
            ++hour;
            hour  %= 24;
            break;

          case  4:
            ++minute;
            minute  %= 60;
            break;
        }
      }
      
      if(pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
      {
        switch(m_LocalTimeTarget)
        {
          case  0:
            --year;
            break;

          case  1:
            --month;
            if(month < 1)  month = 12;
            break;

          case  2:
            --day;
            if(day < 1)  day = 31;
            break;

          case  3:
            --hour;
            if(hour < 0) hour = 23;
            break;

          case  4:
            --minute;
            if(minute < 0)  minute  = 59;
            break;
        }
      }
      m_LocalTime.SetDay(1);

      m_LocalTime.SetYear(year);
      m_LocalTime.SetMonth(month);
      m_LocalTime.SetDay(day);
      m_LocalTime.SetHour(hour);
      m_LocalTime.SetMinute(minute);
    }
      break;
  }
}



//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
wchar_t*  VideoPlayerDataDebug::OnDraw_TimeVal(gfl2::debug::DebugWinItem* pWinItem, const u32 mode)
{
  wchar_t*        pWorkStr = gfl2::debug::DebugWin_GetWorkStr();
  const wchar_t*  pName    = pWinItem->GetName();
  s32       year      = 0;
  s32       month     = 0;
  s32       day       = 0;
  s32       hour      = 0;
  s32       minute    = 0;


  switch(mode)
  {
    case  0:  /*  localtime    */
      year    = m_LocalTime.GetYear();
      month   = m_LocalTime.GetMonth();
      day     = m_LocalTime.GetDay();
      hour    = m_LocalTime.GetHour();
      minute  = m_LocalTime.GetMinute();
      break;
  }

#define cL(_target)   (((mode == 0) && ((_target) == m_LocalTimeTarget)) ? ">" : "")
#define cR(_target)   (((mode == 0) && ((_target) == m_LocalTimeTarget)) ? "<" : "")

  swprintf(
    pWorkStr,
    gfl2::debug::DEBUGWIN_NAME_LEN,
    L"%ls[%s%04d%s/%s%02d%s/%s%02d%s %s%02d%s:%s%02d%s]",
    pWinItem->GetName(),
    cL(0), year,    cR(0),
    cL(1), month,   cR(1),
    cL(2), day,     cR(2),
    cL(3), hour,    cR(3),
    cL(4), minute,  cR(4)
  );

#undef  cL
#undef  cR



  return pWorkStr;
}





}  /*  namespace Debug  */
}  /*  namespace BattleVideoPlayer  */
}  /*  namespace NetAppLib  */
#endif  /*  #if PM_DEBUG                                     */
