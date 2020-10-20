//=============================================================================
/**
 * @file   btl_ActionRecorder.cpp
 * @date   2015/12/24 10:41:25
 * @author obata_toshihiro
 * @brief  ポケモンの行動結果を記録するクラス
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_ActionRecorder.h"

#include <util/include/gfl2_std_string.h>

GFL_NAMESPACE_BEGIN( btl )



//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
ActionRecorder::ActionRecorder( void )
{
  this->Clear();
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
ActionRecorder::~ActionRecorder()
{
}


//-----------------------------------------------------------------------------
/**
 * @brief 行動結果をクリアする
 */
//-----------------------------------------------------------------------------
void ActionRecorder::Clear( void )
{
  for( u32 i=0; i<RECORD_TURN_NUM; ++i )
  {
    this->ClearTurnData( &m_turnData[i] );
  }
}

/**
 * @brief １ターン分のデータをクリアする
 * @param turnData  クリア対象のデータ
 */
void ActionRecorder::ClearTurnData( TurnData* turnData ) const
{
  for( u32 i=0; i<BTL_POKEID_MAX; ++i )
  {
    turnData->pokeData[i].actionFlag = 0;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief ターン開始時の更新処理
 */
//-----------------------------------------------------------------------------
void ActionRecorder::StartTurn( void )
{
  // 1つずつ配列要素を後ろにズラす
  for( u32 i=RECORD_TURN_NUM-1; i>0; --i )
  {
    m_turnData[i] = m_turnData[i-1];
  }
  this->ClearTurnData( &m_turnData[0] );
}

//-----------------------------------------------------------------------------
/**
 * @brief 行動結果をチェックする
 * @param pokeId        チェック対象ポケモンのID
 * @param backTurnCount 遡るターン数( 0～RECORD_TURN_NUM-1, 0なら現在のターン )
 * @param actionId      チェック対象の行動結果
 * @retval true   指定した行動結果に該当する
 * @retval false　指定した行動結果に該当しない
 */
//-----------------------------------------------------------------------------
bool ActionRecorder::CheckAction( u8 pokeId, u8 backTurnCount, ActionID actionId ) const
{
  if( BTL_POKEID_MAX <= pokeId )
  {
    GFL_ASSERT(0);
    return false;
  }

  if( RECORD_TURN_NUM <= backTurnCount )
  {
    GFL_ASSERT(0);
    return false;
  }

  if( ACTION_NUM <= actionId )
  {
    GFL_ASSERT(0);
    return false;
  }

  const u32 checkBit = ( 1 << actionId );
  const u32 actionFlag = m_turnData[ backTurnCount ].pokeData[ pokeId ].actionFlag;
  return ( ( checkBit & actionFlag ) != 0 );
}

//-----------------------------------------------------------------------------
/**
 * @brief 行動結果を設定する
 * @param pokeId    設定対象ポケモンのID
 * @param actionId  設定対象の行動結果
 */
//-----------------------------------------------------------------------------
void ActionRecorder::SetAction( u8 pokeId, ActionID actionId )
{
  if( BTL_POKEID_MAX <= pokeId )
  {
    GFL_ASSERT(0);
    return;
  }

  if( ACTION_NUM <= actionId )
  {
    GFL_ASSERT(0);
    return;
  }

  const u32 setBit = ( 1 << actionId );
  m_turnData[ 0 ].pokeData[ pokeId ].actionFlag |= setBit;
}

  

GFL_NAMESPACE_END( btl )