//==============================================================================
/**
 * @file    StrInputArea.h
 * @brief   入力エリア関連
 * @author  fukushima_yuya
 * @date    2015.05.22
 * @note    sango_project app strinput_area.hから移植
 */
//==============================================================================

// niji
#include "App/StrInput/source/StrInputArea.h"
#include "App/StrInput/source/StrInputDef.h"

// arc
#include <niji_conv_header/app/strinput/strinput_pane.h>


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
* @namespace  App::StrInput
 * @brief     文字入力画面
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( StrInput )


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   lytwk   レイアウトワーク
 * @param   resID   リソースID
 * @param   tbl     ペインテーブル
 * @param   buf     入力文字列
 * @param   siz     入力文字数
 */
 //------------------------------------------------------------------------------
InputArea::InputArea( gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytMultiResID resid, const gfl2::lyt::LytPaneIndex * tbl, const gfl2::str::STRCODE * buf, u32 siz ) :
  flg( false )
{
  lytWk   = lytwk;
  resID   = resid;
  areaTbl = tbl;
  tblSize = siz;
  str     = buf;
  start   = siz;

  for( u32 i=0; i<siz; i++ )
  {
    if( str[i] == gfl2::str::EOM_CODE )
    {
      start = i;
      break;
    }
  }

  end = start;
}

//------------------------------------------------------------------------------
/**
 * @brief    デストラクタ
 */
//------------------------------------------------------------------------------
InputArea::~InputArea( void )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief    入力エリア選択
 *
 * @param    tp  タッチパネル
 *
 * @retval  "true = エリア選択処理中"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool InputArea::CheckInputArea( gfl2::ui::TouchPanel * tp )
{
  if( tp->IsTouchTrigger() == true )
  {
    u32  pos = InputArea::HitCheck( tp );
    if( pos <= tblSize && ( pos == 0 || str[pos-1] != gfl2::str::EOM_CODE ) )
    {
      tp->GetXY( &sx, &sy );
      ex = sx;
      ey = sy;
      start = pos;
      end   = pos;
      flg   = true;
      return true;
    }
  }
  else {
    if( flg == true )
    {
      if( tp->IsTouch() == true )
      {
        u32  pos = InputArea::HitCheck( tp );
        if( pos < tblSize && str[pos] != gfl2::str::EOM_CODE )
        {
          tp->GetXY( &ex, &ey );
          end = pos;
        }
      }
      else {
        flg = false;
      }
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief    タッチ座標のペインテーブルのインデックスを取得
 *
 * @param    tp  タッチパネル
 *
 * @return  ペインテーブルのインデックス
 */
//------------------------------------------------------------------------------
u32 InputArea::HitCheck( gfl2::ui::TouchPanel * tp )
{
  const gfl2::lyt::LytPaneIndex paneID = PANENAME_STRINPUT_BTN_LOW_003_PANE_INPUT__BOUND_00;
  u32  i;
  u16  x, y;

  tp->GetXY( &x, &y );

  for( i=0; i<INPUT_AREA_MAX; i++ )
  {
    gfl2::lyt::LytParts* pParts = lytWk->GetPartsPane( areaTbl[i] );
    gfl2::lyt::LytPane*  pPane  = lytWk->GetPane( pParts, paneID, &resID );

    if( lytWk->GetHitPane( x, y, pPane ) )
    {
      break;
    }
  }

  return i;
}

//------------------------------------------------------------------------------
/**
 * @brief    入力エリアの範囲を取得
 *
 * @param    start_pos  開始位置格納場所
 * @param    end_pos    終了位置格納場所
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void InputArea::GetAreaPos( u16 * start_pos, u16 * end_pos )
{
  *start_pos = start;
  *end_pos   = end;
}

//------------------------------------------------------------------------------
/**
 * @brief    入力エリアサイズを取得
 *
 * @param    none
 *
 * @return  入力エリアサイズ
 */
//------------------------------------------------------------------------------
u32  InputArea::GetAreaSize( void )
{
  if( start == end && gfl2::math::Abs(sx-ex) < 4 )
  {
    return 0;
  }

  if( sx > ex )
  {
    return gfl2::math::Abs( start - end );
  }

  return ( gfl2::math::Abs(start-end) + 1 );
}


GFL_NAMESPACE_END( StrInput )
GFL_NAMESPACE_END( App )
