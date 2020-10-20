//==============================================================================
/**
 * @file	Regulation.cpp
 * @brief レギュレーションデータ構造
 * @author	ichiraku_katsuhiko
 * @date	2015/07/15 19:56:59
 */
// =============================================================================

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_std_string.h>

// pmlib
#include <pml/include/pmlib.h>

// crc
#include <math/include/gfl2_math.h>

// arc
#include <GameSys/include/GameManager.h>
#include <arc_def_index/arc_def.h>
#include <arc_index/RegulationData.gaix>
#include "AppLib/include/Util/app_util_FileAccessor.h"

// PMVersion
#include "System/include/PokemonVersion.h"

#include "Battle/Regulation/include/PokeRegulation.h"

// mine
#include "Battle/Regulation/include/Regulation.h"

//GFL_NAMESPACE_BEGIN(Battle)

// デバッグ用パラメータ
#if PM_DEBUG
bool Regulation::m_debugCrcBroken = false;   //!< CRC不一致デバッグ
#endif


// コンストラクタ
Regulation::Regulation( gfl2::heap::HeapBase* pHeap ) :
 m_pHeap( pHeap )
 ,m_pRegulationData(NULL)
 ,m_regulationDataSize(0)
 ,m_pCoreData(NULL)
{
  STATIC_ASSERT( sizeof(CoreData) == 1192 );
}
// デストラクタ
Regulation::~Regulation()
{
  GflHeapSafeFreeMemory(m_pRegulationData);
}

//------------------------------------------------------------------
/**
 * @brief   シリアライズデータサイズ取得
 */
//------------------------------------------------------------------
u32 Regulation::GetSerializeDataSize() const
{
  GFL_ASSERT(m_regulationDataSize);
  return m_regulationDataSize;
}

//------------------------------------------------------------------
/**
 * @brief   シリアライズ
 */
//------------------------------------------------------------------
void Regulation::Serialize( void* dst ) const
{
  gfl2::std::MemCopy( m_pRegulationData, dst, m_regulationDataSize );
}

//------------------------------------------------------------------
/**
 * @brief   デシリアライズ
 */
//------------------------------------------------------------------
void Regulation::Deserialize( const void* data, const u32 size )
{
  this->SetData( data, size );
}


//----------------------------------------------------------
/**
 * @breif       ＲＯＭからレギュレーションデータを得る
 * @param[in]   preset プリセットレギュレーション
 */
//----------------------------------------------------------
void Regulation::LoadData( const PRESET preset )
{
  this->AllocWork( FIX_CORE_DATA_SIZE );

  // GARCから読み込む
  app::util::FileAccessor::FileOpenSync( ARCID_REGULATION_DATA, m_pHeap );
  app::util::FileAccessor::FileLoadBufSync( ARCID_REGULATION_DATA,
                      static_cast<gfl2::fs::ArcFile::ARCDATID>(preset),
                      m_pRegulationData,
                      m_regulationDataSize,
                      m_pHeap->GetLowerHandle(),
                      false );
  app::util::FileAccessor::FileCloseSync( ARCID_REGULATION_DATA );
  m_pCoreData = reinterpret_cast<CoreData*>(m_pRegulationData);
}


//----------------------------------------------------------
/**
 * @breif       レギュレーションデータの削除
 */
//----------------------------------------------------------
void Regulation::DeleteData( void )
{
  GflHeapSafeFreeMemory(m_pRegulationData);
  m_regulationDataSize = 0;
  m_pCoreData = NULL;
}

//----------------------------------------------------------
/**
 * @breif       DLしたレギュレーションのバッファをセット
 * @param[in]   pBuffer   レギュレーションデータ
 * @param[in]   size      レギュレーションデータのサイズ
 * @note        内部でデータをコピーします。
 */
//----------------------------------------------------------
void Regulation::SetData( const void* pBuffer, const u32 size )
{
  this->AllocWork( size );
  gfl2::std::MemCopy( pBuffer, m_pRegulationData, m_regulationDataSize );
  m_pCoreData = reinterpret_cast<CoreData*>(m_pRegulationData);
}

//----------------------------------------------------------
/**
 * @breif       圧縮レギュレーションのバッファをセット
 * @param[in]   pBuffer   レギュレーションデータ
 * @note        内部でデータをコピーします。
 */
//----------------------------------------------------------
void Regulation::SetSimpleData( const void* pBuffer )
{
  const SimpleData* pSimpleData = reinterpret_cast<const SimpleData*>(pBuffer);
  this->AllocWork( FIX_CORE_DATA_SIZE );
  m_pCoreData = reinterpret_cast<CoreData*>(m_pRegulationData);

  m_pCoreData->regulation_version = pSimpleData->regulation_version;
  m_pCoreData->patch_version_major = pSimpleData->patch_version_major;
  m_pCoreData->patch_version_minor = pSimpleData->patch_version_minor;
  m_pCoreData->patch_version_build = pSimpleData->patch_version_build;
  m_pCoreData->category = pSimpleData->category;
  m_pCoreData->btl_rule = pSimpleData->btl_rule;
  m_pCoreData->team_num_lo = pSimpleData->team_num_lo;
  m_pCoreData->team_num_hi = pSimpleData->team_num_hi;
  m_pCoreData->select_num_lo = pSimpleData->select_num_lo;
  m_pCoreData->select_num_hi = pSimpleData->select_num_hi;
  m_pCoreData->legend_num_hi = pSimpleData->legend_num_hi;
  m_pCoreData->level = pSimpleData->level;
  m_pCoreData->level_range = pSimpleData->level_range;
  m_pCoreData->reverse = pSimpleData->reverse;
  m_pCoreData->both_poke = pSimpleData->both_poke;
  m_pCoreData->both_item = pSimpleData->both_item;
  m_pCoreData->huraette_hakai = pSimpleData->huraette_hakai;
  m_pCoreData->qr_team = pSimpleData->qr_team;
  m_pCoreData->time_limit_type = pSimpleData->time_limit_type;
  m_pCoreData->time_vs = pSimpleData->time_vs;
  m_pCoreData->time_command = pSimpleData->time_command;
  m_pCoreData->nickname = pSimpleData->nickname;
  m_pCoreData->background = pSimpleData->background;
  m_pCoreData->show_poke = pSimpleData->show_poke;
  m_pCoreData->show_poke_time = pSimpleData->show_poke_time;
  m_pCoreData->discriminant_id = pSimpleData->discriminant_id;
  m_pCoreData->battle_num = pSimpleData->battle_num;
  m_pCoreData->battle_num_limit = pSimpleData->battle_num_limit;
  m_pCoreData->start_enable_time = pSimpleData->start_enable_time;
  m_pCoreData->end_enable_time = pSimpleData->end_enable_time;
  m_pCoreData->start_year = pSimpleData->start_year;
  m_pCoreData->start_month = pSimpleData->start_month;
  m_pCoreData->start_day = pSimpleData->start_day;
  m_pCoreData->end_year = pSimpleData->end_year;
  m_pCoreData->end_month = pSimpleData->end_month;
  m_pCoreData->end_day = pSimpleData->end_day;

  ::std::memcpy( m_pCoreData->veto_poke_bit , pSimpleData->veto_poke_bit , sizeof(m_pCoreData->veto_poke_bit) );
  ::std::memcpy( m_pCoreData->specific_veto_poke , pSimpleData->specific_veto_poke , sizeof(m_pCoreData->specific_veto_poke) );
  ::std::memcpy( m_pCoreData->specific_veto_poke_form , pSimpleData->specific_veto_poke_form , sizeof(m_pCoreData->specific_veto_poke_form) );
  ::std::memcpy( m_pCoreData->must_poke_and , pSimpleData->must_poke_and , sizeof(m_pCoreData->must_poke_and) );
  ::std::memcpy( m_pCoreData->must_poke_form_and , pSimpleData->must_poke_form_and , sizeof(m_pCoreData->must_poke_form_and) );
  ::std::memcpy( m_pCoreData->must_poke_or , pSimpleData->must_poke_or , sizeof(m_pCoreData->must_poke_or) );
  ::std::memcpy( m_pCoreData->must_poke_form_or , pSimpleData->must_poke_form_or , sizeof(m_pCoreData->must_poke_form_or) );
  ::std::memcpy( m_pCoreData->producing_area , pSimpleData->producing_area , sizeof(m_pCoreData->producing_area) );
  ::std::memcpy( m_pCoreData->rom_bit , pSimpleData->rom_bit , sizeof(m_pCoreData->rom_bit) );
  ::std::memcpy( m_pCoreData->language_bit , pSimpleData->language_bit , sizeof(m_pCoreData->language_bit) );
  ::std::memcpy( m_pCoreData->cupname1 , pSimpleData->cupname1 , sizeof(m_pCoreData->cupname1) );
  ::std::memcpy( m_pCoreData->cupname2 , pSimpleData->cupname2 , sizeof(m_pCoreData->cupname2) );

  //圧縮レギュレーション固有

  if( pSimpleData->vetoMegaRing != 0 )
  {
    const u32 VETO_RING_LIST[] =
    {
      656,//		ゲンガナイト					
      657,//		サーナイトナイト					
      658,//		デンリュウナイト					
      659,//		フシギバナイト					
      660,//		リザードナイトＸ					
      661,//		カメックスナイト					
      662,//		ミュウツナイトＸ					
      663,//		ミュウツナイトＹ					
      664,//		バシャーモナイト					
      665,//		チャーレムナイト					
      666,//		ヘルガナイト					
      667,//		ボスゴドラナイト					
      668,//		ジュペッタナイト					
      669,//		バンギラスナイト					
      670,//		ハッサムナイト					
      671,//		カイロスナイト					
      672,//		プテラナイト					
      673,//		ルカリオナイト					
      674,//		ユキノオナイト					
      675,//		ガルーラナイト					
      676,//		ギャラドスナイト					
      677,//		アブソルナイト					
      678,//		リザードナイトＹ					
      679,//		フーディナイト					
      680,//		ヘラクロスナイト					
      681,//		クチートナイト					
      682,//		ライボルトナイト					
      683,//		ガブリアスナイト					
      684,//		ラティアスナイト					
      685,//		ラティオスナイト					
      752,//		ラグラージナイト					
      753,//		ジュカインナイト					
      754,//		ヤミラミナイト					
      755,//		チルタリスナイト					
      756,//		エルレイドナイト					
      757,//		タブンネナイト					
      758,//		メタグロスナイト					
      759,//		サメハダナイト					
      760,//		ヤドランナイト					
      761,//		ハガネールナイト					
      762,//		ピジョットナイト					
      763,//		オニゴーリナイト					
      764,//		ディアンシナイト					
      767,//		バクーダナイト					
      768,//		ミミロップナイト					
      769,//		ボーマンダナイト					
      770//		スピアナイト	
    };
				
    for( u32 index = 0 ; index < GFL_NELEMS(VETO_RING_LIST) ; ++index )
    {
      u32 no = VETO_RING_LIST[index];

      int buffno = no / 8;
      int bitno = no % 8;

      m_pCoreData->veto_item_bit[buffno] = m_pCoreData->veto_item_bit[buffno] | (0x1<<bitno);
    }
  }

  if( pSimpleData->vetoBeads != 0 )
  {
    // ピース所持チェック
    u32 max;
    const u16 * tbl = item::ITEM_GetPieceTable( &max );

    for( u32 index = 0 ; index < max ; ++index )
    {
      u32 no = tbl[index];

      int buffno = no / 8;
      int bitno = no % 8;

      m_pCoreData->veto_item_bit[buffno] = m_pCoreData->veto_item_bit[buffno] | (0x1<<bitno);
    }
  }

  m_pCoreData->crc = GetCrc();
}

