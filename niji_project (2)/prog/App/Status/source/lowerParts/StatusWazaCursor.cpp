//======================================================================
/**
 * @file    StatusWazaCursor.cpp
 * @date    2015/12/09 13:06:21
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：技カーソル(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusWazaCursor.h"

#include <App/Status/source/StatusDrawerBase.h>

// pml
#include <pml/include/pmlib.h>

// arc
#include <niji_conv_header/app/field_status/field_status_pane.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
WazaCursor::WazaCursor( DrawerBase* base, MenuCursor* cursor )
  : m_pBase( base )
  , m_pCursor( cursor )
  , m_WazaCount(0)
  , m_CursorPos(0)
{
  for( u8 i=0; i<WAZA_MAX; ++i )
  {
    m_PlateIndex[i] = i;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
void WazaCursor::Initialize( void )
{
  // ペインの設定
  const u32 c_PlateParts[WAZA_MAX] = {
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_00,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_01,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_02,
    PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTSSKILLPLT_03,
  };

  for( u8 i=0; i<WAZA_MAX; ++i )
  {
    gfl2::lyt::LytParts* parts = m_pBase->GetParts( c_PlateParts[i] );

    m_pNullCursor[i] = m_pBase->GetPane( PANENAME_STATUS_PLT_LOW_000_PANE_NULL_CUR, parts );
  }

  // カーソルのセットアップ
  if( m_pCursor )
  {
    m_pCursor->SetupCursor( m_pBase->GetG2D(), m_pNullCursor, WAZA_MAX, gfl2::lyt::DISPLAY_LOWER );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   所持技数の設定
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void WazaCursor::SetWazaCount( const pml::pokepara::PokemonParam* pp )
{
  bool fastmode_flag = pp->StartFastMode();
  {
    // 所持技数の取得
    m_WazaCount = pp->GetWazaCount();
  }
  pp->EndFastMode( fastmode_flag );

  m_CursorPos = 0;

  FUKUSHIMA_PRINT( "%s : waza_count[%d]\n", __FUNCTION__, m_WazaCount );
}


//------------------------------------------------------------------------------
/**
 * @brief   カーソルの位置の設定
 *
 * @param   pos   カーソル位置
 * @param   flg   表示切替を行うかどうか
 */
//------------------------------------------------------------------------------
void WazaCursor::SetCursorPos( u8 pos, bool flg )
{
  if( m_pCursor )
  {
    // カーソル位置の設定
    m_pCursor->SetCursorPos( m_pBase->GetLytWk(), m_PlateIndex[pos] );

    // カーソルを表示
    if( flg )
    {
      m_pCursor->SetVisible( true );
    }

    m_CursorPos = pos;

    FUKUSHIMA_PRINT( "WazaCursor::%s : pos[%d]\n", __FUNCTION__, pos );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   次のプレートへ
 *
 * @param   次のプレートインデックス
 */
//------------------------------------------------------------------------------
u8 WazaCursor::NextWazaIndex( void )
{
  u8 pos = (m_CursorPos + 1) % m_WazaCount;

  // カーソルの位置を設定
  SetCursorPos( pos );

  return pos;
}

//------------------------------------------------------------------------------
/**
 * @brief   前のプレートへ
 *
 * @param   前のプレートインデックス
 */
//------------------------------------------------------------------------------
u8 WazaCursor::PrevWazaIndex( void )
{
  u8 pos = 0;

  if( m_CursorPos == 0 )
  {
    pos = m_WazaCount - 1;
  }
  else {
    pos = m_CursorPos - 1;
  }

  // カーソルの位置を設定
  SetCursorPos( pos );

  return pos;
}



//------------------------------------------------------------------------------
/**
 * @brief   プレートデータの設定
 *
 * @param   plateDataTbl    プレートデータ配列
 */
//------------------------------------------------------------------------------
void WazaCursor::SetPlateData( PlateData* plateDataTbl )
{
  for( u8 i=0; i<WAZA_MAX; ++i )
  {
    gfl2::lyt::LytParts* parts = plateDataTbl[i].parts;

    // カーソル位置ペインの取得
    m_pNullCursor[i] = m_pBase->GetPane( PANENAME_STATUS_PLT_LOW_000_PANE_NULL_CUR, parts );
  }

  // カーソル位置ペインのセット
  m_pCursor->SetNullCursor( m_pNullCursor );
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソルの表示切替
 *
 * @param   visible   表示フラグ
 */
//------------------------------------------------------------------------------
void WazaCursor::SetVisibleCursor( bool visible )
{
  m_pCursor->SetVisible( visible );
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソルの表示位置の設定
 *
 * @param   pos   カーソル位置
 */
//------------------------------------------------------------------------------
void WazaCursor::SetPosCursor( u32 pos )
{
  SetCursorPos( pos );
}

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)
