#if PM_DEBUG
//==============================================================================
/**
 * @file   RotomToneGeneratorDebug.cpp
 * @date   2017/04/27 Thu. 16:50:16
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <Sound/include/Sound.h>
#include  <GameSys/include/GameManager.h>
#include  <System/include/Timer/gfl2_PerformanceCounter.h>
#include  <System/include/HeapDefine.h>

#include  "./RotomToneGeneratorDebug.h"

namespace app {
namespace tool {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_RotomToneGeneratorDebug
namespace FILELOCAL {
  static const int SeTable[16] =
  {
    SEQ_SE_MJ_SYS_046, SEQ_SE_MJ_SYS_047, SEQ_SE_MJ_SYS_048, SEQ_SE_MJ_SYS_049,
    SEQ_SE_MJ_SYS_050, SEQ_SE_MJ_SYS_051, SEQ_SE_MJ_SYS_052, SEQ_SE_MJ_SYS_053,
    SEQ_SE_MJ_SYS_054, SEQ_SE_MJ_SYS_055, SEQ_SE_MJ_SYS_056, SEQ_SE_MJ_SYS_057,
    SEQ_SE_MJ_SYS_058, SEQ_SE_MJ_SYS_059, SEQ_SE_MJ_SYS_060, SEQ_SE_MJ_SYS_061,
  };
  static const u32 SETableElems = GFL_NELEMS(SeTable);


  /*
  */
  bool IsTrig(const u32 buttonID)
  {
    gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

    return (pButton->IsTrigger(buttonID));
  }

  bool IsTrigA(void)      {return IsTrig(gfl2::ui::BUTTON_A);}
  bool IsTrigX(void)      {return IsTrig(gfl2::ui::BUTTON_X);}
  bool IsTrigRight(void)  {return IsTrig(gfl2::ui::BUTTON_RIGHT);}
  bool IsTrigLeft(void)   {return IsTrig(gfl2::ui::BUTTON_LEFT);}


  void  SE_OK(gfl2::debug::DebugWinItem* pItem) {pItem->PlaySystemSound( gfl2::debug::SE_ACCEPT );}
  
  void  dummy(void* _pThis, gfl2::debug::DebugWinItem* pItem){}

}


// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
RotomToneGenerator::RotomToneGeneratorDebug::RotomToneGeneratorDebug(RotomToneGenerator* pOwner)
  : m_pOwner(pOwner)
  , m_pGroup_Root(NULL)
  , m_IntervalBegin(0)
{
  {
    gfl2::heap::HeapBase* pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );

    /*  group  */
    m_pGroup_Root = gfl2::debug::DebugWin_AddGroup(pHeapBase, L"RotomToneGenerator", NULL);

  /*  win  */
#define VALDEF(_ItemID, _CaptionID, _min, _max)   do{ \
                                                    m_ItemInfos[(_ItemID)].pThis      = this;  \
                                                    m_ItemInfos[(_ItemID)].pViewItem  = gfl2::debug::DebugWin_AddItemValueInt(m_pGroup_Root, pHeapBase, pMsgData, _CaptionID, &m_ItemInfos[(_ItemID)].value, (_min), (_max)); \
                                                    m_ItemInfos[(_ItemID)].value      = (_min); \
                                                  }while(false)
#define VALDEFDIRECT(_ItemID, _Caption, _min, _max)   do{ \
                                                    m_ItemInfos[(_ItemID)].pThis      = this;  \
                                                    m_ItemInfos[(_ItemID)].pViewItem  = gfl2::debug::DebugWin_AddItemValueInt(m_pGroup_Root, pHeapBase, _Caption, &m_ItemInfos[(_ItemID)].value, (_min), (_max)); \
                                                    m_ItemInfos[(_ItemID)].value      = (_min); \
                                                  }while(false)

#define PROCDEF(_CaptionID, _ptrFunc)             do{ \
                                                    gfl2::debug::DebugWin_AddItemUpdateFunc(m_pGroup_Root, pHeapBase, pMsgData, _CaptionID, this, _ptrFunc);  \
                                                  }while(false)

#define PROCDEFDIRECT(_Caption, _ptrFunc)       do{ \
                                                    gfl2::debug::DebugWin_AddItemUpdateFunc(m_pGroup_Root, pHeapBase, _Caption, this, _ptrFunc);  \
                                                  }while(false)

    VALDEFDIRECT(VALITEM_tone0,     L"Tone 0:",   0, 15);
    VALDEFDIRECT(VALITEM_tone1,     L"Tone 1:",   0, 15);
    VALDEFDIRECT(VALITEM_tone2,     L"Tone 2:",   0, 15);
    VALDEFDIRECT(VALITEM_tone3,     L"Tone 3:",   0, 15);
    VALDEFDIRECT(VALITEM_interval,  L"interval(frame):", 1, 30);

    PROCDEFDIRECT(L"-----", FILELOCAL::dummy);
    PROCDEFDIRECT(L"request", Request);

#undef  PROCDEFDIRECT
#undef  VALDEFDIRECT
#undef  PROCDEF
#undef  VALDEF
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
RotomToneGenerator::RotomToneGeneratorDebug::~RotomToneGeneratorDebug()
{
  gfl2::debug::DebugWin_RemoveGroup(m_pGroup_Root);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  RotomToneGenerator::RotomToneGeneratorDebug::Tone(const u32 sequence)
{
  u32 itemID = VALITEM_tone0;

  switch(sequence)
  {
    case  0:  itemID  = VALITEM_tone0;  break;
    case  1:  itemID  = VALITEM_tone1;  break;
    case  2:  itemID  = VALITEM_tone2;  break;
    case  3:  itemID  = VALITEM_tone3;  break;
    
    default:
      /*  nop  */
      return;
  }
  
  
  {
    const u32 toneIndex = m_ItemInfos[itemID].value;
    
    if((0 <= toneIndex) && (toneIndex < FILELOCAL::SETableElems))  Sound::PlaySE(FILELOCAL::SeTable[toneIndex], 0);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  RotomToneGenerator::RotomToneGeneratorDebug::PrintInterval(const u32 seq)
{
  const u64   currentMilliSecond = gfl2::system::Timer::PerformanceCounter::GetMilliSecond();

  if(seq)
  {
    GFL_PRINT("tone interval: %lld[msec]\n", currentMilliSecond  - m_IntervalBegin);
  }
  
  m_IntervalBegin = currentMilliSecond;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void RotomToneGenerator::RotomToneGeneratorDebug::Request(void* _pThis, gfl2::debug::DebugWinItem* pItem)
{
  if(FILELOCAL::IsTrigA())
  {
    RotomToneGeneratorDebug*  pThis = static_cast<RotomToneGeneratorDebug*>(_pThis);
    
    pThis->FixupTone();
    pThis->m_pOwner->Request(static_cast<MonsNo>(0xffff), 0, pml::SEX_MALE);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  RotomToneGenerator::RotomToneGeneratorDebug::FixupTone(void)
{
  s32&  rlastTone = m_ItemInfos[VALITEM_tone3].value;

  rlastTone = (m_ItemInfos[VALITEM_tone0].value + m_ItemInfos[VALITEM_tone1].value + m_ItemInfos[VALITEM_tone2].value) & 0x0f;
  if(rlastTone == m_ItemInfos[VALITEM_tone2].value)
  {
    rlastTone = (++rlastTone) & 0x0f;
  }
}




}  /*  namespace tool  */
}  /*  namespace app  */
#endif  /*  PM_DEBUG  */
