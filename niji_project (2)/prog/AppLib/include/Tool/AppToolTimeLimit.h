#ifndef NIJI_PROJECT_APP_TOOL_TIMELIMIT_H_INCLUDED
#define NIJI_PROJECT_APP_TOOL_TIMELIMIT_H_INCLUDED
//==============================================================================
/**
 * @file    AppToolTimeLimit.h
 * @author  fukushima_yuya
 * @date    2015.03.05
 * @brief   制限時間チェック
 *          sango_project app_pokelist_TimeLimit
 * @caution  このクラスはスリープしている最中に時間が進みます。SystemEventListener未対応
 */
//==============================================================================

// niji
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Tool )

//------------------------------------------------------------------------------
// @brief   制限時間チェック
//------------------------------------------------------------------------------
class TimeLimit
{
public:
  //  取り扱う時間の単位
  enum UnitType
  {
    UNIT_TYPE_SEC  = 0, //  秒単
    UNIT_TYPE_MILL_SEC, //  ミリ秒
  };

  TimeLimit() { _Clear(); }

  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @brief[in] limit_time: チェックする制限時間値
   * @brief[in] unit_type:  limit_timeで設定した時間の単位
   * @note  このコンストラクタを使った場合は Setup()関数を呼ばなくてもよい
   */
  //------------------------------------------------------------------
  TimeLimit( u64 limit_time, const UnitType unit_type = UNIT_TYPE_SEC );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~TimeLimit( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   クラスを動かすのに必要なパラメータ設定
   * @brief[in] limit_time: チェックする制限時間値
   * @brief[in] unit_type:  limit_timeで設定した時間の単位
   */
  //------------------------------------------------------------------
  void Setup(  u64 limit_time, const UnitType unit_type = UNIT_TYPE_SEC );

  //------------------------------------------------------------------
  /**
   * @brief   更新内容をリセット
   */
  //------------------------------------------------------------------
  void Reset( void )
  {
    m_startTick = 0;
    m_time      = 0;
    m_bInit     = true;
  }

  //------------------------------------------------------------------
  /**
   * @brief   アップデート
   * @return  trueなら制限時間超過
   * @return  falseならそれ以外
   */
  //------------------------------------------------------------------
  bool Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   進行の値取得
   * @return  進行値( 0.0f ～ 1.0f )
   * @note
   *    f32以上の時間設定をした場合だと正しくとれない。
   *    小数点の精度が低い
   */
  //------------------------------------------------------------------
  f32 GetProgression();

  //------------------------------------------------------------------
  /**
   * @brief   残り時間の取得
   * @return  残り時間(秒)
   */
  //------------------------------------------------------------------
  u32 GetLimitSecond( void );

  //------------------------------------------------------------------
  /**
   * @brief   残り時間の取得
   * @return  残り時間(単位はミクロ秒)
   */
  //------------------------------------------------------------------
  u64 GetLimitTime( void );

private:
  void _Clear()
  {
    m_startTick = 0;
    m_limitSec  = 0;
    m_time      = 0;
    m_bInit     = false;
  }

  void _Update();

  u64       m_startTick;    //!< 開始時のチック値
  u64       m_limitSec;     //!< 制限時間(ミクロ秒)
  u64       m_time;         //!< 経過時間(ミクロ秒)

  bool      m_bInit;
  static const u64 ms_unitSec;
  static const u64 ms_unitMillSec;
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // #ifndef NIJI_PROJECT_APP_TOOL_TIMELIMIT_H_INCLUDED
