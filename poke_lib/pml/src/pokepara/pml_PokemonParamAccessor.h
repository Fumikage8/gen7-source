//=============================================================================
/**
 * @brief  ポケモンパラメータへのアクセッサ
 * @file   pml_PokemonParamAccessor.h
 * @author obata_toshihiro
 * @date   2010.12.24
 */
//=============================================================================
#ifndef __PML_POKEPARA_ACCESSOR_H__
#define __PML_POKEPARA_ACCESSOR_H__

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include "pml_PokemonParamEncoder.h"

namespace pml {
  namespace pokepara { 

    struct CoreData;
    struct CalcData;
    struct CoreDataBlockA;
    struct CoreDataBlockB;
    struct CoreDataBlockC;
    struct CoreDataBlockD;
    enum CoreDataBlockId;


    class Accessor 
    {
      GFL_FORBID_COPY_AND_ASSIGN( Accessor );

      public: 

      static const size_t CORE_DATA_SIZE;           // コアデータのサイズ
      static const size_t CALC_DATA_SIZE;           // 算出データのサイズ
      static const size_t FULL_SERIALIZE_DATA_SIZE; // 全データのシリアライズサイズ
      static const size_t CORE_SERIALIZE_DATA_SIZE; // コアデータのシリアライズサイズ

      //-----------------------------------------------------------------------
      // 初期化
      //-----------------------------------------------------------------------
      static void Initialize( void );

      //-----------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //-----------------------------------------------------------------------
      Accessor( void );
      virtual ~Accessor(); 

      //-----------------------------------------------------------------------
      // データ本体との関連付け
      //-----------------------------------------------------------------------
      void AttachDecodedData( CoreData* core_data, CalcData* calc_data );
      void AttachEncodedData( CoreData* core_data, CalcData* calc_data );
      bool HaveCalcData( void ) const;
      void ClearData( void );
      void ClearCalcData( void );

      //-----------------------------------------------------------------------
      // 高速モード
      //-----------------------------------------------------------------------
      void StartFastMode( void );
      void EndFastMode( void );
      bool IsFastMode( void ) const;
      bool IsEncoded( void ) const; 

      //-----------------------------------------------------------------------
      // シリアライズ・デシリアライズ
      //-----------------------------------------------------------------------
      void Serialize_FullData( void* buffer );
      void Serialize_CoreData( void* buffer );
      void Deserialize_FullData( const void* serialized_data );
      void Deserialize_CoreData( const void* serialized_data );

