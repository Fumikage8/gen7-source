#if !defined( __GAMEDATA_H__ )
#define __GAMEDATA_H__
//===================================================================
/**
 * @file    GameData.h
 * @brief   ゲーム進行データ管理
 * @author  k.ohno -> Miyuki Iwasawa
 * @date    15.2.28
 * @note
 * 基本的にゲーム進行データはセーブデータとして存在するが、
 * 通信時の複数プレイヤー情報やフィールドマップ情報などセーブデータに
 * のるとは限らない情報もある。
 * そのようなセーブデータ・非セーブデータへ並列にアクセスするインターフェイスを
 * 各パートごとに構成するとパートごとにアクセス手法が違ったり、また同じ機能の
 * ものが複数存在するなどプログラム全体の複雑さが増してしまう可能性が高い。
 * それを避けるため、共通インターフェイスを作ることによって簡略化する。
 */
//===================================================================
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <ui/include/gfl2_Ui.h>
//#include <pml_PokeParty.h>
#include "Savedata/include/SaveData.h"

#include "System/include/PokemonVersion.h"
#include "Battle/include/battle_ResultData.h"

#include "SaveData/include/SaveLoadControl.h"
#if   defined(GF_PLATFORM_CTR)
#include "SaveData/include/CTRSaveLoadControl.h"
#else
#include "SaveData/include/Win32SaveLoadControl.h"
#endif

#include "Field/FieldStatic/include/Zone/ZoneDataLoaderTypes.h" //ZoneID型の参照

// 育て屋
#include "Field/FieldStatic/include/Sodateya/FieldSodateyaWalkCheckData.h"

#include "niji_conv_header/field/WeatherKindDef.h"

// バトルフェス
#include "Field/FieldStatic/include/BattleFes/BattleFesManager.h"


//===================================================================
//
//
//  前方参照宣言
//
//
//  GameData/SaveDataはゲーム進行データの各クラスへのアクセスを
//  提供するだけなので、クラス詳細を知る必要はない。
//  カプセル化を進めるためにはそれぞれの利用箇所で、
//  該当クラスのヘッダをインクルードして利用する。
//
//===================================================================

namespace gfl2 {
  namespace util {
    class GLHeapAllocator;
  }
}
namespace pml{
  class PokeParty;
};

namespace Savedata{
  class MyStatus;
  class ZukanSave;
  class BattleInstSave;
  class Sodateya;
};

namespace app{
  namespace bag{
    class PlayData;
  }
}

namespace App{
  namespace FieldMenu{
    class PlayData;
  }
  namespace TrainerPass{
    class PlayData;
  }
}

namespace Field{

  namespace MoveModel{
    class FieldMoveModelManager;
    class FieldMoveCodeManager;
    class FieldActionCommandManager;
  }

  namespace Encount {
    class EncountWork;
  }
  namespace Camera{
    struct SaveData;
  }

  namespace FieldScript{
    class GameData;
  }

  class FieldRootNode;
  class World;
  class Area;
  class ZoneDataLoader;
  class FieldSound;
  class TrafficManager;
  class FieldItemManager;
  class FieldBerryManager;
  class WarpDataTools;
  class FieldSodateya;

  namespace TrialModel{
    struct FinderSpotReserveData;
  }

  namespace Debug{
    class StartSettingAccessor;
  }
}

namespace trainer{
  class TrainerPairData;
}


GFL_NAMESPACE_BEGIN(GameSys)

//--------------------------------------------------------------
/**
 * @class GameData
 */
//--------------------------------------------------------------
class GameData
{
  GFL_FORBID_COPY_AND_ASSIGN(GameData); //コピーコンストラクタ＋代入禁止

private:

  gfl2::util::GLHeapAllocator* m_pDeviceHeapAllocator;  ///< デバイスヒープアロケータ

  Savedata::SaveData *    mSaveData;              ///< セーブデータ本体
  Savedata::SaveLoadControl* mSaveControl;  //セーブコントロールクラス
  pml::PokeParty*         mPlayerParty;           ///< プレイヤーパーティ
  pml::PokeParty*         mQRParty;               ///< QRパーティ
  gfl2::str::STRCODE* mQRPartyName;               ///< QRパーティ名
  u8  m_QRBattleTeamID[12];
  u32 mQRParam1;                                  ///< QRパラメータ１
  u32 mQRParam2;                                  ///< QRパラメータ２
  u8* mSignatureBuffer;                           ///< 署名バッファ
  Savedata::PlayTime*     mPlayTime;              ///< プレイ時間

  app::bag::PlayData*     mBagPlayData;             ///<ゲーム中に保持し続けるバッグ画面のデータ
  App::FieldMenu::PlayData* mFieldMenuPlayData;     ///<ゲーム中に保持し続けるフィールド下画面のデータ
  App::TrainerPass::PlayData* mTrainerPassPlayData; ///<ゲーム中に保持し続けるトレーナーパス画面のデータ

  bool mSaveLock;                                 ///<セーブ処理のロックフラグ(true時はセーブ不可）
  bool mIsLoaded;                                 ///<ロードしたかどうか
  int mFieldMenuMode;                             ///<フィールドメニューの開始モード
  gfl2::heap::HeapBase * mTempHeap;

  Field::World* mWorld;           ///< ワールドデータ
  Field::Area*  mArea;            ///< フィールドエリアデータ
  Field::ZoneDataLoader* mZoneData; ///< ゾーンデータローダー

  Field::FieldSound*                           m_pFieldSound;     ///< フィールドサウンドコントローラー
  Field::WarpDataTools*                        m_pWarpDataTools;  ///< フィールド ワープデータアクセサ

  // 後々プレイヤーのセーブデータ
  Field::Location  mStartLoc;              ///<セーブ復帰場所
  Field::Location  mSpecialLoc;            ///<特殊セーブ復帰
  Field::Location  mEscapeLoc;             ///<脱出先

  // 動作モデル用
  Field::MoveModel::FieldMoveModelManager*     m_pFieldCharaModelManager;
  Field::MoveModel::FieldMoveCodeManager*      m_pFieldMoveCodeManager;
  Field::FieldRootNode*                        m_pFieldRootNode;

  Field::Encount::EncountWork*                 m_pEncountWork;  ///< エンカウント制御ワーク

  BATTLE_RESULT_DATA                           m_LastBattleResultData;    ///<フィールド最後のバトル結果

  trainer::TrainerPairData*                    m_pTrainerPairData;  ///< 視線トレーナーペアデータ

  // 往来
  Field::TrafficManager*                       m_pTrafficManager;

