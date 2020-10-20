//=============================================================================
/**
 * @file   btl_ClientSeq_Capture.cpp
 * @date   2015/08/28 15:52:10
 * @author obata_toshihiro
 * @brief  クライアントのシーケンス( 野生戦捕獲 )
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_ClientSeq_Capture.h"

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
ClientSeq_Capture::ClientSeq_Capture( void ) :
  m_mainModule( NULL ),
  m_client( NULL ),
  m_viewSystem( NULL ),
  m_seq( 0 ),
  m_isFinished( false ),
  m_isOteireButtonDisplayed( false ),
  m_isExitWaitEnableForOteire( true ),
  m_waitCountForOteire( 0 )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
ClientSeq_Capture::~ClientSeq_Capture()
{
}

//-----------------------------------------------------------------------------
/**
 * @brief セットアップ
 * @param setupParam  セットアップパラメータ
 */
//-----------------------------------------------------------------------------
void ClientSeq_Capture::Setup( const SetupParam& setupParam )
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
void ClientSeq_Capture::SetOteireButtonDisplayed( void )
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
void ClientSeq_Capture::UpdateOteireWaitSequenceExist( ServerCmd recievedServerCommand )
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
void ClientSeq_Capture::Start( void )
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
bool ClientSeq_Capture::IsFinished( void ) const
{
  return m_isFinished;
}

//-----------------------------------------------------------------------------
/**
 * @brief シーケンスを更新する
 */
//-----------------------------------------------------------------------------
void ClientSeq_Capture::Update( void )
{
  if( this->IsFinished() )
  {
    return;
  }


  // お手入れボタンを押すために待つフレーム数
  static const u32 WAIT_FRAME_COUNT_FOR_OTEIRE_BUTTON = 30;


  enum Sequence
  {
    SEQ_OTEIRE_WAIT_START,  // お手入れボタンのための「待ち」開始
    SEQ_OTEIRE_WAIT_WAIT,   // お手入れボタンのための「待ち」終了待ち
    SEQ_EXIT,
  };

  switch( m_seq )
  {
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

  

GFL_NAMESPACE_END( btl )