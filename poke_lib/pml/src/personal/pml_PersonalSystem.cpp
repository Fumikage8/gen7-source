//=============================================================================
/**
 * @brief  ポケモンパーソナルシステム
 * @file   pml_PersonalSystem.cpp
 * @author obata_toshihiro
 * @date   2010.12.20
 */
//=============================================================================
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_math.h>
#include <str/include/gfl2_MsgData.h>
#include <debug/include/gfl2_Assert.h>

#include <niji_conv_header/poke_lib/monsno_def.h>
#include <pml/include/pml_Const.h>
#include <pml/include/system/pml_Library.h>
#include <pml/include/personal/pml_PersonalConst.h> 
#include <pml/include/personal/pml_PersonalSystem.h> 
#include <pml/include/personal/pml_PersonalData.h>
#include <pml/include/personal/pml_GrowTable.h>
#include <pml/include/personal/pml_WazaOboeData.h>
#include <pml/include/personal/pml_EvolutionTable.h>
#include <pml/include/personal/pml_MegaEvolveTable.h>
#include "pml_PersonalDataCache.h"
#include "pml_GrowTableCache.h"
#include "pml_WazaOboeDataCache.h"
#include "pml_EvolutionTableCache.h"
#include "pml_MegaEvolveTableCache.h"


namespace pml { 
  namespace personal {


    static const u32 s_TempStrBufLength = 256;
    static gfl2::str::StrBuf* s_TempStrBuf = NULL;
    static gfl2::str::MsgData* s_MonsNameData = NULL;
    static gfl2::str::MsgData* s_TokuseiNameData = NULL;
    static gfl2::str::MsgData* s_TokuseiInfoData = NULL;
    static PersonalDataCache* s_PersonalDataCache = NULL;
    static PersonalData* s_PersonalData = NULL;
    static PersonalData* s_TemporaryPersonalData = NULL;
    static GrowTable* s_GrowTable = NULL;
    static GrowTableCache* s_GrowTableCache = NULL;
    static WazaOboeData* s_WazaOboeData = NULL;
    static WazaOboeData* s_WazaOboeDataNull = NULL;
    static WazaOboeData* s_WazaOboeDataReadBuf = NULL;
    static WazaOboeDataCache* s_WazaOboeDataCache = NULL;
    static EvolutionTable* s_EvolutionTable = NULL;
    static EvolutionTableCache* s_EvolutionTableCache = NULL;
    static MegaEvolveTable* s_TemporaryMegaEvolveTable = NULL;
    static MegaEvolveTable* s_MegaEvolveTable = NULL;
    static MegaEvolveTableCache* s_MegaEvolveTableCache = NULL;
    static void * s_SeedMonsNoBuffer = NULL;
    

