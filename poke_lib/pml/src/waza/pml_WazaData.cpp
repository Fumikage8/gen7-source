//=============================================================================
/**
 * @brief 技データ
 * @file pml_WazaData.h
 * @author obata_toshihiro
 * @date 2011.05.06
 */
//=============================================================================
#include <debug/include/gfl2_Assert.h>
#include <util/include/gfl2_std_string.h>

#include <pml/include/system/pml_Library.h>
#include <pml/include/waza/pml_WazaDataConst.h>
#include <pml/include/waza/pml_WazaData.h>
#include "pml_WazaDataLocal.h"


namespace pml {
  namespace wazadata {


    WAZA_DATA * WazaData::m_alldata = NULL;
    const char* WazaData::m_garcPath = NULL;
    //-------------------------------------------------------------------------
    /**
     * @brief 準備
     * @param heap      作業領域に使用するヒープ
     * @param garc_path 技データファイルのパス
     */
    //-------------------------------------------------------------------------
    void WazaData::Initialize( gfl2::heap::HeapBase* heap, const char* garc_path )
    {
      if( m_alldata == NULL )
      {
        m_garcPath = garc_path;
        //nn::os::Tick tick_s = nn::os::Tick::GetSystemCurrent();

        gfl2::fs::AsyncFileManager::ArcFileOpenReq arcReq;
        arcReq.fileName = m_garcPath;
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
        
        gfl2::fs::BinLinkerAccessor binData;
        size_t  fileSize,readSize;

        void *binBuf;
        gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadRec;
        loadRec.fileName = m_garcPath;
        loadRec.datId = 0;
        loadRec.ppBuf = &binBuf;
        loadRec.align = heap->GetDefaultAlign();
        loadRec.pBufSize = &fileSize;
        loadRec.pRealReadSize = &readSize;
        loadRec.heapForBuf = heap->GetLowerHandle();
        pml::Library::m_pmlFileManager->SyncArcFileLoadData(loadRec);

        binData.Initialize(binBuf);
        
        u32 count = binData.GetDataMax();
        u32 size = binData.GetDataSize(0) * count;
        WAZA_DATA * buf = static_cast<WAZA_DATA*>(GflHeapAllocMemory( heap, size ));
        m_alldata = buf;
        for (u32 i = 0; i < count; i++)
        {
          GFL_ASSERT_STOP( sizeof(WAZA_DATA) == binData.GetDataSize(i) );  //@fix 必ず通るので毎回チェックされる
          gfl2::std::MemCopy( binData.GetData(i) , &buf[i] , binData.GetDataSize(i) );
        }
        GflHeapFreeMemory( binBuf );

        //nn::os::Tick tick_n = nn::os::Tick::GetSystemCurrent();
        //s64 now = tick_n - tick_s;
        // rommkでエラーになるので気をつける
        //GFL_PRINT( "waza make tick %d\n", now );
        //GFL_PRINT( "waza make micro %d\n", nn::os::Tick(now).ToTimeSpan().GetMicroSeconds() );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 後片付け
     */
    //-------------------------------------------------------------------------
    void WazaData::Finalize( void )
    {
      if( m_garcPath )
      {
        gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
        closeReq.fileName = m_garcPath;
        pml::Library::m_pmlFileManager->SyncArcFileClose( closeReq );

        m_garcPath = NULL;
        GflHeapFreeMemory( m_alldata );
        m_alldata = NULL;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap バッファ確保に使用するヒープ
     */
    //-------------------------------------------------------------------------
    WazaData::WazaData( gfl2::heap::HeapBase* heap ) :
      m_wazano( WAZANO_NULL ),
      m_data( NULL )
    {
      m_data = static_cast<WAZA_DATA*>(GflHeapAllocMemory( heap, sizeof(WAZA_DATA) ));
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    WazaData::~WazaData()
    {
      GflHeapFreeMemory( m_data ); 
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技データを読み込む
     * @param wazano 対象の技No.
     */
    //-------------------------------------------------------------------------
    void WazaData::LoadData( WazaNo wazano )
    {
      if( wazano >= WAZANO_MAX )
      {
        GFL_ASSERT( 0 );  // @fix 引数異常を補正する
        wazano = WAZANO_NULL;
      }
      if( m_wazano != wazano )
      {
        *m_data = m_alldata[ wazano ];
        m_wazano = wazano;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief データをコピーする
     * @param[out] dest コピー先
     */
    //-------------------------------------------------------------------------
    void WazaData::CopyData( WazaData* dest ) const
    {
      gfl2::std::MemCopy( m_data, dest->m_data, sizeof(WAZA_DATA) );
      dest->m_wazano = this->m_wazano;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技No.を取得する
     */
    //-------------------------------------------------------------------------
    WazaNo WazaData::GetWazaNo( void ) const
    {
      return m_wazano;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技パラメータを取得する
     * @param paramID 取得したいパラメータのID
     */
    //-------------------------------------------------------------------------
    s32 WazaData::GetParam( ParamID paramID ) const
    {
      switch( paramID ){
      case PARAM_ID_TYPE:                ///< タイプ（みず、くさ、ほのお…）
        return m_data->type;
      case PARAM_ID_CATEGORY:            ///< カテゴリ（ enum WazaCategory )
        return m_data->category;
      case PARAM_ID_DAMAGE_TYPE:         ///< ダメージタイプ（ enum WazaDamageType )
        return m_data->damageType;
      case PARAM_ID_POWER:               ///< 威力
        return m_data->power;
      case PARAM_ID_HITPER:              ///< 命中率
        if( m_data->hitPer != HITRATIO_MUST ){
          return m_data->hitPer;
        }else{
          return 100;
        }
      case PARAM_ID_MAST_HIT_FLAG:
        return (m_data->hitPer == HITRATIO_MUST);
      case PARAM_ID_BASE_PP:             ///< 基本PPMax
        return m_data->basePP;
      case PARAM_ID_PRIORITY:            ///< ワザ優先度
        return m_data->priority;
      case PARAM_ID_CRITICAL_RANK:       ///< クリティカルランク
        return m_data->criticalRank;
      case PARAM_ID_HITCOUNT_MAX:        ///< 最大ヒット回数
        return m_data->hitCountMax;
      case PARAM_ID_HITCOUNT_MIN:        ///< 最小ヒット回数
        return m_data->hitCountMin;
      case PARAM_ID_SHRINK_PER:          ///< ひるみ確率
        return m_data->shrinkPer;
      case PARAM_ID_SICK:                ///< 状態異常コード（enum WazaSick）
        return m_data->sickID;
      case PARAM_ID_SICK_PER:            ///< 状態異常の発生率
        return m_data->sickPer;
      case PARAM_ID_SICK_CONT:           ///< 状態異常継続パターン（enum WazaSickCont）
        return m_data->sickCont;
      case PARAM_ID_SICK_TURN_MIN:       ///< 状態異常の継続ターン最小
        return m_data->sickTurnMin;
      case PARAM_ID_SICK_TURN_MAX:       ///< 状態異常の継続ターン最大
        return m_data->sickTurnMax;

      case PARAM_ID_AI_SEQNO:
        return m_data->AISeqNo;

      case PARAM_ID_RANKTYPE_1:          ///< ステータスランク効果１
      case PARAM_ID_RANKTYPE_2:          ///< ステータスランク効果２
      case PARAM_ID_RANKTYPE_3:          ///< ステータスランク効果３
        return m_data->rankEffType[ paramID - PARAM_ID_RANKTYPE_1 ];

      case PARAM_ID_RANKVALUE_1:         ///< ステータスランク効果１増減値
      case PARAM_ID_RANKVALUE_2:         ///< ステータスランク効果２増減値
      case PARAM_ID_RANKVALUE_3:         ///< ステータスランク効果３増減値
        return m_data->rankEffValue[ paramID - PARAM_ID_RANKVALUE_1 ];

      case PARAM_ID_RANKPER_1:           ///< ステータスランク効果１の発生率
      case PARAM_ID_RANKPER_2:           ///< ステータスランク効果２の発生率
      case PARAM_ID_RANKPER_3:           ///< ステータスランク効果３の発生率
        return m_data->rankEffPer[ paramID - PARAM_ID_RANKPER_1 ];

      case PARAM_ID_ZENRYOKU_WAZANO:         ///< 全力技として使用した場合の、変化後の技No.
        return m_data->zenryokuWazaNo;    
      case PARAM_ID_ZENRYOKU_POWER:          ///< 全力技として使用した場合の、変化後の威力
        return m_data->zenryokuPower;
      case PARAM_ID_ZENRYOKU_EFFECT:         ///< 全力技として使用した場合の、追加効果
        return m_data->zenryokuEffect;

      case PARAM_ID_DIRT_TYPE:
        return m_data->dirtType;
      case PARAM_ID_DIRT_RATE:
        return m_data->dirtRate;

      case PARAM_ID_DAMAGE_RECOVER_RATIO:///< ダメージ回復率
        if( m_data->damageRecoverRatio > 0 ){
          return m_data->damageRecoverRatio;
        }else{
          return 0;
        }
      case PARAM_ID_DAMAGE_REACTION_RATIO:
        if( m_data->damageRecoverRatio < 0 ){
          return m_data->damageRecoverRatio * -1;
        }else{
          return 0;
        }
      case PARAM_ID_HP_RECOVER_RATIO:    ///< HP回復率
        if( m_data->HPRecoverRatio > 0 ){
          return m_data->HPRecoverRatio;
        }else{
          return 0;
        }
      case PARAM_ID_HP_REACTION_RATIO:
        if( m_data->HPRecoverRatio < 0 ){
          return m_data->HPRecoverRatio * -1;
        }else{
          return 0;
        }
      case PARAM_ID_TARGET:             ///< ワザ効果範囲( enum WazaTarget )
        return m_data->target;

      default:
        GFL_ASSERT(0);  //@check プログラム検証用　引数異常は0を返す
        break;
      }
      return 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技の各種フラグを調べる
     * @param wazano  技No.
     * @param flag    調べるフラグ
     * @return 指定したフラグの値
     */
    //-------------------------------------------------------------------------
    bool WazaData::GetFlag( WazaFlag flag ) const
    {
      return ( m_data->flags & ( 1 << flag ) ) != 0;
    }

  }
}
