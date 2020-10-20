#if !defined(NETAPPWAZAUTILITY_H_INCLUDED)
#define NETAPPWAZAUTILITY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NetAppWazaUtility.h
 * @date   2017/01/26 Thu. 20:01:15
 * @author muto_yu
 * @brief  技表示単体
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

#include  <AppLib/include/Util/app_util_2d.h>

#include  "./NetAppTypeIconUtility.h"

namespace NetAppLib {
namespace UI {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  技名表示、タイプ表示、タイプ別台座アニメ切り替え
  *  @date   2017/01/26 Thu. 20:01:46
  */
//==============================================================================
class NetAppWazaUtility
{
  GFL_FORBID_COPY_AND_ASSIGN(NetAppWazaUtility);
public:
  NetAppWazaUtility(void);
  virtual ~NetAppWazaUtility(){}

  /*  技名表示/タイプ別台座アニメ用G2DUtil  */
  void  SetG2DUtil(app::util::G2DUtil* pG2DUtil, const u32 layoutID)
        {
          m_pG2DUtil = pG2DUtil;
          m_LayoutID = layoutID;
        }

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  SetControlPane(gfl2::lyt::LytPane* pPane)
        {
          m_pControlPane  = pPane;
        }

  //------------------------------------------------------------------
  /**
    *  @brief    技名表示
    *  @param    pFormatString  [in]  技名展開フォーマット文字列（複製されないため、呼び出し元で保持する必要がある）
    *  @return   
    */
  //------------------------------------------------------------------
  void  SetTextBoxPane(gfl2::lyt::LytTextBox* pTextBox, gfl2::str::StrBuf* pFormatString)   /*  StrBufの複製にはheapBaseが必要となるため呼び出し元に管理させる方針  */
        {
          m_pTextBox      = pTextBox;
          m_pFormatString = pFormatString;
        }

  /*  台座アニメ  */
  void  SetTypeAnimID(const PokeType_tag type, const u32 animID)
        {
          const bool  bValidIndex = (static_cast<u32>(type) < GFL_NELEMS(m_TypeAnimIDs));
          GFL_ASSERT(bValidIndex);
          if(bValidIndex) m_TypeAnimIDs[type] = animID;
        }

  /*  タイプアイコン  */
  NetAppTypeIconUtility&  GetTypeIconUtility(void)
                          {
                            return m_TypeIconUtility;
                          }

  /*  技設定  */
  void  SetWaza(const WazaNo wazano);

protected:
  NetAppTypeIconUtility   m_TypeIconUtility;
  app::util::G2DUtil*     m_pG2DUtil;
  u32                     m_LayoutID;
  gfl2::lyt::LytPane*     m_pControlPane;                 /**<  表示制御用（WAZANO_NULLの際に非表示にする） */
  u32                     m_TypeAnimIDs[POKETYPE_MAX];    /**<  タイプ別Anim                                */
  gfl2::lyt::LytTextBox*  m_pTextBox;                     /**<  技名表示                                    */
  gfl2::str::StrBuf*      m_pFormatString;                /**<  技名表示用展開フォーマット                  */

  static const u32  INVALID_TYPEANIM  = UINT_MAX;
};



}  /*  namespace UI  */
}  /*  namespace NetAppLib  */
#endif  /*  #if !defined(NETAPPWAZAUTILITY_H_INCLUDED)  */
