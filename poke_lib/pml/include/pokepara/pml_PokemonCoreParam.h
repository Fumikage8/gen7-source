//=======================================================================================
/**
 * @brief  ポケモンコアパラメータ
 * @file   pml_PokemonCoreParam.h
 * @author obata_toshihiro
 * @date   2010.12.28
 */
//=======================================================================================
#ifndef __PML_POKEMONCOREPARAM_H__
#define __PML_POKEMONCOREPARAM_H__
#pragma once

#include <niji_conv_header/poke_lib/seikaku.h>
#include "pml/include/wazasick_def.h"
#include "pml/include/pml_Type.h"
#include "pml/include/pml_Personal.h"
#include "pml/include/pokepara/pml_PokemonParamType.h"
#include "pml/include/pokepara/pml_PokemonParamSpec.h"
#include "pml/src/personal/pml_WazaOboeLocal.h"


namespace pml {

  class PokeParty;  //前方参照


  /**
   * @namespace pml::pokepara
   */
  namespace pokepara {

    class EvolveManager;
    class Accessor;
    struct CoreData;
    struct CalcData;
    struct InitialSpec;


    //=======================================================================================
    /**
     * @class CoreParam
     * @brief ポケモンパラメータ：コア
     * @note  手持ち、ボックス等問わず必ず保持するポケモンのコアデータ
     */
    //=======================================================================================
    class CoreParam
    {
      GFL_FORBID_COPY_AND_ASSIGN( CoreParam ); 

      public: 
      static const u32 DATASIZE = 232;       ///< データのバッファ確保用のサイズ定義
      static const u32 MAX_RIBBON_NUM = 64;  ///< 登録可能なリボンNo.の最大数
      static const u8  TOKUSEI_INDEX_ERROR = 0xFF;  //GetTokuseiIndexStrict()で返る特性が不正な場合のエラー値
      static const u8  MAX_DIRT_POS_NUM = 5;  ///< 登録可能な汚れ発生位置の数
      
      //---------------------------------------------------------------------------------
      // 能力値( 最大HP・こうげき・ぼうぎょ・とくこう・とくぼう・すばやさ )
      //---------------------------------------------------------------------------------
      u32 GetPower( PowerID id ) const;  // 能力値を取得する

#if 0
      /**
       * @brief 能力値を上書きする
       *
       * @attention
       * 個体値・努力値などの値と能力値との整合性が崩れます.
       * 以降, 能力値の再計算が起こると, 上書きした値が元に戻ります.
       */
      void OverwritePower( PowerID id, u32 value );
#endif

      //---------------------------------------------------------------------------------
      // HP
      //---------------------------------------------------------------------------------
      u32 GetMaxHp( void ) const;   ///< 最大HPを取得する
      u32 GetHp( void ) const;      ///< HPを取得する
      void SetHp( u32 value );      ///< HPをセットする
      void ReduceHp( u32 value );   ///< HPを減らす
      void RecoverHp( u32 value );  ///< HPを回復する
      void RecoverHpFull( void );   ///< HPを全回復する
      bool IsHpFull( void ) const;  ///< HPが全快かどうか
      bool IsHpZero( void ) const;  ///< HPが0かどうか
      void RecoverAll( void );      ///< HP, PP, 状態異常・汚れを全回復する
#if PML_DEBUG
      void SetMaxHp( u32 value );   ///< 最大HPをセットする（デバッグ用）
      void SetLevel( u8 level );    ///< レベル設定（デバッグ用）
      void SetAtk( u16 value );     ///<こうげきを設定（デバッグ用）
#endif

      //---------------------------------------------------------------------------------
      // 状態異常
      //---------------------------------------------------------------------------------
      bool HaveSick( void ) const; ///< 状態異常かどうか
      Sick GetSick( void ) const;  ///< 状態を取得する
      void SetSick( Sick sick );   ///< 状態を設定する
      void RecoverSick( void );    ///< 状態異常を治す

      //---------------------------------------------------------------------------------
      // レベル, 経験値
      //---------------------------------------------------------------------------------
      u32 GetLevel( void ) const;               ///< レベルを取得する
      u32 GetExp( void ) const;                 ///< 経験値を取得する
      void SetExp( u32 value );                 ///< 経験値を変更する
      void AddExp( u32 value );                 ///< 経験値を増やす
      u32 GetExpForCurrentLevel( void ) const;  ///< 現在のレベルに必要な経験値を取得する
      u32 GetExpForNextLevel( void ) const;     ///< 次のレベルに必要な経験値を取得する
      void LevelUp( u8 up_val );                ///< レベルを上げる

#if PML_DEBUG
      void SetExpDirect( u32 value );           ///< デバッグ用：経験値を変更する(GlowTableのMinに修正しない)
#endif

      //---------------------------------------------------------------------------------
      // 個体値( 変更には能力値の更新を伴います )
      //---------------------------------------------------------------------------------
      u32 GetNativeTalentPower( PowerID power_id ) const;     ///< 元々の個体値を取得する( 「すごい特訓」済みでも、生まれ持った個体値を返します )
      u32 GetTalentPower( PowerID power_id ) const;           ///< 個体値を取得する( 「すごい特訓」済みなら、最大値が返ります )
      void ChangeTalentPower( PowerID power_id, u32 value );  ///< 個体値を変更する
      u32 GetTalentPowerMaxNum( void ) const;                 ///< 何Ｖか？取得する