//----------------------------------------------------------
/**
 * @breif       圧縮レギュレーションのバッファをセット
 * @param[in]   pBuffer   レギュレーションデータ
 * @note        内部でデータをコピーします。
 */
//----------------------------------------------------------
bool Regulation::SetSimpleData( const void* pBuffer , u64 nexUniqueID )
{
  const SimpleData* pSimpleData = reinterpret_cast<const SimpleData*>(pBuffer);
  GFL_PRINT("Regulation::SetSimpleData : SimpleDataSize[%d]\n",sizeof(SimpleData));
  if( pSimpleData->nexUniqueID != nexUniqueID )
  {//違うので読み込めない
    
    GFL_PRINT("Regulation::SetSimpleData : NUID error[%llu][%llu]\n",pSimpleData->nexUniqueID,nexUniqueID);
    return false;
  }

  SetSimpleData( pBuffer );

  return true;
} 
  
//----------------------------------------------------------
/**
 * @breif       amxデータを取得
 * @return      amxデータ
 */
//----------------------------------------------------------
void* Regulation::GetAmxData() const
{
  if( m_pRegulationData != NULL )
  {
    // スクリプト用データ
    u32 amx_size = m_pCoreData->amx_size; 
    u32 amx_offset = m_pCoreData->amx_offset; 

    if( amx_size > 0 )
    {
      u8* p = reinterpret_cast<u8*>(m_pRegulationData);
      return ( p + amx_offset );
    }
  }
  return NULL;
}

//----------------------------------------------------------
/**
 * @breif       amxデータのサイズを取得
 * @return      amxデータのサイズ
 */
//----------------------------------------------------------
u32 Regulation::GetAmxDataSize() const
{
  if( m_pRegulationData != NULL )
  {
    return m_pCoreData->amx_size;
  }
  return 0;
}

//----------------------------------------------------------
/**
 * @breif       データが読み込まれているか
 * @return      読み込まれているなら true
 */
//----------------------------------------------------------
bool Regulation::IsExistsData() const
{
  return m_pRegulationData != NULL;
}

//----------------------------------------------------------
/**
 * @brief 数値のパラメーターを取得する
 * @param id    ValueParamId enum のどれか
 * @return パラメータ値
 */
//----------------------------------------------------------
u32 Regulation::GetValueParam( Regulation::ValueParamId id ) const
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return 0;
  }

  u32 ret = 0;
  switch( id ){
  //==============================
  //基本レギュレーションデータ
  case TEAM_NUM_LO:         ret = static_cast<u32>(m_pCoreData->team_num_lo);           break;
  case TEAM_NUM_HI:         ret = static_cast<u32>(m_pCoreData->team_num_hi);           break;
  case SELECT_NUM_LO:       ret = static_cast<u32>(m_pCoreData->select_num_lo);         break;
  case SELECT_NUM_HI:       ret = static_cast<u32>(m_pCoreData->select_num_hi);         break;
  case LEGEND_NUM_HI:       ret = static_cast<u32>(m_pCoreData->legend_num_hi);         break;
  case LEVEL:               ret = static_cast<u32>(m_pCoreData->level);                 break;
  case TIME_VS:             ret = static_cast<u32>(m_pCoreData->time_vs);               break;
  case TIME_COMMAND:        ret = static_cast<u32>(m_pCoreData->time_command);          break;
  case SHOW_POKE_TIME:      ret = static_cast<u32>(m_pCoreData->show_poke_time);        break;
  case DISCRIMINANT_ID:     ret = static_cast<u32>(m_pCoreData->discriminant_id);             break;
  //==============================
  // 特殊データ
  case URL_DATA:            ret = static_cast<u32>(m_pCoreData->url_data);              break;
  case BTL_VIDEO:           ret = static_cast<u32>(m_pCoreData->btl_video);             break;
  //==============================
  // 大会専用のデータ
  case BATTLE_NUM:          ret = static_cast<u32>(m_pCoreData->battle_num);            break;
  case BATTLE_NUM_LIMIT:    ret = static_cast<u32>(m_pCoreData->battle_num_limit);      break;
  case START_ENABLE_TIME:   ret = static_cast<u32>(m_pCoreData->start_enable_time);     break;
  case END_ENABLE_TIME:     ret = static_cast<u32>(m_pCoreData->end_enable_time);       break;
  case START_YEAR:          ret = static_cast<u32>(m_pCoreData->start_year);            break;
  case START_MONTH:         ret = static_cast<u32>(m_pCoreData->start_month);           break;
  case START_DAY:           ret = static_cast<u32>(m_pCoreData->start_day);             break;
  case END_YEAR:            ret = static_cast<u32>(m_pCoreData->end_year);              break;
  case END_MONTH:           ret = static_cast<u32>(m_pCoreData->end_month);             break;
  case END_DAY:             ret = static_cast<u32>(m_pCoreData->end_day);               break;
  default: GFL_ASSERT(0); break;
  }

  return ret;
}

