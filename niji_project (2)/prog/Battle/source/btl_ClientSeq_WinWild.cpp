//=============================================================================
/**
 * @file   btl_ClientSeq_WinWild.cpp
 * @date   2015/08/28 15:52:10
 * @author obata_toshihiro
 * @brief  クライアントのシーケンス( 野生戦勝利 )
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_ClientSeq_WinWild.h"

#include "./btl_ui.h"
#include "./btl_mainmodule.h"
#include "./btl_client.h"
#include "./btl_PokeID.h"
#include "./btlv/btlv_core.h"

GFL_NAMESPACE_BEGIN( btl )


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
ClientSeq_WinWild::ClientSeq_WinWild( void ) :
  m_mainModule( NULL ),
  m_client( NULL ),
  m_viewSystem( NULL ),
  m_seq( 0 ),
  m_isFinished( false ),
  m_isOteireButtonDisplayed( false ),
  m_isExitWaitEnableForOteire( true ),
  m_waitCountForOteire( 0 ),
  m_strParam()
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
ClientSeq_WinWild::~ClientSeq_WinWild()
{
}

//-----------------------------------------------------------------------------
/**
 * @brief セットアップ
 * @param setupParam  セットアップパラメータ
 */
//-----------------------------------------------------------------------------
void ClientSeq_WinWild::Setup( const SetupParam& setupParam )
{
  m_mainModule = setupParam.mainModule;
  m_client     = setupParam.client;
  m_pokecon    = setupParam.pokecon;
  m_viewSystem = setupParam.viewSystem;
}

//-----------------------------------------------------------------------------
/**
 * @brief お手入れボタンを表示したことを通知する
 */
//-----------------------------------------------------------------------------
void ClientSeq_WinWild::SetOteireButtonDisplayed( void )
{
  m_isOteireButtonDisplayed = true;
}

//-----------------------------------------------------------------------------
/**
 * @brief お手入れボタンを押すための「待ち」シーケンスを実行するかどうかを更新する
 *
 * @param recievedServerCommand  受信したサーバーコマンド
 */
