#if !defined(JOINFESTAPLAYERLISTFORBATTLEFESUPPERVIEW_H_INCLUDED)
#define JOINFESTAPLAYERLISTFORBATTLEFESUPPERVIEW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   JoinFestaPlayerListForBattleFesUpperView.h
 * @date   2017/01/26 Thu. 17:17:04
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>

#include  <NetStatic/NetAppLib/include/System/ApplicationWorkBase.h>
#include  <NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h>
#include  <NetStatic/NetAppLib/include/UI/NetAppEmptyView.h>
#include  <NetStatic/NetAppLib/include/UI/NetAppPokeparaUtility.h>
#include  <NetStatic/NetAppLib/include/UI/NetAppCommonMessageWindowManipulator.h>
#include  <NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h>

namespace NetAppLib {
namespace JoinFestaUI {
  class JoinFestaPlayerData;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  
  *  @date   2017/01/26 Thu. 17:19:28
  */
//==============================================================================
class JoinFestaPlayerListForBattleFesUpperView
  : public NetAppLib::UI::NetAppEmptyView
{
  GFL_FORBID_COPY_AND_ASSIGN(JoinFestaPlayerListForBattleFesUpperView);
public:
  //==============================================================================
  /**
    *  @brief  RadarChart関連の処理を委譲するためのオブジェクト
    *  @note   RadarChartの実体がROモジュール側にあるため、staticLib側で直接参照するとLinkに失敗する
    *  @date   2017/01/31 Tue. 15:12:10
    */
  //==============================================================================
  class IParameterChartDelegateObject
  {
  public:
    struct CreateParam
    {
      app::ui::UIView*                  view;
      app::util::Heap*                  heap;
      app::util::AppRenderingManager*   renderMan;
      app::tool::AppCommonGrpIconData*  cmnIcon;    //!< アイコン
      gfl2::lyt::LytWk*                 lytwk;
      gfl2::lyt::LytMultiResID          resID;
      u32                               lytID;
    };

    virtual void  ParameterChart_OnCreate(const CreateParam&  rParam)  = 0;
    virtual void  ParameterChart_OnRelease(void) = 0;

    virtual void  ParameterChart_OnUpdate(void) = 0;
    virtual void  ParameterChart_OnSetPokepara(const pml::pokepara::CoreParam* pCoreParam) = 0;
    virtual void  ParameterChart_OnSetVisibility(const bool isVisible) = 0;

    virtual bool  ParameterChart_OnInitFunc(void) = 0;
    virtual bool  ParameterChart_OnEndFunc(void) = 0;

    virtual ~IParameterChartDelegateObject(){}
  };
  void  SetParameterChartDelegateObject(IParameterChartDelegateObject* pObject) {m_pParameterChartDelegate = pObject;}


  /*  ctor  */
  JoinFestaPlayerListForBattleFesUpperView(
    NetAppLib::System::ApplicationWorkBase*         pWorkBase
    , NetAppLib::System::ResourceID                 layoutResourceID
    , NetAppLib::Util::NetAppTrainerIconUtility&    netAppTrainerIconUtility
  );
  /*  dtor  */
  virtual ~JoinFestaPlayerListForBattleFesUpperView();

  /*  uiview  */
  virtual void Update(void);
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  void  SetCommonGrpIcon(app::tool::AppCommonGrpIconData* pAppCommonGrpIconData);

  void SetJoinFestaPlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pData );
  NetAppLib::UI::NetAppPokeparaUtility&  GetPokeparaUtility(void)  {return m_PokeparaUtil;}

  /*  paramchart  */
  bool  ParameterChart_InitFunc(void);
  bool  ParameterChart_EndFunc(void);

protected:
  enum
  {
    TEXTBOX_TrainerName,
    TEXTBOX_TrainerGrade,
    TEXTBOX_MAX
  };

  NetAppLib::System::ApplicationWorkBase*     m_pWorkBase;
  NetAppLib::UI::NetAppPokeparaUtility        m_PokeparaUtil;
  gfl2::str::StrBuf                           m_WazaNameFormatString;
  bool                                        m_bDrawFlag;

  /*  parameterChart  */
  IParameterChartDelegateObject*              m_pParameterChartDelegate;

  NetAppLib::UI::NetAppCommonMessageWindowManipulator m_MessageManip;
  gfl2::lyt::LytTextBox*                      m_pTextBoxes[TEXTBOX_MAX];
  gfl2::lyt::LytPane*                         m_pParamVisibilityControl;

  /*  trainericon  */
  app::tool::AppCommonGrpIconData*            m_pAppCommonGrpIconData;
  JoinFestaFriendKey                          m_CurrentFriendKey;
  NetAppLib::Util::NetAppTrainerIconUtility&  m_rTrainerIconUtility;
  gfl2::lyt::LytPicture*                      m_pTrainerIconPicture;
  gfl2::lyt::LytPane*                         m_pTrainerIconVisibleSync;
  gfl2::lyt::LytPane*                         m_pTrainerIconWindowFrame;

  /*  language icon  */
  gfl2::lyt::LytPicture*                      m_pLanguageIconPicture;
};



}  /*  namespace JoinFestaUI  */
}  /*  namespace NetAppLib  */
#endif  /*  #if !defined(JOINFESTAPLAYERLISTFORBATTLEFESUPPERVIEW_H_INCLUDED)  */