      bool IsTrainingDone( PowerID power_id ) const;          ///< 「すごい特訓」を行ったか？取得する
      void SetTrainingDone( PowerID power_id );               ///< 「すごい特訓」を行ったことを設定する

#if PML_DEBUG
      void ResetTrainingDone( PowerID power_id );   ///< 「すごい特訓」を行っていない状態に戻す
#endif

      //---------------------------------------------------------------------------------
      // 努力値( 変更には能力値の更新を伴います )
      //---------------------------------------------------------------------------------
      u32 GetEffortPower( PowerID power_id ) const;           ///< 努力値を取得する
      u32 GetTotalEffortPower( void ) const;                  ///< 努力値の合計を取得する
      void ChangeEffortPower( PowerID power_id, u32 value );  ///< 努力値を変更する
      void AddEffortPower( PowerID power_id, u32 value );     ///< 努力値を加算する
      void SubEffortPower( PowerID power_id, u32 value );     ///< 努力値を減算する

#if PML_DEBUG
      void ChangeEffortPowerDirect( PowerID power_id, u32 value ); ///< デバッグ用：努力値変更
#endif
      
      //---------------------------------------------------------------------------------
      // モンスターNo., フォルムNo.
      //---------------------------------------------------------------------------------
      MonsNo GetMonsNo( void ) const;               ///< モンスターNo.を取得する
      FormNo GetFormNo( void ) const;               ///< フォルムNo.を取得する


      /**
       * @brief モンスターNo.を変更する
       * @caution 変更する状況はほとんどないはず！使用には注意！！
       */
      void ChangeMonsNo( MonsNo new_monsno, FormNo new_formno );

#if PML_DEBUG
      void SetMonsNoDirect( MonsNo new_monsno );  ///< デバッグ用：モンスターNo.書き換え
      void SetFormNoDirect( FormNo new_formno );  ///< デバッグ用：FormNo.書き換え
#endif
      
      //---------------------------------------------------------------------------------
      // 技
      //---------------------------------------------------------------------------------
      WazaNo GetWazaNo( u8 index ) const;    ///< 技No.
      u8 GetWazaCount( void ) const;         ///< 所持技の数

      void SetDefaultWaza( void );                   ///< デフォルト技をセットする
      void PushWaza( WazaNo wazano );                ///< 技を押し出し式にセットする
      void SetWaza( u8 index, WazaNo wazano );       ///< 位置を指定して技をセットする
      void RemoveWaza( u8 index );                   ///< 位置を指定して技を削除する
      void ExchangeWazaPos( u8 pos1, u8 pos2 );      ///< 技を入れ替える
      void CloseUpWazaPos( void );                   ///< 技を前詰めにシフトする

      bool HaveWaza( WazaNo wazano ) const;           ///< 技を覚えているか？
      bool CheckWazaMachine( u32 machine_no ) const;  ///< 技マシンを使えるか？
      bool CheckWazaOshie( u32 oshie_no ) const;      ///< 技教えが可能か？
      bool CheckBpWazaOshie(u32 waza_no) const;       ///< BP技教えが可能か？

      WazaNo GetTamagoWazaNo( u8 index ) const;         ///< 覚えたタマゴ技を取得する
      void SetTamagoWazaNo( u8 index, WazaNo wazano );  ///< 覚えたタマゴ技を登録する

      // 空きがあったら技を追加する
      WazaLearningResult AddWazaIfEmptyExist( WazaNo wazano );


      // 技習得処理に必要なワーク
      class WazaLearnWork
      {
      public:
        WazaLearnWork( void );
        void Clear( void );
        void AddCheckedWaza( WazaNo waza );
        bool IsCheckedWaza( WazaNo waza ) const;

      private:
        WazaNo m_checkedWazaArray[ pml::personal::MAX_WAZAOBOE_CODE_NUM ];  // 習得チェック済みの技
        u32    m_checkedWazaNum;                                            // 習得チェック済みの技の数
      };

      // 現在のレベルで習得できる技を追加する
      WazaLearningResult LearnNewWazaOnCurrentLevel( u32* same_level_index, WazaNo* new_wazano, WazaLearnWork* work=NULL );

      // 進化した際に習得できる技を追加する
      WazaLearningResult LearnNewWazaOnEvolution( u32* learn_index, WazaNo* new_wazano, WazaLearnWork* work=NULL );

#if PML_DEBUG
      void SetWazaNoDirect( u8 waza_index, WazaNo wazano ); ///< デバッグ用：技ナンバー書き換え
#endif
      