    void Initialize( gfl2::heap::HeapBase* heap )
    {
      PersonalData::Initialize( heap, Library::m_setting->garcPath_PersonalData );
      WazaOboeData::Initialize( heap, Library::m_setting->garcPath_WazaOboeData );
      GrowTable::Initialize( heap, Library::m_setting->garcPath_GrowTable );
      EvolutionTable::Initialize( heap, Library::m_setting->garcPath_EvolutionTable );
      MegaEvolveTable::Initialize( heap, Library::m_setting->garcPath_MegaEvolveTable );
      s_TempStrBuf = GFL_NEW( heap ) gfl2::str::StrBuf( s_TempStrBufLength, heap );
      s_MonsNameData = GFL_NEW( heap ) gfl2::str::MsgData( Library::m_setting->garcPath_Message, Library::m_setting->msgDataID_MonsName, heap, gfl2::str::MsgData::LOAD_FULL );
//      s_TokuseiNameData = GFL_NEW( heap ) gfl2::str::MsgData( Library::m_setting->garcPath_Message, Library::m_setting->msgDataID_TokuseiName, heap, gfl2::str::MsgData::LOAD_PART );
//      s_TokuseiInfoData = GFL_NEW( heap ) gfl2::str::MsgData( Library::m_setting->garcPath_Message, Library::m_setting->msgDataID_TokuseiInfo, heap, gfl2::str::MsgData::LOAD_PART );
      s_PersonalData = GFL_NEW( heap ) PersonalData( heap );
      s_PersonalDataCache = GFL_NEW( heap ) PersonalDataCache( heap, Library::m_setting->cacheSize_PersonalData );
      s_GrowTable = GFL_NEW( heap ) GrowTable();
      s_GrowTableCache = GFL_NEW( heap ) GrowTableCache( heap, Library::m_setting->cacheSize_GrowTable );
      s_WazaOboeData = GFL_NEW( heap ) WazaOboeData( heap );
      s_WazaOboeDataNull = GFL_NEW( heap ) WazaOboeData( heap );
      s_WazaOboeDataReadBuf = GFL_NEW( heap ) WazaOboeData( heap );
      s_WazaOboeDataCache = GFL_NEW( heap ) WazaOboeDataCache( heap, Library::m_setting->cacheSize_WazaOboeData );
      s_EvolutionTable = GFL_NEW( heap ) EvolutionTable( heap );
      s_EvolutionTableCache = GFL_NEW( heap ) EvolutionTableCache( heap, Library::m_setting->cacheSize_EvolutionTable );
      s_MegaEvolveTable = GFL_NEW( heap ) MegaEvolveTable( heap );
      s_MegaEvolveTableCache = GFL_NEW( heap ) MegaEvolveTableCache( heap, Library::m_setting->cacheSize_MegaEvolveTable );
      s_TemporaryPersonalData = GFL_NEW( heap ) PersonalData( heap );
      s_TemporaryMegaEvolveTable = GFL_NEW( heap ) MegaEvolveTable( heap );
      {
        gfl2::fs::AsyncFileManager::ArcFileOpenReq arcReq;
        arcReq.fileName = pml::Library::m_setting->garcPath_SeedMonsNoTable;
        arcReq.heapForFile = heap;
        arcReq.heapForReq = heap->GetLowerHandle();
        /*
          2016/08/25  Yu Muto
          GF_DEVELOP_HIO_RESOURCE!=0の場合は、
          全てのリソースをHIO経由で読み込むモードへの対応のためデフォルト設定（マネージャ設定に依存）とします
        */
#if GF_DEVELOP_HIO_RESOURCE
#else
        arcReq.hioUseSetting = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_MAIN;
#endif
        pml::Library::m_pmlFileManager->SyncArcFileOpen(arcReq);

        u32 id;
        size_t size;
        const gfl2::fs::ArcFile *arc = pml::Library::m_pmlFileManager->GetArcFile( pml::Library::m_setting->garcPath_SeedMonsNoTable );
        arc->GetDataCount(&id);
        id -= 1;
        arc->GetDataSize( &size , id , heap );
        s_SeedMonsNoBuffer = GflHeapAllocMemory( heap, (int)size );
#if PML_DEBUG
        {
          size_t assumed_size = sizeof(u16) * ( MONSNO_END + 1 );
          while( assumed_size % 4 != 0 ) { assumed_size++; } // 4バイトアラインメント
          GFL_ASSERT( assumed_size == size ); //@fix 起動時に必ず毎回チェック
        }
#endif

        gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq readReq;
        readReq.fileName = pml::Library::m_setting->garcPath_SeedMonsNoTable;
        readReq.bufSize = size;
        readReq.datId = id;
        readReq.pBuf = s_SeedMonsNoBuffer;
        pml::Library::m_pmlFileManager->SyncArcFileLoadDataBuf( readReq );

        s_WazaOboeDataNull->LoadData( MONSNO_NULL, 0 );
      }
    }