//-----------------------------------------------------------------------------
void ClientSeq_WinWild::UpdateOteireWaitSequenceExist( ServerCmd recievedServerCommand )
{
  // お手入れボタン表示後に、何らかのメッセージを表示したなら、
  // 野生戦終了時に、お手入れボタンのための待ちシーケンスをスキップする
  if( m_isOteireButtonDisplayed )
  {
    if( ( recievedServerCommand == SC_MSG_SET    ) ||
        ( recievedServerCommand == SC_MSG_SET_SE ) ||
        ( recievedServerCommand == SC_MSG_STD    ) ||
        ( recievedServerCommand == SC_MSG_STD_SE ) )
    {
      m_isExitWaitEnableForOteire = false;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief シーケンスを開始する
 */
//-----------------------------------------------------------------------------
void ClientSeq_WinWild::Start( void )
{
  m_seq = 0;
  m_isFinished = false;
}

//-----------------------------------------------------------------------------
/**
 * @brief シーケンスが終了したか？
 * @retval true   終了した
 * @retval false  終了していない
 */
//-----------------------------------------------------------------------------
bool ClientSeq_WinWild::IsFinished( void ) const
{
  return m_isFinished;
}

//-----------------------------------------------------------------------------
/**
 * @brief シーケンスを更新する
 */
//-----------------------------------------------------------------------------
void ClientSeq_WinWild::Update( void )
{
  if( this->IsFinished() )
  {
    return;
  }


  // お手入れボタンを押すために待つフレーム数
  static const u32 WAIT_FRAME_COUNT_FOR_OTEIRE_BUTTON = 30;


  enum Sequence
  {
    SEQ_MONEY_MESSAGE_START,       // お金取得メッセージ開始
    SEQ_MONEY_MESSAGE_WAIT,        // お金取得メッセージ終了待ち
    SEQ_WIN_VS_NUSI_EFFECT_START,  // ぬし戦勝利演出開始
    SEQ_WIN_VS_NUSI_EFFECT_WAIT,   // ぬし戦勝利演出待ち
    SEQ_WIN_VS_NUSI_MESSAGE_START, // ぬし戦勝利メッセージ開始
    SEQ_WIN_VS_NUSI_MESSAGE_WAIT,  // ぬし戦勝利メッセージ待ち
    SEQ_OTEIRE_WAIT_START,         // お手入れボタンのための「待ち」開始
    SEQ_OTEIRE_WAIT_WAIT,          // お手入れボタンのための「待ち」終了待ち
    SEQ_EXIT,
  };

  switch( m_seq )
  {
  case SEQ_MONEY_MESSAGE_START:
    // シナリオのシン・ム戦では落ちてるお金を拾わない
    if( !m_mainModule->GetSetupStatusFlag(BTL_STATUS_FLAG_LEGEND_EX) )
    {
      u32 bonus = m_mainModule->GetBonusMoney();
      if( bonus )
      {
        m_client->GetBonusMoneyGettingStr( &m_strParam, bonus );
        m_viewSystem->CMD_StartMsg( &m_strParam );
      }
    }
    m_seq = SEQ_MONEY_MESSAGE_WAIT;
    // fall through

  case SEQ_MONEY_MESSAGE_WAIT:
    if( m_viewSystem->CMD_WaitMsg() )
    {
      m_seq = ( m_mainModule->GetSetupStatusFlag( BTL_STATUS_FLAG_VS_NUSI ) ) ? ( SEQ_WIN_VS_NUSI_EFFECT_START ) : ( SEQ_OTEIRE_WAIT_START );
    }
    return;


  case SEQ_WIN_VS_NUSI_EFFECT_START:
    m_mainModule->StartWinBGM();  // 勝利BGM再生開始
    if( this->IsNusiWinEffectEnable() )
    {
      m_viewSystem->CMD_VsNusiWinEffect_Start();
      m_seq = SEQ_WIN_VS_NUSI_EFFECT_WAIT;
    }
    else
    {
      m_seq = SEQ_WIN_VS_NUSI_MESSAGE_START;
    }
    return;

  case SEQ_WIN_VS_NUSI_EFFECT_WAIT:
    if( m_viewSystem->CMD_VsNusiWinEffect_Wait() )
    {
      m_seq = SEQ_WIN_VS_NUSI_MESSAGE_START;
    }
    return;

  case SEQ_WIN_VS_NUSI_MESSAGE_START:
    {
      // 「ぬしの　○○との　しょうぶに　かった！」
      u8 nusiPokeID = PokeID::GetClientBasePokeId( BTL_CLIENT_ENEMY1 );
      BTLV_STRPARAM_Setup( &m_strParam, BTL_STRTYPE_STD, BTL_STRID_STD_WinNusi );
      BTLV_STRPARAM_AddArg( &m_strParam, nusiPokeID );
      m_viewSystem->CMD_StartMsg( &m_strParam );
    }
    m_seq = SEQ_WIN_VS_NUSI_MESSAGE_WAIT;
    // fall through

  case SEQ_WIN_VS_NUSI_MESSAGE_WAIT:
    if( m_viewSystem->CMD_WaitMsg() )
    {
      m_seq = SEQ_OTEIRE_WAIT_START;
    }
    return;


  case SEQ_OTEIRE_WAIT_START:
    if( m_isOteireButtonDisplayed &&
        m_isExitWaitEnableForOteire )
    {
      m_waitCountForOteire = 0;
      m_seq = SEQ_OTEIRE_WAIT_WAIT;
    }
    else
    {
      m_seq = SEQ_EXIT;
    }
    return;

  case SEQ_OTEIRE_WAIT_WAIT:
    ++m_waitCountForOteire;
    if( ( WAIT_FRAME_COUNT_FOR_OTEIRE_BUTTON < m_waitCountForOteire ) ||
        ( ui::CheckKeyCont( BUTTON_A ) ) )
    {
      m_seq = SEQ_EXIT;
    }
    return;


  default:
    GFL_ASSERT(0);
    // fall through

  case SEQ_EXIT:
    m_isFinished = true;
    return;
  }
}

/**
 * @brief ぬし戦勝利演出の再生が可能か？
 * @retval true   再生できる
 * @retval false  再生できない
 */
bool ClientSeq_WinWild::IsNusiWinEffectEnable( void ) const
{
  const BTL_POKEPARAM* poke = m_pokecon->GetFrontPokeDataConst( BTL_POS_1ST_0 );

  if( poke == NULL )
  {
    return false;
  }

  if( poke->IsDead() )
  {
    return false;
  }

  if( poke->IsWazaHide() )
  {
    return false;
  }

  return true;
}
  

GFL_NAMESPACE_END( btl )