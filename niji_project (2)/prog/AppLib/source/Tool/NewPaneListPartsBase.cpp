//=============================================================================
/**
 * @file    NewPaneListPartsBase.cpp
 * @brief   ペインリスト項目関連処理
 * @author  Hiroyuki Nakamura
 * @date    2015.07.23
 */
//=============================================================================

// module
#include "NewPaneListPartsBase.h"
// niji
#include "AppLib/include/Tool/app_tool_MenuCursor.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   param           セットアップパラメータ
 * @param   listener        リスナー
 * @param   button_manager  ボタンマネージャ
 * @param   insert_pane     並び替え時の基準位置ペイン
 * @param   low_tbl         ペインの並びテーブル
 * @param   scroll_max      スクロール最大値
 * @param   effect          エフェクトモード
 */
//-----------------------------------------------------------------------------
NewPaneListPartsBase::NewPaneListPartsBase(
      const NewPaneList::SETUP_PARAM * param,
      NewPaneList::ListListener * listener,
      app::tool::ButtonManager * button_manager,
      gfl2::lyt::LytPane * insert_pane,
      u8 * low_tbl,
      f32 scroll_max,
      NewPaneList::EffectMode effect )
  : m_param( param )
  , m_listener( listener )
  , m_buttonManager( button_manager )
  , m_insertPane( insert_pane )
  , m_paneLowTable( low_tbl )
  , m_scrollMax( scroll_max )
  , m_effect( effect )
  , m_basePaneIndex( 0 )
  , m_itemChangePane( 0 )
  , m_itemChangePos( 0 )
  , m_isItemChange( false )
  , m_changeLowTable( NULL )
  , m_changeLowTableMax( 0 )
  , m_key( NULL )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
NewPaneListPartsBase::~NewPaneListPartsBase()
{
  DeleteChangeLowTable();
}

//-----------------------------------------------------------------------------
/**
 * @func    SetBasePane
 * @brief   リスト開始座標のペインを動作基準ペインに設定
 * @date    2015.07.24
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::SetBasePane(void)
{
  for( u32 i=0; i<m_param->pane_data_max; i++ )
  {
    gfl2::math::VEC3 trans = m_param->pane_data[i].base->GetTranslate();
    if( trans.x == m_param->start_x && trans.y == m_param->start_y )
    {
      m_basePaneIndex = i;
      return;
    }
//    GFL_PRINT( "○[%02d]: %f, %f\n", i, trans.x, trans.y );
  }
  GFL_ASSERT_STOP( 0 );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetDeviceKey
 * @brief   十字キーデバイスを設定
 * @date    2015.07.29
 *
 * @param   key   十字キーデバイス
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::SetDeviceKey( gfl2::ui::VectorDevice * key )
{
  m_key = key;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleCursor
 * @brief   カーソル表示切り替え
 * @date    2015.07.29
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::SetVisibleCursor( bool flg )
{
  if( m_param->menu_cursor != NULL )
  {
    m_param->menu_cursor->SetVisible( flg );
  }
  else
  {
    m_listener->PaneListListener_VisibleCursor( flg );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PutCursor
 * @brief   カーソル配置
 * @date    2015.07.29
 *
 * @param   pos         配置位置
 * @param   is_visible  true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::PutCursor( u32 index, bool is_visible )
{
  if( m_param->menu_cursor != NULL )
  {
    m_param->menu_cursor->Put( m_param->lytwk, m_param->pane_data[index].cursor_pos );
    m_param->menu_cursor->SetVisible( is_visible );
  }
  else
  {
    gfl2::math::VEC3 trans = m_param->pane_data[index].cursor_pos->GetTranslate();
    m_listener->PaneListListener_PutCursor( trans.x, trans.y, is_visible );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    MakePaneLow
 * @brief   ペインの座標順にテーブルを並び替える
 * @date    2015.07.30
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::MakePaneLow(void)
{
  for( u32 i=0; i<m_param->pane_data_max; i++ )
  {
    gfl2::math::VEC3 p1 = m_param->pane_data[ m_paneLowTable[i] ].base->GetTranslate();
    for( u32 j=i; j<m_param->pane_data_max; j++ )
    {
      gfl2::math::VEC3 p2 = m_param->pane_data[ m_paneLowTable[j] ].base->GetTranslate();
//      if( p1.x < p2.x || p1.y > p2.y )
      if( p1.x > p2.x || p1.y < p2.y )
      {
        u8 tmp = m_paneLowTable[i];
        m_paneLowTable[i] = m_paneLowTable[j];
        m_paneLowTable[j] = tmp;
        p1 = p2;
      }
    }
  }
/*
  for( u32 i=0; i<m_param->pane_data_max; i++ )
  {
    GFL_PRINT( "%d, ", m_paneLowTable[i] );
  }
  GFL_PRINT( "\n" );
*/
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPaneLowIndex
 * @brief   ペインの並び位置を取得
 * @date    2015.08.01
 *
 * @param   index   ペインのインデックス
 *
 * @return  並び位置
 */
