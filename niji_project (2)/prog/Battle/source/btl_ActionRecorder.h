//=============================================================================
/**
 * @file   btl_ActionRecorder.h
 * @date   2015/12/24 10:41:25
 * @author obata_toshihiro
 * @brief  ポケモンの行動結果を記録するクラス
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_ACTIONRECORDER_H_INCLUDED
#define BTL_ACTIONRECORDER_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "./btl_const.h"

GFL_NAMESPACE_BEGIN( btl )


/**
 * @brief ポケモンの行動結果を記録するクラス
 */
class ActionRecorder
{
  GFL_FORBID_COPY_AND_ASSIGN( ActionRecorder );

public:

  /**
   * @brief 行動を記録できるターン数
   */
  static const u32 RECORD_TURN_NUM = 2;


  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //---------------------------------------------------------------------------
  ActionRecorder( void );

  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  virtual ~ActionRecorder();


  //---------------------------------------------------------------------------
  /**
   * @brief 行動結果をクリアする
   */
  //---------------------------------------------------------------------------
  void Clear( void );

  //---------------------------------------------------------------------------
  /**
   * @brief ターン開始時の更新処理
   */
  //---------------------------------------------------------------------------
  void StartTurn( void );
    

  /**
   * @brief 行動結果を表すID
   *
   * @note 32個まで拡張可能です
   */
  enum ActionID
  {
    ACTION_WAZA_FAILED_MASTER,          // 技を撃ったが外れた（あらゆるケース）
    ACTION_WAZA_FAILED_HIT_PERCENTAGE,  // 技を撃ったが、命中率の抽選で外れた
    ACTION_WAZA_FAILED_TOKUSEI,         // 技を撃ったが、特性の影響で効果が無かった
    ACTION_WAZA_FAILED_TYPE,            // 技を撃ったが、タイプ相性で効果が無かった
    ACTION_WAZA_FAILED_GUARD_WAZA,      // 技を撃ったが、「まもる」系のワザ効果により防がれた
    ACTION_NUM,
  };

  //---------------------------------------------------------------------------
  /**
   * @brief 行動結果をチェックする
   * @param pokeId        チェック対象ポケモンのID
   * @param backTurnCount 遡るターン数( 0～RECORD_TURN_NUM-1, 0なら現在のターン )
   * @param actionId      チェック対象の行動結果
   * @retval true   指定した行動結果に該当する
   * @retval false　指定した行動結果に該当しない
   */
  //---------------------------------------------------------------------------
  bool CheckAction( u8 pokeId, u8 backTurnCount, ActionID actionId ) const;

  //---------------------------------------------------------------------------
  /**
   * @brief 行動結果を設定する
   * @param pokeId    設定対象ポケモンのID
   * @param actionId  設定対象の行動結果
   */
  //---------------------------------------------------------------------------
  void SetAction( u8 pokeId, ActionID actionId );




private:

  /**
   * @brief ポケモン1体分のデータ
   */
  struct PokeData
  {
    u32 actionFlag;  // ActionID のビットフラグ
  };

  /**
   * @brief １ターン分のデータ
   */
  struct TurnData
  {
    PokeData pokeData[ BTL_POKEID_MAX ];  // 各ポケモンのデータ
  };


  TurnData m_turnData[ RECORD_TURN_NUM ];



private:

  void ClearTurnData( TurnData* turnData ) const;

};


GFL_NAMESPACE_END( btl )

#endif // BTL_ACTIONRECORDER_H_INCLUDED