#if !defined(NETAPPPOKEPARATYPEUTILITY_H_INCLUDED)
#define NETAPPPOKEPARATYPEUTILITY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NetAppPokeparaTypeUtility.h
 * @date   2017/01/26 Thu. 19:42:28
 * @author muto_yu
 * @brief  ポケモンのタイプ表示（2枠）
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <pml/include/pokepara/pml_PokemonParam.h>

#include  "./NetAppTypeIconUtility.h"


namespace NetAppLib {
namespace UI {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  
  *  @date   2017/01/26 Thu. 19:42:31
  */
//==============================================================================
class NetAppPokeparaTypeUtility
{
  GFL_FORBID_COPY_AND_ASSIGN(NetAppPokeparaTypeUtility);
public:
  NetAppPokeparaTypeUtility(void){}
  virtual ~NetAppPokeparaTypeUtility(){}


  //------------------------------------------------------------------
  /**
    *  @brief    AppCommonGrpIconData の設定
    */
  //------------------------------------------------------------------
  void  SetAppCommonGrpIconData(app::tool::AppCommonGrpIconData* pAppCommonGrpIconData)
        {
          m_TypeIconUtil[0].SetAppCommonGrpIconData(pAppCommonGrpIconData);
          m_TypeIconUtil[1].SetAppCommonGrpIconData(pAppCommonGrpIconData);
        }

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  SetTargetPicturePane(gfl2::lyt::LytPicture* pPicturePane0, gfl2::lyt::LytPicture* pPicturePane1)
        {
          m_TypeIconUtil[0].SetTargetPicturePane(pPicturePane0);
          m_TypeIconUtil[1].SetTargetPicturePane(pPicturePane1);
        }
  

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  SetCoreParam(const pml::pokepara::CoreParam* pCoreParam);


protected:
  NetAppTypeIconUtility   m_TypeIconUtil[2];
};



}  /*  namespace UI  */
}  /*  namespace NetAppLib  */
#endif  /*  #if !defined(NETAPPPOKEPARATYPEUTILITY_H_INCLUDED)  */
