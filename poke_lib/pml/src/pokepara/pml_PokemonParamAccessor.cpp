//=============================================================================
/**
 * @brief  ポケモンパラメータへのアクセッサ
 * @file   pml_PokemonParamAccessor.cpp
 * @author obata_toshihiro
 * @date   2010.12.24
 */
//==============================================================================
#include <debug/include/gfl2_Assert.h>
#include <math/include/gfl2_math.h>
#include <util/include/gfl2_std_string.h>

#include <pml/include/pokepara/pml_PokemonParam.h>
#include "pml_PokemonParamLocal.h"
#include "pml_PokemonParamEncoder.h"
#include "pml_PokemonParamAccessor.h"


namespace pml {
  namespace pokepara {


    // コアデータのサイズ
    const size_t Accessor::CORE_DATA_SIZE = sizeof(CoreData);

    // 算出データのサイズ
    const size_t Accessor::CALC_DATA_SIZE = sizeof(CalcData);

    // シリアライズデータのサイズ
    const size_t Accessor::CORE_SERIALIZE_DATA_SIZE = Accessor::CORE_DATA_SIZE;
    const size_t Accessor::FULL_SERIALIZE_DATA_SIZE = Accessor::CORE_DATA_SIZE + Accessor::CALC_DATA_SIZE; 

    // 各リボンフィールドが持てるリボン情報の数
    const u32 Accessor::MAX_RIBBON_NUM_ON_RIBBON_FIELD_1 = 32;
    const u32 Accessor::MAX_RIBBON_NUM_ON_RIBBON_FIELD_2 = 32;

    // 各リボンフィールドに登録されるリボンNo.の最小値
    const u32 Accessor::MIN_RIBBON_NO_ON_RIBBON_FIELD_1 = 0;
    const u32 Accessor::MIN_RIBBON_NO_ON_RIBBON_FIELD_2 = MIN_RIBBON_NO_ON_RIBBON_FIELD_1 + MAX_RIBBON_NUM_ON_RIBBON_FIELD_1;

