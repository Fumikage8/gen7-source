//=============================================================================
/**
 * @file    btl_TrainerMessageManager.h
 * @brief   戦闘中に表示するトレーナーメッセージの管理
 * @author  obata_toshihiro
 * @date    2015.03.10
 */
//=============================================================================
#if !defined( BTL_TRAINER_MESSAGE_MANAGER_H_INCLUDED )
#define BTL_TRAINER_MESSAGE_MANAGER_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_Str.h>
#include "btl_const.h"
#include "btl_TrainerMessage.h"


GFL_NAMESPACE_BEGIN( btl )

class MainModule;


/**
 * @class TrainerMessageManager
 * @brief トレーナーメッセージに関するデータを管理する
 */
class TrainerMessageManager
{
  GFL_FORBID_COPY_AND_ASSIGN( TrainerMessageManager );

public:

  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   * @param pMainModule  参照するメインモジュール
   */
  //---------------------------------------------------------------------------
  TrainerMessageManager( const MainModule* pMainModule );

  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  virtual ~TrainerMessageManager();

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
  bool IsMessageExist( 
    gfl2::heap::HeapBase* pTemporaryHeap, 
    u8 clientID, 
    TrainerMessage::MessageID messageID ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief メッセージが表示済みであることを記録する
   * @param clientID   対象クライアントのID
   * @param messageID  対象メッセージのID
   */
  //---------------------------------------------------------------------------
  void Done( u8 clientID, TrainerMessage::MessageID messageID );



private:
  void ClearClientData( void );


private:

  /**
   * @brief クライアントごとのデータ
   */
  struct ClientData
  {
    bool isDone[ TrainerMessage::MESSAGE_NUM ];  // すでに表示済みか？
  };

  const MainModule* m_pMainModule;
  ClientData m_clientData[ BTL_CLIENT_MAX ];
};


GFL_NAMESPACE_END( btl )

#endif  // BTL_TRAINER_MESSAGE_MANAGER_H_INCLUDED