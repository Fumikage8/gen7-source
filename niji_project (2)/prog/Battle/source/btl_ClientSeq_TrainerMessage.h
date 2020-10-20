//=============================================================================
/**
 * @file    btl_ClientSeq_TrainerMessage.h
 * @brief   戦闘クライアントのシーケンス制御( トレーナーメッセージ )
 * @author  obata_toshihiro
 * @date    2015.03.11
 */
//=============================================================================
#if !defined( BTL_CLIENTSEQ_TRAINERMESSAGE_H_INCLUDED )
#define BTL_CLIENTSEQ_TRAINERMESSAGE_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <niji_conv_header/trainer/trid_def.h>
#include "btl_const.h"
#include "btl_TrainerMessage.h"


GFL_NAMESPACE_BEGIN( btl )

class MainModule;
class BattleViewSystem;


/**
 * @class ClientSeq_TrainerMessage
 * @brief トレーナーメッセージ表示のシーケンスを制御する
 */
class ClientSeq_TrainerMessage
{
  GFL_FORBID_COPY_AND_ASSIGN( ClientSeq_TrainerMessage );

public:

  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //---------------------------------------------------------------------------
  ClientSeq_TrainerMessage( void );

  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  virtual ~ClientSeq_TrainerMessage();
  
  //---------------------------------------------------------------------------
  /**
   * @brief セットアップパラメータ
   */
  //---------------------------------------------------------------------------
  struct SetupParam
  {
    MainModule* pMainModule;
    BattleViewSystem* pViewSystem;
    TrainerMessageManager* pMessageManager;
  };

  //---------------------------------------------------------------------------
  /**
   * @brief セットアップ
   * @param setupParam  セットアップパラメータ
   */
  //---------------------------------------------------------------------------
  void Setup( const SetupParam& setupParam );

  //---------------------------------------------------------------------------
  /**
   * @brief トレーナーメッセージの表示シーケンスを開始する
   * @param trainerId  表示対象クライアントのID
   * @param messageId  表示するメッセージのID
   */
  //---------------------------------------------------------------------------
  void Start( u8 clientId, btl::TrainerMessage::MessageID messageId );

  //---------------------------------------------------------------------------
  /**
   * @brief トレーナーメッセージの表示シーケンスを更新する
   */
  //---------------------------------------------------------------------------
  void Update( void );

  //---------------------------------------------------------------------------
  /**
   * @brief トレーナーメッセージの表示シーケンスが終了したか？
   * @retval true   終了した
   * @retval false  終了していない
   */
  //---------------------------------------------------------------------------
  bool IsFinished( void ) const;


  
private:
  void StartView( void );
  bool WaitView( void );


private:
  MainModule* m_pMainModule;
  BattleViewSystem* m_pViewSystem;
  TrainerMessageManager* m_pMessageManager;

  int m_seq;
  bool m_isFinished;
  u8 m_clientId;
  btl::TrainerMessage::MessageID m_messageId;
};


GFL_NAMESPACE_END( btl )

#endif  // BTL_CLIENTSEQ_TRAINERMESSAGE_H_INCLUDED