//----------------------------------------------------------
/**
 * @brief フラグのパラメーターを取得する
 * @param id    BoolParamId enum のどれか
 * @return パラメータ値
 */
//----------------------------------------------------------
bool Regulation::GetBoolParam( BoolParamId id ) const
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return 0;
  }

  bool ret = false;
  switch( id ){
  case REVERSE:         ret = (m_pCoreData->reverse == 0)   ? false : true;      break;
  case BOTH_POKE:       ret = (m_pCoreData->both_poke == 0) ? false : true;      break;
  case BOTH_ITEM:       ret = (m_pCoreData->both_item == 0) ? false : true;      break;
  case HURAETTE_HAKAI:  ret = (m_pCoreData->huraette_hakai == 0) ? false : true; break;
  case QR_TEAM:         ret = (m_pCoreData->qr_team == 0)   ? false : true;      break;
  case NICKNAME:        ret = (m_pCoreData->nickname == 0)  ? false : true;      break;
  case SHOW_POKE:       ret = (m_pCoreData->show_poke == 0) ? false : true;      break;
  case WATCH:           ret = (m_pCoreData->watch == 0)     ? false : true;      break;
  default: GFL_ASSERT(0); break;
  }

  return ret;
}

//----------------------------------------------------------
/**
 * @brief テキストのパラメータを取得する
 * @param id  TextParamId enum のどれか
 * @param pReturnCupName  テキストを入れるSTRBUFポインタ
 * @return  none
 */
//----------------------------------------------------------
void Regulation::GetTextParam( TextParamId id, gfl2::str::StrBuf* pReturnText ) const
{
  if( m_pCoreData == NULL || pReturnText == NULL )
  {
    GFL_ASSERT(0);
    pReturnText = NULL;
    return;
  }

  switch( id ){
  case DELIVERY_NAME: pReturnText->SetStr( m_pCoreData->delivery_name );  break;
  case DELIVERY_TEXT: pReturnText->SetStr( m_pCoreData->delivery_text );  break;
  case CUPNAME1:      pReturnText->SetStr( m_pCoreData->cupname1 );       break;
  case CUPNAME2:      pReturnText->SetStr( m_pCoreData->cupname2 );       break;
  default: GFL_ASSERT(0); break;
  }
}

//----------------------------------------------------------
/**
 * @brief テキストのパラメータを取得する（STRBUFを生成）
 * @param id  TextParamId enum のどれか
 * @return  gfl::str::StrBuf  テキストを格納したSTRBUFへのポインタ
 */
//----------------------------------------------------------
gfl2::str::StrBuf* Regulation::GetTextParam( TextParamId id ) const
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return NULL;
  }

  // 文字数
  u32 numCharMax = 0; 
  switch( id ){
  case DELIVERY_NAME: numCharMax = DELIVERY_NAME_MOJINUM + System::EOM_LEN; break;
  case DELIVERY_TEXT: numCharMax = DELIVERY_TEXT_MOJINUM + System::EOM_LEN; break;
  case CUPNAME1:      numCharMax = WIFI_PLAYER_TIX_CUPNAME_MOJINUM + System::EOM_LEN; break;
  case CUPNAME2:      numCharMax = WIFI_PLAYER_TIX_CUPNAME_MOJINUM + System::EOM_LEN; break;
  default: GFL_ASSERT(0); break;
  }

  gfl2::str::StrBuf* tmpBuf = GFL_NEW( m_pHeap ) gfl2::str::StrBuf( numCharMax, m_pHeap->GetLowerHandle() );

  this->GetTextParam( id, tmpBuf );

  return tmpBuf;
}

//! バトル場所取得
Regulation::BATTLE_CATEGORY Regulation::GetBattleCategory() const
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return BATTLE_CATEGORY_NET_BTL;
  }

  return static_cast<BATTLE_CATEGORY>(m_pCoreData->category);
}
//! バトルルール取得
Regulation::BATTLE_RULE Regulation::GetBattleRule() const
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return BATTLE_RULE_SINGLE;
  }

  return static_cast<BATTLE_RULE>(m_pCoreData->btl_rule);
}
//! レベル補正条件取得
Regulation::LEVEL_RANGE_TYPE Regulation::GetLevelRangeType() const
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return LEVEL_RANGE_NORMAL;
  }

  return static_cast<LEVEL_RANGE_TYPE>(m_pCoreData->level_range);
}
//! 制限時間タイプ取得
Regulation::TIME_LIMIT_TYPE Regulation::GetTimeLimitType() const
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return TIME_LIMIT_TOTAL;
  }

  return static_cast<TIME_LIMIT_TYPE>(m_pCoreData->time_limit_type);
}
//! バトル背景取得
Regulation::BACKGROUND_TYPE Regulation::GetBackGroundType() const
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return BACKGROUND_NORMAL1;
  }

  return static_cast<BACKGROUND_TYPE>(m_pCoreData->background);
}


//! バージョン取得
u8 Regulation::GetVersion() const
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return 0;
  }

  return m_pCoreData->regulation_version;
}

//----------------------------------------------------------
/**
 * @brief パラメータのBitが建っているかどうかを検査
 * @param id    BitParamId enum のどれか
 * @param no    ポケモン番号かアイテム番号か技番号
 * @return BitがOnでtrue
 */
