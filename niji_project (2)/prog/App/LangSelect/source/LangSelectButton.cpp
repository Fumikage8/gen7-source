//======================================================================
/**
 * @file LangSelectButton.cpp
 * @date 2015/07/23 18:43:26
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "App/LangSelect/source/LangSelectButton.h"
#include "App/LangSelect/source/LangSelectDrawerBase.h"

#include <AppLib/include/Ui/UIView.h>

// arc
#include <niji_conv_header/app/lang_select/lang_select_pane.h>
#include <niji_conv_header/app/lang_select/lang_select_anim_list.h>
#include <niji_conv_header/message/msg_language_select.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( LangSelect )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
static const f32 SELECT_BTN_X = 2.0f;
static const f32 SELECT_BTN_Y = 0.0f;
static const f32 BTN_SPACE_Y = 30.0f;
static const f32 PREV_BTN_OFFSET_Y = 65.0f;


static const u32 c_LangBtnIndex[] = {
  PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_00,
  PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_01,
  PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_02,
  PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_03,
  PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_04,
  PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_05,
  PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_06,
  PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_07,
  PANENAME_LANGSELECT_MIN_UPP_PANE_BUTTON_08,
};


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 * @param   pBase     描画共通処理クラス
 * @param   langData  言語データ
 */
//------------------------------------------------------------------------------
LangSelectButton::LangSelectButton( DrawerBase* pBase, LangData langData[] )
  : m_pBase( pBase )
  , m_Index( 0 )
{
  const u32 c_LangNameAnim[] = {
    LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_00_GRPFONT,
    LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_01_GRPFONT,
    LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_02_GRPFONT,
    LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_03_GRPFONT,
    LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_04_GRPFONT,
    LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_05_GRPFONT,
    LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_06_GRPFONT,
    LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_07_GRPFONT,
    LA_LANG_SELECT_LANGSELECT_MIN_UPP__BUTTON_08_GRPFONT,
  };

  for( u32 i=0; i<LANG_ID_MAX; ++i )
  {
    m_LangData[i]  = langData[i];
    m_pBtnParts[i] = m_pBase->GetParts( c_LangBtnIndex[i] );

    const u32 animID = c_LangNameAnim[i];
    {
      m_pBase->StartAnim( animID, false );
      m_pBase->SetAnimFrame( animID, m_LangData[i].frame );
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ボタンの位置の設定
 */
//------------------------------------------------------------------------------
void LangSelectButton::SetButtonPos( void )
{
  s32 pos = static_cast<s32>( m_Index );

  const f32 c_UppBtnX[] = {
    42.0f, 62.0f, 82.0f, 102.0f, 122.0f, 142.0f, 162.0f, 182.0f, 202.0f,
  };
  const f32 c_LowBtnX[] = {
    2.0f, -18.0f, -38.0f, -58.0f, -78.0f, -98.0f, -118.0f, -138.0f, -158.0f,
  };

  gfl2::math::Vector3 trans( SELECT_BTN_X, SELECT_BTN_Y, 0.0f );
  u8 index = 0;

  for( s32 i=pos; i<LANG_ID_MAX; ++i )
  {
    trans.x = c_LowBtnX[index];
    gfl2::math::VEC3 vec = gfl2::math::ConvertGfVec3ToNwVec3( trans );
    m_pBtnParts[i]->SetTranslate( vec );
    trans.y -= BTN_SPACE_Y;
    index++;
  }

  trans.y = PREV_BTN_OFFSET_Y;
  index = 0;

  for( s32 i=pos-1; i>=0; --i )
  {
    trans.x = c_UppBtnX[index];
    gfl2::math::VEC3 vec = gfl2::math::ConvertGfVec3ToNwVec3( trans );
    m_pBtnParts[i]->SetTranslate( vec );
    trans.y += BTN_SPACE_Y;
    index++;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   次の項目へ
 */
//------------------------------------------------------------------------------
void LangSelectButton::Next( void )
{
  if( m_Index < (LANG_ID_MAX - 1) )
  {
    m_Index++;

    SetButtonPos();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   前の項目へ
 */
//------------------------------------------------------------------------------
void LangSelectButton::Prev( void )
{
  if( m_Index > 0 )
  {
    m_Index--;

    SetButtonPos();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   選択しているボタンインデックスを取得
 * @return  インデックス(0 ～ LANG_ID_MAX)
 */
//------------------------------------------------------------------------------
u32 LangSelectButton::GetSelectBtn( void ) const
{
  return m_Index;
}

GFL_NAMESPACE_END( LangSelect )
GFL_NAMESPACE_END( App )
