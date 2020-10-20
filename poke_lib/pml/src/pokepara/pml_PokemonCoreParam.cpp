//=============================================================================
/**
 * @brief  ポケモンパラメータ( 算出パラメータを除くボックスパラメータ )
 * @file   pml_PokemonCoreParam.cpp
 * @author obata_toshihiro
 * @date   2011.03.06
 */
//=============================================================================
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_math.h>
#include <debug/include/gfl2_Assert.h>
#include <pml/include/pml_Local.h>
#include <pml/include/pml_Type.h>
#include <pml/include/pml_Const.h>
#include <pml/include/pml_Personal.h>
#include <pml/include/pml_Waza.h>
#include <pml/include/pml_System.h>
#include "pml_PokemonParamCalcTool.h"
#include "pml_PokemonParamAccessor.h"
#include "pml_PokemonParamLocal.h"
#include <pml/include/pokepara/pml_EvolveManager.h>
#include <pml/include/pokepara/pml_PokemonCoreParam.h>
#include <pml/include/pokepara/pml_PokemonParamFactory.h>
#include "pml/include/system/pml_Library.h"
#include "pml_PokemonParam_MultiPurposeWorkController.h"
#include "pml_PokemonParam_BoxMarkController.h"

#include <pml/include/item/item.h>


namespace pml {
  namespace pokepara {


    enum {
      BIRTH_FAMILIARITY = 120,   // タマゴが孵化した時のなつき度
    };


    /// コピー処理でのDecodeに使用するテンポラリバッファ
    u8 s_core_serialize_buffer[ CoreParam::DATASIZE ];

    // 進化マネージャ
    EvolveManager* CoreParam::s_evolve_manager = NULL; 


    //-------------------------------------------------------------------------
    /**
     * @brief 進化マネージャを設定する
     * @param manager 関連付けるマネージャ
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetEvolveManager( EvolveManager* manager )
    {
      s_evolve_manager = manager;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief セーブデータ用に定義した DATASIZE の正当性チェック
     */
    //-------------------------------------------------------------------------
    void CoreParam::CheckPublicDataSize( void )
    {
      GFL_ASSERT_STOP( CoreParam::DATASIZE == Accessor::CORE_SERIALIZE_DATA_SIZE );//@check 常にチェックされている
    }

