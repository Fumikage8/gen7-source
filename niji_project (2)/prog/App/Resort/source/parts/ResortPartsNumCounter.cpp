//======================================================================
/**
 * @file ResortPartsNumCounter.cpp
 * @date 2015/10/02 15:06:32
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortPartsNumCounter.h"

#include <App/Resort/source/ResortDrawerBase.h>
#include <AppLib/include/Ui/UIView.h>

// arc
#include <niji_conv_header/app/resort/resort_pane.h>
#include <niji_conv_header/message/msg_pokeresort_system.h>



GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
ResortCounter::ResortCounter( DrawerBase* base, u32 counterPartsIndex, u8 max )
  : m_pBase( base )
  , m_Max( max )
{
  static const u32 c_NumText[] = {
    PANENAME_RESORT_PLT_LOW_000_PANE_TEXTCOUNTER_00,
    PANENAME_RESORT_PLT_LOW_000_PANE_TEXTCOUNTER_01,
  };
  gfl2::lyt::LytParts* parts = m_pBase->GetParts( counterPartsIndex );

  for( u32 i=0; i<NUM_KETA_MAX; ++i )
  {
    m_Number[i]  = 0;
    m_NumText[i] = m_pBase->GetTextBox( c_NumText[i], parts );
  }

  // カウンターの更新
  UpdateCounter();
}

//------------------------------------------------------------------------------
/**
 * @brief   カウンターの操作
 * @param   operation   カウンターの操作
 */
//------------------------------------------------------------------------------
void ResortCounter::CounterOperation( Operation operation )
{
  u8 num = GetTotalNumber();

  switch( operation )
  {
  case OPE_COUNT_UP_10:
    {
      if( num == m_Max )
      {
        num = 0;
      }
      else {
        num += 10;

        if( m_Max < num )
        {
          num = m_Max;
        }
      }
    } break;

  case OPE_COUNT_DW_10:
    {
      if( num == 0 )
      {
        num = m_Max;
      }
      else if( num >= 10 )
      {
        num -= 10;
      }
      else {
        num = 0;
      }
    } break;

  case OPE_COUNT_UP_1:
    {
      if( num == m_Max )
      {
        num = 0;
      }
      else {
        num++;
      }
    } break;

  case OPE_COUNT_DW_1:
    {
      if( num == 0 )
      {
        num = m_Max;
      }
      else {
        num--;
      }
    } break;
  }

  SetTotalNumber( num );

  // カウンターの更新
  UpdateCounter();
}

//------------------------------------------------------------------------------
/**
 * @brief   カウンターの更新
 */
//------------------------------------------------------------------------------
void ResortCounter::UpdateCounter( void )
{
  app::ui::UIView* view = m_pBase->GetUIView();

  // 10
  view->SetTextboxPaneNumber( m_NumText[0], msg_prsys_common_04,
    m_Number[0], 1, 0, print::NUM_DISP_LEFT, print::NUM_CODE_ZENKAKU );

  // 1
  view->SetTextboxPaneNumber( m_NumText[1], msg_prsys_common_04,
    m_Number[1], 1, 0, print::NUM_DISP_LEFT, print::NUM_CODE_ZENKAKU );
}

//------------------------------------------------------------------------------
/**
 * @brief   数値の設定
 */
//------------------------------------------------------------------------------
void ResortCounter::SetTotalNumber( u8 num )
{
  if( num >= 10 )
  {
    m_Number[0] = static_cast<u8>(num / 10);
  }
  else {
    m_Number[0] = 0;
  }
  m_Number[1] = static_cast<u8>(num % 10);
}

//------------------------------------------------------------------------------
/**
 * @brief   数値の取得
 */
//------------------------------------------------------------------------------
u8 ResortCounter::GetTotalNumber( void ) const
{
  return m_Number[0] * 10 + m_Number[1];
}

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )
