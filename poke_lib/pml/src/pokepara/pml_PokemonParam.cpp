//=============================================================================
/**
 * @brief  ポケモンパラメータ 
 * @file   pml_PokemonParam.cpp
 * @author obata_toshihiro
 * @date   2011.01.05
 */
//=============================================================================
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include "pml/include/pml_Const.h"
#include "pml/include/pokepara/pml_PokemonParam.h"
#include "pml/include/pokepara/pml_PokemonParamFactory.h"
#include "./pml_PokemonParamAccessor.h"


namespace pml {
  namespace pokepara {




    /// コピー処理でのDecodeに使用するテンポラリバッファ
    u8 s_param_serialize_buffer[ PokemonParam::DATASIZE ];

    //-------------------------------------------------------------------------
    /**
     * @brief バッファ確保用に定義した DATASIZE の正当性チェック
     */
    //-------------------------------------------------------------------------
    void PokemonParam::CheckPublicDataSize( void )
    {
      GFL_ASSERT_STOP( DATASIZE == Accessor::FULL_SERIALIZE_DATA_SIZE ); 
    }



    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap インスタンスを確保するヒープ
     */
    //-------------------------------------------------------------------------
    PokemonParam::PokemonParam( gfl2::heap::HeapBase* heap ) :
      CoreParam( heap )
    {
      this->CreateAndAttachCalcData( heap );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ( 簡易 )
     * @param heap    インスタンスを確保するヒープ
     * @param monsno  モンスターNo.
     * @param level   レベル[0, pml::pokepara::MAX_POKE_LEVEL]
     * @param id      IDNo.
     */
    //-------------------------------------------------------------------------
    PokemonParam::PokemonParam( gfl2::heap::HeapBase* heap, MonsNo monsno, u16 level, u64 id ) :
      CoreParam( heap, monsno, level, id )
    {
      this->CreateAndAttachCalcData( heap );
      this->InitCalcData();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ( 詳細 )
     * @param heap  インスタンスを確保するヒープ
     * @param spec  パラメータの初期設定
     */
    //-------------------------------------------------------------------------
    PokemonParam::PokemonParam( gfl2::heap::HeapBase* heap, const InitialSpec& spec ) :
      CoreParam( heap, spec )
    {
      this->CreateAndAttachCalcData( heap );
      this->InitCalcData();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ( 複製 )
     * @param heap インスタンスを確保するヒープ
     * @param src  複製元のパラメータ
     */
    //-------------------------------------------------------------------------
    PokemonParam::PokemonParam( gfl2::heap::HeapBase* heap, const PokemonParam& src ) :
      CoreParam( heap )
    {
      this->CreateAndAttachCalcData( heap );
      this->PokemonParam::CopyFrom( src );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ( コアパラメータからの拡張 )
     * @param heap インスタンスを確保するヒープ
     * @param src  複製元のパラメータ
     */
    //-------------------------------------------------------------------------
    PokemonParam::PokemonParam( gfl2::heap::HeapBase* heap, const CoreParam& src ) :
      CoreParam( heap )
    {
      this->CreateAndAttachCalcData( heap );
      this->CoreParam::CopyFrom( src );
      this->InitCalcData();
    }

    /**
     * @brief 算出データを生成・アタッチする
     * @param heap データの生成に使用するヒープ
     */
    void PokemonParam::CreateAndAttachCalcData( gfl2::heap::HeapBase* heap )
    {
      u32 personal_rnd = this->GetPersonalRnd();
      m_calcData = Factory::CreateCalcData( heap, personal_rnd );
      m_accessor->AttachEncodedData( m_coreData, m_calcData );
    }

    /**
     * @brief 算出データを初期化する
     */
    void PokemonParam::InitCalcData( void )
    {
      bool valid_flag = this->StartFastMode();

      //CoreParam::CopyFromを使うPokemonParam(heap,CoreParam）などで
      //CoreParamから算出した際に、CalcDataは破壊されている場合もあるので、
      //クリア処理を読んでおく
      m_accessor->ClearCalcData();
      this->UpdateCalcDatas();
      this->RecoverAll();

      this->EndFastMode( valid_flag );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    PokemonParam::~PokemonParam()
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief セットアップ
     * @param spec  パラメータの初期設定
     */
    //-------------------------------------------------------------------------
    void PokemonParam::Setup( const InitialSpec& spec )
    {
      Factory::SetupCoreData( m_coreData, spec );
      this->InitCoreData();
      this->InitCalcData();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief パラメータをコピーする
     * @param src_param  コピー元となるパラメータ
     */
    //-------------------------------------------------------------------------
    void PokemonParam::CopyFrom( const PokemonParam& src_param )
    {
      src_param.Serialize_Full( s_param_serialize_buffer );
      this->Deserialize_Full( s_param_serialize_buffer );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief シリアライズデータを取得する
     * @param buffer シリアライズデータの格納先( SERIALIZE_DATA_SIZE の大きさが必要 )
     */
    //-------------------------------------------------------------------------
    void PokemonParam::Serialize_Full( void* buffer ) const
    {
      m_accessor->Serialize_FullData( buffer );
    } 
    /**
     * @brief シリアライズ処理（CoreParam版）
     * @param[out]  buffer    展開先
     * @note  DATASIZE分領域を確保すること
     */
    void PokemonParam::Serialize_Core( void* buffer ) const
    {
      this->CoreParam::Serialize_Core( buffer );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief シリアライズデータにより復元する
     * @param serialized_data Serialize_Full()で取得したシリアライズデータ
     */
    //-------------------------------------------------------------------------
    void PokemonParam::Deserialize_Full( const void* serialized_data )
    {
      m_accessor->Deserialize_FullData( serialized_data );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief シリアライズデータにより復元する
     * @param serialized_data Serialize_Core()で取得したシリアライズデータ
     */
    //-------------------------------------------------------------------------
    void PokemonParam::Deserialize_Core( const void* serialized_data )
    {
      this->CoreParam::Deserialize_Core( serialized_data );
      this->InitCalcData();
    } 

#if PML_DEBUG
    //-------------------------------------------------------------------------
    /**
     * @brief パラメータのデバッグ出力
     */
    //-------------------------------------------------------------------------
    void PokemonParam::Dump() const
    {
      GFL_PRINT("-------------------------------\n");
      GFL_PRINT("-- PokemonParam Dump ----------\n");
      GFL_PRINT("-------------------------------\n");

      GFL_PRINT("MonsNo:%d\t", GetMonsNo() );
      GFL_PRINT("FormNo:%d\n", GetFormNo() );
      // 種族名
      {
        STRCODE default_name[ MONS_NAME_BUFFER_SIZE ];
        pml::personal::GetMonsName( default_name, GetMonsNo() );
        gfl2::str::PrintCode( default_name, "MonsName:" );
      }
      GFL_PRINT("Level: %d\t", GetLevel() );
      // 性別
      switch( GetSex() ){
      case SEX_MALE:    GFL_PRINT("Sex: MALE\n"); break;
      case SEX_FEMALE:  GFL_PRINT("Sex: FEMALE\n"); break;
      case SEX_UNKNOWN: GFL_PRINT("Sex: SEX_UNKNOWN\n"); break;
      }

      for( u8 wazaIdx=0; wazaIdx<GetWazaCount(); wazaIdx++ )
      {
        GFL_PRINT("Waza%d[%d] ", wazaIdx, GetWazaNo( wazaIdx ) );
      }
      GFL_PRINT("\n");
      
      // タマゴ
      GFL_PRINT("IsEgg:%d  dame:%d\n", IsEgg(CHECK_BOTH_EGG), IsEgg(CHECK_ONLY_ILLEGAL_EGG) );
      GFL_PRINT("Item: %d\n", GetItem() );

      //デバッグ生成フラグ
      GFL_PRINT("IsDebugEdit:%d\n", GetDebugEditFlag());

      GFL_PRINT("-------------------------------\n");
    }
#endif


  } //namespace pokepara
} //namespace pml