      //-----------------------------------------------------------------------
      // 各種パラメータ取得
      //-----------------------------------------------------------------------
      u32 GetPersonalRnd( void ) const;                                         // 個性乱数
      u32 GetColorRnd( void ) const;                                            // 色違い乱数
      u32 GetCheckSum( void ) const;                                            // チェックサム
      bool IsFuseiTamago( void ) const;                                         // ダメタマゴかどうか
      u32 GetSick( void ) const;                                                // 状態異常
      MonsNo GetMonsNo( void ) const;                                           // モンスターNo.
      u32 GetItemNo( void ) const;                                              // 所持アイテムNo.
      u32 GetID( void ) const;                                                  // ID
      u32 GetExp( void ) const;                                                 // 経験値
      u32 GetFamiliarity( void ) const;                                         // なつき度
      u8 GetFriendship( void ) const;                                           // 仲良し度
      TokuseiNo GetTokuseiNo( void ) const;                                     // 特性No.
      u16 GetBoxMark( void ) const;                                             // ボックスで使用するマーク
      u32 GetCountryCode( void ) const;                                         // 国コード
      u32 GetCountryRegionCode( void ) const;                                   // 所属国内地域コード
      u32 GetHWRegionCode( void ) const;                                        // 本体リージョン
      u32 GetLangId( void ) const;                                              // 言語コード
      u32 GetEffortHp( void ) const;                                            // 努力値 (HP)
      u32 GetEffortAtk( void ) const;                                           // 努力値 (こうげき)
      u32 GetEffortDef( void ) const;                                           // 努力値 (ぼうぎょ)
      u32 GetEffortSpAtk( void ) const;                                         // 努力値 (とくこう)
      u32 GetEffortSpDef( void ) const;                                         // 努力値 (とくぼう)
      u32 GetEffortAgi( void ) const;                                           // 努力値 (すばやさ)
      u32 GetStyle( void ) const;                                               // かっこよさ
      u32 GetBeautiful( void ) const;                                           // うつくしさ
      u32 GetCute( void ) const;                                                // かわいさ
      u32 GetClever( void ) const;                                              // かしこさ
      u32 GetStrong( void ) const;                                              // たくましさ
      u32 GetFur( void ) const;                                                 // 毛艶
      bool HaveRibbon( u32 ribbon_no ) const;                                   // リボン
      u32 GetLumpingRibbon( LumpingRibbon ribbon_id ) const;                    // ひとまとめリボン
      WazaNo GetWazaNo( u8 waza_index ) const;                                  // 所持技No.
      u8 GetPP( u8 waza_index ) const;                                          // 所持技の残りPP
      u8 GetWazaPPUpCount( u8 waza_index ) const;                               // ポイントアップを使用した回数
      WazaNo GetTamagoWazaNo( u8 index ) const;                                 // タマゴ技のストック
      u32 GetTalentHp( void ) const;                                            // 個体値 (HP)
      u32 GetTalentAtk( void ) const;                                           // 個体値 (こうげき)
      u32 GetTalentDef( void ) const;                                           // 個体値 (ぼうぎょ)
      u32 GetTalentSpAtk( void ) const;                                         // 個体値 (とくこう)
      u32 GetTalentSpDef( void ) const;                                         // 個体値 (とくぼう)
      u32 GetTalentAgi( void ) const;                                           // 個体値 (すばやさ)
      bool IsEventPokemon( void ) const;                                        // イベントで配布されたポケモンかどうか
      Sex GetSex( void ) const;                                                 // 性別
      FormNo GetFormNo( void ) const;                                           // フォルムNo.
      u32 GetSeikaku( void ) const;                                             // 性格
      bool IsTokusei1( void ) const;                                            // 第1特性かどうか
      bool IsTokusei2( void ) const;                                            // 第2特性かどうか
      bool IsTokusei3( void ) const;                                            // 第3特性かどうか
      bool IsTamago( void ) const;                                              // タマゴかどうか
      bool HaveNickName( void ) const;                                          // ニックネームをつけたかどうか
      void GetNickName( gfl2::str::StrBuf* buffer ) const;                      // ニックネーム
      void GetNickName( gfl2::str::STRCODE* buffer, u32 buf_len ) const;        // ニックネーム
      u32 GetCassetteVersion( void ) const;                                     // 捕まえたカセット( カラーバージョン )
      void GetOyaName( gfl2::str::StrBuf* buffer ) const;                       // 親の名前
      void GetOyaName( gfl2::str::STRCODE* buffer, u32 buf_len ) const;         // 親の名前                                    
      u32 GetTamagoGetYear( void ) const;                                       // タマゴをもらった年
      u32 GetTamagoGetMonth( void ) const;                                      // タマゴをもらった月
      u32 GetTamagoGetDay( void ) const;                                        // タマゴをもらった日
      u32 GetBirthYear( void ) const;                                           // タマゴが孵化した・捕獲した年
      u32 GetBirthMonth( void ) const;                                          // タマゴが孵化した・捕獲した月
      u32 GetBirthDay( void ) const;                                            // タマゴが孵化した・捕獲した日
      u32 GetGetPlace( void ) const;                                            // タマゴをもらった場所
      u32 GetBirthPlace( void ) const;                                          // タマゴが孵化した・捕獲した場所
      u32 GetPokerus( void ) const;                                             // ポケルス
      u32 GetGetBall( void ) const;                                             // 捕まえたボール
      u32 GetGetLevel( void ) const;                                            // 捕まえたレベル
      u32 GetOyasex( void ) const;                                              // 親の性別
      u32 GetLevel( void ) const;                                               // レベル
      u32 GetHp( void ) const;                                                  // HP
      u32 GetMaxHp( void ) const;                                               // 最大HP
      u32 GetAtk( void ) const;                                                 // こうげき
      u32 GetDef( void ) const;                                                 // ぼうぎょ
      u32 GetSpAtk( void ) const;                                               // とくこう
      u32 GetSpDef( void ) const;                                               // とくぼう
      u32 GetAgi( void ) const;                                                 // すばやさ
      u8 GetFeed( void ) const;                                                 // 満腹度
      u8 GetNadenadeValue( void ) const;                                        // なでなで値
      bool IsMegaForm( void ) const;                                            // メガ進化状態か？
      u8 GetMegaBeforeFormNo( void ) const;                                     // メガ進化する前のフォルムNo.
      u16 GetFamiliarTrainerID( void ) const;                                   // 親以外の仲良しトレーナーのID
      bool GetOwnedOthersFlag( void ) const;                                    // 親以外に所有されているかどうか
      u8 GetOriginalFamiliarity( void ) const;                                  // 最初の親とのなつき度
      u8 GetOthersFamiliarity( void ) const;                                    // 親以外のトレーナーとのなつき度
      u16 GetFriendlyTrainerID( void ) const;                                   // 親以外のなつきトレーナーID
      u8 GetOthersFriendship( void ) const;                                     // 親以外のトレーナーとのなつき度
      u8 GetMemoriesLevel( void ) const;                                        // 親との思い出レベル
      u8 GetMemoriesCode( void ) const;                                         // 親との思い出事象コード
      u16 GetMemoriesData( void ) const;                                        // 親との思い出内容データ
      u8 GetMemoriesFeel( void ) const;                                         // 親との思い出気持ちデータ
      u8 GetOthersMemoriesLevel( void ) const;                                  // 親以外のトレーナーとの思い出レベル
      u8 GetOthersMemoriesCode( void ) const;                                   // 親以外のトレーナーとの思い出事象コード
      u16 GetOthersMemoriesData( void ) const;                                  // 親以外のトレーナーとの思い出内容データ
      u8 GetOthersMemoriesFeel( void ) const;                                   // 親以外のトレーナーとの思い出気持ちデータ
      void GetPastParentsName( gfl2::str::StrBuf* buffer ) const;               // 過去の親の名前
      void GetPastParentsName( gfl2::str::STRCODE* buffer, u32 buf_len ) const; // 過去の親の名前
      Sex GetPastParentsSex( void ) const;                                      // 過去の親の性別
      bool CompareOyaName( const STRCODE* cmp_name ) const;                     // 親の名前一致チェック
      u32 GetTrainingBadge1( void ) const;                                      // トレーニング勲章
      u16 GetTrainingBadge2( void ) const;                                      // トレーニング勲章
      bool IsTrainingBadge( s32 index ) const;                                  // トレーニング勲章を所持しているか
      u32 GetMultiPurposeWork( void ) const;                                    // 多目的ワーク
      u8 GetResortEventStatus( void ) const;                                    // ポケモンリゾートのイベント状態
      u16 GetPokeDeco( void ) const;                                            // ポケデコ
      u8 GetTrainingFlag( void ) const;                                         // すごい特訓をしたか？フラグ
      DirtType GetDirtType( void ) const;                                       // 汚れの種類
      u8 GetDirtPos( void ) const;                                              // 汚れの発生位置

#if PML_DEBUG                                                                   
      void RemoveNickNameEom( void );                                           // デバッグ用：ニックネームのEOMをなくす
      void RemoveOyaNameEom( void );                                            // デバッグ用：親名のEOMをなくす
      void RemovePastOyaNameEom( void );                                            // デバッグ用：過去親名のEOMをなくす
      void SetNickName13Chars( void );                                          // デバッグ用：ニックネームに13文字をいれる
      void SetOyaName13Chars( void );                                        // デバッグ用：親名に13文字をいれる
      void SetDebugEditFlag(bool flag);                                       // デバッグで作成、編集されたか？
      bool GetDebugEditFlag(void);                                            // デバッグで作成、編集されたか？
#endif                                      

