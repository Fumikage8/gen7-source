//======================================================================
/**
 * @file btl_SimpleEffectFailManager.cpp
 * @date 2016/04/15 21:34:16
 * @author taya
 * @brief カテゴリ：シンプルエフェクトのワザが失敗した時の原因保存＆適切なリアクション表示コマンド生成
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <niji_conv_header/message/msg_btl_std.h>
#include <niji_conv_header/message/msg_btl_set.h>
#include <pml/include/pmlib.h>
#include "./btl_common.h"
#include "./btl_MainModule.h"
#include "./btl_event.h"
#include "./btl_str.h"
#include "./btl_server_cmd.h"
#include "./btl_ServerFlow.h"
#include "./btl_SimpleEffectFailManager.h"

GFL_NAMESPACE_BEGIN(btl)

/**
 * @ctor
 */
SimpleEffectFailManager::SimpleEffectFailManager()
{
  init();
  m_isAvailable = false;
}
/**
 * @dtor
 */
SimpleEffectFailManager::~SimpleEffectFailManager()
{

}

// 格納の受付開始（ワザ１件ごとに実行）
void SimpleEffectFailManager::Start( void )
{
  init();
  m_isAvailable = true;
}

// 格納の受付終了（Startと対で使う）
void SimpleEffectFailManager::End( void )
{
  m_isAvailable = false;
}


// 失敗理由コードを１件登録
void SimpleEffectFailManager::AddCause( Cause cause )
{
  if( !m_isAvailable ){
    return;
  }
  if( m_numCause >= GFL_NELEMS(m_causes) ){
    GFL_ASSERT( false );
    return;
  }

  m_causes[ m_numCause++ ] = cause;
}

// 蓄積された失敗コードから妥当な結果コードを返す
SimpleEffectFailManager::Result
  SimpleEffectFailManager::GetResult( void ) const
{
  if( !m_isAvailable ){
    GFL_ASSERT( false );
    return RESULT_OTHER_EFFECTS;
  }

  // 射つ側が原因で失敗する場合：うまく決まらなかった
  if( countFailCode(CAUSE_SELF) > 0 ){
    return RESULT_STD;
  }

  // みがわりで防がれた場合：効果がないようだ
  if( countFailCode(CAUSE_MIGAWARI) > 0 ){
    return RESULT_NO_EFFECT;
  }

  return RESULT_OTHER_EFFECTS;
}



// メンバー初期化
void SimpleEffectFailManager::init( void )
{
  for(uint i=0; i<GFL_NELEMS(m_causes); ++i){
    m_causes[i] = CAUSE_NULL;
  }

  m_numCause = 0;
}

// 指定された失敗理由コードが何件格納されているか数える
uint SimpleEffectFailManager::countFailCode( Cause failCode ) const
{
  uint count = 0;
  for(uint i=0; i<m_numCause; ++i)
  {
    if( m_causes[i] == failCode ){
      ++count;
    }
  }
  return count;
}

GFL_NAMESPACE_END(btl)
