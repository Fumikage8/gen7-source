//=============================================================================
/**
 * @brief  ポケモンライブラリ システム
 * @file   pml_Library.h
 * @author obata_toshhiro
 * @date   2011.01.12
 */
//=============================================================================
#if !defined( __PML_LIBRARY_H__ )
#define __PML_LIBRARY_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>

#include <pml/include/pml_Personal.h>
#include <pml/include/pml_PokePara.h>
#include <pml/include/pml_Waza.h>

namespace pml {


  typedef u32 (RandFunc)(void);
  typedef u32 (LimitRandFunc)(u32 limit);

  //------------------------------------------------------------------------
  /**
   * @brief ポケモンライブラリ管理システム
   *
   * ポケモンライブラリを使用する際にはこのクラスを生成し、Initializeする必要がある。
   */
  //------------------------------------------------------------------------
  class Library 
  { 
    public:

      //------------------------------------------------------------------------
      /// ポケモンライブラリの外部設定
      //------------------------------------------------------------------------
      struct ExternalSetting
      {
        // パーソナル関連アーカイブのパス
        const char* garcPath_PersonalData;       ///< パーソナルデータへのパス
        const char* garcPath_WazaOboeData;       ///< 技覚えデータへのパス
        const char* garcPath_GrowTable;          ///< 成長曲線データへのパス
        const char* garcPath_EvolutionTable;     ///< 進化テーブルへのパス
        const char* garcPath_MegaEvolveTable;    ///< メガ進化テーブルへのパス
        const char* garcPath_KowazaTable;        ///< 子技テーブルへのパス
        const char* garcPath_SeedMonsNoTable;    ///< 種ポケモンテーブルへのパス

        // パーソナル関連データのキャッシュサイズ
        u32 cacheSize_PersonalData;                 ///< パーソナルデータのキャッシュ保持数
        u32 cacheSize_GrowTable;                    ///< 成長テーブルのキャッシュ保持数
        u32 cacheSize_WazaOboeData;                 ///< 技覚えデータのキャッシュ保持数
        u32 cacheSize_EvolutionTable;               ///< 進化テーブルのキャッシュ保持数
        u32 cacheSize_MegaEvolveTable;              ///< メガ進化テーブルのキャッシュ保持数
        const WazaNo **bpWazaOshieTable;           ///< BP技教えのテーブル(4*32)

        // 技データ
        const char* garcPath_WazaData;           ///< 技データアーカイブのパス
        u32 cacheSize_WazaData;                     ///< 技データのキャッシュサイズ

        // アイテムデータ
        const char* garcPath_ItemData;           ///< アイテムデータアーカイブのパス
        const char* garcPath_FieldPocket;        ///< フィールドポケットデータのパス
        const char* garcPath_BattlePocket;       ///< バトルポケットデータのパス
        u32 garcDataID_BattlePocket;                ///< バトルポケットデータのアーカイブ内ID

        // メッセージデータ
        const char* garcPath_Message;            ///< メッセージアーカイブのパス
        const char* (get_garcPath_Message)(void);  ///< メッセージアーカイブのパスを取得する関数

        // メッセージアーカイブ内のデータID
        u32 msgDataID_MonsName;                     ///< 種族名MSGアーカイブID
   //     u32 msgDataID_TokuseiName;                  ///< 特性名MSGアーカイブID
   //     u32 msgDataID_TokuseiInfo;                  ///< 特性の説明文MSGアーカイブID
   //     u32 msgDataID_WazaName;                     ///< 技名MSGアーカイブID
    //    u32 msgDataID_WazaInfo;                     ///< 技の説明文MSGアーカイブID
        u32 msgDataID_ItemName;                     ///< 道具名MSGアーカイブID
        u32 msgDataID_ItemInfo;                     ///< 道具の説明文MSGアーカイブID

        /// ポケモンの進化マネージャ
        pml::pokepara::EvolveManager* evolve_manager; 
        
        
        u8  langId;                                 ///< 言語設定（DS世代以前ではcountry_codeとしていたので注意）
        u8  countryCode;                            ///< 国・地域コード(DS世代以前のcountry_codeではない。注意）
        u8  countryRegionCode;                      ///< 上記内の区分コード
        u8  hwRegionCode;                           ///< 3DSハードウェア本体の仕向地リージョンコード
        u8  cassetteVersion;                        ///< カセットバージョン
        u8  personalRegionID;                       ///< カセットバージョンに対応した地方種ID

        RandFunc*       randFunc;
        LimitRandFunc*  limitRandFunc;
      };


      //------------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //------------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param heap     システム確保に使用するヒープ
       * @param setting  ライブラリの設定
       */
      Library( gfl2::heap::HeapBase* heap, gfl2::fs::AsyncFileManager *fileMng, const ExternalSetting& setting );

      /**
       * @brief デストラクタ
       */
      virtual ~Library();

      /**
       * @brief ライブラリの外部設定
       */
      static const ExternalSetting* m_setting;

      static gfl2::fs::AsyncFileManager *m_pmlFileManager;
  };


}  // namespace pml 

#endif  // __PML_LIBRARY_H__