      //-----------------------------------------------------------------------
      // 各種パラメータ設定
      //-----------------------------------------------------------------------
      void SetPersonalRnd( u32 rnd );                                // 個性乱数
      void SetColorRnd( u32 rnd );                                   // 色違い乱数
      void SetSick( u32 sick );                                      // 状態異常
      void SetFuseiTamagoFlag( bool flag );                          // ダメタマゴかどうか
      void SetCheckSum( u16 checksum );                              // チェックサム
      void SetMonsNo( u32 monsno );                                  // モンスターNo.
      void SetItemNo( u16 itemno );                                  // 所持アイテムNo.
      void SetID( u32 id );                                          // ID
      void SetExp( u32 exp );                                        // 経験値
      void SetFamiliarity( u8 familiarity );                         // なつき度
      void SetFriendship( u8 friendship );                           // 仲良し度
      void SetTokuseiNo( u32 tokusei );                              // 特性No.
      void SetBoxMark( u16 mark );                                   // ボックスで使用するマーク
      void SetCountryCode( u8 country );                             // 国コード
      void SetCountryRegionCode( u8 country_region );                // 所属国内地域コード
      void SetHWRegionCode( u8 hw_region_code );                     // 本体リージョン
      void SetLangId( u8 lang_id );                                  // 言語コード
      void SetEffortHp( u8 value );                                  // 努力値 (HP)
      void SetEffortAtk( u8 value );                                 // 努力値 (こうげき)
      void SetEffortDef( u8 value );                                 // 努力値 (ぼうぎょ)
      void SetEffortSpAtk( u8 value );                               // 努力値 (とくこう)
      void SetEffortSpDef( u8 value );                               // 努力値 (とくぼう)
      void SetEffortAgi( u8 value );                                 // 努力値 (すばやさ)
      void SetStyle( u8 style );                                     // かっこよさ
      void SetBeautiful( u8 beautiful );                             // うつくしさ
      void SetCute( u8 cute );                                       // かわいさ
      void SetClever( u8 clever );                                   // かしこさ
      void SetStrong( u8 strong );                                   // たくましさ
      void SetFur( u8 fur );                                         // 毛艶
      void SetRibbon( u32 ribbon_no );                               // リボン(登録)
      void RemoveRibbon( u32 ribbon_no );                            // リボン(解除)
      void SetLumpingRibbon( LumpingRibbon ribbon_id, u32 num );     // ひとまとめリボン
      void SetWazaNo( u8 waza_index, u32 wazano );                   // 所持技No.
      void SetPP( u8 waza_index, u8 pp );                            // 所持技の残りPP
      void SetWazaPPUpCount( u8 waza_index, u8 count );              // ポイントアップを使用した回数
      void SetTamagoWazaNo( u8 index, u32 wazano );                  // タマゴ技のストック
      void SetTalentHp( u8 value );                                  // 個体値 (HP)
      void SetTalentAtk( u8 value );                                 // 個体値 (こうげき)
      void SetTalentDef( u8 value );                                 // 個体値 (ぼうぎょ)
      void SetTalentSpAtk( u8 value );                               // 個体値 (とくこう)
      void SetTalentSpDef( u8 value );                               // 個体値 (とくぼう)
      void SetTalentAgi( u8 value );                                 // 個体値 (すばやさ)
      void SetEventPokemonFlag( bool flag );                         // イベントで配布されたポケモンかどうか
      void SetSex( u32 sex );                                        // 性別
      void SetFormNo( u8 formno );                                   // フォルムNo.
      void SetSeikaku( u32 seikaku );                                // 性格
      void SetTamagoFlag( bool flag );                               // タマゴかどうか
      void SetTokusei1Flag( bool flag );                             // 第1特性かどうか
      void SetTokusei2Flag( bool flag );                             // 第2特性かどうか
      void SetTokusei3Flag( bool flag );                             // 第3特性かどうか
      void SetNickName( const STRCODE* nickName );                   // ニックネーム
      void SetNickNameFlag( bool flag );                             // ニックネームをつけたかどうか
      void SetCassetteVersion( u32 version );                        // 捕まえたカセット (カラーバージョン)
      void SetOyaName( const STRCODE* oyaName );                     // 親の名前
      void SetTamagoGetYear( u8 year );                              // タマゴをもらった年
      void SetTamagoGetMonth( u8 month );                            // タマゴをもらった月
      void SetTamagoGetDay( u8 day );                                // タマゴをもらった日
      void SetBirthYear( u8 year );                                  // タマゴが孵化した・捕獲した年
      void SetBirthMonth( u8 month );                                // タマゴが孵化した・捕獲した月
      void SetBirthDay( u8 day );                                    // タマゴが孵化した・捕獲した日
      void SetGetPlace( u16 place );                                 // タマゴをもらった場所
      void SetBirthPlace( u16 place );                               // タマゴが孵化した・捕獲した場所
      void SetPokerus( u8 pokerus );                                 // ポケルス
      void SetGetBall( u8 ball );                                    // 捕まえたボール
      void SetGetLevel( u8 level );                                  // 捕まえたレベル
      void SetOyasex( u8 sex );                                      // 親の性別
      void SetLevel( u8 level );                                     // レベル
      void SetMaxHp( u16 hp );                                       // HP最大値
      void SetHp( u16 hp );                                          // HP
      void SetAtk( u16 atk );                                        // こうげき
      void SetDef( u16 def );                                        // ぼうぎょ
      void SetSpAtk( u16 spatk );                                    // とくこう
      void SetSpDef( u16 spdef );                                    // とくぼう
      void SetAgi( u16 agi );                                        // すばやさ
      void SetFeed( u8 feed );                                       // 満腹度
      void SetNadenadeValue( u8 value );                             // なでなで値
      void SetMegaForm( bool is_mega ) const;                        // メガ進化状態か？
      void SetMegaBeforeFormNo( u8 formno );                         // メガ進化する前のフォルムNo.
      void SetFamiliarTrainerID( u16 trainer_id );                   // 親以外の仲良しトレーナーのID
      void SetOwnedOthersFlag( bool flag );                          // 親以外に所有されているかどうか
      void SetOriginalFamiliarity( u8 familiarity );                 // 最初の親とのなつき度
      void SetOthersFamiliarity( u8 familiarity );                   // 親以外のトレーナーとのなつき度
      void SetFriendlyTrainerID( u16 trainer_id );                   // 親以外のなつきトレーナーID
      void SetOthersFriendship( u8 friendship );                     // 親以外のトレーナーとのなつき度
      void SetMemoriesLevel( u8 level );                             // 親との思い出レベル
      void SetMemoriesCode( u8 code );                               // 親との思い出事象コード
      void SetMemoriesData( u16 data );                              // 親との思い出内容データ
      void SetMemoriesFeel( u8 feel );                               // 親との思い出気持ちデータ
      void SetOthersMemoriesLevel( u8 level );                       // 親以外のトレーナーとの思い出レベル
      void SetOthersMemoriesCode( u8 code );                         // 親以外のトレーナーとの思い出事象コード
      void SetOthersMemoriesData( u16 data );                        // 親以外のトレーナーとの思い出内容データ
      void SetOthersMemoriesFeel( u8 feel );                         // 親以外のトレーナーとの思い出気持ちデータ
      void SetPastParentsName( const STRCODE* name );                // 過去の親の名前
      void SetPastParentsSex( Sex sex );                             // 過去の親の性別
      void SetTrainingBadge1( u32 badge );                           // トレーニング勲章
      void SetTrainingBadge2( u16 badge );                           // トレーニング勲章
      void SetTrainingBadge( s32 index, bool flag );                 // トレーニング勲章ビットの立ち下げ
      void SetMultiPurposeWork( u32 value );                         // 多目的ワーク
      void SetResortEventStatus( u8 status );                        // ポケモンリゾートのイベント状態
      void SetPokeDeco( u16 value );                                 // ポケデコ
      void SetTrainingFlag( u8 value );                              // すごい特訓をしたか？フラグ
      void SetDirtType( DirtType value );                            // 汚れの種類
      void SetDirtPos( u8 value );                                   // 汚れの発生位置

