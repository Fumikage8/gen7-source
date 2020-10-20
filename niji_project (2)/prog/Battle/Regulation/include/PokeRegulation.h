//==============================================================================
/**
 * @file  PokeRegulation.h
 * @brief ポケモンのレギュレーション検査用プログラム
 * @author  ichiraku_katsuhiko
 * @data  2015/07/21, 21:21
 *
 * @note  使用方法
 *        1.このクラス(=PokeRegulation)を作成する
 *        2.SetRegulationでレギュレーションを渡す
 *        3.各チェック用関数で検査する
 */
// =============================================================================

#if !defined( __POKE_REGULATION_H__ )
#define __POKE_REGULATION_H__

#include <pml/include/pmlib.h>
#include <macro/include/gfl2_Macros.h>
#include "Battle/Regulation/include/Regulation.h"
#include "Battle/Regulation/include/RegulationScriptChecker.h"

//==========================================================================
//=========================================================================

//GFL_NAMESPACE_BEGIN( Battle )

//! ポケモンのレギュレーション検査
class PokeRegulation
{
  GFL_FORBID_COPY_AND_ASSIGN(PokeRegulation);
public:

  /**
  * @enum PokelistCheckReturn
  * 参加ポケモン選択画面のチェックの検査戻り値
  */
  typedef enum{
    RET_OK = 0,            //! 正常
    RET_NUM_FAILED,        //! ポケモンの数違反
    RET_OVER_LEGEND,       //! 伝説参加上限エラー
    RET_NO_MASTPOKE,       //! 必須なポケモンがいない
    RET_EX_SCRIPT,         //! 拡張レギュレーション違反
  } PokelistCheckReturn;

  /**
   * @struct ViolationDetails
   * レギュレーション違反内容の詳細データ
   */
  struct ViolationDetails
  {
    // 違反した内容のフラグ
    bool isViolationNumLo;     //! 数が足りてない
    bool isViolationNumHi;     //! 数がオーバー
    bool isViolationMustPoke;  //! 必須ポケモンがいない
    bool isViolationVetoWaza;  //! 禁止わざ
    bool isViolationVetoPoke;  //! 禁止ポケモン
    bool isViolationVetoItem;  //! 禁止アイテム
    bool isViolationLevel;     //! レベル違反のポケモンがいる
    bool isViolationBothPoke;  //! 同じポケモン
    bool isViolationBothItem;  //! 同じアイテム
    bool isViolationNumLegend; //! 伝説の数がオーバー
    bool isViolationQR;        //! QRバトルチームは使えない
    bool isViolationExScript;  //! 拡張レギュレーション違反

    // 検査結果詳細データ用
    // (データは詰めて格納している)
    u32  vetoPoke[ pml::PokeParty::MAX_MEMBERS ]; //! 禁止ポケモンのMonsNo
    u16  vetoItem[ pml::PokeParty::MAX_MEMBERS ]; //! 禁止アイテムのItemNo
    u32  vetoWaza[ pml::PokeParty::MAX_MEMBERS ]; //! 禁止技所持のMonsNo
    u32  level[ pml::PokeParty::MAX_MEMBERS ];    //! レベル違反のMonsNo
    u32  bothPoke[ pml::PokeParty::MAX_MEMBERS ]; //! 同じポケモンのMonsNo
    u16  bothItem[ pml::PokeParty::MAX_MEMBERS ]; //! 同じアイテムのItemNo

    // @fix NMCat[1235]：対象のNGポケモンにマークを付けれる用にポケパーティのインデックスでNGが分かるようにする
    // ※タマゴは除外したパーティインデックスになっているので注意
    b8  vetoPokeIndex[ pml::PokeParty::MAX_MEMBERS ]; //! 禁止ポケモンNG
    b8  vetoItemIndex[ pml::PokeParty::MAX_MEMBERS ]; //! 禁止アイテムNG
    b8  vetoWazaIndex[ pml::PokeParty::MAX_MEMBERS ]; //! 禁止ワザNG
    b8  levelIndex[ pml::PokeParty::MAX_MEMBERS ]; //! レベル違反NG
  };

private:

  static const int MONSNO_NONE = -1;    //! モンスター番号なしの定義

  /**
   * @enum 違反フラグのビット
   */
  enum{
    FAILED_BIT_NONE       = 0x0000,   //! エラー無し
    FAILED_BIT_EGG        = 0x0001,   //! タマゴが含まれている
    FAILED_BIT_VETO_WAZA  = 0x0002,   //! 禁止わざ
    FAILED_BIT_VETO_POKE  = 0x0004,   //! 禁止ポケモン
    FAILED_BIT_VETO_ITEM  = 0x0008,   //! 禁止アイテム
    FAILED_BIT_LEVEL      = 0x0010,   //! レベル違反のポケモンがいる
    FAILED_BIT_NUM_LO     = 0x0020,   //! 数が足りてない
    FAILED_BIT_NUM_HI     = 0x0040,   //! 数がオーバー
    FAILED_BIT_MUST_POKE  = 0x0080,   //! 必須ポケモンがいない
    FAILED_BIT_BOTH_POKE  = 0x0100,   //! 同じポケモン
    FAILED_BIT_BOTH_ITEM  = 0x0200,   //! 同じアイテム
    FAILED_BIT_NUM_LEGEND = 0x0400,   //! 伝説の数がオーバー
    FAILED_BIT_QR         = 0x0800,   //! QRバトルチームは使えない
    FAILED_BIT_EX_SCRIPT  = 0x1000,   //! 拡張レギュレーション違反
    FAILED_BIT_UNKNOWN    = 0x8000,   //! 上記以外のエラー
  };


public:
  //! コンストラクタ
  PokeRegulation( gfl2::heap::HeapBase* pHeap );
  //! デストラクタ
  virtual ~PokeRegulation();

public:

  //! 伝説ポケモンかどうか
  static bool CheckLegend( u16 monsno, u8 formno );
  //! 準伝説ポケモンかどうか
  static bool CheckSubLegend( u16 monsno );
  //! 指定したレベルにポケモンパラメータを補正する
  static void MakeLevelRevise( pml::pokepara::PokemonParam *pPokePara, u32 level );
  //! 指定したレベル（以上補正）にポケパーティを補正する
  static void PokePartyLevelRevise( pml::PokeParty *pPokeParty, u32 maxLevel );

  //! nijiとの通信互換用共通関数
  //! niji互換対応ポケモンチェック（=momiji追加なのでnijiに送ってはいけない）
  static bool CheckNijiCompatibleMonsNo( u16 monsNo, u8 formNo );
  static bool CheckNijiCompatible( pml::pokepara::CoreParam *pPokePara );

  /**
   * @brief 検査用のレギュレーションをセット
   * @param pRegulation レギュレーションクラスのポインタ
   */
  void SetRegulation( Regulation *pRegulation );

  /**
   * @brief   検査用のレギュレーションを取得
   * @return  レギュレーションクラスのポインタ
   */
  Regulation* GetRegulation( void ) const { return m_pRegulation; };


  //==========================================================================
  // レギュレーション検査用
  //=========================================================================
  /**
   * @brief バトルチーム用のポケパーティ検査
   * @param[in]   pPokeParty        検査するポケモンパーティ
   * @param[out]  violationDetails  違反内容詳細
   * @retval true:正常
   * @retval false:異常 (違反内容詳細はviolationDetailsに格納されている）
   */
  bool CheckPokePartyForBtlTeam( pml::PokeParty* pPokeParty, ViolationDetails& violationDetails );