      //---------------------------------------------------------------------------------
      // 技PP
      //---------------------------------------------------------------------------------
      u32 GetWazaPP( u8 index ) const;               ///< 残りPP
      u32 GetWazaMaxPP( u8 index ) const;            ///< 最大PP
      void SetWazaPP( u8 index, u8 value );          ///< 残りPPをセットする
      void ReduceWazaPP( u8 index, u8 value );       ///< 残りPPを減らす
      void RecoverWazaPP( u8 index );                ///< 指定した所持技のPPを全回復する
      void RecoverWazaPP( u8 index, u8 recv_value ); ///< 指定した所持技のPPを回復する
      void RecoverWazaPPAll( void );                 ///< 全所持技のPPを全回復する

      bool CanUsePointUp( u8 index );               ///<「ポイントアップ」を使えるかどうか
      void UsePointUp( u8 index );                  ///<「ポイントアップ」を使う
      u32 GetWazaPPUpCount( u8 index ) const;       ///<「ポイントアップ」を使用した回数
      void SetWazaPPUpCount( u8 index, u8 value );  ///<「ポイントアップ」を使用した回数をセットする
      void IncWazaPPUpCount( u8 index );            ///<「ポイントアップ」を使用した回数を1増やす

      //---------------------------------------------------------------------------------
      // 性別
      //---------------------------------------------------------------------------------
      Sex GetSex( void ) const;                   ///< 性別を取得する( オス, メス, 性別なし )
      u8 GetSexVector( void ) const;              ///< 性別ベクトルの値を取得する[0, 255]
      personal::SexType GetSexType( void ) const; ///< 性別のタイプを取得する( ランダム or 固定 )
      void ChangeSex( Sex new_sex );              ///< 性別を変更する

#if PML_DEBUG
      void ChangeSexDirect( Sex new_sex );        ///< デバッグ用：性別を変更する
#endif
      
      //---------------------------------------------------------------------------------
      // 性格 (変更には能力値の更新を伴います)
      //---------------------------------------------------------------------------------
      Seikaku GetSeikaku( void ) const;       ///< 性格を取得する
      void ChangeSeikaku( Seikaku seikaku );  ///< 性格を変更する

      //---------------------------------------------------------------------------------
      // 特性
      //---------------------------------------------------------------------------------
      TokuseiNo GetTokuseiNo( void ) const;     ///< 特性No.を取得する
      u8 GetTokuseiIndex( void ) const;         ///< 何番目の特性なのか
      u8 GetTokuseiIndexStrict( void ) const;   ///< 何番目の特性なのか(持ちえない特性&TokuseiNoとtokusei_x_flagの不整合判定付き)
      void FlipTokuseiIndex( void );            ///< 特性１と２を入れ替える
      /// @caution tokusei_index が0 originであることに注意
      void SetTokuseiIndex( u8 tokusei_index ); ///< 何番目の特性かを書き換える

#if PML_DEBUG
      /**
       * @brief 特性No.を変更する
       *
       * @attention
       * 特性フラグと特性No.の整合性が壊れます.
       * 以降の進化, フォルムチェンジで特性が変化してしまう可能性があります.
       */
      void OverwriteTokuseiNo( TokuseiNo tokusei );
#endif

      //---------------------------------------------------------------------------------
      //  誰に所有されているか
      //---------------------------------------------------------------------------------
      /**
       * @brief   誰に所有されているかの判定
       * @param ownerInfo トレーナー識別情報
       * @return  bool    trueのとき、そのトレーナーに所有されている
       */
      bool CompareOwnerInfo( const OwnerInfo * ownerInfo ) const;
      /**
       * @brief   誰に所有されているかの状態更新
       * @param ownerInfo トレーナー識別情報
       * @return  bool    trueのとき、そのトレーナーに所有されている
       */
      bool UpdateOwnerInfo( const OwnerInfo * ownerInfo );

      /**
       * @brief   最初の親に所有されているか？のチェック
       */
      bool IsOwnedOriginalParent( void ) const;

#if PML_DEBUG
      void SetOwnedOthersFlag( bool flag );
#endif

      //---------------------------------------------------------------------------------
      // 種族名, ニックネーム
      //---------------------------------------------------------------------------------
      bool HaveNickName( void ) const;                                    ///< ニックネームを持っているか
      void GetNickName( gfl2::str::StrBuf* buffer ) const;                ///< ニックネームを取得する
      void GetNickName( gfl2::str::STRCODE* buffer, u32 buf_size ) const; ///< ニックネームを取得する
      void SetNickName( const gfl2::str::StrBuf* nickname );              ///< ニックネームを変更する
      void SetDefaultNickName( void );                                    ///< ニックネームに種族名をセットする
      bool IsDefaultNickName( void );                                     ///< ニックネームがプレイ言語の種族名デフォルトか？

#if PML_DEBUG
      void RemoveNickNameEom( void );                                   ///< デバッグ：ニックネームのEOMを取り除く
      void RemoveOyaNameEom( void );                                    ///< デバッグ：親名のEOMを取り除く
      void RemovePastOyaNameEom( void );                                ///< デバッグ：過去親名のEOMを取り除く

      void SetNickName13Chars( void );                                  ///< デバッグ：ニックネームを強制13文字にする
      void SetOyaName13Chars( void );                                   ///< デバッグ：親名を強制13文字にする
#endif
      
      public:
      //---------------------------------------------------------------------------------
      // 親とのなつき度
      //---------------------------------------------------------------------------------
      u32 GetFamiliarity( void ) const;  ///< なつき度を取得する
      void SetFamiliarity( u32 value );  ///< なつき度を変更する
      void AddFamiliarity( u32 value );  ///< なつき度を加算する
      void SubFamiliarity( u32 value );  ///< なつき度を減算する

      //---------------------------------------------------------------------------------
      // 最初の親のトレーナーとのなつき度
      //---------------------------------------------------------------------------------
      u32 GetOriginalFamiliarity( void ) const;  ///< なつき度を取得する
      void SetOriginalFamiliarity( u32 value );  ///< なつき度を変更する
      void AddOriginalFamiliarity( u32 value );  ///< なつき度を加算する
      void SubOriginalFamiliarity( u32 value );  ///< なつき度を減算する

      //---------------------------------------------------------------------------------
      // 親以外のトレーナーとのなつき度
      //---------------------------------------------------------------------------------
      u16 GetFamiliarTrainerID( void ) const;  ///< 親以外のなつきトレーナーのIDを取得する
      u32 GetOthersFamiliarity( void ) const;  ///< なつき度を取得する
      void SetOthersFamiliarity( u32 value );  ///< なつき度を変更する
      void AddOthersFamiliarity( u32 value );  ///< なつき度を加算する
      void SubOthersFamiliarity( u32 value );  ///< なつき度を減算する

      //---------------------------------------------------------------------------------
      // 親との仲良し度
      //---------------------------------------------------------------------------------
      u8 GetFriendship( void ) const;       ///< 仲良し度を取得する
      void SetFriendship( u32 friendship ); ///< 仲良し度を変更する
      void AddFriendship( u8 value );       ///< 仲良し度を増やす
      void SubFriendship( u8 value );       ///< 仲良し度を減らす

      //---------------------------------------------------------------------------------
      // 親以外のトレーナーとの仲良し度
      //---------------------------------------------------------------------------------
      u16 GetFriendlyTrainerID( void ) const;     ///< 親以外の仲良しトレーナーのIDを取得する
      void SetFriendlyTrainerID( u16 id );        ///< 親以外の仲良しトレーナーのIDを設定する
      u8 GetOthersFriendship( void ) const;       ///< 仲良し度を取得する
      void SetOthersFriendship( u8 friendship );  ///< 仲良し度を変更する
      void AddOthersFriendship( u8 value );       ///< 仲良し度を増やす
      void SubOthersFriendship( u8 value );       ///< 仲良し度を減らす

      //---------------------------------------------------------------------------------
      // タマゴ
      //---------------------------------------------------------------------------------
      bool IsEgg( EggCheckType type ) const;  ///< タマゴかどうか
      void SetEggFlag( void );                ///< タマゴフラグを立てる。タマゴを生成する場合はChangeEgg関数仕様を推奨
      void ChangeEgg( gfl2::heap::HeapBase* tmpHeap );  ///< タマゴにするユーティリティ. タマゴフラグ、孵化歩数、ニックネームをセットする
      void Birth( void );                     ///< タマゴを孵す
#if PML_DEBUG
      void SetFuseiTamagoFlag( void );        ///<不正タマゴにする
#endif

      //---------------------------------------------------------------------------------
      // 所持アイテム
      //---------------------------------------------------------------------------------
      u16 GetItem( void ) const;              ///< 所持アイテムのナンバーを取得する
      void SetItem( u16 itemno );             ///< アイテムを持たせる
      void RemoveItem( void );                ///< 道具を持っていない状態にする

      //---------------------------------------------------------------------------------
      // 進化, フォルムチェンジ
      //---------------------------------------------------------------------------------
      void Evolve( MonsNo next_monsno, u32 root_num );  ///< 進化する
      bool CanEvolve( const EvolveSituation *situation, const pml::PokeParty * party,  MonsNo* next_monsno, u32 * root_num ) const;  ///< 現在のパラメータと状況( 場所, 時間など )で進化できるか？
      bool CanEvolveByItem( const EvolveSituation *situation, u16 itemno, MonsNo* next_monsno, u32 * root_num ) const;  ///< 道具を使用することで進化できるか？＋状況( 場所, 時間など )
      bool CanEvolveByTrade( const CoreParam* pair_poke, MonsNo* next_monsno, u32 * root_num ) const; ///< 通信交換することで進化できるか？

      // 進化する余地があるか？
      bool HaveEvolutionRoot( void ) const;

      void MegaEvolve( void );             ///< メガ進化する
      void ResetMegaEvolve( void );        ///< メガ進化を元に戻す
      bool CanMegaEvolve( void ) const;    ///< メガ進化できるか？
      bool IsMegaEvolved( void ) const;    ///< メガ進化中か？
      FormNo GetMegaFormNo( void ) const;  ///< メガ進化後のフォルムNo.を取得する
      u32 GetMegaFormCount( void ) const;  ///< メガ進化フォルムの数を取得する

      void ChangeFormNo( FormNo next_formno );  ///< フォルムチェンジする

      // 所持どうぐによる影響で変化するフォルムを取得する
      FormNo GetNextFormNoFromHoldItem( u16 hold_itemno ) const;

      // キュレムの合体・分離（フォルムチェンジ）でのわざ変更処理
      void ChangeKyuremuFormWaza( FormNo next_formno );

      // フォルム関連パラメータのチェック・修正（ニャオニクス等）
      bool RegulateFormParams( void );

      //---------------------------------------------------------------------------------
      // レア, IDNo., 個性乱数, チェックサム
      //---------------------------------------------------------------------------------
      bool IsRare( void ) const;     ///< レアかどうか
      u32 GetRareRnd( void ) const;  ///< レア乱数を取得する
      u32 GetID( void ) const;       ///< IDNo.を取得する
      u32 GetPersonalRnd() const;    ///< 個性乱数を取得する
      u32 GetCheckSum() const;       ///< チェックサムを取得する

      void SetID( u32 id );     ///< IDNo.を変更する
#if PML_DEBUG
      void SetRare( void );     ///< レアにする
      void SetNotRare( void );  ///< レアでなくする
#endif

      //---------------------------------------------------------------------------------
      // タイプ(属性)
      //---------------------------------------------------------------------------------
      PokeType GetType1( void ) const;  ///< 属性1(アルセウスの特性, 所持品チェックを含む)
      PokeType GetType2( void ) const;  ///< 属性2(アルセウスの特性, 所持品チェックを含む)

      //---------------------------------------------------------------------------------
      // 親トレーナー
      //---------------------------------------------------------------------------------
      void GetParentName( gfl2::str::StrBuf* buffer ) const;                ///< 親名を取得する
      void GetParentName( gfl2::str::STRCODE* buffer, u32 buf_size ) const; ///< 親名を取得する
      void SetParentName( const gfl2::str::StrBuf* name );                  ///< 親名を変更する
      void SetParentName( const gfl2::str::STRCODE* strcode );              ///< 親名を変更する
      Sex GetParentSex( void ) const;                                       ///< 親の性別を取得する
      void SetParentSex( Sex sex );                                         ///< 親の性別をセットする
      u32 GetMemories( Memories memories_kind ) const;                      ///< 思い出データを取得する
      void SetMemories( Memories memories_kind, u32 value );                ///< 思い出データを設定する

      //---------------------------------------------------------------------------------
      // 過去の親トレーナー
      //---------------------------------------------------------------------------------
      void GetPastParentsName( gfl2::str::StrBuf* buffer ) const;
      void GetPastParentsName( gfl2::str::STRCODE* buffer, u32 buf_size ) const;
      void SetPastParentsName( const gfl2::str::StrBuf* name ) const;
      void SetPastParentsName( const gfl2::str::STRCODE* strcode ) const;
      Sex GetPastParentsSex( void ) const;
      void SetPastParentsSex( Sex sex );

      //---------------------------------------------------------------------------------
      // コンディション( かっこよさ, 毛艶など )
      //---------------------------------------------------------------------------------
      u32 GetCondition( Condition cond ) const;
      void SetCondition( Condition cond, u32 value );

      //---------------------------------------------------------------------------------
      // ボックスマーク
      //---------------------------------------------------------------------------------
      bool IsBoxMarkSet( void ) const;                     ///< ボックスマークの有無をチェックする
      bool IsBoxMarkSet( BoxMark mark ) const;             ///< ボックスマークの有無をチェックする
      void SetBoxMark( BoxMark mark, BoxMarkColor color ); ///< ボックスマークを登録する
      void RemoveBoxMark( BoxMark mark );                  ///< ボックスマークを解除する
      BoxMarkColor GetBoxMark( BoxMark mark ) const;       ///< ボックスマークを取得する
      void RemoveAllBoxMark( void );                                ///< 全てのボックスマークを解除する
      void SetAllBoxMark( const BoxMarkContainer& markContainer );  ///< 全てのボックスマークをセットする
      void GetAllBoxMark( BoxMarkContainer* markContainer ) const;  ///< 全てのボックスマークを取得する

      //---------------------------------------------------------------------------------
      /**
       * @brief 国コードの取得
       * @return  u32 国コード（nn::cfg::CfgCountryCodeに準拠）
       */
      //---------------------------------------------------------------------------------
      u32 GetCountryCode( void ) const;                 ///< 国コードを取得する
      void SetCountryCode( u32 country );               ///< 国コードを設定する

      //---------------------------------------------------------------------------------
      /**
       * @brief 地域コードの取得
       * @return  u32 地域コード(nn::cfg::CfgCountryCodeと対になるコード）
       */
      //---------------------------------------------------------------------------------
      u32 GetCountryRegionCode( void ) const;           ///< 地域コードを取得する
      void SetCountryRegionCode( u32 country_region );  ///< 地域コードを設定する

      //---------------------------------------------------------------------------------
      /**
       * @brief 本体リージョンの取得
       * @return  u32 本体リージョン（nn::cfg::CfgRegionCodeに準拠）
       */
      //---------------------------------------------------------------------------------
      u32 GetHWRegionCode( void ) const;                ///< 本体リージョンを取得する
      void SetHWRegionCode( u32 hw_region_code );       ///< 本体リージョンを設定する

      //---------------------------------------------------------------------------------
      /**
       * @brief 言語コードの取得
       * @return  u32 言語コード（LANG_JAPAN==1等、過去作に準拠）
       */
      //---------------------------------------------------------------------------------
      u32 GetLangId( void ) const;             ///< 言語コードを取得する
      void SetLangId( u32 land_id );           ///< 言語コードを設定する
      u32 GetCassetteVersion( void ) const;    ///< 捕まえたカセットバージョンを取得する
      void SetCassetteVersion( u32 version );  ///< 捕まえたカセットバージョンを設定する
      u32 GetGetBall( void ) const;            ///< 捕まえたボールを取得する
      void SetGetBall( u32 );                  ///< 捕まえたボールを設定する

      //---------------------------------------------------------------------------------
      // かわいがり
      //---------------------------------------------------------------------------------
      u8 GetFeed( void ) const;  ///< 満腹度を取得する
      void SetFeed( u32 value ); ///< 満腹度を設定する
      void AddFeed( u8 value );  ///< 満腹度を増やす
      void SubFeed( u8 value );  ///< 満腹度を減らす

      u8   GetNadenadeValue( void ) const;  ///< なでなで値を取得する
      void SetNadenadeValue( u8 value );    ///< なでなで値を設定する
      void AddNadenadeValue( u8 value );    ///< なでなで値を増やす
      void SubNadenadeValue( u8 value );    ///< なでなで値を減らす

      DirtType GetDirtType( void ) const;        ///< 汚れの種類を取得する
      void SetDirtType( DirtType  dirt );        ///< 汚れの種類を設定する
      bool CheckDirt( void ) const;              ///< 汚れの有無をチェックする
      bool CheckDirtPos( u8 posIndex ) const;    ///< 汚れの有無をチェックする
      void SetDirtPos( u8 posIndex );            ///< 指定位置に汚れを設定する
      void RemoveDirtPos( u8 posIndex );         ///< 指定位置の汚れを取り除く
      void ClearDirt( void );                    ///< 汚れが無い状態にする

      //-------------------------------------------------------------------------
      // トリミアン専用　体毛・日数・経過日数
      //-------------------------------------------------------------------------
      u8 TORIMIAN_GetBodyHairCount( void ) const;      ///< 体毛カウントを取得する
      u8 TORIMIAN_GetKeepDayCount( void ) const;       ///< 日数カウントを取得する
      u8 TORIMIAN_GetKeepMaxDayCount( void ) const;    ///< 経過日数カウントを取得する
                                                            
      void TORIMIAN_SetBodyHairCount( u8 hair_count ); ///< 体毛カウントをセットする
      void TORIMIAN_SetKeepDayCount( u8 day );         ///< 日数カウントをセットする
      void TORIMIAN_SetKeepMaxDayCount( u8 max_day );  ///< 経過日数カウントをセットする

      //---------------------------------------------------------------------------------
      //  スパーリング画面関連
      //---------------------------------------------------------------------------------
      u32  GetTrainingBadge1( void ) const;           ///<新規・トレーニング勲章
      u16  GetTrainingBadge2( void ) const;           ///<新規・トレーニング勲章
      bool IsTrainingBadge( s32 index ) const;        ///<新規・トレーニング勲章を所持しているか

      void SetTrainingBadge1( u32 badge );            ///< 新規・トレーニング勲章
      void SetTrainingBadge2( u16 badge );            ///< 新規・トレーニング勲章
      void SetTrainingBadge( s32 index, bool flag );  ///< 新規・トレーニング勲章ビットの立ち下げ

      //---------------------------------------------------------------------------------
      // めざめるパワー
      //---------------------------------------------------------------------------------
      PokeType GetMezapaType( void ) const;  ///< 「めざめるパワー」のタイプを取得
      u32 GetMezapaPower( void ) const;      ///< 「めざめるパワー」の威力を取得

      //---------------------------------------------------------------------------------
      // 味の好み
      //---------------------------------------------------------------------------------
      TasteJudge JudgeTaste( Taste taste ) const;  // 好き嫌いを調べる

      //---------------------------------------------------------------------------------
      // リボン
      //---------------------------------------------------------------------------------
      bool HaveRibbon( u32 ribbonno ) const;          ///< リボンを持っているか
      void SetRibbon( u32 ribbonno );                 ///< リボンをセットする
      void RemoveAllRibbon( void );                   ///< 全てクリアする
      void SetLumpingRibbon( LumpingRibbon ribbon_id, u32 num ); ///< まとめリボンをセット
      void SetLumpingRibbon( u32 ribbon_no, u32 num )
      {
        if ( ribbon_no == 37 ) SetLumpingRibbon( LUMPING_RIBBON_A, num );
        if ( ribbon_no == 38 ) SetLumpingRibbon( LUMPING_RIBBON_B, num );
      }
      u32 GetLumpingRibbon( LumpingRibbon ribbon_id ) const;     ///< まとめリボンを取得
      u32 GetLumpingRibbon( u32 ribbon_no ) const
      {
        if ( ribbon_no == 37 ) return GetLumpingRibbon( LUMPING_RIBBON_A );
        if ( ribbon_no == 38 ) return GetLumpingRibbon( LUMPING_RIBBON_B );
        return 0;
      }