      // リボンを全てクリアする
      void RemoveAllRibbon( void );




      private: 

      // アクセス情報
      struct AccessState {
        bool isEncoded;   // パラメータが暗号化されているかどうか
        bool isFastMode;  // 高速モードかどうか
        bool padding[2];
      };

      // 各リボンフィールドが持てるリボン情報の数
      static const u32 MAX_RIBBON_NUM_ON_RIBBON_FIELD_1;
      static const u32 MAX_RIBBON_NUM_ON_RIBBON_FIELD_2;

      // 各リボンフィールドに登録されるリボンNo.の最小値
      static const u32 MIN_RIBBON_NO_ON_RIBBON_FIELD_1;
      static const u32 MIN_RIBBON_NO_ON_RIBBON_FIELD_2;

      // 各リボンフィールドに登録されるリボンNo.の最大値
      static const u32 MAX_RIBBON_NO_ON_RIBBON_FIELD_1;
      static const u32 MAX_RIBBON_NO_ON_RIBBON_FIELD_2;
      
      // 不正タマゴ用データ
      static CalcData IllegalCalcData;
      static CoreDataBlockA IllegalCoreBlockA;
      static CoreDataBlockB IllegalCoreBlockB;
      static CoreDataBlockC IllegalCoreBlockC;
      static CoreDataBlockD IllegalCoreBlockD;

      // 不正データ書き込みバッファ
      static CalcData DummyWriteCalcData;
      static CoreDataBlockA DummyWriteCoreBlockA;
      static CoreDataBlockB DummyWriteCoreBlockB;
      static CoreDataBlockC DummyWriteCoreBlockC;
      static CoreDataBlockD DummyWriteCoreBlockD;

      CalcData* m_pCalcData;
      CoreData* m_pCoreData;
      AccessState m_accessState; 

      CalcData* GetCalcData( bool ) const;
      CoreDataBlockA* GetCoreDataBlockA( bool ) const;
      CoreDataBlockB* GetCoreDataBlockB( bool ) const;
      CoreDataBlockC* GetCoreDataBlockC( bool ) const;
      CoreDataBlockD* GetCoreDataBlockD( bool ) const;

      u8 GetCoreDataBlockPos( u32, CoreDataBlockId ) const;
      void UpdateChecksumAndEncode( void );
      void DecodeAndCheckIllegalWrite( void );
      void Serialize( void*, void* );
      void Deserialize( const void*, const void* );
    };



  } // namespace pokepara
} // namespace pml


#endif  // __PML_POKEPARA_ACCESSOR_H__