  /**
   * @brief 参加選択用のポケパーティ検査
   * @param[in]   pPokeParty   検査するポケモンパーティ
   * @return 参加ポケモン選択画面のチェックの検査戻り値
   */
  PokelistCheckReturn CheckPokePartyForPokelist( pml::PokeParty* pPokeParty );

  /**
   * @brief 参加選択の制限時間なくなった時の参加自動選択
   * @param[in]   pPokeParty   ポケモンパーティ
   * @param[in]   joinPokeIndex  参加ポケモンインデックス配列（要素：順番、値：プレートのインデックス）　-1で参加していない
   * @param[out]  retJoinPokeIndex  自動選択後の参加ポケモンインデックス配列（要素：順番、値：プレートのインデックス）
   */
  void SetJoinOrderTimeOutForPokelist( pml::PokeParty* pPokeParty, const s8* joinPokeIndex, s8* retJoinPokeIndex );

  //==========================================================================
  // レギュレーション補正用
  //=========================================================================
  //! ポケパーティをレギュレーションにそったレベル補正をかける
  void ModifyLevelPokeParty( pml::PokeParty *pPokeParty );

  //! ポケパーティをレギュレーションにそったニックネーム補正をかける
  void ModifyNickName( pml::PokeParty *pPokeParty );

private:
  /**
   * @brief ポケパーティ検査
   * @param[in]   pFullPokeParty      チェックに渡されたフルパーティ
   * @param[in]   pRejectEggPokeParty フルパーティからタマゴを除外したパーティ
   * @param[out]  violationDetails    違反内容詳細
   * @retval true:正常
   * @retval false:異常 (違反内容詳細はviolationDetailsに格納されている）
   */
  bool CheckPokeParty( pml::PokeParty* pFullPokeParty, pml::PokeParty* pRejectEggPokeParty, ViolationDetails& violationDetails );

  //! ポケモンパーティを渡してポケモン単体チェック
  bool CheckPokeParaForParty( pml::PokeParty* pPokeParty, ViolationDetails& violationDetails );

  //! ポケモン単体検査
  int CheckPokePara( pml::pokepara::PokemonParam* pPokePara );

  //! 伝説参加上限チェック
  bool CheckLegendNum( pml::PokeParty* pPokeParty );

  //! 同じポケモンチェック
  bool CheckBothPoke( pml::PokeParty* pPokeParty, ViolationDetails& violationDetails ) const;

  //! 同じアイテムチェック
  bool CheckBothItem( pml::PokeParty* pPokeParty, ViolationDetails& violationDetails ) const;

  //! 必須ポケモンチェック（パーティ）
  bool CheckMustPokeForParty( pml::PokeParty* pPokeParty ) const;

  //! 必須ポケモンチェック(ポケモン単体）
  bool CheckMustPokeForPokeParam( pml::pokepara::PokemonParam* pPokemonParam, u16 mustPoke, u8 mustPokeForm ) const;

  //! 必須ポケモンに含まれているか(ポケモン単体）AndとOrは区別しない
  bool IsContainMustPoke( pml::pokepara::PokemonParam* pPokemonParam ) const;

  //! レベルにレギュレーションにそったレベル補正をかける
  u32 ModifyLevelCalc( u32 level ) const;

  /**
   * @brief ポケパーティを拡張スクリプトで検査
   * @param[in]   pPokeParty        検査するポケモンパーティ
   * @param[out]  violationDetails  違反内容詳細
   * @retval true:正常
   * @retval false:異常 (違反内容詳細はviolationDetailsに格納されている）
   */
  bool CheckPokePartyExScript( pml::PokeParty* pPokeParty, ViolationDetails& violationDetails, RegulationScriptChecker::Mode eMode );

private:
  gfl2::heap::HeapBase*   m_pHeap;         //! レギュレーションバッファ確保用ヒープ
  Regulation*             m_pRegulation;   //! チェック用のレギュレーション

};

//GFL_NAMESPACE_END( Battle )

#endif  /* __POKE_REGULATION_H__ */