//----------------------------------------------------------
bool Regulation::CheckParamBit( BitParamId id, u32 no ) const
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return false;
  }

  int buffno = no / 8;
  int bitno = no % 8;

  switch( id ){
  case VETO_POKE_BIT:              //! 禁止ポケモン 128byte
    if(m_pCoreData->veto_poke_bit[buffno] & (0x1<<bitno)){
      ICHI_PRINT("禁止ポケモンだった %d %d %d \n", no, buffno, bitno );
      return true;
    }
    break;
  case VETO_WAZA_BIT:              //! 禁止わざ 128byte
    if(m_pCoreData->veto_waza_bit[buffno] & (0x1<<bitno)){
      ICHI_PRINT("禁止ワザだった %d %d %d \n", no, buffno, bitno );
      return true;
    }
    break;
  case VETO_ITEM_BIT:              //! 禁止道具 128byte
    if(m_pCoreData->veto_item_bit[buffno] & (0x1<<bitno)){
      ICHI_PRINT("禁止アイテムだった %d %d %d \n", no, buffno, bitno );
      return true;
    }
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
  return false;
}

//----------------------------------------------------------
/**
 * @brief 参加ロムバージョンチェック
 * @param version   チェックするバージョン
 * @return true:参加OK  false:参加NG
 */
//----------------------------------------------------------
bool Regulation::CheckParamRom( int version ) const
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return false;
  }

  // VERSION_SUNを基準にする
  version -= VERSION_SUN;

  int buffno = version / 8;
  int bitno = version % 8;

  GFL_PRINT("参加ROM: %d\n", version );

  // ビットが立っていると参加OK
  if(m_pCoreData->rom_bit[buffno] & (0x1<<bitno)){
    return true;
  }

  ICHI_PRINT("禁止ROMだった %d %d %d \n", version, buffno, bitno );
  return false;
}

//----------------------------------------------------------
/**
 * @brief 参加言語バージョンチェック
 * @param lang      言語
 * @return true:参加OK  false:参加NG
 */
//----------------------------------------------------------
bool Regulation::CheckParamLanguage( int lang ) const
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return false;
  }

  int buffno = lang / 8;
  int bitno = lang % 8;

  GFL_PRINT("参加言語 %d\n", lang );

  if(m_pCoreData->language_bit[buffno] & (0x1<<bitno)){
    return true;
  }

  ICHI_PRINT("禁止言語だった %d %d %d \n", lang, buffno, bitno );
  return false;
}

/**
 * @brief 必至ポケモンが指定されているか
 * @return trueで指定されている
 */
bool Regulation::IsMustPoke() const
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return false;
  }
  // 先頭に0だと設定なし
  if( m_pCoreData->must_poke_and[0] == 0 && m_pCoreData->must_poke_or[0] == 0 )
  {
    return false;
  }
  return true;
}

/**
 * @brief AND条件必至ポケモン番号取得
 * @param idx   添字
 * @return 必至ポケモン番号
 */
u16 Regulation::GetParamMustPokeAND( u8 idx ) const
{
  if( idx >= MUST_POKE_NUM )
  {
    return 0;
  }
  return m_pCoreData->must_poke_and[idx];
}
u8  Regulation::GetParamMustPokeFormAND( u8 idx ) const
{
  if( idx >= MUST_POKE_NUM )
  {
    return 0;
  }
  return m_pCoreData->must_poke_form_and[idx];
}

/**
 * @brief OR条件必至ポケモン番号取得
 * @param idx   添字
 * @return 必至ポケモン番号
 */
u16 Regulation::GetParamMustPokeOR( u8 idx ) const
{
  if( idx >= MUST_POKE_NUM )
  {
    return 0;
  }
  return m_pCoreData->must_poke_or[idx];
}
u8  Regulation::GetParamMustPokeFormOR( u8 idx ) const
{
  if( idx >= MUST_POKE_NUM )
  {
    return 0;
  }
  return m_pCoreData->must_poke_form_or[idx];
}

/**
 * @brief 禁止特定フォルムポケモンが指定されているか
 * @return trueで指定されている
 */
bool Regulation::IsSpecificVetoPoke() const
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return false;
  }
  // 先頭に0だと設定なし
  return ( m_pCoreData->specific_veto_poke[0] != 0 );
}

/**
 * @brief 禁止特定フォルムポケモンチェック
 * @param monsno   チェックするポケモンのモンスター番号
 * @param formno   チェックするポケモンのフォルム番号
 * @return true:含まれている  false:含まれていない
 */
bool Regulation::CheckParamSpecifcVetoPoke( u16 monsno, u8 formno ) const
{
  // 禁止特定フォルムポケモンが指定されていない場合は、OKとする
  if( IsSpecificVetoPoke() == false )
  {
    return false;
  }

  for(int i=0; i<SPECIFIC_VETO_POKE_NUM; i++){
    if( monsno == m_pCoreData->specific_veto_poke[i] &&
        formno == m_pCoreData->specific_veto_poke_form[i] )
    {
      return true;
    }
  }
  return false;
}


//----------------------------------------------------------
/**
 * @brief 産地限定チェック
 * @param version   チェックするバージョン
 * @return true:参加OK  false:参加NG
 */
//----------------------------------------------------------
bool Regulation::CheckParamProducingArea( int version ) const
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return false;
  }

  // データがある場合のみチェック（先頭に0だと設定なし）
  if( m_pCoreData->producing_area[0] == 0 )
  {// 無条件で参加OK
    return true;
  }

  // 最高8つまで指定がある
  for( u8 i=0; i<PRODUCING_AREA_MAX_BYTE; i++ )
  {
    // バージョン一致チェック
    if( version == m_pCoreData->producing_area[i] )
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  レギュレーションカードのCRCチェック (開催ワーク statusに値が書かれているとチェックできません)
 *  @return trueで正常  falseで不正
 */
//-----------------------------------------------------------------------------
bool Regulation::CheckCrc() const
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return false;
  }