//-----------------------------------------------------------------------------
u32 NewPaneListPartsBase::GetPaneLowIndex( u32 index )
{
  for( u32 i=0; i<m_param->pane_data_max; i++ )
  {
    if( m_paneLowTable[i] == index )
    {
      return i;
    }
  }
  GFL_ASSERT( 0 );
  return 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePaneLow
 * @brief   入れ替え用：ペイン並び替え
 * @date    2015.07.30
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::ChangePaneLow(void)
{
  MakePaneLow();

  if( m_insertPane == NULL )
  {
    return;
  }

//  for( s32 i=m_param->pane_data_max-1; i>=0; i-- )
  for( s32 i=0; i<m_param->pane_data_max; i++ )
  {
    if( m_isItemChange != false )
    {
      if( m_paneLowTable[i] == m_itemChangePane )
      {
        continue;
      }
    }
    InsertPane( m_paneLowTable[i] );
  }

  if( m_isItemChange != false )
  {
    InsertPane( m_itemChangePane );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InsertPane
 * @brief   ペインの表示優先を変更
 * @date    2015.08.04
 *
 * @param   index   ペインのインデックス
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::InsertPane( u32 index )
{
  gfl2::lyt::LytPane * parent = m_param->pane_data[index].base->GetParent();
  if( parent != NULL )
  {
    parent->RemoveChild( m_param->pane_data[index].base );
  }
  parent->InsertChild( m_insertPane, m_param->pane_data[index].base );
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckKeyInputSkip
 * @brief   スキップ入力をチェック
 * @date    2015.07.30
 *
 * @param   button  ボタンデバイス
 *
 * @retval  INPUT_NONE  = 入力なし
 * @retval  INPUT_PLUS  = ＋方向
 * @retval  INPUT_MINUS = －方向
 */
//-----------------------------------------------------------------------------
NewPaneListPartsBase::InputID NewPaneListPartsBase::CheckKeyInputSkip( gfl2::ui::Button * button )
{
  if( button->IsRepeat(gfl2::ui::BUTTON_R) )
  {
    return INPUT_PLUS;
  }
  if( button->IsRepeat(gfl2::ui::BUTTON_L) )
  {
    return INPUT_MINUS;
  }
  return INPUT_NONE;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetEffect
 * @brief   範囲外のペインにエフェクトをかける
 * @date    2015.07.31
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::SetEffect(void)
{
  if( m_effect == NewPaneList::EFFECT_MODE_ALPHA )
  {
    SetEffectAlpha();
  }else if( m_effect == NewPaneList::EFFECT_MODE_COLOR )
  {
    SetEffectColor();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CalcEffectParam
 * @brief   エフェクト値を計算
 * @date    2015.07.31
 *
 * @param   index   ペインのインデックス
 *
 * @return  エフェクト値
 */
//-----------------------------------------------------------------------------
u32 NewPaneListPartsBase::CalcEffectParam( u32 index )
{
  return 255;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetEffectAlpha
 * @brief   ペインに半透明エフェクトをかける
 * @date    2015.07.31
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::SetEffectAlpha(void)
{
  for( u32 i=0; i<m_param->pane_data_max; i++ )
  {
    m_param->pane_data[i].base->SetAlpha( CalcEffectParam(i) );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetEffectColor
 * @brief   ペインにカラーエフェクトをかける
 * @date    2015.07.31
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::SetEffectColor(void)
{
  GFL_ASSERT(0);
  //EFFECT_MODE_COLOR機能を使用する場合にはコメントを外してDynamicCastが適正かどうか確認を行って下さい
  /*
  nw::ut::Color8 color( 255, 255, 255, 255 );
  
  for( u32 i=0; i<m_param->pane_data_max; i++ )
  {
    color.r = CalcEffectParam( i );
    color.g = color.r;
    color.b = color.r;

    nw::lyt::PaneList & list = m_param->pane_data[i].base->GetChildList();
    nw::lyt::PaneList::Iterator it     = list.GetBeginIter();
    nw::lyt::PaneList::Iterator it_end = list.GetEndIter();

    while( it != it_end )
    {
      gfl2::lyt::LytPane * pane = nw::ut::DynamicCast<gfl2::lyt::LytPane *>( &(*it) );
      if(pane)
      {
        gfl2::lyt::LytMaterial * material = pane->GetMaterial();
        if( material != NULL )
        {
          material->SetColor( nw::lyt::INTERPOLATECOLOR_WHITE, color );
        }
      }
      it++;
    }

    { // 自分
      gfl2::lyt::LytMaterial * material = m_param->pane_data[i].base->GetMaterial();
      if( material != NULL )
      {
        material->SetColor( nw::lyt::INTERPOLATECOLOR_WHITE, color );
      }
    }
  }
     */
}

//-----------------------------------------------------------------------------
/**
 * @func    GetEffectParam
 * @brief   エフェクト値を取得
 * @date    2015.07.31
 *
 * @param   p1      座標１
 * @param   p2      座標２
 * @param   space   配置スペース
 *
 * @return  エフェクト値
 */
//-----------------------------------------------------------------------------
u32 NewPaneListPartsBase::GetEffectParam( f32 p1, f32 p2, f32 space )
{
  f32 prm_f = static_cast<f32>( EFFECT_PARAM );
  f32 val = gfl2::math::FAbs( p1 - p2 );
  space *= 2.0f;

  if( val <= space )
  {
    return static_cast<u32>( prm_f - prm_f / space * val );
  }

  return EFFECT_PARAM;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetItemChange
 * @brief   入れ替え処理
 * @date    2015.08.01
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::SetItemChange(void)
{
  if( m_isItemChange != false )
  {
    m_isItemChange = false;
    m_listener->PaneListListener_EndItemChange( m_param->pane_data, m_itemChangePane, m_itemChangePos, static_cast<u32>(m_param->list_scroll)+m_param->list_pos );
    ChangePaneLow();
//    MakePaneLow();
    SetBasePane();
  }
  else
  {
    m_itemChangePane = GetPosPaneIndex( m_param->list_pos );
    m_itemChangePos  = static_cast<u32>(m_param->list_scroll) + m_param->list_pos;
    m_isItemChange   = true;
    m_listener->PaneListListener_StartItemChange( m_param->pane_data, m_itemChangePane, m_itemChangePos );
    ChangePaneLow();
    InitChangeLowTable();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CancelItemChange
 * @brief   入れ替えキャンセル
 * @date    2015.08.04
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::CancelItemChange(void)
{
  m_itemChangePos  = static_cast<u32>(m_param->list_scroll) + m_param->list_pos;
  SetItemChange();
  InitListPut();
}

//-----------------------------------------------------------------------------
/**
 * @func    IsItemChange
 * @brief   入れ替え処理中か
 * @date    2015.08.01
 *
 * @retval  true  = 処理中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool NewPaneListPartsBase::IsItemChange(void)
{
  return m_isItemChange;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetItemChangePos
 * @brief   入れ替え位置を取得
 * @date    2015.08.01
 *
 * @return  入れ替え位置
 */
//-----------------------------------------------------------------------------
u32 NewPaneListPartsBase::GetItemChangePos(void)
{
  return m_itemChangePos;
}

//-----------------------------------------------------------------------------
/**
 * @func    SwapItemChangePane
 * @brief   入れ替え：ペインを入れ替える（方向指定）
 * @date    2015.08.01
 *
 * @param   mv  入れ替え方向
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::SwapItemChangePane( s32 mv )
{
  s32 idx = GetPaneLowIndex( m_itemChangePane );
  s32 chg = 0;

  if( mv > 0 )
  {
    chg = idx + 1;
    if( idx >= m_param->pane_data_max )
    {
      chg -= m_param->pane_data_max;
    }
  }
  else if( mv < 0 )
  {
    chg = idx - 1;
    if( idx < 0 )
    {
      chg += m_param->pane_data_max;
    }
  }

  gfl2::math::VEC3 idx_pos = m_param->pane_data[ m_paneLowTable[idx] ].base->GetTranslate();
  gfl2::math::VEC3 chg_pos = m_param->pane_data[ m_paneLowTable[chg] ].base->GetTranslate();
  m_param->pane_data[ m_paneLowTable[idx] ].base->SetTranslate( chg_pos );
  m_param->pane_data[ m_paneLowTable[chg] ].base->SetTranslate( idx_pos );

  u8 tmp = m_paneLowTable[idx];
  m_paneLowTable[idx] = m_paneLowTable[chg];
  m_paneLowTable[chg] = tmp;
}

//-----------------------------------------------------------------------------
/**
 * @func    SwapItemChangePane
 * @brief   入れ替え：ペインを入れ替える（範囲指定）
 * @date    2015.08.03
 *
 * @param   p1  開始ペイン位置
 * @param   p2  終了ペイン位置
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::SwapItemChangePane( s32 p1, s32 p2 )
{
  u32 p1_idx = GetPosPaneIndex( p1 );
  gfl2::math::VEC3 pos1 = m_param->pane_data[p1_idx].base->GetTranslate();
  gfl2::math::VEC3 pos2;
  if( p1 < p2 )
  {
    for( s32 i=p1; i<p2; i++ )
    {
      u32 idx = GetPosPaneIndex( i+1 );
      pos2 = m_param->pane_data[idx].base->GetTranslate();
      m_param->pane_data[idx].base->SetTranslate( pos1 );
      pos1 = pos2;
    }
  }
  else
  {
    for( s32 i=p1; i>p2; i-- )
    {
      u32 idx = GetPosPaneIndex( i-1 );
      pos2 = m_param->pane_data[idx].base->GetTranslate();
      m_param->pane_data[idx].base->SetTranslate( pos1 );
      pos1 = pos2;
    }
  }
  m_param->pane_data[p1_idx].base->SetTranslate( pos2 );

  MakePaneLow();
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateChangeLowTable
 * @brief   入れ替え用のテーブルを生成
 * @date    2015.08.03
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::CreateChangeLowTable( app::util::Heap * heap )
{
  m_changeLowTableMax = m_param->pane_data_max - 1;   // 画面に表示されている分だけ
  m_changeLowTable = GFL_NEW_ARRAY(heap->GetSystemHeap()) s32[m_changeLowTableMax];
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteChangeLowTable
 * @brief   入れ替え用のテーブルを削除
 * @date    2015.08.03
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::DeleteChangeLowTable(void)
{
  GFL_SAFE_DELETE_ARRAY( m_changeLowTable );
}

//-----------------------------------------------------------------------------
/**
 * @func    InitChangeLowTable
 * @brief   入れ替え用のテーブルを初期化
 * @date    2015.08.03
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::InitChangeLowTable(void)
{
  GFL_PRINT( "■Start: " );
  for( s32 i=0; i<m_changeLowTableMax; i++ )
  {
    m_changeLowTable[i] = static_cast<s32>(m_param->list_scroll)+m_param->limit_top + i;
    GFL_PRINT( "%d, ", m_changeLowTable[i] );
  }
  GFL_PRINT( "\n" );
}

//-----------------------------------------------------------------------------
/**
 * @func    SwapChangeLowTable
 * @brief   入れ替え用のテーブル入れ替え
 * @date    2015.08.03
 *
 * @param   p1  位置１
 * @param   p2  位置２
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::SwapChangeLowTable( s32 p1, s32 p2 )
{
  p1 -= m_param->limit_top;
  p2 -= m_param->limit_top;

  s32 tmp = m_changeLowTable[p1];
  if( p1 < p2 )
  {
    for( s32 i=p1; i<p2; i++ )
    {
      m_changeLowTable[i] = m_changeLowTable[i+1];
    }
  }
  else
  {
    for( s32 i=p1; i>p2; i-- )
    {
      m_changeLowTable[i] = m_changeLowTable[i-1];
    }
  }
  m_changeLowTable[p2] = tmp;

  /////
  GFL_PRINT( "  " );
  for( s32 i=0; i<m_changeLowTableMax; i++ )
  {
    GFL_PRINT( "%d, ", m_changeLowTable[i] );
  }
  GFL_PRINT( "\n" );
  /////
}

//-----------------------------------------------------------------------------
/**
 * @func    ScrollChangeLowTable
 * @brief   入れ替え用テーブルをスクロール
 * @date    2015.08.03
 *
 * @param   mv  スクロール方向
 *
 * @return  回り込んだペインのインデックス
 */
//-----------------------------------------------------------------------------
s32 NewPaneListPartsBase::ScrollChangeLowTable( s32 mv )
{
  s32 p = -1;
  for( s32 i=0; i<m_changeLowTableMax; i++ )
  {
    if( m_changeLowTable[i] == m_itemChangePos )
    {
      p = i;
      break;
    }
  }

  if( mv < 0 )
  {
    if( p == -1 || p == 0 )
    {
      GFL_ASSERT( 0 );
      return m_changeLowTable[0] - 1;
    }
    for( s32 i=m_changeLowTableMax-1; i>p+1; i-- )
    {
      m_changeLowTable[i] = m_changeLowTable[i-1];
    }
    if( (p+1) < m_changeLowTableMax )
    {
      m_changeLowTable[p+1] = m_changeLowTable[p-1];
    }
    for( s32 i=p-1; i>0; i-- )
    {
      m_changeLowTable[i] = m_changeLowTable[i-1];
    }
    m_changeLowTable[0] -=1;
    if( m_changeLowTable[0] == m_itemChangePos )
    {
      m_changeLowTable[0] -= 1;
    }
    /////
    GFL_PRINT( "  " );
    for( s32 i=0; i<m_changeLowTableMax; i++ )
    {
      GFL_PRINT( "%d, ", m_changeLowTable[i] );
    }
    GFL_PRINT( "\n" );
    /////
    return m_changeLowTable[0];
  }
  else
  {
    if( p == -1 || p == (m_changeLowTableMax-1) )
    {
      GFL_ASSERT( 0 );
      return m_changeLowTable[m_changeLowTableMax-1] + 1;
    }
    for( s32 i=0; i<p-1; i++ )
    {
      m_changeLowTable[i] = m_changeLowTable[i+1];
    }
    if( (p-1) >= 0 )
    {
      m_changeLowTable[p-1] = m_changeLowTable[p+1];
    }
    for( s32 i=p+1; i<m_changeLowTableMax-1; i++ )
    {
      m_changeLowTable[i] = m_changeLowTable[i+1];
    }
    m_changeLowTable[m_changeLowTableMax-1] +=1;
    if( m_changeLowTable[m_changeLowTableMax-1] == m_itemChangePos )
    {
      m_changeLowTable[m_changeLowTableMax-1] += 1;
    }
    /////
    GFL_PRINT( "  " );
    for( s32 i=0; i<m_changeLowTableMax; i++ )
    {
      GFL_PRINT( "%d, ", m_changeLowTable[i] );
    }
    GFL_PRINT( "\n" );
    /////
    return m_changeLowTable[m_changeLowTableMax-1];
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    LoopChangeLowTable
 * @brief   入れ替え用のテーブルをループ
 * @date    2015.08.03
 *
 * @param   mv  ループ方向
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::LoopChangeLowTable( s32 mv )
{
  for( s32 i=0; i<m_changeLowTableMax; i++ )
  {
    m_changeLowTable[i] = -1;
  }

  if( mv > 0 )
  {
    m_changeLowTable[-m_param->limit_top] = m_itemChangePos;
    s32 p = 0;
    for( s32 i=(-m_param->limit_top+1); i<m_changeLowTableMax; i++ )
    {
      if( p == m_itemChangePos )
      {
        p++;
      }
      m_changeLowTable[i] = p;
      p++;
    }
  }
  else
  {
    m_changeLowTable[m_changeLowTableMax-2] = m_itemChangePos;
    m_changeLowTable[m_changeLowTableMax-1] = m_param->list_max;  // 余剰分
    s32 p = m_param->list_max - 1;
    for( s32 i=m_changeLowTableMax-3; i>=0; i-- )
    {
      if( p == m_itemChangePos )
      {
        p--;
      }
      m_changeLowTable[i] = p;
      p--;
    }
  }

  /////
  GFL_PRINT( "  " );
  for( s32 i=0; i<m_changeLowTableMax; i++ )
  {
    GFL_PRINT( "%d, ", m_changeLowTable[i] );
  }
  GFL_PRINT( "\n" );
  /////
}

//-----------------------------------------------------------------------------
/**
 * @func    SkipChangeLowTable
 * @brief   入れ替え用のテーブルをスキップ
 * @date    2015.08.03
 *
 * @param   offs  スキップする値
 * @param   mv    方向
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsBase::SkipChangeLowTable( s32 offs, s32 mv )
{
  s32 idx = 0;

  for( s32 i=0; i<m_changeLowTableMax; i++ )
  {
    if( m_changeLowTable[i] == m_itemChangePos )
    {
      idx = i;
      break;
    }
  }

  if( idx == 0 || idx == (m_changeLowTableMax-1) )
  {
    GFL_ASSERT( 0 );
    return;
  }

  s32 p = 0;

  if( mv > 0 )
  {
    p = m_changeLowTable[idx+1] + offs;
    s32 tmp = p;
    for( s32 i=idx+1; i<m_changeLowTableMax; i++ )
    {
      m_changeLowTable[i] = p;
      p++;
      if( p == m_itemChangePos )
      {
        p++;
      }
    }
    p = tmp - 1;
    for( s32 i=idx-1; i>=0; i-- )
    {
      m_changeLowTable[i] = p;
      p--;
      if( p == m_itemChangePos )
      {
        p--;
      }
    }
  }
  else
  {
    p = m_changeLowTable[idx-1] + offs;
    s32 tmp = p;
    for( s32 i=idx-1; i>=0; i-- )
    {
      m_changeLowTable[i] = p;
      p--;
      if( p == m_itemChangePos )
      {
        p--;
      }
    }
    p = tmp + 1;
    for( s32 i=idx+1; i<m_changeLowTableMax; i++ )
    {
      m_changeLowTable[i] = p;
      p++;
      if( p == m_itemChangePos )
      {
        p++;
      }
    }
  }

  /////
  GFL_PRINT( "  " );
  for( s32 i=0; i<m_changeLowTableMax; i++ )
  {
    GFL_PRINT( "%d, ", m_changeLowTable[i] );
  }
  GFL_PRINT( "\n" );
  /////
}



GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)
