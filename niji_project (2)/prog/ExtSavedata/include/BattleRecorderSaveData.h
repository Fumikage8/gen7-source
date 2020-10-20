//============================================================================================
/**
 * @file    BattleRecorderSaveData.h
 * @brief   バトルレコーダーセーブデータ
 * @author  sogabe
 * @date    12.09.20
 */
//============================================================================================
#if !defined(__BATTLE_RECORDER_SAVE_DATA_H__)
#define __BATTLE_RECORDER_SAVE_DATA_H__


#include "Battle/include/battle_SetupParam.h"
#include "Savedata/include/ConfigSave.h"
#include "Savedata/include/Mystatus.h"
#include <math/include/gfl2_math_random.h>
#include <pml/include/pml_PokeParty.h>
#include <heap/include/gfl2_heap_base.h>
#include <GameSys/include/GameManager.h>
#include <system/include/Date/gfl2_Date.h>
#include <fs/include/CTR/gfl2_FsCtrExtSaveFile.h>
#include "ExtSavedata/include/NijiExtSaveSystem.h"
#include "ExtSavedata/include/NijiExtSaveData.h"

#include "System/include/GameStrLen.h"
#include "Battle/Regulation/include/Regulation.h"
#include "Savedata/include/ConfigSave.h"

namespace ExtSavedata {

  /**
   * @brief   バトルレコーダーセーブデータ
   */
  class BattleRecorderSaveData
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleRecorderSaveData );

    public:
    /**
     * @brief セーブ結果
     */
    enum  Result
    {
      RESULT_SUCCESS = 0,           ///<セーブ成功
      RESULT_SUCCESS_AND_FULL,      ///<セーブ成功したが、セーブ件数が満杯になった
      RESULT_FAILED_FULL,           ///<セーブ件数が満杯で失敗
      RESULT_FAILED_SD_FULL,        ///<SDカードが満杯で失敗
      RESULT_FAILED_NO_EXIST,       ///<ファイルが存在しない
      RESULT_FAILED_ALREADY_EXIST,  ///<ファイルがすでに存在する
      RESULT_FAILED_DATA_BROKEN,    ///<データ破壊
      RESULT_FAILED_FATAL_ERROR,    ///<ゲーム復帰不可能なエラー
      RESULT_FAILED_WAIT_ASYNC,     ///<非同期待ち中
      RESULT_FAILED_NO_ASYNC,       ///<同期処理なのに非同期待ちをした
      RESULT_FAILED,                ///<その他の失敗
      RESULT_WAIT_ASYNC,            ///<非同期終了待ち
    };

    public:

    static  const s32 NONE_FILE_INDEX = -1;   ///<ファイルインデックス無し
    static  const s32 NONE_FILE_COUNT = -1;   ///<ファイルをカウントしていない状態
    static  const s32 MAX_SAVE_DATA = 100;    ///<何件までセーブできるか
    static  const s32 LIVE_SAVE_INDEX = 100;  ///<ライブ大会専用のセーブインデックス
    static  const u16 REC_MAGIC_KEY = 0xe281; ///<マジックキー
    static  const u32 SD_WAIT_COUNT = 30;
    static  const s32 PP_HALF = pml::PokeParty::MAX_MEMBERS / 2;  ///<手持ちの半分
    static  const s32 PP_FULL = pml::PokeParty::MAX_MEMBERS;      ///<手持ちすべて

    typedef tinymt32_t                           RandomContext;

    //BATTLE_SETUP_PARAMにセットするために必要な最低限の要素を抽出 ※BATTLE_SETUP_PARAMの更新に追随する必要がある
    /**
     *  @brief  BATTLE_SETUP_PARAMにセットするために必要な最低限の要素
     */
    struct  BTLREC_SETUP_SUBSET
    {
      u32                         recRandSeed;      ///<ランダムの種
      BTL_FIELD_SITUATION         fieldSituation;   ///<画面構築用情報
      BSP_BATTLE_EFFECT           battleEffect;     ///<バトル演出パラメータ
      u8                          config[ Savedata::ConfigSave::SerialDataSize ]; ///<コンフィグ
      SHOOTER_ITEM_BIT_WORK       shooterBitWork;   ///<シューター使用制限パラメータ
      BtlCompetitor               competitor;       ///<バトルコンペチター
      BtlRule                     rule;             ///<バトルルール
      BtlMultiMode                multiMode;        ///<マルチモード
      bool                        skyBattle;        ///<スカイバトルかどうか（true:スカイバトル）
      bool                        sakasaBattle;     ///<さかさバトルかどうか（true:さかさバトル）
      u16                         debugFlag;        ///<デバッグ用フラグ
      u8                          commPos;          ///<通信対戦時の立ち位置
      u8                          btlFesFieldType;  ///<バトルフェス専用フィールド効果
    };

    /**
     *  @brief  クライアント操作内容保存バッファ
     */
    struct BTLREC_OPERATION_BUFFER
    {
      u32 size;                                   ///<保存サイズ
      u8  buffer[ BTLREC_OPERATION_BUFFER_SIZE ]; ///<保存バッファ
    };

    /**
     *  @brief  録画用PokeParty
     */
    struct  BTLREC_POKEPARTY
    {
      bool  is_exist;                         ///<存在するかどうか（true:存在）
      pml::PokeParty::SERIALIZE_DATA members; ///<PokePartyデータ
    };

    /**
     *  @brief  録画用MyStatus
     */
    struct  BTLREC_MYSTATUS
    {
      bool  is_exist;                                           ///<存在するかどうか（true:存在）
      u8    status[ Savedata::MyStatus::SERIALIZE_DATA_SIZE ];  ///<MyStatusデータ
    };

    /**
     *  @brief  録画セーブデータ本体
     */
    struct  BATTLE_REC_BODY
    {
      BTLREC_SETUP_SUBSET     setupSubset;  ///<BATTLE_SETUP_PARAMにセットするために必要な最低限の要素
      BTLREC_OPERATION_BUFFER opBuffer;     ///<クライアント操作内容保存バッファ

      BTLREC_POKEPARTY        rec_party[ BTL_CLIENT_NUM ];        ///<録画用PokeParty
      BTLREC_MYSTATUS         myStatus[ BTL_CLIENT_NUM ];         ///<録画用MyStatus
      BSP_TRAINER_DATA::SERIALIZE_DATA tr_data[ BTL_CLIENT_NUM ]; ///<録画用TrData

      u16                     magic_key;  ///<マジックキー
      u16                     padding;    ///<パディング

      u32                     crc;        ///<CRC
    };


    public:
    /**
     *  @brief  録画セーブデータヘッダー
     */
    struct  BATTLE_REC_HEADER
    {
      u8                category;   ///<対戦施設種別 regulation::Regulation::CATEGORY_TYPE
      u8                rule;       ///<対戦モード   regulation::Regulation::BATTLE_TYPE
      u16               magic_key;  ///<マジックキー

      bool              sakasaBattle; ///<さかさバトルかどうか（true:さかさバトル）

      gfl2::str::STRCODE cupname1[ Regulation::WIFI_PLAYER_TIX_CUPNAME_MOJINUM + System::EOM_LEN ];  ///<大会名１
      gfl2::str::STRCODE cupname2[ Regulation::WIFI_PLAYER_TIX_CUPNAME_MOJINUM + System::EOM_LEN ];  ///<大会名２
      Savedata::MyStatus::ICON_DATA   pssIcon[ BTL_CLIENT_NUM ];                              ///<録画用プレイヤーICONデータ
      gfl2::str::STRCODE name[ BTL_CLIENT_NUM ][ pml::PERSON_NAME_BUFFER_SIZE ]; ///<プレーヤー名
      u16               mons_no[ pml::PokeParty::MAX_MEMBERS * 2 ];             ///<モンスターナンバー
      u8                form_no[ pml::PokeParty::MAX_MEMBERS * 2 ];             ///<フォルムナンバー
      u8                gender[ pml::PokeParty::MAX_MEMBERS * 2 ];              ///<性別
      bool              rare[ pml::PokeParty::MAX_MEMBERS * 2 ];                ///<レアフラグ

      u16               battle_counter;       ///<連勝数
      u16               end_turn_num;         ///<バトルビデオが終了した際の経過ターン数
      u8                server_version;       ///<バトルサーババージョン
      u8                background;           ///<背景タイプ regulation::Regulation::BACKGROUND_TYPE

      u8                patch_version[3];     ///<パッチのバージョン
      u8                battle_result1;       ///<対戦結果１ BtlRecordResult1
      u8                battle_result2;       ///<対戦結果２ BtlRecordResult2

      u8                royal_result;        ///<バトルロイヤルの順位
      u8                trainer_id[ 4 ];   ///<プレイヤーアイコン用のトレーナーID

      u32               crc;                  ///<CRC
    };

    /**
     *  @brief  サーバにアップロードするデータ
     */
    struct BATTLE_REC_UPLOAD_DATA
    {
      BATTLE_REC_HEADER header; ///<録画セーブデータヘッダー
      BATTLE_REC_BODY   body;   ///<録画セーブデータ本体
    };

    /**
     *  @brief  時間情報
     */
    struct REC_DATE{
      u16   year;     ///< 作成日時：年
      u8    month;    ///< 作成日時：月
      u8    day;      ///< 作成日時：日
      u8    hour;     ///< 作成日時：時
      u8    minute;   ///< 作成日時：分
      u8    second;   ///< 作成日時：秒
      u8    pad;      ///< パディング

      // コンストラクタ初期化
      REC_DATE():
      year(2000),month(1),day(1),
      hour(0),minute(0),second(0)
      {}
    };

    /**
     *  @brief  SDカードにセーブするデータ
     */
    struct BATTLE_REC_DATA
    {
      BATTLE_REC_UPLOAD_DATA  upload;       ///<サーバにアップロードするデータ
      bool                    upload_flag;  ///<アップロードフラグ
      u32                     playCount;    ///<再生回数
      u64                     dataID;       ///<データID
      REC_DATE                createDate;   ///<作成日時
      REC_DATE                registDate;   ///<登録日時
    };
    
    private:
    gfl2::heap::HeapBase*      mHeap;                ///<ヒープ
    GameSys::GameManager*  mGameManager;         ///<ゲームマネージャ
    BATTLE_REC_DATA*          mBattleRecData;       ///<1件分のバトルレコーダーデータ
    bool                      mIsLoadData;          ///<バトルレコーダーデータを読み込んだかどうか
    s32                       mFileIndex;           ///<SDカード上のファイルインデックス
    s32                       mFileCount;           ///<有効なセーブデータの数
    bool                      mIsAsync;             ///<非同期待ちセーブしているか
    bool                      mReadContinue;        ///<妥当性チェック継続版かどうか
    bool                      mContinueResult;      ///<妥当性チェック継続版返り値
    s32                       mContinueCount;       ///<妥当性チェック継続版カウント数
    s32                       mContinueIndex;       ///<妥当性チェック継続版インデックス
    u32                       m_waitCount;

    public:
    //--------------------------------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     *
     * @param[in] heap    ヒープ
     * @param[in] gameMgr ゲームマネージャ
     */
    //--------------------------------------------------------------------------------------------
    BattleRecorderSaveData( gfl2::heap::HeapBase* heap, GameSys::GameManager* gameMgr );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   デストラクタ
     */
    //--------------------------------------------------------------------------------------------
    ~BattleRecorderSaveData();

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   BattleSetupParamとRegulationからバトルレコーダーセーブデータを生成
     *
     * @param[in] bsp BattleSetupParam
     * @param[in] reg Regulation
     *
     * @retval  true:生成成功　false:生成失敗
     */
    //--------------------------------------------------------------------------------------------
    bool  CreateSaveData( const BATTLE_SETUP_PARAM* bsp, const Regulation* reg );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   SDカードからバトルレコーダーセーブデータをロード
     *
     * @param[in] index  ファイルインデックス
     *
     * @retval  ExtSavedata::BattleRecorderSaveData::Result
     */
    //--------------------------------------------------------------------------------------------
    ExtSavedata::BattleRecorderSaveData::Result LoadData( int index );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   SDカードからライブ大会用セーブデータをロード
     *
     * @retval  ExtSavedata::BattleRecorderSaveData::Result
     */
    //--------------------------------------------------------------------------------------------
    ExtSavedata::BattleRecorderSaveData::Result LiveCupLoadData( void );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   SDカードにバトルレコーダーセーブデータをセーブ
     *          空いているインデックスをサーチしてセーブ
     *
     * @param[in] is_async  非同期セーブにするかどうか(true:非同期 false:同期)
     *
     * @retval  ExtSavedata::BattleRecorderSaveData::Result
     */
    //--------------------------------------------------------------------------------------------
    ExtSavedata::BattleRecorderSaveData::Result SaveData( bool is_async = false );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   SDカードにバトルレコーダーセーブデータをセーブ
     *
     * @param[in] index     ファイルインデックス
     * @param[in] is_async  非同期セーブにするかどうか(true:非同期 false:同期)
     *
     * @retval  ExtSavedata::BattleRecorderSaveData::Result
     */
    //--------------------------------------------------------------------------------------------
    ExtSavedata::BattleRecorderSaveData::Result SaveData( int index, bool is_async = false );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   非同期処理待ち関数
     *
     * @retval  ExtSavedata::BattleRecorderSaveData::Result
     */
    //--------------------------------------------------------------------------------------------
    ExtSavedata::BattleRecorderSaveData::Result WaitAsync( bool isDummyWaitSkip = false );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   SDカードにライブ大会専用セーブデータをセーブ
     *
     * @retval  ExtSavedata::BattleRecorderSaveData::Result
     * 
     * @li  ライブ大会専用です
     */
    //--------------------------------------------------------------------------------------------
    ExtSavedata::BattleRecorderSaveData::Result LiveCupSaveData( void );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   SDカードからバトルレコーダーセーブデータを削除
     *
     * @param[in] index  ファイルインデックス
     *
     * @retval  ExtSavedata::BattleRecorderSaveData::Result
     */
    //--------------------------------------------------------------------------------------------
    ExtSavedata::BattleRecorderSaveData::Result DeleteData( int index );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   SDカードからライブ大会専用セーブデータを削除
     *
     * @retval  ExtSavedata::BattleRecorderSaveData::Result
     * 
     * @li  ライブ大会専用です
     */
    //--------------------------------------------------------------------------------------------
    ExtSavedata::BattleRecorderSaveData::Result LiveCupDeleteData( void );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   読み込んだバトルレコーダーデータからBattleSetupParamを生成
     *
     * @param[in] bsp   BattleSetupParam
     * @param[in] bsp   configData   ユーザーコンフィグ設定（NULLなら録画時の状態を採用する）
     *
     * @retval  正しく生成できたかどうか（true:生成できた　false：生成できない）
     */
    //--------------------------------------------------------------------------------------------
    bool  CreateBattleSetupParam( BATTLE_SETUP_PARAM* bsp, const Savedata::ConfigSave* configData=NULL );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   ビデオデータ取得
     *
     * @retval  ビデオデータ
     */
    //--------------------------------------------------------------------------------------------
    const ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA* GetBattleRecData( void ) const;
    ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA* GetBattleRecDataDirect( void );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   アップロードデータ取得
     *
     * @retval  アップロードデータ
     */
    //--------------------------------------------------------------------------------------------
    const ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATA* GetBattleRecUploadData( void ) const;

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   アップロードデータを設定
     *
     * @param   upload  アップロードデータ
     */
    //--------------------------------------------------------------------------------------------
    void SetBattleRecUploadData( BATTLE_REC_UPLOAD_DATA * upload );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   連勝数セット
     *
     * @param[in] value セットする連勝数
     */
    //--------------------------------------------------------------------------------------------
    void  SetBattleCounter( s32 value );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   バトルビデオIDセット
     *
     * @param[in] battle_video_id セットするバトルビデオID
     */
    //--------------------------------------------------------------------------------------------
    void  SetBattleVideoID( u64 battle_video_id );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   バトルビデオアップロードフラグセット
     *
     * @param[in] upload_flag セットするアップロードフラグ
     */
    //--------------------------------------------------------------------------------------------
    void  SetBattleVideoUploadFlag( bool upload_flag );

    //--------------------------------------------------------------------------
    /**
     * @brief  バトルビデオの再生回数をセット
     *
     * @param[in] count  再生回数
     */
    //--------------------------------------------------------------------------
    void SetBattleVideoPlayCount( const u32 count );

    //--------------------------------------------------------------------------
    /**
     * @brief   バトルビデオアップロード登録日をセット
     *
     * @param[in] date  日時
     */
    //--------------------------------------------------------------------------
    void SetBattleVideoRegistDate( const gfl2::system::Date* date );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   有効データの中の位置から、SDカード上での位置を取得する
     *
     * @param[in] pos 有効データの中の位置  0 <= pos < GetBattleVideoCount()
     *
     * @return  SDカード上での位置
     */
    //--------------------------------------------------------------------------------------------
    s32  GetBattleVideoIndex( s32 pos );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   バトルビデオセーブ件数取得
     *
     * @retval  バトルビデオセーブ件数
     */
    //--------------------------------------------------------------------------------------------
    s32  GetBattleVideoCount( void );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   バトルビデオセーブ件数取得
     *
     * @retval  バトルビデオセーブ件数
     */
    //--------------------------------------------------------------------------------------------
    s32 CountBattleVideo( void );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   アップロードしているバトルビデオセーブ件数取得
     *
     * @retval  アップロードしているバトルビデオセーブ件数
     */
    //--------------------------------------------------------------------------------------------
    s32  GetUploadBattleVideoCount( void );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   全セーブデータの妥当性チェック
     *
     * @param[in] heap  妥当性チェックするときにロードを介すので、現録画データの退避先確保用ヒープ
     *
     * @retval  true:正常 false:壊れたファイルが存在したので、消去した
     */
    //--------------------------------------------------------------------------------------------
    bool  CheckAllSaveDataValidity( gfl2::heap::HeapBase* heap = NULL );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   全セーブデータの妥当性チェック（指定した個数チェックしたらリターンする版）
     *
     * @param[in]   heap    妥当性チェックするときにロードを介すので、現録画データの退避先確保用ヒープ
     * @param[out]  result  チェック結果を格納するワークのポインタ  true:正常 false:壊れたファイルが存在したので、消去した
     * @param[in]   count   チェックする個数  初回は個数を指定する  継続では無視されます
     * @param[in]   is_init 初回初期化はtrue  継続ではfalseを指定
     *
     * @retval  true:継続中 false:チェック終了
     */
    //--------------------------------------------------------------------------------------------
    bool  CheckAllSaveDataValidityContinue( gfl2::heap::HeapBase* heap, bool* result = NULL, s32 count = 0, bool is_init = false );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   ライブ大会のセーブデータの妥当性チェック
     *
     * @param[in] heap  妥当性チェックするときにロードを介すので、現録画データの退避先確保用ヒープ
     *
     * @retval  true:正常 false:壊れたファイルが存在したので、消去した
     */
    //--------------------------------------------------------------------------------------------
    bool  CheckLiveCupSaveDataValidity( gfl2::heap::HeapBase* heap = NULL );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   バトルビデオセーブ件数MAX取得
     *
     * @retval  バトルビデオセーブ件数MAX
     */
    //--------------------------------------------------------------------------------------------
    s32  GetMaxBattleVideoCount( void ) { return MAX_SAVE_DATA; }

