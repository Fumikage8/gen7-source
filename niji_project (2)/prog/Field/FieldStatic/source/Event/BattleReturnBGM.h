//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file BattleReturnBGM.h
 *  @brief バトル終了処理イベント内でのBGM管理
 *  @author saita_kazuki
 *  @date 2016.03.24
 */
//=============================================================================

#if !defined __FIELD_BATTLE_RETURN_BGM_H_INCLUDED__
#define __FIELD_BATTLE_RETURN_BGM_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN( Field )

/**
 * @class BattleReturnBGM
 * @brief バトル終了処理イベント内でのBGM管理
 */
class BattleReturnBGM
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleReturnBGM ); // コピーコンストラクタ＋代入禁止

public:

  /**
   * @brief 設定
   */
  struct Description
  {
    u32     battleWinBGM;   ///< バトル勝利BGM
  };

  /**
   * @brief BGMシーケンス
   */
  enum Seq
  {
    SEQ_BATTLE_WIN,   ///< バトル勝利BGM継続中
    SEQ_EVOLVE,       ///< 進化画面
    SEQ_ZUKAN,        ///< 図鑑画面
    SEQ_OTEIRE,       ///< お手入れ

    SEQ_MAX
  };

  /**
   * @brief BGM変更タイプ
   */
  enum ChangeType
  {
    CHANGE_TYPE_NONE,       ///< 変更しない
    CHANGE_TYPE_NORMAL,     ///< 通常
    CHANGE_TYPE_SILENCE,    ///< 無音
  };


public:
  static u32 GetWinBGM( u32 battleWinBGM );

public:

  /**
   * @brief コンストラクタ
   */
  BattleReturnBGM( const Description& desc );

  /**
   * @brief デストラクタ
   */
  virtual ~BattleReturnBGM();

  /**
   * @brief BGMを変更する
   * @param next 次のBGMシーケンス
   * @note シーケンスによっては変更しない場合もある
   */
  void ChangeBGMReq( Seq next );

private:

  /**
   * @brief BGM変更パターン
   */
  struct BGMPattern
  {
    Seq         now;      ///< 現在のシーケンス
    Seq         next;     ///< 次のシーケンス
    ChangeType  type;     ///< 変更タイプ
  };

private:

  ChangeType GetChangeType( Seq next ) const ;

  void ChangeBGMReq_Normal( Seq next, u32 battleWinBGM );
  void ChangeBGMReq_Silence( Seq next );
  void ChangeBGMReq( Seq next, u32 bgm );

private:

  Description m_desc;           ///< 設定
  Seq         m_seq;            ///< 現在のシーケンス

};

GFL_NAMESPACE_END( Field )

#endif // __FIELD_BATTLE_RETURN_BGM_H_INCLUDED__

