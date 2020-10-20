//========================================================================================
/**
 * @brief タッチバーの描画クラス
 * @file app_tool_ScrollBar.cpp
 * @author obata_toshihiro
 * @date 2011.05.20
 */
//========================================================================================
#include "AppLib/include/Tool/app_tool_ScrollValue.h"
#include "AppLib/include/Tool/app_tool_ScrollBar.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   parts   本体の部品ペイン
 * @param   mark    ツマミ部分のペイン
 * @param   top     スクロール最小値の位置ペイン
 * @param   bottom  スクロール最大値の位置ペイン
 * @param   dir     スクロールの向き
 */
//-----------------------------------------------------------------------------
ScrollBar::ScrollBar( gfl2::lyt::LytPane * parts, gfl2::lyt::LytPane * mark, gfl2::lyt::LytPane * top, gfl2::lyt::LytPane * bottom, ScrollDirection dir )
  : m_partsPane( parts )
  , m_markPane( mark )
  , m_markSize( mark->GetSize() )
  , m_topPos( top->GetTranslate() )
  , m_bottomPos( bottom->GetTranslate() )
  , m_scrollDirection( dir )
  , m_isMarkSizeUpdate( false )
{
  m_railLen = m_topPos.y - m_bottomPos.y;
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
ScrollBar::~ScrollBar()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2015.03.09
 *
 * @param   scroll_value  スクロール値
 * @param   alpha         アルファ値
 */
//-----------------------------------------------------------------------------
void ScrollBar::Draw( const ScrollValue & scroll_value, f32 alpha )
{
  f32 min = scroll_value.GetMinValue();
  f32 max = scroll_value.GetMaxValue();
  f32 now = scroll_value.GetCurrentValue();
  f32 knob_pos = ( now - min ) / ( max - min );
  Draw( knob_pos, alpha );
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2015.03.09
 *
 * @param   val     描画位置（現在値と最大値の割合）
 * @param   alpha   アルファ値
 */
//-----------------------------------------------------------------------------
void ScrollBar::Draw( f32 val, f32 alpha )
{
  gfl2::math::VEC3 pos = m_markPane->GetTranslate();

  pos.y = m_topPos.y - CalcPos(m_markSize.width,val);

  if( m_isMarkSizeUpdate != false )
  {
    m_markPane->SetSize( m_markSize );
    m_isMarkSizeUpdate = false;
  }
  m_markPane->SetTranslate( pos );
  m_partsPane->SetAlpha( alpha );
}

//-----------------------------------------------------------------------------
/**
 * @func    CalcPos
 * @brief   ツマミの表示位置計算
 * @date    2015.03.09
 *
 * @param   mark_size   ツマミのサイズ
 * @param   val         描画位置（現在値と最大値の割合）
 */
//-----------------------------------------------------------------------------
f32 ScrollBar::CalcPos( f32 mark_size, f32 val )
{
  return ( (m_railLen-mark_size) * val + (mark_size/2.0f) );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMarkSize
 * @brief   ツマミ部分のペインのサイズを設定
 * @date    2015.03.09
 *
 * @param   scroll_max  スクロール最大値
 */
//-----------------------------------------------------------------------------
void ScrollBar::SetMarkSize( f32 scroll_max )
{
  if( scroll_max == 0.0f )
  {
    m_markSize.width = m_railLen;
  }
  else
  {
    u32 size = static_cast<u32>( m_railLen / scroll_max );
    if( size < MARK_SIZE_MIN )
    {
      size = MARK_SIZE_MIN;
    }
    m_markSize.width = static_cast<f32>( size );
  }
  m_isMarkSizeUpdate = true;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisible
 * @brief   表示切り替え
 * @date    2015.03.09
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void ScrollBar::SetVisible( bool flg )
{
  m_partsPane->SetVisible( flg );
}


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)