#if PM_DEBUG
    //--------------------------------------------------------------------------------------------
    /**
     * @brief   読み込んだバトルレコーダーデータからBattleSetupParamを生成（デバッグ用）
     *
     * @param[in] bsp   BattleSetupParam
     * @param[in] heap  動的に確保するメモリの確保先ヒープ
     *
     * @retval  正しく生成できたかどうか（true:生成できた　false：生成できない）
     */
    //--------------------------------------------------------------------------------------------
    bool  CreateBattleSetupParamDebug( BATTLE_SETUP_PARAM* bsp, gfl2::heap::HeapBase* heap );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   バトルレコーダーデータを設定し、BattleSetupParamを生成（デバッグ用）
     *
     * @param[out] bsp     BattleSetupParam
     * @param[in] recData  使用するバトルレコーダーデータ
     * @param[in] heap     動的に確保するメモリの確保先ヒープ
     *
     * @retval  正しく生成できたかどうか（true:生成できた　false：生成できない）
     */
    //--------------------------------------------------------------------------------------------
    bool  CreateBattleSetupParamDebug( BATTLE_SETUP_PARAM* bsp, const BATTLE_REC_DATA* recData, gfl2::heap::HeapBase* heap );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   バトルレコーダーデータが存在するかチェック
     *
     * @param[in] index     ファイルインデックス
     *
     * @retval  true:存在する　false：存在しない
     */
    //--------------------------------------------------------------------------------------------
    bool  IsExistData( s32  index );
