//============================================================================================
/**
  * @file    GTSSave.h
  * @brief   GTSセーブデータ
  * @author  Akito Mori
  * @date    12/07/10
  */
//============================================================================================
#if !defined( GTSSAVE_H_INCLUDE )
#define GTSSAVE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <pml/include/pml_PokePara.h>
#include <util/include/gfl2_std_string.h>
#include "Savedata/include/SaveDataInterface.h"
#include "NetApp/GTS/source/GTSTypes.h"


GFL_NAMESPACE_BEGIN(Savedata)

/**
  * @class GtsData
  * @brief GTSセーブデータ
  */
class GtsData : public SaveDataInterface
{
public:

  enum{
    SINGLE_PHRASE_MAX        = 16, ///< 一言メッセージ用MAX

    RETURN_POSITION_TEMOTI   = 0,  ///< ポケモンを戻す場所が手持ち
    RETURN_POSITION_TRAY1    = 1,  ///< ポケモンを戻す場所がボックストレイ１番
                                   ///< 以後ボックスの数だけ〜
    SINGLE_PHRASE_DATA_SIZE  = SINGLE_PHRASE_MAX+1, ///< 一言メッセージ用STRCODEサイズ
  };
  
  enum WANT_LEVEL {
    WANT_LV_NONE = 0, ///< レベル指定なし
    WANT_LV_01_10,    ///< レベル１〜１０
    WANT_LV_11_20,    ///< レベル１１〜２０
    WANT_LV_21_30,    ///< レベル２１〜３０
    WANT_LV_31_40,    ///< レベル３１〜４０
    WANT_LV_41_50,    ///< レベル４１〜５０
    WANT_LV_51_60,    ///< レベル５１〜６０
    WANT_LV_61_70,    ///< レベル６１〜７０
    WANT_LV_71_80,    ///< レベル７１〜８０
    WANT_LV_81_90,    ///< レベル８１〜９０
    WANT_LV_91_100,   ///< レベル９１〜１００
  };
  
  /// コンストラクタ
  GtsData(void);
  /// デストラクタ
  ~GtsData(void);


private:
  /// GTSセーブデータ
  struct Data {
    NetApp::GTS::GtsSerializePokemonParam backupPokemon;         ///< GTSポケモン退避領域 232byte
    gfl2::str::STRCODE SinglePhrase[ SINGLE_PHRASE_DATA_SIZE ];   ///< 一言メッセージ保存領域
    gfl2::str::STRCODE WantPokeName[ pml::MONS_NAME_BUFFER_SIZE ];///< 欲しいポケモン名
    b8      DepositFlag;  ///< 預けてるかフラグ true:預けてる, false:預けてない
    u8      toBoxTray;    ///< 最後に見ていたBOXトレイ
    u8      toBoxPos;     ///< 最後に見ていたBOXトレイ内の位置
    u16     WantPoke;     ///< 欲しいポケモン種族NO
    u8      WantSex;      ///< 欲しいポケモンの性別(
    u8      WantLevel;    ///< 欲しいポケモンのレベル
    u64     uploadId;     ///< アップロード時に取得するID(ユニークIDと同じならなくします）
    NetApp::GTS::GtsSerializeRecordKey UploadKey; ///< シリアライズされたアップロードキー
    NetApp::GTS::GtsIndexData uploadGtsIndexData; ///< アップロードしたGTSインデックスデータ
  };

  Data mGtsData;  ///< GTSセーブデータ本体

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief    読み込んだデータをセットする関数
   * @param    pData   先頭のポインタ
   */
  //-----------------------------------------------------------------------------
  virtual void SetData(void* pData){ gfl2::std::MemCopy(pData, reinterpret_cast<void*>(&mGtsData), sizeof(Data)); };
  //-----------------------------------------------------------------------------
  /**
   * @brief    データの先頭ポインタを得る関数
   * @return   先頭のポインタ
   */
  //-----------------------------------------------------------------------------
  virtual void* GetData(void){ return reinterpret_cast<void*>(&mGtsData); };
  //-----------------------------------------------------------------------------
  /**
   * @brief    データサイズ
   * @retval   バイト
   */
  //-----------------------------------------------------------------------------
  virtual size_t GetDataSize(void){ return sizeof(Data); };

  //--------------------------------------------------------------------------------------------
  /**
   * @brief セーブデータクリア処理
   * @param heap
   */
  //--------------------------------------------------------------------------------------------
  void Clear( gfl2::heap::HeapBase * heap );