  // 配置アイテム
  Field::FieldItemManager*                     m_pFieldItemManager;

  // きのみ
  Field::FieldBerryManager*                    m_pFieldBerryManager;

  // 育てやデータ
  Field::FieldSodateyaWalkCheckData*         m_pSodateyaWalkCheckData;

  // カメラデータ
  Field::Camera::SaveData*                   m_pFieldCameraData;

  // スクリプトデータ
  Field::FieldScript::GameData*              m_pFieldScriptData;

  // バトルフェスマネージャー
  BattleFes::BattleFesManager*                 m_pBattleFesManager;

  //ショート版デモ再生フラグ
  bool m_FlyDemoShowed;    //リザードンライドデモ見た
  bool m_ResortDemoShowed; //リゾートへの移動デモ見た
  //マンタインサーフ
  int m_SurfRecord;
  //ルナソルダイブ
  u32 m_LunaSolDistance;
  u32 m_LunaSolMonsno;
  //@fix MMCat[379]:天候「暗黒」時のポニのビーチ、ナッシー・アイランドの状態について >>　Mcatコメント内有るaの案件対処
  bool m_SkyboxDarkFlag;

#if PM_DEBUG
  Field::Debug::StartSettingAccessor*        m_pDebugStartSettingAccessor;
  Field::weather::WeatherKind m_DebugWeather;
#endif //PMDEBUG

  //--------------------------------------------------------------------
  /// JoinFesta差し替え用のデータフィールド常駐データ
  //    ここ以外では、FieldResidentのGimmickEncount::Manager*も動作をとめます。
  /// Field関連の常駐データをGameDataに置く方は高橋までご連絡ください。
  struct JoinFestaSwitchFieldData
  {
    // JoinFestaかどうか。
    b32   mIsJoinFesta;

    // 通常状態のFieldClose後か？
    b32 mIsFieldClose;

    // 2重もち
    Savedata::FieldMoveModelSave mFieldMoveModelSave; ///< 動作モデルセーブデータ
    Savedata::Misc               mMisc;               ///< Misc
    Savedata::Situation          mSituation;          ///< Situation
    Field::Location  mStartLoc;              ///<セーブ復帰場所
    Field::Location  mSpecialLoc;            ///<特殊セーブ復帰
    Field::Location  mEscapeLoc;             ///<脱出先

    // 更新停止するもの
    Field::TrafficManager*                       m_pTrafficManager;
    Field::FieldItemManager*                     m_pFieldItemManager;
    Field::FieldBerryManager*                    m_pFieldBerryManager;
    Savedata::BerrySpot*          mpBerrySpot;
    Savedata::FishingSpot*        mpFishingSpot;

    JoinFestaSwitchFieldData() : 
        mIsJoinFesta(false)
      , mFieldMoveModelSave()
      , mMisc()
      , mSituation()
      , m_pTrafficManager(NULL)
      , m_pFieldItemManager(NULL)
      , m_pFieldBerryManager(NULL)
      , mpBerrySpot(NULL)
      , mpFishingSpot(NULL)
    {
    }
  };
  JoinFestaSwitchFieldData  m_JoinFestaSwitchFieldData;
  /// JoinFesta差し替え用のデータフィールド常駐データ
  //--------------------------------------------------------------------

  //--------------------------------------------------------------------
  /// Battleの非セーブデータ構造体
  //--------------------------------------------------------------------
  struct BattleData
  {
    //  バトルのボール使用ショートカットで最後に使ったボール
    u16 mLastShortCutUseBallItemId;
    u16 mDummy;

    BattleData()
    {
      mLastShortCutUseBallItemId  = LAST_SHORT_CUT_USE_BALL_ITEM_ID_NONE;
      mDummy  = 0;
    }
  };
  BattleData  m_battleData;
  

  /// セーブファイル書き込み状態
  enum WRITE_STATE {
    WRITE_STATE_NULL = 0,
    WRITE_STATE_NORMAL,
    WRITE_STATE_FIRST,
    WRITE_STATE_FIRST_END,
    WRITE_STATE_LAST,
    WRITE_STATE_FORMAT,
  };
  WRITE_STATE mWriteState;

  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
  // FinderSpot予約管理データ
  Field::TrialModel::FinderSpotReserveData *m_pFinderSpotReserveData[Savedata::PokeFinderSave::MAX_POKE_FINDER_SPOT];
  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

public:
  //コンストラクタ
  GameData(gfl2::heap::HeapBase* deviceHeap, gfl2::ui::DeviceManager* uiDeviceManager );
  //デストラクタ
  ~GameData();

  //  バトル中のショートカットで使用するモンスターボールアイテムidの無効値
  enum
  {
    LAST_SHORT_CUT_USE_BALL_ITEM_ID_NONE  = ITEM_DUMMY_DATA,
  };

  ///「さいしょから」ゲーム開始用データ設定
  void SetFirstData( gfl2::heap::HeapBase * pTempDevHeap );

#if PM_DEBUG
  ///デバッグ開始用データ設定
  enum DebugDataType
  {
    DEBUG_DATA_TYPE_CTRNAME,  ///<プレイヤー名にCTR本体名を使う
    DEBUG_DATA_TYPE_DEFAULTNAME,  ///<エル等デフォルト名を使う
    DEBUG_DATA_TYPE_NETWORK_RAND,  ///< 通信テスト用容姿ランダム　プレイヤー名にCTR本体名を使う
    DEBUG_DATA_TYPE_TOP_PFM_DRESS,  ///< 処理負荷用容姿固定　プレイヤー名にCTR本体名を使う
  };
  void SetDebugData( DebugDataType type, gfl2::heap::HeapBase* pTempDevHeap, int sex = PM_FEMALE );
  ///バトルデバッグ用データ設定（全アイテムを100ずつ持ちます）
  void SetDebugBattleData( gfl2::heap::HeapBase * pTempHeap, bool dontClearZukan );

  void SetDebugWeather(Field::weather::WeatherKind weather_kind);
  Field::weather::WeatherKind GetDebugWeather(void);
  void Set(Field::Debug::StartSettingAccessor* pStartSettingAccessor) { m_pDebugStartSettingAccessor = pStartSettingAccessor; }
  const Field::Debug::StartSettingAccessor* GetDebugStartSettingAccessor( void ) const { return m_pDebugStartSettingAccessor; }
#endif

  /**
   *  @biref フィールドゾーンデータ取得
   */
  inline Field::ZoneDataLoader* GetFieldZoneDataLoader(){ return mZoneData; }
  inline const Field::ZoneDataLoader* GetFieldZoneDataLoader() const { return mZoneData; }

