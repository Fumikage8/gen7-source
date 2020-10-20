#if !defined(NETAPPPOKEPARAUTILITY_H_INCLUDED)
#define NETAPPPOKEPARAUTILITY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NetAppPokeparaUtility.h
 * @date   2015/10/15 Thu. 16:01:21
 * @author muto_yu
 * @brief  pokeparaを元にPokeIconTool、レベル、性別表示などを行う
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  <util/include/gfl2_List.h>

#include  "AppLib/include/Util/app_util_heap.h"
#include  "AppLib/include/Tool/app_tool_PokeIcon.h"

#include  "./NetAppPokeparaTypeUtility.h"
#include  "./NetAppWazaUtility.h"

// =============================================================================
/**/
//==============================================================================


namespace NetAppLib {
  namespace System  {class  ApplicationWorkBase;}
namespace UI        {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/10/15 Thu. 16:02:38
  */
//==============================================================================
class NetAppPokeparaUtility
{
  GFL_FORBID_COPY_AND_ASSIGN(NetAppPokeparaUtility);

public:
  NetAppPokeparaUtility(System::ApplicationWorkBase* pWorkBase);
  virtual ~NetAppPokeparaUtility();


  void  OnUpdate(void);
  bool  IsStable(void) const;   /**<  終了可能であるか  */


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    *  @param    [in] iconCapacity  管理アイコン数
    *  @param    [in] pTargetView   置換対象Paneが含まれるView（PokeIconToolがGetG2DUtilするためCreatedであること）
    *  @return   
    */
  //------------------------------------------------------------------
  void  Setup(const u32 iconCapacity, app::ui::UIView*  pTargetView, const u32 layoutID, app::tool::AppCommonGrpIconData* pAppCommonGrpIconData = NULL);

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  bool  Cleanup(void);

  //------------------------------------------------------------------
  /**
   *  PokeIconTool::SetPokeIcon呼び出し（アイコン差し替え）
   */
  //------------------------------------------------------------------
  void  SetCoreParam(const u32 iconIndex, const pml::pokepara::CoreParam* pCoreParam);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  static const u32  ANIMID_NONE = UINT_MAX;
  typedef struct  RegisterInfo
  {
    gfl2::lyt::LytPane*     pVisibilityControl;       /**<  MONSNO_NULLのとき、Visibilityを制御するためのPane  */
    gfl2::lyt::LytPicture*  pIconPicture;
    gfl2::lyt::LytTextBox*  pTextBox_Name;
    gfl2::lyt::LytTextBox*  pTextBox_Level;
    gfl2::lyt::LytParts*    pGenderIconParts;

    gfl2::str::StrBuf*      pNameStringSource;        /**<  名前表示フォーマット文字列（複製される）    */
    gfl2::str::StrBuf*      pLevelStringSource;       /**<  レベル表示フォーマット文字列（複製される）  */

    u32                     genderIconAnim_Male;
    u32                     genderIconAnim_Female;

    /*  タイプ表示  */
    gfl2::lyt::LytPicture*  pPokeparaTypePicturePane0;
    gfl2::lyt::LytPicture*  pPokeparaTypePicturePane1;

    /*  特性/アイテム  */
    gfl2::lyt::LytTextBox*  pTextBox_Ability;             /**<  特性用      */
    gfl2::str::StrBuf*      pAbilityStringSource;         /**<  特性表示フォーマット文字列（複製される）      */
    gfl2::lyt::LytPane*     pVisibilityControl_Ability;

    gfl2::lyt::LytTextBox*  pTextBox_Item;                /**<  アイテム用  */
    gfl2::str::StrBuf*      pItemStringSource;            /**<  アイテム表示フォーマット文字列（複製される）  */
    gfl2::lyt::LytPane*     pVisibilityControl_Item;

    /*  技  */
    u32                     wazaUtilLayoutID;
    struct  WazaUtilInfo
    {
      gfl2::lyt::LytPane*     pControlPane;               /**<  visibility制御用      */
      gfl2::lyt::LytTextBox*  pTextBoxPane;               /**<  技名表示              */
      gfl2::str::StrBuf*      pFormatString;              /**<  技名展開フォーマット  */
      u32                     typeAnimID[POKETYPE_MAX];   /**<  タイプ別台座アニメ    */
      gfl2::lyt::LytPicture*  pPicturePane;               /**<  タイプアイコン        */
      
      /*  ctor  */
      WazaUtilInfo(void)
        : pControlPane(NULL)
        , pTextBoxPane(NULL)
        , pFormatString(NULL)
        , pPicturePane(NULL)
      {
        for(u32 index = 0; index < GFL_NELEMS(typeAnimID); ++index) {typeAnimID[index] = UINT_MAX;}
      }
    }m_WazaUtilInfos[pml::MAX_WAZA_NUM];


    /*  ctor  */
    RegisterInfo(void)
      : pVisibilityControl(NULL)
      , pIconPicture(NULL)
      , pTextBox_Name(NULL)
      , pTextBox_Level(NULL)
      , pGenderIconParts(NULL)
      , pNameStringSource(NULL)
      , pLevelStringSource(NULL)
      , genderIconAnim_Male(ANIMID_NONE)
      , genderIconAnim_Female(ANIMID_NONE)
      /*  タイプ表示  */
      , pPokeparaTypePicturePane0(NULL)
      , pPokeparaTypePicturePane1(NULL)
      /*  特性/アイテム  */
      , pTextBox_Ability(NULL)     /**<  特性用      */
      , pAbilityStringSource(NULL) /**<  特性表示フォーマット文字列（複製される）      */
      , pVisibilityControl_Ability(NULL)
      , pTextBox_Item(NULL)        /**<  アイテム用  */
      , pItemStringSource(NULL)    /**<  アイテム表示フォーマット文字列（複製される）  */
      , pVisibilityControl_Item(NULL)
      /*  技  */
      , wazaUtilLayoutID(0)
    {}
  }RegisterInfo;
  void  RegisterStatusResource(const u32 iconIndex, const RegisterInfo& rRegisterInfo);


protected:
  void                                      SetStr(gfl2::str::StrBuf* pDst, const gfl2::str::StrBuf* pSrc = NULL);
  gfl2::util::List<RegisterInfo>::Iterator  GetRegisterInfo(const u32 iconIndex);
  bool                                      IsValidIndex(const u32 iconIndex) const {return (iconIndex < m_IconCapacity);}

  System::ApplicationWorkBase*          m_pWorkBase;
  app::tool::PokeIconToolEx*            m_pPokeIconTool;
  app::ui::UIView*                      m_pTargetView;
  u32                                   m_TargetLayoutID;
  u32                                   m_IconCapacity;
  gfl2::util::List<RegisterInfo>        m_RegisterInfoList;

  app::tool::AppCommonGrpIconData*      m_pCommonGrpIconData;

  /*  タイプ表示  */
  NetAppPokeparaTypeUtility             m_TypeUtil;
  /*  技表示  */
  NetAppWazaUtility                     m_WazaUtil[pml::MAX_WAZA_NUM];
};


} /*  namespace UI        */
} /*  namespace NeAppLib  */
#endif  /*  #if !defined(NETAPPPOKEPARAUTILITY_H_INCLUDED)  */
