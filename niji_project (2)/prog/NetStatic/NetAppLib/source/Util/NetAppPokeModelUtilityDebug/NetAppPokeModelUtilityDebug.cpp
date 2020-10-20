#if PM_DEBUG

//==============================================================================
/**
 * @file   NetAppPokeModelUtilityDebug.cpp
 * @date   2016/06/01 Wed. 12:56:21
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <Debug/DebugWin/include/DebugWinSystem.h>

#include  "GameSys/include/GameManager.h"
#include  "GameSys/include/GameData.h"

#include  <NetStatic/NetAppLib/include/System/ApplicationWorkBase.h>

#include "niji_conv_header/message/debug/msg_debug_menu.h"
#include  "./NetAppPokeModelUtilityDebug.h"


// =============================================================================
/**/
//==============================================================================
namespace
{
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



namespace NetAppLib {
namespace Util      {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
NetAppPokeModelUtility::NetAppPokeModelUtilityDebug::NetAppPokeModelUtilityDebug(NetAppPokeModelUtility* pOwner)
  : m_pOwner(pOwner)
  , m_pGroup_Root(pOwner->m_pParentDebugWinGroup)
  , m_SimpleParam()
  , m_PersonalData(pOwner->m_pWorkBase->GetDeviceHeap())
{
  ::Debug::DebugWin::DebugWinWork*  devWork   = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData*               pMsgData  = devWork->GetMessageData();
  gfl2::heap::HeapBase*             pHeapBase = m_pOwner->m_pWorkBase->GetDeviceHeap();



  /*  group  */
  m_pGroup_Root = gfl2::debug::DebugWin_AddGroup(pHeapBase, L"NetAppPokeModelUtil", m_pOwner->m_pParentDebugWinGroup);


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

    VALDEFDIRECT(VALITEM_slot,  L"Slot",  0, m_pOwner->m_Slots-1);

    PROCDEFDIRECT(L"first", First);
    PROCDEFDIRECT(L"next",  Next);


    PROCDEFDIRECT(L"-----", dummy);

    VALDEFDIRECT(VALITEM_monsNo,  L"mons:",  0, MONSNO_END);
    VALDEFDIRECT(VALITEM_formNo,  L"form:",  0, MAX_FORM_NUM-1);
    VALDEFDIRECT(VALITEM_gender,  L"gender", 0, pml::SEX_NUM-1);
    VALDEFDIRECT(VALITEM_rare,    L"rare",   0, 1);
    VALDEFDIRECT(VALITEM_isEgg,   L"egg",    0, 1);

    PROCDEFDIRECT(L"set", Set);

    PROCDEFDIRECT(L"-----", dummy);

    VALDEFDIRECT(VALITEM_isDrawFrame,   L"frame",  0, 1);
    VALDEFDIRECT(VALITEM_ofsX,          L"ofsX",   -512, 512);
    VALDEFDIRECT(VALITEM_ofsY,          L"ofsY",   -512, 512);
    VALDEFDIRECT(VALITEM_ofsSize,       L"ofsWH",  -512, 512);

#undef  PROCDEFDIRECT
#undef  VALDEFDIRECT
#undef  PROCDEF
#undef  VALDEF

  m_ItemInfos[VALITEM_ofsX].value     = 0;
  m_ItemInfos[VALITEM_ofsY].value     = 0;
  m_ItemInfos[VALITEM_ofsSize].value  = 0;

}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
NetAppPokeModelUtility::NetAppPokeModelUtilityDebug::~NetAppPokeModelUtilityDebug()
{
  gfl2::debug::DebugWin_RemoveGroup(m_pGroup_Root);
  m_pGroup_Root = NULL;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void NetAppPokeModelUtility::NetAppPokeModelUtilityDebug::First(  void* _pThis, gfl2::debug::DebugWinItem* pItem)
{
  if(IsTrigA() || IsTrigRight())
  {
    NetAppPokeModelUtilityDebug*  pThis = static_cast<NetAppPokeModelUtilityDebug*>(_pThis);
    const u32 slot  = pThis->m_ItemInfos[VALITEM_slot].value;

    NetAppLib::Debug::PokeToolSimpleParamDebugger::SetFirst(pThis->m_SimpleParam, &(pThis->m_PersonalData));

    pThis->ToInfo();

    pThis->m_pOwner->RequestEx(pThis->m_pOwner->GetEnvRenderPlaceType(slot), slot, pThis->m_SimpleParam, pThis->m_pOwner->GetEnvTargetLayer(slot));
    SE_OK(pItem);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void NetAppPokeModelUtility::NetAppPokeModelUtilityDebug::Next(   void* _pThis, gfl2::debug::DebugWinItem* pItem)
{
  NetAppPokeModelUtilityDebug*  pThis = static_cast<NetAppPokeModelUtilityDebug*>(_pThis);
  const u32                     slot  = pThis->m_ItemInfos[VALITEM_slot].value;

  if(IsTrigA() || IsTrigRight())
  {
    if(NetAppLib::Debug::PokeToolSimpleParamDebugger::SetNext(pThis->m_SimpleParam, &(pThis->m_PersonalData), pThis->m_pOwner->m_PokeModelSystem))
    {
      pThis->ToInfo();

      pThis->m_pOwner->RequestEx(pThis->m_pOwner->GetEnvRenderPlaceType(slot), slot, pThis->m_SimpleParam, pThis->m_pOwner->GetEnvTargetLayer(slot));
      SE_OK(pItem);
    }
  }
  else if(IsTrigX() || IsTrigLeft())
  {
    /*  簡易Rewind  */
    if(MONSNO_NULL < pThis->m_ItemInfos[VALITEM_monsNo].value)
    {
      --pThis->m_ItemInfos[VALITEM_monsNo].value;

      pThis->FromInfo();
      NetAppLib::Debug::PokeToolSimpleParamDebugger::Set(pThis->m_SimpleParam, &(pThis->m_PersonalData));
      pThis->ToInfo();
      pThis->m_pOwner->RequestEx(pThis->m_pOwner->GetEnvRenderPlaceType(slot), slot, pThis->m_SimpleParam, pThis->m_pOwner->GetEnvTargetLayer(slot));
      SE_OK(pItem);
    }
  }

}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void NetAppPokeModelUtility::NetAppPokeModelUtilityDebug::Set(    void* _pThis, gfl2::debug::DebugWinItem* pItem)
{
  if(IsTrigA() || IsTrigRight())
  {
    NetAppPokeModelUtilityDebug*  pThis = static_cast<NetAppPokeModelUtilityDebug*>(_pThis);
    const u32 slot  = pThis->m_ItemInfos[VALITEM_slot].value;

    pThis->FromInfo();
    NetAppLib::Debug::PokeToolSimpleParamDebugger::Set(pThis->m_SimpleParam, &(pThis->m_PersonalData));
    pThis->ToInfo();
    pThis->m_pOwner->RequestEx(pThis->m_pOwner->GetEnvRenderPlaceType(slot), slot, pThis->m_SimpleParam, pThis->m_pOwner->GetEnvTargetLayer(slot));
    SE_OK(pItem);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokeModelUtility::NetAppPokeModelUtilityDebug::ToInfo(void)
{
  m_ItemInfos[VALITEM_monsNo].value  = m_SimpleParam.monsNo;
  m_ItemInfos[VALITEM_formNo].value  = m_SimpleParam.formNo;
  m_ItemInfos[VALITEM_gender].value  = m_SimpleParam.sex   ;
  m_ItemInfos[VALITEM_rare].value    = m_SimpleParam.isRare;
  m_ItemInfos[VALITEM_isEgg].value   = m_SimpleParam.isEgg;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokeModelUtility::NetAppPokeModelUtilityDebug::FromInfo(void)
{
  m_SimpleParam.monsNo = static_cast<MonsNo>(m_ItemInfos[VALITEM_monsNo].value);
  m_SimpleParam.formNo = static_cast<FormNo>(m_ItemInfos[VALITEM_formNo].value);
  m_SimpleParam.sex    = static_cast<pml::Sex>(m_ItemInfos[VALITEM_gender].value);
  m_SimpleParam.isRare = static_cast<bool>(m_ItemInfos[VALITEM_rare].value);
  m_SimpleParam.isEgg  = static_cast<bool>(m_ItemInfos[VALITEM_isEgg].value);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokeModelUtility::NetAppPokeModelUtilityDebug::OnUpdate(void)
{
  m_pOwner->SetDebugOffset(
    static_cast<f32>(m_ItemInfos[VALITEM_ofsX].value),
    static_cast<f32>(m_ItemInfos[VALITEM_ofsY].value),
    static_cast<f32>(m_ItemInfos[VALITEM_ofsSize].value)
  );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokeModelUtility::NetAppPokeModelUtilityDebug::OnDraw(const gfl2::gfx::CtrDisplayNo displayNo)
{
  /*
    とりあえず上専用
  */
  if(displayNo != gfl2::gfx::CtrDisplayNo::LEFT)  return;


  if(m_ItemInfos[VALITEM_isDrawFrame].value)
  {
    if(m_pOwner->m_pInFrameSystem)
    {
      gfl2::math::VEC2  screenRectVec[2];
      

      screenRectVec[0].x  = m_pOwner->m_FrameRect[0] + m_pOwner->m_DebugOffset_OfsX;
      screenRectVec[0].y  = m_pOwner->m_FrameRect[1] + m_pOwner->m_DebugOffset_OfsY;
      screenRectVec[1].x  = m_pOwner->m_FrameRect[2] + m_pOwner->m_DebugOffset_OfsX + m_pOwner->m_DebugOffset_OfsSize;
      screenRectVec[1].y  = m_pOwner->m_FrameRect[3] + m_pOwner->m_DebugOffset_OfsY + m_pOwner->m_DebugOffset_OfsSize;

      /*
        とりあえず上専用
      */
      gfl2::gfx::GFGL::BeginScene();
//      gfl2::gfx::DrawUtil::BeginRender();
      gfl2::gfx::DrawUtil::BeginRenderExtension();

      gfl2::gfx::DrawUtil::SetCullMode(gfl2::gfx::CullMode::None);
      gfl2::gfx::DrawUtil::SetFillMode(gfl2::gfx::FillMode::Fill);
      gfl2::gfx::DrawUtil::SetMaterialColor(gfl2::math::Vector4(1,1,1,1));
      gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);
      gfl2::gfx::DrawUtil::SetDepthTestEnable(false);


      gfl2::gfx::DrawUtil::SetScreenSize( 400, 240);
      gfl2::gfx::GFGL::SetViewport( 0, 0, 400, 240);
      gfl2::gfx::GFGL::SetScissor(0, 0, 400, 240);

      {
        const f32 w = screenRectVec[1].x - screenRectVec[0].x;
        const f32 h = screenRectVec[1].y - screenRectVec[0].y;
        const gfl2::math::Vector4 col( 0.8f, 0.3f, 0.3f, 0.5f);
        const f32 lineW = 2.0f;

        gfl2::gfx::DrawUtil::DrawRect(screenRectVec[0].x, screenRectVec[0].y, lineW,  h,      col);
        gfl2::gfx::DrawUtil::DrawRect(screenRectVec[0].x, screenRectVec[0].y, w,      lineW,  col);
        gfl2::gfx::DrawUtil::DrawRect(screenRectVec[1].x, screenRectVec[0].y, lineW,  h,      col);
        gfl2::gfx::DrawUtil::DrawRect(screenRectVec[0].x, screenRectVec[1].y, w,      lineW,  col);
      }
    
//      gfl2::gfx::DrawUtil::EndRender();
      gfl2::gfx::DrawUtil::EndRenderExtension();
      gfl2::gfx::GFGL::EndScene();
    }
  }
}


} /*  namespace Util      */
} /*  namespace NetAppLib */

#endif  /*  #if PM_DEMUG  */


