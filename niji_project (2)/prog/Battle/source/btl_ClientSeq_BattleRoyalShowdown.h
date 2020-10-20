//=============================================================================
/**
 * @file    btl_ClientSeq_BattleRoyalShowdown.h
 * @brief   バトルロイヤル決着時の演出
 * @author  obata_toshihiro
 * @date    2015.03.11
 */
//=============================================================================
#if !defined( BTL_CLIENTSEQ_BATTLEROYALSHOWDOWN_H_INCLUDED )
#define BTL_CLIENTSEQ_BATTLEROYALSHOWDOWN_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "./btl_common.h"


GFL_NAMESPACE_BEGIN( btl )

class MainModule;
class BattleViewSystem;


/**
 * @class ClientSeq_BattleRoyalShowdown
 * @brief バトルロイヤル決着時の演出シーケンスを制御する
 */
class ClientSeq_BattleRoyalShowdown
{
  GFL_FORBID_COPY_AND_ASSIGN( ClientSeq_BattleRoyalShowdown );

public:

  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //---------------------------------------------------------------------------
  ClientSeq_BattleRoyalShowdown( void );

  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  virtual ~ClientSeq_BattleRoyalShowdown();

  //---------------------------------------------------------------------------
  /**
    * @brief セットアップパラメータ
    */
  //---------------------------------------------------------------------------
  struct SetupParam
  {
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

  BattleViewSystem* m_viewSystem;

  int  m_seq;
  bool m_isFinished;
};


GFL_NAMESPACE_END( btl )

#endif  // BTL_CLIENTSEQ_BATTLEROYALSHOWDOWN_H_INCLUDED