//==============================================================================
/**
 * @file	handicap_battle.h
 * @brief	ハンデ戦	計算や定義等
 * @author	ichiraku_katsuhiko
 * @data	2012/08/08, 17:22
 */
// =============================================================================

#if !defined( __HANDICAP_BATTLE_H__ )
#define	__HANDICAP_BATTLE_H__

#include <Battle/include/battle_def.h>

GFL_NAMESPACE_BEGIN(btl)

/// ハンデ戦（計算や定義等）
class HandicapBattle{

public:
	static const f32 INITIAL_DIFF;			///< ハンデがない場合の初期ハンデ値
	static const int BATTLE_MAX = 20;					///< ハンデ戦の最大数
	static const int CALC_TBL_MAX = 4;				///< ハンデ差計算用テーブルの要素数MAX

private:
	// 浮動小数点を16bitに収める
	static const int FX16_SHIFT					=	12;
	static const int FX16_INT_MASK			=	0x7000;
	static const int FX16_DEC_MASK			=	0x0fff;
	static const int FX16_SIGN_MASK			=	0x8000;

  /* 16bit浮動小数　*/
	/* 最大値 7.9375 */
	/* 最小値 -8 */
  static const f32 VALUE_MAX;
  static const f32 VALUE_MIN;

public:
	// 1byteの浮動小数点に変換
	static f32 S16ToF32( s16 value );
	static s16 F32ToS16( f32 value );

	/// ハンデ差計算
	static f32	CalcPoint( u8 now_battle, f32 now_handicap, bool is_win ); 

	/// ハンデ差に応じた参加ポケモン数を返す
	static u8	CalcPokeJoinCount( f32 now_handicap, BtlRule rule );
};

GFL_NAMESPACE_END(btl)

#endif	/* __HANDICAP_BATTLE_H__ */