#if PM_DEBUG
  if( m_debugCrcBroken )
  {
    GFL_RELEASE_PRINT("Regulation:ForceCrcBroken!!!\n");
    return false;
  }
#endif

  ICHI_PRINT( "CRCチェック buffer[0x%x]== calc[0x%x]\n", m_pCoreData->crc, GetCrc() );
  return m_pCoreData->crc == GetCrc();
}

//----------------------------------------------------------------------------
/**
 *  @brief  CRCを返す
 *
 *  @param  pReg  REGULATIONワークポインタ
 *
 *  @return CRC
 */
//-----------------------------------------------------------------------------
u32 Regulation::GetCrc() const
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return 0;
  }
  // crc の2バイト+余り2バイトをマイナス
  return gfl2::math::Crc::Crc16( 0, reinterpret_cast<const u8*>(m_pCoreData), sizeof(CoreData) - 4 );
}

//----------------------------------------------------------
/**
 * @breif       レーティングバトルかどうか
 * @return      レーティングバトルならtrue
 */
//----------------------------------------------------------
bool Regulation::IsRateBattle() const
{
  switch( GetBattleCategory() ){
  case BATTLE_CATEGORY_SPOT_RATE:            //! ランダムマッチ[レーティング]
  case BATTLE_CATEGORY_SPOT_SP:              //! ランダムマッチ[スペシャル]
  case BATTLE_CATEGORY_NET_CUP:              //! インターネット大会
  case BATTLE_CATEGORY_WCS:                  //! ランダムマッチ[レーティング]WCSルール
  case BATTLE_CATEGORY_GROUP_CUP:            //! なかま大会
    return true;
  }

  return false;
}

//----------------------------------------------------------
/**
 * @breif       WCSかどうか
 * @return      WCSならtrue
 */
//----------------------------------------------------------
bool Regulation::IsWcs() const
{
  BACKGROUND_TYPE bg = GetBackGroundType();

  if( bg == BACKGROUND_WCS1 || 
      bg == BACKGROUND_WCS2 )
  {
    return true;
  }
  return false;
}


//----------------------------------------------------------
/**
 * @breif       特別なポケモンが参加できるか
 * @retval      ture：さんかできる
 * @retval      false：さんかできない
 *
 * @note        NMCat[3141]：とくべつなさんかポケモンの表示用関数
 */
//----------------------------------------------------------
bool Regulation::IsLegendEntryView()
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return true;
  }

  // 禁止ポケモンに設定されているものが、伝説・幻テーブルと完全一致するなら、
  // 特別なポケモンは参加出来ない。
  // 1つでも不一致ならば参加出来る。

  // ポケモン全部回す
  for( u32 monsNo=0; monsNo<MONSNO_END; monsNo++ )
  {
    // 伝説・幻ポケモンかつ禁止ポケモンになっているか
    if( PokeRegulation::CheckLegend( monsNo, 0 ) )
    {
      if( CheckParamBit( VETO_POKE_BIT, monsNo ) == false )
      {
        GFL_PRINT(">Regulation MonsNo[%d]が禁止ポケモンでない\n", monsNo );
        return true;
      }
    }
  }

  // 破滅の花フラエッテ、サトシピカチュウ、サトシゲッコウガは、
  // 配布ポケモンだがバトルに出せるポケモンとの事で、禁止扱いではない。
  

  return false;
}

//----------------------------------------------------------
/**
 * @breif       レギュレーションデータのメモリ確保
 * @param[in]   size      レギュレーションデータのサイズ
 */
//----------------------------------------------------------
void Regulation::AllocWork( const u32 size )
{
  GflHeapSafeFreeMemory(m_pRegulationData);
  m_regulationDataSize = size;
  m_pRegulationData = GflHeapAllocMemory( m_pHeap, size );
  gfl2::std::MemClear( m_pRegulationData, size );

  ICHI_PRINT("Regulation::AllocWork size=%d\n", size);
}


#if PM_DEBUG
//----------------------------------------------------------------------------
/**
 *  @brief  デバッグプリント
 */
