#ifndef __SAVEDATA_H__
#define __SAVEDATA_H__
//===================================================================
/**
 * @file    SaveData.h
 * @brief   セーブデータのメモリ確保クラス
 * @author  k.ohno
 * @date    11.12.3
 * @note    セーブデータをメモリに常駐するためのクラス
 */
//===================================================================
#pragma once

//#include <gflib.h>
#include "heap/include/gfl2_heap.h"

#include "Savedata/include/SaveDataInterface.h"
#include "Savedata/include/ISaveDataTotal.h"

#include "Savedata/include/RandomGroup.h"
#include "Savedata/include/MyStatus.h"
#include "Savedata/include/PokePartySave.h"
//#include <pml_PokeParty.h>
#include "Savedata/include/EventWork.h"

#include "Savedata/include/MyItemSave.h"
#include "Savedata/include/ZukanSave.h"
#include "Savedata/include/GTSSave.h"
#include "Savedata/include/UnionPokemonSave.h"
#include "Savedata/include/MiscSave.h"
#include "Savedata/include/FieldMenuSave.h"
#include "Savedata/include/ConfigSave.h"
#include "Savedata/include/GameTime.h"
#include "Savedata/include/BoxSave.h"
#include "Savedata/include/BoxPokemonSave.h"
#include "Savedata/include/Situation.h"
#include "Savedata/include/ResortSave.h"
#include "Savedata/include/PlayTime.h"
#include "Savedata/include/FieldMoveModelSave.h"
#include "Savedata/include/FashionSave.h"
#include "Savedata/include/JoinFestaPersonalSave.h"
#include "Savedata/include/JoinFestaDataSave.h"
#include "Savedata/include/BerrySpot.h"
#include "Savedata/include/FishingSpot.h"
#include "Savedata/include/LiveMatchSave.h"
#include "Savedata/include/BattleSpotSave.h"
#include "Savedata/include/PokeFinderSave.h"
#include "Savedata/include/Record.h"
#include "Savedata/include/MysteryGiftSave.h"
#include "Savedata/include/ValidationSave.h"
#include "Savedata/include/GameSyncSave.h"
#include "Savedata/include/PokeDiarySave.h"
#include "Savedata/include/BattleInstSave.h"
#include "Savedata/include/SodateyaSave.h"
#include "Savedata/include/WeatherSave.h"
#include "Savedata/include/QRReaderSave.h"
#include "Savedata/include/TurtleSalmonSave.h"
#include "Savedata/include/BattleFesSave.h"
#include "Savedata/include/FinderStudioSave.h"

GFL_NAMESPACE_BEGIN(Savedata)

class SaveLoadControl;

class Footer;

//-----------------------------------------------------------------------------
/**
 * @class SaveData
 */
//-----------------------------------------------------------------------------
class SaveData  : public ISaveDataTotal
{
  GFL_FORBID_COPY_AND_ASSIGN(SaveData); //コピーコンストラクタ＋代入禁止

  friend class Footer;

private:
  Footer * mFooter;

