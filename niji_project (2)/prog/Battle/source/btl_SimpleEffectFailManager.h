//======================================================================
/**
 * @file btl_SimpleEffectFailManager.h
 * @date 2016/04/15 21:09:58
 * @author taya
 * @brief カテゴリ：シンプルエフェクトのワザが失敗した時の原因保存＆適切なリアクション表示コマンド生成
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BTL_SIMPLEEFFECTFAILMANAGER_H_INCLUDED__
#define __BTL_SIMPLEEFFECTFAILMANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include  "./btl_Common.h"
#include  "./btl_MainModule.h"
#include  "./btl_PokeParam.h"
#include  "./btl_server_const.h"

GFL_NAMESPACE_BEGIN(btl)

class ServerFlow;

  /**
   * @class SimpleEffectFailManager
   * @brief カテゴリ：シンプルエフェクトのワザが失敗した時の原因保存＆適切なリアクション表示コマンド生成
   */
  class SimpleEffectFailManager
  {
      GFL_FORBID_COPY_AND_ASSIGN(SimpleEffectFailManager);

  public:

    /**
     * @enum  失敗コード
     */
    enum Cause {
      CAUSE_NULL,          ///< 無効値
      CAUSE_SELF,          ///< 自身が発動条件を満たしていない（草タイプでないのに”たがやす”など）
      CAUSE_LIMIT,         ///< 効果がすでに上限または下限に達している
      CAUSE_MIGAWARI,      ///< 効果の対象が「みがわり」を使っている
      CAUSE_OTHER_EFFECTS, ///< とくせい、その他の特殊効果（しろいきりなど）による無効化
    };


    /**
     * @enum  結果コード
     */
    enum Result {
      RESULT_STD,           ///< 標準的なワザ失敗メッセージ「しかしうまく決まらなかった」
      RESULT_NO_EFFECT,     ///< 無効な相手に撃ってしまった「効果がないようだ」
      RESULT_OTHER_EFFECTS, ///< それ以外の複合的な特殊効果によるもの（判定メソッドを再呼び出ししてリアクションコマンド生成を任せる）
    };


    /** @ctor */
    SimpleEffectFailManager();

    /** @dtor */
    ~SimpleEffectFailManager();

    /**
     * @Clear
     * 
     * @brief   失敗理由の格納を開始する。
     @ @detail  以後、End が呼ばれるまでの間のみ、格納を受け付ける。それ以外のタイミングでAddCauseが呼ばれた場合は無視する。
     */
    void Start( void );

    /**
     * @End
     * 
     * @brief   失敗理由の格納を終了する。
     @ @detail  Start～End 以外のタイミングでAddCauseが呼ばれた場合は無視する。
     */
    void End( void );

    /**
     * @AddCause
     * 
     * @brief 失敗理由１件追加
     * @param[in] cause  失敗理由コード
     */
    void AddCause( Cause cause );

    /**
     * @GetResult
     * 
     * @brief   蓄積された失敗コードから妥当な結果コードを返す
     * @return  結果コード
     */
    Result  GetResult( void ) const;

    /**
     * @Clear
     * 
     * @brief 格納されている情報を初期化する
     * @param[in] var1 var1の説明
     * @param[out] var2 var2の説明
     * @param[in,out] var3 var3の説明
     * @return 戻り値の説明
     * @sa 参照すべき関数を書けばリンクが貼れる
     * @detail 詳細な説明
     */


  private:
    /**
     * @enum 定数
     */
    enum {
      //! １回のワザごとに保存する失敗理由の最大数（=ワザ１つに割り当てられる最大の効果種類数）
      NUM_CAUSE_PER_WAZA = 5,
    };

    //! 失敗理由コード
    Cause m_causes[ NUM_CAUSE_PER_WAZA ];

    //! 格納されている失敗理由コードの数
    uint  m_numCause;

    //! 格納受付中フラグ
    bool  m_isAvailable;



    void init( void );
    uint countFailCode( Cause failCode ) const;

  };

GFL_NAMESPACE_END(btl)

#endif // __HEADER_H_INCLUDED__