#endif

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   CreateBattleSetupParamで動的に確保したメモリを解放する
     *
     * @param[in] bsp   BattleSetupParam
     */
    //--------------------------------------------------------------------------------------------
    void  ClearBattleSetupParam( BATTLE_SETUP_PARAM* bsp );

    //--------------------------------------------------------------------------
    /**
     *  @brief  アップロードデータからMyStatusを取得
     *
     *  @param[in]  clientId  クライアント番号
     *  @param[in]  heap      ヒープ
     *
     *  @return MyStatus    ない場合はNULLが返る
     *
     *  @li       解放は各自で！
     */
    //--------------------------------------------------------------------------
    Savedata::MyStatus*  GetMyStatus( BTL_CLIENT_ID clientId, gfl2::heap::HeapBase* heap );

    //--------------------------------------------------------------------------
    /**
     *  @brief  アップロードデータからPokePartyを取得
     *
     *  @param[in]  clientId  クライアント番号
     *  @param[in]  heap      ヒープ
     *
     *  @return PokeParty    ない場合はNULLが返る
     *
     *  @li       解放は各自で！
     */
    //--------------------------------------------------------------------------
    pml::PokeParty*  GetPokeParty( BTL_CLIENT_ID clientId, gfl2::heap::HeapBase* heap );

    /**
     *  @brief  サーバにアップロードするデータサイズを取得
     *  
     *  @retval アップロードデータサイズ
     */
    u32 GetBattleRecUploadDataSize( void ) { return sizeof( BATTLE_REC_UPLOAD_DATA ); }
    /**
     *  @brief  SDカードにセーブするデータサイズを取得
     *
     *  @retval セーブするデータサイズ
     */
    u32 GetBattleRecDataSize( void ) { return sizeof( BATTLE_REC_DATA ); }
    /**
     *  @brief  データロードが済んでいるかどうか
     *
     *  @retval true:済んでいる false:済んでいない
     */
    bool IsLoadData( void ) { return mIsLoadData; }
    /**
     *  @brief  データロードフラグをOFF
     */
    void ResetLoadData( void ) { mIsLoadData = false; }

    private:
    //--------------------------------------------------------------------------------------------
    /**
     * @brief   セーブロード結果からバトルビデオ独自の結果を返す
     *
     * @param[in] result  gfl2::fs::ExtSaveFile::Result
     * @param[in] is_save セーブからよばれたかどうか（true:セーブから呼んだ）
     *
     * @retval  ExtSavedata::BattleRecorderSaveData::Result
     */
    //--------------------------------------------------------------------------------------------
    ExtSavedata::BattleRecorderSaveData::Result  GetSaveLoadResult( gfl2::fs::ExtSaveFile::Result result, bool is_save = false );

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   結果が成功かどうか？
     *
     * @retval  true:成功
     */
    //--------------------------------------------------------------------------------------------
    bool  IsSuccess( Result result ) { return ( ( result == RESULT_SUCCESS ) || ( result == RESULT_SUCCESS_AND_FULL ) ); }

  };


} //namespace extsavedata

#endif