  //----------------------------------------------------------
  //@caution セーブメモリクラスを列挙
  //----------------------------------------------------------
  // ↓下記を追加したら「SAVEDATA_CLASS_NUM」も書き換えること！！！！
  /// 並びにも意味があるという事なのでオリジナルの番号を残す.
  Savedata::MyItem                mMyItem;                ///< 1  ///< 0
  Savedata::Situation             mSituation;             ///< 4  ///< 1
  Savedata::RandomGroup           mRandomGroup;           ///< 5  ///< 2
  Savedata::MyStatus              mMyStatus;              ///< 17 ///< 3
  Savedata::PokePartySave         mPokePartySave;         ///< 18 ///< 4
  Field::EventWork                mEventWork;             ///< 19 ///< 5
  Savedata::ZukanData             mZukanData;             ///< 20 ///< 6
  Savedata::GtsData               mGtsData;               ///< 28 ///< 7
  Savedata::UnionPokemon          mUnionPokemon;          ///< 22 ///< 8
  Savedata::Misc                  mMisc;                  ///< 11 ///< 9
  Savedata::FieldMenu             mFieldMenu;             ///< 29 ///< 10
  Savedata::ConfigSave            mConfig;                ///< 23 ///< 11
  Savedata::GameTime              mGameTime;              ///< 3  ///< 12
  Savedata::BOX                   mBox;                   ///< 12 ///< 13
  Savedata::BoxPokemon            mBoxPokemon;            ///< 56 ///< 14
  Savedata::ResortSave            mResort;                /// new ///< 15
  Savedata::PlayTime              mPlayTime;              ///< 6  ///< 16
  Savedata::FieldMoveModelSave    mFieldMoveModelSave;    ///< 7  ///< 17
  Savedata::Fashion               mFashion;               ///< 7  ///< 18
  Savedata::JoinFestaPersonalSave mJoinFestaVip;          ///< 16 ///< 19
  Savedata::JoinFestaPersonalSave mJoinFestaGuest;        ///< 16 ///< 20
  Savedata::JoinFestaDataSave     mJoinFestaData;         ///< new///< 21
  Savedata::BerrySpot             mBerrySpot;             ///< new///< 22
  Savedata::FishingSpot           mFishingSpot;           ///< new///< 23
  Savedata::LiveMatchData         mLiveMatchData;         ///< new///< 24
  Savedata::BattleSpotData        mBattleSpotData;        ///< new///< 25
  Savedata::PokeFinderSave        mPokeFinder;            ///< new///< 26
  Savedata::MysteryGiftSave       mMysteryGiftSave;       ///< 41 ///< 27
  Savedata::Record                mRecord;                ///< 44 ///< 28
  Savedata::ValidationSave        mValidation;            ///< 52 ///< 29
  Savedata::GameSyncSave          mGameSync;              ///< 50 ///< 30
  Savedata::PokeDiarySave         mPokeDiarySaveData;     ///< 43 ///< 31
  Savedata::BattleInstSave        mBattleInstSave;        ///< 43 ///< 32
  Savedata::Sodateya              mSodateya;              ///< 43 ///< 33
  Savedata::WeatherSave           mWeather;               ///< new///< 34
  Savedata::QRReaderSaveData      mQRReader;              ///< new///< 35
  Savedata::TurtleSalmonSave      mTurtleSalmonSave;      ///< new///< 36
  Savedata::BattleFesSave         mBattleFesSave;         ///< new///< 37
  Savedata::FinderStudioSave      mFinderStudioSave;      ///< new///< 38

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // 
  // momijiはここより下に追加すること！
  // （タートル＆サーモンでの都合上、nijiのセーブ部分は変えない）
  // 
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  // ↓上記を追加したら「SAVEDATA_CLASS_NUM」も書き換えること！！！！
  //----------------------------------------------------------
  //@caution 上記メンバ変数の数を記載
  //----------------------------------------------------------

  static const int SAVEDATA_CLASS_NUM = 39;
  static const int TURTLESALMON_SAVE_INDEX = 36;    // @caution 署名改ざん検知用に使用する為、数値を変更しないでください

  SaveDataInterface * mSaveObject[ SAVEDATA_CLASS_NUM ];

  //----------------------------------------------------------
  /**
   *  @brief    バックアップデータの整合性チェック
   *  @retval   true    正常なセーブデータ
   *  @retval   false   問題があるセーブデータ
   */
  //----------------------------------------------------------
  bool IsValidCheckData( s32 * error_info ) const;

  ///初期状態かどうかのチェック
  bool IsNullData( void ) const;

public:
  static const int SAVEDATA_ID_POKEMON_BOX    = 13; //ポケモンBOXデータのID
  //static const int SAVEDATA_ID_MY_PHOTO_ICON  = 50; //写真アイコンデータのID
  //static const int SAVEDATA_ID_PHOTO_DATA     = 57; //ゲーム内写真データのID

public:
  //コンストラクタ
  SaveData( gfl2::heap::HeapBase * heap );
  //デストラクタ
  virtual ~SaveData(){};


  //----------------------------------------------------------
  /**
   */
  //----------------------------------------------------------
  virtual int GetSaveObjectNum(void) const;

  //----------------------------------------------------------
  //----------------------------------------------------------
  virtual SaveDataInterface * GetSaveObject( u32 class_id );

  //----------------------------------------------------------
  //----------------------------------------------------------
  virtual size_t GetAlignmentSize( void ) const;

  //----------------------------------------------------------
  /**
   *  @brief    バックアップデータのクリア
   *  @param heap  クリア処理に使用するテンポラリヒープ
   */
  //----------------------------------------------------------
  virtual void Clear( gfl2::heap::HeapBase * heap );

