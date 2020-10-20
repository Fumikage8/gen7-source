//==============================================================================
/**
 * @file	JoinFestaFacilitiesData.h
 * @brief	ジョインフェスタ施設データ
 * @author	ichiraku_katsuhiko
 * @date	2015/10/22　12:52
 */
// =============================================================================
#ifndef __JOIN_FESTA_FACILITIES_DATA_H__
#define __JOIN_FESTA_FACILITIES_DATA_H__
#pragma once

// ライブラリ
#include <macro/include/gfl2_macros.h>
#include <str/include/gfl2_StrBuf.h>

// system
#include "System/include/GameStrLen.h"
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
    struct ShopItemInfo;
    struct LotInfo;
  }
}

namespace NetAppLib {
  namespace JoinFesta {
    class JoinFestaPersonalData;
  }
}

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

class JoinFestaFacilitiesData
{
  GFL_FORBID_COPY_AND_ASSIGN(JoinFestaFacilitiesData);
public:

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------

  /**
   * @sturct CoreData
   *         施設データ 72byte
   */
  struct CoreData{
    u8                      facilitiesId;             //!< 施設ID 全施設の連番の値 
    u8                      color;                    //!< 施設カラー
    b8                      playerFlag;               //!< 施設紹介者がいるかどうかフラグ
    u8                      playerSex;                //!< 施設紹介者の性別
    gfl2::str::STRCODE      playerName[ ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN ];    //!< 施設紹介者の名前         26byte(2x(12+1))
    u16                     patternHello;             //!< 定型：あいさつ
    u16                     patternParting;           //!< 定型：別れ
    u16                     patternEmotion;           //!< 定型：感動
    u16                     patternRegret;            //!< 定型：残念
    //--------------------------------------------------------------------
    u8                      fortuneResultHit;         //!< 占い結果：当たり
    u8                      fortuneResultAction;      //!< 占い結果：行動
    //--------------------------------------------------------------------
    u32                     itemBuyBit;               //!< 商品購入Bit
    u32                     randomSeed;               //!< 乱数の種
    u32                     characterId;              //!< 店員のキャラクターID
    u32                     myPrincipalId;            //!< 施設紹介者のプリンシバルID
    u64                     transferdId;              //!< 施設紹介者のトランスファードID
    // momiji追加
    u8                      tradeRemainCount;         //!< こうかんじょの残り回数
    u8                      dummy;
  };

public:
  // コンストラクタ
  JoinFestaFacilitiesData( JoinFestaScript::FacilitiesIndex facilitiesIndex, const JoinFestaDataTable* pDataTable );
  // デストラクタ
  virtual ~JoinFestaFacilitiesData();


  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
public:

  //=============================================================================
  //----------------------------------------------------------------------------
  // データテーブルのデータ取得系
  //----------------------------------------------------------------------------
  //! 施設ランク
  s32 GetRank() const;
  //! 施設種類
  JoinFestaScript::FacilitiesType GetFacilitiesType() const;
  //! 店名のラベル
  u32 GetShopNameMsgId() const;
  //! 店説明のラベル
  u32 GetShopDescriptionMsgId() const;
  //! サービス取得
  u32 GetServiceID( int slot ) const;

  //----------------------------------------------------------------------------
  // セーブのデータ系
  //----------------------------------------------------------------------------
  //! 紹介者のセット
  void SetIntroducer( NetAppLib::JoinFesta::JoinFestaPersonalData *personal );

  //! 施設IDの取得
  u8  GetFacilitiesId() const;
  //! 施設カラーの取得
  u8  GetFacilitiesColor() const;
  //! 店員のキャラモデルID取得
  u32 GetCharaModelId() const;
  //! 店員のキャラクターID取得
  u32 GetCharacterId() const;
  //! 店員の性別
  u8  GetAssistantSex() const;

  //! 施設紹介者がいるかどうか
  bool IsPlayerFlag() const;
  //! 施設紹介者の名前を取得
  const gfl2::str::STRCODE* GetPlayerName() const;
  //! 施設紹介者の性別を取得
  u8  GetPlayerSex() const;
  //! 施設紹介者の定型文を取得
  u16 GetPlayerPattern( JoinFestaScript::PatternType pattern ) const;

  //! 占い結果：当たり取得
  u8 GetFortuneResultHit() const;
  //! 占い結果：行動取得
  u8 GetFortuneResultAction() const;

  //! こうかんじょの残り回数取得
  u8 GetTradeRemainCount() const;
  //! こうかんじょの残り回数減算
  void SubTradeRemainCount();

  //! 乱数の種取得
  u32 GetRandomSeed() const;
  //! 乱数の種リセット
  void ResetRandomSeed();

  //! 抽選テーブルから乱数で抽選する
  u32 LotteryExecute( gfl2::math::Random* random ) const;

  //! アイテムIDで抽選アイテムのインデックスを取得
  u32 GetLotInfoIndexByItemId( const u32 itemId ) const;

  //! ビックリハウス（ホラーハウス）の抽選
  void LotteryFunHouse( u32 (&outHitItem)[3] );

  //! くじやの再挑戦出来るか抽選
  bool IsRetryDrawLot( gfl2::math::Random* random ) const;

  //! うらないやの[当たり・中あたり・大当たり]の抽選
  JoinFestaScript::FortuneResultHit LotteryFortuneResultHit( gfl2::math::Random* random ) const;
  //! うらないやの当たり行動の抽選
  JoinFestaScript::FortuneResultAction LotteryFortuneResultAction( gfl2::math::Random* random ) const;

#if PM_DEBUG
  //! 強制初期化
  void DEBUG_ForceInitialize( JoinFestaScript::FacilitiesIndex facilitiesIndex, const JoinFestaDataTable* pDataTable );
#endif // PM_DEBUG
  
  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================
private:

  //! 初期化
  void InitializeSetup( JoinFestaScript::FacilitiesIndex facilitiesIndex, const JoinFestaDataTable* pDataTable );

private:
  const ShopItemInfo*                m_pShopItemInfo;   //! 店データ
  const LotInfo*                     m_pLotInfo;        //! 抽選データ
  CoreData*                          m_pCoreData;       //! 施設データ 実体はセーブデータ

#if PM_DEBUG
private:
  u32 m_DebugForceSlot; // 強制抽選スロット
public:
  void DEBUG_SetForceSlot( u32 slot ){ m_DebugForceSlot = slot; }
#endif // PM_DEBUG
};

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

#endif // __JOIN_FESTA_FACILITIES_DATA_H__
