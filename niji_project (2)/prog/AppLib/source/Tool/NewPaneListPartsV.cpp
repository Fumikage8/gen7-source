#if 0 // 未実装
//=============================================================================
/**
 * @file    NewPaneListPartsV.cpp
 * @brief   ペインリスト項目関連処理（縦リスト）
 * @author  Hiroyuki Nakamura
 * @date    2015.07.22
 */
//=============================================================================

// module
#include "NewPaneListPartsV.h"
// niji
#include "AppLib/include/Tool/app_tool_MenuCursor.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

NewPaneListPartsV::NewPaneListPartsV(
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

NewPaneListPartsV::~NewPaneListPartsV()
{
}

void NewPaneListPartsV::InitListPut(void)
{
}

f32 NewPaneListPartsV::InitListPutBase( s32 mv )
{
  return 0.0f;
}

f32 NewPaneListPartsV::MoveList( s32 offs )
{
  return 0.0f;
}

u32 NewPaneListPartsV::GetPosPaneIndex( int pos )
{
  f32 py = m_param->start_y - m_param->space * static_cast<f32>(pos);

  for( u32 i=0; i<m_param->pane_data_max; i++ )
  {
    gfl2::math::VEC3 trans = m_param->pane_data[i].base->GetTranslate();
    if( trans.y == py )
    {
      return i;
    }
  }
  GFL_ASSERT( 0 );
  return m_basePaneIndex;
}

s32 NewPaneListPartsV::GetPosPaneLow( u32 index )
{
  gfl2::math::VEC3 trans = m_param->pane_data[index].base->GetTranslate();
  return static_cast<s32>( ( m_param->start_y - trans.y ) / m_param->space );
}


void NewPaneListPartsV::ChangeButtonEnable(void)
{
  f32 max_y = m_param->start_y + m_param->space * static_cast<f32>(m_param->list_pos_max);

  for( u32 i=0; i<m_param->pane_data_max; i++ )
  {
    bool flg = false;
    if( m_param->pane_data[i].base->IsVisible() != false )
    {
      gfl2::math::VEC3 trans = m_param->pane_data[i].base->GetTranslate();
      if( trans.y <= m_param->start_y && trans.y >= max_y )
      {
        flg = true;
      }
    }
    m_buttonManager->SetButtonInputEnable( i, flg );
  }
}


NewPaneListPartsBase::InputID NewPaneListPartsV::CheckKeyInput(void)
{
  if( m_key == NULL )
  {
    GFL_ASSERT( 0 );
    return INPUT_NONE;
  }
  return CheckKeyInput( m_key );
}

NewPaneListPartsBase::InputID NewPaneListPartsV::CheckKeyInput( gfl2::ui::VectorDevice * key )
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

NewPaneListPartsBase::InputID NewPaneListPartsV::CheckKeyInputSkip( gfl2::ui::VectorDevice * key )
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

bool NewPaneListPartsV::IsKeyHold( NewPaneListPartsBase::InputID id )
{
  if( m_key == NULL )
  {
    GFL_ASSERT( 0 );
    return false;
  }
  if( id == INPUT_PLUS )
  {
    return m_key->IsHold(gfl2::ui::DIR_DOWN);
  }
  else if( id == INPUT_MINUS )
  {
    return m_key->IsHold(gfl2::ui::DIR_UP);
  }
  return false;
}


u32 NewPaneListPartsV::CalcEffectParam( u32 index )
{
  f32 end_y = m_param->start_y - static_cast<f32>(m_param->list_pos_max) * m_param->space;

  gfl2::math::VEC3 trans = m_param->pane_data[index].base->GetTranslate();

  if( trans.y > m_param->start_y )
  {
    return GetEffectParam( trans.y, m_param->start_y, m_param->space );
  }
  else if( trans.y < end_y )
  {
    return GetEffectParam( trans.y, end_y, m_param->space );
  }
  return NewPaneListPartsBase::EFFECT_PARAM;
}



void NewPaneListPartsV::StartItemChangeScroll( s32 mv )
{
/*
  f32 lx = m_param->start_x + m_param->space * static_cast<f32>(m_param->limit_top);
  f32 rx = m_param->start_x + m_param->space * static_cast<f32>(m_param->limit_bottom);

  for( u32 i=0; i<m_param->pane_data_max; i++ )
  {
    gfl2::math::VEC3 trans = m_param->pane_data[i].base->GetTranslate();
    if( !( trans.x >= lx && trans.x <= rx ) )
    {
      s32 pos = static_cast<u32>(m_param->list_scroll);
      if( mv > 0 )
      {
        trans.x = rx + m_param->space;
        pos += ( m_param->limit_bottom + 1 );
      }
      else if( mv < 0 )
      {
        trans.x = rx - m_param->space;
        pos += ( m_param->limit_top - 1 );
      }
      m_param->pane_data[i].base->SetTranslate( trans );
      if( pos >= 0 && pos < m_param->list_max )
      {
        m_param->pane_data[i].base->SetVisible( true );
        m_listener->PaneListListener_Write( m_param->pane_data, i, pos );
      }
      break;
    }
  }

  ChangePaneLow();
*/
}

void NewPaneListPartsV::MoveChangeList( s32 offs )
{
/*
  MakePaneLow();

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
  }

  SetEffect();
*/
}

void NewPaneListPartsV::AdjustChangeList(void)
{
/*
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
*/
}

void NewPaneListPartsV::EndItemChangeScroll(void)
{
/*
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
*/
}

void NewPaneListPartsV::InitListPutItemChangeLoop(void)
{
}

void NewPaneListPartsV::InitListPutItemChangeSkip(void)
{
}


GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

#endif