  //----------------------------------------------------------
  /**
   * @brief   状態を取得する
   * @retval  STATE_NULL    データが存在しない
   * @retval  STATE_OK      正常
   * @retval  STATE_NG      異常、使用できない
   */
  //----------------------------------------------------------
  virtual ISaveDataTotal::State GetStatus( s32 * error_info = NULL ) const;

  //----------------------------------------------------------
  /**
   *  @brief    バックアップデータの整合性情報埋め込み
   */
  //----------------------------------------------------------
  virtual void SetupCheckData( void );

  //----------------------------------------------------------
  /**
   *  @brief    バックアップデータの無効情報埋め込み
   */
  //----------------------------------------------------------
  virtual void SetupNullData( void );

  //----------------------------------------------------------
  /**
   * @brief     セーブデータ書き換え禁止
   */
  //----------------------------------------------------------
  virtual void LockData( void );

  //----------------------------------------------------------
  /**
   * @brief     セーブデータ書き換え禁止を解除
   */
  //----------------------------------------------------------
  virtual void UnlockData( void );

  //----------------------------------------------------------
  /**
   * @brief 巻き戻し禁止：参照値の取得
   */
  //----------------------------------------------------------
  bit64 GetSecureValue( void ) const;

  //----------------------------------------------------------
  /**
   * @brief 巻き戻し禁止：ひとつ前の参照値の取得
   */
  //----------------------------------------------------------
  bit64 GetPreviousSecureValue( void ) const;

  //----------------------------------------------------------
  /**
   * @brief 巻き戻し防止：参照値を更新する
   */
  //----------------------------------------------------------
  bit64 RenewSecureValue( void );

  //----------------------------------------------------------
  /**
   * @brief 巻き戻し防止：参照値更新を取り消す
   */
  //----------------------------------------------------------
  void CancelSecureValue( void );



  //-----------------------------------------------------------------------------
  //@caution アクセス用クラスを列挙
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  /**
   *  @brief    手持ちアイテムの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::MyItem* GetMyItem( void ) { return &mMyItem; }
  inline const Savedata::MyItem* GetMyItem( void ) const { return &mMyItem; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ランダムグループの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::RandomGroup* GetRandomGroup( void ) { return &mRandomGroup; }
  inline const Savedata::RandomGroup* GetRandomGroup( void ) const { return &mRandomGroup; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    プレイヤーステータスの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::MyStatus* GetMyStatus( void ) { return &mMyStatus; }
  inline const Savedata::MyStatus* GetMyStatus( void ) const { return &mMyStatus; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    手持ちポケモンデータのセーブバイナリ変換
   */
  //-----------------------------------------------------------------------------
  inline void SerializePokeParty( const pml::PokeParty * pParty ) { mPokePartySave.Serialize( pParty ); };
  inline void DeserializePokeParty( pml::PokeParty * pParty ) { mPokePartySave.Deserialize( pParty ); };

  //-----------------------------------------------------------------------------
  /**
   * @brief   イベントワーク・フラグ領域の取得
   * @return  操作用ポインタ
   */
  //-----------------------------------------------------------------------------
  inline Field::EventWork * GetEventWork( void ) { return &mEventWork; };
  inline const Field::EventWork * GetEventWork( void ) const { return &mEventWork; };

