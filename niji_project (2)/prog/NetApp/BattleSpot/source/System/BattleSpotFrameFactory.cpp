
//==============================================================================
/**
 * @file   BattleSpotFrameFactory.cpp
 * @date   2015/10/02 Fri. 12:47:01
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../BattleSpotFrameDef.h"
#include  "./BattleSpotWork.h"

/*  frames  */
#include  "../Frames/BattleSpotFrame_Welcome.h"
#include  "../Frames/BattleSpotFrame_FreeBattleEntry.h"
#include  "../Frames/BattleSpotFrame_RatingBattleEntry.h"
#include  "../Frames/BattleSpotFrame_InternetMatchEntry.h"
#include  "../Frames/BattleSpotFrame_FriendMatchEntry.h"

#include  "../Frames/BattleSpotFrame_FreeBattleMatching.h"
#include  "../Frames/BattleSpotFrame_RatingBattleMatching.h"
#include  "../Frames/BattleSpotFrame_FreeAfterBattle.h"
#include  "../Frames/BattleSpotFrame_RatingAfterBattle.h"
#include  "../Frames/BattleSpotFrame_RatingAfterRuleSelect.h"

#include  "../Frames/BattleSpotFrame_InternetBattleMatching.h"
#include  "../Frames/BattleSpotFrame_InternetAfterBattle.h"

#include  "../Frames/BattleSpotFrame_FriendBattleMatching.h"
#include  "../Frames/BattleSpotFrame_FriendAfterBattle.h"

#include  "../Frames/BattleSpotFrame_UpdateDisconnectCount.h"

#include  "../Frames/BattleSpotFrame_ViewTest.h"

#include  "./BattleSpotFrameFactory.h"


namespace NetApp      {
namespace BattleSpot  {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     FrameFactory
  *  @brief    ctor
  */
//------------------------------------------------------------------
FrameFactory::FrameFactory(BattleSpotWork* pWork)
  : m_pWork(pWork)
{
}


//------------------------------------------------------------------
/**
  *  @func     ~FrameFactory
  *  @brief    dtor
  */
//------------------------------------------------------------------
FrameFactory::~FrameFactory()
{
}


/*
  implement IApplciationFrameFactory
*/
//------------------------------------------------------------------
/**
  *  @func     CreateFrame
  *  @brief    Frameの生成
  */
//------------------------------------------------------------------
NetAppLib::System::NetAppFrameBase* FrameFactory::CreateNetAppFrame(NetAppLib::System::FrameID frameID)
{
  NetAppLib::System::NetAppFrameBase* pFrame    = NULL;
  gfl2::heap::HeapBase*               pHeapBase = m_pWork->GetAppHeap()->GetDeviceHeap();


  switch(frameID)
  {
    case  FRAME_Welcome:              pFrame  = GFL_NEW(pHeapBase)  Frame_Welcome(m_pWork);             break;
    case  FRAME_FreeBattleEntry:      pFrame  = GFL_NEW(pHeapBase)  Frame_FreeBattleEntry(m_pWork);     break;
    case  FRAME_RatingBattleEntry:    pFrame  = GFL_NEW(pHeapBase)  Frame_RatingBattleEntry(m_pWork);   break;
    case  FRAME_InternetMatchEntry:   pFrame  = GFL_NEW(pHeapBase)  Frame_InternetMatchEntry(m_pWork);  break;
    case  FRAME_FriendMatchEntry:     pFrame  = GFL_NEW(pHeapBase)  Frame_FriendMatchEntry(m_pWork);    break;

    case  FRAME_FreeBattleMatching:      pFrame  = GFL_NEW(pHeapBase)  Frame_FreeBattleMatching(m_pWork);     break;
    case  FRAME_FreeAfterBattle:    pFrame  = GFL_NEW(pHeapBase)  Frame_FreeAfterBattle(m_pWork);   break;
    case  FRAME_RatingBattleMatching:   pFrame  = GFL_NEW(pHeapBase)  Frame_RatingBattleMatching(m_pWork);  break;
    case  FRAME_RatingAfterBattle:     pFrame  = GFL_NEW(pHeapBase)  Frame_RatingAfterBattle(m_pWork);    break;
    case  FRAME_RatingAfterRuleSelect:     pFrame  = GFL_NEW(pHeapBase)  Frame_RatingAfterRuleSelect(m_pWork);    break;
    case  FRAME_InternetBattleMatching: pFrame = GFL_NEW(pHeapBase) Frame_InternetBattleMatching(m_pWork); break;
    case  FRAME_InternetAfterBattle: pFrame = GFL_NEW(pHeapBase) Frame_InternetAfterBattle(m_pWork); break;
    case  FRAME_FriendBattleMatching: pFrame = GFL_NEW(pHeapBase) Frame_FriendBattleMatching(m_pWork); break;
    case  FRAME_FriendAfterBattle: pFrame = GFL_NEW(pHeapBase) Frame_FriendAfterBattle(m_pWork); break;
    case  FRAME_UpdateDisconnectCount: pFrame = GFL_NEW(pHeapBase) Frame_UpdateDisconnectCount(m_pWork); break;

#if PM_DEBUG
    case  FRAME_ViewTest:             pFrame  = GFL_NEW(pHeapBase)  Frame_ViewTest(m_pWork);            break;
#endif

    default:
      GFL_ASSERT(0);//@fix
      GFL_PRINT("%s(%d):[warning]: unknown frameID(%d)\n", __FILE__, __LINE__, static_cast<u32>(frameID));
      pFrame  = GFL_NEW(pHeapBase)  Frame_Welcome(m_pWork);             break;
  }


  return pFrame;
}



} /*  namespace BattleSpot */
} /*  namespace NetApp    */