      //---------------------------------------------------------------------------------
      // ポケルス
      //---------------------------------------------------------------------------------
      bool HavePokerusJustNow( void ) const;   ///< 今ポケルスに感染しているか？
      bool HavePokerusUntilNow( void ) const;  ///< 今までにポケルスに感染したことがあるか？
      bool HavePokerusPast( void ) const;      ///< 今までにポケルスに感染したことがあるか？
      void CatchPokerus( void );               ///< ポケルスに感染する
      void InfectPokerusWith( pml::pokepara::CoreParam* target ) const;  ///< ポケルスを伝染させる
      void DecreasePokerusDayCount( s32 passed_day_count );              ///< ポケルスの残り日数をデクリメントする
      u32 GetPokerus( void ) const;   ///< DEBUG用！ポケルスカウンタを取得
      void SetPokerus( u32 pokerus ); ///< DEBUG用！ポケルスカウンタを設定

      //---------------------------------------------------------------------------------
      // 配布ポケモン
      //---------------------------------------------------------------------------------
      bool GetEventPokeFlag( void ) const;  ///< 配布ポケモンかどうか
      void SetEventPokeFlag( bool flag );   ///< 配布ポケモンフラグを操作する

      //---------------------------------------------------------------------------------
      // ロトム専用
      //---------------------------------------------------------------------------------
      void RemoveAllRotomWaza( void );     ///< ロトムの専用技を忘れさせる
      void SetRotomWaza( u8 waza_index );  ///< ロトムの専用技を覚えさせる

      //---------------------------------------------------------------------------------
      // 相性チェック
      //---------------------------------------------------------------------------------
      LoveLevel CheckLoveLevel( const CoreParam& partner ) const;  ///< 相性をチェックする

      //---------------------------------------------------------------------------------
      // ポケモンリゾート
      //---------------------------------------------------------------------------------
      u8 GetResortEventStatus( void ) const;    ///< ポケモンリゾートのイベント状態を取得する
      void SetResortEventStatus( u8 status );   ///< ポケモンリゾートのイベント状態を設定する

      //---------------------------------------------------------------------------------
      // ポケデコ
      //---------------------------------------------------------------------------------
#if 0 // niji では使用しないはずなのでコメントアウト
      u16 GetPokeDeco( void ) const;   ///< ポケデコを取得する
      void SetPokeDeco( u16 value );   ///< ポケデコを設定する
#endif

#if PML_DEBUG
      //---------------------------------------------------------------------------------
      // デバッグ生成チェック
      //---------------------------------------------------------------------------------
      bool GetDebugEditFlag(void) const;  ///< デバッグで生成、変更されたポケモンか？
      void SetDebugEditFlag(bool flag);   ///< デバッグで生成、変更されたポケモンか？
#endif

      //---------------------------------------------------------------------------------
      // 高速モード
      //---------------------------------------------------------------------------------
      /**
       * @brief 高速モードを開始する
       * @retval true   高速モードに移行した場合
       * @retval false  すでに高速モードだった場合
       *
       * @note
       * 高速モード中は, 暗号化処理を省略します.
       * 一連の操作が完了したら, 必ず EndFstMode() を呼び, 高速モードを終了してください.
       */
      bool StartFastMode( void ) const;

      /**
       * @brief 高速モードを終了する
       * @param valid_flag  モード移行可能かどうかを判断するフラグ
       * @retval true   通常モードに移行した場合
       * @retval false  すでに通常モードだった場合
       *
       * @attention
       * valid_flag には true/false の直値を指定せず,
       * StartFastMode() の戻り値を指定してください。
       */
      bool EndFastMode( bool valid_flag ) const;

      /**
       * @brief 高速モードかどうかを調べる
       * @retval true   高速モード
       * @retval false  通常モード
       */
      bool IsFastMode( void ) const;


      //---------------------------------------------------------------------------------
      // データ操作
      //---------------------------------------------------------------------------------
      /**
       * @brief パラメータをゼロクリアする
       * @note MONSNO_NULLを指定して生成した状態になります.
       */
      void Clear( void );

      /**
       * @brief 空パラメータかどうかを調べる
       * @retval true   空パラメータ( MONSNO_NULLを指定して初期化した状態 )の場合
       * @retval false  何らかのポケモンのパラメータの場合
       * @note モンスターNo.がMONSNO_NULLかどうか、だけで判定しています.
       */
      bool IsNull( void ) const;

      /**
       * @brief 算出パラメータを含んでいるか？
       * @retval true  算出パラメータを含んでいる
       * @retval false コアパラメータのみ
       */
      bool HaveCalcParam( void ) const;

      /**
       * @brief パラメータをコピーする
       * @param src_param  コピー元となるパラメータ
       */
      void CopyFrom( const CoreParam& src_param );

      //---------------------------------------------------------------------------------
      /**
       * @brief     再計算可能パラメータの更新処理
       *
       * @note  CalcDataを再計算し、正しい値にするための処理
       * @caution メガ進化前パラメータも同時にクリアするので、バトル内部では使用しないこと！
       */
      //---------------------------------------------------------------------------------
      void RecalculateCalcData( void );

