//============================================================================================
/**
 * @file  RotomPowerManager.h
 * @brief	ロトムパワー効果制御の管理
 * @date	2017.1.24
 * @author	munakata_kai
 *
 */
//============================================================================================
#if !defined( ROTOMPOWER_MANAGER_H_INCLUDED )
#define ROTOMPOWER_MANAGER_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(RotomPower)

class RotomPowerTime;

// 制限時間のあるロトムパワー一覧
typedef enum{
  // ※ＩＴＥＭ 番号と合わせている
  ROTOM_POWER_ID_NULL = 0,  //0 : ロトムパワーが発動していない
  ROTOM_POWER_ID_EGG_INC,   //1 : タマゴの孵化までの歩数ポイントが1.5倍になります。
  ROTOM_POWER_ID_SALE,      //2 : ポケモンセンターの商品が50%引きで販売されます。
  ROTOM_POWER_ID_MONEY,     //3 : バトル後のおこづかいが3倍になります。
  ROTOM_POWER_ID_EXP_INC,   //4 : バトル後の経験値が2倍になります。
  ROTOM_POWER_ID_NATSUKI,   //5 : なつき度が上昇するときに＋3されます。
  ROTOM_POWER_ID_SOUGUU_INC,//6 : エンカウント率が1.5倍になります。
  ROTOM_POWER_ID_SOUGUU_DEC,//7 : エンカウント率が0.33倍になります。

  ROTOM_POWER_ID_MAX,		//8 : ロトムパワー最大数
}ROTOM_POWER_ID;

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class RotomPowerManager
{
  GFL_FORBID_COPY_AND_ASSIGN(RotomPowerManager); ///< コンストラクタ + 代入禁止
private:
  // GetSystemCurrent()をコールする間隔
  // ※上記関数は負荷が大きい
  static const int  TICK_TIME_INTERVAL = 20;
  // ロトム効果定数定義
  static const u32  POWER_EGG_INC    = 150;   // (add_count * POWER_EGG_INC) / 100);
  static const u32  POWER_SALE       = 50;    // (price * POWER_SALE) / 100;
  static const u32  POWER_MONEY      = 3;     // money * POWER_MONEY;
  static const u32  POWER_EXP_INC    = 150;   // (exp * POWER_EXP_INC) / 100;
  static const s32  POWER_NATSUKI    = 3;     // natsuki + POWER_NATSUKI;
  static const u32  POWER_SOUGUU_INC = 80;    // encount * POWER_SOUGUU_INC;
  static const u32  POWER_SOUGUU_DEC = 15;    // (encount * POWER_SOUGUU_DEC) / 100;

  static const u32  POWER_EGG_INC_TIME = 30;  // たまごふかポン効果時間(分)
  static const u32  POWER_SOUGUU_DEC_TIME = 3;// かくれポン効果時間(分)
  static const u32  POWER_TIME = 10;          // 汎用ロトムパワー効果時間(分)
private:
  bool m_isStartFlag; // 計測開始フラグ
  s64  m_lastTick;    // 前回計測時のチック数
  s64  m_passedSec;   // 経過チック数
  s64  m_startTick;   // 計測開始時のチック数
  u32  m_interval;    // 計測周期カウント

  RotomPowerTime* m_pRotomPowerTime;    // 効果時間計測クラス
  ROTOM_POWER_ID m_rotomPowerID;        // 効果種類
  ROTOM_POWER_ID m_endRotomPowerID;     // 終了用効果種類
#if PM_DEBUG
  s32 m_rotomPowerTime;
#endif

public:
  RotomPowerManager(gfl2::heap::HeapBase * pHeap);
  virtual ~RotomPowerManager();

  //------------------------------------------------------------------
  /**
  * @brief ロトムパワーセット（発動はしない）
  */
  //------------------------------------------------------------------
  void SetRotomPower(const ROTOM_POWER_ID powerId);
  //------------------------------------------------------------------
  /**
  * @brief ロトムパワークリア
  */
  //------------------------------------------------------------------
  void ClearRotomPower();

  //------------------------------------------------------------------
  /**
   * @brief ロトムパワー発動（カウント開始命令）効果エフェクト後に使用
   */
  //------------------------------------------------------------------
  void Start();

  //------------------------------------------------------------------
  /**
   * @brief ロトムパワー効果時間更新（メインループ内で毎回呼ぶ）
   */
  //------------------------------------------------------------------
  void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief スリープ復帰処理（スリープしていた時間はカウントしない）
   */
  //------------------------------------------------------------------
  void RestartFromSleep( void );

  //------------------------------------------------------------------
  /**
  * @brief ロトムパワータイムクラス取得
  */
  //------------------------------------------------------------------
  inline RotomPowerTime* GetRotomPowerTime() const { return m_pRotomPowerTime; }

  //------------------------------------------------------------------
  /**
  * @brief EvTimeにおいて分更新処理（効果終了処理）
  */
  //------------------------------------------------------------------
  void UpdateMinuteEvent();

  //------------------------------------------------------------------
  /**
  * @brief ロトムパワーが発動しているか（時間が０でも発動している場合あり）
  */
  //------------------------------------------------------------------
  bool IsRotomPower() const;

  //------------------------------------------------------------------
  /**
  * @brief 今のロトムパワー状態取得（発動していない場合もある）
  */
  //------------------------------------------------------------------
  inline ROTOM_POWER_ID GetRotomPowerID() const { return m_rotomPowerID; }

  //------------------------------------------------------------------
  /**
  * @brief 効果終了時のロトムパワー状態取得
  */
  //------------------------------------------------------------------
  inline ROTOM_POWER_ID GetEndRotomPowerID() const { return m_endRotomPowerID; }

  //------------------------------------------------------------------
  /**
  * @brief 効果終了時のロトムパワークリア
  */
  //------------------------------------------------------------------
  inline void ClearEndRotomPowerID() { m_endRotomPowerID = ROTOM_POWER_ID_NULL; }

  //================================================
  // 各種影響の計算
  //================================================
  //--------------------------------------------------------------------------
  /**
  * @brief パワー計算：タマゴ孵化
  * @param add_count   加算する値（下位8ビット少数）
  * @return u32 パワー影響後の加算値（下位8ビット少数）
  */
  //--------------------------------------------------------------------------
  u32 CalcHatch(const u32 add_count);             // タマゴ孵化
  
  //--------------------------------------------------------------------------
  /**
  * @brief パワー計算：商品の値段
  * @param price     商品の値段
  * @return u32 パワー影響後の商品の値段
  */
  //--------------------------------------------------------------------------
  u32 CalcSale(const u32 price);                    // 商品の値段

  //--------------------------------------------------------------------------
  /**
  * @brief パワー計算：獲得おこづかい
  * @param money   所持道具効果などの補正後のお小遣い
  * @return u32 パワー影響後のおこづかい
  */
  //--------------------------------------------------------------------------
  u32 CalcMoney(const u32 money);                 // おこづかい

  /**
  * @brief パワー計算：獲得経験値
  * @param exp   他人の、外国の、学習装置分配、の補正後の獲得経験値
  * @return u32 パワー影響後の獲得経験値
  */
  //--------------------------------------------------------------------------
  u32 CalcExp(const u32 exp);                     // けいけんち

  //--------------------------------------------------------------------------
  /**
  * @brief パワー計算：なつきどへの加算値
  * @param natsuki   道具などの補正が入った後のなつき度加算値
  * @return u32 パワー影響後のなつき度加算値
  */
  //--------------------------------------------------------------------------
  s32 CalcNatsuki(const s32 natsuki);             // なつき度

  //--------------------------------------------------------------------------
  /**
  * @brief パワー計算：エンカウント率
  * @param encount   現在のエンカウント率（上限100%）
  * @return u32 パワー影響後のエンカウント率
  */
  //--------------------------------------------------------------------------
  u32 CalcEncount(const u32 encount);             // エンカウント率

  //--------------------------------------------------------------------------
  /**
  * @brief レベル計算：特性「やるき」の絶対発動パターン
  * @param level    レベル
  * @param maxLevel 最大レベル
  * @return u32 パワー影響後のエンカウント率
  */
  //--------------------------------------------------------------------------
  int CalcLevel(const int level, const int maxLevel);

  //--------------------------------------------------------------------------
  /**
  * @brief エンカウント回避判定
  * @return bool 回避できるか（true:回避 false:回避不可）
  */
  //--------------------------------------------------------------------------
  bool IsEncountAvoid();

#if PM_DEBUG
  //--------------------------------------------------------------------------
  /**
  * @brief デバッグ用ロトム効果時間設定
  * @param time   ロトム効果時間
  */
  //--------------------------------------------------------------------------
  void SetDebugRotomPowerTime(const s32 time);
  s32 GetDebugRotomPowerTime() const { return m_rotomPowerTime; }
#endif

private:
  //------------------------------------------------------------------
  /**
   * @brief システムチックを秒数に変換
   *
   * @param tickTime システムチック
   *
   * @return 秒数
   */
  //------------------------------------------------------------------
  inline s64 convertTicksToSeconds( const s64 tickTime ) const;

  //--------------------------------------------------------------------------
  /**
  * @brief 余剰時間計算
  * @param s32 現在時刻
  * @return s32 余剰時間
  */
  //--------------------------------------------------------------------------
  s32 getCorrectionTime(const s32 sec);
};

GFL_NAMESPACE_END(RotomPower)
GFL_NAMESPACE_END(Field)

#endif	// ROTOMPOWER_MANAGER_H_INCLUDED