    // 各リボンフィールドに登録されるリボンNo.の最大値
    const u32 Accessor::MAX_RIBBON_NO_ON_RIBBON_FIELD_1 = MIN_RIBBON_NO_ON_RIBBON_FIELD_1 + MAX_RIBBON_NUM_ON_RIBBON_FIELD_1 - 1;
    const u32 Accessor::MAX_RIBBON_NO_ON_RIBBON_FIELD_2 = MIN_RIBBON_NO_ON_RIBBON_FIELD_2 + MAX_RIBBON_NUM_ON_RIBBON_FIELD_2 - 1;



    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    //-------------------------------------------------------------------------
    Accessor::Accessor( void ) :
      m_pCalcData( NULL ),
      m_pCoreData( NULL ),
      m_accessState()
    {
      m_accessState.isEncoded = false;
      m_accessState.isFastMode = false;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    Accessor::~Accessor()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief データ本体を関連付ける
     * @param core_data  関連付ける( 復号化済み )コアデータ
     * @param calc_data  関連付ける( 復号化済み )算出データ
     */
    //-------------------------------------------------------------------------
    void Accessor::AttachDecodedData( CoreData* core_data, CalcData* calc_data )
    {
      m_pCoreData = core_data;
      m_pCalcData = calc_data;
      m_accessState.isEncoded = false;
      m_accessState.isFastMode = false;

      // 暗号化する
      this->UpdateChecksumAndEncode();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief データ本体を関連付ける
     * @param core_data  関連付ける( 暗号化済み )コアデータ
     * @param calc_data  関連付ける( 暗号化済み )算出データ
     */
    //-------------------------------------------------------------------------
    void Accessor::AttachEncodedData( CoreData* core_data, CalcData* calc_data )
    {
      m_pCoreData = core_data;
      m_pCalcData = calc_data;
      m_accessState.isEncoded = true;
      m_accessState.isFastMode = false;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 算出データを持っているかどうか調べる
     * @retval true   持っている
     * @retval false  持っていない
     */
    //-------------------------------------------------------------------------
    bool Accessor::HaveCalcData( void ) const
    {
      return ( m_pCalcData != NULL );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief データをゼロクリアする
     */
    //-------------------------------------------------------------------------
    void Accessor::ClearData( void )
    {
      if( m_pCoreData ) {
        gfl2::std::MemFill( m_pCoreData, 0, CORE_DATA_SIZE );
      }

      if( m_pCalcData ) {
        gfl2::std::MemFill( m_pCalcData, 0, CALC_DATA_SIZE );
      }

      // クリアした状態を復号化データとしてアタッチしなおす
      this->AttachDecodedData( m_pCoreData, m_pCalcData );
    } 

    //--------------------------------------------------------------------------
    /**
     * @brief 算出データをゼロクリアする
     */
    //--------------------------------------------------------------------------
    void Accessor::ClearCalcData( void )
    {
      if( m_pCalcData ) {
        bool fast_mode = this->IsFastMode();
        if( !fast_mode ) {
          this->StartFastMode();
        }
        gfl2::std::MemFill( m_pCalcData, 0, CALC_DATA_SIZE );
        if( !fast_mode ) {
          this->EndFastMode();
        }
      }
    }

    //--------------------------------------------------------------------------
    /**
     * @brief シリアライズデータを作成する
     * @param buffer  作成したデータの格納先( FULL_SERIALIZE_DATA_SIZE の大きさが必要 )
     */
    //--------------------------------------------------------------------------
    void Accessor::Serialize_FullData( void* buffer )
    {
      void* buffer_for_calc = gfl2::std::AddAddress( buffer, CORE_DATA_SIZE );
      this->Serialize( buffer, buffer_for_calc );
    }

    //--------------------------------------------------------------------------
    /**
     * @brief シリアライズデータを作成する
     * @param buffer  作成したデータの格納先( CORE_SERIALIZE_DATA_SIZE の大きさが必要 )
     */
    //--------------------------------------------------------------------------
    void Accessor::Serialize_CoreData( void* buffer )
    {
      this->Serialize( buffer, NULL );
    }

    /**
     * @brief シリアライズデータを作成する
     * @param[out] buffer_for_core    作成したデータの格納先
     * @param[out] buffer_for_calc    作成したデータの格納先
     */
    void Accessor::Serialize( void* buffer_for_core, void* buffer_for_calc )
    {
      // シリアライズデータは, 必ず暗号化した状態にする
      bool fast_mode = this->IsFastMode();
      if( fast_mode ) {
        this->EndFastMode();
      } 

      if( buffer_for_core ) {
        gfl2::std::MemCopy( m_pCoreData, buffer_for_core, CORE_DATA_SIZE );
      }

      if( buffer_for_calc ) {
        gfl2::std::MemCopy( m_pCalcData, buffer_for_calc, CALC_DATA_SIZE );
      }

      // 元の状態に戻す
      if( fast_mode ) {
        this->StartFastMode();
      }
    } 

    //--------------------------------------------------------------------------
    /**
     * @brief シリアライズデータにより復元する
     * @param serialized_data  Serialize_FullData()で取得したシリアライズデータ
     */
    //--------------------------------------------------------------------------
    void Accessor::Deserialize_FullData( const void* serialized_data )
    { 
      const void* serialized_calc_data = gfl2::std::AddAddress( serialized_data, CORE_DATA_SIZE );
      this->Deserialize( serialized_data, serialized_calc_data );
    }

    //--------------------------------------------------------------------------
    /**
     * @brief シリアライズデータによりコアデータを復元する
     * @param serialized_data  Serialize_CoreData()で取得したシリアライズデータ
     */
    //--------------------------------------------------------------------------
    void Accessor::Deserialize_CoreData( const void* serialized_data )
    {
      this->Deserialize( serialized_data, NULL );
    }

    /**
     * @brief シリアライズデータにより, パラメータを復元する
     * @param serialized_core_data シリアライズされたコアデータ
     * @param serialized_calc_data シリアライズされた算出データ
     */
    void Accessor::Deserialize( 
        const void* serialized_core_data,
        const void* serialized_calc_data )
    {
      if( serialized_core_data )
      {
        GFL_ASSERT_STOP( m_pCoreData ); //@check データ破壊 or プログラムミス
        gfl2::std::MemCopy( serialized_core_data, m_pCoreData, CORE_DATA_SIZE );
      }

      if( serialized_calc_data )
      {
        GFL_ASSERT_STOP( m_pCalcData ); //@check データ破壊 or プログラムミス
        gfl2::std::MemCopy( serialized_calc_data, m_pCalcData, CALC_DATA_SIZE );
      }

      // データを復号化し, 正当なデータかどうかチェックする
      // ( 不正データなら不正タマゴにする )
      m_accessState.isFastMode = false;
      m_accessState.isEncoded = true;  // シリアライズデータは必ず暗号化されている
      this->DecodeAndCheckIllegalWrite();
      this->UpdateChecksumAndEncode();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 暗号化されているかどうかを調べる
     * @retval true   暗号化されている
     * @ertavl false  暗号化されていない
     */
    //-------------------------------------------------------------------------
    bool Accessor::IsEncoded( void ) const
    {
      return m_accessState.isEncoded;
    }

    /**
     * @brief データを暗号化し, チェックサムを更新する
     */
    void Accessor::UpdateChecksumAndEncode( void )
    {
      if( m_accessState.isEncoded ) { return; }   // 暗号化済み
      if( m_accessState.isFastMode ) { return; }  // 高速モード

      if( m_pCoreData ) { // @fix cov_ctr[10091]  m_pCoreDataがNULLになる可能性があるため
        // チェックサムを更新
        m_pCoreData->header.checksum = 
          Encoder::CalcChecksum( m_pCoreData->blocks, sizeof(CoreDataBlock) * CORE_DATA_BLOCK_NUM );

        // コアパラメータを暗号化
        Encoder::Encode( 
            m_pCoreData->blocks,
            sizeof(CoreDataBlock) * CORE_DATA_BLOCK_NUM,
            m_pCoreData->header.personal_rnd );
      }

      // 算出データを暗号化
      if( m_pCalcData && m_pCoreData ) {
        Encoder::Encode( m_pCalcData, CALC_DATA_SIZE, m_pCoreData->header.personal_rnd );
      }

      // 暗号化完了
      m_accessState.isEncoded = true;
    } 

    /**
     * @brief データを復号化し, 改ざんチェックをする
     */
    void Accessor::DecodeAndCheckIllegalWrite( void )
    {
      if( m_accessState.isEncoded == false ) { return; }  // 復号化済み

      // コアパラメータを復号化
      Encoder::Decode( 
          m_pCoreData->blocks,
          sizeof(CoreDataBlock) * CORE_DATA_BLOCK_NUM,
          m_pCoreData->header.personal_rnd );

      // 算出データを復号化
      if( m_pCalcData ) {
        Encoder::Decode( m_pCalcData, CALC_DATA_SIZE, m_pCoreData->header.personal_rnd );
      }

      // 復号化完了
      m_accessState.isEncoded = false;

      // 改ざんチェック
      u32 old_checksum = m_pCoreData->header.checksum;
      u32 new_checksum = Encoder::CalcChecksum( m_pCoreData->blocks, sizeof(CoreDataBlock) * CORE_DATA_BLOCK_NUM );
      if( old_checksum != new_checksum ) {
        m_pCoreData->header.fusei_tamago_flag = true; // 改ざんされていたら不正タマゴにする
      }
    } 

    //--------------------------------------------------------------------------
    /**
     * @brief 高速モードを開始する
     * @note 復号化し, 高速モード終了まで暗号化しないようにフラグを立てる
     */
    //--------------------------------------------------------------------------
    void Accessor::StartFastMode( void )
    {
      // 復号化
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      m_accessState.isFastMode = true; 
      GFL_ASSERT( m_accessState.isFastMode && !m_accessState.isEncoded ); //@check プログラム検証用
    }

    //--------------------------------------------------------------------------
    /**
     * @brief 高速モードを終了する
     */
    //--------------------------------------------------------------------------
    void Accessor::EndFastMode( void )
    {
      // 暗号化
      m_accessState.isFastMode = false;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode(); 
      GFL_ASSERT( !m_accessState.isFastMode && m_accessState.isEncoded ); //@check プログラム検証用
    }

    //--------------------------------------------------------------------------
    /**
     * @brief 高速モードかどうかを調べる
     * @retval true   高速モード
     * @retval false  通常モード
     */
    //--------------------------------------------------------------------------
    bool Accessor::IsFastMode( void ) const
    {
      return m_accessState.isFastMode;
    }







    // 各種パラメータを取得する ///////////////////////////////////////////////

    /**
     * @brief 個性乱数
     */
    u32 Accessor::GetPersonalRnd( void ) const
    {
      return m_pCoreData->header.personal_rnd;
    }

    /**
     * @brief チェックサム
     */ 
    u32 Accessor::GetCheckSum( void ) const
    {
      return m_pCoreData->header.checksum;
    }

    /**
     * @brief ダメタマゴかどうか
     */
    bool Accessor::IsFuseiTamago( void ) const
    {
      return m_pCoreData->header.fusei_tamago_flag;
    } 

    /**
     * @brief 状態異常
     */
    u32 Accessor::GetSick( void ) const
    {
      if( !this->HaveCalcData() ) {
        return SICK_NULL;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCalcData( false )->sick;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief HP
     */
    u32 Accessor::GetHp( void ) const
    {
      if( !this->HaveCalcData() ) {
        return 0;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCalcData( false )->hp;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 最大HP
     */
    u32 Accessor::GetMaxHp( void ) const
    {
      if( !this->HaveCalcData() ) {
        return 0;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCalcData( false )->max_hp;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief こうげき
     */
    u32 Accessor::GetAtk( void ) const
    {
      if( !this->HaveCalcData() ) {
        return 0;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCalcData( false )->atk;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief ぼうぎょ
     */
    u32 Accessor::GetDef( void ) const
    {
      if( !this->HaveCalcData() ) {
        return 0;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCalcData( false )->def;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief とくこう
     */
    u32 Accessor::GetSpAtk( void ) const
    {
      if( !this->HaveCalcData() ) {
        return 0;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCalcData( false )->spatk;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief とくぼう
     */
    u32 Accessor::GetSpDef( void ) const
    {
      if( !this->HaveCalcData() ) {
        return 0;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCalcData( false )->spdef;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief すばやさ
     */
    u32 Accessor::GetAgi( void ) const
    {
      if( !this->HaveCalcData() ) {
        return 0;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCalcData( false )->agi;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief レベル
     */
    u32 Accessor::GetLevel( void ) const
    {
      if( !this->HaveCalcData() ) {
        return 0;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCalcData( false )->level;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 満腹度
     */
    u8 Accessor::GetFeed( void ) const
    {
      if( !this->HaveCalcData() ) {
        return 0;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockC( false )->feed;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief なでなで値
     */
    u8 Accessor::GetNadenadeValue( void ) const
    {
      if( !this->HaveCalcData() ) {
        return 0;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockC( false )->nade_nade_value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief メガ進化状態か？
     */
    bool Accessor::IsMegaForm( void ) const
    {
      if( !this->HaveCalcData() ) {
        return false;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCalcData( false )->is_mega;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return (ret!=0?true:false);
    }

    /**
     * @brief メガ進化する前のフォルムNo.
     */
    u8 Accessor::GetMegaBeforeFormNo( void ) const
    {
      if( !this->HaveCalcData() ) {
        return 0;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCalcData( false )->mega_before_formno;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief モンスターNo.
     */
    MonsNo Accessor::GetMonsNo( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->monsno;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return static_cast<MonsNo>(ret);
    }

    /**
     * @brief 所持アイテムNo.
     */
    u32 Accessor::GetItemNo( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->itemno;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief ID
     */
    u32 Accessor::GetID( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->id;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 経験値
     */
    u32 Accessor::GetExp( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->exp;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief なつき度
     */
    u32 Accessor::GetFamiliarity( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret;
      if ( this->GetCoreDataBlockC( false )->owned_by_others )
      {
        ret = this->GetCoreDataBlockC( false )->others_familiarity;
      }
      else
      {
        ret = this->GetCoreDataBlockD( false )->familiarity;
      }
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 仲良し度
     */
    u8 Accessor::GetFriendship( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->friendship;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 特性No.
     */
    TokuseiNo Accessor::GetTokuseiNo( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u8 ret = this->GetCoreDataBlockA( false )->tokuseino;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return static_cast<TokuseiNo>(ret);
    }

    /**
     * @brief ボックスで使用するマーク
     */
    u16 Accessor::GetBoxMark( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u16 ret = this->GetCoreDataBlockA( false )->box_mark;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 国コード
     */
    u32 Accessor::GetCountryCode( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->user_country_code;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 所属国内の地域コード
     */
    u32 Accessor::GetCountryRegionCode( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->user_region_code;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 3DS本体リージョンコード
     */
    u32 Accessor::GetHWRegionCode( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->hw_region_code;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 言語コード
     */
    u32 Accessor::GetLangId( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->lang_id;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 努力値 (HP)
     */
    u32 Accessor::GetEffortHp( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->effort_hp;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 努力値 (こうげき)
     */
    u32 Accessor::GetEffortAtk( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->effort_atk;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 努力値 (ぼうぎょ)
     */
    u32 Accessor::GetEffortDef( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->effort_def;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 努力値 (とくこう)
     */
    u32 Accessor::GetEffortSpAtk( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->effort_spatk;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 努力値 (とくぼう)
     */
    u32 Accessor::GetEffortSpDef( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->effort_spdef;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 努力値 (すばやさ)
     */
    u32 Accessor::GetEffortAgi( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->effort_agi;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief かっこよさ
     */
    u32 Accessor::GetStyle( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->style;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief うつくしさ
     */
    u32 Accessor::GetBeautiful( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->beautiful;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief かわいさ
     */
    u32 Accessor::GetCute( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->cute;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief かしこさ
     */
    u32 Accessor::GetClever( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->clever;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief たくましさ
     */
    u32 Accessor::GetStrong( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->strong;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 毛艶
     */
    u32 Accessor::GetFur( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->fur;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 所持技No.
     */
    WazaNo Accessor::GetWazaNo( u8 waza_index ) const
    {
      if( MAX_WAZA_NUM <= waza_index ) {
        GFL_ASSERT(0);  /* @check 引数異常は無効値を返す */
        return WAZANO_NULL;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockB( false )->waza[ waza_index ];
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return static_cast<WazaNo>( ret );
    }

    /**
     * @brief 所持技の残りPP
     */
    u8 Accessor::GetPP( u8 waza_index ) const
    {
      if( MAX_WAZA_NUM <= waza_index ) {
        GFL_ASSERT(0);  /* @check 引数異常は無効値を返す */
        return 0;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u8 ret = this->GetCoreDataBlockB( false )->pp[ waza_index ];
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 所持技に対してポイントアップを使用した回数
     */
    u8 Accessor::GetWazaPPUpCount( u8 waza_index ) const
    {
      if( MAX_WAZA_NUM <= waza_index ) {
        GFL_ASSERT(0);  /* @check 引数異常は無効値を返す */
        return 0;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u8 ret =this->GetCoreDataBlockB( false )->pointup_used_count[ waza_index ];
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief タマゴ技のストック
     */
    WazaNo Accessor::GetTamagoWazaNo( u8 index ) const
    {
      if( MAX_WAZA_NUM <= index ) {
        GFL_ASSERT(0);  /* @check 引数異常は無効値を返す */
        return WAZANO_NULL;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret =this->GetCoreDataBlockB( false )->tamago_waza[ index ];
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return static_cast<WazaNo>( ret );
    }

    /**
     * @brief 個体値 (HP)
     */
    u32 Accessor::GetTalentHp( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockB( false )->talent_hp;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 個体値 (こうげき)
     */
    u32 Accessor::GetTalentAtk( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret =this->GetCoreDataBlockB( false )->talent_atk;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 個体値 (ぼうぎょ)
     */
    u32 Accessor::GetTalentDef( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockB( false )->talent_def;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 個体値 (とくこう)
     */
    u32 Accessor::GetTalentSpAtk( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockB( false )->talent_spatk;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 個体値 (とくぼう)
     */
    u32 Accessor::GetTalentSpDef( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockB( false )->talent_spdef;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 個体値 (すばやさ)
     */
    u32 Accessor::GetTalentAgi( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockB( false )->talent_agi;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief タマゴかどうか
     */
    bool Accessor::IsTamago( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      bool ret = this->GetCoreDataBlockB( false )->tamago_flag;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief ニックネームをつけたかどうか
     */
    bool Accessor::HaveNickName( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      bool ret = this->GetCoreDataBlockB( false )->nickname_flag;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief イベントで配布されたポケモンかどうか
     */
    bool Accessor::IsEventPokemon( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      bool ret = this->GetCoreDataBlockA( false )->event_get_flag;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 性別
     */
    Sex Accessor::GetSex( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->sex;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return static_cast<Sex>(ret);
    }

    /**
     * @brief フォルムNo.
     */
    FormNo Accessor::GetFormNo( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret =this->GetCoreDataBlockA( false )->form_no;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 性格
     */
    u32 Accessor::GetSeikaku( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->seikaku;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 第1特性かどうか
     */
    bool Accessor::IsTokusei1( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      bool ret = this->GetCoreDataBlockA( false )->tokusei_1_flag;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 第2特性かどうか
     */
    bool Accessor::IsTokusei2( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      bool ret = this->GetCoreDataBlockA( false )->tokusei_2_flag;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 第3特性かどうか
     */
    bool Accessor::IsTokusei3( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      bool ret = this->GetCoreDataBlockA( false )->tokusei_3_flag;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 色違い乱数
     */
    u32 Accessor::GetColorRnd( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->color_rnd;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief ニックネーム
     * @param[out] buffer 文字列の格納先
     */
    void Accessor::GetNickName( gfl2::str::StrBuf* buffer ) const
    {
      if ( this->IsFuseiTamago() )
      {
        personal::GetMonsName( buffer, MONSNO_TAMAGO );
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      buffer->SetStr( this->GetCoreDataBlockB( false )->nickname );
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief ニックネーム
     * @param[out] buffer  文字列を格納する配列
     * @param[in]  buf_len 配列の要素数
     */
    void Accessor::GetNickName( gfl2::str::STRCODE* buffer, u32 buf_len ) const
    {
      u32 copy_char_count = gfl2::math::Min( (u32)( buf_len ), (u32)( MONS_NAME_BUFFER_SIZE ) );
      if ( this->IsFuseiTamago() )
      {
        STRCODE tamago_name[ MONS_NAME_BUFFER_SIZE ];
        personal::GetMonsName( tamago_name, MONSNO_TAMAGO );
        gfl2::str::StrNCopy( buffer, tamago_name, copy_char_count );
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      const STRCODE* src = this->GetCoreDataBlockB( false )->nickname;
      gfl2::str::StrNCopy( buffer, src, copy_char_count );
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

#if PML_DEBUG
    void Accessor::RemoveNickNameEom( void )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      STRCODE * code = this->GetCoreDataBlockB( true )->nickname;
      for (int i = 0; i < MONS_NAME_BUFFER_SIZE; ++ i )
      {
        if ( code[i] == gfl2::str::EOM_CODE ) code[i] = 0xff20; // '＠'
      }
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    void Accessor::SetNickName13Chars( void )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      STRCODE * code = this->GetCoreDataBlockB( true )->nickname;

      for (int i = 0; i < MONS_NAME_BUFFER_SIZE; ++ i )
      {
        code[i] = L'a'+i;
      }
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }
#endif
    
    /**
     * @brief 捕まえたカセット (カラーバージョン)
     */
    u32 Accessor::GetCassetteVersion( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u8 ret = this->GetCoreDataBlockD( false )->get_cassette;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 親の名前
     * @param[out] buffer 文字列の格納先
     */
    void Accessor::GetOyaName( gfl2::str::StrBuf* buffer ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      buffer->SetStr( this->GetCoreDataBlockD( false )->parents_name );
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 親の名前
     * @param[out] buffer  文字列を格納する配列
     * @param[in]  buf_len 配列の要素数
     */
    void Accessor::GetOyaName( gfl2::str::STRCODE* buffer, u32 buf_len ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      const STRCODE* src = this->GetCoreDataBlockD( false )->parents_name;
      u32 copy_char_count = gfl2::math::Min( (u32)( buf_len ), (u32)( PERSON_NAME_BUFFER_SIZE ) );
      gfl2::str::StrNCopy( buffer, src, copy_char_count );
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

#if PML_DEBUG
    void Accessor::RemoveOyaNameEom( void )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      STRCODE * code = this->GetCoreDataBlockD( true )->parents_name;
      for (int i = 0; i < MONS_NAME_BUFFER_SIZE; ++ i )
      {
        if ( code[i] == gfl2::str::EOM_CODE ) code[i] = 0xff20; // '＠'
      }
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }
    void Accessor::RemovePastOyaNameEom( void )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      STRCODE * code = this->GetCoreDataBlockC( true )->past_parents_name;
      for (int i = 0; i < MONS_NAME_BUFFER_SIZE; ++ i )
      {
        if ( code[i] == gfl2::str::EOM_CODE ) code[i] = 0xff20; // '＠'
      }
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }
    void Accessor::SetOyaName13Chars( void )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      STRCODE * code = this->GetCoreDataBlockD( true )->parents_name;
      for (int i = 0; i < MONS_NAME_BUFFER_SIZE; ++ i )
      {
        code[i] = L'a'+i;
      }
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

#endif
    
    /**
     * @brief タマゴをもらった年
     */
    u32 Accessor::GetTamagoGetYear( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->egg_get_year;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief タマゴをもらった月
     */
    u32 Accessor::GetTamagoGetMonth( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->egg_get_month;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief タマゴをもらった日
     */
    u32 Accessor::GetTamagoGetDay( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->egg_get_day;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief タマゴが孵化した・捕獲した年
     */
    u32 Accessor::GetBirthYear( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->first_contact_year;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief タマゴが孵化した・捕獲した月
     */
    u32 Accessor::GetBirthMonth( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->first_contact_month;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief タマゴが孵化した・捕獲した日
     */
    u32 Accessor::GetBirthDay( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->first_contact_day;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief タマゴをもらった場所
     */
    u32 Accessor::GetGetPlace( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->get_place;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief タマゴが孵化した・捕獲した場所
     */
    u32 Accessor::GetBirthPlace( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->birth_place;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief ポケルス
     */
    u32 Accessor::GetPokerus( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->pokerus;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 捕まえたボール
     */
    u32 Accessor::GetGetBall( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->get_ball;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 捕まえたレベル
     */
    u32 Accessor::GetGetLevel( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->get_level;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 親の性別
     */
    u32 Accessor::GetOyasex( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->parents_sex;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }


    /**
     * @brief 親に所有されているかどうかのフラグ 
     */
    bool Accessor::GetOwnedOthersFlag( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u8 ret = this->GetCoreDataBlockC( false )->owned_by_others;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return (ret!=0?true:false);
    }

    /**
     * @brief 親以外の仲良しトレーナーのID
     */
    u16 Accessor::GetFamiliarTrainerID( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockC( false )->familiar_trainer_id;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    } 

    /**
     * @brief 最初の親のトレーナーとのなつき度
     */
    u8 Accessor::GetOriginalFamiliarity( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->familiarity;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    } 
    /**
     * @brief 親以外のトレーナーとのなつき度
     */
    u8 Accessor::GetOthersFamiliarity( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockC( false )->others_familiarity;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    } 

    /**
     * @brief 親以外のなつきトレーナーID
     */
    u16 Accessor::GetFriendlyTrainerID( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockC( false )->friendly_trainer_id;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    } 

    /**
     * @brief 親以外のトレーナーとのなつき度
     */
    u8 Accessor::GetOthersFriendship( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockC( false )->others_friendship;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 親との思い出レベル
     */
    u8 Accessor::GetMemoriesLevel( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->memories_level;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 親との思い出事象コード
     */
    u8 Accessor::GetMemoriesCode( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->memories_code;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 親との思い出内容データ
     */
    u16 Accessor::GetMemoriesData( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->memories_data;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 親との思い出気持ちデータ
     */
    u8 Accessor::GetMemoriesFeel( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockD( false )->memories_feel;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 親以外のトレーナーとの思い出レベル
     */
    u8 Accessor::GetOthersMemoriesLevel( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockC( false )->others_memories_level;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 親以外のトレーナーとの思い出事象コード
     */
    u8 Accessor::GetOthersMemoriesCode( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockC( false )->others_memories_code;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 親以外のトレーナーとの思い出内容データ
     */
    u16 Accessor::GetOthersMemoriesData( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockC( false )->others_memories_data;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 親以外のトレーナーとの思い出気持ちデータ
     */
    u8 Accessor::GetOthersMemoriesFeel( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockC( false )->others_memories_feel;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief 過去の親の名前
     * @param[out] buffer 文字列の格納先
     */
    void Accessor::GetPastParentsName( gfl2::str::StrBuf* buffer ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      buffer->SetStr( this->GetCoreDataBlockC( false )->past_parents_name );
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 過去の親の名前
     * @param[out] buffer  文字列を格納する配列
     * @param[in]  buf_len 配列の要素数
     */
    void Accessor::GetPastParentsName( gfl2::str::STRCODE* buffer, u32 buf_len ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      const STRCODE* src = this->GetCoreDataBlockC( false )->past_parents_name;
      u32 copy_char_count = gfl2::math::Min( (u32)( buf_len ), (u32)( PERSON_NAME_BUFFER_SIZE ) );
      gfl2::str::StrNCopy( buffer, src, copy_char_count );
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 過去の親の性別
     */
    Sex Accessor::GetPastParentsSex( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockC( false )->past_parents_sex;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return static_cast<Sex>( ret );
    }

    /**
     * @brief 親の名前が一致するか比較
     * @param cmp_name
     * @return  bool  trueのとき、一致している
     */
    bool Accessor::CompareOyaName( const STRCODE* cmp_name ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      const STRCODE* src = this->GetCoreDataBlockD( false )->parents_name;
      bool ret = gfl2::str::StrComp( src, cmp_name );
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }

    /**
     * @brief トレーニング勲章
     */
    u32 Accessor::GetTrainingBadge1( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 ret = this->GetCoreDataBlockA( false )->training_badge1;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }
    u16 Accessor::GetTrainingBadge2( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u16 ret = this->GetCoreDataBlockA( false )->training_badge2;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }
    /**
     * @brief トレーニング勲章を所持しているか
     * @param[in] index いくつ目かを示すインデックス
     */
    bool  Accessor::IsTrainingBadge( s32 index ) const
    {
      if ( index >= pml::MAX_TRAINING_BADGE )
      {
        GFL_ASSERT( 0 );  //@check 引数異常はfalseを返す
        return false;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      bool ret = false;
      if( index < 32 )
      { 
        u32 badge1 = this->GetCoreDataBlockA( false )->training_badge1;
        ret = ( ( badge1 & ( 1 << index ) ) != 0 );
      }
      else
      { 
        u16 badge2 = this->GetCoreDataBlockA( false )->training_badge2;
        ret = ( ( badge2 & ( 1 << ( index - 32 ) ) ) != 0 );
      }
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return ret;
    }



    // 各種パラメータを設定する ///////////////////////////////////////////////

    /**
     * @brief 個性乱数
     */
    void Accessor::SetPersonalRnd( u32 rnd )
    {
      // 個性乱数をキーにして暗号化しているので, いったん復号化する必要がある
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      m_pCoreData->header.personal_rnd = rnd;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief チェックサム
     */
    void Accessor::SetCheckSum( u16 checksum )
    {
      m_pCoreData->header.checksum = checksum;
    }

    /**
     * @brief ダメタマゴかどうか
     */
    void Accessor::SetFuseiTamagoFlag( bool flag )
    {
      m_pCoreData->header.fusei_tamago_flag = flag;
    } 

    /**
     * @brief 状態異常
     */
    void Accessor::SetSick( u32 sick )
    {
      if( !this->HaveCalcData() ) {
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCalcData( true )->sick = sick;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief レベル
     */
    void Accessor::SetLevel( u8 level )
    {
      if( !this->HaveCalcData() ) {
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCalcData( true )->level = level;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief HP最大値
     */
    void Accessor::SetMaxHp( u16 max_hp )
    {
      if( !this->HaveCalcData() ) {
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCalcData( true )->max_hp = max_hp;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief HP
     */
    void Accessor::SetHp( u16 hp )
    {
      if( !this->HaveCalcData() ) {
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCalcData( true )->hp = hp;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief こうげき
     */
    void Accessor::SetAtk( u16 atk )
    {
      if( !this->HaveCalcData() ) {
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCalcData( true )->atk = atk;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief ぼうぎょ
     */
    void Accessor::SetDef( u16 def )
    {
      if( !this->HaveCalcData() ) {
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCalcData( true )->def = def;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief とくこう
     */
    void Accessor::SetSpAtk( u16 spatk )
    {
      if( !this->HaveCalcData() ) {
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCalcData( true )->spatk = spatk;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief とくぼう
     */
    void Accessor::SetSpDef( u16 spdef )
    {
      if( !this->HaveCalcData() ) {
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCalcData( true )->spdef = spdef;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief すばやさ
     */
    void Accessor::SetAgi( u16 agi )
    {
      if( !this->HaveCalcData() ) {
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCalcData( true )->agi = agi;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 満腹度
     */
    void Accessor::SetFeed( u8 feed )
    {
      if( !this->HaveCalcData() ) {
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockC( true )->feed = feed;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief なでなで値
     */
    void Accessor::SetNadenadeValue( u8 value )
    {
      if( !this->HaveCalcData() ) {
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockC( true )->nade_nade_value = value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    } 

    /**
     * @brief メガ進化状態か？
     */
    void Accessor::SetMegaForm( bool is_mega ) const
    {
      if( !this->HaveCalcData() ) {
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCalcData( true )->is_mega = is_mega;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief メガ進化する前のフォルムNo.
     */
    void Accessor::SetMegaBeforeFormNo( u8 formno )
    {
      if( !this->HaveCalcData() ) {
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCalcData( true )->mega_before_formno = formno;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief モンスターNo.
     */
    void Accessor::SetMonsNo( u32 monsno )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->monsno = monsno;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 所持アイテムNo.
     */
    void Accessor::SetItemNo( u16 itemno )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->itemno = itemno;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief ID
     */
    void Accessor::SetID( u32 id )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->id = id;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 経験値
     */
    void Accessor::SetExp( u32 exp )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->exp = exp;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief なつき度
     */
    void Accessor::SetFamiliarity( u8 familiarity )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      if ( this->GetCoreDataBlockC( false )->owned_by_others )
      {
        this->GetCoreDataBlockC( true )->others_familiarity = familiarity;
      }
      else
      {
        this->GetCoreDataBlockD( true )->familiarity = familiarity;
      }
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 仲良し度
     */
    void Accessor::SetFriendship( u8 friendship )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->friendship = friendship;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 特性No.
     */
    void Accessor::SetTokuseiNo( u32 tokusei )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->tokuseino = static_cast<u8>(tokusei);
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief ボックスで使用するマーク
     */
    void Accessor::SetBoxMark( u16 mark )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->box_mark = mark;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 国コード
     */
    void Accessor::SetCountryCode( u8 country )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->user_country_code = country;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 所属国内の地域コード
     */
    void Accessor::SetCountryRegionCode( u8 country_region )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      // @fix GFNMCat5790 対応
      this->GetCoreDataBlockD( true )->user_region_code = country_region;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 3DS本体リージョンコード
     */
    void Accessor::SetHWRegionCode( u8 region_code )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      // @fix GFNMCat5790 対応
      this->GetCoreDataBlockD( true )->hw_region_code = region_code;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 言語コード
     */
    void Accessor::SetLangId( u8 lang_id )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      // @fix GFNMCat5790 対応
      this->GetCoreDataBlockD( true )->lang_id = lang_id;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 努力値 (HP)
     */
    void Accessor::SetEffortHp( u8 value )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->effort_hp = value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 努力値 (こうげき)
     */
    void Accessor::SetEffortAtk( u8 value )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->effort_atk = value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 努力値 (ぼうぎょ)
     */
    void Accessor::SetEffortDef( u8 value )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->effort_def = value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 努力値 (とくこう)
     */
    void Accessor::SetEffortSpAtk( u8 value )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->effort_spatk = value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 努力値 (とくぼう)
     */
    void Accessor::SetEffortSpDef( u8 value )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->effort_spdef = value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 努力値 (すばやさ)
     */
    void Accessor::SetEffortAgi( u8 value )
    { 
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->effort_agi = value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief かっこよさ
     */
    void Accessor::SetStyle( u8 style )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->style = style;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief うつくしさ
     */
    void Accessor::SetBeautiful( u8 beautiful )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->beautiful = beautiful;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief かわいさ
     */
    void Accessor::SetCute( u8 cute )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->cute = cute;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief かしこさ
     */
    void Accessor::SetClever( u8 clever )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->clever = clever;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief たくましさ
     */
    void Accessor::SetStrong( u8 strong )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->strong = strong;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 毛艶
     */
    void Accessor::SetFur( u8 fur )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->fur = fur;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    } 

    /**
     * @brief 所持技No.
     */
    void Accessor::SetWazaNo( u8 waza_index, u32 wazano )
    {
      if( MAX_WAZA_NUM <= waza_index ) {
        GFL_ASSERT(0);  /* @check 引数異常は何もしない */
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockB( true )->waza[ waza_index ] = wazano;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 所持技の残りPP
     */
    void Accessor::SetPP( u8 waza_index, u8 pp )
    {
      if( MAX_WAZA_NUM <= waza_index ) {
        GFL_ASSERT(0);  /* @check 引数異常は何もしない */
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockB( true )->pp[ waza_index ] = pp;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    } 

    /**
     * @brief 所持技に対してポイントアップを使用した回数
     */
    void Accessor::SetWazaPPUpCount( u8 waza_index, u8 count )
    {
      if( MAX_WAZA_NUM <= waza_index ) {
        GFL_ASSERT(0);  /* @check 引数異常は何もしない */
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockB( true )->pointup_used_count[ waza_index ] = count;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief タマゴ技のストック
     */
    void Accessor::SetTamagoWazaNo( u8 index, u32 wazano )
    {
      if( MAX_WAZA_NUM <= index ) {
        GFL_ASSERT(0);  /* @check 引数異常は何もしない */
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockB( true )->tamago_waza[ index ] = wazano;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 個体値 (HP)
     */
    void Accessor::SetTalentHp( u8 value )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockB( true )->talent_hp = value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 個体値 (こうげき)
     */
    void Accessor::SetTalentAtk( u8 value )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockB( true )->talent_atk = value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 個体値 (ぼうぎょ)
     */
    void Accessor::SetTalentDef( u8 value )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockB( true )->talent_def = value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 個体値 (とくこう)
     */
    void Accessor::SetTalentSpAtk( u8 value )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockB( true )->talent_spatk = value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 個体値 (とくぼう)
     */
    void Accessor::SetTalentSpDef( u8 value )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockB( true )->talent_spdef = value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 個体値 (すばやさ)
     */
    void Accessor::SetTalentAgi( u8 value )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockB( true )->talent_agi = value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief タマゴかどうか
     */
    void Accessor::SetTamagoFlag( bool flag )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockB( true )->tamago_flag = flag;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief ニックネームをつけたかどうか
     */
    void Accessor::SetNickNameFlag( bool flag )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockB( true )->nickname_flag = flag;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief イベントで配布されたポケモンかどうか
     */
    void Accessor::SetEventPokemonFlag( bool flag )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->event_get_flag = flag;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 性別
     */
    void Accessor::SetSex( u32 sex )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->sex = sex;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief フォルムNo.
     */
    void Accessor::SetFormNo( u8 formno )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->form_no = formno;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 性格
     */
    void Accessor::SetSeikaku( u32 seikaku )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->seikaku = seikaku;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 第1特性かどうか
     */
    void Accessor::SetTokusei1Flag( bool flag )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->tokusei_1_flag = flag;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 第2特性かどうか
     */
    void Accessor::SetTokusei2Flag( bool flag )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->tokusei_2_flag = flag;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 第3特性かどうか
     */
    void Accessor::SetTokusei3Flag( bool flag )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->tokusei_3_flag = flag;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 色違い乱数
     */
    void Accessor::SetColorRnd( u32 rnd )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->color_rnd = rnd;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    } 

    /**
     * @brief ニックネーム
     */
    void Accessor::SetNickName( const STRCODE* nickName )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      gfl2::str::StrNCopy( this->GetCoreDataBlockB( true )->nickname, nickName , MONS_NAME_BUFFER_SIZE );
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    } 

    /**
     * @brief 捕まえたカセット (カラーバージョン)
     */
    void Accessor::SetCassetteVersion( u32 version )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->get_cassette = version;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    } 

    /**
     * @brief 親の名前
     */
    void Accessor::SetOyaName( const STRCODE* parents_name )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      gfl2::str::StrNCopy( this->GetCoreDataBlockD( true )->parents_name, parents_name , PERSON_NAME_BUFFER_SIZE );
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief タマゴをもらった年
     */
    void Accessor::SetTamagoGetYear( u8 year )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->egg_get_year = year;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief タマゴをもらった月
     */
    void Accessor::SetTamagoGetMonth( u8 month )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->egg_get_month = month;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief タマゴをもらった日
     */
    void Accessor::SetTamagoGetDay( u8 day )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->egg_get_day = day;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief タマゴが孵化した・捕獲した年
     */
    void Accessor::SetBirthYear( u8 year )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->first_contact_year = year;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief タマゴが孵化した・捕獲した月
     */
    void Accessor::SetBirthMonth( u8 month )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->first_contact_month = month;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief タマゴが孵化した・捕獲した日
     */
    void Accessor::SetBirthDay( u8 day )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->first_contact_day = day;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief タマゴをもらった場所
     */
    void Accessor::SetGetPlace( u16 place )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->get_place = place;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief タマゴが孵化した・捕獲した場所
     */
    void Accessor::SetBirthPlace( u16 place )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->birth_place = place;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief ポケルス
     */
    void Accessor::SetPokerus( u8 pokerus )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->pokerus = pokerus;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 捕まえたボール
     */
    void Accessor::SetGetBall( u8 ball )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->get_ball = ball;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 捕まえたレベル
     */
    void Accessor::SetGetLevel( u8 level )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->get_level = level;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 親の性別
     */
    void Accessor::SetOyasex( u8 sex )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->parents_sex = sex;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 親に所有されているかどうかのフラグ 
     * @param flag
     */
    void Accessor::SetOwnedOthersFlag( bool flag )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockC( true )->owned_by_others = flag;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }
    /**
     * @brief 親以外の仲良しトレーナーのID
     */
    void Accessor::SetFamiliarTrainerID( u16 trainer_id )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockC( true )->familiar_trainer_id = trainer_id;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 最初の親のトレーナーとのなつき度
     */
    void Accessor::SetOriginalFamiliarity( u8 familiarity )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->familiarity = familiarity;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 親以外のトレーナーとのなつき度
     */
    void Accessor::SetOthersFamiliarity( u8 familiarity )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockC( true )->others_familiarity = familiarity;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 親以外のなつきトレーナーID
     */
    void Accessor::SetFriendlyTrainerID( u16 trainer_id )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockC( true )->friendly_trainer_id = trainer_id;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 親以外のトレーナーとのなつき度
     */
    void Accessor::SetOthersFriendship( u8 friendship )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockC( true )->others_friendship = friendship;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 親との思い出レベル
     */
    void Accessor::SetMemoriesLevel( u8 level )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->memories_level = level;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 親との思い出事象コード
     */
    void Accessor::SetMemoriesCode( u8 code )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->memories_code = code;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 親との思い出内容データ
     */
    void Accessor::SetMemoriesData( u16 data )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->memories_data = data;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 親との思い出気持ちデータ
     */
    void Accessor::SetMemoriesFeel( u8 feel )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->memories_feel = feel;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 親以外のトレーナーとの思い出レベル
     */
    void Accessor::SetOthersMemoriesLevel( u8 level )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockC( true )->others_memories_level = level;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 親以外のトレーナーとの思い出事象コード
     */
    void Accessor::SetOthersMemoriesCode( u8 code )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockC( true )->others_memories_code = code;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 親以外のトレーナーとの思い出内容データ
     */
    void Accessor::SetOthersMemoriesData( u16 data )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockC( true )->others_memories_data = data;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 親以外のトレーナーとの思い出気持ちデータ
     */
    void Accessor::SetOthersMemoriesFeel( u8 feel )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockC( true )->others_memories_feel = feel;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 過去の親の名前
     */
    void Accessor::SetPastParentsName( const STRCODE* name )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      gfl2::str::StrNCopy( this->GetCoreDataBlockC( true )->past_parents_name, name , PERSON_NAME_BUFFER_SIZE );
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 過去の親の性別
     */
    void Accessor::SetPastParentsSex( Sex sex )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockC( true )->past_parents_sex = sex;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief トレーニング勲章
     */
    void Accessor::SetTrainingBadge1( u32 badge )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      // @fix GFNMCat5790 対応
      this->GetCoreDataBlockA( true )->training_badge1 = badge;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }
    void  Accessor::SetTrainingBadge2( u16 badge )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      // @fix GFNMCat5790 対応
      this->GetCoreDataBlockA( true )->training_badge2 = badge;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }
    /**
     * @brief トレーニング勲章ビットの立ち下げ
     * @param[in] index いくつ目かを示すインデックス
     * @param[in] flag  true:立てる　false:落とす
     */
    void  Accessor::SetTrainingBadge( s32 index, bool flag )
    {
      if ( index >= pml::MAX_TRAINING_BADGE )
      {
        GFL_ASSERT( 0 );  //@check 引数異常は何もしない
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      if( index < 32 )
      { 
        if( flag != false )
        { 
          // @fix GFNMCat5790 対応
          this->GetCoreDataBlockA( true )->training_badge1 |= ( 1 << index );
        }
        else
        { 
          // @fix GFNMCat5790 対応
          this->GetCoreDataBlockA( true )->training_badge1 &= ( ( 1 << index ) ^ 0xffffffff );
        }
      }
      else
      { 
        if( flag != false )
        { 
          // @fix GFNMCat5790 対応
          this->GetCoreDataBlockA( true )->training_badge2 |= ( 1 << ( index - 32 ) );
        }
        else
        { 
          // @fix GFNMCat5790 対応
          this->GetCoreDataBlockA( true )->training_badge2 &= ( ( 1 << ( index - 32 ) ) ^ 0xffff );
        }
      }
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief リボンを持っているかどうか
     */
    bool Accessor::HaveRibbon( u32 ribbon_no ) const
    {
      bool have = false;
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();

      if( ribbon_no <= MAX_RIBBON_NO_ON_RIBBON_FIELD_1 ) {
        u64 ribbon_bit = ( static_cast<u64>(1) << ( ribbon_no - MIN_RIBBON_NO_ON_RIBBON_FIELD_1 ) ); // @fix cov_ctr[10289]intでは精度が足りない為u64へキャスト
        u64 ribbon_field = this->GetCoreDataBlockA( false )->ribbon_a;
        have = ( ribbon_bit & ribbon_field )!=0?true:false;
      }
      else if( ribbon_no <= MAX_RIBBON_NO_ON_RIBBON_FIELD_2 ) {
        u64 ribbon_bit = ( static_cast<u64>(1) << ( ribbon_no - MIN_RIBBON_NO_ON_RIBBON_FIELD_2 ) ); // @fix cov_ctr[10289]intでは精度が足りない為u64へキャスト
        u64 ribbon_field = this->GetCoreDataBlockA( false )->ribbon_b;
        have = ( ribbon_bit & ribbon_field )!=0?true:false;
      }

      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();

      return have;
    } 

    /**
     * @brief リボン( 登録 )
     */
    void Accessor::SetRibbon( u32 ribbon_no )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite(); 

      if( ribbon_no <= MAX_RIBBON_NO_ON_RIBBON_FIELD_1 ) {
        u64 ribbon_bit = ( static_cast<u64>(1) << ( ribbon_no - MIN_RIBBON_NO_ON_RIBBON_FIELD_1 ) ); // @fix cov_ctr[10291]intでは精度が足りない為u64へキャスト
        this->GetCoreDataBlockA( true )->ribbon_a |= ribbon_bit;
      }
      else if( ribbon_no <= MAX_RIBBON_NO_ON_RIBBON_FIELD_2 ) {
        u64 ribbon_bit = ( static_cast<u64>(1) << ( ribbon_no - MIN_RIBBON_NO_ON_RIBBON_FIELD_2 ) ); // @fix cov_ctr[10291]intでは精度が足りない為u64へキャスト
        this->GetCoreDataBlockA( true )->ribbon_b |= ribbon_bit;
      }

      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    } 

    /**
     * @brief リボン( 解除 )
     */
    void Accessor::RemoveRibbon( u32 ribbon_no )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();

      if( ribbon_no <= MAX_RIBBON_NO_ON_RIBBON_FIELD_1 ) {
        u64 ribbon_bit = ( static_cast<u64>(1) << ( ribbon_no - MIN_RIBBON_NO_ON_RIBBON_FIELD_1 ) ); // @fix cov_ctr[10292]intでは精度が足りない為u64へキャスト
        this->GetCoreDataBlockA( true )->ribbon_a &= ( ~ribbon_bit );
      }
      else if( ribbon_no <= MAX_RIBBON_NO_ON_RIBBON_FIELD_2 ) {
        u64 ribbon_bit = static_cast<u64>(1) << ( ( ribbon_no - MIN_RIBBON_NO_ON_RIBBON_FIELD_2 ) ); // @fix cov_ctr[10292]intでは精度が足りない為u64へキャスト
        this->GetCoreDataBlockA( true )->ribbon_b &= ( ~ribbon_bit );
      }

      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    } 

    /**
     * @brief リボンを全てクリアする
     */
    void Accessor::RemoveAllRibbon( void )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->ribbon_a = 0;
      this->GetCoreDataBlockA( true )->ribbon_b = 0;
      this->GetCoreDataBlockA( true )->lumping_ribbon_a = 0;
      this->GetCoreDataBlockA( true )->lumping_ribbon_b = 0;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    } 
    
    /**
     * @brief ひとまとめリボン設定
     */
    void Accessor::SetLumpingRibbon( LumpingRibbon ribbon_id, u32 num )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite(); 

      if( ribbon_id == LUMPING_RIBBON_A )
      {
        this->GetCoreDataBlockA( true )->lumping_ribbon_a = num;
      }
      else if( ribbon_id == LUMPING_RIBBON_B ) {
        this->GetCoreDataBlockA( true )->lumping_ribbon_b = num;
      }
      else
      {
        GFL_ASSERT(0);  /* @check 引数異常は何もしない */
      }

      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief ひとまとめリボン取得
     */
    u32 Accessor::GetLumpingRibbon( LumpingRibbon ribbon_id ) const
    {
      u32 num = 0;
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();

      if( ribbon_id == LUMPING_RIBBON_A )
      {
        num = this->GetCoreDataBlockA( false )->lumping_ribbon_a;
      }
      else if( ribbon_id == LUMPING_RIBBON_B )
      {
        num = this->GetCoreDataBlockA( false )->lumping_ribbon_b;
      }
      else
      {
        GFL_ASSERT(0);  /* @check 引数異常は0を返す */
      }
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();

      return num;
    }

    /**
     * @brief 多目的ワークの値を設定
     */
    void Accessor::SetMultiPurposeWork( u32 value )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->multi_work = value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 多目的ワークの値を取得
     */
    u32 Accessor::GetMultiPurposeWork( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u32 num = this->GetCoreDataBlockA( false )->multi_work;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return num;
    }

    /**
     * @brief ポケモンリゾートのイベント状態を設定
     */
    void Accessor::SetResortEventStatus( u8 value )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA( true )->resort_event_status = value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief ポケモンリゾートのイベント状態を取得
     */
    u8 Accessor::GetResortEventStatus( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u16 num = this->GetCoreDataBlockA( false )->resort_event_status;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return num;
    }

    /**
     * @brief ポケデコを設定
     */
    void Accessor::SetPokeDeco( u16 value )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockB( true )->pokedeco = value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief ポケデコを取得
     */
    u16 Accessor::GetPokeDeco( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u16 num = this->GetCoreDataBlockB( false )->pokedeco;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return num;
    }

    /**
     * @brief すごい特訓をしたか？フラグを設定
     */
    void Accessor::SetTrainingFlag( u8 value )
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockD( true )->training_flag = value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief すごい特訓をしたか？フラグを取得
     */
    u8 Accessor::GetTrainingFlag( void ) const
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u8 num = this->GetCoreDataBlockD( false )->training_flag;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return num;
    }

    /**
     * @brief 汚れの種類を取得
     */
    DirtType Accessor::GetDirtType( void ) const
    {
      if( !this->HaveCalcData() ) {
        return DIRT_TYPE_NONE;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u8 value = this->GetCalcData( false )->dirt_type;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return static_cast<DirtType>( value );
    }

    /**
     * @brief 汚れの種類を設定
     */
    void Accessor::SetDirtType( DirtType value )
    {
      if( !this->HaveCalcData() ) {
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCalcData( true )->dirt_type = static_cast<u8>( value );
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

    /**
     * @brief 汚れの発生位置を取得
     */
    u8 Accessor::GetDirtPos( void ) const
    {
      if( !this->HaveCalcData() ) {
        return 0;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u8 value = this->GetCalcData( false )->dirt_pos;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return value;
    }

    /**
     * @brief 汚れの発生位置を設定
     */
    void Accessor::SetDirtPos( u8 value )
    {
      if( !this->HaveCalcData() ) {
        return;
      }
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCalcData( true )->dirt_pos = value;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }

#if PML_DEBUG
    /**
    * @brief デバッグで作成、編集されたか？
    */
    void Accessor::SetDebugEditFlag(bool flag)
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      this->GetCoreDataBlockA(true)->debug_edit_flag = flag;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
    }
    bool Accessor::GetDebugEditFlag(void)
    {
      const_cast<Accessor*>(this)->DecodeAndCheckIllegalWrite();
      u8 ret = this->GetCoreDataBlockA(true)->debug_edit_flag;
      const_cast<Accessor*>(this)->UpdateChecksumAndEncode();
      return (ret != 0 ? true : false);
    }
#endif //PML_DEBUG



    // 不正タマゴ用データ
    CalcData Accessor::IllegalCalcData;
    CoreDataBlockA Accessor::IllegalCoreBlockA;
    CoreDataBlockB Accessor::IllegalCoreBlockB;
    CoreDataBlockC Accessor::IllegalCoreBlockC;
    CoreDataBlockD Accessor::IllegalCoreBlockD;

    // 不正データ書き込みバッファ
    CalcData Accessor::DummyWriteCalcData;
    CoreDataBlockA Accessor::DummyWriteCoreBlockA;
    CoreDataBlockB Accessor::DummyWriteCoreBlockB;
    CoreDataBlockC Accessor::DummyWriteCoreBlockC;
    CoreDataBlockD Accessor::DummyWriteCoreBlockD;

    //-------------------------------------------------------------------------
    /**
     * @brief 初期化
     */
    //-------------------------------------------------------------------------
    void Accessor::Initialize( void )
    {
      gfl2::std::MemClear( &IllegalCalcData, sizeof(CalcData) );
      gfl2::std::MemClear( &IllegalCoreBlockA, sizeof(CoreDataBlockA) );
      gfl2::std::MemClear( &IllegalCoreBlockB, sizeof(CoreDataBlockB) );
      gfl2::std::MemClear( &IllegalCoreBlockC, sizeof(CoreDataBlockC) );
      gfl2::std::MemClear( &IllegalCoreBlockD, sizeof(CoreDataBlockD) );
      IllegalCalcData.max_hp = 1; // 念のため, ゼロ除算しないように1をセット
      IllegalCoreBlockA.monsno = MONSNO_TAMAGO;
    }

    /**
     * @brief 算出データを取り出す
     * @param for_write  true:書き込み用 false:読み出し用
     */
    CalcData* Accessor::GetCalcData( bool for_write ) const
    {
      if( this->IsFuseiTamago() ) 
      {
        return ( for_write ) ? &DummyWriteCalcData : &IllegalCalcData;
      }
      else
      {
        return m_pCalcData;
      }
    }

    /**
     * @brief コアパラメータブロックＡを取り出す
     * @param for_write  true:書き込み用 false:読み出し用
     */
    CoreDataBlockA* Accessor::GetCoreDataBlockA( bool for_write ) const
    {
      if( this->IsFuseiTamago() ) 
      {
        return ( for_write ) ? &DummyWriteCoreBlockA : &IllegalCoreBlockA;
      }
      else
      {
        u8 block_pos = this->GetCoreDataBlockPos( m_pCoreData->header.personal_rnd, CORE_DATA_BLOCK_A );
        return &( m_pCoreData->blocks[ block_pos ].block_a );
      }
    } 

    /**
     * @brief コアパラメータブロックＢを取り出す
     * @param for_write  true:書き込み用 false:読み出し用
     */
    CoreDataBlockB* Accessor::GetCoreDataBlockB( bool for_write ) const
    {
      if( this->IsFuseiTamago() ) 
      {
        return ( for_write ) ? &DummyWriteCoreBlockB : &IllegalCoreBlockB;
      }
      else
      {
        u8 block_pos = this->GetCoreDataBlockPos( m_pCoreData->header.personal_rnd, CORE_DATA_BLOCK_B );
        return &( m_pCoreData->blocks[ block_pos ].block_b );
      }
    } 

    /**
     * @brief コアデータブロックＣを取り出す
     * @param for_write  true:書き込み用 false:読み出し用
     */
    CoreDataBlockC* Accessor::GetCoreDataBlockC( bool for_write ) const
    {
      if( this->IsFuseiTamago() ) 
      {
        return ( for_write ) ? &DummyWriteCoreBlockC : &IllegalCoreBlockC;
      }
      else
      {
        u8 block_pos = this->GetCoreDataBlockPos( m_pCoreData->header.personal_rnd, CORE_DATA_BLOCK_C );
        return &( m_pCoreData->blocks[ block_pos ].block_c );
      }
    } 

    /**
     * @brief コアデータブロックＤを取り出す
     * @param for_write  true:書き込み用 false:読み出し用
     */
    CoreDataBlockD* Accessor::GetCoreDataBlockD( bool for_write ) const
    {
      if( this->IsFuseiTamago() ) 
      {
        return ( for_write ) ? &DummyWriteCoreBlockD : &IllegalCoreBlockD;
      }
      else
      {
        u8 block_pos = this->GetCoreDataBlockPos( m_pCoreData->header.personal_rnd, CORE_DATA_BLOCK_D );
        return &( m_pCoreData->blocks[ block_pos ].block_d );
      }
    } 

    /**
     * @brief 目的のコアパラメータブロックが, ４ブロック中の何番目に位置するのかを計算する
     * @param key       データ入れ替えのキー
     * @param block_id  取得したい主要データブロックのID
     * @return 指定したパラメータブロックの位置
     */
    u8 Accessor::GetCoreDataBlockPos( u32 key, CoreDataBlockId block_id ) const
    {
      enum {
        POS1,
        POS2,
        POS3,
        POS4,
      };

      static const u8 BLOCK_POS_TABLE[ 32 ][ CORE_DATA_BLOCK_NUM ] =
      {
        { POS1, POS2, POS3, POS4 },
        { POS1, POS2, POS4, POS3 },
        { POS1, POS3, POS2, POS4 },
        { POS1, POS4, POS2, POS3 },
        { POS1, POS3, POS4, POS2 },
        { POS1, POS4, POS3, POS2 },
        { POS2, POS1, POS3, POS4 },
        { POS2, POS1, POS4, POS3 },
        { POS3, POS1, POS2, POS4 },
        { POS4, POS1, POS2, POS3 },
        { POS3, POS1, POS4, POS2 },
        { POS4, POS1, POS3, POS2 },
        { POS2, POS3, POS1, POS4 },
        { POS2, POS4, POS1, POS3 },
        { POS3, POS2, POS1, POS4 },
        { POS4, POS2, POS1, POS3 },
        { POS3, POS4, POS1, POS2 },
        { POS4, POS3, POS1, POS2 },
        { POS2, POS3, POS4, POS1 },
        { POS2, POS4, POS3, POS1 },
        { POS3, POS2, POS4, POS1 },
        { POS4, POS2, POS3, POS1 },
        { POS3, POS4, POS2, POS1 },
        { POS4, POS3, POS2, POS1 },
        { POS1, POS2, POS3, POS4 },
        { POS1, POS2, POS4, POS3 },
        { POS1, POS3, POS2, POS4 },
        { POS1, POS4, POS2, POS3 },
        { POS1, POS3, POS4, POS2 },
        { POS1, POS4, POS3, POS2 },
        { POS2, POS1, POS3, POS4 },
        { POS2, POS1, POS4, POS3 },
      };

      u16 rnd = ( key & 0x0003e000 ) >> 13;

      GFL_ASSERT( rnd < 32 ); //@check アルゴリズム検証用
      GFL_ASSERT( block_id <= CORE_DATA_BLOCK_NUM );  //@check プログラム検証用

      return BLOCK_POS_TABLE[ rnd ][ block_id ];
    }



  } // namespace pokepara
} // namespace pml
