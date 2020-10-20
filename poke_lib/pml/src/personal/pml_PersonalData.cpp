//=============================================================================
/**
 * @brief  ポケモンパーソナルデータのアクセッサ
 * @file   pml_PersonalData.cpp
 * @author obata_toshihiro
 * @date   2010.12.21
 */
//=============================================================================
#include <debug/include/gfl2_Assert.h>
#include <util/include/gfl2_std_string.h>

#include <pml/include/system/pml_Library.h>
#include <pml/include/personal/pml_PersonalConst.h>
#include <pml/include/personal/pml_PersonalData.h>
#include "pml_PersonalLocal.h"

namespace pml { 
  namespace personal { 


    const char* PersonalData::m_garcPath = NULL;
    BasicParams * PersonalData::m_alldata = NULL;
    //-------------------------------------------------------------------------
    /**
     * @brief 準備
     * @param heap      作業領域に使用するヒープ
     * @param garc_path パーソナルデータファイルのパス
     */
    //-------------------------------------------------------------------------
    void PersonalData::Initialize( 
        gfl2::heap::HeapBase* heap, const char* garc_path )
    {
      if( m_garcPath == NULL )
      {
        m_garcPath = garc_path;

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

        const gfl2::fs::ArcFile *arc = pml::Library::m_pmlFileManager->GetArcFile( garc_path );
        u32 count;
        size_t size,topSize;
        arc->GetDataCount(&count);
        u32 id = count - 1;
        arc->GetDataSize( &size , id , heap );
        arc->GetDataSize( &topSize , 0 , heap );
        GFL_ASSERT_STOP( sizeof(BasicParams) * (count - 1 ) == size );  //@fix 必ず通るので毎回チェックされる
        GFL_ASSERT_STOP( sizeof(BasicParams) == topSize );  //@fix 必ず通るので毎回チェックされる
        void * buf = static_cast<BasicParams*>(GflHeapAllocMemory( heap, (int)size ));

        gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq readReq;
        readReq.fileName = m_garcPath;
        readReq.bufSize = size;
        readReq.datId = id;
        readReq.pBuf = buf;
        pml::Library::m_pmlFileManager->SyncArcFileLoadDataBuf( readReq );

        m_alldata = static_cast<BasicParams*>(buf);
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 後片付け
     */
    //-------------------------------------------------------------------------
    void PersonalData::Finalize( void )
    {
      if( m_garcPath )
      {
        GflHeapFreeMemory( m_alldata );

        gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
        closeReq.fileName = m_garcPath;
        pml::Library::m_pmlFileManager->SyncArcFileClose( closeReq );
        m_garcPath = NULL;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap インスタンスの生成に使用するヒープ
     */
    //-------------------------------------------------------------------------
    PersonalData::PersonalData( gfl2::heap::HeapBase* heap ) :
      m_monsno( MONSNO_NULL ),
      m_formno( 0 ),
      m_data( NULL )
    {
      m_data = static_cast<BasicParams*>(GflHeapAllocMemory( heap, sizeof(BasicParams) ));
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    PersonalData::~PersonalData()
    {
      GflHeapFreeMemory( m_data );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief モンスターNo.を取得する
     */
    //-------------------------------------------------------------------------
    MonsNo PersonalData::GetMonsNo( void ) const
    {
      return m_monsno;
    }
    
    //-------------------------------------------------------------------------
    /**
     * @brief フォルムNo.を取得する
     */
    //-------------------------------------------------------------------------
    FormNo PersonalData::GetFormNo( void ) const
    {
      return m_formno;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief パーソナルデータを読み込gむ
     * @param monsno モンスターNo.
     * @param formno フォルムNo.
     */
    //-------------------------------------------------------------------------
    void PersonalData::LoadData( MonsNo monsno, FormNo formno )
    {
      u32 dataID = PersonalData::GetDataID( monsno, formno );
      //m_arcfile->LoadData( dataID, m_data ); 
      *m_data = m_alldata[ dataID ];
      m_monsno = monsno;
      m_formno = formno;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief パーソナルデータのIDを取得する
     * @param monsno  対象のモンスターナンバー
     * @param formno  対象のフォルムナンバー
     */
    //-------------------------------------------------------------------------
    u32 PersonalData::GetDataID( MonsNo monsno, FormNo formno )
    { 
      // タマゴにはパーソナルがないので, ダミーデータを読み込ませるようにする
      if( ( monsno == MONSNO_TAMAGO ) || 
          ( monsno == MONSNO_DAMETAMAGO ) ||
          ( MONSNO_END < monsno ) )
      {
        return 0;
      }

      if( formno == 0 )
      {
        return monsno;
      }
      else
      {
        BasicParams params;
        params = m_alldata[ monsno ];
        //m_arcfile->LoadDataPiece( monsno, 0, sizeof(BasicParams), &params );

        if( ( params.form_index == 0 ) || ( params.form_max <= formno ) )
        {
          return monsno;
        }
        else
        {
          return params.form_index + formno - 1;
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief データをコピーする
     * @param[out] dest コピー先
     */
    //-------------------------------------------------------------------------
    void PersonalData::CopyData( PersonalData* dest ) const
    {
      gfl2::std::MemCopy( m_data, dest->m_data, sizeof(BasicParams) );
      dest->m_monsno = m_monsno;
      dest->m_formno = m_formno;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 基本パーソナルデータを取得する
     */
    //-------------------------------------------------------------------------
    u32 PersonalData::GetParam( ParamID paramId ) const
    {
      switch( paramId ) {
      case PARAM_ID_basic_hp:             return m_data->basic_hp;            // 基本ＨＰ
      case PARAM_ID_basic_atk:            return m_data->basic_atk;           // 基本攻撃力
      case PARAM_ID_basic_def:            return m_data->basic_def;           // 基本防御力
      case PARAM_ID_basic_agi:            return m_data->basic_agi;           // 基本素早さ
      case PARAM_ID_basic_spatk:          return m_data->basic_spatk;         // 基本特殊攻撃力
      case PARAM_ID_basic_spdef:          return m_data->basic_spdef;         // 基本特殊防御力
      case PARAM_ID_type1:                return m_data->type1;               // 属性１
      case PARAM_ID_type2:                return m_data->type2;               // 属性２
      case PARAM_ID_get_rate:             return m_data->get_rate;            // 捕獲率
      case PARAM_ID_give_exp:             return m_data->give_exp;            // 贈与経験値
      case PARAM_ID_pains_hp:             return m_data->pains_hp;            // 贈与努力値ＨＰ
      case PARAM_ID_pains_atk:            return m_data->pains_atk;           // 贈与努力値攻撃力
      case PARAM_ID_pains_def:            return m_data->pains_def;           // 贈与努力値防御力
      case PARAM_ID_pains_agi:            return m_data->pains_agi;           // 贈与努力値素早さ
      case PARAM_ID_pains_spatk:          return m_data->pains_spatk;         // 贈与努力値特殊攻撃力
      case PARAM_ID_pains_spdef:          return m_data->pains_spdef;         // 贈与努力値特殊防御力
      case PARAM_ID_no_jump:              return m_data->no_jump;             // はねる禁止フラグ
      case PARAM_ID_item1:                return m_data->item1;               // アイテム１
      case PARAM_ID_item2:                return m_data->item2;               // アイテム２
      case PARAM_ID_item3:                return m_data->item3;               // アイテム３
      case PARAM_ID_sex:                  return m_data->sex;                 // 性別ベクトル
      case PARAM_ID_egg_birth:            return m_data->egg_birth;           // タマゴの孵化歩数
      case PARAM_ID_initial_familiarity:  return m_data->initial_familiarity; // なつき度初期値
      case PARAM_ID_grow:                 return m_data->grow;                // 成長曲線識別
      case PARAM_ID_egg_group1:           return m_data->egg_group1;          // こづくりグループ1
      case PARAM_ID_egg_group2:           return m_data->egg_group2;          // こづくりグループ2
      case PARAM_ID_tokusei1:             return m_data->tokusei1;            // 特殊能力１
      case PARAM_ID_tokusei2:             return m_data->tokusei2;            // 特殊能力２
      case PARAM_ID_tokusei3:             return m_data->tokusei3;            // 特殊能力３
      case PARAM_ID_form_index:           return m_data->form_index;          // 別フォルムパーソナルデータ開始位置
      case PARAM_ID_form_gra_index:       return m_data->form_gra_index;      // 別フォルムグラフィックデータ開始位置
      case PARAM_ID_form_max:             return m_data->form_max;            // 別フォルムMAX
      case PARAM_ID_color:                return m_data->color;               // 色（図鑑で使用）
      case PARAM_ID_rank:                 return m_data->rank;                // ポケモンランク
      case PARAM_ID_height:               return m_data->height;              // 高さ
      case PARAM_ID_weight:               return m_data->weight;              // 重さ
      case PARAM_ID_machine1:             return m_data->machine1;            // 技マシンフラグ1
      case PARAM_ID_machine2:             return m_data->machine2;            // 技マシンフラグ2
      case PARAM_ID_machine3:             return m_data->machine3;            // 技マシンフラグ3
      case PARAM_ID_machine4:             return m_data->machine4;            // 技マシンフラグ4
      case PARAM_ID_waza_oshie_kyukyoku:  return m_data->waza_oshie_kyukyoku;     // 技教え究極
      case PARAM_ID_waza_oshie_momiji1:   return m_data->waza_oshie_momiji1;      // 技教え(momiji追加分1)
      case PARAM_ID_waza_oshie_momiji2:   return m_data->waza_oshie_momiji2;      // 技教え(momiji追加分2)
      case PARAM_ID_waza_oshie_momiji3:   return m_data->waza_oshie_momiji3;      // 技教え(momiji追加分3)
      case PARAM_ID_sos_rate:             return m_data->sos_rate;            // 助けを呼ぶ割合
      case PARAM_ID_zenryoku_waza_item:   return m_data->zenryoku_waza_item;      // ポケモン専用全力技を使用するための道具No.
      case PARAM_ID_zenryoku_waza_before: return m_data->zenryoku_waza_before;    // ポケモン専用全力技の元技の技No.
      case PARAM_ID_zenryoku_waza_after:  return m_data->zenryoku_waza_after;     // ポケモン専用全力技の技No.
      case PARAM_ID_region:               return m_data->region;                  // 地方種ID

      default: GFL_ASSERT(0); /* @check */ return 0;
      }
    }

    //------------------------------------------------------------------------------
    /**
     * @brief 性別ベクトルのタイプを取得する
     */
    //------------------------------------------------------------------------------
    SexType PersonalData::GetSexType( void ) const
    {
      u32 sex_vector = this->GetParam( PARAM_ID_sex );

      if( ( sex_vector == SEX_VECTOR_ONLY_MALE ) ||
          ( sex_vector == SEX_VECTOR_ONLY_FEMALE ) ||
          ( sex_vector == SEX_VECTOR_UNKNOWN ) )
      {
        return SEX_TYPE_FIX;
      }
      else
      {
        return SEX_TYPE_RANDOM;
      }
    }

    //------------------------------------------------------------------------------
    /**
     * @brief 技マシンで技を覚えるかどうかをチェックする
     *
     * @param machineNo 技マシンNo.
     */
    //------------------------------------------------------------------------------
    bool PersonalData::CheckWazaMachine( u32 machineNo ) const
    {
      u32 machine_bit = ( 1 << ( machineNo % 32 ) );
      ParamID param_id = static_cast<ParamID>(PARAM_ID_machine1 + ( machineNo / 32 ));
      u32 param = this->GetParam( param_id );
      return ( ( param & machine_bit ) != 0 );
    }

    //------------------------------------------------------------------------------
    /**
     * @brief 技教えで技を覚えるかどうかをチェックする（究極技）
     *
     * @param oshieNo 技教えNo.
     */
    //------------------------------------------------------------------------------
    bool PersonalData::CheckWazaOshie( u32 oshieNo ) const
    {
      u32 oshie_bit = ( 1 << ( oshieNo % 32 ) );
      ParamID param_id = PARAM_ID_waza_oshie_kyukyoku;
      u32 param = this->GetParam( param_id );
      return ( ( param & oshie_bit ) != 0 );
    }

    //------------------------------------------------------------------------------
    /**
    * @brief BP技教えで技を覚えるかどうかをチェックする
    *
    * @param wazaNo 技No.
    */
    //------------------------------------------------------------------------------
    bool PersonalData::CheckBpWazaOshie(u32 wazaNo) const
    {
      // BP技教え技テーブル
      static const WazaNo wazaTable[] = {
        WAZANO_MUSIKUI,
        WAZANO_HOSIGARU,
        WAZANO_IKARINOMAEBA,
        WAZANO_DABURUTYOPPU,
        WAZANO_SIGUNARUBIIMU,
        WAZANO_AIANHEDDO,
        WAZANO_TANEBAKUDAN,
        WAZANO_DORIRURAINAA,
        WAZANO_TOBIHANERU,
        WAZANO_KETAGURI,
        WAZANO_DASUTOSYUUTO,
        WAZANO_SAWAGU,
        WAZANO_KAMINARIPANTI,
        WAZANO_HONOONOPANTI,
        WAZANO_REITOUPANTI,
        WAZANO_MAZIKKUKOOTO,
        WAZANO_TOOSENBOU,
        WAZANO_DAITINOTIKARA,
        WAZANO_IKASAMA,
        WAZANO_ZYUURYOKU,
        WAZANO_DENZIHUYUU,
        WAZANO_TEPPEKI,
        WAZANO_TOTTEOKI,
        WAZANO_BAKADIKARA,
        WAZANO_EREKINETTO,
        WAZANO_KOGOERUKAZE,
        WAZANO_AKUATEERU,
        WAZANO_SINENNOZUTUKI,
        WAZANO_RYUUNOHADOU,
        WAZANO_HAIPAABOISU,
        WAZANO_AIANTEERU,
        WAZANO_SIMETUKERU,
        WAZANO_IBIKI,
        WAZANO_HATAKIOTOSU,
        WAZANO_KOUGOUSEI,
        WAZANO_NEPPUU,
        WAZANO_NARIKIRI,
        WAZANO_IYASINOSUZU,
        WAZANO_OIKAZE,
        WAZANO_GODDOBAADO,
        WAZANO_ITAMIWAKE,
        WAZANO_GIGADOREIN,
        WAZANO_DOREINPANTI,
        WAZANO_KIAIPANTI,
        WAZANO_DENGEKIHA,
        WAZANO_MIZUNOHADOU,
        WAZANO_IEKI,
        WAZANO_NAYAMINOTANE,
        WAZANO_URAMI,
        WAZANO_OSAKINIDOUZO,
        WAZANO_TEDASUKE,
        WAZANO_TORIKKU,
        WAZANO_MAZIKKURUUMU,
        WAZANO_WANDAARUUMU,
        WAZANO_GAMUSYARA,
        WAZANO_GEKIRIN,
        WAZANO_RISAIKURU,
        WAZANO_YOKODORI,
        WAZANO_SUTERUSUROKKU,
        WAZANO_SUKIRUSUWAPPU,
        WAZANO_TEREKINESISU,
        WAZANO_SAIDOTHENZI,
        WAZANO_KIRIBARAI,
        WAZANO_AKUABUREIKU,
        WAZANO_ZIDANDA,
        WAZANO_ZIGOKUDUKI,
        WAZANO_TOGISUMASU
      };

      // テーブルから検索
      int waza_oshie_idx = -1;
      for(u32 i = 0;i < GFL_NELEMS(wazaTable);i++)
      {
        if(wazaTable[i] == wazaNo)
        {
          waza_oshie_idx = i;
          break;
        }
      }

      // リストにない技を指定した
      if(waza_oshie_idx < 0) return false;

      // リストにあった技のビットを見る
      u32 oshie_bit = (1 << (waza_oshie_idx % 32));
      ParamID param_id = static_cast<ParamID>(PARAM_ID_waza_oshie_momiji1 + waza_oshie_idx / 32);
      GFL_ASSERT_STOP_MSG(param_id <= PARAM_ID_waza_oshie_momiji3,"教え技：不正な値です\n");
      u32 param = this->GetParam(param_id);
      return ((param & oshie_bit) != 0);
    }

  } // namespace personal
} // namespace pml
