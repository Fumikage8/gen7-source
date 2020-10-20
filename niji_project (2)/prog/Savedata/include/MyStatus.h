//===================================================================
/**
 * @file    MyStatus.h
 * @brief   プレイヤーデータ管理
 * @author  taya
 * @date    11.4.6
 */
//===================================================================
#ifndef __SAVEDATA_MYSTATUS_H__
#define __SAVEDATA_MYSTATUS_H__

#include <str/include/gfl2_StrBuf.h>
#include <pml/include/pmlib.h>

#include "System/include/GameStrLen.h"
#include "SaveDataInterface.h"
#include "System/include/DressUp.h"

GFL_NAMESPACE_BEGIN(Savedata)

  /**
   *  プレイヤーデータ管理クラス
   */
  class MyStatus : public SaveDataInterface
  {
    GFL_FORBID_COPY_AND_ASSIGN(MyStatus); //コピーコンストラクタ＋代入禁止

  public:
      //  アイコンデータ
      struct ICON_DATA
      {
        //  着せ替え
        //  16 byte
        System::DressUpParamSaveData dressUpParam;

        //  モーションid
        //  1 byte
        s8 animeId;

        //  モーションフレーム値
        //  1 byte
        u8 animeFrameNum;

        //  キャラの顔情報
        //  1 byte
        u8  eyeIndex;

        //  着せ替えをデフォルトにするか
        //  1byte
        u8  bDressDefault;

        //  カメラ情報
        //  カメラ座標
        //  8 byte
        f32 camPosX;
        f32 camPosY;

        //  カメラの画角
        //  4 byte
        f32 camFovy;

        //  キャラRot情報
        //  4 byte
        f32 modelYRot;

      };  // 36 byte

#if PM_DEBUG
  public:
#else
  private:
#endif

      /**
       * @brief コアデータ
       * @note  8バイトアラインメントです。
       */
      //※最低限のものを移植したのみです。 141020Ariizumi
      struct CORE_DATA
      {
        // 基本情報
        u32                  ID;                          ///< プレーヤーID                       // 4
        u8                   rom_code;                    ///< PM_VERSION                         // 5
        u8                   sex;                         ///< 性別                               // 6
        u8                   padding1[2];                 ///< パディング領域

        // 本体情報
        u64                  pssDataStoreId;              ///< PssPersonalData格納先データストアId 368
        u64                  nexUniqueId;                 ///< NEXユニークID                      // 16
        u64                  transferdId;                 ///< 使用中CTRのTransferdID             // 24
        u64                  firstTransferdId;            ///< セーブデータ生成時のTransferdID    // 32
        u32                  principalId;                 ///< プリンシパルID                     // 36
        u32                  addressId;                   ///< 簡易アドレス情報(国など）          // 40
        u16                  latitude;                    ///< 緯度(nn::cfg::SimpleAddresssより)  // 42
        u16                  longitude;                   ///< 経度(nn::cfg::SimpleAddresssより)  // 44
        u8                   regionCode;                  ///< 3DS本体リージョンコード            // 45
        u8                   pokeLanguageId;              ///< ポケモン言語コード(LANG_～)        // 46
        u8                   coppacsRestriction;          ///< COPPA制限フラグ                    // 47
        u8                   coppacsSetting;              ///< 前回のCOPPA制限フラグ              // 48

        // 文字情報
        gfl2::str::STRCODE    name[ System::STRLEN_PLAYER_NAME+System::EOM_LEN ];                        ///< 自身の名前   2Byte*STRLEN_PLAYER_NAME(12+1)文字*1 // 98

        //  アイコン情報
        ICON_DATA                            icon;

        u16                  has_mega_ring     :1;             ///< メガリング入手フラグ       　332
        u16                  has_zenryoku_ring :1;             ///< ぜんりょくリング入手フラグ
        u16                  quick_is_penalty  :1;             ///< クイックマッチ用：交換ペナルティフラグ
        u16                  quick_trade_ok    :1;             ///< クイックマッチ用：交換可能フラグ
        u16                  quick_doubule_ok  :1;             ///< クイックマッチ用：ダブルバトル可能フラグ
        u16                  has_shimameguri_no_akashi : 1;    ///< しまめぐりのあかし入手フラグ
        u16                  quick_type        :3;             ///< クイックマッチタイプ
        u16                  padding2          :7;            ///< パディング領域

        u8                   ballThrowType;                    ////<バトルのボール投げの種類(0～8) momijiで追加 7→8
        u8                   padding3[64];
      };

   public:
      static const u32 SERIALIZE_DATA_SIZE      = sizeof(CORE_DATA);   ///< シリアライズバッファのサイズ

  public:
    /*
      CORE_DATAのメンバ位置
      通信互換性チェック用
      CORE_DATAがfinalでprivateになるため、必要な情報のみpublicに出すこととした
    */
    static const u32 CORE_DATA_OffsetOf_ID                  = offsetof(CORE_DATA, ID);
    static const u32 CORE_DATA_OffsetOf_rom_code            = offsetof(CORE_DATA, rom_code);
    static const u32 CORE_DATA_OffsetOf_sex                 = offsetof(CORE_DATA, sex);
    static const u32 CORE_DATA_OffsetOf_padding1            = offsetof(CORE_DATA, padding1);
    static const u32 CORE_DATA_OffsetOf_pssDataStoreId      = offsetof(CORE_DATA, pssDataStoreId);              ///< PssPersonalData格納先データストアId 368
    static const u32 CORE_DATA_OffsetOf_nexUniqueId         = offsetof(CORE_DATA, nexUniqueId);                 ///< NEXユニークID                      // 16
    static const u32 CORE_DATA_OffsetOf_transferdId         = offsetof(CORE_DATA, transferdId);                 ///< 使用中CTRのTransferdID             // 24
    static const u32 CORE_DATA_OffsetOf_firstTransferdId    = offsetof(CORE_DATA, firstTransferdId);            ///< セーブデータ生成時のTransferdID    // 32
    static const u32 CORE_DATA_OffsetOf_principalId         = offsetof(CORE_DATA, principalId);                 ///< プリンシパルID                     // 36
    static const u32 CORE_DATA_OffsetOf_addressId           = offsetof(CORE_DATA, addressId);                   ///< 簡易アドレス情報(国など）          // 40
    static const u32 CORE_DATA_OffsetOf_latitude            = offsetof(CORE_DATA, latitude);                    ///< 緯度(nn::cfg::SimpleAddresssより)  // 42
    static const u32 CORE_DATA_OffsetOf_longitude           = offsetof(CORE_DATA, longitude);                   ///< 経度(nn::cfg::SimpleAddresssより)  // 44
    static const u32 CORE_DATA_OffsetOf_regionCode          = offsetof(CORE_DATA, regionCode);                  ///< 3DS本体リージョンコード            // 45
    static const u32 CORE_DATA_OffsetOf_pokeLanguageId      = offsetof(CORE_DATA, pokeLanguageId);              ///< ポケモン言語コード(LANG_～)        // 46
    static const u32 CORE_DATA_OffsetOf_coppacsRestriction  = offsetof(CORE_DATA, coppacsRestriction);          ///< COPPA制限フラグ                    // 47
    static const u32 CORE_DATA_OffsetOf_coppacsSetting      = offsetof(CORE_DATA, coppacsSetting);              ///< 前回のCOPPA制限フラグ              // 48
    static const u32 CORE_DATA_OffsetOf_name                = offsetof(CORE_DATA, name);                        ///< 自身の名前   2Byte*STRLEN_PLAYER_NAME(12+1)文字*1 // 98
    static const u32 CORE_DATA_OffsetOf_icon                = offsetof(CORE_DATA, icon);
    static const u32 CORE_DATA_OffsetOf_ballThrowType       = offsetof(CORE_DATA, ballThrowType);                    ////<バトルのボール投げの種類(0～8) momijiで追加 7→8
    static const u32 CORE_DATA_OffsetOf_padding3            = offsetof(CORE_DATA, padding3);


   public:
      MyStatus( void );
      virtual ~MyStatus();

   public:
      //=============================================================================================
      /**
       * @brief 初期化処理
       *
       * @note  およそ4KBのヒープを一時的に確保する
       *
       * @param heap ヒープ
       */
      //=============================================================================================
      void Initialize( gfl2::heap::HeapBase* heap );

      //=============================================================================================
      /**
       *  データコピー
       *
       * @param[in]   src   コピー元
       */
      //=============================================================================================
      void CopyFrom( const MyStatus& src );

      //=============================================================================================
      /**
       * @brief １日毎に更新を行う処理
       */
      //=============================================================================================
      void ResetWithDaytime( void );

      //=============================================================================================
      /**
       * @brief セーブデータ生成時＆ロード時に毎回情報をセットする
       *
       * @param heap 一時的に利用するヒープ
       */
      //=============================================================================================
      void SetupBasicInfo( gfl2::heap::HeapBase * heap );

      //=============================================================================================
      /**
      * @brief 緯度・経度・シンプルアドレス情報を初期化する
      *
      * @note  引数で渡したメモリは一時的なメモリエリアとして利用され、常駐使用はされない
      *
      * @param heap    ワークメモリ
      */
      //=============================================================================================
      void InitializeGeoParam( gfl2::heap::HeapBase * heap );

   public:
      //=============================================================================================
      /**
       * 名前文字列を設定
       *
       * @param[out]   src    コピー元バッファ
       */
      //=============================================================================================
      void SetNameString( gfl2::str::StrBuf* src );

      //=============================================================================================
      /**
       * 名前文字列を取得
       *
       * @param[out]   dst    取得先バッファ
       */
      //=============================================================================================
      void GetNameString( gfl2::str::StrBuf* dst ) const;

      //=============================================================================================
      /**
       * 名前文字列を取得
       *
       * @param[out]   dst    取得先バッファ
       */
      //=============================================================================================
      void GetNameString( gfl2::str::STRCODE* dst ) const;

      //=============================================================================================
      /**
       * @brief ROMコードを取得
       *
       * @retval        u8    ロムコード
       */
      //=============================================================================================
      u8   GetRomCode( void ) const;

      //=============================================================================================
      /**
       * @brief 国を取得
       *
       * @retval        u8    ロムコード
       */
      //=============================================================================================
      u8 GetCountry( void ) const;
      
      //=============================================================================================
      /**
       * @brief 地域を取得
       *
       * @retval        u8    ロムコード
       */
      //=============================================================================================
      u8 GetArea( void ) const;

      //=============================================================================================
      /**
       * 性別コードを設定
       *
       * @param[in]        sex    PM_MALE or PM_FEMALE
       */
      //=============================================================================================
      void   SetSex( u8 sex );

      //=============================================================================================
      /**
       * @brief 性別コードを取得
       *
       * @retval        u8    PM_MALE or PM_FEMALE
       */
      //=============================================================================================
      u8   GetSex( void ) const;

#if defined(GF_PLATFORM_CTR)
      //=============================================================================================
      /**
       * @brief   簡易アドレス情報IDを取得する
       *
       * @retval  簡易アドレスID
       */
      //=============================================================================================
      nn::cfg::SimpleAddressId GetSimpleAddressId( void ) const;
#endif // defined(GF_PLATFORM_CTR)

      //=============================================================================================
      /**
       * @brief アプリ起動時の経度を取得
       *
       * @note 値の範囲は0x0000-0x7FFF(0°～+179.995°)、0x8000-0xFFFF(-180°～-0.005°)。
       * @note 「+(プラス)」は東経、「-(マイナス)」は西経。
       *
       * @retval        u16    経度
       */
      //=============================================================================================
      u16   GetLongitude( void ) const;

      //=============================================================================================
      /**
       * @brief アプリ起動時の緯度を取得
       *
       * @note 値の範囲は0x0000-0x4000(0°～+90°)、0xC000-0xFFFF(-90°～-0.005°)。
       * @note 「+(プラス)」は北緯、「-(マイナス)」は南緯。
       *
       * @retval        u16    緯度
       */
      //=============================================================================================
      u16   GetLatitude( void ) const;

      /**
       * リージョンコードを取得
       */
      u8   GetRegionCode( void ) const;

      //=============================================================================================
      /**
       * @brief 着せ替え情報をセット
       *
       * @param[in]        dressUpParam    着せ替え情報
       */
      //=============================================================================================
      void SetDressup( poke_3d::model::DressUpParam dressUpParam );

      //=============================================================================================
      /**
       * @brief 着せ替え情報を取得
       *
       * @retval        DressUpParam    着せ替え情報
       */
      //=============================================================================================
      poke_3d::model::DressUpParam GetDressup( void ) const;

      //=============================================================================================
      /**
       * @brief アイコン情報を設定
       *
       * @retval        ICON_DATA アイコン情報
       */
      //=============================================================================================
      void SetIcon( const ICON_DATA& rIconData );

      //=============================================================================================
      /**
       * @brief アイコン情報を取得
       *
       * @retval        ICON_DATA アイコン情報
       */
      //=============================================================================================
      const ICON_DATA&  GetIcon( void ) const;

      //=============================================================================================
      /**
       * 言語コードを取得
       *
       * @retval        u8
       *
       * @note  この言語コードは3DS本体の言語コードでなく、ポケモン独自の定義であることに注意。
       */
      //=============================================================================================
      u8   GetPokeLanguageId( void ) const;

      //=============================================================================================
      /**
       * @brief ポケモンの親判定用データを作成する
       * @param owner_info  生成対象のOwnerInfo
       */
      //=============================================================================================
      void CreateOwnerInfo( pml::pokepara::OwnerInfo& owner_info ) const;

      //=============================================================================================
      /**
       *  指定ポケモンの親が自分かどうか判定
       *
       *  @param[in]    poke      ポケモンパラメータ
       *
       *  @retval     bool    自分が親なら true
       */
      //=============================================================================================
      bool  IsMyPokemon( const pml::pokepara::CoreParam& poke ) const;

      //=============================================================================================
      /**
       * IDをセット
       *
       * @param   id
       */
      //=============================================================================================
      void   SetID( u32 id );

      //=============================================================================================
      /**
       * IDを取得
       *
       * @retval        u32    ID
       */
      //=============================================================================================
      u32   GetID( void ) const;

      //=============================================================================================
      /**
       * NEXユニークIDをセット
       *
       * @param   NEXユニークid
       */
      //=============================================================================================
      void   SetNexUniqueID( u64 id );

      //=============================================================================================
      /**
       * NEXユニークIDを取得
       *
       * @retval        u32    NEXユニークID
       */
      //=============================================================================================
      u64   GetNexUniqueID( void ) const;

      //=============================================================================================
      /**
       * NEXユニークIDが設定されているか判定
       *
       * @return NEXユニークIDが発行された値がセットされているならtrueを返却
       */
      //=============================================================================================
      bool   IsValidNexUniqueID( void ) const;

      //=============================================================================================
      /**
       * @brief PssPersonalData格納先データストアIDの設定
       *
       * @return PssPersonalData格納先データストアID
       */
      //=============================================================================================
      void  SetPssPersonalDataStoreId( u64 id );

      //=============================================================================================
      /**
       * @brief PssPersonalData格納先データストアIDの取得
       *
       * @return PssPersonalData格納先データストアID。未設定なら0を返却
       */
      //=============================================================================================
      u64   GetPssPersonalDataStoreId( void ) const;

      //=============================================================================================
      /**
       * IDの下位2バイトを取得
       *
       * @retval        u16    IDの下位2バイト
       */
      //=============================================================================================
      u16   GetIDLow( void ) const;

      //=============================================================================================
      /**
       * 表示用IDのを取得
       *
       * @retval        u32   IDの下6桁
       */
      //=============================================================================================
      u32 GetDrawID( void ) const;

      //=============================================================================================
      /**
      * @brief プリンシパルIDを取得
      *
      * @retval プリンシパルID
      */
      //=============================================================================================
      u32   GetPrincipalId( void ) const;

      //=============================================================================================
      /**
      * @brief TransferdIDを取得
      *
      * @retval TransferdID
      */
      //=============================================================================================
      u64   GetTransferdID( void ) const;

      //=============================================================================================
      /**
       *    CTR本体が変更されたかチェックする
       */
      //=============================================================================================
      void  CheckChangeCTR( void );

      //=============================================================================================
      /**
       * @brief COPPACSの初期化
       *
       * @param isForceChange 強制更新フラグ
       */
      //=============================================================================================
      void InitCoppacs( bool isForceChange );

      //=============================================================================================
      /**
       * @brief COPPACS制限有無の取得
       *
       * @return COPPACSの制限が有効ならtrueを返却する
       */
      //=============================================================================================
      bool IsCoppacsRestriction( void ) const;

      //=============================================================================================
      /**
       * @brief COPPACSの制限を有効にする
       */
      //=============================================================================================
      void RestrictCoppacs( void );

      //=============================================================================================
      /**
       * @brief COPPACSの制限を解除する
       */
      //=============================================================================================
      void PermitCoppacs( void );

      //=============================================================================================
      /**
       * 前回セーブ時からCTR本体が変更されたかを判定
       *
       * @return 本体が変更されたならtrueを返却
       */
      //=============================================================================================
      bool IsChangedCTR( void ) const;


   public:

      //=============================================================================================
      /**
       *    メガリングを所持しているかチェック
       */
      //=============================================================================================
      bool HasMegaRing( void ) const;

      //=============================================================================================
      /**
       *    メガリングの所持状態を変更
       */
      //=============================================================================================
      void SetMegaRing( bool flag );

      //=============================================================================================
      /**
       *    ぜんりょくリングを所持しているかチェック
       */
      //=============================================================================================
      bool HasZenryokuRing( void ) const;

      //=============================================================================================
      /**
       *    ぜんりょくリングの所持状態を変更
       */
      //=============================================================================================
      void SetZenryokuRing( bool flag );

      //=============================================================================================
      /**
       *    しまめぐりのしるしを所持しているかチェック
       */
      //=============================================================================================
      bool HasShimameguriNoShirushi( void ) const;

      //=============================================================================================
      /**
       *    しまめぐりのしるしの所持状態を変更
       */
      //=============================================================================================
      void SetShimameguriNoShirushi( bool flag );

      //=============================================================================================
      /**
       *    着せ替えパラメータのROMバージョンNIJIフラグをセット
       *     MMCat[337]対処用に作成
       */
      //=============================================================================================
      void SetDressUpParamRomVersionNiji();

      //=============================================================================================
      /**
       *    着せ替えパラメータのROMバージョンNIJIフラグを取得
       *     MMCat[337]対処用に作成
       */
      //=============================================================================================
      bool IsDressUpParamRomVersionNiji() const;

      //=============================================================================================
      /**
       *    クイックマッチ用情報取得
       */
      //=============================================================================================
      void GetQuickMatchInfo( bool* isPenalty , bool* isTradeOK , bool* isDoubleOK ) const;
      //=============================================================================================
      /**
       *    クイックマッチ用情報設定
       */
      //=============================================================================================
      void SetQuickMatchInfo( bool isPenalty , bool isTradeOK , bool isDoubleOK );
      
      //=============================================================================================
      /**
       *    クイックマッチ用情報設定
       */
      //=============================================================================================
      void SetQuickMatchType( u8 type ){ m_coreData.quick_type = type; };
      u8 GetQuickMatchType( void ){ return m_coreData.quick_type; };

      //=============================================================================================
      /**
       *    ボール投げの種類 設定・取得
       */
      //=============================================================================================
      void SetBallThrowType( u8 type );
      u8 GetBallThrowType( void )const;

#if PM_DEBUG
      //=============================================================================================
      /**
       *  自分を指定ポケモンの親として設定する
       *
       *  @param    poke        ポケモンパラメータ
       *  @param    pTmpHeap    内部で一時利用するヒープ
       */
      //=============================================================================================
      void  SetPokemonParent( pml::pokepara::CoreParam* poke, gfl2::heap::HeapBase* pTempHeap ) const;
#endif

    public:
      //=============================================================================================
      /**
       * シリアライズデータのサイズを取得
       *
       * @retval       u32    シリアライズデータのバイトサイズ
       */
      //=============================================================================================
      static u32 GetSerializeDataSize( void );

      //=============================================================================================
      /**
       * データシリアライズ
       *
       * @param[out]   dst    シリアライズデータ格納用バッファ
       *
       * @retval       u32    シリアライズデータのバイトサイズ
       */
      //=============================================================================================
      u32     Serialize( void* dst ) const;

      //=============================================================================================
      /**
       * データデシリアライズ
       *
       * @param[in]   data    デシリアライズ用データ
       */
      //=============================================================================================
      void    Deserialize( const void* data );

    public:
      //-----------------------------------------------------------------------------
      /**
       *  @brief    読み込んだデータをセットする関数
       *  @param    pData   先頭のポインタ
       */
      //-----------------------------------------------------------------------------
      virtual void SetData(void* pData);

      //=============================================================================================
      /**
       * [for SaveData]  保存するデータバイナリへのポインタを返す
       *
       * @retval        void*
       */
      //=============================================================================================
      virtual void*  GetData( void );

      //=============================================================================================
      /**
       * [for SaveData]  保存するデータバイナリのサイズを返す
       *
       * @retval        void*
       */
      //=============================================================================================
      virtual size_t    GetDataSize( void );

      //=============================================================================================
      /**
       * [for SaveData]  セーブデータを初期化する
       *
       * @param heap    ワークメモリ
       */
      //=============================================================================================
      virtual void    Clear( gfl2::heap::HeapBase * heap );

#if PM_DEBUG
      void SetDebugName( bool is_ctrname );
      void RemoveNameEom( void );
      void SetName13Chars( void );
#endif

    private:
      CORE_DATA           m_coreData;
      bool                m_isChangedCTR;     //!< 本体が変更されたか？

#if PM_DEBUG

      struct DEBUG_MYSTATUS_DATA
      {
        u32 m_dbgIconFriend;
        u32 m_dbgIconStranger;
        u32 m_dbgCountry;
        u32 m_dbgArea;
        u32 m_dbgRegion;
        u32 m_dbgLangId;
        s32 m_dbgEyeColor;

        u32 m_dbgE_SAVE_FASHION_BIT_NUM_VERSION;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_MODEL_TYPE;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_SKIN;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_HAIRCOLOR;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_PLACE_HAT;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_PLACE_FRONT;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_PLACE_HAIR;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_PLACE_FACE;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_PLACE_ACCBRACELET;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_PLACE_STAND;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_PLACE_TOPS;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_PLACE_BOTTOMS;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_PLACE_ONEPIECE;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_PLACE_SOCKS;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_PLACE_SHOES;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_PLACE_HOOK;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_PLACE_BAG;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_PLACE_ACCHAT;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_MAKE_PLACE_CONTACT;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_MAKE_PLACE_MASCARA;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_MAKE_PLACE_EYESHADOW;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_MAKE_PLACE_CHEEK;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_MAKE_PLACE_LIP;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_MAKE_CONTACT_COLOR;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_MAKE_MASCARA_COLOR;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_MAKE_EYESHADOW_COLOR;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_MAKE_CHEEK_COLOR;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_MAKE_LIP_COLOR;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_MAKE_PLACE_HIGE;
        u32 m_dbgE_SAVE_FASHION_BIT_NUM_MAKE_HIGE_COLOR;



        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_VERSION;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_MODEL_TYPE;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_SKIN;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_HAIRCOLOR;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_PLACE_HAT;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_PLACE_FRONT;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_PLACE_HAIR;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_PLACE_FACE;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_PLACE_ACCBRACELET;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_PLACE_STAND;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_PLACE_TOPS;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_PLACE_BOTTOMS;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_PLACE_SOCKS;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_PLACE_SHOES;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_PLACE_BAG;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_PLACE_ACCHAT;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_PLACE_HOOK;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_MAKE_PLACE_CONTACT;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_MAKE_PLACE_HIGE;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_MAKE_CONTACT_COLOR;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_MAKE_HIGE_COLOR;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_PAINT_PLACE_LEFT;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_PAINT_PLACE_RIGHT;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_PAINT_LEFT_COLOR;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_PAINT_RIGHT_COLOR;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_MAKE_PLACE_CHEEK;
        u32 m_dbgHERO_SAVE_FASHION_BIT_NUM_MAKE_CHEEK_COLOR;
        
        bool m_dbgMegaRing;
        bool m_dbgMegaRekkuuza;

      };

      DEBUG_MYSTATUS_DATA* m_pDbgData;

#endif
  };  // class MyStatus

GFL_NAMESPACE_END(Savedata)




#endif //__MYSTATUS_H__
