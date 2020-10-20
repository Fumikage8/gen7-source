//=============================================================================
/**
 * @file    NewPaneListPartsH.cpp
 * @brief   ペインリスト項目関連処理（横リスト）
 * @author  Hiroyuki Nakamura
 * @date    2015.07.22
 */
//=============================================================================

// module
#include "NewPaneListPartsH.h"
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
NewPaneListPartsH::NewPaneListPartsH(
      const NewPaneList::SETUP_PARAM * param,
      NewPaneList::ListListener * listener,
      app::tool::ButtonManager * button_manager,
      gfl2::lyt::LytPane * insert_pane,
      u8 * low_tbl,
      f32 scroll_max,
      NewPaneList::EffectMode effect )
  : NewPaneListPartsBase( param, listener, button_manager, insert_pane, low_tbl, scroll_max, effect )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
NewPaneListPartsH::~NewPaneListPartsH()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    InitListPut
 * @brief   リスト初期配置
 * @date    2015.07.22
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsH::InitListPut(void)
{
  int pos = static_cast<int>( m_param->list_scroll ) + m_param->limit_top;
  int limit_bottom = static_cast<int>( m_param->list_scroll ) + m_param->limit_bottom;
  f32 px  = m_param->start_x + m_param->limit_top * m_param->space;
  if( pos < 0 )
  {
    px -= ( static_cast<f32>(pos) * m_param->space );   // posが-なので結果はpxに+される
    pos = 0;
  }

  for( int i=0; i<m_param->pane_data_max; i++ )
  {
    gfl2::math::VEC3 trans( px, m_param->start_y, 0.0f );
    m_param->pane_data[i].base->SetTranslate( trans );

    if( pos >= static_cast<int>(m_param->list_max) || pos > limit_bottom )
    {
      m_param->pane_data[i].base->SetVisible( false );
    }
    else
    {
      m_param->pane_data[i].base->SetVisible( true );
      m_listener->PaneListListener_Write( m_param->pane_data, i, pos );
    }
    pos++;
    px += m_param->space;
  }

  ChangePaneLow();

  ChangeButtonEnable();

  SetBasePane();

  SetEffect();
}

//-----------------------------------------------------------------------------
/**
 * @func    InitListPutBase
 * @brief   基準ペインからリストを配置
 * @date    2015.07.24
 *
 * @param   mv  配置方向
 *
 * @return  スクロール値
 */
//-----------------------------------------------------------------------------
f32 NewPaneListPartsH::InitListPutBase( s32 mv )
{
  f32 scroll = m_param->list_scroll;

  gfl2::math::VEC3 base_trans = m_param->pane_data[m_basePaneIndex].base->GetTranslate();

  if( base_trans.x != m_param->start_x )
  {
    f32 offs = m_param->start_x - base_trans.x;

    if( mv == 0 )
    {
      if( offs < 0 )
      {
        if( m_param->list_scroll != 0.0f )
        {
          f32 tmp = m_param->start_x+m_param->space - base_trans.x;
          if( gfl2::math::FAbs(tmp) < gfl2::math::FAbs(offs) )
          {
            offs = tmp;
            scroll -= 1.0f;
          }
        }
      }
      else
      {
        if( m_param->list_scroll != m_scrollMax )
        {
          f32 tmp = m_param->start_x-m_param->space - base_trans.x;
          if( gfl2::math::FAbs(tmp) < gfl2::math::FAbs(offs) )
          {
            offs = tmp;
            scroll += 1.0f;
          }
        }
      }
    }
    else if( mv > 0 )
    {
      if( offs < 0 )
      {
        offs = base_trans.x - m_param->start_x;
  //      scroll += 1.0f;
      }
    }
    else
    {
      if( offs > 0 )
      {
        offs = base_trans.x - m_param->start_x;
  //      scroll -= 1.0f;
      }
    }

    for( u32 i=0; i<m_param->pane_data_max; i++ )
    {
      gfl2::math::VEC3 trans = m_param->pane_data[i].base->GetTranslate();
      trans.x += offs;
      m_param->pane_data[i].base->SetTranslate( trans );
    }

    SetBasePane();

    SetEffect();
  }

  PutCursor( GetPosPaneIndex(m_param->list_pos), true );

  return scroll;
}