  /**
   *  @biref フィールドワールドデータ取得
   */
  inline Field::World* GetFieldWorld(){ return mWorld; }
  /**
   *  @biref フィールドワールドデータ取得
   */
  inline const Field::World* GetFieldWorld() const { return mWorld; }

  /**
   *  @biref フィールドエリアデータ取得
   */
  inline Field::Area* GetFieldArea(){ return mArea; }
  /**
   *  @biref フィールドエリアデータ取得
   */
  inline const Field::Area* GetFieldArea() const { return mArea; }

  /**
   *  @biref フィールドサウンドコントローラー取得
   */
  inline Field::FieldSound* GetFieldSound() { return m_pFieldSound; }
  inline const Field::FieldSound* GetFieldSound() const { return m_pFieldSound; }

  /**
   *  @biref ワープデータコントローラー取得
   */
  inline Field::WarpDataTools* GetWarpDataTools() { return m_pWarpDataTools; }
  inline const Field::WarpDataTools* GetWarpDataTools() const { return m_pWarpDataTools; }

  /**
   *  @biref メニューオープンモード設定
   */
  inline void SetFieldMenuOpen(int mode){ mFieldMenuMode=mode; }
  /**
   *  @biref メニューオープンモード取得
   */
  inline const int GetFieldMenuOpen(void) const { return mFieldMenuMode; }

  /**
   *  @biref 視線トレーナーペアデータ取得
   */
  inline trainer::TrainerPairData* GetTrainerPairData() { return m_pTrainerPairData; }
  inline const trainer::TrainerPairData* GetTrainerPairData() const { return m_pTrainerPairData; }

  /**
   *  @biref バトルでボールショートカットボタンで最後に使ったボールid
   */
   // @fix ディレクターさん要望　バトル中にショートカットボタンを使って最後に使ったボールアイテムidをゲーム終了まで保持
  u32 GetBattleDataShortcutUseBallItemId() const { return m_battleData.mLastShortCutUseBallItemId; }
  void SetBattleDataShortcutUseBallItemId( const u16 itemId )  { m_battleData.mLastShortCutUseBallItemId  = itemId; }

  //============================================================================================
  //    セーブ処理
  //============================================================================================

  //--------------------------------------------------------------
  /**
   * @brief   セーブデータの初期化開始
   * @retval  gfl::fs::BackupSystem::RESULT_OK       初期化開始
   */
  //--------------------------------------------------------------
  void FormatStart( gfl2::heap::HeapBase * pTempHeap );

  //--------------------------------------------------------------
  /**
   * @brief   セーブデータの初期化メイン
   * @retval  gfl2::fs::FsCtrBackupSystem::RESULT_OK       初期化処理完了
   * @retval  gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE 初期化処理継続中
   * @retval  gfl2::fs::FsCtrBackupSystem::RESULT_NG_xxxx  失敗
   */
  //--------------------------------------------------------------
  bool FormatMain( gfl2::fs::FsCtrBackupSystem::Result * result );


  //--------------------------------------------------------------
  /**
   * @brief   セーブデータワークからゲームデータが持つ情報をロードする
   */
  //--------------------------------------------------------------
  gfl2::fs::FsCtrBackupSystem::Result LoadStart( gfl2::heap::HeapBase * pTempHeap );


  //--------------------------------------------------------------
  /**
   * @brief   ゲームデータが持つ情報を元にロードを実行
   * @retval  ロード結果  RESULT_OKで終了
   */
  //--------------------------------------------------------------
  gfl2::fs::FsCtrBackupSystem::Result LoadMain(void);

  Savedata::ISaveDataTotal::State LoadAtOnce( gfl2::heap::HeapBase * pTempHeap );

  //--------------------------------------------------------------
  /**
   * @brief   ゲームデータが持つ情報を元にセーブを実行
   * @param   gamedata    ゲームデータへのポインタ
   * @retval  セーブ結果
   */
  //--------------------------------------------------------------
  void SaveStart( gfl2::heap::HeapBase * pTempHeap );


  //--------------------------------------------------------------
  /**
   * @brief   ゲームデータが持つ情報を元にセーブを実行
   * @retval  セーブ結果  RESULT_OKで終了
   */
  //--------------------------------------------------------------
  gfl2::fs::FsCtrBackupSystem::Result SaveMain(void);

    //--------------------------------------------------------------
  /**
   * @brief   セーブを途中まで実行：開始
   * @param[in]   pTempHeap         作業用ヒープ
   *
   * @note  LastSave～と組みにして使用します。
   * @attention   セーブ失敗は致命的なので内部でFATALにします
   */
  //--------------------------------------------------------------
  void FirstSaveStart( gfl2::heap::HeapBase * pTempHeap );

  //--------------------------------------------------------------
  /**
   * @brief   セーブを途中まで実行：メイン
   * @param[out]  result  結果を受け取るポインタ
   * @return  bool  trueのとき、完了  falseのとき実行中
   *
   * -  gfl::fs::BackupSystem::RESULT_OK       セーブ完了
   * -  gfl::fs::BackupSystem::RESULT_NG_xxxx  セーブ失敗
   */
  //--------------------------------------------------------------
  bool FirstSaveMain( gfl2::fs::FsCtrBackupSystem::Result * result );

  //--------------------------------------------------------------
  /**
   * @brief   セーブを途中まで実行：解除
   */
  //--------------------------------------------------------------
  void FirstSaveCancel( void );

  //--------------------------------------------------------------
  /**
   * @brief   セーブを途中から完了処理
   * @note    エラーはすべて製品版では発生してはならないもの
   */
  //--------------------------------------------------------------
  void LastSave( void );

  //--------------------------------------------------------------
  /**
   * @brief   セーブ途中から完了処理：開始
   * @param[in]   pTempHeap         作業用ヒープ
   *
   * @note  FirstSave～と組みにして使用します。
   * @note  FirstSaveMain完了後、通信同期をとって呼び出すことで
   * @note  人為的なデータの増殖などを予防するための機能です。
   */
  //--------------------------------------------------------------
  bool LastSaveStart( gfl2::heap::HeapBase * pTempHeap );

  //--------------------------------------------------------------
  /**
   * @brief   セーブ途中から完了処理：メイン
   * @param[out]  result  結果を受け取るポインタ
   * @return      bool  trueのとき、完了  falseのとき実行中
   *
   * -  gfl::fs::BackupSystem::RESULT_OK       セーブ完了
   * -  gfl::fs::BackupSystem::RESULT_NG_xxxx  セーブ失敗
   */
  //--------------------------------------------------------------
  bool LastSaveMain( gfl2::fs::FsCtrBackupSystem::Result * result );