    //-------------------------------------------------------------------------
    /**
     * @brief IDとカラー乱数からレアになる組み合わせかチェック
     */
    //-------------------------------------------------------------------------
    bool CoreParam::IsRareFromValue( u32 id, u32 color_rnd )
    {
      return CalcTool::IsRareColor( id, color_rnd );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap インスタンスを確保するヒープ
     */
    //-------------------------------------------------------------------------
    CoreParam::CoreParam( gfl2::heap::HeapBase* heap ) :
      m_coreData( NULL ),
      m_calcData( NULL ),
      m_accessor( NULL )
    {
      m_coreData = Factory::CreateCoreData( heap );
      m_accessor = GFL_NEW( heap ) Accessor();
      m_accessor->AttachDecodedData( m_coreData, NULL );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ( 簡易版 )
     * @param heap    インスタンスを確保するヒープ
     * @param monsno  モンスターNo.
     * @param level   レベル[0, pml::pokepara::MAX_POKE_LEVEL]
     * @param id      IDNo.
     */
    //-------------------------------------------------------------------------
    CoreParam::CoreParam( gfl2::heap::HeapBase* heap, MonsNo monsno, u16 level, u64 id ) :
      m_coreData( NULL ),
      m_calcData( NULL ),
      m_accessor( NULL )
    {
      m_coreData = Factory::CreateCoreData( heap, monsno, level, id ); 
      m_accessor = GFL_NEW( heap ) Accessor();
      m_accessor->AttachEncodedData( m_coreData, NULL );
      this->InitCoreData(); 
    }

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ( 詳細版 )
     * @param heap  インスタンスを確保するヒープ
     * @param spec  パラメータの初期設定
     */
    //-------------------------------------------------------------------------
    CoreParam::CoreParam( gfl2::heap::HeapBase* heap, const InitialSpec& spec ) :
      m_coreData( NULL ),
      m_calcData( NULL ),
      m_accessor( NULL )
    {
      m_coreData = Factory::CreateCoreData( heap, spec );
      m_accessor = GFL_NEW( heap ) Accessor();
      m_accessor->AttachEncodedData( m_coreData, NULL );
      this->InitCoreData();
    }

    /**
     * @brief コアデータを初期化する
     */
    void CoreParam::InitCoreData( void )
    {
      this->SetDefaultWaza();     // デフォルト技をセット
      this->SetDefaultNickName(); // ニックネームにモンスター名をセット
      this->RecoverAll();
      this->SetCountryCode( Library::m_setting->countryCode ); //ポケモンライブラリに渡された国コードをセット
      this->SetCountryRegionCode( Library::m_setting->countryRegionCode );  //ポケモンライブラリに渡された地域コードをセット
      this->SetLangId( Library::m_setting->langId );  //ポケモンライブラリに渡された言語コードをセット
      this->SetHWRegionCode( Library::m_setting->hwRegionCode );  //ポケモンライブラリに渡された本体リージョンをセット
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンパラメータを生成する( コアパラメータ版 )
     * @param heap  インスタンスの生成に使用するアロケータ
     * @param src   複製元となるパラメータ
     */
    //-------------------------------------------------------------------------
    CoreParam::CoreParam( gfl2::heap::HeapBase* heap, const CoreParam& src ) :
      m_coreData( NULL ),
      m_calcData( NULL ),
      m_accessor( NULL )
    {
      m_coreData = Factory::CreateCoreData( heap );
      m_accessor = GFL_NEW( heap ) Accessor();
      m_accessor->AttachEncodedData( m_coreData, NULL );
      this->CopyFrom( src );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    CoreParam::~CoreParam()
    {
      GFL_SAFE_DELETE( m_coreData );
      GFL_SAFE_DELETE( m_calcData );
      GFL_SAFE_DELETE( m_accessor );
    }





    //-------------------------------------------------------------------------
    /**
     * @brief シリアライズデータを取得する
     * @param buffer  シリアライズデータの格納先( CoreParam::DATASIZE の大きさが必要 )
     */
    //-------------------------------------------------------------------------
    void CoreParam::Serialize_Core( void* buffer ) const
    {
      m_accessor->Serialize_CoreData( buffer );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief シリアライズデータにより復元する
     * @param serialized_data  Serialize() で取得したシリアライズデータ
     */
    //-------------------------------------------------------------------------
    void CoreParam::Deserialize_Core( const void* serialized_data )
    {
      m_accessor->Deserialize_CoreData( serialized_data );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief パラメータをゼロクリアする
     *
     * @note MONSNO_NULLを指定して生成した状態になります.
     */
    //-------------------------------------------------------------------------
    void CoreParam::Clear( void )
    {
      m_accessor->ClearData();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 空パラメータかどうかを調べる
     * @retval true   空パラメータ( MONSNO_NULLを指定して初期化した状態 )の場合
     * @retval false  何らかのポケモンのパラメータの場合
     *
     * @note モンスターNo.がMONSNO_NULLかどうか、だけで判定しています.
     */
    //-------------------------------------------------------------------------
    bool CoreParam::IsNull( void ) const
    {
      return ( this->GetMonsNo() == MONSNO_NULL );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 算出パラメータを含んでいるか？
     * @retval true  算出パラメータを含んでいる
     * @retval false コアパラメータのみ
     */
    //-------------------------------------------------------------------------
    bool CoreParam::HaveCalcParam( void ) const
    {
      return m_accessor->HaveCalcData();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief パラメータをコピーする
     * @param src_param  コピー元となるパラメータ
     */
    //-------------------------------------------------------------------------
    void CoreParam::CopyFrom( const CoreParam& src_param )
    {
      src_param.Serialize_Core( s_core_serialize_buffer );
      this->Deserialize_Core( s_core_serialize_buffer );
    }







    //-------------------------------------------------------------------------
    /**
     * @brief 所持アイテムを取得する
     */
    //-------------------------------------------------------------------------
    u16 CoreParam::GetItem( void ) const
    {
      return m_accessor->GetItemNo();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 所持アイテムを設定する
     * @param u16  セットするアイテム番号
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetItem( u16 itemno )
    {
      m_accessor->SetItemNo( itemno ); 
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 道具を持っていない状態にする
     */
    //-------------------------------------------------------------------------
    void CoreParam::RemoveItem( void )
    {
      this->SetItem( ITEM_DUMMY_DATA );
    }



    //-------------------------------------------------------------------------
    /**
     * @brief ボックスマークの有無をチェックする
     * @param mark  チェックするマークの種類
     * @retval true   いずれかのマークが登録されている
     * @retval false  いずれかのマークが登録されていない
     */
    //-------------------------------------------------------------------------
    bool CoreParam::IsBoxMarkSet( void ) const
    {
      for( u32 i=0; i<BOX_MARK_NUM; ++i )
      {
        BoxMark mark = static_cast<BoxMark>( i );
        if( this->IsBoxMarkSet( mark ) ) {
          return true;
        }
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボックスマークの有無をチェックする
     * @param mark  チェックするマークの種類
     * @retval true   指定したマークが登録されている
     * @retval false  指定したマークが登録されていない
     */
    //-------------------------------------------------------------------------
    bool CoreParam::IsBoxMarkSet( BoxMark mark ) const
    {
      if( BOX_MARK_NUM <= mark )
      {
        GFL_ASSERT(0);
        return false;
      }

      BoxMarkColor color = this->GetBoxMark( mark );
      return ( color != BOX_MARK_COLOR_NONE );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボックスマークを設定する
     * @param mark  設定するマークの種類
     * @param color 設定するマークの色
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetBoxMark( BoxMark mark, BoxMarkColor color )
    {
      if( BOX_MARK_NUM <= mark )
      {
        GFL_ASSERT(0);
        return;
      }

      if( BOX_MARK_COLOR_NUM <= color )
      {
        GFL_ASSERT(0);
        return;
      }

      u16 old_value = m_accessor->GetBoxMark();
      u16 new_value = BoxMarkController::SetBoxMarkColor( old_value, mark, color );
      m_accessor->SetBoxMark( new_value );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ボックスマークを解除する
     * @param mark  解除するマーク
     */
    //-------------------------------------------------------------------------
    void CoreParam::RemoveBoxMark( BoxMark mark )
    {
      this->SetBoxMark( mark, BOX_MARK_COLOR_NONE );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ボックスマークを取得する
     * @param mark  取得対象のマークの種類
     * @return 指定したマークの色
     */
    //-------------------------------------------------------------------------
    BoxMarkColor CoreParam::GetBoxMark( BoxMark mark ) const
    {
      u16 value = m_accessor->GetBoxMark();
      return BoxMarkController::GetBoxMarkColor( value, mark );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 全てのボックスマークを解除する
     */
    //-------------------------------------------------------------------------
    void CoreParam::RemoveAllBoxMark( void )
    {
      for( u32 i=0; i<BOX_MARK_NUM; ++i )
      {
        BoxMark mark = static_cast<BoxMark>( i );
        this->RemoveBoxMark( mark );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 全てのボックスマークをセットする
     * @param markContainer  セットするマーク
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetAllBoxMark( const BoxMarkContainer& markContainer )
    {
      for( u32 i=0; i<BOX_MARK_NUM; ++i )
      {
        BoxMark mark = static_cast<BoxMark>( i );
        BoxMarkColor color = markContainer.markColor[ mark ];
        this->SetBoxMark( mark, color );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 全てのボックスマークを取得する
     * @param[out] markContainer  取得したマークの格納先
     */
    //-------------------------------------------------------------------------
    void CoreParam::GetAllBoxMark( BoxMarkContainer* markContainer ) const
    {
      for( u32 i=0; i<BOX_MARK_NUM; ++i )
      {
        BoxMark mark = static_cast<BoxMark>( i );
        markContainer->markColor[ mark ] = this->GetBoxMark( mark );
      }
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 国コードを取得する
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetCountryCode( void ) const
    {
      return m_accessor->GetCountryCode();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 国コードを設定する
     * @param country 設定する国コード
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetCountryCode( u32 country )
    {
      m_accessor->SetCountryCode( country );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 地域コードを取得する
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetCountryRegionCode( void ) const
    {
      return m_accessor->GetCountryRegionCode();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 地域コードを設定する
     * @param country_region 設定する地域コード
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetCountryRegionCode( u32 country_region )
    {
      m_accessor->SetCountryRegionCode( country_region );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 本体リージョンを取得する
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetHWRegionCode( void ) const
    {
      return m_accessor->GetHWRegionCode();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 本体リージョンを設定する
     * @param hw_region_code 設定する本体リージョン
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetHWRegionCode( u32 hw_region_code )
    {
      m_accessor->SetHWRegionCode( hw_region_code );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 言語IDを取得する
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetLangId( void ) const
    {
      return m_accessor->GetLangId();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 言語IDを設定する
     * @param country_region 設定する言語ID
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetLangId( u32 country_region )
    {
      m_accessor->SetLangId( country_region );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 捕まえたカセットバージョンを取得する
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetCassetteVersion( void ) const
    {
      return m_accessor->GetCassetteVersion();
    } 
    //-------------------------------------------------------------------------
    /**
     * @brief 捕まえたカセットバージョンを設定
     * @param u32 バージョン
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetCassetteVersion( u32 version )
    {
      m_accessor->SetCassetteVersion( version );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 捕まえたボールを取得する
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetGetBall( void ) const
    {
      return m_accessor->GetGetBall();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 捕まえたボールを取得する
     * @param ball ボールID
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetGetBall( u32 ball )
    {
      m_accessor->SetGetBall(ball);
    }





    //-------------------------------------------------------------------------
    /**
     * @brief 満腹度を取得する
     */
    //-------------------------------------------------------------------------
    u8 CoreParam::GetFeed( void ) const
    {
      return m_accessor->GetFeed();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 満腹度を設定する
     * @param value  セットする値[0, MAX_FEED]
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetFeed( u32 value )
    {
      if( value < MAX_FEED ) {
        m_accessor->SetFeed( value );
      }
      else {
        m_accessor->SetFeed( MAX_FEED );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 満腹度を増やす
     * @param value  加算値
     */
    //-------------------------------------------------------------------------
    void CoreParam::AddFeed( u8 value )
    {
      u32 now = this->GetFeed();
      this->SetFeed( now + value );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 満腹度を減らす
     * @param value  減算値
     */
    //-------------------------------------------------------------------------
    void CoreParam::SubFeed( u8 value )
    {
      u32 now = this->GetFeed();
      u32 after = ( now < value ) ? 0 : (now - value);
      this->SetFeed( after );
    }






    //-------------------------------------------------------------------------
    /**
     * @brief なでなで値を取得する
     */
    //-------------------------------------------------------------------------
    u8 CoreParam::GetNadenadeValue( void ) const
    {
      return m_accessor->GetNadenadeValue();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief なでなで値を設定する
     * @param value  セットする値[0, MAX_NADENADE_VALUE]
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetNadenadeValue( u8 value )
    {
      if( value < MAX_NADENADE_VALUE ) {
        m_accessor->SetNadenadeValue( value );
      }
      else {
        m_accessor->SetNadenadeValue( MAX_NADENADE_VALUE );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief なでなで値を増やす
     * @param value  加算値
     */
    //-------------------------------------------------------------------------
    void CoreParam::AddNadenadeValue( u8 value )
    {
      u32 now = this->GetNadenadeValue();
      this->SetNadenadeValue( now + value );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief なでなで値を減らす
     * @param value  減算値
     */
    //-------------------------------------------------------------------------
    void CoreParam::SubNadenadeValue( u8 value )
    {
      u32 now = this->GetNadenadeValue();
      u32 after = ( now < value ) ? 0 : (now - value);
      this->SetNadenadeValue( after );
    }



    //-------------------------------------------------------------------------
    /**
     * @brief 汚れの種類を取得する
     * @retval DIRT_TYPE_NONE  汚れが無い場合
     */
    //-------------------------------------------------------------------------
    DirtType CoreParam::GetDirtType( void ) const
    {
      if( this->CheckDirt() == false )
      {
        return DIRT_TYPE_NONE;
      }

      return m_accessor->GetDirtType();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 汚れの種類を設定する
     * @param dirt  設定する汚れの種類
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetDirtType( DirtType  dirt )
    {
      if( dirt == DIRT_TYPE_NONE )
      {
        this->ClearDirt();
      }
      else
      {
        m_accessor->SetDirtType( dirt );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 汚れの有無をチェックする
     * @retval true    汚れている
     * @retval false   汚れていない
     */
    //-------------------------------------------------------------------------
    bool CoreParam::CheckDirt( void ) const
    {
      // 汚れている場所が1箇所でもあるなら、汚れていると判定
      for( u32 posIndex=0; posIndex<MAX_DIRT_POS_NUM; ++posIndex )
      {
        if( this->CheckDirtPos( posIndex ) )
        {
          return true;
        }
      }

      // 汚れている場所が無い
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 汚れの有無をチェックする
     * @param チェック対象の汚れ位置[ 0, MAX_DIRT_POS_NUM - 1 ]
     * @retval true    指定した位置に汚れが有る
     * @retval false   指定した位置に汚れが無い
     */
    //-------------------------------------------------------------------------
    bool CoreParam::CheckDirtPos( u8 posIndex ) const
    {
      if( MAX_DIRT_POS_NUM <= posIndex )
      {
        GFL_ASSERT(0);
        return false;
      }

      // 汚れの種類が DIRT_TYPE_NONE なら、汚れていないと判定
      if( m_accessor->GetDirtType() == DIRT_TYPE_NONE )
      {
        return false;
      }

      const u8 pos_bit   = m_accessor->GetDirtPos();
      const u8 check_bit = ( 1 << posIndex );
      return ( ( pos_bit & check_bit ) != 0 );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 指定位置に汚れを設定する
     * @param チェック対象の汚れ位置[ 0, MAX_DIRT_POS_NUM - 1 ]
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetDirtPos( u8 posIndex )
    {
      if( MAX_DIRT_POS_NUM <= posIndex )
      {
        GFL_ASSERT(0);
        return;
      }

      const u8 target_bit  = ( 1 << posIndex );
      const u8 pos_bit_old = m_accessor->GetDirtPos();
      const u8 pos_bit_new = pos_bit_old | target_bit;
      m_accessor->SetDirtPos( pos_bit_new );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 指定位置の汚れを取り除く
     * @param 操作対象の汚れ位置[ 0, MAX_DIRT_POS_NUM - 1 ]
     */
    //-------------------------------------------------------------------------
    void CoreParam::RemoveDirtPos( u8 posIndex )
    {
      if( MAX_DIRT_POS_NUM <= posIndex )
      {
        GFL_ASSERT(0);
        return;
      }

      const u8 target_bit  = ( 1 << posIndex );
      const u8 pos_bit_old = m_accessor->GetDirtPos();
      const u8 pos_bit_new = pos_bit_old & (~target_bit);
      m_accessor->SetDirtPos( pos_bit_new );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 汚れが無い状態にする
     */
    //-------------------------------------------------------------------------
    void CoreParam::ClearDirt( void )
    {
      m_accessor->SetDirtType( DIRT_TYPE_NONE );
      m_accessor->SetDirtPos( 0 );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief トリミアン：体毛カウントを取得する
     */
    //-------------------------------------------------------------------------
    u8 CoreParam::TORIMIAN_GetBodyHairCount( void ) const
    {
      const u32 work = m_accessor->GetMultiPurposeWork();
      return MultiPurposeWorkController::TORIMIAN_GetBodyHairCount( work );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief  トリミアン：日数カウントを取得する
     */
    //-------------------------------------------------------------------------
    u8 CoreParam::TORIMIAN_GetKeepDayCount( void ) const
    {
      const u32 work = m_accessor->GetMultiPurposeWork();
      return MultiPurposeWorkController::TORIMIAN_GetKeepDayCount( work );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief  トリミアン：経過日数カウントを取得する
     */
    //-------------------------------------------------------------------------
    u8 CoreParam::TORIMIAN_GetKeepMaxDayCount( void ) const
    {
      //トリミアンでなければ意味がない
      if ( this->GetMonsNo() == MONSNO_TORIMIAN )
      {
        const u32 work = m_accessor->GetMultiPurposeWork();
        return MultiPurposeWorkController::TORIMIAN_GetMaxKeepDayCount( work );
      }
      return 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief トリミアン専用：体毛カウントをセットする
     * @param   u8  体毛カウント
     */
    //-------------------------------------------------------------------------
    void CoreParam::TORIMIAN_SetBodyHairCount( u8 hair_count )
    {
      const u32 work_old = m_accessor->GetMultiPurposeWork();
      const u32 work_new = MultiPurposeWorkController::TORIMIAN_SetBodyHairCount( work_old, hair_count );
      m_accessor->SetMultiPurposeWork( work_new );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief  トリミアン専用：日数カウントをセットする
     * @param   u8  日数カウント
     */
    //-------------------------------------------------------------------------
    void CoreParam::TORIMIAN_SetKeepDayCount( u8 day )
    {
      const u32 work_old = m_accessor->GetMultiPurposeWork();
      const u32 work_new = MultiPurposeWorkController::TORIMIAN_SetKeepDayCount( work_old, day );
      m_accessor->SetMultiPurposeWork( work_new );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief  トリミアン専用：経過日数カウントをセットする
     * @param   u8  経過日数カウント
     */
    //-------------------------------------------------------------------------
    void CoreParam::TORIMIAN_SetKeepMaxDayCount( u8 max_day )
    {
      //トリミアンでなければ意味がない
      if ( this->GetMonsNo() == MONSNO_TORIMIAN )
      {
        const u32 work_old = m_accessor->GetMultiPurposeWork();
        const u32 work_new = MultiPurposeWorkController::TORIMIAN_SetMaxKeepDayCount( work_old, max_day );
        m_accessor->SetMultiPurposeWork( work_new );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 新規・トレーニング勲章取得
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetTrainingBadge1( void ) const
    { 
      return m_accessor->GetTrainingBadge1();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 新規・トレーニング勲章取得
     */
    //-------------------------------------------------------------------------
    u16 CoreParam::GetTrainingBadge2( void ) const
    { 
      return m_accessor->GetTrainingBadge2();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 新規・トレーニング勲章を所持しているか取得
     * @param[in] index いくつ目かを示すインデックス
     */
    //-------------------------------------------------------------------------
    bool  CoreParam::IsTrainingBadge( s32 index ) const
    { 
      return m_accessor->IsTrainingBadge( index );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 新規・トレーニング勲章
     * @param[in] badge セットする値
     */
    //-------------------------------------------------------------------------
    void  CoreParam::SetTrainingBadge1( u32 badge )
    { 
      m_accessor->SetTrainingBadge1( badge );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 新規・トレーニング勲章
     * @param[in] badge セットする値
     */
    //-------------------------------------------------------------------------
    void  CoreParam::SetTrainingBadge2( u16 badge )
    { 
      m_accessor->SetTrainingBadge2( badge );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 新規・トレーニング勲章ビットの立ち下げ
     * @param[in] index いくつ目かを示すインデックス
     * @param[in] flag  セットする値
     */
    //-------------------------------------------------------------------------
    void  CoreParam::SetTrainingBadge( s32 index, bool flag )
    { 
      m_accessor->SetTrainingBadge( index, flag );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief コンディションの値を取得する
     * @param cond  取得するパラメータ
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetCondition( Condition cond ) const
    {
      switch( cond ) {
      case CONDITION_STYLE: return m_accessor->GetStyle();
      case CONDITION_BEAUTIFUL: return m_accessor->GetBeautiful();
      case CONDITION_CUTE: return m_accessor->GetCute();
      case CONDITION_CLEVER: return m_accessor->GetClever();
      case CONDITION_STRONG: return m_accessor->GetStrong();
      case CONDITION_FUR: return m_accessor->GetFur();
      }
      GFL_ASSERT(0);  //@check 引数異常は0を返して動作継続
      return 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief コンディションの値を変更する
     * @param cond   変更するパラメータ
     * @param value  セットする値
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetCondition( Condition cond, u32 value )
    {
      switch( cond ) 
      {
      case CONDITION_STYLE:
        if( MAX_STYLE < value ) { 
          m_accessor->SetStyle( MAX_STYLE ); 
        }
        else { 
          m_accessor->SetStyle( value ); 
        }
        break; 

      case CONDITION_BEAUTIFUL:
        if( MAX_BEAUTIFUL < value ) { 
          m_accessor->SetBeautiful( MAX_BEAUTIFUL ); 
        }
        else { 
          m_accessor->SetBeautiful( value ); 
        }
        break; 

      case CONDITION_CUTE:
        if( MAX_CUTE < value ) { 
          m_accessor->SetCute( MAX_CUTE );
        }
        else { 
          m_accessor->SetCute( value );
        }
        break; 

      case CONDITION_CLEVER:
        if( MAX_CLEVER < value ) { 
          m_accessor->SetClever( MAX_CLEVER );
        }
        else { 
          m_accessor->SetClever( value );
        }
        break; 

      case CONDITION_STRONG:
        if( MAX_STRONG < value ) { 
          m_accessor->SetStrong( MAX_STRONG );
        }
        else { 
          m_accessor->SetStrong( value );
        }
        break; 

      case CONDITION_FUR:
        if( MAX_FUR < value ) { 
          m_accessor->SetFur( MAX_FUR );
        }
        else { 
          m_accessor->SetFur( value );
        }
        break; 
      }
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 思い出データを取得する
     * @param memories_kind  取得するデータの種類
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetMemories( Memories memories_kind ) const
    {
      switch( memories_kind ) 
      {
      case MEMORIES_EGG_TAKEN_YEAR:       return m_accessor->GetTamagoGetYear();
      case MEMORIES_EGG_TAKEN_MONTH:      return m_accessor->GetTamagoGetMonth();
      case MEMORIES_EGG_TAKEN_DAY:        return m_accessor->GetTamagoGetDay();
      case MEMORIES_EGG_TAKEN_PLACE:      return m_accessor->GetGetPlace();
      case MEMORIES_FIRST_CONTACT_YEAR:   return m_accessor->GetBirthYear();
      case MEMORIES_FIRST_CONTACT_MONTH:  return m_accessor->GetBirthMonth();
      case MEMORIES_FIRST_CONTACT_DAY:    return m_accessor->GetBirthDay();
      case MEMORIES_FIRST_CONTACT_PLACE:  return m_accessor->GetBirthPlace();
      case MEMORIES_CAPTURED_BALL:        return m_accessor->GetGetBall();
      case MEMORIES_CAPTURED_LEVEL:       return m_accessor->GetGetLevel();
      case MEMORIES_LEVEL_WITH_PARENT:    return m_accessor->GetMemoriesLevel();
      case MEMORIES_CODE_WITH_PARENT:     return m_accessor->GetMemoriesCode();
      case MEMORIES_DATA_WITH_PARENT:     return m_accessor->GetMemoriesData();
      case MEMORIES_FEEL_WITH_PARENT:     return m_accessor->GetMemoriesFeel();
      case MEMORIES_LEVEL_WITH_OTHERS:    return m_accessor->GetOthersMemoriesLevel();
      case MEMORIES_CODE_WITH_OTHERS:     return m_accessor->GetOthersMemoriesCode();
      case MEMORIES_DATA_WITH_OTHERS:     return m_accessor->GetOthersMemoriesData();
      case MEMORIES_FEEL_WITH_OTHERS:     return m_accessor->GetOthersMemoriesFeel();
      }
      GFL_ASSERT(0);  //@check 引数異常は0を返して動作継続
      return 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 思い出データを設定する
     * @param memories_kind セットするデータの種類
     * @param value         セットする値
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetMemories( Memories memories_kind, u32 value )
    {
      switch( memories_kind ) 
      {
      case MEMORIES_EGG_TAKEN_YEAR:       m_accessor->SetTamagoGetYear( value );       break;
      case MEMORIES_EGG_TAKEN_MONTH:      m_accessor->SetTamagoGetMonth( value );      break;
      case MEMORIES_EGG_TAKEN_DAY:        m_accessor->SetTamagoGetDay( value );        break;
      case MEMORIES_EGG_TAKEN_PLACE:      m_accessor->SetGetPlace( value );            break;
      case MEMORIES_FIRST_CONTACT_YEAR:   m_accessor->SetBirthYear( value );           break;
      case MEMORIES_FIRST_CONTACT_MONTH:  m_accessor->SetBirthMonth( value );          break;
      case MEMORIES_FIRST_CONTACT_DAY:    m_accessor->SetBirthDay( value );            break;
      case MEMORIES_FIRST_CONTACT_PLACE:  m_accessor->SetBirthPlace( value );          break;
      case MEMORIES_CAPTURED_BALL:        m_accessor->SetGetBall( value );             break;
      case MEMORIES_CAPTURED_LEVEL:       m_accessor->SetGetLevel( value );            break;
      case MEMORIES_LEVEL_WITH_PARENT:    m_accessor->SetMemoriesLevel( value );       break;
      case MEMORIES_CODE_WITH_PARENT:     m_accessor->SetMemoriesCode( value );        break;
      case MEMORIES_DATA_WITH_PARENT:     m_accessor->SetMemoriesData( value );        break;
      case MEMORIES_FEEL_WITH_PARENT:     m_accessor->SetMemoriesFeel( value );        break;
      case MEMORIES_LEVEL_WITH_OTHERS:    m_accessor->SetOthersMemoriesLevel( value ); break;
      case MEMORIES_CODE_WITH_OTHERS:     m_accessor->SetOthersMemoriesCode( value );  break;
      case MEMORIES_DATA_WITH_OTHERS:     m_accessor->SetOthersMemoriesData( value );  break;
      case MEMORIES_FEEL_WITH_OTHERS:     m_accessor->SetOthersMemoriesFeel( value );  break;
      default: GFL_ASSERT(0); /* @check　引数異常は何もしない */ break;
      }
    }






    //-------------------------------------------------------------------------
    /**
     * @brief 元々の個体値を取得する( 「すごい特訓」済みでも、生まれ持った個体値を返します )
     * @param power_id  どの能力値なのか
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetNativeTalentPower( PowerID power_id ) const
    {
      switch( power_id ) {
      case POWER_HP:     return m_accessor->GetTalentHp();   
      case POWER_ATK:    return m_accessor->GetTalentAtk();  
      case POWER_DEF:    return m_accessor->GetTalentDef();  
      case POWER_SPATK:  return m_accessor->GetTalentSpAtk();
      case POWER_SPDEF:  return m_accessor->GetTalentSpDef();
      case POWER_AGI:    return m_accessor->GetTalentAgi();  
      }

      GFL_ASSERT(0);
      return 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 個体値を取得する( 「すごい特訓」済みなら、最大値を返します )
     * @param power_id  どの能力値なのか
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetTalentPower( PowerID power_id ) const
    {
      // 「すごい特訓」済み
      if( this->IsTrainingDone( power_id ) )
      {
        return MAX_TALENT_POWER;
      }

      return this->GetNativeTalentPower( power_id );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 何Ｖか？取得する
     * @retval [0, 6]
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetTalentPowerMaxNum( void ) const
    {
      u32 count = 0;

      for( u32 i=0; i<POWER_NUM; ++i )
      {
        PowerID powerId = static_cast<PowerID>( i );
        u32 powerValue = this->GetTalentPower( powerId );
        if( MAX_TALENT_POWER <= powerValue )
        {
          ++count;
        }
      }

      return count;
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 個体値を変更する
     * @param power_id  どの能力値なのか
     * @param value 設定する値
     */
    //-------------------------------------------------------------------------
    void CoreParam::ChangeTalentPower( PowerID power_id, u32 value )
    {
      u32 adjust_value = ( value < MAX_TALENT_POWER ) ? value : MAX_TALENT_POWER;

      // 個体値を変更し, 関連するパラメータを更新する
      switch( power_id ) 
      {
      case POWER_HP:
        m_accessor->SetTalentHp( adjust_value ); 
        this->UpdateMaxHpAndCorrectHp();
        break;

      case POWER_ATK:
        m_accessor->SetTalentAtk( adjust_value );
        this->UpdateAtk();
        break;

      case POWER_DEF:
        m_accessor->SetTalentDef( adjust_value );
        this->UpdateDef();
        break;

      case POWER_AGI:
        m_accessor->SetTalentAgi( adjust_value );
        this->UpdateAgi();
        break;

      case POWER_SPATK:
        m_accessor->SetTalentSpAtk( adjust_value ); 
        this->UpdateSpAtk();
        break;

      case POWER_SPDEF:
        m_accessor->SetTalentSpDef( adjust_value ); 
        this->UpdateSpDef();
        break;

      default:
        GFL_ASSERT(0); /* @check　引数異常は何もしない */
        break;
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 「すごい特訓」を行ったか？取得する
     * @param power_id  取得対象の能力ID
     * @retval true   「すごい特訓」を行った
     * @retval false  「すごい特訓」を行っていない
     */
    //-------------------------------------------------------------------------
    bool CoreParam::IsTrainingDone( PowerID power_id ) const
    {
      if( POWER_NUM <= power_id )
      {
        GFL_ASSERT(0);
        return false;
      }

      const u8 flag      = m_accessor->GetTrainingFlag();
      const u8 check_bit = ( 1 << power_id );
      return ( ( flag & check_bit ) != 0 );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 「すごい特訓」を行ったことを設定する
     * @param power_id  設定対象の能力ID
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetTrainingDone( PowerID power_id )
    {
      if( POWER_NUM <= power_id )
      {
        GFL_ASSERT(0);
        return;
      }

      const u8 target_bit = ( 1 << power_id );
      const u8 old_flag   = m_accessor->GetTrainingFlag();
      const u8 new_flag   = ( old_flag | target_bit ); 
      m_accessor->SetTrainingFlag( new_flag );
    }

#if PML_DEBUG
    //-------------------------------------------------------------------------
    /**
     * @brief 「すごい特訓」を行っていない状態に戻す
     * @param power_id  設定対象の能力ID
     */
    //-------------------------------------------------------------------------
    void CoreParam::ResetTrainingDone( PowerID power_id )
    {
      if( POWER_NUM <= power_id )
      {
        GFL_ASSERT(0);
        return;
      }

      const u8 target_bit = ( 1 << power_id );
      const u8 old_flag   = m_accessor->GetTrainingFlag();
      const u8 new_flag   = ( old_flag & ~target_bit ); 
      m_accessor->SetTrainingFlag( new_flag );
    }
#endif





    //-------------------------------------------------------------------------
    /**
     * @brief「めざめるパワー」のタイプを取得する
     */
    //-------------------------------------------------------------------------
    PokeType CoreParam::GetMezapaType( void ) const
    {
      return CalcTool::CalcMezamerupawaaType( 
          this->GetNativeTalentPower( POWER_HP ),
          this->GetNativeTalentPower( POWER_ATK ),
          this->GetNativeTalentPower( POWER_DEF ),
          this->GetNativeTalentPower( POWER_AGI ),
          this->GetNativeTalentPower( POWER_SPATK ),
          this->GetNativeTalentPower( POWER_SPDEF ) );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief「めざめるパワー」の威力を取得する
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetMezapaPower( void ) const
    {
      return CalcTool::CalcMezamerupawaaPower(
          this->GetNativeTalentPower( POWER_HP ),
          this->GetNativeTalentPower( POWER_ATK ),
          this->GetNativeTalentPower( POWER_DEF ),
          this->GetNativeTalentPower( POWER_AGI ),
          this->GetNativeTalentPower( POWER_SPATK ),
          this->GetNativeTalentPower( POWER_SPDEF ) );
    }






    //-------------------------------------------------------------------------
    /**
     * @brief 努力値を取得する
     * @param power_id  どの能力値なのか
     * @retval [0, MAX_EFFORT_POWER]
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetEffortPower( PowerID power_id ) const
    {
      switch( power_id ) 
      {
      case POWER_HP:    return m_accessor->GetEffortHp();
      case POWER_ATK:   return m_accessor->GetEffortAtk();
      case POWER_DEF:   return m_accessor->GetEffortDef();
      case POWER_SPATK: return m_accessor->GetEffortSpAtk();
      case POWER_SPDEF: return m_accessor->GetEffortSpDef();
      case POWER_AGI:   return m_accessor->GetEffortAgi();
      } 
      GFL_ASSERT(0);  //@check 引数異常は0を返して動作継続
      return 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 努力値の合計を取得する
     * @retval [0, MAX_POWER_EXP_TOTAL]
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetTotalEffortPower( void ) const
    {
      u32 sum = 
        this->GetEffortPower( POWER_HP ) +
        this->GetEffortPower( POWER_ATK ) +
        this->GetEffortPower( POWER_DEF ) +
        this->GetEffortPower( POWER_SPATK ) +
        this->GetEffortPower( POWER_SPDEF ) +
        this->GetEffortPower( POWER_AGI ); 
      return sum;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 努力値を加算する
     * @param power_id  どの能力値なのか
     * @param value  加算する値
     */
    //-------------------------------------------------------------------------
    void CoreParam::AddEffortPower( PowerID power_id, u32 value )
    {
      u32 before_value = this->GetEffortPower( power_id );
      u32 after_value = before_value + value;
      if( MAX_EFFORT_POWER < after_value ) {
        after_value = MAX_EFFORT_POWER;
      }
      this->ChangeEffortPower( power_id, after_value );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 努力値を減算する
     * @param power_id     どの能力値なのか
     * @param value  減算する値
     */
    //-------------------------------------------------------------------------
    void CoreParam::SubEffortPower( PowerID power_id, u32 value )
    {
      u32 before_value = this->GetEffortPower( power_id );
      u32 after_value = 0;
      if( before_value >= value ) {
        after_value = before_value - value;
      }
      this->ChangeEffortPower( power_id, after_value );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 努力値を変更する
     * @param power_id  どの能力値なのか
     * @param value  設定する値
     */
    //-------------------------------------------------------------------------
    void CoreParam::ChangeEffortPower( PowerID power_id, u32 value )
    {
      u32 before_value = this->GetEffortPower( power_id );
      u32 adjust_value = this->AdjustEffortPower( before_value, value );

      // 努力値を変更し, 関連する能力値を更新する.
      switch( power_id ) 
      {
      case POWER_HP:
        m_accessor->SetEffortHp( adjust_value );
        this->UpdateMaxHpAndCorrectHp();
        break;

      case POWER_ATK:
        m_accessor->SetEffortAtk( adjust_value );
        this->UpdateAtk();
        break;

      case POWER_DEF:
        m_accessor->SetEffortDef( adjust_value );
        this->UpdateDef();
        break;

      case POWER_SPATK:
        m_accessor->SetEffortSpAtk( adjust_value );
        this->UpdateSpAtk();
        break;

      case POWER_SPDEF:
        m_accessor->SetEffortSpDef( adjust_value );
        this->UpdateSpDef();
        break;

      case POWER_AGI:
        m_accessor->SetEffortAgi( adjust_value );
        this->UpdateAgi();
        break;

      default:
        GFL_ASSERT(0); /* @check　引数異常は何もしない */
        break;
      }
    }

#if PML_DEBUG
    //-------------------------------------------------------------------------
    /**
     * @brief 努力値を変更する（デバッグ用）
     * @param power_id  どの能力値なのか
     * @param value  設定する値
     */
    //-------------------------------------------------------------------------
    void CoreParam::ChangeEffortPowerDirect( PowerID power_id, u32 value )
    {
      // 努力値を変更し, 関連する能力値を更新する.
      switch( power_id ) 
      {
      case POWER_HP:
        m_accessor->SetEffortHp( value );
        this->UpdateMaxHpAndCorrectHp();
        break;

      case POWER_ATK:
        m_accessor->SetEffortAtk( value );
        this->UpdateAtk();
        break;

      case POWER_DEF:
        m_accessor->SetEffortDef( value );
        this->UpdateDef();
        break;

      case POWER_SPATK:
        m_accessor->SetEffortSpAtk( value );
        this->UpdateSpAtk();
        break;

      case POWER_SPDEF:
        m_accessor->SetEffortSpDef( value );
        this->UpdateSpDef();
        break;

      case POWER_AGI:
        m_accessor->SetEffortAgi( value );
        this->UpdateAgi();
        break;

      default:
        GFL_ASSERT(0); /* @check　引数異常は何もしない */
        break;
      }
    }
#endif
    
    /**
     * @brief 努力値の合計が上限を超えないように調整する
     * @param before_value  変更前の値
     * @param after_value   変更後の値
     * @return 合計が上限を超えないように設定するための値
     */
    u32 CoreParam::AdjustEffortPower( u32 before_value, u32 after_value ) const
    { 
      u32 adjust_value = (after_value < MAX_EFFORT_POWER) ? after_value : MAX_EFFORT_POWER;
      
      // 加算では無い場合はそのまま返す
      if( before_value >= adjust_value ) 
      {
        return adjust_value;
      }
      
      // 加算の場合は努力値の合計値がMAX_TOTAL_EFFORT_POWERを超えないようにまるめる
      u32 before_total = this->GetTotalEffortPower();
      u32 add_value = adjust_value - before_value;
      u32 after_total = before_total + add_value;
      if( MAX_TOTAL_EFFORT_POWER < after_total ) {
        u32 over_value = after_total - MAX_TOTAL_EFFORT_POWER;
        adjust_value -= over_value;
      }
      return adjust_value;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 能力値を取得する
     * @param power_id  取得する能力値を指定
     * @return 指定した能力値の値
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetPower( PowerID power_id ) const
    {
      switch( power_id ) 
      {
      case POWER_HP:    return GetMaxHp();
      case POWER_ATK:   return GetAtk();
      case POWER_DEF:   return GetDef();
      case POWER_SPATK: return GetSpAtk();
      case POWER_SPDEF: return GetSpDef();
      case POWER_AGI:   return GetAgi();
      } 
      GFL_ASSERT(0);  //@check 引数異常は0を返して動作継続
      return 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief「HP最大値」を取得する
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetMaxHp( void ) const
    {
      if( m_accessor->HaveCalcData() ) {
        return m_accessor->GetMaxHp();
      }
      else {
        return this->CalcMaxHp();
      }
    }

    /**
     * @brief「こうげき」を取得する
     */
    u32 CoreParam::GetAtk( void ) const
    {
      if( m_accessor->HaveCalcData() ) {
        return m_accessor->GetAtk();
      }
      else {
        return this->CalcAtk();
      }
    }

    /**
     * @brief「ぼうぎょ」を取得する
     */
    u32 CoreParam::GetDef( void ) const
    {
      if( m_accessor->HaveCalcData() ) {
        return m_accessor->GetDef();
      }
      else {
        return this->CalcDef();
      }
    }

    /**
     * @brief「とくこう」を取得する
     */
    u32 CoreParam::GetSpAtk( void ) const
    {
      if( m_accessor->HaveCalcData() ) {
        return m_accessor->GetSpAtk();
      }
      else {
        return this->CalcSpAtk();
      }
    }

    /**
     * @brief「とくぼう」を取得する
     */
    u32 CoreParam::GetSpDef( void ) const
    {
      if( m_accessor->HaveCalcData() ) {
        return m_accessor->GetSpDef();
      }
      else {
        return this->CalcSpDef();
      }
    }

    /**
     * @brief「すばやさ」を取得する
     */
    u32 CoreParam::GetAgi( void ) const
    {
      if( m_accessor->HaveCalcData() ) {
        return m_accessor->GetAgi();
      }
      else {
        return this->CalcAgi();
      }
    }




#if 0  // 2011.06.20 廃止
    //-------------------------------------------------------------------------
    /**
     * @brief 能力値を上書きする
     * @param power_id  上書きする能力値を指定
     * @param value  設定する値
     * @attention 以降, 能力値の再計算が起こると, 上書きした値が元に戻ります.
     */
    //-------------------------------------------------------------------------
    void CoreParam::OverwritePower( PowerID power_id, u32 value )
    {
      switch( power_id ) {
      case POWER_HP:
        m_accessor->SetMaxHp( value );
        break;
      case POWER_ATK:
        m_accessor->SetAtk( value );
        break;
      case POWER_DEF:
        m_accessor->SetDef( value );
        break;
      case POWER_SPATK:
        m_accessor->SetSpAtk( value );
        break;
      case POWER_SPDEF:
        m_accessor->SetSpDef( value );
        break;
      case POWER_AGI:
        m_accessor->SetAgi( value );
        break;
      default:
        GFL_ASSERT(0);
        break;
      } 
    }
#endif









    //-------------------------------------------------------------------------
    /**
     * @brief「HP」を取得する
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetHp( void ) const
    {
      if( m_accessor->HaveCalcData() ) {
        return m_accessor->GetHp();
      }
      else {
        return this->CalcMaxHp();  // 最大HPを返す
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief「HP」を設定する 
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetHp( u32 value )
    {
      u32 max_hp = m_accessor->GetMaxHp();
      u32 hp = ( max_hp < value ) ? max_hp : value;  // 最大値を超えないよう補正
      m_accessor->SetHp( hp );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief HPを減らす
     * @param value  ダメージ量
     */
    //-------------------------------------------------------------------------
    void CoreParam::ReduceHp( u32 value )
    {
      u32 hp = 0;
      u32 max_hp = m_accessor->GetMaxHp();
      if( value < max_hp ) {
        hp = max_hp - value;
      }
      m_accessor->SetHp( hp );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief HPを回復する
     * @param value  回復量
     */
    //-------------------------------------------------------------------------
    void CoreParam::RecoverHp( u32 value )
    {
      u32 max_hp = m_accessor->GetMaxHp();
      u32 before_hp = m_accessor->GetHp();
      u32 after_hp = before_hp + value;
      if( max_hp < after_hp ) {
        after_hp = max_hp;
      }
      m_accessor->SetHp( after_hp );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief HPを全回復する
     */
    //-------------------------------------------------------------------------
    void CoreParam::RecoverHpFull( void )
    {
      u32 max_hp = m_accessor->GetMaxHp();
      m_accessor->SetHp( max_hp );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief HPが全快かどうかを調べる
     * @retval true   ダメージを受けていない
     * @retval false  ダメージを受けている
     */
    //-------------------------------------------------------------------------
    bool CoreParam::IsHpFull( void ) const
    {
      if( m_accessor->HaveCalcData() ) 
      {
        u32 max_hp = m_accessor->GetMaxHp();
        u32 hp = m_accessor->GetHp();
        return ( max_hp == hp );
      }
      else
      {
        return true;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief HPが0かどうかをしらべる
     * @retval true   HPが0
     * @retval false  HPが0ではない
     */
    //-------------------------------------------------------------------------
    bool CoreParam::IsHpZero( void ) const
    {
      if( m_accessor->HaveCalcData() ) 
      {
        u32 hp = m_accessor->GetHp();
        return ( hp == 0 );
      }
      else
      {
        return false;
      }
    }








    //-------------------------------------------------------------------------
    /**
     * @brief レベルを取得する
     * @retval [0, MAX_POKE_LEVEL]
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetLevel( void ) const
    {
      if( m_accessor->HaveCalcData() ) {
        return m_accessor->GetLevel();
      }
      else {
        return this->CalcLevel();
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief レベルを上げる
     * @param up_val  増やすレベル
     */
    //-------------------------------------------------------------------------
    void CoreParam::LevelUp( u8 up_val )
    {
      u32 now_level = this->GetLevel();
      u32 next_level = now_level + up_val;
      next_level = gfl2::math::Min( next_level, (u32)MAX_POKE_LEVEL );
      if( next_level != now_level ) {
        MonsNo monsno = m_accessor->GetMonsNo();
        FormNo formno = m_accessor->GetFormNo();
        pml::personal::LoadGrowTable( monsno, formno );
        u32 exp = pml::personal::GetMinExp( next_level );
        this->SetExp( exp );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 経験値を取得する
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetExp( void ) const
    {
      return m_accessor->GetExp();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 経験値を変更する
     * @param value  設定する経験値
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetExp( u32 value )
    {
      MonsNo monsno = m_accessor->GetMonsNo();
      FormNo formno = m_accessor->GetFormNo();
      pml::personal::LoadGrowTable( monsno, formno );
      u32 max = pml::personal::GetMinExp( MAX_POKE_LEVEL );
      u32 after = ( max < value ) ? max : value;
      m_accessor->SetExp( after ); 
      this->UpdateCalcDatas( false ); // 経験値が変わったら, 全てのパラメータを再計算する
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 経験値を増やす
     * @param value  経験値の加算値
     */
    //-------------------------------------------------------------------------
    void CoreParam::AddExp( u32 value )
    {
      u32 now = m_accessor->GetExp();
      this->SetExp( now + value );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 現在のレベルに必要な経験値を取得する
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetExpForCurrentLevel( void ) const
    {
      MonsNo monsno = this->GetMonsNo();
      FormNo formno = this->GetFormNo();
      u32 level = this->GetLevel();
      pml::personal::LoadGrowTable( monsno, formno ); 
      return pml::personal::GetMinExp( level );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 次のレベルに必要な経験値を取得する
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetExpForNextLevel( void ) const
    {
      u32 level = this->GetLevel();
      u32 next_level = level + 1;
      if( MAX_POKE_LEVEL < next_level ) {
        next_level = MAX_POKE_LEVEL;
      }
      MonsNo monsno = this->GetMonsNo();
      FormNo formno = this->GetFormNo();
      pml::personal::LoadGrowTable( monsno, formno ); 
      return pml::personal::GetMinExp( next_level );
    }



#if PML_DEBUG
    //-------------------------------------------------------------------------
    /**
     * @brief デバッグ用に経験値を変更する
     * @param value  設定する経験値
     * @note GlowTableをみてmin修正をしない
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetExpDirect( u32 value )
    {
      m_accessor->SetExp( value ); 
      this->UpdateCalcDatas( false ); // 経験値が変わったら, 全てのパラメータを再計算する
    }
#endif




    //-------------------------------------------------------------------------
    /**
     * @brief モンスターNo.を取得する
     */
    //-------------------------------------------------------------------------
    MonsNo CoreParam::GetMonsNo( void ) const
    {
      return m_accessor->GetMonsNo();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief フォルムNo.を取得する
     */
    //-------------------------------------------------------------------------
    FormNo CoreParam::GetFormNo( void ) const
    {
      return m_accessor->GetFormNo();
    }






    //-------------------------------------------------------------------------
    /**
     * @brief 状態異常かどうかを調べる
     * @retval true  状態異常の場合
     * @retval false 状態異常でない場合
     */
    //-------------------------------------------------------------------------
    bool CoreParam::HaveSick( void ) const
    {
      return ( this->GetSick() != SICK_NULL );
    } 
    //-------------------------------------------------------------------------
    /**
     * @brief 状態異常を取得する
     * @return 現在の状態( pml::pokepara::SICK_xxxx )
     * @return 状態異常でない場合は SICK_NULL
     */
    //-------------------------------------------------------------------------
    Sick CoreParam::GetSick( void ) const
    {
      u32 value = m_accessor->GetSick();
      return static_cast<Sick>( value );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 状態異常をセットする
     * @param sick  設定する状態異常( SICK_NULL を指定すると, 状態以上が治る )
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetSick( Sick sick )
    {
      m_accessor->SetSick( sick );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 状態異常を治す
     */
    //-------------------------------------------------------------------------
    void CoreParam::RecoverSick( void )
    {
      this->SetSick( SICK_NULL );
    }








    //-------------------------------------------------------------------------
    /**
     * @brief 所持技の残りPPを取得する
     * @param waza_index  何番目の技か？[0, MAX_WAZA_NUM-1]
     * @return 指定した位置の技の残りPP
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetWazaPP( u8 waza_index ) const
    {
      return m_accessor->GetPP( waza_index );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 残りPPをセットする
     * @param waza_index  PPを減らす技のインデックス [0, MAX_WAZA_NUM-1]
     * @param value  セットする値
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetWazaPP( u8 waza_index, u8 value )
    {
      u32 max_pp = this->GetWazaMaxPP( waza_index );
      u32 set_pp = ( max_pp < value ) ? max_pp : value; 
      m_accessor->SetPP( waza_index, set_pp );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 残りPPを減らす
     * @param waza_index  PPを減らす技のインデックス [0, MAX_WAZA_NUM-1]
     * @param value  PPの消費量
     */
    //-------------------------------------------------------------------------
    void CoreParam::ReduceWazaPP( u8 waza_index, u8 value )
    {
      u32 before_pp = m_accessor->GetPP( waza_index );
      u32 after_pp = ( value < before_pp ) ? ( before_pp - value ) : 0;
      this->SetWazaPP( waza_index, after_pp );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 全ての技のPPを全回復する
     */
    //-------------------------------------------------------------------------
    void CoreParam::RecoverWazaPPAll( void )
    {
      for( u8 waza_index=0; waza_index<MAX_WAZA_NUM; waza_index++ ) {
        this->RecoverWazaPP( waza_index );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 指定した技のPPを全回復する
     * @param waza_index  回復する技のインデックス [0, MAX_WAZA_NUM-1]
     */
    //-------------------------------------------------------------------------
    void CoreParam::RecoverWazaPP( u8 waza_index )
    {
      u32 max_pp = this->GetWazaMaxPP( waza_index );
      this->RecoverWazaPP( waza_index, max_pp );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 指定した技のPPを回復する
     * @param waza_index  回復する技のインデックス [0, MAX_WAZA_NUM-1]
     * @param recv_value  回復する値  
     */
    //-------------------------------------------------------------------------
    void CoreParam::RecoverWazaPP( u8 waza_index, u8 recv_value )
    {
      WazaNo wazano = m_accessor->GetWazaNo( waza_index ); 
      if( wazano != WAZANO_NULL ) {
        u32 max_pp = this->GetWazaMaxPP( waza_index );
        u32 now_pp = this->GetWazaPP( waza_index );
        u32 new_pp = gfl2::math::Min( now_pp + recv_value, max_pp );
        this->SetWazaPP( waza_index, new_pp );
      }
    }






    //-------------------------------------------------------------------------
    /**
     * @brief 所持技に対して「ポイントアップ」を使用した回数を取得する
     * @param waza_index 何番目の技か？[0, MAX_WAZA_NUM-1]
     * @return 指定した位置の技に対して「ポイントアップ」を使用した回数
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetWazaPPUpCount( u8 waza_index ) const
    {
      return m_accessor->GetWazaPPUpCount( waza_index );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 「ポイントアップ」を使えるかどうか
     * @param waza_index  チェックする技のインデックス
     * @retval true   使える
     * @retval false  使えない
     */
    //-------------------------------------------------------------------------
    bool CoreParam::CanUsePointUp( u8 waza_index )
    {
      u8 count = this->GetWazaPPUpCount( waza_index );
      return ( count < MAX_WAZAPP_UPCOUNT );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 「ポイントアップ」を使う
     * @param waza_index  対象技ののインデックス
     *
     * @note PPの上限が増えた分, 残量が回復する.
     */
    //-------------------------------------------------------------------------
    void CoreParam::UsePointUp( u8 waza_index )
    {
      u8 old_max = this->GetWazaPP( waza_index );
      this->IncWazaPPUpCount( waza_index );
      u8 new_max = this->GetWazaPP( waza_index );
      this->RecoverWazaPP( waza_index, ( new_max - old_max ) );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 「ポイントアップ」を使用した回数をセットする
     * @param waza_index  技のインデックス [0, MAX_WAZA_NUM-1]
     * @param value       セットする値
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetWazaPPUpCount( u8 waza_index, u8 value )
    {
      if( MAX_WAZAPP_UPCOUNT < value ) {
        m_accessor->SetWazaPPUpCount( waza_index, MAX_WAZAPP_UPCOUNT );
      }
      else {
        m_accessor->SetWazaPPUpCount( waza_index, value );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 「ポイントアップ」を使用した回数を1増やす
     * @param waza_index 技のインデックス [0, MAX_WAZA_NUM-1]
     */
    //-------------------------------------------------------------------------
    void CoreParam::IncWazaPPUpCount( u8 waza_index )
    {
      u32 now = m_accessor->GetWazaPPUpCount( waza_index );
      this->SetWazaPPUpCount( waza_index, now + 1 );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 所持技のPP最大値を取得する
     * @param waza_index  何番目の技か？[0, MAX_WAZA_NUM-1]
     * @return 指定した位置の技の最大PP(ポイントアップによる増加を考慮)
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetWazaMaxPP( u8 waza_index ) const
    {
      WazaNo wazano = m_accessor->GetWazaNo( waza_index );
      u32 up_count = m_accessor->GetWazaPPUpCount( waza_index );
      return pml::wazadata::GetMaxPP( wazano, up_count );
    }







    //-------------------------------------------------------------------------
    /**
     * @brief 所持技の技No.を取得する
     * @param waza_index  何番目の技か？[0, MAX_WAZA_NUM-1]
     * @return 指定した位置の技No.
     */
    //-------------------------------------------------------------------------
    WazaNo CoreParam::GetWazaNo( u8 waza_index ) const
    {
      return m_accessor->GetWazaNo( waza_index );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 所持技の数を取得する
     * @return 覚えている技の数
     */
    //-------------------------------------------------------------------------
    u8 CoreParam::GetWazaCount( void ) const
    {
      u8 count = 0;
      for( u8 i=0; i<MAX_WAZA_NUM; i++ ) {
        if( m_accessor->GetWazaNo(i) != WAZANO_NULL ) {
          count++;
        }
      }
      return count;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技を覚えているかどうかを調べる
     * @param wazano  調べる技No.
     * @retval true   覚えいる
     * @retval false  覚えていない
     */
    //-------------------------------------------------------------------------
    bool CoreParam::HaveWaza( WazaNo wazano ) const
    {
      for( u32 i=0; i<MAX_WAZA_NUM; i++ ) {
        if( this->GetWazaNo(i) == wazano ) {
          return true;
        }
      }
      return false;
    } 




    //-------------------------------------------------------------------------
    /**
     * @brief 覚えたタマゴ技を取得する
     * @param index  技のインデックス[0, MAX_WAZA_NUM ]
     * @return 覚えたことのあるタマゴ技
     * @retval WAZANO_NULL  ストックがない場合
     */
    //-------------------------------------------------------------------------
    WazaNo CoreParam::GetTamagoWazaNo( u8 index ) const
    {
      return m_accessor->GetTamagoWazaNo( index );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 覚えたタマゴ技を登録する
     * @param index   技のインデックス
     * @param wazano  タマゴ技
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetTamagoWazaNo( u8 index, WazaNo wazano )
    {
      if( MAX_WAZA_NUM <= index ) {
        GFL_ASSERT(0); /* @check　引数異常は何もしない */
        return;
      }
      m_accessor->SetTamagoWazaNo( index, wazano );
    }





    //-------------------------------------------------------------------------
    /**
     * @brief モンスターNo.とレベルから, デフォルト技をセットする
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetDefaultWaza( void )
    { 
      // 最初に全クリアしておく
      for( u8 waza_index=0; waza_index<MAX_WAZA_NUM; waza_index++ ) {
        this->RemoveWaza( waza_index );
      }

      MonsNo monsno = m_accessor->GetMonsNo();
      FormNo formno = m_accessor->GetFormNo();
      u8 level = this->CalcLevel();  // Boxパラメータはレベルを持っていないので, 計算で求める

      pml::personal::LoadWazaOboeData( monsno, formno ); 
      for( u32 i=0; i<pml::personal::GetWazaOboeNum(); i++ ) {
        u16 wazalv = pml::personal::GetWazaOboeLevel(i); 
        if( wazalv <= level ) {
          WazaNo wazano = pml::personal::GetWazaOboeWazaNo(i);
          this->PushWaza( wazano );
        }
        else {
          break;
        }
      } 
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 技を押し出し式にセットする
     * @param wazano 覚えさせる技の技No.
     */
    //-------------------------------------------------------------------------
    void CoreParam::PushWaza( WazaNo wazano )
    {
      // すでに覚えてるなら, 何もしない
      if( this->IsHaveWaza( wazano ) ) {
        return;
      }

      // 空きがあるなら, 空いている場所にセットする
      for( u8 i=0; i<MAX_WAZA_NUM; i++ ) 
      {
        if( m_accessor->GetWazaNo( i ) == WAZANO_NULL ) {
          this->SetWaza( i, wazano );
          return;
        }
      }

      // 空きがないなら, プッシュする
      for( u8 i=1; i<MAX_WAZA_NUM; i++ ) 
      {
        WazaNo temp_wazano = m_accessor->GetWazaNo( i );
        u8 temp_pp = m_accessor->GetPP( i );
        u8 temp_count = m_accessor->GetWazaPPUpCount( i );
        m_accessor->SetWazaNo( i-1, temp_wazano );
        m_accessor->SetPP( i-1, temp_pp );
        m_accessor->SetWazaPPUpCount( i-1, temp_count );
      }
      this->SetWaza( MAX_WAZA_NUM - 1, wazano );
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 位置を指定して技をセットする
     * @param waza_index   技をセットする位置 [0, MAX_WAZA_NUM-1]
     * @param wazano  技No.
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetWaza( u8 waza_index, WazaNo wazano )
    {
      if( MAX_WAZA_NUM <= waza_index ) {
        GFL_ASSERT(0); /* @check　引数異常は何もしない */
        return;
      }

      u32 pp = pml::wazadata::GetParam( wazano, wazadata::PARAM_ID_BASE_PP );
      m_accessor->SetWazaNo( waza_index, wazano );
      m_accessor->SetWazaPPUpCount( waza_index, 0 );
      m_accessor->SetPP( waza_index, pp );
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 位置を指定して技を削除する
     * @param waza_index  削除する技のインデックス [0, MAX_WAZA_NUM-1]
     */
    //-------------------------------------------------------------------------
    void CoreParam::RemoveWaza( u8 waza_index )
    {
      if( MAX_WAZA_NUM <= waza_index ) {
        GFL_ASSERT(0); /* @check　引数異常は何もしない */
        return;
      }

      m_accessor->SetWazaNo( waza_index, WAZANO_NULL );
      m_accessor->SetWazaPPUpCount( waza_index, 0 );
      m_accessor->SetPP( waza_index, 0 );
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 技を入れ替える
     * @param pos1  入れ替える技のインデックス1 [0, MAX_WAZA_NUM-1]
     * @param pos2  入れ替える技のインデックス2 [0, MAX_WAZA_NUM-1]
     */
    //-------------------------------------------------------------------------
    void CoreParam::ExchangeWazaPos( u8 pos1, u8 pos2 )
    {
      if( (MAX_WAZA_NUM <= pos1) || (MAX_WAZA_NUM <= pos2) ) {
        GFL_ASSERT(0); /* @check　引数異常は何もしない */
        return;
      }

      if( pos1 != pos2 ) {
        WazaNo temp_wazano = m_accessor->GetWazaNo( pos1 );
        u8 temp_pp        = m_accessor->GetPP( pos1 );
        u8 temp_count     = m_accessor->GetWazaPPUpCount( pos1 );

        m_accessor->SetWazaNo( pos1, m_accessor->GetWazaNo( pos2 ) );
        m_accessor->SetPP( pos1, m_accessor->GetPP( pos2 ) );
        m_accessor->SetWazaPPUpCount( pos1, m_accessor->GetWazaPPUpCount( pos2 ) );

        m_accessor->SetWazaNo( pos2, temp_wazano );
        m_accessor->SetWazaPPUpCount( pos2, temp_count );
        m_accessor->SetPP( pos2, temp_pp );
      }
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 技を前詰めにシフトする
     */
    //-------------------------------------------------------------------------
    void CoreParam::CloseUpWazaPos( void )
    {
      for( u8 i=0; i<MAX_WAZA_NUM-1; i++ )
      {
        if( m_accessor->GetWazaNo(i) == WAZANO_NULL )
        {
          for( u8 j=i; j<MAX_WAZA_NUM-1; j++ )
          {
            this->ExchangeWazaPos( j, j+1 );
          }
        }
      }
    }







    //-------------------------------------------------------------------------
    /**
     * @brief 技マシンで技を覚えるかどうかをチェックする
     * @param machine_no  技マシン番号
     */
    //-------------------------------------------------------------------------
    bool CoreParam::CheckWazaMachine( u32 machine_no ) const
    {
      MonsNo monsno = m_accessor->GetMonsNo();
      FormNo formno = m_accessor->GetFormNo();
      personal::LoadPersonalData( monsno, formno );
      return personal::CheckPersonalWazaMachine( machine_no );
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 技教えで技を覚えるかどうかをチェックする
     * @param oshie_no  技教え番号
     */
    //-------------------------------------------------------------------------
    bool CoreParam::CheckWazaOshie( u32 oshie_no ) const
    {
      MonsNo monsno = m_accessor->GetMonsNo();
      FormNo formno = m_accessor->GetFormNo();
      personal::LoadPersonalData( monsno, formno );
      return personal::CheckPersonalWazaOshie( oshie_no );
    }

    //-------------------------------------------------------------------------
    /**
    * @brief BP技教えで技を覚えるかどうかをチェックする
    * @param waza_no  技番号
    */
    //-------------------------------------------------------------------------
    bool CoreParam::CheckBpWazaOshie(u32 waza_no) const
    {
      MonsNo monsno = m_accessor->GetMonsNo();
      FormNo formno = m_accessor->GetFormNo();
      personal::LoadPersonalData(monsno,formno);
      return personal::CheckPersonalBpWazaOshie(waza_no);
    }





    /**
     * @brief コンストラクタ
     */
    CoreParam::WazaLearnWork::WazaLearnWork( void )
    {
      this->Clear();
    }

    /**
     * @brief 技習得処理を開始する前の状態にする
     */
    void CoreParam::WazaLearnWork::Clear( void )
    {
      m_checkedWazaNum = 0;

      for( u32 i=0; i<GFL_NELEMS(m_checkedWazaArray); ++i )
      {
        m_checkedWazaArray[i] = WAZANO_NULL;
      }
    }

    /**
     * @brief 指定した技が、チェック済み技配列に含まれているかチェックする
     * @param waza  チェック対象の技
     * @retval true   チェック済み技配列に含まれている
     * @retval false  チェック済み技配列に含まれていない
     */
    bool CoreParam::WazaLearnWork::IsCheckedWaza( WazaNo waza ) const
    {
      for( u32 i=0; i<GFL_NELEMS(m_checkedWazaArray); ++i )
      {
        if( m_checkedWazaArray[i] == waza )
        {
          return true;
        }
      }

      return false;
    }

    /**
     * @brief チェック済みの技を追加する
     * @param waza  追加する技
     */
    void CoreParam::WazaLearnWork::AddCheckedWaza( WazaNo waza )
    {
      if( this->IsCheckedWaza( waza ) )
      {
        return;
      }

      if( GFL_NELEMS(m_checkedWazaArray) <= m_checkedWazaNum )
      {
        GFL_ASSERT(0);
        return;
      }

      m_checkedWazaArray[ m_checkedWazaNum ] = waza;
      m_checkedWazaNum++;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 現在のレベルで習得できる技を追加する
     *
     * @param[in,out] same_level_index  現在のレベルで覚える技のうち, 何番目の技を追加するのか
     * @param[out]    new_wazano        追加した技No.( 失敗した場合はその技No. )
     * @param[in,out] work              習得処理ワーク
     *
     * @retval WAZA_LEARNING_SUCCEEDED    新しい技を覚えた
     * @retval WAZA_LEARNING_FAILED_FULL  手持ち技がいっぱいで覚えられなかった
     * @retval WAZA_LEARNING_FAILED_SAME  すでに覚えているので変化なし
     *
     * @note 
     * レベルアップ時に呼ばれることを想定しています.
     *
     * @caution
     * 同じレベルで複数の技を覚えるポケモンが存在します. ( バタフリーなど )
     * 呼び出し側では, 戻り値に WAZA_LEARNING_NOT_EXIST が返ってくるまで
     * この関数を繰り返し呼んでください.
     */
    //-------------------------------------------------------------------------
    WazaLearningResult CoreParam::LearnNewWazaOnCurrentLevel( u32* same_level_index, WazaNo* new_wazano, WazaLearnWork* work )
    {
      MonsNo monsno = m_accessor->GetMonsNo();
      FormNo formno = m_accessor->GetFormNo();
      u32 level = this->GetLevel();

      pml::personal::LoadWazaOboeData( monsno, formno ); 

      u32 same_level_count = 0;
      u32 max_num = pml::personal::GetWazaOboeNum();
      for( u32 i=0; i<max_num; i++ ) 
      {
        const u32 learn_level = pml::personal::GetWazaOboeLevel(i);
        const WazaNo learn_wazano = pml::personal::GetWazaOboeWazaNo(i);
        bool learn_enable = true;

        if( work != NULL )
        {
          // 既にチェック済みの技なら、習得しない
          if( work->IsCheckedWaza( learn_wazano ) )
          {
            learn_enable = false;
          }
        }

        if( level < learn_level ) 
        {
          break;
        } 
        else if( level == learn_level ) 
        {
          if( learn_enable && ( same_level_count == *same_level_index ) )
          {
            // @fix NMCat[4268]チェック済みの技として登録
            if( work != NULL )
            {
              work->AddCheckedWaza( learn_wazano );
            }

            // 新技を追加し, その結果を返す.
            *new_wazano = learn_wazano;
            *same_level_index += 1;
            return this->AddWazaIfEmptyExist( learn_wazano );
          }
          else 
          {
            same_level_count++;
          }
        }
      } 

      return WAZA_LEARNING_FAILED_NOT_EXIST; // 今のレベルでは覚えられる技は無い
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化時に習得できる技を追加する
     *
     * @param[in,out] learn_index  何番目の技から追加するのか
     * @param[out]    new_wazano   追加した技No.( 失敗した場合はその技No. )
     * @param[in,out] work         習得処理ワーク
     *
     * @retval WAZA_LEARNING_SUCCEEDED    新しい技を覚えた
     * @retval WAZA_LEARNING_FAILED_FULL  手持ち技がいっぱいで覚えられなかった
     * @retval WAZA_LEARNING_FAILED_SAME  すでに覚えているので変化なし
     *
     * @caution
     * 進化時に複数の技を覚えるポケモンが存在します。
     * 呼び出し側では, 戻り値に WAZA_LEARNING_NOT_EXIST が返ってくるまで
     * この関数を繰り返し呼んでください。
     */
    //-------------------------------------------------------------------------
    WazaLearningResult CoreParam::LearnNewWazaOnEvolution( u32* learn_index, WazaNo* new_wazano, WazaLearnWork* work )
    {
      MonsNo monsno = m_accessor->GetMonsNo();
      FormNo formno = m_accessor->GetFormNo();
      pml::personal::LoadWazaOboeData( monsno, formno ); 

      u32 max_num = pml::personal::GetWazaOboeNum();
      for( u32 i=*learn_index; i<max_num; i++ ) 
      {
        const WazaNo learn_wazano = pml::personal::GetWazaOboeWazaNo(i);
        const pml::personal::WazaOboeData::OboeWazaKind learn_kind = pml::personal::GetWazaOboeKind(i);

        // 進化習得技以外は無視
        if( learn_kind != pml::personal::WazaOboeData::OBOEWAZA_KIND_EVOLVE )
        {
          continue;
        }

        if( work != NULL )
        {
          // 既にチェック済みの技なら、無視する
          if( work->IsCheckedWaza( learn_wazano ) )
          {
            continue;
          }

          // チェック済みの技として登録
          work->AddCheckedWaza( learn_wazano );
        }

        // 新技を追加し, その結果を返す.
        *new_wazano = learn_wazano;
        *learn_index = i+1;
        return this->AddWazaIfEmptyExist( learn_wazano );
      }

      return WAZA_LEARNING_FAILED_NOT_EXIST; // 進化習得技が存在しない
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 空きがあったら技を追加する
     * @param wazano  追加する技
     * @retval WAZA_LEARNING_SUCCEEDED    新しい技を覚えた
     * @retval WAZA_LEARNING_FAILED_FULL  手持ち技がいっぱいで覚えられなかった
     * @retval WAZA_LEARNING_FAILED_SAME  すでに覚えているので変化なし
     */
    //-------------------------------------------------------------------------
    WazaLearningResult CoreParam::AddWazaIfEmptyExist( WazaNo wazano )
    {
      if( this->IsHaveWaza( wazano ) ) {
        return WAZA_LEARNING_FAILED_SAME;
      }

      for( u8 i=0; i<MAX_WAZA_NUM; i++ ) 
      {
        WazaNo own_wazano = this->GetWazaNo(i);
        if( own_wazano == WAZANO_NULL ) {
          this->SetWaza( i, wazano );
          return WAZA_LEARNING_SUCCEEDED;
        }
      } 
      return WAZA_LEARNING_FAILED_FULL;
    }

    /**
     * @brief 技を覚えているかどうか調べる
     * @param wazano  チェックする技No.
     * @retval true   覚えている
     * @retval false  覚えていない
     */
    bool CoreParam::IsHaveWaza( WazaNo wazano ) const
    {
      for( u8 i=0; i<MAX_WAZA_NUM; i++ ) 
      {
        WazaNo own_wazano = this->GetWazaNo(i);
        if( own_wazano == wazano ) {
          return true;
        }
      } 
      return false;
    }


#if PML_DEBUG
    //-------------------------------------------------------------------------
    /**
     * @brief 位置を指定して技をセットする（デバッグ用）
     * @param waza_index   技をセットする位置 [0, MAX_WAZA_NUM-1]
     * @param wazano  技No.
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetWazaNoDirect( u8 waza_index, WazaNo wazano )
    {
      m_accessor->SetWazaNo( waza_index, wazano );
    }
#endif






    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンの性別を取得する
     * @retval SEX_MALE     オス
     * @retval SEX_FEMALE   メス
     * @retval SEX_UNKNOWN  不明
     */
    //-------------------------------------------------------------------------
    Sex CoreParam::GetSex( void ) const
    {
      return m_accessor->GetSex();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンの性別ベクトルの値を取得する
     * @retval [0, 255]
     */
    //-------------------------------------------------------------------------
    u8 CoreParam::GetSexVector( void ) const
    {
      MonsNo monsno = m_accessor->GetMonsNo();
      FormNo formno = m_accessor->GetFormNo();
      personal::LoadPersonalData( monsno, formno );
      return personal::GetPersonalParam( personal::PARAM_ID_sex );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンの性別のタイプを取得する( ランダム or 固定 )
     * @retval SEX_TYPE_RANDOM  ランダム
     * @retval SEX_TYPE_FIX     固定
     */
    //-------------------------------------------------------------------------
    personal::SexType CoreParam::GetSexType( void ) const
    {
      MonsNo monsno = m_accessor->GetMonsNo();
      FormNo formno = m_accessor->GetFormNo();
      personal::LoadPersonalData( monsno, formno );
      return personal::GetPersonalSexType();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 性別を変更する
     * @param new_sex  新しい性別
     */
    //-------------------------------------------------------------------------
    void CoreParam::ChangeSex( Sex new_sex )
    {
      if( m_accessor->GetSex() != new_sex ) 
      {
        u8 sex_vector = this->GetSexVector();
        u8 min_sex_vector = 0;
        u8 max_sex_vector = 0;
        switch( new_sex ) {
        case SEX_MALE:
          min_sex_vector = personal::SEX_VECTOR_ONLY_MALE;
          max_sex_vector = personal::SEX_VECTOR_RANDOM_MAX;
          break;
        case SEX_FEMALE:
          min_sex_vector = personal::SEX_VECTOR_RANDOM_MIN;
          max_sex_vector = personal::SEX_VECTOR_ONLY_FEMALE;
          break;
        default:
          min_sex_vector = personal::SEX_VECTOR_UNKNOWN;
          max_sex_vector = personal::SEX_VECTOR_UNKNOWN;
          break;
        }
        if( ( min_sex_vector <= sex_vector ) && ( sex_vector <= max_sex_vector ) ) {
          m_accessor->SetSex( new_sex );
        }
      }
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 性別を変更する（デバッグ用）
     */
    //-------------------------------------------------------------------------
#if PML_DEBUG
    void CoreParam::ChangeSexDirect( Sex new_sex )
    {
      m_accessor->SetSex( new_sex );
    }
#endif



    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンの性格を取得する
     */
    //-------------------------------------------------------------------------
    Seikaku CoreParam::GetSeikaku( void ) const
    {
      u32 value = m_accessor->GetSeikaku();
      return static_cast<Seikaku>( value );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンの性格を変更する
     * @param seikaku  設定する性格
     */
    //-------------------------------------------------------------------------
    void CoreParam::ChangeSeikaku( Seikaku seikaku )
    {
      m_accessor->SetSeikaku( seikaku );
      this->UpdateCalcDatas();  // 性格でパラメータが変化する
    }




    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンの好みの味かどうかをチェックする
     * @param taste  調べる味
     * @retval true   指定した味が好みの味の場合
     * @retval false  指定した味が好みの味でない場合
     */
    //-------------------------------------------------------------------------
    TasteJudge CoreParam::JudgeTaste( Taste taste ) const
    {
      Seikaku seikaku = this->GetSeikaku();
      return CalcTool::JudgeTaste( seikaku, taste );
    }







    //-------------------------------------------------------------------------
    /**
     * @brief 特性No.を取得する
     * @return 特性No.
     */
    //-------------------------------------------------------------------------
    TokuseiNo CoreParam::GetTokuseiNo( void ) const
    {
      return m_accessor->GetTokuseiNo();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 何番目の特性なのかを取得する
     * @retval [0, 2]
     */
    //-------------------------------------------------------------------------
    u8 CoreParam::GetTokuseiIndex( void ) const
    {
      u8 index = 0;
      bool tokusei_2_flag = m_accessor->IsTokusei2();
      bool tokusei_3_flag = m_accessor->IsTokusei3();

      if( tokusei_3_flag ) {
        index = 2;
      }
      else if( tokusei_2_flag ) {
        index = 1;
      }
      else {
        index = 0;
      } 
      return index;
    } 
    
    //-------------------------------------------------------------------------
    /**
     * @brief 何番目の特性なのかを取得する(持ちえない特性、ToukseiNoとTokuseiIndexの不整合判定付き)
     * @retval [0, 2] 正常
     * @retval TOKUSEI_INDEX_ERROR(=0xFF) 不正な特性
     *
     * @note  不正と判定されるとGetTokuseiIndexと異なりエラー値を返すので注意
     *        130320時点で育て屋用のpml_EggGenerator.cppでのみ利用している
     */
    //-------------------------------------------------------------------------
    u8 CoreParam::GetTokuseiIndexStrict( void ) const
    {
      int on_flag_num = 0;
      on_flag_num += m_accessor->IsTokusei1();
      on_flag_num += m_accessor->IsTokusei2();
      on_flag_num += m_accessor->IsTokusei3();

      if( on_flag_num != 1 ){
        return TOKUSEI_INDEX_ERROR;  //フラグが立っていない、または複数立っているのはおかしい
      }
      int tokusei_index = this->GetTokuseiIndex();
      TokuseiNo tokusei_no = m_accessor->GetTokuseiNo();
      
      //不正な特性Noかをチェックするために、パーソナルデータを取得
      personal::LoadPersonalData( m_accessor->GetMonsNo(), m_accessor->GetFormNo() );
      
      for( int i = 0;i < 3; i++ ){
        TokuseiNo tn = static_cast<TokuseiNo>( personal::GetPersonalParam(static_cast<personal::ParamID>(personal::PARAM_ID_tokusei1+i)) );
        if( tokusei_no == tn && tokusei_index == i ){
          return i; //パーソナルのデータとCoreParamから取得できる値の整合性が取れている
        }
      }
      return TOKUSEI_INDEX_ERROR;
    } 


    //-------------------------------------------------------------------------
    /**
     * @brief 特性１と特性２を入れ替える
     * @note  特性３の場合はASSERT（製品版では何もしない)
     */
    //-------------------------------------------------------------------------
    void CoreParam::FlipTokuseiIndex( void )        //特性１と２を入れ替える
    {
      switch ( this->GetTokuseiIndex() )
      {
      case 0:
        SetTokuseiIndex(1);
        break;
      case 1:
        SetTokuseiIndex(0);
        break;
      case 2:
        GFL_ASSERT(0);  /* @check 異常な場合は何もしない */
        break;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 何番目の特性なのかを書き換える
     * @param tokusei_index
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetTokuseiIndex( u8 tokusei_index )
    {
      MonsNo monsno = m_accessor->GetMonsNo();
      FormNo formno = m_accessor->GetFormNo();
      if ( tokusei_index < 3 )
      {
        TokuseiNo tokusei_no = CalcTool::GetTokuseiNo( monsno, formno, tokusei_index );
        m_accessor->SetTokusei1Flag( tokusei_index == 0 );
        m_accessor->SetTokusei2Flag( tokusei_index == 1 );
        m_accessor->SetTokusei3Flag( tokusei_index == 2 );
        m_accessor->SetTokuseiNo( tokusei_no );
      }
      else
      {
        GFL_ASSERT(0); /* @check　引数異常は何もしない */
      }
    }


#if PML_DEBUG
    //-------------------------------------------------------------------------
    /**
     * @brief 特性No.を上書きする 
     * @attention 進化, フォルムチェンジで特性が変化してしまう恐れあり.
     */
    //-------------------------------------------------------------------------
    void CoreParam::OverwriteTokuseiNo( TokuseiNo tokusei )
    {
      m_accessor->SetTokuseiNo( tokusei ); 
      // この時点で, フラグ・パーソナルデータと特性No.に不整合が生じる.
    }
#endif





    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンの親かどうかの認証
     * @param ownerInfo
     * @return bool 親であると認証されたとき、true
     *
     * @note  「親が一致する」とは、名前、性別、トレーナーIDが一致
     */
    //-------------------------------------------------------------------------
    bool CoreParam::CompareOwnerInfo( const OwnerInfo * ownerInfo ) const 
    {
      if ( ( m_accessor->GetOyasex() == ownerInfo->sex )
        && ( m_accessor->GetID() == ownerInfo->trainer_id )
        && m_accessor->CompareOyaName( ownerInfo->name ) )
      {
        return true;
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 親に所有されているかどうかの情報を更新する
     * @param ownerInfo   親の識別情報をパックした構造体
     * @return bool   親が所有しているとき、true
     */
    //-------------------------------------------------------------------------
    bool CoreParam::UpdateOwnerInfo( const OwnerInfo * ownerInfo )
    {
      if ( this->CompareOwnerInfo(ownerInfo) == true )
      {
        m_accessor->SetOwnedOthersFlag( false );
        return true;
      }
      m_accessor->SetOwnedOthersFlag( true );

      //オリジナル親と一致しない場合、親情報を更新
      m_accessor->SetPastParentsName( ownerInfo->name );
      m_accessor->SetPastParentsSex( static_cast<Sex>(ownerInfo->sex) );
      // 思い出を初期化
      m_accessor->SetOthersMemoriesLevel( 0 );
      m_accessor->SetOthersMemoriesCode( 0 );
      m_accessor->SetOthersMemoriesData( 0 );
      m_accessor->SetOthersMemoriesFeel( 0 );

      // なつき度関連パラメータを更新する
      //　親以外のなつきトレーナーIDを更新
      // 2013.04.30 なつきトレーナーIDは使用しないように修正
      //m_accessor->SetFamiliarTrainerID(ownerInfo->trainer_id);

      //　親以外のなつきトレーナー用なつき度を初期化
      personal::LoadPersonalData( m_accessor->GetMonsNo(), m_accessor->GetFormNo() );
      u8 familiarity = personal::GetPersonalParam( personal::PARAM_ID_initial_familiarity );
      m_accessor->SetOthersFamiliarity( familiarity );

      //  親以外のトレーナー用なかよし度を初期化
      m_accessor->SetOthersFriendship( 0 );

      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 最初の親に所有されているか？の取得
     * @note  所持しているフラグとは値が逆転しているので注意
     */
    //-------------------------------------------------------------------------
    bool CoreParam::IsOwnedOriginalParent( void ) const
    {
      return !(m_accessor->GetOwnedOthersFlag());
    }

#if PML_DEBUG
    void CoreParam::SetOwnedOthersFlag( bool flag )
    {
      m_accessor->SetOwnedOthersFlag( flag );
    }
#endif

    //-------------------------------------------------------------------------
    /**
     * @brief ニックネームを持っているかどうかを調べる
     * @retval true  ニックネームを持っている場合
     * @retval false ニックネームを持っていない場合
     */
    //-------------------------------------------------------------------------
    bool CoreParam::HaveNickName( void ) const
    {
      return m_accessor->HaveNickName();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ニックネームを取得する
     * @param[out] dest_buf 文字列の格納先
     */
    //-------------------------------------------------------------------------
    void CoreParam::GetNickName( gfl2::str::StrBuf* dest_buf ) const
    {
      m_accessor->GetNickName( dest_buf ); 
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ニックネームを取得する
     * @param[out] buffer   文字列の格納先
     * @param[in]  buf_size 配列の要素数
     */
    //-------------------------------------------------------------------------
    void CoreParam::GetNickName( gfl2::str::STRCODE* buffer, u32 buf_size ) const
    {
      m_accessor->GetNickName( buffer, buf_size );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ニックネームを変更する
     * @param nickname  設定するニックネーム
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetNickName( const gfl2::str::StrBuf* nickname )
    {
      const STRCODE* strcode = reinterpret_cast<const STRCODE*>( nickname->GetPtr() );
      m_accessor->SetNickName( strcode );
      m_accessor->SetNickNameFlag( true );

      //デフォルト名と一致する場合、元に戻したい意志があるとみなして
      //ニックネームフラグを落とす
      STRCODE default_name[ MONS_NAME_BUFFER_SIZE ];
      MonsNo monsno = m_accessor->GetMonsNo();
      pml::personal::GetMonsName( default_name, monsno );
      if ( gfl2::str::StrComp( strcode, default_name ) == true )
      {
        m_accessor->SetNickNameFlag( false );
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ニックネームに種族名をセットする
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetDefaultNickName( void )
    {
      STRCODE str[ MONS_NAME_BUFFER_SIZE ];
      MonsNo monsno = m_accessor->GetMonsNo();
      pml::personal::GetMonsName( str, monsno );
      m_accessor->SetNickName( str );
      m_accessor->SetNickNameFlag( false );
    }

#if PML_DEBUG
    //-------------------------------------------------------------------------
    // デバッグ：ニックネームのEOMを取り除く
    //-------------------------------------------------------------------------
    void CoreParam::RemoveNickNameEom( void )
    {
      m_accessor->RemoveNickNameEom();
    }

    //-------------------------------------------------------------------------
    // デバッグ：親名のEOMを取り除く
    //-------------------------------------------------------------------------
    void CoreParam::RemoveOyaNameEom( void )
    {
      m_accessor->RemoveOyaNameEom();
    }
    //-------------------------------------------------------------------------
    // デバッグ：過去親名のEOMを取り除く
    //-------------------------------------------------------------------------
    void CoreParam::RemovePastOyaNameEom( void )
    {
      m_accessor->RemovePastOyaNameEom();
    }
    //-------------------------------------------------------------------------
    // デバッグ：ニックネームを強制13文字にする
    //-------------------------------------------------------------------------
    void CoreParam::SetNickName13Chars( void )
    {
      m_accessor->SetNickName13Chars();
    }

    //-------------------------------------------------------------------------
    // デバッグ：親名を強制13文字にする
    //-------------------------------------------------------------------------
    void CoreParam::SetOyaName13Chars( void )
    {
      m_accessor->SetOyaName13Chars();
    }
#endif
  
    //-------------------------------------------------------------------------
    /**
     * @brief ニックネームが種族名デフォルトか？
     *
     * @note  プレイ言語のデフォルト名と一致するか?　で判定する
     *
     *        プレイ言語が同じロム間では
     *        HaveNickName()==trueの時、IsDefaultNickName()==false
     *        HaveNickName()==falseの時、IsDefaultNickName()==true
     *        と、HaveNickNameとIsDefaultNickNameの結果は逆になる事が保障される
     *
     *        他言語Verで捕獲した、他言語デフォルト名のポケモンを交換で手に入れた場合
     *        HaveNickName()==false且つIsDefaultNickName()==falseがあり得る
     *
     *        例：英語データのレジアイス(=Regice)を日本語verに連れてくると
     *        　　日本語デフォルト名"レジアイス"とは一致しないため、この関数はfalseを返す
     */
    //-------------------------------------------------------------------------
    bool CoreParam::IsDefaultNickName( void )
    {
      //デフォルト名と一致するかどうか？
      STRCODE default_name[ MONS_NAME_BUFFER_SIZE ];
      STRCODE nick_name[ MONS_NAME_BUFFER_SIZE ];

      MonsNo monsno = m_accessor->GetMonsNo();
      pml::personal::GetMonsName( default_name, monsno );

      m_accessor->GetNickName( nick_name, MONS_NAME_BUFFER_SIZE );

      return gfl2::str::StrComp( nick_name, default_name );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 親トレーナーの名前を取得する
     * @param[out] dest_buf 文字列の格納先
     */
    //-------------------------------------------------------------------------
    void CoreParam::GetParentName( gfl2::str::StrBuf* dest_buf ) const
    {
      m_accessor->GetOyaName( dest_buf ); 
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 親トレーナーの名前を取得する
     * @param[out] buffer   文字列の格納先
     * @param[in]  buf_size 配列の要素数
     */
    //-------------------------------------------------------------------------
    void CoreParam::GetParentName( gfl2::str::STRCODE* buffer, u32 buf_len ) const
    {
      m_accessor->GetOyaName( buffer, buf_len );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 親トレーナーの名前を変更する
     * @param name  設定する名前
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetParentName( const gfl2::str::StrBuf* name )
    {
      const STRCODE* strcode = reinterpret_cast<const STRCODE*>( name->GetPtr() );
      m_accessor->SetOyaName( strcode );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 親トレーナーの名前を変更する
     * @param name  設定する名前
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetParentName( const gfl2::str::STRCODE* strcode )
    {
      m_accessor->SetOyaName( strcode );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 親トレーナーの性別を取得する
     */
    //-------------------------------------------------------------------------
    Sex CoreParam::GetParentSex( void ) const
    {
      return static_cast<Sex>( m_accessor->GetOyasex() );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 親トレーナーの性別を変更する
     * @param sex  親の性別
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetParentSex( Sex sex )
    {
      m_accessor->SetOyasex( sex );
    }






    //-------------------------------------------------------------------------
    /**
     * @brief 過去の親トレーナーの名前を取得する
     * @param[out] dest_buf 文字列の格納先
     */
    //-------------------------------------------------------------------------
    void CoreParam::GetPastParentsName( gfl2::str::StrBuf* dest_buf ) const
    {
      m_accessor->GetPastParentsName( dest_buf ); 
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 過去の親トレーナーの名前を取得する
     * @param[out] buffer   文字列の格納先
     * @param[in]  buf_size 配列の要素数
     */
    //-------------------------------------------------------------------------
    void CoreParam::GetPastParentsName( gfl2::str::STRCODE* buffer, u32 buf_len ) const
    {
      m_accessor->GetPastParentsName( buffer, buf_len );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 過去の親トレーナーの名前を変更する
     * @param name  セットする名前
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetPastParentsName( const gfl2::str::StrBuf* name ) const
    {
      const STRCODE* strcode = reinterpret_cast<const STRCODE*>( name->GetPtr() );
      m_accessor->SetPastParentsName( strcode );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 過去の親トレーナーの名前を変更する
     * @param name  セットする名前
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetPastParentsName( const gfl2::str::STRCODE* strcode ) const
    {
      m_accessor->SetPastParentsName( strcode );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 過去の親トレーナーの性別を取得する
     */
    //-------------------------------------------------------------------------
    Sex CoreParam::GetPastParentsSex( void ) const
    {
      return m_accessor->GetPastParentsSex();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 過去の親トレーナーの性別を変更する
     * @param sex  セットする性別
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetPastParentsSex( Sex sex )
    {
      m_accessor->SetPastParentsSex( sex );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 親とのなつき度を取得する
     * @retval [0, MAX_FAMILIARITY]
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetFamiliarity( void ) const
    {
      return m_accessor->GetFamiliarity();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 親とのなつき度を変更する
     * @param value  設定する値 [0, MAX_FAMILIARITY]
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetFamiliarity( u32 value )
    {
      if( MAX_FAMILIARITY < value ) {
        m_accessor->SetFamiliarity( MAX_FAMILIARITY );
      }
      else {
        m_accessor->SetFamiliarity( value );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 親とのなつき度を加算する
     * @param value  加算値
     */
    //-------------------------------------------------------------------------
    void CoreParam::AddFamiliarity( u32 value )
    {
      u32 now = this->GetFamiliarity();
      u32 after = now + value;
      this->SetFamiliarity( after );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 親とのなつき度を減算する
     * @param value  減算値
     */
    //-------------------------------------------------------------------------
    void CoreParam::SubFamiliarity( u32 value )
    { 
      u32 now = this->GetFamiliarity();
      u32 after = ( now < value ) ? 0 : (now - value);
      this->SetFamiliarity( after );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 親以外のなつきトレーナーのIDを取得する
     */
    //--------------------------------------------------------------------------
    u16 CoreParam::GetFamiliarTrainerID( void ) const
    {
      return m_accessor->GetFamiliarTrainerID();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 最初の親のトレーナーとのなつき度を取得する
     * @retval [0, MAX_FAMILIARITY]
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetOriginalFamiliarity( void ) const
    {
      return m_accessor->GetOriginalFamiliarity();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 最初の親のトレーナーとのなつき度を変更する
     * @param value  設定する値 [0, MAX_FAMILIARITY]
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetOriginalFamiliarity( u32 value )
    {
      if( MAX_FAMILIARITY < value ) {
        m_accessor->SetOriginalFamiliarity( MAX_FAMILIARITY );
      }
      else {
        m_accessor->SetOriginalFamiliarity( value );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 最初の親のトレーナーとのなつき度を加算する
     * @param value  加算値
     */
    //-------------------------------------------------------------------------
    void CoreParam::AddOriginalFamiliarity( u32 value )
    {
      u32 now = this->GetOriginalFamiliarity();
      u32 after = now + value;
      this->SetOriginalFamiliarity( after );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 最初の親のトレーナーとのなつき度を減算する
     * @param value  減算値
     */
    //-------------------------------------------------------------------------
    void CoreParam::SubOriginalFamiliarity( u32 value )
    { 
      u32 now = this->GetOriginalFamiliarity();
      u32 after = ( now < value ) ? 0 : (now - value);
      this->SetOriginalFamiliarity( after );
    }



    //-------------------------------------------------------------------------
    /**
     * @brief 親以外のトレーナーとのなつき度を取得する
     * @retval [0, MAX_FAMILIARITY]
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetOthersFamiliarity( void ) const
    {
      return m_accessor->GetOthersFamiliarity();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 親以外のトレーナーとのなつき度を変更する
     * @param value  設定する値 [0, MAX_FAMILIARITY]
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetOthersFamiliarity( u32 value )
    {
      if( MAX_FAMILIARITY < value ) {
        m_accessor->SetOthersFamiliarity( MAX_FAMILIARITY );
      }
      else {
        m_accessor->SetOthersFamiliarity( value );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 親以外のトレーナーとのなつき度を加算する
     * @param value  加算値
     */
    //-------------------------------------------------------------------------
    void CoreParam::AddOthersFamiliarity( u32 value )
    {
      u32 now = this->GetOthersFamiliarity();
      u32 after = now + value;
      this->SetOthersFamiliarity( after );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 親以外のトレーナーとのなつき度を減算する
     * @param value  減算値
     */
    //-------------------------------------------------------------------------
    void CoreParam::SubOthersFamiliarity( u32 value )
    { 
      u32 now = this->GetOthersFamiliarity();
      u32 after = ( now < value ) ? 0 : (now - value);
      this->SetOthersFamiliarity( after );
    }






    //-------------------------------------------------------------------------
    /**
     * @brief 親との仲良し度を取得する
     * @retval [0, MAX_FRIENDSHIP]
     */
    //-------------------------------------------------------------------------
    u8 CoreParam::GetFriendship( void ) const
    {
      return m_accessor->GetFriendship();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 親との仲良し度を変更する
     * @param value  設定する値 [0, MAX_FRIENDSHIP]
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetFriendship( u32 value )
    {
      if( MAX_FRIENDSHIP < value ) {
        m_accessor->SetFriendship( MAX_FRIENDSHIP );
      }
      else {
        m_accessor->SetFriendship( value );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 親との仲良し度を加算する
     * @param value  加算値
     */
    //-------------------------------------------------------------------------
    void CoreParam::AddFriendship( u8 value )
    {
      u32 now = this->GetFriendship();
      u32 after = now + value;
      this->SetFriendship( after );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 親との仲良し度を減算する
     * @param value  減算値
     */
    //-------------------------------------------------------------------------
    void CoreParam::SubFriendship( u8 value )
    { 
      u32 now = this->GetFriendship();
      u32 after = ( now < value ) ? 0 : (now - value);
      this->SetFriendship( after );
    }






    //-------------------------------------------------------------------------
    /**
     * @brief 親以外の仲良しトレーナーのIDを取得す
     */
    //-------------------------------------------------------------------------
    u16 CoreParam::GetFriendlyTrainerID( void ) const
    {
      return m_accessor->GetFriendlyTrainerID();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 親以外の仲良しトレーナーのIDを設定する
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetFriendlyTrainerID( u16 id )
    {
      m_accessor->SetFriendlyTrainerID( id );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 親以外のトレーナーとの仲良し度を取得する
     * @retval [0, MAX_FRIENDSHIP]
     */
    //-------------------------------------------------------------------------
    u8 CoreParam::GetOthersFriendship( void ) const
    {
      return m_accessor->GetOthersFriendship();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 親以外のトレーナーとの仲良し度を変更する
     * @param value  設定する値 [0, MAX_FRIENDSHIP]
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetOthersFriendship( u8 value )
    {
      if( MAX_FRIENDSHIP < value ) {
        m_accessor->SetOthersFriendship( MAX_FRIENDSHIP );
      }
      else {
        m_accessor->SetOthersFriendship( value );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 親以外のトレーナーとの仲良し度を加算する
     * @param value  加算値
     */
    //-------------------------------------------------------------------------
    void CoreParam::AddOthersFriendship( u8 value )
    {
      u32 now = this->GetOthersFriendship();
      u32 after = now + value;
      this->SetOthersFriendship( after );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 親以外のトレーナーとの仲良し度を減算する
     * @param value  減算値
     */
    //-------------------------------------------------------------------------
    void CoreParam::SubOthersFriendship( u8 value )
    { 
      u32 now = this->GetOthersFriendship();
      u32 after = ( now < value ) ? 0 : (now - value);
      this->SetOthersFriendship( after );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンリゾートのイベント状態を取得する
     */
    //-------------------------------------------------------------------------
    u8 CoreParam::GetResortEventStatus( void ) const
    {
      return m_accessor->GetResortEventStatus();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンリゾートのイベント状態を設定する
     * @param value  設定する値
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetResortEventStatus( u8 status )
    {
      m_accessor->SetResortEventStatus( status );
    }

#if 0 // niji では使用しないはずなのでコメントアウト
    //-------------------------------------------------------------------------
    /**
     * @brief ポケデコを取得する
     */
    //-------------------------------------------------------------------------
    u16 CoreParam::GetPokeDeco( void ) const
    {
      return m_accessor->GetPokeDeco();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ポケデコを設定する
     * @param value  設定する値
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetPokeDeco( u16 value )
    {
      m_accessor->SetPokeDeco( value );
    }
#endif






    //-------------------------------------------------------------------------
    /**
     * @brief タマゴかどうかを調べる
     * @param type  調べるタマゴの種類を指定
     * @return type == CHECK_ONLY_LEGAL_EGG の場合, 正規タマゴで, かつ不正タマゴでないなら true
     * @return type == CHECK_ONLY_ILLEGAL_EGG の場合, 不正タマゴなら true
     * @return type == CHECK_BOTH_EGG の場合, 正規タマゴか, または不正タマゴなら true
     */
    //-------------------------------------------------------------------------
    bool CoreParam::IsEgg( EggCheckType type ) const
    {
      bool legal_egg = m_accessor->IsTamago();
      bool illegal_egg = m_accessor->IsFuseiTamago();

      switch( type ) {
      case CHECK_ONLY_LEGAL_EGG: return ( legal_egg && !illegal_egg );
      case CHECK_ONLY_ILLEGAL_EGG: return illegal_egg;
      case CHECK_BOTH_EGG: return ( legal_egg || illegal_egg );
      }

      GFL_ASSERT(0);  //@check 引数異常は常にfalse
      return false;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief タマゴフラグを立てる。タマゴを生成する場合はChangeEgg関数仕様を推奨
     * 
     * このフラグが立っていると「タマゴ」と扱われるが、
     * 実際ゲーム内で手に入るタマゴとしてCoreParamを生成するには
     * EggFlagにプラスして以下２つが最低限セットされている必要がある
     * ・ニックネームに"タマゴ"
     * ・パーソナルの孵化歩数パラメータ
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetEggFlag( void )
    {
      if( this->IsEgg( CHECK_ONLY_ILLEGAL_EGG ) == false ) {
        m_accessor->SetTamagoFlag( true );
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief タマゴにするユーティリティ. タマゴフラグ、孵化歩数、ニックネームをセットする
     * 
     * タマゴ技などはセットされないので注意
     */
    //-------------------------------------------------------------------------
    void CoreParam::ChangeEgg( gfl2::heap::HeapBase* tmpHeap )
    {
      if( this->IsEgg( CHECK_ONLY_ILLEGAL_EGG ) != false ) {
        return;  //ダメタマゴに対しては何もしない
      }
      m_accessor->SetTamagoFlag( true );

      // ニックネームを "タマゴ" にする
      gfl2::str::StrBuf nickname( pml::MONS_NAME_LENGTH + pml::EOM_LENGTH, tmpHeap->GetLowerHandle() );
      pml::personal::GetMonsName( &nickname, MONSNO_TAMAGO );
      m_accessor->SetNickName( nickname.GetPtr() );
      m_accessor->SetNickNameFlag( false ); //ニックネームフラグは落とす

      //孵化歩数をセットする
      {
        personal::LoadPersonalData(  this->GetMonsNo(), this->GetFormNo() );
        u32 birth_count = personal::GetPersonalParam( personal::PARAM_ID_egg_birth );
        m_accessor->SetOriginalFamiliarity( birth_count );
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief タマゴを孵す
     */
    //-------------------------------------------------------------------------
    void CoreParam::Birth( void )
    {
      if( m_accessor->IsTamago() == false ) {
        GFL_ASSERT(0); /* @check　引数異常は何もしない */
        return;
      }

      // タマゴフラグを落とす
      m_accessor->SetTamagoFlag( false );

      // なつき度を初期値にする
      m_accessor->SetOriginalFamiliarity( BIRTH_FAMILIARITY );  // タマゴ孵化した時のなつき度は固定値

      // ニックネームをデフォルト名にする
      this->SetDefaultNickName();

      // 生まれたときの親がオリジナル親になる
      m_accessor->SetOwnedOthersFlag( false );

      // トレーナーメモセットはプロジェクト側で行っている
    }

#if PML_DEBUG
    //-------------------------------------------------------------------------
    /**
     * @brief 不正タマゴにする
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetFuseiTamagoFlag( void )
    {
      m_accessor->SetFuseiTamagoFlag( true );
    }
#endif

    //-------------------------------------------------------------------------
    /**
     * @brief ポケモン同士の相性をチェックする
     * @param partner チェック対象のポケモン
     */
    //-------------------------------------------------------------------------
    LoveLevel CoreParam::CheckLoveLevel( const CoreParam& partner ) const
    {
      u32 my_egg_group[2];
      personal::LoadPersonalData( this->GetMonsNo(), this->GetFormNo() );
      my_egg_group[0] = personal::GetPersonalParam( personal::PARAM_ID_egg_group1 );
      my_egg_group[1] = personal::GetPersonalParam( personal::PARAM_ID_egg_group2 );

      u32 partner_egg_group[2];
      personal::LoadPersonalData( partner.GetMonsNo(), partner.GetFormNo() );
      partner_egg_group[0] = personal::GetPersonalParam( personal::PARAM_ID_egg_group1 );
      partner_egg_group[1] = personal::GetPersonalParam( personal::PARAM_ID_egg_group2 );

      // どちらかのタマゴグループが「無生殖」
      if( ( my_egg_group[0] == personal::EGG_GROUP_MUSEISYOKU ) ||
          ( my_egg_group[1] == personal::EGG_GROUP_MUSEISYOKU ) ||
          ( partner_egg_group[0] == personal::EGG_GROUP_MUSEISYOKU ) ||
          ( partner_egg_group[1] == personal::EGG_GROUP_MUSEISYOKU )  )
      {
        return LOVE_LV_WORST;
      }

      // 両方ともメタモン
      MonsNo my_monsno = this->GetMonsNo();
      MonsNo partner_monsno = partner.GetMonsNo();
      if( ( my_monsno == MONSNO_METAMON ) &&
          ( partner_monsno == MONSNO_METAMON ) )
      {
        return LOVE_LV_WORST;
      }

      // 同性
      Sex my_sex = this->GetSex();
      Sex partner_sex = partner.GetSex();
      if( my_sex == partner_sex )
      {
        return LOVE_LV_WORST;
      }

      // どちらかが性別なし
      if( ( my_sex == SEX_UNKNOWN ) ||
          ( partner_sex == SEX_UNKNOWN ) )
      {
        return LOVE_LV_WORST;
      }

      // 子作りグループが異なる
      if( ( my_egg_group[0] != partner_egg_group[0] ) && 
          ( my_egg_group[0] != partner_egg_group[1] ) &&
          ( my_egg_group[1] != partner_egg_group[0] ) &&
          ( my_egg_group[1] != partner_egg_group[1] ) )
      {
        return LOVE_LV_WORST;
      } 

      // ここから、タマゴが生まれる可能性あり
      if( my_monsno == partner_monsno ) 
      {
        u32 my_ID = this->GetID();
        u32 partner_ID = partner.GetID();

        if( my_ID == partner_ID ) 
        {
          return LOVE_LV_NORMAL; // 同種族, 同ID
        }
        else 
        {
          return LOVE_LV_GOOD; // 同種族, 異ID
        }
      }
      else 
      {
        u32 my_ID = this->GetID();
        u32 partner_ID = partner.GetID();

        if( my_ID == partner_ID ) 
        {
          return LOVE_LV_BAD; // 異種族, 同ID
        }
        else 
        {
          return LOVE_LV_NORMAL; // 異種族, 異ID
        }
      }
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 進化ルートを持っているかどうかの判定
     * @retval true   進化できる
     * @retval false  進化できない
     * 
     * @note  ミツハニー♂など、本当は進化しないものもtrueとなる
     */
    //-------------------------------------------------------------------------
    bool CoreParam::HaveEvolutionRoot( void ) const
    {
      return s_evolve_manager->HaveEvolutionRoot( this );
    }
    //-------------------------------------------------------------------------
    /**
     * @brief 現在のパラメータと状況( 場所, 時間など )で進化できるか？
     * @param[out] next_monsno  進化後のモンスターNo.の格納先
     * @param[in]   cond        進化できるかどうかの状況指定
     * @retval true   進化できる
     * @retval false  進化できない
     */
    //-------------------------------------------------------------------------
    bool CoreParam::CanEvolve( const EvolveSituation *situation, const pml::PokeParty * party,  MonsNo* next_monsno, u32 * root_num) const
    {
      MonsNo current_monsno = this->GetMonsNo();
      *next_monsno = s_evolve_manager->GetEvolvedMonsNo_byLevelUp( this, party, situation, root_num );
      return ( current_monsno != *next_monsno );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 道具を使用することで進化できるか？
     * @param[in]  itemno       使用する道具
     * @param[out] next_monsno  進化後のモンスターNo.の格納先
     * @retval true   進化できる
     * @retval false  進化できない
     */
    //-------------------------------------------------------------------------
    bool CoreParam::CanEvolveByItem( const EvolveSituation *situation, u16 itemno, MonsNo* next_monsno, u32 * root_num ) const
    {
      MonsNo current_monsno = this->GetMonsNo();
      *next_monsno = s_evolve_manager->GetEvolvedMonsNo_byItem( this, situation, itemno, root_num );
      return ( current_monsno != *next_monsno );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 通信交換することで進化できるか？
     * @param[in]  pair_poke    使用する道具
     * @param[out] next_monsno  進化後のモンスターNo.の格納先
     * @retval true   進化できる
     * @retval false  進化できない
     */
    //-------------------------------------------------------------------------
    bool CoreParam::CanEvolveByTrade( 
        const CoreParam* pair_poke, MonsNo* next_monsno, u32 * root_num ) const
    {
      MonsNo current_monsno = this->GetMonsNo();
      *next_monsno = s_evolve_manager->GetEvolvedMonsNo_byTrade( this, pair_poke, root_num );
      return ( current_monsno != *next_monsno );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化させる
     * @param next_monsno  進化後のモンスターNo.
     * @param route_index  進化経路のインデックス
     */
    //-------------------------------------------------------------------------
    void CoreParam::Evolve( MonsNo next_monsno, u32 route_index )
    {
      const MonsNo before_monsno = this->GetMonsNo();
      const FormNo before_formno = this->GetFormNo();  

      pml::personal::LoadEvolutionTable( before_monsno, before_formno );
      personal::EvolveCond evolve_cond = pml::personal::GetEvolutionCondition( route_index );
      GFL_ASSERT_STOP( next_monsno == pml::personal::GetEvolvedMonsNo( route_index ) );//@check 進化データ異常 or プログラムミス

      // 進化前のフォルムNo.を引き継ぐ
      FormNo next_formno = before_formno;

      // 進化後のフォルムNo.が変化
      if( pml::personal::IsEvolvedFormNoSpecified( route_index ) )
      {
        next_formno = pml::personal::GetEvolvedFormNo( route_index );
      }

      if( evolve_cond == personal::EVOLVE_COND_FEMALE_FORMCHANGE )
      {
        //違うフォルムナンバーにする
        //本来的には進化条件からフォルムナンバーを取得可能にするべき
        next_formno += 1;
      }

      // モンスターNo.を変更する( フォルムNo.は維持する )
      this->ChangeMonsNo( next_monsno, next_formno );

      // 進化のための所持アイテムを消去する
      if( ( evolve_cond == personal::EVOLVE_COND_TUUSHIN_ITEM ) ||
          ( evolve_cond == personal::EVOLVE_COND_SOUBI_NOON ) ||
          ( evolve_cond == personal::EVOLVE_COND_SOUBI_NIGHT ) )
      {
        this->RemoveItem();
      }

      // 特性「ARシステム」のグリプス２は、持っている道具によってフォルムが変化する
      {
        const TokuseiNo tokusei = this->GetTokuseiNo(); 
        if( ( next_monsno == MONSNO_GURIPUSU2 ) && ( tokusei == TOKUSEI_arSISUTEMU  ) )
        {
          u32 itemno = this->GetItem();
          FormNo formno = CalcTool::GetGuripusu2Type( itemno );  // タイプとフォルム番号は一致
          this->ChangeFormNo( formno );
        }
      }
    }


    /**
     * @brief モンスターNo.を変更する
     * @param new_monsno  変化後のモンスターNo.
     * @param new_formno  変化後のフォルムNo.
     */
    void CoreParam::ChangeMonsNo( MonsNo new_monsno, FormNo new_formno )
    {
      MonsNo old_monsno = m_accessor->GetMonsNo();
      if( old_monsno == new_monsno ) { 
        return; // 変化しない
      }

      // モンスターNo.
      m_accessor->SetMonsNo( new_monsno );

      // フォルムNo.
      m_accessor->SetFormNo( new_formno );

      // 特性No.
      {
        u8 tokusei_index = this->GetTokuseiIndex();
        TokuseiNo tokusei_no = CalcTool::GetTokuseiNo( new_monsno, new_formno, tokusei_index );
        m_accessor->SetTokusei1Flag( tokusei_index == 0 );
        m_accessor->SetTokusei2Flag( tokusei_index == 1 );
        m_accessor->SetTokusei3Flag( tokusei_index == 2 );
        m_accessor->SetTokuseiNo( tokusei_no );
      }

      // 性別
      {
        Sex old_sex = m_accessor->GetSex(); // @fix cov_ctr[10087]old_sexが2重代入されていたため削除
        Sex new_sex = CalcTool::GetCorrectSexInPersonalData( new_monsno, new_formno, old_sex );
        m_accessor->SetSex( new_sex );
      }

      // デフォルト名のままなら、ニックネームも書き換える
      if( m_accessor->HaveNickName() == false ) {
        this->SetDefaultNickName();
      }

      // 算出パラメータを更新
      this->UpdateCalcDatas();
    } 

#if PML_DEBUG
    ///デバッグ用：MonsNo書き換え
    void CoreParam::SetMonsNoDirect( MonsNo new_monsno )
    {
      m_accessor->SetMonsNo( new_monsno );
    }
    ///デバッグ用：FormNo書き換え
    void CoreParam::SetFormNoDirect( FormNo new_formno )
    {
      m_accessor->SetFormNo( new_formno );
    }
#endif
  
    //-------------------------------------------------------------------------
    /**
     * @brief フォルム関連パラメータのチェック・修正
     * @return  bool  変更があった場合、true
     *
     * @note    ニャオニクスの特殊仕様対処など
     */
    //-------------------------------------------------------------------------
    bool CoreParam::RegulateFormParams( void )
    {
      MonsNo monsno = this->GetMonsNo();
      FormNo formno = this->GetFormNo();

      //ニャオニクスは♂♀でフォルムが違うので、
      //フォルムにあわせて性別を変更する
      if ( monsno == MONSNO_NYAONIKUSU )
      {
        Sex sex = this->GetSex();
        if ( formno == FORMNO_NYAONIKUSU_MALE && sex != SEX_MALE )
        {
          m_accessor->SetSex( SEX_MALE );
          return true;
        }
        else if ( formno == FORMNO_NYAONIKUSU_FEMALE && sex != SEX_FEMALE )
        {
          m_accessor->SetSex( SEX_FEMALE );
          return true;
        }
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メガ進化する
     */
    //-------------------------------------------------------------------------
    void CoreParam::MegaEvolve( void )
    {
      if( this->CanMegaEvolve() ) {
        FormNo old_form = this->GetFormNo(); 
        FormNo mega_form = this->GetMegaFormNo();
        this->ChangeFormNo( mega_form );
        m_accessor->SetMegaForm( true );
        m_accessor->SetMegaBeforeFormNo( old_form );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メガ進化状態を元に戻す
     */
    //-------------------------------------------------------------------------
    void CoreParam::ResetMegaEvolve( void )
    {
      if( this->IsMegaEvolved() ) {
        FormNo formno = m_accessor->GetMegaBeforeFormNo();
        this->ChangeFormNo( formno );
        m_accessor->SetMegaForm( false );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メガ進化中か？
     * @retval true   メガ進化中
     * @retval false  メガ進化中でない
     */
    //-------------------------------------------------------------------------
    bool CoreParam::IsMegaEvolved( void ) const
    {
      return m_accessor->IsMegaForm();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メガ進化できるか？
     * @retval true   メガ進化できる
     * @retval false  メガ進化できない
     */
    //-------------------------------------------------------------------------
    bool CoreParam::CanMegaEvolve( void ) const
    {
      FormNo current_form = this->GetFormNo();
      FormNo mega_form = this->GetMegaFormNo();
      return ( current_form != mega_form );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メガ進化後のフォルムNo.を取得する
     */
    //-------------------------------------------------------------------------
    FormNo CoreParam::GetMegaFormNo( void ) const
    {
      return s_evolve_manager->GetMegaEvolvedFormNo( this );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メガ進化フォルムの数を取得する
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetMegaFormCount( void ) const
    {
      MonsNo monsno = this->GetMonsNo();
      pml::personal::LoadMegaEvolveTable( monsno );  
      return pml::personal::GetMegaEvolveRouteNum();
    }


    //-------------------------------------------------------------------------
    /**
     * @brief フォルムNo.を書き換える
     */
    //-------------------------------------------------------------------------
    void CoreParam::ChangeFormNo( FormNo next_formno )
    {
      MonsNo monsno = m_accessor->GetMonsNo();
      u16 old_formno = m_accessor->GetFormNo(); 
      if( old_formno == next_formno ) { return; }  // 変化しない

      //キュレム合体でのわざ変化対応
      if( monsno == MONSNO_KYUREMU )
      {
        ChangeKyuremuFormWaza( next_formno );
      }

      // フォルムNo.
      m_accessor->SetFormNo( next_formno );

      // 特性No.
      {
        u8 tokusei_index = this->GetTokuseiIndex();
        TokuseiNo tokusei_no = CalcTool::GetTokuseiNo( monsno, next_formno, tokusei_index );
        m_accessor->SetTokuseiNo( tokusei_no );
      }

      // 性別
      {
        Sex old_sex = m_accessor->GetSex();
        Sex next_sex = CalcTool::GetCorrectSexInPersonalData( monsno, next_formno, old_sex );
        m_accessor->SetSex( next_sex );
      }

      // 算出パラメータを更新
      this->UpdateCalcDatas();
    }

    //-------------------------------------------------------------------------
    //  キュレム合体・分離によるわざの変化
    //-------------------------------------------------------------------------
    void CoreParam::ChangeKyuremuFormWaza( FormNo next_formno )
    {
      static const WazaNo change_waza_table[2][3] = {
        { WAZANO_KOWAIKAO,      WAZANO_KUROSUHUREIMU, WAZANO_KUROSUSANDAA },
        { WAZANO_KOGOERUSEKAI,  WAZANO_KOORUDOHUREA,  WAZANO_HURIIZUBORUTO },
      };
      for (int waza_pos = 0; waza_pos < MAX_WAZA_NUM; ++ waza_pos )
      {
        WazaNo waza = this->GetWazaNo(waza_pos);
        for (int sets = 0; sets < GFL_NELEMS(change_waza_table); ++ sets )
        {
          for (int index = 0; index < FORMNO_KYUREMU_MAX; ++ index )
          {
            if ( change_waza_table[sets][index] == waza && index != next_formno )
            {
              // わざナンバーだけ書き換えたい！
              //this->SetWaza( waza_pos, change_waza_table[sets][next_formno] );
              u32 rest_pp = m_accessor->GetPP( waza_pos );
              m_accessor->SetWazaNo( waza_pos, change_waza_table[sets][next_formno] );
              u32 max_pp = this->GetWazaMaxPP( waza_pos );
              if ( rest_pp > max_pp )
              {
                m_accessor->SetPP( waza_pos, max_pp );
              }
            }
          }
        }
      }
    }

    //-------------------------------------------------------------------------
      //所持どうぐによる影響で変化するフォルムを取得する
    //-------------------------------------------------------------------------
    FormNo CoreParam::GetNextFormNoFromHoldItem( u16 hold_itemno ) const
    {
      FormNo formno;
      bool change = CalcTool::DecideFormNoFromHoldItem( m_accessor->GetMonsNo(), hold_itemno, &formno );
      if ( change ) 
      { //どうぐに対応したフォルムナンバーを返す
        return formno;
      }
      else
      { //どうぐでフォルムが変化しない場合、現在のフォルムナンバーを返す
        return m_accessor->GetFormNo();
      }
    }








    //-------------------------------------------------------------------------
    /**
     * @brief ロトムの専用技を全て忘れさせる
     */
    //-------------------------------------------------------------------------
    void CoreParam::RemoveAllRotomWaza( void )
    {
      for( u8 i=0; i<MAX_WAZA_NUM; i++ ) {
        WazaNo my_waza = m_accessor->GetWazaNo(i); 
        for( FormNo formno=FORMNO_ROTOMU_NORMAL; formno<FORMNO_ROTOMU_MAX; formno++ ) {
          WazaNo rotomu_waza = CalcTool::GetRotomuWazaNo( formno ); 
          if( ( rotomu_waza != WAZANO_NULL ) && ( my_waza == rotomu_waza ) ) {
            this->RemoveWaza(i);
          }
        }
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ロトムの専用技を覚えさせる
     * @param waza_index  専用技に置き換える技のインデックス番号 [0, MAX_WAZA_NUM-1]
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetRotomWaza( u8 waza_index )
    {
      FormNo formno = m_accessor->GetFormNo();
      WazaNo wazano = CalcTool::GetRotomuWazaNo( formno );
      u8 waza_count = this->GetWazaCount();

      // ノーマルフォルムの場合は, 技が無い場合にのみ「でんきショック」を覚える
      if( formno == FORMNO_ROTOMU_NORMAL ) {
        if( waza_count == 0 ) {
          this->SetWaza( 0, WAZANO_DENKISYOKKU );
        }
        return;
      }

      if( waza_count < MAX_WAZA_NUM ) {
        this->PushWaza( wazano ); // 空きがあれば, そこにセットする
      }
      else { 
        this->SetWaza( waza_index, wazano ); // 空きがなければ, 上書きする
      }
    }








    //-------------------------------------------------------------------------
    /**
     * @brief レアかどうかを調べる
     */
    //-------------------------------------------------------------------------
    bool CoreParam::IsRare( void ) const
    {
      u32 id = m_accessor->GetID();
      u32 rnd = m_accessor->GetColorRnd();
      return CalcTool::IsRareColor( id, rnd );
    }


    //-------------------------------------------------------------------------
    /**
     * @brief レア乱数を取得する
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetRareRnd( void ) const
    {
      return m_accessor->GetColorRnd();
    }


#if PML_DEBUG
    //-------------------------------------------------------------------------
    /**
     * @brief レアにする
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetRare( void )
    {
      if( !this->IsRare() ) {
        u32 id = m_accessor->GetID();
        u32 old_rare_rnd = m_accessor->GetColorRnd();
        u32 new_rare_rnd = CalcTool::CorrectColorRndForRare( id, old_rare_rnd );
        m_accessor->SetColorRnd( new_rare_rnd );
      }
    } 
    //-------------------------------------------------------------------------
    /**
     * @brief レアでなくする
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetNotRare( void )
    {
      if( this->IsRare() ) {
        u32 id = m_accessor->GetID();
        u32 old_rare_rnd = m_accessor->GetColorRnd();
        u32 new_rare_rnd = CalcTool::CorrectColorRndForNormal( id, old_rare_rnd );
        m_accessor->SetColorRnd( new_rare_rnd );
      }
    }
#endif






    //-------------------------------------------------------------------------
    /**
     * @brief IDNo.を取得する
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetID( void ) const
    {
      return m_accessor->GetID();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief IDNo.を変更する
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetID( u32 id )
    {
      m_accessor->SetID( id );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 個性乱数を取得する
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetPersonalRnd() const
    {
      return m_accessor->GetPersonalRnd();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief チェックサムを取得する
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetCheckSum() const
    {
      return m_accessor->GetCheckSum();
    }






    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンのタイプ1を取得する
     */
    //-------------------------------------------------------------------------
    PokeType CoreParam::GetType1( void ) const
    {
      MonsNo monsno = m_accessor->GetMonsNo();
      TokuseiNo tokusei = m_accessor->GetTokuseiNo(); 

      // 特性が「マルチタイプ」のアルセウス　は, 持ち物でタイプが変化する
      if( ( monsno == MONSNO_ARUSEUSU  ) && ( tokusei == TOKUSEI_MARUTITAIPU ) )
      {
        u32 itemno = m_accessor->GetItemNo();
        return CalcTool::GetAruseusuType( itemno );
      }

      // 特性が「ARシステム」のグリプス２　は, 持ち物でタイプが変化する
      if( ( monsno == MONSNO_GURIPUSU2 ) && ( tokusei == TOKUSEI_arSISUTEMU  ) )
      {
        u32 itemno = m_accessor->GetItemNo();
        return CalcTool::GetGuripusu2Type( itemno );
      }

      {
        FormNo formno = m_accessor->GetFormNo();
        personal::LoadPersonalData( monsno, formno );
        return personal::GetPersonalParam( personal::PARAM_ID_type1 );
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンのタイプ2を取得する
     */
    //-------------------------------------------------------------------------
    PokeType CoreParam::GetType2( void ) const
    {
      MonsNo monsno = m_accessor->GetMonsNo();
      TokuseiNo tokusei = m_accessor->GetTokuseiNo(); 

      // 特性が「マルチタイプ」のアルセウス　は, 持ち物でタイプが変化する
      if( ( monsno == MONSNO_ARUSEUSU  ) && ( tokusei == TOKUSEI_MARUTITAIPU ) )
      {
        u32 itemno = m_accessor->GetItemNo();
        return CalcTool::GetAruseusuType( itemno );
      }

      // 特性が「ARシステム」のグリプス２　は, 持ち物でタイプが変化する
      if( ( monsno == MONSNO_GURIPUSU2 ) && ( tokusei == TOKUSEI_arSISUTEMU  ) )
      {
        u32 itemno = m_accessor->GetItemNo();
        return CalcTool::GetGuripusu2Type( itemno );
      }

      {
        FormNo formno = m_accessor->GetFormNo();
        personal::LoadPersonalData( monsno, formno );
        return personal::GetPersonalParam( personal::PARAM_ID_type2 );
      }
    } 






    //-------------------------------------------------------------------------
    /**
     * @brief リボンを持っているかどうかを調べる
     *
     * @param ribbon_no リボンNo.
     * @return 指定したリボンを持っている場合は true
     */
    //-------------------------------------------------------------------------
    bool CoreParam::HaveRibbon( u32 ribbon_no ) const
    {
      return m_accessor->HaveRibbon( ribbon_no );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief リボンを登録する
     *
     * @param ribbon_no 登録するリボンを指定
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetRibbon( u32 ribbon_no )
    {
      m_accessor->SetRibbon( ribbon_no );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief リボンを全てクリアする
     */
    //-------------------------------------------------------------------------
    void CoreParam::RemoveAllRibbon( void )
    {
      m_accessor->RemoveAllRibbon();
    }
    
    //-------------------------------------------------------------------------
    /**
     * @brief まとめリボンをセットする
     *
     * @param ribbon_id 登録するリボンを指定
     * @param num リボンの個数
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetLumpingRibbon( LumpingRibbon ribbon_id, u32 num )
    {
      m_accessor->SetLumpingRibbon( ribbon_id, num );
    }
    
    //-------------------------------------------------------------------------
    /**
     * @brief まとめリボンを取得する
     *
     * @param ribbon_id 登録するリボンを指定
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetLumpingRibbon( LumpingRibbon ribbon_id ) const
    {
      return m_accessor->GetLumpingRibbon( ribbon_id );
    }







    //-------------------------------------------------------------------------
    /**
     * @brief 今ポケルスに感染しているかどうかを調べる
     * @retval true  現在ポケルスに感染している
     * @retval false  現在はポケルスに感染していない( 過去に感染していた場合を含む )
     */
    //-------------------------------------------------------------------------
    bool CoreParam::HavePokerusJustNow( void ) const
    {
      u8 pokerus = m_accessor->GetPokerus();
      u8 day_count = pokerus & 0x0f;
      return ( 0 < day_count );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 今までにポケルスに感染したことがあるかどうかを調べる
     * @retval true   今までにポケルスに感染してことがある( 感染中も含む )
     * @retval false  一度もポケルスに感染したことがない
     */
    //-------------------------------------------------------------------------
    bool CoreParam::HavePokerusUntilNow( void ) const
    {
      u8 pokerus = m_accessor->GetPokerus();
      return ( pokerus != 0 );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 今までにポケルスに感染したことがあるかどうかを調べる
     * @retval true   今までにポケルスに感染してことがある( 感染中を含まない )
     * @retval false  一度もポケルスに感染したことがない
     */
    //-------------------------------------------------------------------------
    bool CoreParam::HavePokerusPast( void ) const
    {
      return this->HavePokerusUntilNow() && !this->HavePokerusJustNow();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ポケルスに感染する
     */
    //-------------------------------------------------------------------------
    void CoreParam::CatchPokerus( void )
    {
      u8 pokerus = 0;
      while( ( pokerus & 0x07 ) == 0 ) {
        pokerus = pml::local::Random::GetValue( 0x100 );
      } 
      if( pokerus & 0xf0 ) {
        pokerus &= 0x07;
      }
      pokerus |= ( pokerus << 4 );
      pokerus &= 0xf3;
      pokerus++;
      GFL_ASSERT( pokerus != 0 ); //@check アルゴリズム検証用
      m_accessor->SetPokerus( pokerus );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ポケルスを伝染させる
     */
    //-------------------------------------------------------------------------
    void CoreParam::InfectPokerusWith( pml::pokepara::CoreParam* target ) const
    {
      u8 my_pokerus = this->m_accessor->GetPokerus();
      target->m_accessor->SetPokerus( my_pokerus );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ポケルスの残り日数をデクリメントする
     * @param passed_day_count  経過日数
     */
    //-------------------------------------------------------------------------
    void CoreParam::DecreasePokerusDayCount( s32 passed_day_count )
    {
      u8 pokerus = m_accessor->GetPokerus();
      if( pokerus != 0 ) 
      {
        u8 pokerus_type = pokerus & 0xf0;
        u8 day_count = pokerus & 0x0f;
        u8 new_pokerus = 0; 

        // @fix GFNMCat[3922] 改造で作られたポケルスに感染するとポケルスが消滅することがある。
        // pokerus_type が 0 なのは不正な状態だが、この状態のまま day_count がゼロになってしまうと、
        // ポケルスに感染していた事実が無かった事になってしまう。
        // 意図せず不正な状態になってしまったプレイヤからはバグのように見えてしまうので、
        // ここで pokerus_type を強制的に 1 に書き換える。
        if( pokerus_type == 0 )
        {
          pokerus_type = ( 1 << 4 );
        }

        if( ( day_count < passed_day_count ) || ( passed_day_count > 4 ) ) 
        {
          new_pokerus = pokerus_type;
        }
        else 
        {
          new_pokerus = pokerus_type | ( day_count - static_cast<u8>(passed_day_count) );
        }

        GFL_ASSERT( new_pokerus != 0 ); //@check アルゴリズム検証用
        m_accessor->SetPokerus( new_pokerus );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ポケルスを取得【DEBUG用！】
     */
    //-------------------------------------------------------------------------
    u32 CoreParam::GetPokerus( void ) const
    {
      return this->m_accessor->GetPokerus();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ポケルスを設定【DEBUG用！】
     * @param u32 ポケルス値
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetPokerus( u32 pokerus )
    {
      this->m_accessor->SetPokerus( pokerus );
    }







    //-------------------------------------------------------------------------
    /**
     * @brief 配布ポケモンかどうかを調べる
     * @retval true   配布ポケモンの場合
     * @retval false  配布ポケモンでない場合
     */
    //-------------------------------------------------------------------------
    bool CoreParam::GetEventPokeFlag( void ) const
    {
      return m_accessor->IsEventPokemon();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 配布ポケモンフラグを操作する
     * @param flag フラグを立てるかどうか
     */
    //-------------------------------------------------------------------------
    void CoreParam::SetEventPokeFlag( bool flag )
    { 
      m_accessor->SetEventPokemonFlag( flag );
    }





    //-------------------------------------------------------------------------
    /**
     * @brief 全回復
     * @note HP, PP, 状態異常, 汚れを回復
     */
    //-------------------------------------------------------------------------
    void CoreParam::RecoverAll( void )
    {
      this->RecoverHpFull();
      this->RecoverSick();
      this->RecoverWazaPPAll();
      this->ClearDirt();
    }


#if PML_DEBUG
    //---------------------------------------------------------------------------------
    /**
    * @brief デバッグで生成された、編集されたかを取得
    * @note HP, PP, 状態異常, 汚れを回復
    */
    //---------------------------------------------------------------------------------
    bool CoreParam::GetDebugEditFlag(void) const
    {
      return m_accessor->GetDebugEditFlag();
    }
    void CoreParam::SetDebugEditFlag(bool flag)
    {
      m_accessor->SetDebugEditFlag(flag);
    }
#endif


    //-------------------------------------------------------------------------
    /**
     * @brief 高速モードを開始する
     * @retval true   高速モードに移行した場合
     * @retval false  すでに高速モードだった場合
     */
    //-------------------------------------------------------------------------
    bool CoreParam::StartFastMode( void ) const
    {
      if( m_accessor->IsFastMode() ) {
        return false;
      }
      else {
        const_cast<Accessor*>( m_accessor )->StartFastMode();
        return true;
      } 
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 高速モードを終了する
     * @param valid_flag  モード移行可能かどうかを判断するフラグ
     * @retval true   通常モードに移行した場合
     * @retval false  すでに通常モードだった場合
     * @caution 
     * valid_flag には true/false の直値を指定せず,
     * StartFastMode() の戻り値を指定してください。
     */
    //-------------------------------------------------------------------------
    bool CoreParam::EndFastMode( bool valid_flag ) const
    {
      if( m_accessor->IsFastMode() && valid_flag ) {
        const_cast<Accessor*>( m_accessor )->EndFastMode();
        return true;
      }
      else {
        return false;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 高速モードかどうかを調べる
     * @retval true   高速モード
     * @retval false  通常モード
     */
    //-------------------------------------------------------------------------
    bool CoreParam::IsFastMode( void ) const
    {
      return m_accessor->IsFastMode();
    }





    //-------------------------------------------------------------------------
    /**
     * @brief
     */
    //-------------------------------------------------------------------------
    void CoreParam::RecalculateCalcData( void )
    {
      this->UpdateCalcDatas();
      m_accessor->SetMegaForm( false );
      m_accessor->SetMegaBeforeFormNo( 0 );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 経験値を元に, すべての算出パラメータを更新する
     */
    //-------------------------------------------------------------------------
    void CoreParam::UpdateCalcDatas( bool keep_dead )
    {
      bool valid_flag = this->StartFastMode();

      this->UpdateLevel();
      this->UpdateMaxHpAndCorrectHp( keep_dead );
      this->UpdateAtk();
      this->UpdateDef();
      this->UpdateSpAtk();
      this->UpdateSpDef();
      this->UpdateAgi();

      this->EndFastMode( valid_flag );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 算出パラメータ(レベル)を更新する
     */
    //-------------------------------------------------------------------------
    void CoreParam::UpdateLevel( void )
    {
      u8 level = this->CalcLevel(); 
      m_accessor->SetLevel( level );
    }

    /**
     * @brief レベルを計算する
     */
    u8 CoreParam::CalcLevel( void ) const
    {
      MonsNo monsno = m_accessor->GetMonsNo();
      FormNo formno = m_accessor->GetFormNo();
      u32 exp = m_accessor->GetExp();
      return CalcTool::CalcLevel( monsno, formno, exp );
    } 

#if PML_DEBUG
    /**
     * @brief レベル設定（デバッグ用）
     */
    void CoreParam::SetLevel( u8 level )
    {
      m_accessor->SetLevel( level );
    }
#endif
    
    //-------------------------------------------------------------------------
    /**
     * @brief 算出パラメータ(HP最大値)を更新し, 残りHPを補正する
     */
    //-------------------------------------------------------------------------
    void CoreParam::UpdateMaxHpAndCorrectHp( bool keep_dead )
    {
      u32 before_max = this->GetMaxHp();
      u32 before_hp = this->GetHp();

      this->UpdateMaxHP(); 

      // 最大値の変化に合わせて, 残りHPも補正する

      if( keep_dead && before_hp == 0 ) { // HPが元々0なら,そのまま
        return;
      } 
      u32 after_max = this->GetMaxHp();
      u32 after_hp = before_hp; 
      if( after_max < before_max ) { // もし最大値が減っているなら, 最大値を超えないように補正
        if( after_max < after_hp ) {
          after_hp = after_max;
        }
      }
      else { // 最大値が増えているなら, 増えただけ回復する
        u32 add_value = after_max - before_max;
        after_hp += add_value;
      }
      m_accessor->SetHp( after_hp ); 
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 算出パラメータ(最大HP)を更新する
     */
    //-------------------------------------------------------------------------
    void CoreParam::UpdateMaxHP( void )
    {
      u32 max_hp = this->CalcMaxHp();
      m_accessor->SetMaxHp( max_hp );
    }

    /**
     * @brief HPの最大値を計算する
     */
    u16 CoreParam::CalcMaxHp( void ) const
    {
      MonsNo monsno = m_accessor->GetMonsNo();
      u32 level = this->GetLevel();
      u32 base = 0;                                // 種族値
      u32 rnd = this->GetTalentPower( POWER_HP );  // 個体値
      u32 exp = this->GetEffortPower( POWER_HP );  // 努力値

      // 種族値を取得
      {
        FormNo formno = m_accessor->GetFormNo();
        personal::LoadPersonalData( monsno, formno );
        base = personal::GetPersonalParam( personal::PARAM_ID_basic_hp );
      }

      return CalcTool::CalcMaxHp( monsno, level, base, rnd, exp );
    }
#if PML_DEBUG
    /**
     * @brief 最大HPをセットする（デバッグ用）
     */
    void CoreParam::SetMaxHp( u32 value )
    {
      m_accessor->SetMaxHp( value );
    }
#endif


    //-------------------------------------------------------------------------
    /**
     * @brief 算出パラメータ(こうげき)を更新する
     */
    //-------------------------------------------------------------------------
    void CoreParam::UpdateAtk( void )
    {
      u32 atk = this->CalcAtk();
      m_accessor->SetAtk( atk );
    }

    /**
     * @brief 「こうげき」を計算する
     */
    u16 CoreParam::CalcAtk( void ) const
    {
      u32 level = this->GetLevel();
      u32 base = 0;                                 // 種族値
      u32 rnd = this->GetTalentPower( POWER_ATK );  // 個体値
      u32 exp = this->GetEffortPower( POWER_ATK );  // 努力値
      Seikaku seikaku = this->GetSeikaku();

      // 種族値を取得
      {
        MonsNo monsno = m_accessor->GetMonsNo();
        FormNo formno = m_accessor->GetFormNo();
        personal::LoadPersonalData( monsno, formno );
        base = personal::GetPersonalParam( personal::PARAM_ID_basic_atk );
      }

      return CalcTool::CalcAtk( level, base, rnd, exp, seikaku );
    }

#if PML_DEBUG
    /**
     * @brief こうげきを設定（デバッグ用）
     */
    void CoreParam::SetAtk( u16 value )
    {
      m_accessor->SetAtk( value );
    }
#endif

    //-------------------------------------------------------------------------
    /**
     * @brief 算出パラメータ(ぼうぎょ)を更新する
     */
    //-------------------------------------------------------------------------
    void CoreParam::UpdateDef( void )
    {
      u32 def = this->CalcDef();
      m_accessor->SetDef( def );
    }

    /**
     * @brief ぼうぎょを計算する
     */
    u16 CoreParam::CalcDef( void ) const
    {
      u32 level = this->GetLevel();
      u32 base = 0;                                // 種族値
      u32 rnd = this->GetTalentPower( POWER_DEF ); // 個体値
      u32 exp = this->GetEffortPower( POWER_DEF ); // 努力値
      Seikaku seikaku = this->GetSeikaku();

      // 種族値を取得
      {
        MonsNo monsno = m_accessor->GetMonsNo();
        FormNo formno = m_accessor->GetFormNo();
        personal::LoadPersonalData( monsno, formno );
        base = personal::GetPersonalParam( personal::PARAM_ID_basic_def );
      }

      return CalcTool::CalcDef( level, base, rnd, exp, seikaku );
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 算出パラメータ(とくこう)を更新する
     */
    //-------------------------------------------------------------------------
    void CoreParam::UpdateSpAtk( void )
    {
      u32 spatk = this->CalcSpAtk();
      m_accessor->SetSpAtk( spatk );
    }

    /**
     * @brief 「とくこう」を計算する
     */
    u16 CoreParam::CalcSpAtk( void ) const
    {
      u32 level = this->GetLevel();
      u32 base = 0;                                  // 種族値
      u32 rnd = this->GetTalentPower( POWER_SPATK ); // 個体値
      u32 exp = this->GetEffortPower( POWER_SPATK ); // 努力値
      Seikaku seikaku = this->GetSeikaku();

      // 種族値を取得
      {
        MonsNo monsno = m_accessor->GetMonsNo();
        FormNo formno = m_accessor->GetFormNo();
        personal::LoadPersonalData( monsno, formno );
        base = personal::GetPersonalParam( personal::PARAM_ID_basic_spatk ); 
      }

      return CalcTool::CalcSpAtk( level, base, rnd, exp, seikaku );
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 算出パラメータ(とくぼう)を更新する
     */
    //-------------------------------------------------------------------------
    void CoreParam::UpdateSpDef( void )
    {
      u32 spdef = this->CalcSpDef();
      m_accessor->SetSpDef( spdef );
    }

    /**
     * @brief 「とくぼう」を計算する
     */
    u16 CoreParam::CalcSpDef( void ) const
    {
      u32 level = this->GetLevel();
      u32 base = 0;                                   // 種族値
      u32 rnd = this->GetTalentPower( POWER_SPDEF );  // 個体値
      u32 exp = this->GetEffortPower( POWER_SPDEF );  // 努力値
      Seikaku seikaku = this->GetSeikaku();

      // 種族値を取得
      {
        MonsNo monsno = m_accessor->GetMonsNo();
        FormNo formno = m_accessor->GetFormNo();
        personal::LoadPersonalData( monsno, formno );
        base = personal::GetPersonalParam( personal::PARAM_ID_basic_spdef );
      }

      return CalcTool::CalcSpDef( level, base, rnd, exp, seikaku );
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 算出パラメータ(すばやさ)を更新する
     */
    //-------------------------------------------------------------------------
    void CoreParam::UpdateAgi( void )
    {
      u32 agi = this->CalcAgi();
      m_accessor->SetAgi( agi );
    }

    /**
     * @brief 「すばやさ」を計算する
     */
    u16 CoreParam::CalcAgi( void ) const
    {
      u32 level = this->GetLevel();
      u32 base = 0;                                 // 種族値
      u32 rnd = this->GetTalentPower( POWER_AGI );  // 個体値
      u32 exp = this->GetEffortPower( POWER_AGI );  // 努力値
      Seikaku seikaku = this->GetSeikaku();

      // 種族値を取得
      {
        MonsNo monsno = m_accessor->GetMonsNo();
        FormNo formno = m_accessor->GetFormNo();
        personal::LoadPersonalData( monsno, formno );
        base = personal::GetPersonalParam( personal::PARAM_ID_basic_agi );
      }

      return CalcTool::CalcAgi( level, base, rnd, exp, seikaku );
    }
    

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
    s8 CoreParam::GetPowerTransformBySeikaku( u16 seikaku, PowerID power_id )
    {
      return CalcTool::GetPowerTransformBySeikaku( seikaku, power_id );
    }
    

  } // namespace pokepara
} // namespace pml
