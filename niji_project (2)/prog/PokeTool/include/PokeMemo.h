//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		poke_memo.h
 *	@brief  トレーナーメモ設定クラス
 *	@author	Toru=Nagihashi   -->  k.ohno
 *	@date		2012.05.08
 *
 *	@note   静的クラスです。トレーナーメモなのにpoke_memoという名前なのは
 *	        前からの名残＋ポケモンパラムに対する操作だからです。
 *          渡辺さんがポケメモといっていたためという説があるそうです。
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __POKE_MEMO_H__
#define __POKE_MEMO_H__
#pragma once


#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <str/include/gfl2_Str.h>
#include <pml/include/pmlib.h>
#include <Savedata/include/MyStatus.h>

GFL_NAMESPACE_BEGIN(PokeTool)

class PokeMemo
{
  GFL_FORBID_COPY_AND_ASSIGN(PokeMemo);
  PokeMemo(){} //生成も禁止

public:
  //----------------------------------------------------------------------------
  /**
   *	@brief  ポケモンへのメモ設定
   *	        以下、状況によってSetFrom~関数でポケパラに設定してください。
   *	        状況は以下
   *	        ・ゲーム内で捕獲した    SetFromCapture
   *	        ・ゲーム内交換した      SetFromInnerTrade
   *	        ・人と交換した          SetFromOuterTrade(何もしなくてもよいが明示的に)
   *	        ・過去作を持ってきた    SetFromPastTranslate（なにもしない）
   *          ・配布ポケモンを貰った  SetFromDistribution（ふしぎな贈り物のみ）
   *	        ・タマゴから孵化した    SetFromEggBirth
   *	        ・自分が親のタマゴを貰った  SetFromEggTaken
   *	        ・３DSDLAPPから貰った   SetFrom3DSDLApp
   */
  //-----------------------------------------------------------------------------
  /**
   *  @brief 捕獲で捕まえた場合のポケメモを設定
   *  @param  param           ポケモンパラム
   *  @param  mystatus        マイステータス
   *  @param  place_no        捕まえた場所の地名メッセージ番号
   *  @param  temporary_heap  テンポラリ用ヒープ
   */
  static void SetFromCapture( pml::pokepara::CoreParam * param, const Savedata::MyStatus *mystatus, u32 place_no, gfl2::heap::HeapBase *temporary_heap );

  /**
   *  @brief ゲーム内交換した場合のポケメモを設定
   *  @param  param           ポケモンパラム
   */
  static void SetFromInnerTrade( pml::pokepara::CoreParam * param );

  /**
   *  @brief 人と交換した場合のポケメモを設定
   *  @param  param           ポケモンパラム
   *
   *  @note 何もしません
   */
  static void SetFromOuterTrade( pml::pokepara::CoreParam * param );

  /**
   *  @brief タマゴから孵化した場合のポケメモを設定
   *  @param  param           ポケモンパラム
   *  @param  mystatus        マイステータス
   *  @param  place_no        捕まえた場所の地名メッセージ番号
   *  @param  temporary_heap  テンポラリ用ヒープ
   */
  static void SetFromEggBirth( pml::pokepara::CoreParam * param, const Savedata::MyStatus *mystatus, u32 place_no, gfl2::heap::HeapBase *temporary_heap );

  /**
   *  @brief 過去作から持ってきた場合のポケメモを設定
   *  @param  param           ポケモンパラム
   */
  static void SetFromPastTranslate( pml::pokepara::CoreParam * param );

  /**
   *  @brief 配布で取得した場合のポケメモを設定 ->version、mystatus、EventFalgは外で設定してください。MEMORIES_CAPTURED_LEVELも外で設定して下さい。
   *  @param  param           ポケモンパラム
   *  @param  place_no        場所（ふしぎな贈り物等に入っている場所）
   *  @param  year            年（ふしぎな贈り物等に入っている年）
   *  @param  month           月（ふしぎな贈り物等に入っている月）
   *  @param  day             日（ふしぎな贈り物等に入っている日）
   */
  static void SetFromDistribution( pml::pokepara::CoreParam * param, u32 place_no, u32 year, u32 month, u32 day );

  /**
   *  @brief 自分が親のタマゴを貰った場合のポケメモを設定(育て屋、ゲーム内イベント)
   *  @param  param           ポケモンパラム
   *  @param  mystatus        マイステータス
   *  @param  place_no        捕まえた場所の地名メッセージ番号
   *  @param  temporary_heap  テンポラリ用ヒープ
   */
  static void SetFromEggTaken( pml::pokepara::CoreParam * param, const Savedata::MyStatus *mystatus, u32 place_no, gfl2::heap::HeapBase *temporary_heap );
  
  /**
   * @brief 3DSDLAppにて貰った場合のポケメモを設定 ->version、mystatusは外で設定してください。MEMORIES_CAPTURED_LEVELも外で設定して下さい。
   *  @param  param           ポケモンパラム
   *  @param  place_no        場所（配布データに入っている場所）
   *  @param  year            年（配布データに入っている年）
   *  @param  month           月（配布データに入っている月）
   *  @param  day             日（配布データに入っている日）
   */
  static void SetFrom3dsDlApp( pml::pokepara::CoreParam * param, u32 year, u32 month, u32 day );
  
private :
 
  //タマゴ初回取得  DATA_TYPE_EGG_TAKENにいれる
  //タマゴ通信交換  DATA_TYPE_FIRST_CONTACTにいれる
  //タマゴ孵化      DATA_TYPE_FIRST_CONTACTをDATA_TYPE_EGG_TAKENにいれ、CONTACTにいれる
  //ポケモン取得    DATA_TYPE_FIRST_CONTACTにいれる

  enum DataType
  {
    DATA_TYPE_EGG_TAKEN,      ///<場所１ 時間１ タマゴを貰った～
    DATA_TYPE_FIRST_CONTACT,  ///<場所２ 時間２ 孵化or捕獲した～
  };
  static void SetPlaceTime( pml::pokepara::CoreParam * param, u32 place_no, DataType type );
  static void SetPlaceTime( pml::pokepara::CoreParam * param, u32 place_no, u32 year, u32 month, u32 day, DataType type );
  static void ClearPlaceTime( pml::pokepara::CoreParam * param, DataType type );
  static void CopyPlaceTime( pml::pokepara::CoreParam * param );
  static void SetMyStatus( pml::pokepara::CoreParam * param, const Savedata::MyStatus *mystatus, gfl2::heap::HeapBase *temporary_heap );
  static void SetGetLevel( pml::pokepara::CoreParam * param );
  static void SetVersion( pml::pokepara::CoreParam * param );
//  static void SetVersionPlace( pml::pokepara::CoreParam * param );
};



GFL_NAMESPACE_END(PokeTool)

#endif		// __POKE_MEMO_H__