//-----------------------------------------------------------------------------
void Regulation::DebugPrint()
{
  if( m_pCoreData == NULL )
  {
    return;
  }

  CoreData* pReg = m_pCoreData;

  //基本レギュレーションデータ(476byte)
  GFL_RELEASE_PRINT( "=====================================\n" );
  GFL_RELEASE_PRINT( "      レギュレーションDump\n" );
  GFL_RELEASE_PRINT( "=====================================\n" );
  GFL_RELEASE_PRINT( "reg size =%d\n", m_regulationDataSize );
  GFL_RELEASE_PRINT( "category =%d\n", pReg->category);     ///< カテゴリID
  GFL_RELEASE_PRINT( "btl_rule =%d\n", pReg->btl_rule);     ///< ルール番号
  GFL_RELEASE_PRINT( "team_num_lo   =%d\n", pReg->team_num_lo); ///<    #参加数下限
  GFL_RELEASE_PRINT( "team_num_hi   =%d\n", pReg->team_num_hi); ///<    #参加数上限
  GFL_RELEASE_PRINT( "select_num_lo   =%d\n", pReg->select_num_lo); ///<    #参加数下限
  GFL_RELEASE_PRINT( "select_num_hi   =%d\n", pReg->select_num_hi); ///<    #参加数上限
  GFL_RELEASE_PRINT( "legend_num_hi=%d\n", pReg->legend_num_hi);  ///< 伝説参加上限
  GFL_RELEASE_PRINT( "level    =%d\n", pReg->level);  ///<    #参加レベル
  GFL_RELEASE_PRINT( "lv_range =%d\n", pReg->level_range); ///<    #レベル範囲
  GFL_RELEASE_PRINT( "reverse = %d\n", pReg->reverse);        ///< 逆さバトルフラグ
  GFL_RELEASE_PRINT( "both_poke=%d\n", pReg->both_poke); ///<    #同じポケモン
  GFL_RELEASE_PRINT( "both_item=%d\n", pReg->both_item);  ///<    #同じどうぐ             60
  for(int i=0; i<MUST_POKE_NUM; i++){
    GFL_RELEASE_PRINT( "must_poke_and[%d]=%d\n", i, pReg->must_poke_and[i]);   ///<    #必須ポケモン
    GFL_RELEASE_PRINT( "must_form_and[%d]=%d\n", i, pReg->must_poke_form_and[i]);   ///<    #必須ポケモンフォルム
  }
  for(int i=0; i<MUST_POKE_NUM; i++){
    GFL_RELEASE_PRINT( "must_poke_or[%d]=%d\n", i, pReg->must_poke_or[i]);   ///<    #必須ポケモン
    GFL_RELEASE_PRINT( "must_form_or[%d]=%d\n", i, pReg->must_poke_form_or[i]);   ///<    #必須ポケモンフォルム
  }
  for(int i=0; i<SPECIFIC_VETO_POKE_NUM; i++){
    GFL_RELEASE_PRINT( "specific_veto_poke[%d]=%d\n", i, pReg->specific_veto_poke[i]);
    GFL_RELEASE_PRINT( "specific_veto_form[%d]=%d\n", i, pReg->specific_veto_poke_form[i]);
  }
  for(int i=0; i<REG_POKENUM_MAX_BYTE; i++){
    GFL_RELEASE_PRINT( "veto_poke[%d]=0x%x\n", i, pReg->veto_poke_bit[i]);  ///<    #参加禁止ポケモン 142
  }
  for(int i=0; i<REG_WAZANUM_MAX_BYTE; i++){
    GFL_RELEASE_PRINT( "veto_waza[%d]=0x%x\n", i,pReg->veto_waza_bit[i]);   ///< 禁止わざ     267
  }
  for(int i=0; i<REG_ITEMNUM_MAX_BYTE; i++){
    GFL_RELEASE_PRINT( "veto_item[%d]=0x%x\n", i, pReg->veto_item_bit[i]); ///<    #持ち込み禁止道具 218
  }
  GFL_RELEASE_PRINT( "huraette_hakai  =%d\n", pReg->huraette_hakai);
  for(int i=0; i<PRODUCING_AREA_MAX_BYTE; i++){
    GFL_RELEASE_PRINT( "producing_area =%d\n", pReg->producing_area[i]);///< 産地限定     416
  }
  GFL_RELEASE_PRINT( "qr_team  =%d\n", pReg->qr_team);
  GFL_RELEASE_PRINT( "time_limit_type =%d\n", pReg->time_limit_type);     ///<    #対戦時間
  GFL_RELEASE_PRINT( "time_vs  =%d\n", pReg->time_vs);     ///<    #対戦時間
  GFL_RELEASE_PRINT( "time_cmd =%d\n", pReg->time_command); ///<    #入力時間
  GFL_RELEASE_PRINT( "nick_view=%d\n", pReg->nickname); ///<    #ニックネーム表示
  GFL_RELEASE_PRINT( "background=%d\n", pReg->background);  ///< #背景指定
  GFL_RELEASE_PRINT( "show_poke=%d\n", pReg->show_poke);  ///<    #ポケモン見せ合い
  GFL_RELEASE_PRINT( "show_time=%d\n", pReg->show_poke_time);  ///<ポケモン見せ合い時間
  for(int i=0; i<ROM_MAX_BYTE; i++){
    GFL_RELEASE_PRINT( "rom_bit[%d]  =0x%x\n", i, pReg->rom_bit[ i ]);  ///< 参加ロムバージョン
  }
  for(int i=0; i<LANGUAGE_MAX_BYTE; i++){
    GFL_RELEASE_PRINT( "language_bit[%d] = 0x%x\n", i, pReg->language_bit[ i ]);  ///< 参加言語バージョン   430
  }
  GFL_RELEASE_PRINT( "discriminant_id=%d\n", pReg->discriminant_id);

  // 特殊データ(480byte)
  gfl2::str::PrintCode( pReg->delivery_name, "delivery_name:");
  gfl2::str::PrintCode( pReg->delivery_text, "delivery_text:");
  GFL_RELEASE_PRINT( "url_data=%d\n", pReg->url_data);
  GFL_RELEASE_PRINT( "btl_video=%d\n", pReg->btl_video);    ///< バトルビデオ送信条件
  GFL_RELEASE_PRINT( "watch=%d\n", pReg->watch);

  gfl2::str::PrintCode( pReg->cupname1, "cupname1:");
  gfl2::str::PrintCode( pReg->cupname2, "cupname2:");
  GFL_RELEASE_PRINT( "battle_num = %d\n", pReg->battle_num);    ///< 対戦回数
  GFL_RELEASE_PRINT( "battle_num_limit =%d\n",  pReg->battle_num_limit);  ///< 1日の対戦回数上限
  GFL_RELEASE_PRINT( "start y  =%d\n", pReg->start_year);//開始年：00-99
  GFL_RELEASE_PRINT( "start m  =%d\n", pReg->start_month);//開始月：01-12
  GFL_RELEASE_PRINT( "start d  =%d\n", pReg->start_day);//開始日：01-31
  GFL_RELEASE_PRINT( "end   y  =%d\n", pReg->end_year);//終了年：00-99
  GFL_RELEASE_PRINT( "end   m  =%d\n", pReg->end_month);//終了月：01-12
  GFL_RELEASE_PRINT( "end   d  =%d\n", pReg->end_day);//終了日：01-31

  GFL_RELEASE_PRINT( "start_enable_time = %d\n" , pReg->start_enable_time );
  GFL_RELEASE_PRINT( "end_enable_time = %d\n" , pReg->end_enable_time );

  GFL_RELEASE_PRINT( "amx_size =%d\n", pReg->amx_size);
  GFL_RELEASE_PRINT( "amx_offset =%d\n", pReg->amx_offset);
  GFL_RELEASE_PRINT( "crc      =%d\n", pReg->crc);  //整合性検査
}