      //---------------------------------------------------------------------------------
      // シリアライズ・デシリアライズ
      //--------------------------------------------------------------------------------- 
      /**
       * @brief シリアライズデータを取得する
       * @param buffer  シリアライズデータの格納先( SERIALIZE_DATA_SIZE の大きさが必要 )
       */
      void Serialize_Core( void* buffer ) const;

      /**
       * @brief シリアライズデータにより復元する
       * @param serialized_data  Serialize()で取得したシリアライズデータ
       */
      virtual void Deserialize_Core( const void* serialized_data );


      //--------------------------------------------------------------------------------- 
      // コンストラクタ・デストラクタ
      //--------------------------------------------------------------------------------- 
      CoreParam( gfl2::heap::HeapBase* heap );
      CoreParam( gfl2::heap::HeapBase* heap, MonsNo monsno, u16 level, u64 id );
      CoreParam( gfl2::heap::HeapBase* heap, const InitialSpec& spec );
      CoreParam( gfl2::heap::HeapBase* heap, const CoreParam& src );
      virtual ~CoreParam();

      //---------------------------------------------------------------------------------
      // 静的なデータ、計算等
      //--------------------------------------------------------------------------------- 
      /**
       * @brief 性格によるパワー乱数の補正値を取得
       *
       * @param seikaku 性格
       * @param power_id  パワー乱数
       *
       * @return 0ならば補正なし +1ならば上昇補正 -1ならば下降補正
       */
      static s8 GetPowerTransformBySeikaku( u16 seikaku, PowerID power_id );



      protected:

      CoreData* m_coreData;
      CalcData* m_calcData;
      Accessor* m_accessor;


      void InitCoreData( void );

      void UpdateCalcDatas( bool keep_dead = true );
      void UpdateLevel( void );              // レベル
      void UpdateMaxHP( void );              // HP最大値
      void UpdateMaxHpAndCorrectHp( bool keep_dead = true );  // HP最大値( 残りHPを補正する )
      void UpdateAtk( void );                // こうげき
      void UpdateDef( void );                // ぼうぎょ
      void UpdateSpAtk( void );              // とくこう
      void UpdateSpDef( void );              // とくぼう
      void UpdateAgi( void );                // すばやさ

      u32 GetAtk( void ) const;    // こうげき
      u32 GetDef( void ) const;    // ぼうぎょ
      u32 GetSpAtk( void ) const;  // とくこう
      u32 GetSpDef( void ) const;  // とくぼう
      u32 GetAgi( void ) const;    // すばやさ

      u8 CalcLevel( void ) const;   // レベル
      u16 CalcMaxHp( void ) const;  // 最大HP
      u16 CalcAtk( void ) const;    // こうげき
      u16 CalcDef( void ) const;    // ぼうぎょ
      u16 CalcSpAtk( void ) const;  // とくこう
      u16 CalcSpDef( void ) const;  // とくぼう
      u16 CalcAgi( void ) const;    // すばやさ

      /**
       * @brief 努力値の合計が上限を超えないように調整する
       * @param before_value  変更前の努力値の値
       * @param after_value   変更後の努力値の値
       * @return 合計値の上限を考慮した, 変更後の値
       */
      u32 AdjustEffortPower( u32 before_value, u32 after_value ) const;

      /**
       * @brief 技を覚えているかどうか調べる
       * @param wazano  チェックする技No.
       * @retval true   指定した技を覚えている
       * @retval false  指定した技を覚えていない
       */
      bool IsHaveWaza( WazaNo wazano ) const;





      // クラスの静的な設定 /////////////////////////////////////////////////////////////

      public:

      /**
       * @brief 進化マネージャを設定する
       * @param manager 関連付けるマネージャ
       */
      static void SetEvolveManager( EvolveManager* manager );

      /**
       * @brief バッファ確保用に定義した DATASIZE の正当性チェック
       */
      static void CheckPublicDataSize( void );

      /**
       * @brief IDとカラー乱数からレアになる組み合わせかチェック
       */
      static bool IsRareFromValue( u32 id, u32 color_rnd );

      private: 
      static EvolveManager* s_evolve_manager;

    };







#if PML_DEBUG
/*
    static const gfl2::str::STRCODE* SEIKAKU_NAME[ SEIKAKU_NUM ] =
    {
      L"がんばりや",
      L"さみしがり",
      L"ゆうかん",
      L"いじっぱり",
      L"やんちゃ",
      L"ずぶとい",
      L"すなお",
      L"のんき",
      L"わんぱく",
      L"のうてんき",
      L"おくびょう",
      L"せっかち",
      L"まじめ",
      L"ようき",
      L"むじゃき",
      L"ひかえめ",
      L"おっとり",
      L"れいせい",
      L"てれや",
      L"うっかりや",
      L"おだやか",
      L"おとなしい",
      L"なまいき",
      L"しんちょう",
      L"きまぐれ",
    };
    */
#endif


  }  // namespace pokepara
}  // namespace pml


#endif  // __PML_POKEMONCOREPARAM_H__
