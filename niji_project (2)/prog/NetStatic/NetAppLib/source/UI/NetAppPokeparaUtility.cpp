//==============================================================================
/**
 * @file   NetAppPokeparaUtility.cpp
 * @date   2015/10/15 Thu. 16:57:41
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "AppLib/include/UI/UIView.h"

#include  "NetStatic/NetAppLib/Include/System/ApplicationWorkBase.h"
#include  "NetStatic/NetAppLib/Include/UI/NetAppPokeparaUtility.h"


namespace
{
  static const u32                    LevelDigits   = 3;
  static const print::NumberDispType  LeveldispType = print::NUM_DISP_LEFT;
  static const print::NumberCodeType  LevelCodeType = print::NUM_CODE_DEFAULT;

}


// =============================================================================
/**/
//==============================================================================





namespace NetAppLib {
namespace UI        {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppPokeparaUtility::NetAppPokeparaUtility(System::ApplicationWorkBase* pWorkBase)
  : m_pWorkBase(pWorkBase)
  , m_pPokeIconTool(NULL)
  , m_pTargetView(NULL)
  , m_TargetLayoutID(0)
  , m_IconCapacity(0)
  , m_RegisterInfoList()

  , m_pCommonGrpIconData(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppPokeparaUtility::~NetAppPokeparaUtility()
{
  bool  bCleanupDone  = Cleanup();
  
  GFL_ASSERT(bCleanupDone);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokeparaUtility::OnUpdate(void)
{
  if(m_pPokeIconTool)
  {
    m_pPokeIconTool->Update();
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  NetAppPokeparaUtility::IsStable(void) const
{
  return (!m_pPokeIconTool);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokeparaUtility::Setup(const u32 iconCapacity, app::ui::UIView* pTargetView, const u32 layoutID, app::tool::AppCommonGrpIconData* pAppCommonGrpIconData)
{
  GFL_ASSERT(m_pWorkBase);
  GFL_ASSERT(!m_pPokeIconTool);
  GFL_ASSERT(!m_RegisterInfoList.Size());
  GFL_ASSERT(iconCapacity);
  GFL_ASSERT(pTargetView);
  GFL_ASSERT(pTargetView->GetG2DUtil());


  gfl2::heap::HeapBase* pHeapBase = m_pWorkBase->GetDeviceHeap();

  m_pTargetView     = pTargetView;
  m_IconCapacity    = iconCapacity;
  m_TargetLayoutID  = layoutID;

  /*  tool  */
  {
    app::tool::PokeIconToolEx::PokeIconToolParam  param;

    param.pUIView   = m_pTargetView;
    param.pHeap     = m_pWorkBase->GetAppHeap();
    param.iconMax   = m_IconCapacity;
    param.sync      = true;   /*  NetAppFrameworkで気軽に使えるよう敢えてsync固定とする  */
    param.layout_id = layoutID;

    m_pPokeIconTool = GFL_NEW(pHeapBase) app::tool::PokeIconToolEx(param);
  }
  
  /*  infos  */
  {

    RegisterInfo  registerInfo;

    m_RegisterInfoList.CreateBuffer(pHeapBase, m_IconCapacity);
    for(u32 index = 0; index < m_IconCapacity; ++index)
    {
      registerInfo.pNameStringSource    = GFL_NEW(pHeapBase) gfl2::str::StrBuf(32, pHeapBase);
      registerInfo.pLevelStringSource   = GFL_NEW(pHeapBase) gfl2::str::StrBuf(32, pHeapBase);
      /*  特性/アイテム  */
      registerInfo.pAbilityStringSource = GFL_NEW(pHeapBase) gfl2::str::StrBuf(32, pHeapBase);  /**<  特性表示フォーマット文字列（複製される）      */
      registerInfo.pItemStringSource    = GFL_NEW(pHeapBase) gfl2::str::StrBuf(32, pHeapBase);  /**<  アイテム表示フォーマット文字列（複製される）  */

      m_RegisterInfoList.PushBack(registerInfo);
    }
  }

  /*  type/waza  */
  {
    m_TypeUtil.SetAppCommonGrpIconData(pAppCommonGrpIconData);
    for(u32 index = 0; index < GFL_NELEMS(m_WazaUtil); ++index)
    {
      m_WazaUtil[index].GetTypeIconUtility().SetAppCommonGrpIconData(pAppCommonGrpIconData);
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  NetAppPokeparaUtility::Cleanup(void)
{
  bool  bDone = false;

  if(m_pPokeIconTool)
  {
    if(m_pPokeIconTool->EndFunc())
    {
      GFL_SAFE_DELETE(m_pPokeIconTool);
      for(gfl2::util::List<RegisterInfo>::Iterator itr = m_RegisterInfoList.Begin(); itr != m_RegisterInfoList.End(); ++itr)
      {
        GFL_SAFE_DELETE(itr->pNameStringSource);
        GFL_SAFE_DELETE(itr->pLevelStringSource);
        /*  特性/アイテム  */
        GFL_SAFE_DELETE(itr->pAbilityStringSource);
        GFL_SAFE_DELETE(itr->pItemStringSource);
      }
      m_RegisterInfoList.Clear();
      m_IconCapacity    = 0;
      m_pTargetView     = NULL;
      m_TargetLayoutID  = 0;

      bDone = true;
    }
  }
  else
  {
    bDone = true;
  }


  return bDone;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokeparaUtility::SetCoreParam(const u32 iconIndex, const pml::pokepara::CoreParam* pCoreParam)
{
  GFL_ASSERT(m_pPokeIconTool);
  GFL_ASSERT(m_RegisterInfoList.Size());
  GFL_ASSERT(pCoreParam);
  GFL_ASSERT(IsValidIndex(iconIndex));


  RegisterInfo&       rInfo           = *GetRegisterInfo(iconIndex);
  app::util::G2DUtil* pG2DUtil        = m_pTargetView->GetG2DUtil();
  const bool          isAvailableMons = (pCoreParam->GetMonsNo() != MONSNO_NULL);

  /*  visibility  */
  if(rInfo.pVisibilityControl)
  {
    rInfo.pVisibilityControl->SetVisible(isAvailableMons);
    if(!isAvailableMons)  return;   /*  以降不要  */
  }
  
  /*  iconPicture  */
  if(rInfo.pIconPicture)
  {
    m_pPokeIconTool->SetPokeIcon(iconIndex, pCoreParam, rInfo.pIconPicture);
    rInfo.pIconPicture->SetVisible(isAvailableMons);
  }

  /*  name  */
  if(pG2DUtil && rInfo.pTextBox_Name && rInfo.pNameStringSource)
  {
    pG2DUtil->SetRegisterMonsName(0, pCoreParam);
    pG2DUtil->SetTextBoxPaneStringExpand(rInfo.pTextBox_Name, *rInfo.pNameStringSource);
  }

  /*  level  */
  if(pG2DUtil && rInfo.pTextBox_Level && rInfo.pLevelStringSource)
  {
    pG2DUtil->SetTextBoxPaneNumber(
        rInfo.pTextBox_Level,
        rInfo.pLevelStringSource,
        pCoreParam->GetLevel(),
        LevelDigits,
        0,
        LeveldispType,
        LevelCodeType
    );
  }

  /*  genderIcon  */
  if(pG2DUtil && rInfo.pGenderIconParts && (rInfo.genderIconAnim_Male != ANIMID_NONE) && (rInfo.genderIconAnim_Female != ANIMID_NONE))
  {
    const bool  isMale    = pCoreParam->GetSex() == pml::SEX_MALE;
    const bool  isFemale  = pCoreParam->GetSex() == pml::SEX_FEMALE;
    const bool  isEgg     = pCoreParam->IsEgg(pml::pokepara::CHECK_BOTH_EGG);

    if((isMale || isFemale) && (!isEgg))
    {
      rInfo.pGenderIconParts->SetVisible(true);
      pG2DUtil->StartAnime(m_TargetLayoutID, isMale ? rInfo.genderIconAnim_Male : rInfo.genderIconAnim_Female);
    }
    else
    {
      rInfo.pGenderIconParts->SetVisible(false);
    }
  }

  /*  type/waza  */
  {
    m_TypeUtil.SetCoreParam(pCoreParam);
    for(u32 index = 0; index < GFL_NELEMS(m_WazaUtil); ++index)
    {
      m_WazaUtil[index].SetWaza(pCoreParam->GetWazaNo(index));
    }
  }

  /*  特性/アイテム  */
  {
    const TokuseiNo ability = pCoreParam->GetTokuseiNo();
    const u16       item    = pCoreParam->GetItem();

    /*    */
    if(rInfo.pVisibilityControl_Ability)
    {
      rInfo.pVisibilityControl_Ability->SetVisible(ability != TOKUSEI_NULL);
    }
    if(pG2DUtil && rInfo.pTextBox_Ability && rInfo.pAbilityStringSource)
    {
      pG2DUtil->SetRegisterTokuseiName(0, ability);
      pG2DUtil->SetTextBoxPaneStringExpand(rInfo.pTextBox_Ability, *rInfo.pAbilityStringSource);
    }

    /*    */
    if(rInfo.pVisibilityControl_Item)
    {
      rInfo.pVisibilityControl_Item->SetVisible(item != ITEM_DUMMY_DATA);
    }
    if(pG2DUtil && rInfo.pTextBox_Item && rInfo.pItemStringSource)
    {
      pG2DUtil->SetRegisterItemName(0, item);
      pG2DUtil->SetTextBoxPaneStringExpand(rInfo.pTextBox_Item, *rInfo.pItemStringSource);
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokeparaUtility::RegisterStatusResource(const u32 iconIndex, const RegisterInfo& rRegisterInfo)
{
  GFL_ASSERT(m_RegisterInfoList.Size());
  GFL_ASSERT(IsValidIndex(iconIndex));

  RegisterInfo&  rInfo  = *GetRegisterInfo(iconIndex);

  /*  複製  */
  {
    rInfo.pVisibilityControl    = rRegisterInfo.pVisibilityControl;
    rInfo.pIconPicture          = rRegisterInfo.pIconPicture;
    rInfo.pTextBox_Name         = rRegisterInfo.pTextBox_Name;
    rInfo.pTextBox_Level        = rRegisterInfo.pTextBox_Level;
    rInfo.pGenderIconParts      = rRegisterInfo.pGenderIconParts;
    rInfo.genderIconAnim_Male   = rRegisterInfo.genderIconAnim_Male;
    rInfo.genderIconAnim_Female = rRegisterInfo.genderIconAnim_Female;

    SetStr(rInfo.pNameStringSource,   rRegisterInfo.pNameStringSource);
    SetStr(rInfo.pLevelStringSource,  rRegisterInfo.pLevelStringSource);

    /*  タイプ表示  */
    rInfo.pPokeparaTypePicturePane0 = rRegisterInfo.pPokeparaTypePicturePane0;
    rInfo.pPokeparaTypePicturePane1 = rRegisterInfo.pPokeparaTypePicturePane1;

    /*  特性/アイテム  */
    rInfo.pTextBox_Ability          = rRegisterInfo.pTextBox_Ability;
    rInfo.pTextBox_Item             = rRegisterInfo.pTextBox_Item;
    SetStr(rInfo.pAbilityStringSource,  rRegisterInfo.pAbilityStringSource);
    SetStr(rInfo.pItemStringSource,     rRegisterInfo.pItemStringSource);

    rInfo.pVisibilityControl_Ability  = rRegisterInfo.pVisibilityControl_Ability;
    rInfo.pVisibilityControl_Item     = rRegisterInfo.pVisibilityControl_Item;

    /*  技  */
    rInfo.wazaUtilLayoutID  = rRegisterInfo.wazaUtilLayoutID;
    for(u32 index = 0; index < GFL_NELEMS(rInfo.m_WazaUtilInfos); ++index)
    {
      rInfo.m_WazaUtilInfos[index]  = rRegisterInfo.m_WazaUtilInfos[index];
    }
  }


  /*
    サブオブジェクトへの設定
  */
  {
    /*  タイプ表示  */
    m_TypeUtil.SetTargetPicturePane(rInfo.pPokeparaTypePicturePane0, rInfo.pPokeparaTypePicturePane1);
    /*  技表示  */
    {
      app::util::G2DUtil* pG2DUtil  = m_pTargetView->GetG2DUtil();
    
      for(u32 index = 0; index < GFL_NELEMS(m_WazaUtil); ++index)
      {
        const RegisterInfo::WazaUtilInfo& rUtilInfo = rInfo.m_WazaUtilInfos[index];

        m_WazaUtil[index].SetG2DUtil(pG2DUtil, rInfo.wazaUtilLayoutID);
        m_WazaUtil[index].SetControlPane(rUtilInfo.pControlPane);
        m_WazaUtil[index].SetTextBoxPane(rUtilInfo.pTextBoxPane, rUtilInfo.pFormatString);
        for(u32 type = 0; type < POKETYPE_MAX; ++type)
        {
          m_WazaUtil[index].SetTypeAnimID(static_cast<PokeType_tag>(type), rUtilInfo.typeAnimID[type]);
        }
        m_WazaUtil[index].GetTypeIconUtility().SetTargetPicturePane(rUtilInfo.pPicturePane);
      }
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
gfl2::util::List<NetAppPokeparaUtility::RegisterInfo>::Iterator  NetAppPokeparaUtility::GetRegisterInfo(const u32 iconIndex)
{
  gfl2::util::List<RegisterInfo>::Iterator itr  = m_RegisterInfoList.Begin();
  for(u32 index = 0; (itr != m_RegisterInfoList.End()) && (index < iconIndex); ++index)
  {
    ++itr;
  }

  return itr;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokeparaUtility::SetStr(gfl2::str::StrBuf* pDst, const gfl2::str::StrBuf* pSrc)
{
  if(!pDst) return;

  if(pSrc)  pDst->SetStr(*pSrc);
  else      pDst->Clear();
}


} /*  namespace UI        */
} /*  namespace NetAppLib */