    void Finalize( void )
    {
      GFL_SAFE_DELETE( s_TempStrBuf );
      GFL_SAFE_DELETE( s_PersonalData );
      GFL_SAFE_DELETE( s_PersonalDataCache );
      GFL_SAFE_DELETE( s_GrowTable );
      GFL_SAFE_DELETE( s_GrowTableCache );
      GFL_SAFE_DELETE( s_WazaOboeDataReadBuf );
      GFL_SAFE_DELETE( s_WazaOboeDataNull );
      GFL_SAFE_DELETE( s_WazaOboeData );
      GFL_SAFE_DELETE( s_WazaOboeDataCache );
      GFL_SAFE_DELETE( s_EvolutionTable );
      GFL_SAFE_DELETE( s_EvolutionTableCache );
      GFL_SAFE_DELETE( s_MegaEvolveTable );
      GFL_SAFE_DELETE( s_MegaEvolveTableCache );
      GFL_SAFE_DELETE( s_MonsNameData );
//      GFL_SAFE_DELETE( s_TokuseiNameData );
//      GFL_SAFE_DELETE( s_TokuseiInfoData );
      GFL_SAFE_DELETE( s_TemporaryPersonalData );
      GFL_SAFE_DELETE( s_TemporaryMegaEvolveTable );

      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.fileName = pml::Library::m_setting->garcPath_SeedMonsNoTable;
      pml::Library::m_pmlFileManager->SyncArcFileClose( closeReq );

      GflHeapFreeMemory( s_SeedMonsNoBuffer );
      MegaEvolveTable::Finalize();
      EvolutionTable::Finalize();
      GrowTable::Finalize();
      WazaOboeData::Finalize();
      PersonalData::Finalize();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンの種族名を取得する
     *
     * @param[in] buffer  文字列を格納するバッファ
     * @param[in] monsno  モンスターNo.
     * @param[in] formno  フォルムNo.
     */
    //-------------------------------------------------------------------------
    u16 GetMonsName( STRCODE* buffer, MonsNo monsno )
    {
      u16 userParam = GetMonsName( s_TempStrBuf, monsno );
      gfl2::str::StrNCopy( buffer, s_TempStrBuf->GetPtr() , MONS_NAME_BUFFER_SIZE );
      return userParam;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ポケモンの種族名を取得する
     *
     * @param[out] buffer  文字列を格納するバッファ
     * @param[in]  monsno  モンスターNo.
     * @param[in]  formno  フォルムNo.
     */
    //-------------------------------------------------------------------------
    u16 GetMonsName( gfl2::str::StrBuf* buffer, MonsNo monsno )
    {
      if( MONSNO_MAX < monsno ) {
        GFL_ASSERT(0);  //@check　引数異常は何もせず戻る
        return 0;
      }
      FormNo formno = 0;  // フォルムでポケモン名が変わる企画は現在ないのでノーマル指定
      u32 strID = PersonalData::GetDataID( monsno, formno );  // パーソナルデータと同じ並び
      return s_MonsNameData->GetString( strID, *buffer );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief パーソナルデータを読み込む
     * @param monsno  対象のモンスターNo.
     * @param formno  対象のフォルムNo.
     */
    //-------------------------------------------------------------------------
    void LoadPersonalData( MonsNo monsno, FormNo formno )
    {
      LoadPersonalData( monsno, formno, s_PersonalData ); 
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief パーソナルデータを取得する
     * @param[in]  monsno  モンスターNo.
     * @param[in]  formno  フォルムNo.
     * @param[out] buffer  読み込んだデータの格納先
     */
    //-------------------------------------------------------------------------
    void LoadPersonalData( 
        MonsNo monsno,
        FormNo formno,
        PersonalData* buffer )
    {
      buffer->LoadData( monsno, formno );
#if 0
      // キャッシュをチェック
      bool hit = s_PersonalDataCache->GetData( monsno, formno, buffer );

      // キャッシュにヒットしなかったら読み込む
      if( !hit ) {
        buffer->LoadData( monsno, formno );
        s_PersonalDataCache->RegisterData( *buffer );
      }
#endif
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したパーソナルデータを取得する
     *
     * @param paramId  取得するデータのID
     */
    //-------------------------------------------------------------------------
    u32 GetPersonalParam( ParamID paramId )
    {
      return s_PersonalData->GetParam( paramId );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技マシンで技を覚えられるかどうかを調べる
     *
     * @param machineNo  技マシンNo.
     *
     * @retval true   覚えられる
     * @retval false  覚えられない
     */
    //-------------------------------------------------------------------------
    bool CheckPersonalWazaMachine( u16 machineNo )
    {
      return s_PersonalData->CheckWazaMachine( machineNo );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技教えで技を覚えられるかどうかを調べる
     *
     * @param oshieNo  技教えNo.
     *
     * @retval true   覚えられる
     * @retval false  覚えられない
     */
    //-------------------------------------------------------------------------
    bool CheckPersonalWazaOshie( u16 oshieNo )
    {
      return s_PersonalData->CheckWazaOshie( oshieNo );
    }

    //-------------------------------------------------------------------------
    /**
    * @brief BP技教えで技を覚えられるかどうかを調べる
    *
    * @param wazaNo  技No.
    *
    * @retval true   覚えられる
    * @retval false  覚えられない
    */
    //-------------------------------------------------------------------------
    bool CheckPersonalBpWazaOshie(u16 wazaNo)
    {
      return s_PersonalData->CheckBpWazaOshie(wazaNo);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 性別タイプを取得する
     */
    //-------------------------------------------------------------------------
    SexType GetPersonalSexType( void )
    {
      return s_PersonalData->GetSexType();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 持っている特性のパターンを返す
     * @return TokuseiPattern
     */ 
    //-------------------------------------------------------------------------
    TokuseiPattern GetTokuseiPattern( void )
    {
      TokuseiNo tokusei1 = static_cast<TokuseiNo>( s_PersonalData->GetParam( PARAM_ID_tokusei1 ) );
      TokuseiNo tokusei2 = static_cast<TokuseiNo>( s_PersonalData->GetParam( PARAM_ID_tokusei2 ) );
      TokuseiNo tokusei3 = static_cast<TokuseiNo>( s_PersonalData->GetParam( PARAM_ID_tokusei3 ) );

      if ( tokusei1 == tokusei2 ) {
        if ( tokusei1 == tokusei3 ) {
          return TOKUSEI_PATTERN_AAA;
        } else {
          return TOKUSEI_PATTERN_AAB;
        }
      } 
      GFL_ASSERT( tokusei1 != tokusei3 );// ABA はNG!　@check 外部で静的検査する
      GFL_ASSERT( tokusei2 != tokusei3 );// ABB はNG!　@check 外部で静的検査する
      return TOKUSEI_PATTERN_ABC;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 成長曲線テーブルを読み込む
     * @param monsno  対象のモンスターNo.
     * @param formno  対象のフォルムNo.
     */
    //-------------------------------------------------------------------------
    void LoadGrowTable( MonsNo monsno, FormNo formno )
    {
      LoadGrowTable( monsno, formno, s_GrowTable );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 成長曲線テーブルを読み込む
     * @param[in]  monsno  対象のモンスターNo.
     * @param[in]  formno  対象のフォルムNo.
     * @param[out] buffer  読み込んだテーブルの格納先
     */
    //-------------------------------------------------------------------------
    void LoadGrowTable( MonsNo monsno, FormNo formno, GrowTable* buffer )
    {
      LoadPersonalData( monsno, formno, s_TemporaryPersonalData );
      u32 grow_type = s_TemporaryPersonalData->GetParam( personal::PARAM_ID_grow ); 
      LoadGrowTable( grow_type, buffer );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 成長曲線テーブルを読み込む
     * @param grow_type 成長タイプ
     */
    //-------------------------------------------------------------------------
    void LoadGrowTable( u32 grow_type )
    {
      LoadGrowTable( grow_type, s_GrowTable );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 成長曲線テーブルを読み込む
     * @param[in]  grow_type 成長タイプ
     * @param[out] buffer    読み込んだテーブルの格納先
     */
    //-------------------------------------------------------------------------
    void LoadGrowTable( u32 grow_type, GrowTable* buffer )
    {
      // キャッシュからデータを取り出す
      bool hit = s_GrowTableCache->GetTable( grow_type, buffer );

      // キャッシュにHitしなかったら読み込む
      if( !hit )
      {
        buffer->Load( grow_type );
        s_GrowTableCache->RegisterTable( *buffer );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したレベルになるための最小経験値を取得する
     * @param level  調べたいレベル
     */
    //-------------------------------------------------------------------------
    u32 GetMinExp( u8 level )
    {
      level = gfl2::math::Min( level, pml::MAX_POKE_LEVEL );
      return s_GrowTable->GetMinExp( level );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技覚えデータを読み込む
     * @param monsno  対象のモンスターNo.
     * @param formno  対象のフォルムNo.
     */
    //-------------------------------------------------------------------------
    void LoadWazaOboeData( MonsNo monsno, FormNo formno )
    {
      LoadWazaOboeData( monsno, formno, s_WazaOboeData );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技覚えデータを読み込む
     * @param[in]  monsno  対象のモンスターNo.
     * @param[in]  formno  対象のフォルムNo.
     * @param[out] buffer  データの格納先
     */
    //-------------------------------------------------------------------------
    void LoadWazaOboeData( MonsNo monsno, FormNo formno, WazaOboeData* buffer )
    {
      // MONSNOがNULLなら一々ロードしない
      if (monsno == MONSNO_NULL)
      {
        s_WazaOboeDataNull->CopyData( buffer );
        return;
      }

      // キャッシュからデータを取り出す
      bool hit = s_WazaOboeDataCache->GetData( monsno, formno, buffer );

      // キャッシュにHitしなかったら読み込む
      if( !hit )
      {
        buffer->LoadData( monsno, formno );
        s_WazaOboeDataCache->RegisterData( *buffer );
      }
    }
    
    /* ------------------------------------------------------------------------- */
    /**
     * @brief 技覚えデータを非同期で読み込む
     *
     * @param monsno  対象のモンスターNo.
     * @param formno  対象のフォルムNo.
     * @param p_temp_heap スレッド用レンポラリヒープ
     */
    /* ------------------------------------------------------------------------- */
    void StartAsyncReadWazaOboe( MonsNo monsno, FormNo formno, gfl2::heap::HeapBase * p_temp_heap )
    {
      WazaOboeData * buffer = s_WazaOboeDataReadBuf;
      bool hit = s_WazaOboeDataCache->GetData( monsno, formno, buffer );

      if ( !hit )
      {
        buffer->StartAsyncRead( monsno, formno, p_temp_heap );
      }
    }

    /* ------------------------------------------------------------------------- */
    /**
     * @brief 技覚えデータの非同期読み込み待ち＆完了処理
     *
     * @return ture 完了, false 未完了
     */
    /* ------------------------------------------------------------------------- */
    bool WaitAsyncReadFinishWazaOboe( void )
    {
      WazaOboeData * buffer = s_WazaOboeDataReadBuf;
      if (buffer->WaitAsyncReadFinish())
      {
        s_WazaOboeDataCache->RegisterData( *buffer );
        return true;
      }

      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技覚えデータを取得する( 覚える技の数 )
     */
    //-------------------------------------------------------------------------
    u8 GetWazaOboeNum( void )
    {
      return s_WazaOboeData->GetValidCodeNum();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 技覚えデータを取得する( 技を習得するレベル )
     * @param learn_index 習得技のインデックス
     */
    //-------------------------------------------------------------------------
    u16 GetWazaOboeLevel( u16 learn_index )
    {
      return s_WazaOboeData->GetLevel( learn_index );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 技覚えデータを取得する( 技No. )
     * @param learn_index 習得技のインデックス
     */
    //-------------------------------------------------------------------------
    WazaNo GetWazaOboeWazaNo( u16 learn_index )
    {
      return s_WazaOboeData->GetWazaNo( learn_index );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技覚えデータを取得する( 覚え技の種類 )
     * @param learn_index 習得技のインデックス
     */
    //-------------------------------------------------------------------------
    extern WazaOboeData::OboeWazaKind GetWazaOboeKind( u16 learn_index )
    {
      return s_WazaOboeData->GetOboeWazaKind( learn_index );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化テーブルを読み込む
     * @param monsno  対象のモンスターNo.
     * @param formno  対象のフォルムNo.
     */
    //-------------------------------------------------------------------------
    void LoadEvolutionTable( MonsNo monsno, FormNo formno )
    {
      LoadEvolutionTable( monsno, formno, s_EvolutionTable );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化テーブルを読み込む
     * @param[in]  monsno  対象のモンスターNo.
     * @param formno  対象のフォルムNo.
     * @param[out] buffer  データの格納先
     */
    //-------------------------------------------------------------------------
    void LoadEvolutionTable( MonsNo monsno, FormNo formno, EvolutionTable* buffer )
    {
      // キャッシュからデータを取り出す
      bool hit = s_EvolutionTableCache->GetTable( monsno, formno, buffer );

      // キャッシュにHitしなかったら読み込む
      if( !hit )
      {
        s_EvolutionTable->LoadTable( monsno, formno );
        s_EvolutionTableCache->RegisterTable( *s_EvolutionTable );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化経路の数を取得する
     */
    //-------------------------------------------------------------------------
    u8 GetEvolutionRouteNum( void ) 
    {
      return s_EvolutionTable->GetEvolutionRouteNum();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化の条件を取得する
     *
     * @param route_index  進化経路のインデックス [0, MaxEvolutionRouteNum]
     */
    //-------------------------------------------------------------------------
    EvolveCond GetEvolutionCondition( u8 route_index ) 
    {
      return s_EvolutionTable->GetEvolutionCondition( route_index );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 進化の条件を取得する
     *
     * @param next_monsno  進化後のモンスターNo.
     *
     * @return 指定したポケモンに進化するための条件
     */
    //-------------------------------------------------------------------------
    EvolveCond GetEvolutionCondition( MonsNo next_monsno ) 
    {
      u32 route_num = s_EvolutionTable->GetEvolutionRouteNum();
      for( u32 i=0; i<route_num; i++ )
      {
        if( s_EvolutionTable->GetEvolvedMonsNo(i) == next_monsno ) {
          return s_EvolutionTable->GetEvolutionCondition(i);
        }
      }
      return EVOLVE_COND_NONE;  // 進化しない
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化条件に関するパラメータを取得する
     *
     * @param route_index  進化経路のインデックス [0, MaxEvolutionRouteNum]
     */
    //-------------------------------------------------------------------------
    u16 GetEvolutionParam( u8 route_index ) 
    {
      return s_EvolutionTable->GetEvolutionParam( route_index );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化後のモンスターNo.を取得する
     *
     * @param route_index  進化経路のインデックス [0, MaxEvolutionRouteNum]
     */
    //-------------------------------------------------------------------------
    MonsNo GetEvolvedMonsNo( u8 route_index )
    {
      return s_EvolutionTable->GetEvolvedMonsNo( route_index );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化後のフォルムNo.を取得する
     *
     * @param route_index  進化経路のインデックス [0, MaxEvolutionRouteNum]
     */
    //-------------------------------------------------------------------------
    FormNo GetEvolvedFormNo( u8 route_index )
    {
      return s_EvolutionTable->GetEvolvedFormNo( route_index );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化後のフォルムNo.が指定されているか？
     *
     * @param route_index  進化経路のインデックス[0, MaxEvolutionRouteNum]
     *
     * @retval true    指定されている( PersonalSystem::GetEvolvedFormNo で取得したフォルムNo.にしてください )
     * @retval false   指定されていない( 進化前のフォルムNo.を引きついてください )
     */
    //-------------------------------------------------------------------------
    bool IsEvolvedFormNoSpecified( u8 route_index )
    {
      return s_EvolutionTable->IsEvolvedFormNoSpecified( route_index );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 進化可能なレベルを取得する
     *
     * @param route_index  進化経路のインデックス[0, MaxEvolutionRouteNum]
     *
     * @retval [ 0, MAX_POKE_LEVEL ]
     */
    //-------------------------------------------------------------------------
    u8 GetEvolveEnableLevel( u8 route_index )
    {
      return s_EvolutionTable->GetEvolveEnableLevel( route_index );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メガ進化テーブルを読み込む
     * @param monsno  対象のモンスターNo.
     */
    //-------------------------------------------------------------------------
    void LoadMegaEvolveTable( MonsNo monsno )
    {
      LoadMegaEvolveTable( monsno, s_MegaEvolveTable );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メガ進化テーブルを読み込む
     * @param[in]  monsno  対象のモンスターNo.
     * @param[out] buffer  データの格納先
     */
    //-------------------------------------------------------------------------
    void LoadMegaEvolveTable( MonsNo monsno, MegaEvolveTable* buffer )
    {
      // キャッシュからデータを取り出す
      bool hit = s_MegaEvolveTableCache->GetTable( monsno, buffer );

      // キャッシュにHitしなかったら読み込む
      if( !hit )
      {
        s_MegaEvolveTable->LoadTable( monsno );
        s_MegaEvolveTableCache->RegisterTable( *s_MegaEvolveTable );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メガ進化経路の数を取得する
     */
    //-------------------------------------------------------------------------
    u8 GetMegaEvolveRouteNum( void ) 
    {
      return s_MegaEvolveTable->GetEvolutionRouteNum();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メガ進化の条件を取得する
     *
     * @param route_index  進化経路のインデックス [0, MAX_MEGA_EVOLVE_ROUTE_NUM]
     */
    //-------------------------------------------------------------------------
    MegaEvolveCond GetMegaEvolveCondition( u8 route_index ) 
    {
      return s_MegaEvolveTable->GetEvolutionCondition( route_index );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief メガ進化の条件を取得する
     *
     * @param next_monsno  進化後のモンスターNo.
     *
     * @return 指定したポケモンに進化するための条件
     */
    //-------------------------------------------------------------------------
    MegaEvolveCond GetMegaEvolveCondition( MonsNo next_monsno ) 
    {
      u32 route_num = s_MegaEvolveTable->GetEvolutionRouteNum();
      for( u32 i=0; i<route_num; i++ )
      {
        if( s_MegaEvolveTable->GetEvolvedFormNo(i) == next_monsno ) {
          return s_MegaEvolveTable->GetEvolutionCondition(i);
        }
      }
      return MEGA_EVOLVE_COND_NONE;  // 進化しない
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メガ進化条件に関するパラメータを取得する
     *
     * @param[in]  route_index  進化経路のインデックス[0, MAX_MEGA_EVOLVE_ROUTE_NUM]
     *
     * @return  進化条件に関するパラメータ
     */
    //-------------------------------------------------------------------------
    u16 GetMegaEvolveParam( u8 route_index )
    {
      return s_MegaEvolveTable->GetEvolutionParam( route_index );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メガ進化後のフォルムNo.を取得する
     *
     * @param route_index  進化経路のインデックス [0, MAX_MEGA_EVOLVE_ROUTE_NUM]
     */
    //-------------------------------------------------------------------------
    FormNo GetMegaEvolvedFormNo( u8 route_index )
    {
      return s_MegaEvolveTable->GetEvolvedFormNo( route_index );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief メガ進化後のフォルムかどうかを調べる
     * @param monsno  モンスターNo.
     * @param formno  フォルムNo.
     * @retval true   メガ進化後の姿
     * @retval false  メガ進化後の姿じゃない
     */
    //-------------------------------------------------------------------------
    bool CheckMegaForm( MonsNo monsno, FormNo formno )
    {
      LoadMegaEvolveTable( monsno, s_TemporaryMegaEvolveTable );

      u8 route_num = s_TemporaryMegaEvolveTable->GetEvolutionRouteNum();
      for( u8 i=0; i<route_num; i++ ) 
      {
        MegaEvolveCond cond = s_TemporaryMegaEvolveTable->GetEvolutionCondition( i );
        FormNo next_formno = s_TemporaryMegaEvolveTable->GetEvolvedFormNo( i );
        if( ( cond != MEGA_EVOLVE_COND_NONE ) && ( next_formno == formno ) )
        {
          return true;
        }
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ゲンシカイキ後のフォルムか？
     * @param monsno  モンスターNo.
     * @param formno  フォルムNo.
     * @retval true   ゲンシカイキ後の姿
     * @retval false  ゲンシカイキ後の姿じゃない
     */
    //-------------------------------------------------------------------------
    bool CheckGensiKaiki( MonsNo monsno, FormNo formno )
    {
      if( ( monsno == MONSNO_KAIOOGA  && formno == FORMNO_KAIOOGA_ALPHA  ) ||
          ( monsno == MONSNO_GURAADON && formno == FORMNO_GURAADON_OMEGA ) )
      {
        return true;
      }
      return false;
    }

    //-------------------------------------------------------------------------
    /**
    * @brief ウルトラバースト後のフォルムか？
    * @param monsno  モンスターNo.
    * @param formno  フォルムNo.
    * @retval true   ウルトラバースト後の姿
    * @retval false  ウルトラバースト後の姿じゃない
    */
    //-------------------------------------------------------------------------
    bool CheckUltraBurst(const MonsNo monsno, const FormNo formno)
    {
      if (monsno == MONSNO_PURIZUMU && formno == FORMNO_PURIZUMU_FORM_N)
      {
        return true;
      }
      return false;
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 種ポケモンのモンスターNo.を取得する
     * @param monsno 種ポケモンを調べるモンスター
     */
    //-------------------------------------------------------------------------
    MonsNo GetSeedMonsNo( MonsNo monsno )
    {
      if( MONSNO_END < monsno ) {
        GFL_ASSERT(0);  //@check　引数異常は何もせず戻る
        return monsno;
      }
      //BTS2874 kujiraの場合ヌケニンの種ポケがs_SeedMonsNoBuffer上ではヌケニンなので、ツチニンに強制置き換え
      if( monsno == MONSNO_NUKENIN ){
        return MONSNO_TUTININ;
      }
      //u16 data = 0;
      //s_SeedMonsNoTable->LoadData( monsno, &data );
      //GFL_ASSERT( data == static_cast<u16*>(s_SeedMonsNoBuffer)[ monsno ] ); @fix 検査用コード＆処理負荷となるのでコメント化
      //return static_cast<MonsNo>( data );
      return static_cast<MonsNo>(static_cast<u16*>(s_SeedMonsNoBuffer)[ monsno ]);
    }




    namespace
    {

      struct METENO_FormSet
      {
        FormNo shellForm;  // 殻つきフォルム
        FormNo coreForm;   // コアフォルム
      };

      static const METENO_FormSet METENO_FORM_TABLE[] = 
      {
        { FORMNO_NAGAREBOSI_M_RED,    FORMNO_NAGAREBOSI_RED    },
        { FORMNO_NAGAREBOSI_M_ORANGE, FORMNO_NAGAREBOSI_ORANGE },
        { FORMNO_NAGAREBOSI_M_YELLOW, FORMNO_NAGAREBOSI_YELLOW },
        { FORMNO_NAGAREBOSI_M_GREEN,  FORMNO_NAGAREBOSI_GREEN	 },
        { FORMNO_NAGAREBOSI_M_BLUE,   FORMNO_NAGAREBOSI_BLUE   },
        { FORMNO_NAGAREBOSI_M_INDIGO, FORMNO_NAGAREBOSI_INDIGO },
        { FORMNO_NAGAREBOSI_M_PURPLE, FORMNO_NAGAREBOSI_PURPLE },
      };

    }

    //-------------------------------------------------------------------------
    /**
     * @brief [メテノ] 指定したフォルムが殻つきフォルムか？
     * @param formno チェック対象のフォルム番号
     * @retval true    殻つきフォルムである
     * @retval false   殻つきフォルムでない
     */
    //-------------------------------------------------------------------------
    bool METENO_IsShellForm( FormNo formno )
    {
      for( u32 i=0; i<GFL_NELEMS(METENO_FORM_TABLE); ++i )
      {
        if( METENO_FORM_TABLE[i].shellForm == formno )
        {
          return true;
        }
      }

      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief [メテノ] 指定したフォルムがコアフォルムか？
     * @param formno チェック対象のフォルム番号
     * @retval true    殻つきフォルムである
     * @retval false   殻つきフォルムでない
     */
    //-------------------------------------------------------------------------
    bool METENO_IsCoreForm( FormNo formno )
    {
      for( u32 i=0; i<GFL_NELEMS(METENO_FORM_TABLE); ++i )
      {
        if( METENO_FORM_TABLE[i].coreForm == formno )
        {
          return true;
        }
      }

      return false;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief [メテノ] コアフォルムに対応する殻つきフォルムを取得する
     * @param coreFormNo コアフォルムのフォルム番号
     *
     * @return 殻つきフォルムのフォルム番号
     * @retval coreFormNo  対応する殻つきフォルムが存在しない場合
     */
    //-------------------------------------------------------------------------
    FormNo METENO_GetShellFormNo( FormNo coreFormNo )
    {
      if( METENO_IsShellForm( coreFormNo ) )
      {
        return coreFormNo;
      }

      for( u32 i=0; i<GFL_NELEMS(METENO_FORM_TABLE); ++i )
      {
        if( METENO_FORM_TABLE[i].coreForm == coreFormNo )
        {
          return METENO_FORM_TABLE[i].shellForm;
        }
      }

      GFL_ASSERT(0);
      return coreFormNo;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief [メテノ] 殻つきフォルムに対応するコアフォルムを取得する
     * @param 殻つきフォルムのフォルム番号
     *
     * @return コアフォルムのフォルム番号
     * @retval coreFormNo  対応するコアフォルムが存在しない場合
     */
    //-------------------------------------------------------------------------
    FormNo METENO_GetCoreFormNo( FormNo shellFormNo )
    {
      if( METENO_IsCoreForm( shellFormNo ) )
      {
        return shellFormNo;
      }

      for( u32 i=0; i<GFL_NELEMS(METENO_FORM_TABLE); ++i )
      {
        if( METENO_FORM_TABLE[i].shellForm == shellFormNo )
        {
          return METENO_FORM_TABLE[i].coreForm;
        }
      }

      GFL_ASSERT(0);
      return shellFormNo;
    }


  } // namespace personal
} // namespace pml 