  //----------------------------------------------------------------------------
  /**
   *  @brief  セーブ前にSaveDataへの反映処理
   */
  //-----------------------------------------------------------------------------
  void SerializeSaveData(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  ロード後にGameDataへの展開処理
   */
  //-----------------------------------------------------------------------------
  void DeserializeSaveData(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief セーブデータのクリア
   */
  //-----------------------------------------------------------------------------
  void ClearSaveData( gfl2::heap::HeapBase * pTempHeap );


  //-----------------------------------------------------------------------------
  /// 巻き戻し防止用保存値を更新
  //-----------------------------------------------------------------------------
  void RenewSaveDataSecureValue( void );

  //-----------------------------------------------------------------------------
  /// 巻き戻し防止用保存値の更新をキャンセル
  //-----------------------------------------------------------------------------
  void CancelSaveDataSecureValue( void );

  //-----------------------------------------------------------------------------
  /// 巻き戻し防止用保存値を3DS本体に反映
  //-----------------------------------------------------------------------------
  void Update3DSSecureValue( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief 巻き戻し防止機能の状態取得
   * @retval  true    問題なし
   * @retval  false   巻き戻りが発生している
   */
  //-----------------------------------------------------------------------------
  bool VerifySecureValue( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief 巻き戻し防止機能の状態取得（ひとつ前のSecureValueとの比較）
   * @retval  true    問題なし
   * @retval  false   巻き戻りが発生している
   */
  //-----------------------------------------------------------------------------
  bool VerifyPreiousvSecureValue( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief　巻き戻し防止機能：本体保存SecureValueを現在のセーブデータに一致させる
   */
  //-----------------------------------------------------------------------------
  void RepairCTRSecureValue( void );

  //--------------------------------------------------------------
  /**
   * @brief   セーブデータを格納するために必要なバッファサイズを取得する(PGL通信用）
   *
   * @param   isEnablePokemonBox ポケモンボックスデータを含めるならtrueを指定
   *
   * @return  セーブデータを格納するために必要なバッファサイズ
   */
  //--------------------------------------------------------------
  u32 GerSaveDataBufferSize( bool isEnablePokemonBox );

  //--------------------------------------------------------------
  /**
   * @brief   ゲームデータをバッファに書き込み(PGL通信用）
   * @param   pBuffer            格納先バッファ
   * @param   pHeap              ワーク用ヒープ
   * @param   isEnablePokemonBox ポケモンボックスデータを含めるならtrueを指定
   */
  //--------------------------------------------------------------
  void WriteSaveDataToBuffer( u8* pBuffer, gfl2::heap::HeapBase * pHeap, bool isEnablePokemonBox );

#if PM_DEBUG
  //--------------------------------------------------------------
  /**
   * @brief   ゲームデータをバッファから読み込む(PGLデバッグ用）
   * @param   pBuffer            読み込み先バッファ
   * @param   pHeap              ワーク用ヒープ
   * @param   isEnablePokemonBox ポケモンボックスデータを含めるならtrueを指定
   */
  //--------------------------------------------------------------
  void LoadSaveDataFromBuffer( const u8* pBuffer, gfl2::heap::HeapBase * pHeap, bool isEnablePokemonBox );
#endif

  //-----------------------------------------------------------------------------
  /**
   * @brief　ジョインフェスタモード開始
   * @note このタイミングで２重化されているセーブデータの差し替え、ゾーンフラグ・ワークのリセットをフック
   */
  //-----------------------------------------------------------------------------
  void StartJoinFestaFieldData( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief　ジョインフェスタモード遷移時に　FieldmapがCloseされたか？
   * @note    このタイミングで、Manager系を切り替える。
   */
  //-----------------------------------------------------------------------------
  void SetJoinFestaManagerSwitch( b32 flag ){ m_JoinFestaSwitchFieldData.mIsFieldClose = flag; }


  //-----------------------------------------------------------------------------
  /**
   * @brief　ジョインフェスタモード終了
   * @note このタイミングで２重化されているセーブデータの差し替え、ゾーンフラグ・ワークのリセットをマネージャーの切り替えが元に戻ります。
   */
  //-----------------------------------------------------------------------------
  void EndJoinFestaFieldData( void );


  //-----------------------------------------------------------------------------
  /**
   * @brief　現在ジョインフェスタ　常駐データ切り替えモードかチェック
   */
  //-----------------------------------------------------------------------------
  b32 IsNowJoinFestaFieldData( void ) const { return m_JoinFestaSwitchFieldData.mIsJoinFesta; }

  //-----------------------------------------------------------------------------
  /**
   * @brief　現在ジョインフェスタ　常駐データ切り替えモードかチェック(Manager版）
   */
  //-----------------------------------------------------------------------------
  b32 IsNowJoinFestaFieldData_Manager( void ) const { return (m_JoinFestaSwitchFieldData.mIsJoinFesta && m_JoinFestaSwitchFieldData.mIsFieldClose); };

  //============================================================================================
  //    セーブデータアクセサ
  //============================================================================================



  //----------------------------------------------------------------------------
  /**
   *  @brief      セーブデータ管理
   */
  //-----------------------------------------------------------------------------
  inline Savedata::SaveLoadControl* GetSaveLoadControl( void )  { return mSaveControl; }
  inline Savedata::SaveData* GetSaveData( void )  { return mSaveData; }



  //----------------------------------------------------------------------------
  /**
   *  @brief      セーブ復帰　ロケーション　設定・取得
   */
  //-----------------------------------------------------------------------------
  inline void SetStartLocation( const Field::Location* cp_loc ) 
  {
    if( !m_JoinFestaSwitchFieldData.mIsJoinFesta )
    {
      mStartLoc = *cp_loc; 
    }
    else
    {
      m_JoinFestaSwitchFieldData.mStartLoc = *cp_loc;
    }
  }
  inline const Field::Location* GetStartLocation( const b32 original = false ) const 
  {
    return ((!m_JoinFestaSwitchFieldData.mIsJoinFesta) || original) ? &mStartLoc : &m_JoinFestaSwitchFieldData.mStartLoc; 
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief      特殊セーブ復帰　ロケーション　設定・取得
   */
  //-----------------------------------------------------------------------------
  inline void SetSpecialLocation( const Field::Location* cp_loc ) 
  {
    if( !m_JoinFestaSwitchFieldData.mIsJoinFesta )
    {
      mSpecialLoc = *cp_loc; 
    }
    else
    {
      m_JoinFestaSwitchFieldData.mSpecialLoc = *cp_loc;
    }
  }

  inline const Field::Location* GetSpecialLocation( void ) const 
  {
    return !m_JoinFestaSwitchFieldData.mIsJoinFesta ? &mSpecialLoc : &m_JoinFestaSwitchFieldData.mSpecialLoc; 
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief      脱出先　ロケーション　設定・取得
   */
  //-----------------------------------------------------------------------------
  inline void SetEscapeLocation( const Field::Location* cp_loc ,u32 FormNo ) 
  {
    if( !m_JoinFestaSwitchFieldData.mIsJoinFesta )
    {
      mEscapeLoc = *cp_loc; 
      mEscapeLoc.formNo = FormNo;
    }
    else
    {
      m_JoinFestaSwitchFieldData.mEscapeLoc = *cp_loc;
      m_JoinFestaSwitchFieldData.mEscapeLoc.formNo = FormNo;
    }
  }
  inline const Field::Location* GetEscapeLocation( void ) const 
  {
    return !m_JoinFestaSwitchFieldData.mIsJoinFesta ? &mEscapeLoc : &m_JoinFestaSwitchFieldData.mEscapeLoc; 
  }

  //----------------------------------------------------------------------------
  /**
  *  @brief      カメラ保存情報　設定・取得
  */
  //-----------------------------------------------------------------------------
  void SetFieldCameraData( const Field::Camera::SaveData& rData );
  void GetFieldCameraData( Field::Camera::SaveData* pData );
  void ClearFieldCameraData( void );

  //--------------------------------------------------------------
  /**
   * @brief   セーブ処理のロック設定
   * @retval  セーブ処理のロック状態  true時は、レポート画面でのセーブが行えない
   */
  //--------------------------------------------------------------
  bool IsSaveLock( void ) const {return mSaveLock;}

  //----------------------------------------------------------------------------
  /**
   *  @brief      ロード済み　取得
   */
  //-----------------------------------------------------------------------------
  inline bool IsSaveDataLoaded( void ) const { return mIsLoaded; }


  //----------------------------------------------------------------------------
  /**
   *	@brief  シチュエーションの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::Situation* GetSituation()
  {
    return !m_JoinFestaSwitchFieldData.mIsJoinFesta ? mSaveData->GetSituation() : &m_JoinFestaSwitchFieldData.mSituation; 
  }
  inline const Savedata::Situation* GetSituation( void ) const 
  {
    return !m_JoinFestaSwitchFieldData.mIsJoinFesta ? mSaveData->GetSituation() : &m_JoinFestaSwitchFieldData.mSituation; 
  }

  //----------------------------------------------------------------------------
  /**
   *	@brief  ワープIDの保存・取得
   */
  //-----------------------------------------------------------------------------
  void SetWarpID( int warp_id );
  int GetWarpID( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  現在のゾーンID取得(中身はStartLocationのゾーンを取得している)
   */
  //-----------------------------------------------------------------------------
  Field::ZoneID GetNowZoneID( void ) const;

  //----------------------------------------------------------------------------
  /**
   *  @brief      プレイヤーステータスの取得
   *  @return     マップマトリクスワーク
   */
  //-----------------------------------------------------------------------------
  inline Savedata::MyStatus* GetPlayerStatus( void ) { return mSaveData->GetMyStatus(); }
  inline const Savedata::MyStatus* GetPlayerStatusConst( void ) const { return mSaveData->GetMyStatus(); }
  inline const Savedata::MyStatus* GetPlayerStatus( void ) const { return mSaveData->GetMyStatus(); }

  //----------------------------------------------------------------------------
  /**
   *  @brief      プレイヤーパーティの取得
   *  @return     プレイヤーパーティデータ
   */
  //-----------------------------------------------------------------------------
  inline pml::PokeParty* GetPlayerParty( void ) { return mPlayerParty; }
  inline const pml::PokeParty* GetPlayerPartyConst( void ) const { return mPlayerParty; }
  inline const pml::PokeParty* GetPlayerParty( void ) const { return mPlayerParty; }


  //----------------------------------------------------------------------------
  /**
   *  @brief      QRパーティの取得
   *  @return     QRパーティデータ
   */
  //-----------------------------------------------------------------------------
  inline pml::PokeParty* GetQRParty( void ) { return mQRParty; }
  inline const pml::PokeParty* GetQRPartyConst( void ) const { return mQRParty; }
  inline const pml::PokeParty* GetQRParty( void ) const { return mQRParty; }
  gfl2::str::STRCODE* GetQRPartyName( void ) { return mQRPartyName; };

  inline u32 GetQRParam1( void ){ return mQRParam1; }
  inline void SetQRParam1( u32 param ){ mQRParam1 = param; }
  inline u32 GetQRParam2( void ){ return mQRParam2; }
  inline void SetQRParam2( u32 param ){ mQRParam2 = param; }
  inline void SetQRBattleTeamID( u8* pID ){ std::memcpy( m_QRBattleTeamID , pID , sizeof(m_QRBattleTeamID) ); }
  inline u8* GetQRBattleTeamID( void ){ return m_QRBattleTeamID; }


  //----------------------------------------------------------------------------
  /**
   *  @brief      対戦用署名の取得
   *  @return     署名データ
   */
  //-----------------------------------------------------------------------------
  inline u8* GetSignature( void ) { return mSignatureBuffer; }
  inline const u8* GetSignatureConst( void ) const { return mSignatureBuffer; }
  inline const u8* GetSignature( void ) const { return mSignatureBuffer; }

  //----------------------------------------------------------------------------
  /**
   *  @brief      イベントワークの取得
   *  @return     イベントーワークデータ
   */
  //-----------------------------------------------------------------------------
  inline Field::EventWork* GetEventWork( void ) { return mSaveData->GetEventWork(); }
  inline const Field::EventWork* GetEventWork( void ) const { return mSaveData->GetEventWork(); }

  //-----------------------------------------------------------------------------
  /**
   *  @brief      手持ちアイテムの取得
   *  @return     手持ちアイテムデータ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::MyItem* GetMyItem( void ) { return mSaveData->GetMyItem(); }
  inline const Savedata::MyItem* GetMyItem( void ) const { return mSaveData->GetMyItem(); }

  //-----------------------------------------------------------------------------
  /**
   *  @brief      GTSセーブデータの取得
   *  @return     GTSセーブデータ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::GtsData* GetGtsData( void ) { return mSaveData->GetGtsData(); }
  inline const Savedata::GtsData* GetGtsData( void ) const { return mSaveData->GetGtsData(); }

  //-----------------------------------------------------------------------------
  /**
   *  @brief      合体ポケモンのセーブデータを取得
   *  @return     合体ポケモンのセーブデータ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::UnionPokemon* GetUnionPokemon( void ) { return mSaveData->GetUnionPokemon(); }
  inline const Savedata::UnionPokemon* GetUnionPokemon( void ) const { return mSaveData->GetUnionPokemon(); }

  //-----------------------------------------------------------------------------
  /**
   *  @brief      きせかえセーブデータの取得
   *  @return     きせかえセーブデータ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::Fashion* GetFashion( void ) { return mSaveData->GetFashion(); }
  inline const Savedata::Fashion* GetFashion( void ) const { return mSaveData->GetFashion(); }

  //-----------------------------------------------------------------------------
  /**
   *  @brief      未分類セーブデータの取得
   *  @return     未分類セーブデータ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::Misc* GetMisc( void ) 
  {
    return !m_JoinFestaSwitchFieldData.mIsJoinFesta ? mSaveData->GetMisc() : &m_JoinFestaSwitchFieldData.mMisc; 
  }
  inline const Savedata::Misc* GetMisc( void ) const 
  {
    return !m_JoinFestaSwitchFieldData.mIsJoinFesta ? mSaveData->GetMisc() : &m_JoinFestaSwitchFieldData.mMisc; 
}

  //-----------------------------------------------------------------------------
  /**
   *  @brief      図鑑セーブデータの取得
   *  @return     図鑑セーブデータ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::ZukanData* GetZukanData( void ) { return mSaveData->GetZukanData(); }
  inline const Savedata::ZukanData* GetZukanData( void ) const { return mSaveData->GetZukanData(); }

  //-----------------------------------------------------------------------------
  /**
   *  @brief      フィールドメニューセーブデータの取得
   *  @return     フィールドメニューセーブデータ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::FieldMenu* GetFieldMenu( void ) { return mSaveData->GetFieldMenu(); }
  inline const Savedata::FieldMenu* GetFieldMenu( void ) const { return mSaveData->GetFieldMenu(); }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ゲーム内時間の取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::GameTime* GetGameTime( void ) { return mSaveData->GetGameTime(); }
  inline const Savedata::GameTime* GetGameTime( void ) const { return mSaveData->GetGameTime(); }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    コンフィグデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::ConfigSave* GetConfig( void ) { return mSaveData->GetConfig(); };
  inline const Savedata::ConfigSave* GetConfig( void ) const { return mSaveData->GetConfig(); };

  //-----------------------------------------------------------------------------
  /**
   *  @brief    BOXデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::BOX* GetBOX( void ) { return mSaveData->GetBOX(); };
  inline const Savedata::BOX* GetBOX( void ) const { return mSaveData->GetBOX(); };
  //-----------------------------------------------------------------------------
  /**
   *  @brief    BOXPokemonデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::BoxPokemon* GetBoxPokemon( void ) { return mSaveData->GetBoxPokemon(); };
  inline const Savedata::BoxPokemon* GetBoxPokemon( void ) const { return mSaveData->GetBoxPokemon(); };

  //-----------------------------------------------------------------------------
  /**
   *  @brief    resortデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::ResortSave* GetResortSave( void ) { return mSaveData->GetResort(); };
  inline const Savedata::ResortSave* GetResortSave( void ) const { return mSaveData->GetResort(); };

  //-----------------------------------------------------------------------------
  /**
   *  @brief    プレイ時間セーブデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::PlayTime* GetPlayTime( void ) { return mPlayTime; };
  inline const Savedata::PlayTime* GetPlayTime( void ) const { return mPlayTime; };

  //-----------------------------------------------------------------------------
  /**
   *  @brief    動作モデルセーブデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::FieldMoveModelSave* GetFieldMoveModelSave( void ) 
  {
    return !m_JoinFestaSwitchFieldData.mIsJoinFesta ? mSaveData->GetFieldMoveModelSave() : &m_JoinFestaSwitchFieldData.mFieldMoveModelSave; 
  }
  inline const Savedata::FieldMoveModelSave* GetFieldMoveModelSave( void ) const 
  {
    return !m_JoinFestaSwitchFieldData.mIsJoinFesta ? mSaveData->GetFieldMoveModelSave() : &m_JoinFestaSwitchFieldData.mFieldMoveModelSave; 
  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ジョインフェスタVIPパーソナルデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::JoinFestaPersonalSave* GetJoinFestaVipData( void ) { return mSaveData->GetJoinFestaVipData(); }
  inline const Savedata::JoinFestaPersonalSave* GetJoinFestaVipData( void ) const { return mSaveData->GetJoinFestaVipData(); }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ジョインフェスタゲストパーソナルデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::JoinFestaPersonalSave* GetJoinFestaGuestData( void ) { return mSaveData->GetJoinFestaGuestData(); }
  inline const Savedata::JoinFestaPersonalSave* GetJoinFestaGuestData( void ) const { return mSaveData->GetJoinFestaGuestData(); }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ジョインフェスタデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::JoinFestaDataSave* GetJoinFestaData( void ) { return mSaveData->GetJoinFestaData(); }
  inline const Savedata::JoinFestaDataSave* GetJoinFestaData( void ) const { return mSaveData->GetJoinFestaData(); }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    フィールド上のきのみ状態データの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::BerrySpot* GetBerrySpotData( const b32 original = false ) 
  {
    return ( (!m_JoinFestaSwitchFieldData.mIsJoinFesta) || original ) ? mSaveData->GetBerrySpotData() : m_JoinFestaSwitchFieldData.mpBerrySpot; 
  }
  inline const Savedata::BerrySpot* GetBerrySpotData( const b32 original = false ) const 
  {
    return ( (!m_JoinFestaSwitchFieldData.mIsJoinFesta) || original ) ? mSaveData->GetBerrySpotData() : m_JoinFestaSwitchFieldData.mpBerrySpot; 
  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    フィールド上の釣りスポット状態データの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::FishingSpot* GetFishingSpotData(void) 
  {
    return !m_JoinFestaSwitchFieldData.mIsJoinFesta ? mSaveData->GetFishingSpotData() : m_JoinFestaSwitchFieldData.mpFishingSpot; 
  }
  inline const Savedata::FishingSpot* GetFishingSpotData(void) const 
  {
    return !m_JoinFestaSwitchFieldData.mIsJoinFesta ? mSaveData->GetFishingSpotData() : m_JoinFestaSwitchFieldData.mpFishingSpot; 
  }


  //-----------------------------------------------------------------------------
  /**
   *  @brief    ライブ大会データの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::LiveMatchData*       GetLiveMatchData(void)        {return mSaveData->GetLiveMatchData();}
  inline const Savedata::LiveMatchData* GetLiveMatchData(void) const  {return mSaveData->GetLiveMatchData();}

  //-----------------------------------------------------------------------------
  /**
   *  @brief    バトルスポットデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::BattleSpotData*       GetBattleSpotData(void)        {return mSaveData->GetBattleSpotData();}
  inline const Savedata::BattleSpotData* GetBattleSpotData(void) const  {return mSaveData->GetBattleSpotData();}

  //-----------------------------------------------------------------------------
  /**
   *  @brief    ポケファインダーデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::PokeFinderSave*       GetPokeFinderSave(void)        {return mSaveData->GetPokeFinderSave();}
  inline const Savedata::PokeFinderSave* GetPokeFinderSave(void) const  {return mSaveData->GetPokeFinderSave();}

  //-----------------------------------------------------------------------------
  /**
   *  @brief  レコードのセーブデータを取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::Record*        GetRecordSaveData( void )       { return mSaveData->GetRecordSaveData(); }
  inline const Savedata::Record*  GetRecordSaveData( void ) const { return mSaveData->GetRecordSaveData(); }
  //-----------------------------------------------------------------------------
  /**
   *  @brief    ポケモンダイアリーデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::PokeDiarySave*       GetPokeDiarySave(void)        {return mSaveData->GetPokeDiarySaveData();}
  inline const Savedata::PokeDiarySave* GetPokeDiarySave(void) const  {return mSaveData->GetPokeDiarySaveData();}
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief    不正チェックセーブの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::ValidationSave*       GetValidationSave(void)        {return mSaveData->GetValidationSaveData();}
  inline const Savedata::ValidationSave* GetValidationSave(void) const  {return mSaveData->GetValidationSaveData();}

  //----------------------------------------------------------------------------
  /**
   *  @brief    バトル施設セーブデータの取得
   *  @return   バトル施設セーブデータ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::BattleInstSave* GetBattleInstSave( void ) { return mSaveData->GetBattleInstSave(); }
  inline const Savedata::BattleInstSave* GetBattleInstSave( void ) const { return mSaveData->GetBattleInstSave(); }

  //----------------------------------------------------------------------------
  /**
   *  @brief    育て屋セーブデータの取得
   *  @return   育て屋セーブデータ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::Sodateya* GetSodateyaSave( void ) { return mSaveData->GetSodateyaSave(); }
  inline const Savedata::Sodateya* GetSodateyaSave( void ) const { return mSaveData->GetSodateyaSave(); }

  //----------------------------------------------------------------------------
  /**
   *  @brief    天候セーブデータの取得
   *  @return   天候セーブデータ
   */
  //-----------------------------------------------------------------------------
  inline Savedata::WeatherSave* GetWeatherSave( void ) { return mSaveData->GetWeatherSave(); }
  inline const Savedata::WeatherSave* GetWeatherSave( void ) const { return mSaveData->GetWeatherSave(); }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    QRReaderセーブデータの取得
   */
  //-----------------------------------------------------------------------------
  inline Savedata::QRReaderSaveData*        GetQRReaderSave(void)       {return mSaveData->GetQRReaderSave();}
  inline const Savedata::QRReaderSaveData*  GetQRReaderSave(void) const {return mSaveData->GetQRReaderSave();}

  //-----------------------------------------------------------------------------
  /**
  *  @brief    ファインダースタジオ用セーブデータの取得
  */
  //-----------------------------------------------------------------------------
  inline Savedata::FinderStudioSave*        GetFinderStudioSave( void )       { return mSaveData->GetFinderStudioSave(); }
  inline const Savedata::FinderStudioSave*  GetFinderStudioSave( void ) const { return mSaveData->GetFinderStudioSave(); }


  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------
  inline u32 GetRealRegionFormNo( void ) const{ return mSaveData->GetMisc()->GetRealRegionFormNo();}

  //============================================================================================
  //============================================================================================

  //-----------------------------------------------------------------------------
  /**
   *  @brief      ゲーム中に保持し続けるバッグ画面のデータを取得
   */
  //-----------------------------------------------------------------------------
  inline app::bag::PlayData* GetBagPlayData( void ){ return mBagPlayData; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief      ゲーム中に保持し続けるフィールド下画面のデータを取得
   */
  //-----------------------------------------------------------------------------
  inline App::FieldMenu::PlayData* GetFieldMenuPlayData( void ){ return mFieldMenuPlayData; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief      ゲーム中に保持し続けるトレーナーパス画面のデータを取得
   */
  //-----------------------------------------------------------------------------
  inline App::TrainerPass::PlayData* GetTrainerPassPlayData( void ){ return mTrainerPassPlayData; }

  //============================================================================================
  //  動作モデル用アクセサ
  //============================================================================================

  //-----------------------------------------------------------------------------
  /**
   * @brief 動作モデルマネージャーを取得
   */
  //-----------------------------------------------------------------------------
  inline Field::MoveModel::FieldMoveModelManager* GetFieldCharaModelManager( void ) const { return m_pFieldCharaModelManager; }

  //-----------------------------------------------------------------------------
  /**
   * @brief 動作コードマネージャーを取得
   */
  //-----------------------------------------------------------------------------
  inline Field::MoveModel::FieldMoveCodeManager* GetFieldMoveCodeManager( void ) const { return m_pFieldMoveCodeManager; }

  //-----------------------------------------------------------------------------
  /**
   * @brief フィールド用ノードルートを取得
   */
  //-----------------------------------------------------------------------------
  inline Field::FieldRootNode* GetFieldRootNode( void ) const { return m_pFieldRootNode; }

  //-----------------------------------------------------------------------------
  /**
   * @brief エンカウント制御ワークを取得
   */
  //-----------------------------------------------------------------------------
  inline Field::Encount::EncountWork* GetEncountWork( void ){ return m_pEncountWork; }

  //----------------------------------------------------------------------------
  /**
   *	@brief  最後のバトル結果を取得
   */
  //-----------------------------------------------------------------------------
  inline BtlResult GetLastBattleResult( void ) const { return m_LastBattleResultData.result; }
  inline void SetLastBattleResult( BtlResult result ) { m_LastBattleResultData.result = result; }
  inline BATTLE_RESULT_DATA* GetLastBattleResultData( void ) { return &m_LastBattleResultData; }
  inline const BATTLE_RESULT_DATA* GetLastBattleResultData( void ) const { return &m_LastBattleResultData; }
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief      育て屋用歩数カウンター
   */
  //-----------------------------------------------------------------------------
  ///育て屋用歩数カウンター取得
  inline u32 SodateyaWalkCounterGet( void ){ return m_pSodateyaWalkCheckData->mSodateyaWalkCount; }
  ///育て屋用歩数カウンターリセット
  inline void SodateyaWalkCounterReset( void ){ m_pSodateyaWalkCheckData->mSodateyaWalkCount = 0; }
  ///育て屋用歩数カウンターインクリメント
  inline u32 SodateyaWalkCounterInc( void ){
    if( m_pSodateyaWalkCheckData->mSodateyaWalkCount < 0xFFFFFFFF ) ++m_pSodateyaWalkCheckData->mSodateyaWalkCount;
    return m_pSodateyaWalkCheckData->mSodateyaWalkCount;
  }
 
  //============================================================================================
  //  往来用アクセサ
  //============================================================================================
  //-----------------------------------------------------------------------------
  /**
   * @brief 往来マネージャーを取得
   */
  //-----------------------------------------------------------------------------
  inline void SetFieldTrafficManager( Field::TrafficManager * pTrafficManager )
  {
    m_pTrafficManager = pTrafficManager; 
  }

  //-----------------------------------------------------------------------------
  /**
   * @brief 往来マネージャーを取得
   */
  //-----------------------------------------------------------------------------
  inline Field::TrafficManager* GetFieldTrafficManager( const b32 origin = false ) const 
  {
    return ((!IsNowJoinFestaFieldData_Manager()) || origin) ?  m_pTrafficManager : m_JoinFestaSwitchFieldData.m_pTrafficManager; 
  }

  // ドロップアイテムマネージャを取得
  inline Field::FieldItemManager* GetFieldItemManager( void ) const 
  {
    return !IsNowJoinFestaFieldData_Manager() ?  m_pFieldItemManager : m_JoinFestaSwitchFieldData.m_pFieldItemManager; 
  }

  inline Field::FieldBerryManager* GetFieldBerryManager( const b32 original = false ) const
  {
    return ((!IsNowJoinFestaFieldData_Manager()) || original) ?  m_pFieldBerryManager : m_JoinFestaSwitchFieldData.m_pFieldBerryManager; 
  }

  /**
   *  @brief  ファインダースポットデータを取得
   */
  inline Field::TrialModel::FinderSpotReserveData *GetFinderSpotReserveData( u32 idx )
  {
    if( idx < Savedata::PokeFinderSave::MAX_POKE_FINDER_SPOT )
    {
      return m_pFinderSpotReserveData[idx];
    }

    return NULL;
  }

  /**
   *  @brief  スクリプト関連常駐データを取得
   */
  inline Field::FieldScript::GameData* GetFieldScriptGameData()
  {
    return m_pFieldScriptData;
  }

  /**
  *  @biref バトルフェスマネジャー取得
  */
  inline BattleFes::BattleFesManager* GetBattleFesManager() { return m_pBattleFesManager; }
  inline const BattleFes::BattleFesManager* GetBattleFesManager() const { return m_pBattleFesManager; }

  /**
  *  @biref バトルフェスセーブデータ取得
  */
  inline Savedata::BattleFesSave* GetBattleFesSave(bool isEvent) 
  {
    return (isEvent) ? m_pBattleFesManager->GetBattleFesSave() : mSaveData->GetBattleFesSave();
  }
  inline const Savedata::BattleFesSave* GetBattleFesSave(bool isEvent) const
  {
    return (isEvent) ? m_pBattleFesManager->GetBattleFesSave() : mSaveData->GetBattleFesSave();
  }

private:

  //-----------------------------------------------------------------------------
  /**
   * @brief　ジョインフェスタで2重化しているセーブ領域の反映させるべきものをオリジナルに下記戻す。
   */
  //-----------------------------------------------------------------------------
  void serializeJoinFestaFieldData( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief さいしょからはじめる、ときのバトル施設データ初期化
   */
  //-----------------------------------------------------------------------------
  void setFirstDataBattleInst( void );

public:
  //--------------------------------------------------------------
  /**
   * @brief   セーブ処理（電源切断時用）
   * @retval  true  セーブ処理終了
   * @retval  false セーブ処理継続中
   *
   * @note    セーブの状態にあわせてなるべく適切に終えられるように分岐する
   */
  //--------------------------------------------------------------
  bool FinalizeSaveForShutdown( void );

  //--------------------------------------------------------------
  /**
  * @brief   ゲーム起動後、リザードンライドのロング版デモを見たか
  * @retval  true  見た
  * @retval  false 見てない
  */
  //--------------------------------------------------------------
  bool CheckFlyDemoShowed(void) { return m_FlyDemoShowed; }

  //--------------------------------------------------------------
  /**
  * @brief   ゲーム起動後、リゾート移動のロング版デモを見たか
  * @retval  true  見た
  * @retval  false 見てない
  */
  //--------------------------------------------------------------
  bool CheckResortDemoShowed(void) { return m_ResortDemoShowed; }

  //--------------------------------------------------------------
  /**
  * @brief   リザードンライドのロング版デモを見たフラグをセットする
  */
  //--------------------------------------------------------------
  void SetFlyDemoShowed(void) { m_FlyDemoShowed = true; }

  //--------------------------------------------------------------
  /**
  * @brief   リゾート移動のロング版デモを見たフラグをセットする
  */
  //--------------------------------------------------------------
  void SetResortDemoShowed(void) { m_ResortDemoShowed = true; }

  void SetSurfScore(int score) { m_SurfRecord = score; }
  int GetSurfScore(void) { return m_SurfRecord; }

  void SetLunaSolDiveDistance(u32 distance) { m_LunaSolDistance = distance; }
  u32 GetLunaSolDiveDistance(void) { return m_LunaSolDistance; }

  void SetLunaSolDiveMonsno(u32 monsno) { m_LunaSolMonsno = monsno; }
  u32 GetLunaSolDiveMonsno(void) { return m_LunaSolMonsno; }
  void SetSkyboxDarkFlag(bool flg) { m_SkyboxDarkFlag = flg; }//@fix MMCat[379]:天候「暗黒」時のポニのビーチ、ナッシー・アイランドの状態について >>　Mcatコメント内有るaの案件対処
  bool GetSkyboxDarkFlag(void) { return m_SkyboxDarkFlag; }
  
};  //GameData

GFL_NAMESPACE_END(GameSys)



#endif //__GAMEDATA_H__