//-----------------------------------------------------------------------------
/**
 * @func    MoveList
 * @brief   リスト移動
 * @date    2015.07.24
 *
 * @param   offs  移動値
 *
 * @return  スクロール値
 */
//-----------------------------------------------------------------------------
f32 NewPaneListPartsH::MoveList( s32 offs )
{
  f32 scroll = m_param->list_scroll;
  bool loop = false;

  MakePaneLow();

  if( offs < 0 )
  {
    // スクロール限界の場合
    if( m_param->list_scroll == m_scrollMax )
    {
      gfl2::math::VEC3 trans = m_param->pane_data[m_basePaneIndex].base->GetTranslate();
      if( trans.x == m_param->start_x )
      {
        return m_param->list_scroll;
      }
      // 動ける場合は移動量をチェック
      s32 tmp = static_cast<s32>( gfl2::math::FAbs(trans.x-m_param->start_x) );
      if( tmp < gfl2::math::Abs(offs) )
      {
        offs = -tmp;
      }
    }

    f32 min_x = m_param->start_x + static_cast<f32>( m_param->limit_top - 1 ) * m_param->space;
    f32 base_x = 0.0f;
    u32 idx = GetPaneLowIndex( m_basePaneIndex );

    for( int i=0; i<m_param->pane_data_max; i++ )
    {
      gfl2::math::VEC3 trans = m_param->pane_data[ m_paneLowTable[idx] ].base->GetTranslate();

      // 基準ペイン
      if( i == 0 )
      {
        f32 tmp_x = trans.x + static_cast<f32>( offs );

        // スクロール変更位置へ移動したか
        f32 limit_x = m_param->start_x - m_param->space;
        if( tmp_x <= limit_x )
        {
          s32 move = static_cast<s32>( gfl2::math::FAbs(m_param->start_x-tmp_x) / m_param->space );
          GFL_ASSERT( move != 0 );
          if( (scroll+static_cast<f32>(move)) >= m_scrollMax )
          {
            move = static_cast<s32>( m_scrollMax-scroll );
            tmp_x = m_param->start_x - m_param->space*static_cast<f32>(move);
            offs = static_cast<s32>( tmp_x - trans.x );
          }
          scroll += static_cast<f32>( move );

          s32 tmp_idx = GetPaneLowIndex( m_basePaneIndex ) + move;
          if( tmp_idx >= m_param->pane_data_max )
          {
            tmp_idx -= m_param->pane_data_max;
          }
          m_basePaneIndex = m_paneLowTable[tmp_idx];
        }

        base_x = trans.x + static_cast<f32>( offs );
      }

      trans.x += static_cast<f32>( offs );

      if( trans.x <= min_x )
      {
        trans.x += ( m_param->space * static_cast<f32>(m_param->pane_data_max) );
        u32 put_pos = static_cast<u32>(m_param->list_scroll) + static_cast<u32>(gfl2::math::FAbs(trans.x-base_x)) / static_cast<u32>(m_param->space);
        if( put_pos < m_param->list_max )
        {
          m_param->pane_data[ m_paneLowTable[idx] ].base->SetVisible( true );
          m_listener->PaneListListener_Write( m_param->pane_data, m_paneLowTable[idx], put_pos );
        }
        else
        {
          m_param->pane_data[ m_paneLowTable[idx] ].base->SetVisible( false );
        }
        loop = true;
      }
      else
      {
        if( m_param->pane_data[ m_paneLowTable[idx] ].base->IsVisible() == false )
        {
          u32 put_pos = static_cast<u32>(m_param->list_scroll) + static_cast<u32>(gfl2::math::FAbs(trans.x-base_x)) / static_cast<u32>(m_param->space);
          if( put_pos < m_param->list_max && trans.x > m_param->start_x )
          {
            m_param->pane_data[ m_paneLowTable[idx] ].base->SetVisible( true );
            m_listener->PaneListListener_Write( m_param->pane_data, m_paneLowTable[idx], put_pos );
          }
        }
      }

      m_param->pane_data[ m_paneLowTable[idx] ].base->SetTranslate( trans );

      idx++;
      if( idx >= m_param->pane_data_max )
      {
        idx -= m_param->pane_data_max;
      }
    }
  }
  else if( offs > 0 )
  {
    // スクロール限界の場合
    if( m_param->list_scroll == 0.0f )
    {
      gfl2::math::VEC3 trans = m_param->pane_data[m_basePaneIndex].base->GetTranslate();
      if( trans.x == m_param->start_x )
      {
        return m_param->list_scroll;
      }
      // 動ける場合は移動量をチェック
      s32 tmp = static_cast<s32>( gfl2::math::FAbs(trans.x-m_param->start_x) );
      if( tmp < gfl2::math::Abs(offs) )
      {
        offs = tmp;
      }
    }

    f32 max_x = m_param->start_x + static_cast<f32>( m_param->limit_bottom + 1 ) * m_param->space;
    f32 base_x = 0.0f;
    u32 idx = GetPaneLowIndex( m_basePaneIndex );

    for( int i=0; i<m_param->pane_data_max; i++ )
    {
      gfl2::math::VEC3 trans = m_param->pane_data[ m_paneLowTable[idx] ].base->GetTranslate();

      // 基準ペイン
      if( i == 0 )
      {
        f32 tmp_x = trans.x + static_cast<f32>( offs );

        // スクロール変更位置へ移動したか
        f32 limit_x = m_param->start_x + m_param->space;
        if( tmp_x >= limit_x )
        {
          s32 move = static_cast<s32>( gfl2::math::FAbs(m_param->start_x-tmp_x) / m_param->space );
          GFL_ASSERT( move != 0 );
          if( (scroll-static_cast<f32>(move)) <= 0.0f )
          {
            move = static_cast<s32>( scroll );
            tmp_x = m_param->start_x + m_param->space*static_cast<f32>(move);
            offs = static_cast<s32>( tmp_x - trans.x );
          }
          scroll -= static_cast<f32>( move );

          s32 tmp_idx = static_cast<s32>(GetPaneLowIndex(m_basePaneIndex)) - move;
          if( tmp_idx < 0 )
          {
            tmp_idx += m_param->pane_data_max;
          }
          m_basePaneIndex = m_paneLowTable[tmp_idx];
        }

        base_x = trans.x + static_cast<f32>( offs );
      }

      trans.x += static_cast<f32>( offs );

      if( trans.x >= max_x )
      {
        trans.x -= ( m_param->space * static_cast<f32>(m_param->pane_data_max) );
        s32 put_pos = static_cast<s32>(m_param->list_scroll) - static_cast<s32>(gfl2::math::FAbs(trans.x-base_x)) / static_cast<u32>(m_param->space);
        if( put_pos >= 0 )
        {
          m_param->pane_data[ m_paneLowTable[idx] ].base->SetVisible( true );
          m_listener->PaneListListener_Write( m_param->pane_data, m_paneLowTable[idx], static_cast<u32>(put_pos) );
        }
        else
        {
          m_param->pane_data[ m_paneLowTable[idx] ].base->SetVisible( false );
        }
        loop = true;
      }
      else
      {
        if( m_param->pane_data[ m_paneLowTable[idx] ].base->IsVisible() == false )
        {
          s32 put_pos = static_cast<s32>(m_param->list_scroll) - static_cast<s32>(gfl2::math::FAbs(trans.x-base_x)) / static_cast<u32>(m_param->space);
          if( put_pos >= 0 && trans.x < m_param->start_x )
          {
            m_param->pane_data[ m_paneLowTable[idx] ].base->SetVisible( true );
            m_listener->PaneListListener_Write( m_param->pane_data, m_paneLowTable[idx], static_cast<u32>(put_pos) );
          }
        }
      }

      m_param->pane_data[ m_paneLowTable[idx] ].base->SetTranslate( trans );

      idx++;
      if( idx >= m_param->pane_data_max )
      {
        idx -= m_param->pane_data_max;
      }
    }
  }

  if( loop != false )
  {
    ChangePaneLow();
  }

  SetEffect();

  return scroll;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPosPaneIndex
 * @brief   リスト位置のペインを取得
 * @date    2015.07.29
 *
 * @param   リスト位置
 *
 * @return  ペインのインデックス
 */
//-----------------------------------------------------------------------------
u32 NewPaneListPartsH::GetPosPaneIndex( int pos )
{
  f32 px = m_param->start_x + m_param->space * static_cast<f32>(pos);

  for( u32 i=0; i<m_param->pane_data_max; i++ )
  {
    gfl2::math::VEC3 trans = m_param->pane_data[i].base->GetTranslate();
    if( trans.x == px )
    {
      return i;
    }
  }
  GFL_ASSERT( 0 );
  return m_basePaneIndex;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPosPaneLow
 * @brief   ペインの配置位置を取得
 * @date    2015.07.29
 *
 * @param   index   ペインのインデックス
 *
 * @return  配置位置
 */
//-----------------------------------------------------------------------------
s32 NewPaneListPartsH::GetPosPaneLow( u32 index )
{
  gfl2::math::VEC3 trans = m_param->pane_data[index].base->GetTranslate();
  return static_cast<s32>( ( trans.x - m_param->start_x ) / m_param->space );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeButtonEnable
 * @brief   ボタン入力可否を切り替える
 * @date    2015.07.30
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsH::ChangeButtonEnable(void)
{
  f32 max_x = m_param->start_x + m_param->space * static_cast<f32>(m_param->list_pos_max);

  for( u32 i=0; i<m_param->pane_data_max; i++ )
  {
    bool flg = false;
    if( m_param->pane_data[i].base->IsVisible() != false )
    {
      gfl2::math::VEC3 trans = m_param->pane_data[i].base->GetTranslate();
      if( trans.x >= m_param->start_x && trans.x <= max_x )
      {
        flg = true;
      }
    }
    m_buttonManager->SetButtonInputEnable( i, flg );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckKeyInput
 * @brief   キー入力チェック
 * @date    2015.07.29
 *
 * @retval  INPUT_NONE  = 入力なし
 * @retval  INPUT_PLUS  = ＋方向
 * @retval  INPUT_MINUS = －方向
 */
//-----------------------------------------------------------------------------
NewPaneListPartsBase::InputID NewPaneListPartsH::CheckKeyInput(void)
{
  if( m_key == NULL )
  {
    GFL_ASSERT( 0 );
    return INPUT_NONE;
  }
  return CheckKeyInput( m_key );
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckKeyInput
 * @brief   キー入力チェック
 * @date    2015.07.29
 *
 * @param   key   十字キーデバイス
 *
 * @retval  INPUT_NONE  = 入力なし
 * @retval  INPUT_PLUS  = ＋方向
 * @retval  INPUT_MINUS = －方向
 */
//-----------------------------------------------------------------------------
NewPaneListPartsBase::InputID NewPaneListPartsH::CheckKeyInput( gfl2::ui::VectorDevice * key )
{
  if( key->IsRepeat(gfl2::ui::DIR_RIGHT) )
  {
    return INPUT_PLUS;
  }
  if( key->IsRepeat(gfl2::ui::DIR_LEFT) )
  {
    return INPUT_MINUS;
  }
  return INPUT_NONE;
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckKeyInputSkip
 * @brief   スキップ入力チェック
 * @date    2015.07.30
 *
 * @param   key   十字キーデバイス
 *
 * @retval  INPUT_NONE  = 入力なし
 * @retval  INPUT_PLUS  = ＋方向
 * @retval  INPUT_MINUS = －方向
 */
//-----------------------------------------------------------------------------
NewPaneListPartsBase::InputID NewPaneListPartsH::CheckKeyInputSkip( gfl2::ui::VectorDevice * key )
{
  if( key->IsRepeat(gfl2::ui::DIR_DOWN) )
  {
    return INPUT_PLUS;
  }
  if( key->IsRepeat(gfl2::ui::DIR_UP) )
  {
    return INPUT_MINUS;
  }
  return INPUT_NONE;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsKeyHold
 * @brief   指定方向にキーが入力されているか
 * @date    2015.07.29
 *
 * @param   id  入力ID
 *
 * @retval  true  = 入力されている
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool NewPaneListPartsH::IsKeyHold( NewPaneListPartsBase::InputID id )
{
  if( m_key == NULL )
  {
    GFL_ASSERT( 0 );
    return false;
  }
  if( id == INPUT_PLUS )
  {
    return m_key->IsHold(gfl2::ui::DIR_RIGHT);
  }
  else if( id == INPUT_MINUS )
  {
    return m_key->IsHold(gfl2::ui::DIR_LEFT);
  }
  return false;
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
u32 NewPaneListPartsH::CalcEffectParam( u32 index )
{
  f32 end_x = m_param->start_x + static_cast<f32>(m_param->list_pos_max) * m_param->space;

  gfl2::math::VEC3 trans = m_param->pane_data[index].base->GetTranslate();

  if( trans.x < m_param->start_x )
  {
    return GetEffectParam( trans.x, m_param->start_x, m_param->space );
  }
  else if( trans.x > end_x )
  {
    return GetEffectParam( trans.x, end_x, m_param->space );
  }
  return NewPaneListPartsBase::EFFECT_PARAM;
}

//-----------------------------------------------------------------------------
/**
 * @func    StartItemChangeScroll
 * @brief   入れ替え用：スクロール開始
 * @date    2015.08.01
 *
 * @param   mv  スクロール方向
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsH::StartItemChangeScroll( s32 mv )
{
  f32 lx = m_param->start_x + m_param->space * static_cast<f32>(m_param->limit_top);
  f32 rx = m_param->start_x + m_param->space * static_cast<f32>(m_param->limit_bottom);

  for( u32 i=0; i<m_param->pane_data_max; i++ )
  {
    gfl2::math::VEC3 trans = m_param->pane_data[i].base->GetTranslate();
    if( !( trans.x >= lx && trans.x <= rx ) )
    {
      if( mv > 0 )
      {
        trans.x = rx + m_param->space;
      }
      else if( mv < 0 )
      {
        trans.x = lx - m_param->space;
      }
      m_param->pane_data[i].base->SetTranslate( trans );
      s32 pos = ScrollChangeLowTable( mv );
      if( pos >= 0 && pos < m_param->list_max )
      {
        m_param->pane_data[i].base->SetVisible( true );
        m_listener->PaneListListener_Write( m_param->pane_data, i, pos );
      }
      break;
    }
  }

  ChangePaneLow();
}

//-----------------------------------------------------------------------------
/**
 * @func    MoveChangeList
 * @brief   入れ替え用：リスト動作
 * @date    2015.08.01
 *
 * @param   offs  移動値
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsH::MoveChangeList( s32 offs )
{
  s32 chg_idx = GetPaneLowIndex( m_itemChangePane );
  s32 bst_idx;
  if( offs < 0 )
  {
    bst_idx = chg_idx + 1;
    if( bst_idx >= m_param->pane_data_max )
    {
      bst_idx -= m_param->pane_data_max;
    }
  }
  else
  {
    bst_idx = chg_idx - 1;
    if( bst_idx < 0 )
    {
      bst_idx += m_param->pane_data_max;
    }
  }

  for( int i=0; i<m_param->pane_data_max; i++ )
  {
    if( i == chg_idx )
    {
      continue;
    }
    gfl2::math::VEC3 trans = m_param->pane_data[ m_paneLowTable[i] ].base->GetTranslate();
    trans.x += offs;
    if( i == bst_idx )
    {
      trans.x += offs;
    }
    m_param->pane_data[ m_paneLowTable[i] ].base->SetTranslate( trans );
  }

  SetEffect();
}

//-----------------------------------------------------------------------------
/**
 * @func    AdjustChangeList
 * @brief   入れ替え用：リスト配置補正
 * @date    2015.08.01
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsH::AdjustChangeList(void)
{
  MakePaneLow();

  s32 idx = GetPaneLowIndex( m_itemChangePane );

  gfl2::math::VEC3 trans = m_param->pane_data[ m_itemChangePane ].base->GetTranslate();

  for( int i=0; i<m_param->pane_data_max-1; i++ )
  {
    trans.x += m_param->space;
    idx++;
    if( idx >= m_param->pane_data_max )
    {
      idx = 0;
      trans.x -= ( m_param->space * static_cast<f32>(m_param->pane_data_max) );
    }
    m_param->pane_data[ m_paneLowTable[idx] ].base->SetTranslate( trans );
  }

  SetEffect();
}

//-----------------------------------------------------------------------------
/**
 * @func    EndItemChangeScroll
 * @brief   入れ替え用：スクロール終了
 * @date    2015.08.01
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsH::EndItemChangeScroll(void)
{
  f32 lx = m_param->start_x + m_param->space * static_cast<f32>(m_param->limit_top);
  f32 rx = m_param->start_x + m_param->space * static_cast<f32>(m_param->limit_bottom);

  for( u32 i=0; i<m_param->pane_data_max; i++ )
  {
    gfl2::math::VEC3 trans = m_param->pane_data[i].base->GetTranslate();
    if( !( trans.x >= lx && trans.x <= rx ) )
    {
      m_param->pane_data[i].base->SetVisible( false );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InitListPutItemChangeLoop
 * @brief   入れ替え用：ループ時のリスト配置
 * @date    2015.08.01
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsH::InitListPutItemChangeLoop(void)
{
  if( m_param->list_pos == 0 )
  {
    gfl2::math::VEC3 trans = m_param->pane_data[ m_itemChangePane ].base->GetTranslate();
    trans.x = m_param->start_x;
    m_param->pane_data[ m_itemChangePane ].base->SetTranslate( trans );

    u32 idx = m_itemChangePane;

    f32 rx = m_param->start_x + m_param->space * static_cast<f32>(m_param->limit_bottom);
    u32 pos = 0;
    
    for( int i=0; i<m_param->pane_data_max-1; i++ )
    {
      trans.x += m_param->space;
      idx++;
      if( idx >= m_param->pane_data_max )
      {
        idx = 0;
//        trans.x -= ( m_param->space * static_cast<f32>(m_param->pane_data_max) );
      }
      m_param->pane_data[idx].base->SetTranslate( trans );

      if( trans.x > rx )
      {
        m_param->pane_data[idx].base->SetVisible( false );
      }
      else
      {
        m_param->pane_data[idx].base->SetVisible( true );
        if( pos == m_itemChangePos )
        {
          pos++;
        }
        m_listener->PaneListListener_Write( m_param->pane_data, idx, pos );
        pos++;
      }
    }
  }
  else
  {
    gfl2::math::VEC3 trans = m_param->pane_data[ m_itemChangePane ].base->GetTranslate();
    trans.x = m_param->start_x + m_param->space * static_cast<f32>(m_param->list_pos_max);
    m_param->pane_data[ m_itemChangePane ].base->SetTranslate( trans );

    u32 idx = m_itemChangePane;

    f32 lx = m_param->start_x + m_param->space * static_cast<f32>(m_param->limit_top);
    u32 pos = m_param->list_max - 1;
    
    for( int i=0; i<m_param->pane_data_max-1; i++ )
    {
      trans.x -= m_param->space;
      idx++;
      if( idx >= m_param->pane_data_max )
      {
        idx = 0;
//        trans.x += ( m_param->space * static_cast<f32>(m_param->pane_data_max) );
      }
      m_param->pane_data[idx].base->SetTranslate( trans );

      if( trans.x < lx )
      {
        m_param->pane_data[idx].base->SetVisible( false );
      }
      else
      {
        m_param->pane_data[idx].base->SetVisible( true );
        if( pos == m_itemChangePos )
        {
          pos--;
        }
        m_listener->PaneListListener_Write( m_param->pane_data, idx, pos );
        pos--;
      }
    }
  }

  MakePaneLow();
  SetEffect();
}

//-----------------------------------------------------------------------------
/**
 * @func    InitListPutItemChangeSkip
 * @brief   入れ替え用：スキップ時のリスト配置
 * @date    2015.08.03
 */
//-----------------------------------------------------------------------------
void NewPaneListPartsH::InitListPutItemChangeSkip(void)
{
  f32 px = m_param->start_x + m_param->limit_top * m_param->space;
  u32 idx = 0;

  for( u32 i=0; i<m_param->pane_data_max-1; i++ )
  {
    if( m_changeLowTable[i] != m_itemChangePos )
    {
      if( idx == m_itemChangePane )
      {
        idx++;
      }
      gfl2::math::VEC3 trans = m_param->pane_data[idx].base->GetTranslate();
      trans.x = px;
      m_param->pane_data[idx].base->SetTranslate( trans );
      if( m_changeLowTable[i] >= 0 && m_changeLowTable[i] < m_param->list_max )
      {
        m_param->pane_data[idx].base->SetVisible( true );
        m_listener->PaneListListener_Write( m_param->pane_data, idx, m_changeLowTable[i] );
      }
      else
      {
        m_param->pane_data[idx].base->SetVisible( false );
      }
      idx++;
    }
    px += m_param->space;
  }

  { // 余剰分を配置
    idx = m_param->pane_data_max - 1;
    if( idx == m_itemChangePane )
    {
      idx--;
    }
    gfl2::math::VEC3 trans = m_param->pane_data[idx].base->GetTranslate();
    trans.x = px;
    m_param->pane_data[idx].base->SetTranslate( trans );
    m_param->pane_data[idx].base->SetVisible( false );
  }

  MakePaneLow();
  SetEffect();
}


GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)
