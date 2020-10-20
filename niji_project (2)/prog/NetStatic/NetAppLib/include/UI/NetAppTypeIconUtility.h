#if !defined(NETAPPTYPEICONUTILITY_H_INCLUDED)
#define NETAPPTYPEICONUTILITY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NetAppTypeIconUtility.h
 * @date   2017/01/26 Thu. 19:25:24
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
#include  <Layout/include/gfl2_Layout.h>


namespace app {
  namespace tool  {
    class AppCommonGrpIconData;
  }
}
// =============================================================================
/**/
//==============================================================================


namespace NetAppLib {
namespace UI {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  
  *  @date   2017/01/26 Thu. 19:25:58
  */
//==============================================================================
class NetAppTypeIconUtility
{
  GFL_FORBID_COPY_AND_ASSIGN(NetAppTypeIconUtility);

public:
  NetAppTypeIconUtility(void);
  virtual ~NetAppTypeIconUtility();

  //------------------------------------------------------------------
  /**
    *  @brief    AppCommonGrpIconData の設定
    */
  //------------------------------------------------------------------
  void  SetAppCommonGrpIconData(app::tool::AppCommonGrpIconData* pAppCommonGrpIconData) { m_pAppCommonGrpIconData = pAppCommonGrpIconData; }


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
//------------------------------------------------------------------
  void  SetTargetPicturePane(gfl2::lyt::LytPicture* pPicturePane) {m_pPicturePane = pPicturePane;}

  //------------------------------------------------------------------
  /**
    *  @brief    タイプ設定（AppCommonGrpIconDataが設定済み且つInitialize済みであり、ターゲットとなるPicturePaneを設定済みであること）
    *  @return   アイコンの変更に成功したか
    *  @note     成否によってVisibilityが変化する
    */
  //------------------------------------------------------------------
  bool  SetType(const PokeType_tag type);
  bool  SetTypeFromWazaNo(const WazaNo  waza)    {return SetType(GetWazaType(waza));}


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  SetVisibility(const bool isVisible)   { if(m_pPicturePane) m_pPicturePane->SetVisible(isVisible); }


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  static PokeType_tag  GetWazaType(const WazaNo wazano) {return static_cast<PokeType_tag>(pml::wazadata::GetType(wazano));}

protected:
  app::tool::AppCommonGrpIconData*    m_pAppCommonGrpIconData;
  gfl2::lyt::LytPicture*              m_pPicturePane;
};


}  /*  namespace UI  */
}  /*  namespace NetAppLib  */
#endif  /*  #if !defined(NETAPPTYPEICONUTILITY_H_INCLUDED)  */