  //--------------------------------------------------------------------------------------------
  /**
   * @brief  退避ポケモンデータ取得
   * @param  [out] セーブデータからポケモンデータを受け取るバッファ
   */
  //--------------------------------------------------------------------------------------------
  void GetBackupPokemonCoreParam( pml::pokepara::CoreParam *coreParam );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief  退避ポケモンデータセット
   * @param  [in]coreParam セーブデータに入れるポケモンデータ
   * @param  [in]tray      ポケモンがいたトレイNO（RETURN_POSITION_TEMOTI=0, trayNoは1オリジン)
   * @param  [in]pos       ポケモンがいたトレイ内の位置
   * @param  [in]poke      欲しいポケモンの種族NO
   * @param  [in]sex       欲しいポケモンの性別
   * @param  [in]level     欲しいポケモンのレベル
   */
  //--------------------------------------------------------------------------------------------
  void SetBackupPokemonCoreParam( const pml::pokepara::CoreParam *coreParam, u8 tray, u8 pos, MonsNo poke, int sex, int level );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief  ポケモンを戻す場所の取得
   * @retval RETURN_POSITION_TEMOTI = てもち、  以降1オリジンでBOXトレイのNO
   */
  //--------------------------------------------------------------------------------------------
  int GetReturnTrayNo( void );
  int GetReturnPosNo( void );
 
  //--------------------------------------------------------------------------------------------
  /**
   * @brief  欲しいポケモンの種族NO
   * @retval MonsNo
   */
  //--------------------------------------------------------------------------------------------
  MonsNo GetWantPoke(void);
  //--------------------------------------------------------------------------------------------
  /**
   * @brief  欲しいポケモンの性別
   * @retval enum WANT_SEX
   */
  //--------------------------------------------------------------------------------------------
  int    GetWantSex(void);
  //--------------------------------------------------------------------------------------------
  /**
   * @brief  欲しいポケモンのレベル
   * @retval enum WANT_LEVEL
   */
  //--------------------------------------------------------------------------------------------
  int    GetWantLevel(void);
  
  //--------------------------------------------------------------------------------------------
  /**
   * @brief  GTSに預けているか？取得
   * @param  none
   * @retval GTSに預けているか？  true:預けている, false:預けていない
   */
  //--------------------------------------------------------------------------------------------
  bool GetDepositFlag( void );
  //--------------------------------------------------------------------------------------------
  /**
   * @brief  GTSに預けているか？セット
   * @param  [in] true or false
   */
  //--------------------------------------------------------------------------------------------
  void SetDepositFlag( bool flag );
  
  //--------------------------------------------------------------------------------------------
  /**
   * @brief  一言メッセージを入力している？
   * @param  none
   * @retval 一言メッセージを入力している？  true:入力している, false:していない
   */
  //--------------------------------------------------------------------------------------------
  bool GetSinglePhraseExist( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief  一言メッセージ取得
   * @param  [out] セーブデータから一言メッセージをコピーするSTRBUF
   */
  //--------------------------------------------------------------------------------------------
  void GetSinglePhrase( gfl2::str::StrBuf *des );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief  一言メッセージをセーブデータにセット
   * @param  [in] 一言メッセージ
   */
  //--------------------------------------------------------------------------------------------
  void SetSinglePhrase( const gfl2::str::StrBuf *src );
  
  //--------------------------------------------------------------------------------------------
  /**
   * @brief  欲しいポケモン名取得
   * @param  [out] セーブデータからポケモン名をコピーするSTRBUF
   */
  //--------------------------------------------------------------------------------------------
  void GetWantPokeName( gfl2::str::StrBuf *des );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief  欲しいポケモン名をセーブデータにセット
   * @param  [in] ポケモン名
   */
  //--------------------------------------------------------------------------------------------
  void SetWantPokeName( const gfl2::str::StrBuf *src );

  
  //--------------------------------------------------------------------------------------------
  /**
   * @brief  アップロード結果IDを取得
   */
  //--------------------------------------------------------------------------------------------
  u64 GetUploadResultId(void);
  
  //--------------------------------------------------------------------------------------------
  /**
   * @brief  アップロード結果IDを格納
   * @param  [in] UPLODAD_RESULT_ID
   */
  //--------------------------------------------------------------------------------------------
  void SetUploadResultId( u64 dataId );
  
  const NetApp::GTS::GtsSerializeRecordKey& GetUploadKey() const;
  void SetUploadKey( const NetApp::GTS::GtsSerializeRecordKey& uploadKey );

  const NetApp::GTS::GtsIndexData& GetUploadGtsIndexData() const;
  void SetUploadGtsIndexData( const NetApp::GTS::GtsIndexData& uploadGtsIndexData );

};

GFL_NAMESPACE_END(Savedata)

#endif // GTSSAVE_H_INCLUDE
