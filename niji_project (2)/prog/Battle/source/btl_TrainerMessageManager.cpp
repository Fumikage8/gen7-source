//=============================================================================
/**
 * @file    btl_TrainerMessageManager.h
 * @brief   戦闘中に表示するトレーナーメッセージの管理
 * @author  obata_toshihiro
 * @date    2015.03.11
 */
//=============================================================================
#include "btl_mainmodule.h"
#include "btl_TrainerMessage.h"
#include "btl_TrainerMessageManager.h"


GFL_NAMESPACE_BEGIN( btl )


//---------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param pMainModule  参照するメインモジュール
 */
//---------------------------------------------------------------------------
TrainerMessageManager::TrainerMessageManager( const MainModule* pMainModule ) :
  m_pMainModule( pMainModule )
{
  this->ClearClientData();
}

/**
 * @brief クライアントごとのデータをクリアする
 */
void TrainerMessageManager::ClearClientData( void )
{
  for( uint clientId=0; clientId<BTL_CLIENT_MAX; ++clientId )
  {
    for( uint i=0; i<TrainerMessage::MESSAGE_NUM; ++i ) {
      m_clientData[ clientId ].isDone[i] = false;
    }
  }
}

//---------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//---------------------------------------------------------------------------
TrainerMessageManager::~TrainerMessageManager()
{
}

//---------------------------------------------------------------------------
/**
 * @brief まだ表示していないメッセージが存在するか？
 * @param pTemporaryHeap  一時的に使用するヒープ
 * @param clientID        チェック対象のクライアントID
 * @param messageID       チェック対象のメッセージID
 * @retval true   表示していないメッセージが存在する
 * @retval false  表示していないメッセージが存在しない
 */
//---------------------------------------------------------------------------
bool TrainerMessageManager::IsMessageExist( 
  gfl2::heap::HeapBase* pTemporaryHeap, 
  u8 clientID, 
  TrainerMessage::MessageID messageID ) const
{
  if( ( BTL_CLIENT_MAX <= clientID ) ||
      ( TrainerMessage::MESSAGE_NUM <= messageID ) ) {
    GFL_ASSERT(0);
    return false;
  }

  if( m_clientData[ clientID ].isDone[ messageID ] ) {
    return false;
  }

  u32 trainerID = m_pMainModule->GetClientTrainerID( clientID );
  return TrainerMessage::IsExist( pTemporaryHeap, static_cast<trainer::TrainerID>(trainerID), messageID );
}

//---------------------------------------------------------------------------
/**
 * @brief メッセージが表示済みであることを記録する
 * @param clientID   対象クライアントのID
 * @param messageID  対象メッセージのID
 */
//---------------------------------------------------------------------------
void TrainerMessageManager::Done( u8 clientID, TrainerMessage::MessageID messageID )
{
  if( ( BTL_CLIENT_MAX <= clientID ) ||
      ( TrainerMessage::MESSAGE_NUM <= messageID ) ) {
    GFL_ASSERT(0);
    return;
  }
  m_clientData[ clientID ].isDone[ messageID ] = true;
}


GFL_NAMESPACE_END( btl )