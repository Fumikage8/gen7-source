//=============================================================================
/**
 * @file    btl_ClientSeq_BattleRoyalShowdown.cpp
 * @brief   バトルロイヤル決着時の演出
 * @author  obata_toshihiro
 * @date    2015.03.11
 */
//=============================================================================
#include "btl_ClientSeq_BattleRoyalShowdown.h"
#include "btl_common.h"
#include "btl_sound.h"
#include "btl_mainmodule.h"
#include "btlv/btlv_core.h"

#include <niji_conv_header/message/msg_btl_std.h>


GFL_NAMESPACE_BEGIN( btl )


//---------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//---------------------------------------------------------------------------
ClientSeq_BattleRoyalShowdown::ClientSeq_BattleRoyalShowdown( void ) : 
  m_viewSystem( NULL ),
  m_seq( 0 ),
  m_isFinished( true )
{
}

//---------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//---------------------------------------------------------------------------
ClientSeq_BattleRoyalShowdown::~ClientSeq_BattleRoyalShowdown()
{
}

//-----------------------------------------------------------------------------
/**
 * @brief セットアップ
 * @param setupParam  セットアップパラメータ
 */
//-----------------------------------------------------------------------------
void ClientSeq_BattleRoyalShowdown::Setup( const SetupParam& setupParam )
{
  m_viewSystem = setupParam.viewSystem;
}

//---------------------------------------------------------------------------
/**
 * @brief シーケンスを開始する
 */
//---------------------------------------------------------------------------
void ClientSeq_BattleRoyalShowdown::Start( void )
{
  m_seq = 0;
  m_isFinished = false;
}

//---------------------------------------------------------------------------
/**
 * @brief 表示シーケンスを更新する
 */
//---------------------------------------------------------------------------
void ClientSeq_BattleRoyalShowdown::Update( void )
{
  if( m_isFinished ) {
    return;
  }

  switch( m_seq )
  {
  case 0:
    m_viewSystem->PlaySe( SEQ_SE_BT_BROYAL_GONGEND );  // カン！カン！カン！カン！
    ++( m_seq );
    break;

  case 1:
    if( m_viewSystem->IsFinishSe( SEQ_SE_BT_BROYAL_GONGEND ) )
    {
      ++( m_seq );
    }
    break;

  case 2:
    m_viewSystem->CMD_StartMsgStd( BTL_STRID_STD_Royal_End, NULL );  //「しあい　しゅうりょう！」
    ++( m_seq );
    break;

  case 3:
    if( m_viewSystem->CMD_WaitMsg_WithoutHide() )
    {
      ++( m_seq );
    }
    break;

  case 4:
    m_isFinished = true;
    break;
  }
}

//---------------------------------------------------------------------------
/**
 * @brief 表示シーケンスが終了したか？
 * @retval true   終了した
 * @retval false  終了していない
 */
//---------------------------------------------------------------------------
bool ClientSeq_BattleRoyalShowdown::IsFinished( void ) const
{
  return m_isFinished;
}



GFL_NAMESPACE_END( btl )