/**
 * @brief 数値のパラメーターを設定する
 * @param id    ValueParamId enum のどれか
 * @param value パラメータ値
   */
void Regulation::DebugSetValueParam( ValueParamId id, u32 value )
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return ;
  }

  switch( id ){
  //==============================
  //基本レギュレーションデータ
  case TEAM_NUM_LO:         m_pCoreData->team_num_lo      = static_cast<u8>(value);   break;
  case TEAM_NUM_HI:         m_pCoreData->team_num_hi      = static_cast<u8>(value);   break;
  case SELECT_NUM_LO:       m_pCoreData->select_num_lo    = static_cast<u8>(value);   break;
  case SELECT_NUM_HI:       m_pCoreData->select_num_hi    = static_cast<u8>(value);   break;
  case LEGEND_NUM_HI:       m_pCoreData->legend_num_hi    = static_cast<u8>(value);   break;
  case LEVEL:               m_pCoreData->level            = static_cast<u8>(value);   break;
  case TIME_VS:             m_pCoreData->time_vs          = static_cast<u8>(value);   break;
  case TIME_COMMAND:        m_pCoreData->time_command     = static_cast<u8>(value);   break;
  case SHOW_POKE_TIME:      m_pCoreData->show_poke_time   = static_cast<u8>(value);   break;
  case DISCRIMINANT_ID:     m_pCoreData->discriminant_id  = static_cast<u32>(value);  break;
  //==============================
  // 特殊データ
  case URL_DATA:            m_pCoreData->url_data         = static_cast<u32>(value);  break;
  case BTL_VIDEO:           m_pCoreData->btl_video        = static_cast<u16>(value);  break;
  //==============================
  // 大会専用のデータ
  case BATTLE_NUM:          m_pCoreData->battle_num       = static_cast<u8>(value);   break;
  case BATTLE_NUM_LIMIT:    m_pCoreData->battle_num_limit = static_cast<u8>(value);   break;
  case START_ENABLE_TIME:   m_pCoreData->start_enable_time= static_cast<u8>(value);   break;
  case END_ENABLE_TIME:     m_pCoreData->end_enable_time  = static_cast<u8>(value);   break;
  case START_YEAR:          m_pCoreData->start_year       = static_cast<u16>(value);  break;
  case START_MONTH:         m_pCoreData->start_month      = static_cast<u8>(value);   break;
  case START_DAY:           m_pCoreData->start_day        = static_cast<u8>(value);   break;
  case END_YEAR:            m_pCoreData->end_year         = static_cast<u16>(value);  break;
  case END_MONTH:           m_pCoreData->end_month        = static_cast<u8>(value);   break;
  case END_DAY:             m_pCoreData->end_day          = static_cast<u8>(value);   break;
  default: GFL_ASSERT(0); break;
  }
}


/**
 * @brief フラグのパラメーターを設定する
 * @param id    BoolParamId enum のどれか
 * @param flag  フラグ
 */
void Regulation::DebugSetBoolParam( BoolParamId id, bool flag )
{
  if( m_pCoreData == NULL )
  {
    GFL_ASSERT(0);
    return;
  }

  switch( id ){
  case REVERSE:         m_pCoreData->reverse = flag;   break;
  case BOTH_POKE:       m_pCoreData->both_poke = flag; break;
  case BOTH_ITEM:       m_pCoreData->both_item = flag; break;
  case HURAETTE_HAKAI:  m_pCoreData->huraette_hakai = flag; break;
  case QR_TEAM:         m_pCoreData->qr_team = flag;   break;
  case NICKNAME:        m_pCoreData->nickname = flag;  break;
  case SHOW_POKE:       m_pCoreData->show_poke = flag; break;
  case WATCH:           m_pCoreData->watch = flag;     break;
  default: GFL_ASSERT(0); break;
  }
}

/**
 * @brief AND条件必至ポケモン番号設定
 * @param idx   添字
 * @param pokeNo 必至ポケモン番号
 * @param pokeForm 必至ポケモンフォルム
 */
void Regulation::DebugSetParamMustPokeAND( u8 idx, u16 pokeNo, u8 pokeForm )
{
  if( idx >= MUST_POKE_NUM )
  {
    return;
  }
  m_pCoreData->must_poke_and[idx] = pokeNo;
  m_pCoreData->must_poke_form_and[idx] = pokeForm;
}

/**
 * @brief OR条件必至ポケモン番号設定
 * @param idx   添字
 * @param pokeNo 必至ポケモン番号
 * @param pokeForm 必至ポケモンフォルム
 */
void Regulation::DebugSetParamMustPokeOR( u8 idx, u16 pokeNo, u8 pokeForm )
{
  if( idx >= MUST_POKE_NUM )
  {
    return;
  }
  m_pCoreData->must_poke_or[idx] = pokeNo;
  m_pCoreData->must_poke_form_or[idx] = pokeForm;
}

#endif //PM_DEBUG


//GFL_NAMESPACE_END(Battle)
