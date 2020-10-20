//=============================================================================
/**
 * @file    btl_ClientSeq_TrainerMessage.cpp
 * @brief   戦闘クライアントのシーケンス制御( トレーナーメッセージ )
 * @author  obata_toshihiro
 * @date    2015.03.11
 */
//=============================================================================
#include "btlv/btlv_core.h"
#include "btl_mainmodule.h"
#include "btl_TrainerMessage.h"
#include "btl_TrainerMessageManager.h"
#include "btl_ClientSeq_TrainerMessage.h"


GFL_NAMESPACE_BEGIN( btl )


//---------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//---------------------------------------------------------------------------
ClientSeq_TrainerMessage::ClientSeq_TrainerMessage( void ) : 
  m_pMainModule( NULL ),
  m_pViewSystem( NULL ),
  m_pMessageManager( NULL ),
  m_seq( 0 ),
  m_isFinished( true ),
  m_messageId( TrainerMessage::MESSAGE_NUM )
{
}

//---------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//---------------------------------------------------------------------------
ClientSeq_TrainerMessage::~ClientSeq_TrainerMessage()
{
}

//---------------------------------------------------------------------------
/**
 * @brief セットアップ
 * @param setupParam  セットアップパラメータ
 */
//---------------------------------------------------------------------------
void ClientSeq_TrainerMessage::Setup( const SetupParam& setupParam )
{
  m_pMainModule = setupParam.pMainModule;
  m_pViewSystem = setupParam.pViewSystem;
  m_pMessageManager = setupParam.pMessageManager;
}

//---------------------------------------------------------------------------
/**
 * @brief トレーナーメッセージの表示シーケンスを開始する
 * @param trainerId  表示対象クライアントのID
 * @param messageId  表示するメッセージのID
 */
//---------------------------------------------------------------------------
void ClientSeq_TrainerMessage::Start( u8 clientId, btl::TrainerMessage::MessageID messageId )
{
  m_seq = 0;
  m_isFinished = false;
  m_clientId = clientId;
  m_messageId = messageId;
}

//---------------------------------------------------------------------------
/**
 * @brief トレーナーメッセージの表示シーケンスを更新する
 */
//---------------------------------------------------------------------------
void ClientSeq_TrainerMessage::Update( void )
{
  if( m_isFinished ) {
    return;
  }

  switch( m_seq )
  {
  case 0:
    this->StartView();
    ++( m_seq );
    break;

  case 1:
    if( this->WaitView() ) {
      ++( m_seq );
    }
    break;

  case 2:
    m_pMessageManager->Done( m_clientId, m_messageId );
    m_isFinished = true;
    break;
  }
}

/**
 * @brief トレーナーメッセージの表示を開始する
 */
void ClientSeq_TrainerMessage::StartView( void )
{
  if(m_pViewSystem==NULL) return;
  BtlvTrainerPos trainerPos = m_pMainModule->ClientIDtoTrainerViewPos( m_clientId );
  m_pViewSystem->CMD_TrainerIn_Event( trainerPos );

  u32 trainerId = m_pMainModule->GetClientTrainerID( m_clientId );
  trainer::MessageKind messageKind = btl::TrainerMessage::GetTrainerMessageKind( m_messageId );
  m_pViewSystem->CMD_StartMsgTrainer( static_cast<trainer::TrainerID>(trainerId), messageKind, true );
}

/**
 * @brief トレーナーメッセージ表示の終了を待つ
 * @retval true   表示が終了した
 * @retval false  表示が終了していない
 */
bool ClientSeq_TrainerMessage::WaitView( void )
{
  if(m_pViewSystem==NULL) return true;
  bool message = m_pViewSystem->CMD_WaitMsg();
  bool trainer = m_pViewSystem->CMD_WaitTrainerIn();
  return ( message && trainer );
}

//---------------------------------------------------------------------------
/**
 * @brief トレーナーメッセージの表示シーケンスが終了したか？
 * @retval true   終了した
 * @retval false  終了していない
 */
//---------------------------------------------------------------------------
bool ClientSeq_TrainerMessage::IsFinished( void ) const
{
  return m_isFinished;
}



GFL_NAMESPACE_END( btl )