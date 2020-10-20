//==============================================================================
/**
 * @file	handicap_battle.cpp
 * @brief	ハンデ戦	計算や定義等
 * @author	ichiraku_katsuhiko
 * @data	2012/08/08, 19:40
 */
// =============================================================================
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_math.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_assert.h>

#include <Battle/include/handicap_battle.h>

GFL_NAMESPACE_BEGIN(btl)

const f32 HandicapBattle::INITIAL_DIFF = 1.0f;			///< ハンデがない場合の初期ハンデ値
const f32 HandicapBattle::VALUE_MAX    = 4.0f;
const f32 HandicapBattle::VALUE_MIN    = -4.0f;


// ハンデ差計算に用いる
typedef struct{
	int		battle;		///< 対戦数				
	f32		change;		///< 変動値
}HANDICAP_CALC;

static const HANDICAP_CALC calc_tbl[ HandicapBattle::CALC_TBL_MAX ] = {
	{ HandicapBattle::BATTLE_MAX,	0.1f },		///< 20戦目以降
	{ 11,	0.2f },		///< 11-19戦目 
	{ 4,	0.3f },		///< 4-10戦目 
	{ 0,	0.5f },		///< 1-3戦目 
};

/*--------------------------------------------------*/
/*--------------------------------------------------*/
/* ハンデ差と参加ポケモンの対応表											*/
/*--------------------------------------------------*/
/*--------------------------------------------------*/
// 参加ポケモン定義
struct JOIN_COUNT{
	f32		difference;		///< ハンデ差
	u8		join_count_1;	///< ハンデ差がプラスの参加ポケモン数
	u8		join_count_2;	///< ハンデ差がマイナスの参加ポケモン数
};

// シングル
static const JOIN_COUNT single_tbl[] = {
	{ 0,		3,	3 },		// ハンデ差：0
	{ 0.5f,	3,	4 },		// ハンデ差：1
	{ 1.0f,	2,	3 },		// ハンデ差：2
	{ 1.5f,	2,	4 },		// ハンデ差：3
	{ 2.0f,	2,	5 },		// ハンデ差：4
	{ 2.5f,	2,	6 },		// ハンデ差：5
  { 3.0f,	1,	4 },		// ハンデ差：6
  { 3.5f,	1,	5 },		// ハンデ差：7
  { 4.0f,	1,	6 },		// ハンデ差：8
};

// ダブル
static const JOIN_COUNT double_tbl[] = {
  { 0,		4,	4 },		// ハンデ差：0
  { 0.5f,	4,	5 },		// ハンデ差：1
  { 1.0f,	3,	4 },		// ハンデ差：2
  { 1.5f,	3,	5 },		// ハンデ差：3
  { 2.0f,	3,	6 },		// ハンデ差：4
  { 2.5f,	2,	4 },		// ハンデ差：5
  { 3.0f,	2,	5 },		// ハンデ差：6
  { 3.5f,	2,	5 },		// ハンデ差：7
  { 4.0f,	2,	6 },		// ハンデ差：7
};

// ローテーション
static const JOIN_COUNT rotation_tbl[] = {
  { 0,		4,	4 },		// ハンデ差：0
  { 0.5f,	4,	5 },		// ハンデ差：1
  { 1.0f,	4,	6 },		// ハンデ差：2
  { 1.5f,	3,	4 },		// ハンデ差：3
  { 2.0f,	3,	4 },		// ハンデ差：4
  { 2.5f,	3,	5 },		// ハンデ差：5
  { 3.0f,	3,	5 },		// ハンデ差：6
  { 3.5f,	3,	5 },		// ハンデ差：7
  { 4.0f,	3,	6 },		// ハンデ差：8
};

// テーブルのテーブル
static const JOIN_COUNT* tbl_list[] = {
	single_tbl,		// BTL_RULE_SINGLE
	double_tbl,		// BTL_RULE_DOUBLE
	rotation_tbl,	// BTL_RULE_ROTATION
};