  //-----------------------------------------------------------------------------
  /**
   * @brief   図鑑セーブデータの取得
   * @return  操作用ポインタ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::ZukanData * GetZukanData( void ) { return &mZukanData; };
  inline const Savedata::ZukanData * GetZukanData( void ) const { return &mZukanData; };

  //-----------------------------------------------------------------------------
  /**
   * @brief   GTSセーブデータの取得
   * @return  操作用ポインタ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::GtsData* GetGtsData( void ){return &mGtsData;}
  inline const Savedata::GtsData* GetGtsData( void ) const {return &mGtsData;}

  //-----------------------------------------------------------------------------
  /**
   * @brief   合体ポケモン退避先のセーブデータの取得
   * @return  操作用ポインタ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::UnionPokemon* GetUnionPokemon( void ) { return &mUnionPokemon; };
  inline const Savedata::UnionPokemon* GetUnionPokemon( void ) const { return &mUnionPokemon; };

  //-----------------------------------------------------------------------------
  /**
   *  @brief    未分類セーブデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::Misc* GetMisc( void ) { return &mMisc; }
  inline const Savedata::Misc* GetMisc( void ) const { return &mMisc; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    フィールドメニューセーブデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::FieldMenu * GetFieldMenu( void ) { return &mFieldMenu; }
  inline const Savedata::FieldMenu * GetFieldMenu( void ) const { return &mFieldMenu; }

  //-----------------------------------------------------------------------------
  /**
   * @brief   コンフィグのセーブデータの取得
   * @return  操作用ポインタ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::ConfigSave* GetConfig( void ) { return &mConfig; };
  inline const Savedata::ConfigSave* GetConfig( void ) const { return &mConfig; };

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ゲームタイムの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::GameTime* GetGameTime( void ) { return &mGameTime; }
  inline const Savedata::GameTime* GetGameTime( void ) const { return &mGameTime; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ボックスのデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::BOX* GetBOX( void ) { return &mBox; }
  inline const Savedata::BOX* GetBOX( void ) const { return &mBox; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ボックス内のポケモンデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::BoxPokemon* GetBoxPokemon( void ) { return &mBoxPokemon; }
  inline const Savedata::BoxPokemon* GetBoxPokemon( void ) const { return &mBoxPokemon; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    シチュエーションの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::Situation* GetSituation( void ) { return &mSituation; }
  inline const Savedata::Situation* GetSituation( void ) const { return &mSituation; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    リゾートの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::ResortSave* GetResort( void ) { return &mResort; }
  inline const Savedata::ResortSave* GetResort( void ) const { return &mResort; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    プレイタイムの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::PlayTime* GetPlayTime( void ) { return &mPlayTime; }
  inline const Savedata::PlayTime* GetPlayTime( void ) const { return &mPlayTime; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    動作モデルセーブデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::FieldMoveModelSave* GetFieldMoveModelSave( void ) { return &mFieldMoveModelSave; }
  inline const Savedata::FieldMoveModelSave* GetFieldMoveModelSave( void ) const { return &mFieldMoveModelSave; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    着せ替えのデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::Fashion* GetFashion( void ) { return &mFashion; }
  inline const Savedata::Fashion* GetFashion( void ) const { return &mFashion; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ジョインフェスタVIPパーソナルデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::JoinFestaPersonalSave* GetJoinFestaVipData( void ) { return &mJoinFestaVip; }
  inline const Savedata::JoinFestaPersonalSave* GetJoinFestaVipData( void ) const { return &mJoinFestaVip; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ジョインフェスタゲストパーソナルデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::JoinFestaPersonalSave* GetJoinFestaGuestData( void ) { return &mJoinFestaGuest; }
  inline const Savedata::JoinFestaPersonalSave* GetJoinFestaGuestData( void ) const { return &mJoinFestaGuest; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ジョインフェスタデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::JoinFestaDataSave* GetJoinFestaData( void ) { return &mJoinFestaData; }
  inline const Savedata::JoinFestaDataSave* GetJoinFestaData( void ) const { return &mJoinFestaData; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    フィールド上のきのみ状態データの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::BerrySpot* GetBerrySpotData(void) {return &mBerrySpot;}
  inline const Savedata::BerrySpot* GetBerrySpotData(void) const {return &mBerrySpot;}

  //-----------------------------------------------------------------------------
  /**
   *  @brief    フィールド上の釣りスポット状態データの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::FishingSpot* GetFishingSpotData(void) {return &mFishingSpot;}
  inline const Savedata::FishingSpot* GetFishingSpotData(void) const {return &mFishingSpot;}

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ライブ大会データの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::LiveMatchData*       GetLiveMatchData(void) {return &mLiveMatchData;}
  inline const Savedata::LiveMatchData* GetLiveMatchData(void) const {return &mLiveMatchData;}

  //-----------------------------------------------------------------------------
  /**
   *  @brief    バトルスポットデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::BattleSpotData*       GetBattleSpotData(void) {return &mBattleSpotData;}
  inline const Savedata::BattleSpotData* GetBattleSpotData(void) const {return &mBattleSpotData;}

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ポケファインダーデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::PokeFinderSave*       GetPokeFinderSave(void) {return &mPokeFinder;}
  inline const Savedata::PokeFinderSave* GetPokeFinderSave(void) const {return &mPokeFinder;}

  //-----------------------------------------------------------------------------
  /**
   *  @brief    不思議な贈り物セーブデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::MysteryGiftSave* GetMysteryGiftSave( void ) { return &mMysteryGiftSave; };
  inline const Savedata::MysteryGiftSave* GetMysteryGiftSave( void ) const { return &mMysteryGiftSave; };

  //-----------------------------------------------------------------------------
  /**
   *  @brief  レコードのセーブデータを取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::Record* GetRecordSaveData( void ) { return &mRecord; }
  inline const Savedata::Record* GetRecordSaveData( void ) const { return &mRecord; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief  不正チェックサーバ公開鍵のセーブデータを取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::ValidationSave* GetValidationSaveData( void ) { return &mValidation; }
  inline const Savedata::ValidationSave* GetValidationSaveData( void ) const { return &mValidation; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief  ゲームシンク用セーブデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::GameSyncSave* GetGameSync( void ) { return &mGameSync; }
  inline const Savedata::GameSyncSave* GetGameSync( void ) const { return &mGameSync; }



  //-----------------------------------------------------------------------------
  /**
   *  @brief    ポケモンダイアリーの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::PokeDiarySave* GetPokeDiarySaveData( void ) { return &mPokeDiarySaveData; }
  inline const Savedata::PokeDiarySave* GetPokeDiarySaveData( void ) const { return &mPokeDiarySaveData; }

  //-----------------------------------------------------------------------------
  /**
   * @brief   バトル施設セーブデータの取得
   * @return  操作用ポインタ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::BattleInstSave* GetBattleInstSave( void ) { return &mBattleInstSave; };
  inline const Savedata::BattleInstSave* GetBattleInstSave( void ) const { return &mBattleInstSave; };

  //-----------------------------------------------------------------------------
  /**
   * @brief   育て屋セーブデータの取得
   * @return  操作用ポインタ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::Sodateya* GetSodateyaSave( void ) { return &mSodateya; };
  inline const Savedata::Sodateya* GetSodateyaSave( void ) const { return &mSodateya; };

  //-----------------------------------------------------------------------------
  /**
   *  @brief    天候セーブデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::WeatherSave* GetWeatherSave(void) {return &mWeather;}
  inline const Savedata::WeatherSave* GetWeatherSasve(void) const {return &mWeather;}

  //-----------------------------------------------------------------------------
  /**
   *  @brief    QRReaderセーブデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::QRReaderSaveData*        GetQRReaderSave(void)       {return &mQRReader;}
  inline const Savedata::QRReaderSaveData*  GetQRReaderSave(void) const {return &mQRReader;}

#if 0
  //-----------------------------------------------------------------------------
  /**
   *  @brief    かわいがりのデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::Kawaigari* GetKawaigari( void ) { return &mKawaigari; }
  inline const Savedata::Kawaigari* GetKawaigari( void ) const { return &mKawaigari; }
  //-----------------------------------------------------------------------------
  /**
   *  @brief    トレーニングデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::TrainingSaveData* GetTraining( void ) { return &mTrainingSaveData; }
  inline const Savedata::TrainingSaveData* GetTraining( void ) const { return &mTrainingSaveData; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ミニゲームのデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::MiniGame* GetMiniGame( void ) { return &mMiniGame; };
  inline const Savedata::MiniGame* GetMiniGame( void ) const { return &mMiniGame; };

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ギミックワークの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::GimmickWork* GetGimmickWork( void ) { return &mGimmickWork; }
  inline const Savedata::GimmickWork* GetGimmickWork( void ) const { return &mGimmickWork; }





  //-----------------------------------------------------------------------------
  /**
   *  @brief    バトルボックスのデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::BattleBox* GetBattleBox( void ) { return &mBattleBox; }
  inline const Savedata::BattleBox* GetBattleBox( void ) const { return &mBattleBox; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    動作モデルのデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::MoveModelSave* GetMoveModelSave( void ) { return &mMmdl; }
  inline const Savedata::MoveModelSave* GetMoveModelSave( void ) const { return &mMmdl; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    サブイベント用セーブデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::SubEventSaveData* GetSubEvent( void ) { return &mSubEventSaveData; }
  inline const Savedata::SubEventSaveData* GetSubEvent( void ) const { return &mSubEventSaveData; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    PSSともだちデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::PssPersonalSaveData* GetPssFriendData( void ) { return &mPssFriend; }
  inline const Savedata::PssPersonalSaveData* GetPssFriendData( void ) const { return &mPssFriend; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    PSS知り合いデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::PssPersonalSaveData* GetPssAcquaintanceData( void ) { return &mPssAcquaintance; }
  inline const Savedata::PssPersonalSaveData* GetPssAcquaintanceData( void ) const { return &mPssAcquaintance; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    PSSお気に入りデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::PssPersonalSaveData* GetPssFavoriteData( void ) { return &mPssFavorite; }
  inline const Savedata::PssPersonalSaveData* GetPssFavoriteData( void ) const { return &mPssFavorite; }
  //-----------------------------------------------------------------------------
  /**
   * @brief   ホログラム会話セーブデータの取得
   * @return  操作用ポインタ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::HologramMailData* GetHologramMailData( void ) { return &mHologramMailData; };
  inline const Savedata::HologramMailData* GetHologramMailData( void ) const { return &mHologramMailData; };

  //-----------------------------------------------------------------------------
  /**
   * @brief   コンフィグのセーブデータの取得
   * @return  操作用ポインタ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::Config* GetConfig( void ) { return &mConfig; };
  inline const Savedata::Config* GetConfig( void ) const { return &mConfig; };

  //-----------------------------------------------------------------------------
  /**
   * @brief   かわいがり模様替え関連のセーブデータの取得
   * @return  操作用ポインタ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::KawaigariGoods* GetKawaigariGoods( void ) { return &mKawaigariGoods; };
  inline const Savedata::KawaigariGoods* GetKawaigariGoods( void ) const { return &mKawaigariGoods; };

  //-----------------------------------------------------------------------------
  /**
   * @brief   アシストパワーのセーブデータの取得
   * @return  操作用ポインタ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::AssistPowerData* GetAssistPowerData( void ) { return &mAssistPowerData; };
  inline const Savedata::AssistPowerData* GetAssistPowerData( void ) const { return &mAssistPowerData; };

  //-----------------------------------------------------------------------------
  /**
   * @brief   怪力岩セーブデータの取得
   * @return  操作用ポインタ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::FieldRockData* GetFieldRockData( void ) { return &mFieldRockData; };
  inline const Savedata::FieldRockData* GetFieldRockData( void ) const { return &mFieldRockData; };

  //-----------------------------------------------------------------------------
  /**
   * @brief   プロモーションビデオセーブデータの取得
   * @return  操作用ポインタ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::Promotion* GetPromotion( void ){return &mPromotion;}
  inline const Savedata::Promotion* GetPromotion( void ) const {return &mPromotion;}

  //-----------------------------------------------------------------------------
  /**
   *  @brief  フィールドメニューのセーブデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::FieldMenu* GetFieldMenu( void ) { return &mFieldMenu; }
  inline const Savedata::FieldMenu* GetFieldMenu( void ) const { return &mFieldMenu; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief  プロフィールアンケート用セーブデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::ProfileEnqueteData* GetProfileEnqueteData( void ) { return &mProfileEnquete; }
  inline const Savedata::ProfileEnqueteData* GetProfileEnqueteData( void ) const { return &mProfileEnquete; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief  プロフィール通信履歴用セーブデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::ProfileHistory* GetProfileHistory( void ) { return &mProfileHistory; }
  inline const Savedata::ProfileHistory* GetProfileHistory( void ) const { return &mProfileHistory; }



  //-----------------------------------------------------------------------------
  /**
   *  @brief    エンカウント関連のデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::Encount* GetEncount( void ) { return &mEncount; }
  inline const Savedata::Encount* GetEncount( void ) const { return &mEncount; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    BOSSアーカイブ取得履歴関連のデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::BossHistory* GetBossHistory( void ) { return &mBossHistory; }
  inline const Savedata::BossHistory* GetBossHistory( void ) const { return &mBossHistory; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    すれちがいBOX取得履歴関連のデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::CecHistory* GetCecHistory( void ) { return &mCecHistory; };
  inline const Savedata::CecHistory* GetCecHistory( void ) const { return &mCecHistory; };

  //-----------------------------------------------------------------------------
  /**
   *  @brief    レギュレーションの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::BattleMatch* GetBattleMatch( void ) { return &mBattleMatch; };
  inline const Savedata::BattleMatch* GetBattleMatch( void ) const { return &mBattleMatch; };

  //-----------------------------------------------------------------------------
  /**
   *  @brief    アクセスポイントデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::AccessPointSaveData* GetAccessPointSaveData( void ) { return &mAccessPointSaveData; };
  inline const Savedata::AccessPointSaveData* GetAccessPointSaveData( void ) const { return &mAccessPointSaveData; };

  //-----------------------------------------------------------------------------
  /**
   *  @brief  殿堂入りのセーブデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::Dendou* GetDendouiriData( void ) { return &mDendou; }
  inline const Savedata::Dendou* GetDendouiriData( void ) const { return &mDendou; }


  //-----------------------------------------------------------------------------
  /**
   * @brief   トライアルハウスセーブデータの取得
   * @return  操作用ポインタ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::TrialHouse* GetTrialHouseSave( void ) { return &mTrialHouse; };
  inline const Savedata::TrialHouse* GetTrialHouseSave( void ) const { return &mTrialHouse; };

  //-----------------------------------------------------------------------------
  /**
   * @brief   木の実セーブデータの取得
   * @return  操作用ポインタ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::KinomiData* GetKinomiSaveData( void ) { return &mKinomiData; };
  inline const Savedata::KinomiData* GetKinomiSaveData( void ) const { return &mKinomiData; };



  //-----------------------------------------------------------------------------
  /**
   *  @brief  フレンドサファリのセーブデータを取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::FriendSafariSaveData* GetFriendSafariSaveData( void ) { return &mFriendSafariSaveData; }
  inline const Savedata::FriendSafariSaveData* GetFriendSafariSaveData( void ) const { return &mFriendSafariSaveData; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief  写真アイコンのセーブデータを取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::MyPhotoIconSaveData* GetMyPhotoIconSaveData( void ) { return &mMyPhotoIcon; }
  inline const Savedata::MyPhotoIconSaveData* GetMyPhotoIconSaveData( void ) const { return &mMyPhotoIcon; }



  //-----------------------------------------------------------------------------
  /**
   *  @brief  Eショップセーブデータを取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::EShopSaveData* GetEShopSaveData( void ) { return &mEShopSaveData; }
  inline const Savedata::EShopSaveData* GetEShopSaveData( void ) const { return &mEShopSaveData; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    コンテストまわりの情報の取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::Contest* GetContest( void ) { return &mContest; }
  inline const Savedata::Contest* GetContest( void ) const { return &mContest; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ひみつきちのセーブデータを取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::SecretBase* GetSecretBase( void ) { return &mSecretBase; }
  inline const Savedata::SecretBase* GetSecretBase( void ) const { return &mSecretBase; }

  //-----------------------------------------------------------------------------
  /**
   * @brrief  ゲーム内撮影写真データの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::PhotoSaveData * GetPhotoSaveData( void ) { return &mPhotoSaveData; }
  inline const Savedata::PhotoSaveData * GetPhotoSaveData( void ) const { return &mPhotoSaveData; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    sango用通信セーブデータを取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::SangoNetworkSavedata* GetSangoNetworkSavedata( void ) { return &mSangoNetworkSavedata; }
  inline const Savedata::SangoNetworkSavedata* GetSangoNetworkSavedata( void ) const { return &mSangoNetworkSavedata; }
#endif

  //-----------------------------------------------------------------------------
  /**
  *  @brief    バトルフェスサークルセーブデータの取得
  */
  //-----------------------------------------------------------------------------
  inline Savedata::BattleFesSave* GetBattleFesSave(void) { return &mBattleFesSave; }
  inline const Savedata::BattleFesSave* GetBattleFesSave(void) const { return &mBattleFesSave; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief    ファインダースタジオ用セーブデータの取得
  */
  //-----------------------------------------------------------------------------
  inline Savedata::FinderStudioSave* GetFinderStudioSave( void ) { return &mFinderStudioSave; }
  inline const Savedata::FinderStudioSave* GetFinderStudioSave( void ) const { return &mFinderStudioSave; }


};  //SaveData

GFL_NAMESPACE_END(Savedata)



#endif //__GAMEDATA_H__




