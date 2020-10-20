#if !defined( __BTL_TABLES_H__ )
#define __BTL_TABLES_H__
//=============================================================================================
/**
 * @file  btl_tables.h
 * @brief ポケモンWBで てきとうなサイズのテーブルをVRAM_Hに置いてアクセスしていたもの
 * @author  taya
 *
 * @date  2011.01.25  作成
 */
//=============================================================================================
#pragma once

#include "./btl_common.h"
#include "./btl_SideEff.h"

GFL_NAMESPACE_BEGIN(btl)
namespace tables {


/**
 *  アンコール除外対象のワザか判定
 */
extern bool IsMatchEncoreFail( WazaID waza );

/**
 *  ものまね失敗対象のワザ判定
 */
extern bool IsMatchMonomaneFail( WazaID waza );

/**
 *  プレッシャー対象ワザ判定
 */
extern bool IsPressureEffectiveWaza( WazaID waza );

/**
 * フェイントなどで無効化できるタイプのサイドエフェクトかどうかを判定
 */
bool IsGuardTypeSideEffect( BtlSideEffect eff );

/**
 *  合体ワザ判定
 */
extern bool IsCombiWaza( WazaID waza );


/**
 *  さきどり失敗対象ワザ判定
 */
extern bool IsSakidoriFailWaza( WazaID waza );

/**
 *  「ねごと」で出ないワザ判定
 */
extern bool IsNegotoOmmit( WazaID waza );


/**
 *  「ネコのて」で出ないワザ判定
 */
extern bool IsNekoNoteOmmit( WazaID waza );

/**
 *  「まねっこ」で出ないワザ判定
 */
extern bool IsManekkoOmmit( WazaID waza );

/**
 *  「さいはい」で出ないワザ判定
 */
extern bool IsSaihaiOmmit( WazaID waza );


/**
 *  時間差攻撃ワザ判定
 */
extern bool IsDelayAttackWaza( WazaID waza );

/**
 *  Zワザ判定
 */
extern bool IsZWaza( WazaID waza );
extern bool IsZWaza_General( WazaID waza );
extern bool IsZWaza_Special( WazaID waza );


/**
 *  トレースできないとくせい判定
 */
extern bool CheckTraceFailTokusei( TokuseiNo tokuseiID );

/**
 *  「なりきり」できないとくせい判定
 */
extern bool CheckNarikiriFailTokusei( TokuseiNo tokuseiID );

/**
 *  「レシーバー」できないとくせい判定
 */
extern bool CheckReceiverFailTokusei( TokuseiNo tokuseiID );

/**
 *  「なかまづくり」できないとくせい判定
 */
extern bool CheckNakamaDukuriFailTokusei( TokuseiNo tokuseiID );

/**
 *  「スキルスワップ」できないとくせい判定
 */
extern bool CheckSkillSwapFailTokusei( TokuseiNo tokuseiID );


/**
 *  絶対に書き換わってはいけないとくせい判定
 */
extern bool IsNeverChangeTokusei( TokuseiNo tokuseiID );

/**
 *  アルセウスのプレート判定
 */
extern bool IsMatchAruseusPlate( u16 itemID );

/**
 *  グリプス２のチップ判定
 */
extern bool IsMatchGuripusu2Chip( u16 itemID );

/**
 *  インセクタのカセット判定
 */
extern bool IsMatchInsectaCasette( u16 itemID );


/**
 *  かたやぶりが無効化する対象のとくせい判定
 */
extern bool IsMatchKatayaburiTarget( TokuseiNo tokuseiID );


/**
 *  タイプ書き換え失敗するモンスターナンバー判定
 */
extern bool IsTypeChangeForbidPoke( u16 monsno );


/**
 *  ユーザコンフィグ「わざエフェクト：みない」でスキップするエフェクト指定
 */
extern bool IsEffectDisableType( u16 effectNo );



extern const WazaID* GetYubiFuruOmmitTable( u32* elems );
extern bool IsYubiFuruOmmit( WazaID waza );
extern bool CheckItemCallNoEffect( u16 itemID );
/**
 *  AIClientがアイテム使用する際の優先度を返す（0:最低）
 */
extern u32 GetAIItemPriority( u16 itemID );


/**
 *  ターンチェック処理順アクセス
 */
extern WazaSick  GetTurnCheckWazaSickByOrder( u32 idx );

/**
 *  メンタルハーブ対応の状態異常チェック
 */
extern WazaSick  GetMentalSickID( u32 idx );

/**
 * メンタルハーブで治すことの出来る状態異常かどうか判定
 */
extern bool   IsMentalSickID( WazaSick sickID );


/**
 *  使用する対象ポケモンの選択を行う必要が無いアイテムか判別
 */
extern bool IsNoTargetItem( u16 itemNo );

/**
*  使用する対象ポケモンの選択を行う必要が無いロトポンアイテムか判別
*  //@momiji追加分
*/
extern bool IsRotoponItem( u16 itemID );

/**
 *  「ぼうじん」でガードできる対象のワザチェック
 */
extern bool IsBoujinGuardWaza( WazaID waza);

/**
 *  「がんじょうあご」で威力増加するワザチェック
 */
extern bool IsAgoBoostWaza( WazaID waza );

/**
 *  「ぼうだん」で防ぐことができるワザかチェック
 */
extern bool IsBoudanWaza( WazaID waza );


/**
 *  「おやこあい」が無効なワザかチェック
 */
extern bool IsOyakoaiOmitWaza( WazaID waza );


/**
 * @brief UIにダメージ相性を表示しない技か？
 */
extern bool IsNotDisplayUiAffinity( WazaID waza );

/**
 * @brief とくせい「ちからづく」の対象ワザとするか？
 */
extern bool IsTikarazukuEffeciveWaza( WazaID waza );


/**
 * @brief 配列に特定の値が含まれているか判定
 *
 * @param[in]  value     配列にこの値が含まれているかどうか判定する
 * @param[in]  table     配列
 * @param[in]  numTableElems   配列の要素数
 */
template <typename T> 
bool IsInclude( T value, const T* table, u32 numTableElems )
{
  for(u32 i=0; i<numTableElems; ++i)
  {
    if( table[i] == value ){
      return true;
    }
  }
  return false;
}



#if PM_DEBUG
/**
 *  デバッグ「ゆびをふるテーブル（グローバル）」
 */
extern int g_YubiFuruDebugNumber[ BTL_POS_NUM ];

extern void YubifuruDebugReset( void );
extern void YubifuruDebugSetEnd( void );
extern void YubifuruDebugInc( u8 pos );
extern int  YubifuruDebug_GetNumber( u8 pos );

#endif



}   // end of namespace 'tables'
GFL_NAMESPACE_END(btl)


#endif // __BTL_TABLES_H__
