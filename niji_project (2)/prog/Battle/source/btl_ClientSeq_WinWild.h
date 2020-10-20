//=============================================================================
/**
 * @file   btl_ClientSeq_WinWild.h
 * @date   2015/08/28 15:52:10
 * @author obata_toshihiro
 * @brief  クライアントのシーケンス( 野生戦勝利 )
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_CLIENTSEQ_WILDWIN_H_INCLUDED
#define BTL_CLIENTSEQ_WILDWIN_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "./btl_common.h"
#include "./btl_server_const.h"
#include "./btlv/btlv_core.h"

GFL_NAMESPACE_BEGIN( btl )

  class MainModule;
  class BTL_CLIENT;
  class POKECON;
  class BattleViewSystem;


  /**
   * @class ClientSeq_WinWild
   * @brief クライアントのシーケンス( 野生戦勝利 )
   */
  class ClientSeq_WinWild
  {
    GFL_FORBID_COPY_AND_ASSIGN( ClientSeq_WinWild );

  public:

    //---------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    //---------------------------------------------------------------------------
    ClientSeq_WinWild( void );
    
    //---------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //---------------------------------------------------------------------------
    virtual ~ClientSeq_WinWild();


    
    //---------------------------------------------------------------------------
    /**
     * @brief セットアップパラメータ
     */
    //---------------------------------------------------------------------------
    struct SetupParam
    {
      MainModule* mainModule;        // 参照する MainModule
      const BTL_CLIENT* client;      // 参照する BTL_CLIENT
      const POKECON* pokecon;        // 参照する POKECON
      BattleViewSystem* viewSystem;  // 参照する BattleViewSystem
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
     * @brief お手入れボタンを表示したことを通知する
     */
    //---------------------------------------------------------------------------
    void SetOteireButtonDisplayed( void );

    //---------------------------------------------------------------------------
    /**
     * @brief お手入れボタンを押すための「待ち」シーケンスを実行するかどうかを更新する
     *
     * @param recievedServerCommand  受信したサーバーコマンド
     */
    //---------------------------------------------------------------------------
    void UpdateOteireWaitSequenceExist( ServerCmd recievedServerCommand );


    
    //---------------------------------------------------------------------------
    /**
     * @brief シーケンスを開始する
     */
    //---------------------------------------------------------------------------
    void Start( void );
    
    //---------------------------------------------------------------------------
    /**
     * @brief シーケンスを更新する
     */
    //---------------------------------------------------------------------------
    void Update( void );
    
    //---------------------------------------------------------------------------
    /**
     * @brief シーケンスが終了したか？
     * @retval true   終了した
     * @retval false  終了していない
     */
    //---------------------------------------------------------------------------
    bool IsFinished( void ) const;
  
  


  private:

    bool IsNusiWinEffectEnable( void ) const;


    MainModule*       m_mainModule;
    const BTL_CLIENT* m_client;
    const POKECON*    m_pokecon;
    BattleViewSystem* m_viewSystem;

    int  m_seq;
    bool m_isFinished;

    bool m_isOteireButtonDisplayed;   // お手入れボタンを表示したか？
    bool m_isExitWaitEnableForOteire; // 戦闘終了時、お手入れボタンを押すための「待ち」を行うか？
    u32  m_waitCountForOteire;        // お手入れボタンのための「待ち」カウンタ

    BTLV_STRPARAM m_strParam;
  };


GFL_NAMESPACE_END( btl )

#endif // BTL_CLIENTSEQ_WILDWIN_H_INCLUDED