static const u32 tbl_list_num [] = {
	( sizeof( single_tbl) / sizeof( JOIN_COUNT ) ),
	( sizeof( double_tbl) / sizeof( JOIN_COUNT ) ),
	( sizeof( rotation_tbl) / sizeof( JOIN_COUNT ) ),
};

//--------------------------------------------------------------------------
/**
 *	@brief	1byteの浮動小数点計算（）			
 */
//--------------------------------------------------------------------------
f32 HandicapBattle::S16ToF32( s16 value )
{
	return (f32)( (value) / (f32)(1 << FX16_SHIFT) );
}

//--------------------------------------------------------------------------
/**
 *	@brief	1byteの浮動小数点計算（）			
 */
//--------------------------------------------------------------------------
s16 HandicapBattle::F32ToS16( f32 value )
{
  if( value > VALUE_MAX ){ value = VALUE_MAX; }
  if( value < VALUE_MIN ){ value = VALUE_MIN; }

	if( value > 0.0f ){
		return (s16)( (value) * ( 1 << FX16_SHIFT) + 0.5f );
	}else {
		return (s16)( (value) * ( 1 << FX16_SHIFT) - 0.5f );
	}
}

//--------------------------------------------------------------------------
/**
 *	@brief	ハンデ差計算
 * 
 *  @param	u8	now_battle	 現在のハンデバトル総数
 *  @param	f32	now_handicap 現在のハンデ差
 *  @param	bool	is_win		 勝利した場合true
 * 
 *  @return 変動後のハンデ差
 */
//--------------------------------------------------------------------------
f32	HandicapBattle::CalcPoint( u8 now_battle, f32 now_handicap, bool is_win )
{
	f32 ret = 0.0f;
	for( int i=0; i<HandicapBattle::CALC_TBL_MAX; i++ ){
		// 何戦目か
		if( now_battle >= calc_tbl[i].battle )
		{
			if( is_win ){ 
				ret = now_handicap + calc_tbl[i].change;
				ICHI_PRINT("ハンデ戦勝ちなのでプラス\n");
			}else{
				ret = now_handicap - calc_tbl[i].change;
				ICHI_PRINT("ハンデ戦負けなのでマイナス\n");
			}
			break;
		}
	}

  if( ret > VALUE_MAX ){ ret = VALUE_MAX; }
  if( ret < VALUE_MIN ){ ret = VALUE_MIN; }
	
	ICHI_PRINT("ハンデ差計算 %f -> %f\n", now_handicap, ret );
	return ret;
}

//--------------------------------------------------------------------------
/**
 *	@brief	ハンデ差に応じた参加ポケモン数を返す
 * 
 *  @param	now_handicap	ハンデ差
 *  @param rule						バトルルール
 * 
 *  @param	u8						参加ポケモンの数
 */
//--------------------------------------------------------------------------
u8 HandicapBattle::CalcPokeJoinCount( f32 now_handicap, BtlRule rule )
{
	const JOIN_COUNT *tbl = tbl_list[ rule ];
	const u32 max = tbl_list_num[ rule ];
	u8	ret = 0;		// 戻り値

	ICHI_PRINT("rule %d max %d hand %f \n", rule, max, now_handicap );
	// ハンデ差求める
	for( int i=max-1; i>=0; i-- )
	{
		f32 abs = gfl2::math::Abs( now_handicap );
		ICHI_PRINT("abs %f diff %f \n", abs, tbl[i].difference );
		if( abs >= tbl[i].difference )
		{
			ICHI_PRINT("ハンデ差 %f 参加ポケモン %d %d \n", now_handicap, tbl[i].join_count_1, tbl[i].join_count_2 );
			// 符号チェック
			if( now_handicap >= 0 ){ ret = tbl[i].join_count_1; }
			else									 { ret = tbl[i].join_count_2; }
			break;
		}
	}

	GFL_ASSERT(ret);

	return ret;
}
GFL_NAMESPACE_END(btl)



