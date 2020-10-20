//==============================================================================
/**
 * @file	JoinFestaFacilitiesUtil.h
 * @brief	ジョインフェスタ施設データ
 * @author	ichiraku_katsuhiko
 * @date	2015/10/22　12:52
 */
// =============================================================================
#ifndef __JOIN_FESTA_FACILITIES_UTIL_H__
#define __JOIN_FESTA_FACILITIES_UTIL_H__
#pragma once

// ライブラリ
#include <macro/include/gfl2_macros.h>
#include <str/include/gfl2_StrBuf.h>
#include "System/include/GflUse.h"

// JoinFestaScript
#include "niji_reference_files/script/JoinFestaPawnDefine.h"
// JoinFesta
#include "NetApp/JoinFesta/include/JoinFestaDefine.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// 前方宣言
namespace NetApp{
  namespace JoinFesta{
    struct JoinFestaDataTable;
  }
}

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFesta)

class JoinFestaFacilitiesUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(JoinFestaFacilitiesUtil);
public:

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------

  //! 店員のキャラクター種類最大数
  static const int CHARACTER_TYPE_MAX = 12;

public:
  // コンストラクタ
  JoinFestaFacilitiesUtil(){};
  // デストラクタ
  virtual ~JoinFestaFacilitiesUtil(){}


  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
public:
  //=============================================================================

  //! キャラクターIDから店員のキャラモデルIDを取得
  static u32 GetCharaModelIdFromCharacterId( const u32 characterId );

  //! キャラクターIDからトレーナータイプを取得
  static u32 GetTrTypeFromCharacterId( const u32 characterId );

  //! キャラクターIDから性別を取得
  static u8 GetCharaSexFromCharacterId( const u32 characterId );

  //! 施設抽選（ご褒美イベント）
  static u32 LotteryFacilitiesId( gfl2::math::Random* pRandom, const NetApp::JoinFesta::JoinFestaDataTable* pjfDataTable );

  //! 施設カラーをランダムで取得
  static JoinFestaScript::FacilitiesColor GetRandomColor( gfl2::math::Random* pRandom, const JoinFestaScript::FacilitiesType facilitiesType, const u32 facilitiesId );

  //! 染物屋から施設カラー定義に変換
  static JoinFestaScript::FacilitiesColor ConvertDyeingColor( const u32 facilitiesId );

  //! こうかんじょの残り回数値を取得
  static u8 GetTradeRemainCountMax( const u32 facilitiesId );

  //! フェスランクからランクアップに必要なコイン取得
  static u32 GetNextRankFesCoin( const u16 rank );

private:
  static const u32 CHARAMODEL_ID_TABLE[CHARACTER_TYPE_MAX];    //! 店員のキャラクターIDテーブル
  static const u32 TRTYPE_TBL[CHARACTER_TYPE_MAX];          //! 店員のトレーナータイプテーブル
  static const u8 CHARA_SEX_TBL[CHARACTER_TYPE_MAX];          //! 店員の性別テーブル

  static const u32 RANK_TABLE[20][2]; //! ランクに必要なフェスコインテーブル

};

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetAppLib)

#endif // __JOIN_FESTA_FACILITIES_UTIL_H__
