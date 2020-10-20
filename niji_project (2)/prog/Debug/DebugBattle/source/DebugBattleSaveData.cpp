#if PM_DEBUG
//=============================================================================
/**
 * @file    DebugBattleSaveData.cpp
 * @brief   デバッグバトルProc用セーブデータ
 * @author  obata_toshihiro
 * @data    2015.02.25
 * @note    sango_projectから移植
 */
//=============================================================================
#include <fs/include/gfl2_Fs.h>
#include <debug/include/gfl2_Assert.h>

#include <GameSys/include/GameManager.h>
#include <Battle/include/battle_DefaultPowerUp.h>
#include "./DebugBattleSaveData.h"

#include <niji_conv_header/battle/background/bgsys_near_type_def.h>
#include <niji_conv_header/battle/background/bgsys_far_type_def.h>


namespace
{
  // ポケモンデータのトータルサイズ
  static const u32 POKEDATASIZE = pml::pokepara::PokemonParam::DATASIZE * debug::battle::POKEPARAM_NUM;

  // デフォルト能力アップデータのトータルサイズ
  static const u32 DEFAULT_POWERUP_DESC_SIZE = sizeof( DefaultPowerUpDesc ) * debug::battle::POKEPARAM_NUM; 
}

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(battle)


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 *
 * @param pHeap  使用するヒープ
 */
 //----------------------------------------------------------------------------
DebugBattleSaveData::DebugBattleSaveData( gfl2::heap::HeapBase* pHeap ) : 
  m_heap( pHeap )
{
  m_pokeData           = (u8*)GflHeapAllocMemory( pHeap, POKEDATASIZE );
  m_param.stampCount     = 5;
  m_param.msgSpeed       = MSG_VERY_FAST;
  m_param.nearBackGround = btl::bg::NEAR_TYPE_GRASS;
  m_param.farBackGround  = btl::bg::FAR_TYPE_Z_F0102_S0101;
  m_param.bgm            = 21; // DebugBattleProcのBGM_TABLEのSTRM_BGM_MJ_M02に対応する
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
 //----------------------------------------------------------------------------
DebugBattleSaveData::~DebugBattleSaveData( )
{
  if( m_pokeData )
  {
    GflHeapFreeMemory( (void*)m_pokeData );
  }
}


//-----------------------------------------------------------------------------
/**
 * @brief ポケモンパラメータを取得する
 *
 * @param[out] pDst ポケモンパラメータの格納先
 * @param      idx  取得するポケモンパラメータのインデックス
 */
//-----------------------------------------------------------------------------
void DebugBattleSaveData::GetPokeParam( pml::pokepara::PokemonParam* pDst, u32 idx )
{
  if( idx < POKEPARAM_NUM )
  {
    u8* p = &m_pokeData[ idx * pml::pokepara::PokemonParam::DATASIZE ];
    pDst->Deserialize_Full( (const void*)p );
    if( pDst->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) ) {
      pDst->Clear();
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief ポケモンパラメータを設定する
 *
 * @param pSrc ポケモンパラメータ
 * @param idx  設定先インデックス
 */
//-----------------------------------------------------------------------------
void DebugBattleSaveData::SetPokeParam( const pml::pokepara::PokemonParam* pSrc, u32 idx )
{
  if( idx < POKEPARAM_NUM )
  {
    u8* p = &m_pokeData[ idx * pml::pokepara::PokemonParam::DATASIZE ];
    pSrc->Serialize_Full( (void*)p );
  }
}


void DebugBattleSaveData::GetDefaultPowerUpDesc( u32 index, DefaultPowerUpDesc* dest )
{
  DEFAULT_POWERUP_DESC_Copy( dest, m_defalutPowerUpDesc[ index ] );
}

void DebugBattleSaveData::SetDefaultPowerUpDesc( u32 index, const DefaultPowerUpDesc& desc )
{
  DEFAULT_POWERUP_DESC_Copy( &m_defalutPowerUpDesc[ index ], desc );
}

u8 DebugBattleSaveData::GetParam( DBItemID item )
{
  if( (item>=ITEM_BITFLAG_BEGIN) && (item<=ITEM_BITFLAG_END) )
  {
    u32 flagIdx = item - ITEM_BITFLAG_BEGIN;
    u32 byteIdx = (flagIdx / 8);
    u32 bitIdx  = (flagIdx % 8);
    u8 result = (m_param.bitFlags[ byteIdx ] & (1 << bitIdx)) != 0;
    return result;
  }

  switch( item ){
  case  ITEM_BTLMODE:              return m_param.btlMode;
  case  ITEM_MSGSPEED:             return m_param.msgSpeed;
  case  ITEM_CHANGERULE:           return m_param.changeRule;
  case  ITEM_STAMP_COUNT:          return m_param.stampCount;
  case  ITEM_COMMMODE:             return m_param.commMode;
  case  ITEM_EFFENABLE:            return m_param.bEffectEnable;
  case  ITEM_BTLHOUSE_ENABLE:      return m_param.bBtlHouseMode;
  case  ITEM_RECMODE:              return m_param.recMode;
  case  ITEM_RECIDX:               return m_param.recIdx;
  case  ITEM_TIME_GAME_M:          return m_param.time_game_minute;
  case  ITEM_TIME_GAME_S:          return m_param.time_game_sec;
  case  ITEM_TIME_CLIENT_M:        return m_param.time_client_minute;
  case  ITEM_TIME_CLIENT_S:        return m_param.time_client_sec;
  case  ITEM_TIME_COMMAND:         return m_param.time_command;
  case  ITEM_DEFAULT_WEATHER:      return m_param.weather;
  case  ITEM_BACKGRND_NEAR:        return m_param.nearBackGround;
  case  ITEM_BACKGRND_FAR:         return m_param.farBackGround;
  case  ITEM_BGM:                  return m_param.bgm;
  case  ITEM_REINFORCE_COND_1:     return m_param.reinforceCond[0];
  case  ITEM_REINFORCE_COND_2:     return m_param.reinforceCond[1];
  case  ITEM_REINFORCE_COND_3:     return m_param.reinforceCond[2];
  case  ITEM_REINFORCE_COND_4:     return m_param.reinforceCond[3];
  case  ITEM_REINFORCE_COUNT_1:    return m_param.reinforceCount[0];
  case  ITEM_REINFORCE_COUNT_2:    return m_param.reinforceCount[1];
  case  ITEM_REINFORCE_COUNT_3:    return m_param.reinforceCount[2];
  case  ITEM_REINFORCE_COUNT_4:    return m_param.reinforceCount[3];
  }
  return 0;
}

void DebugBattleSaveData::SetParam( DBItemID item, int value )
{
  if( (item>=ITEM_BITFLAG_BEGIN) && (item<=ITEM_BITFLAG_END) )
  {
    u32 flagIdx = item - ITEM_BITFLAG_BEGIN;
    u32 byteIdx = (flagIdx / 8);
    u32 bitIdx  = (flagIdx % 8);

    if( value ) {
      m_param.bitFlags[ byteIdx ] |= (1 << bitIdx);
    }
    else {
      u8 mask = ~(1 << bitIdx);
      m_param.bitFlags[ byteIdx ] &= mask;
    }
    return;
  }

  switch( item ) {
  case  ITEM_BTLMODE:              m_param.btlMode            = value; break;
  case  ITEM_MSGSPEED:             m_param.msgSpeed           = value; break;
  case  ITEM_CHANGERULE:           m_param.changeRule         = value; break;
  case  ITEM_STAMP_COUNT:          m_param.stampCount         = value; break;
  case  ITEM_COMMMODE:             m_param.commMode           = value; break;
  case  ITEM_EFFENABLE:            m_param.bEffectEnable      = value; break;
  case  ITEM_BTLHOUSE_ENABLE:      m_param.bBtlHouseMode      = value; break;
  case  ITEM_RECMODE:              m_param.recMode            = value; break;
  case  ITEM_RECIDX:               m_param.recIdx             = value; break;
  case  ITEM_TIME_GAME_M:          m_param.time_game_minute   = value; break;
  case  ITEM_TIME_GAME_S:          m_param.time_game_sec      = value; break;
  case  ITEM_TIME_CLIENT_M:        m_param.time_client_minute = value; break;
  case  ITEM_TIME_CLIENT_S:        m_param.time_client_sec    = value; break;
  case  ITEM_TIME_COMMAND:         m_param.time_command       = value; break;
  case  ITEM_DEFAULT_WEATHER:      m_param.weather            = value; break;
  case  ITEM_BACKGRND_NEAR:        m_param.nearBackGround     = value; break;
  case  ITEM_BACKGRND_FAR:         m_param.farBackGround      = value; break;
  case  ITEM_BGM:                  m_param.bgm                = value; break;
  case  ITEM_REINFORCE_COND_1:     m_param.reinforceCond[0]   = value; break;
  case  ITEM_REINFORCE_COND_2:     m_param.reinforceCond[1]   = value; break;
  case  ITEM_REINFORCE_COND_3:     m_param.reinforceCond[2]   = value; break;
  case  ITEM_REINFORCE_COND_4:     m_param.reinforceCond[3]   = value; break;
  case  ITEM_REINFORCE_COUNT_1:    m_param.reinforceCount[0]  = value; break;
  case  ITEM_REINFORCE_COUNT_2:    m_param.reinforceCount[1]  = value; break;
  case  ITEM_REINFORCE_COUNT_3:    m_param.reinforceCount[2]  = value; break;
  case  ITEM_REINFORCE_COUNT_4:    m_param.reinforceCount[3]  = value; break;
  }
}

/**
 * 録画データのセーブ
 */
bool  DebugBattleSaveData::SaveRecordData( u32 idx, const void* data, u32 dataSize )
{
  setupRecFilePath( m_datFilePath, idx );

  gfl2::fs::DirectSdmcFile* file = gfl2::fs::System::CreateDirectSdmcFile( m_heap, m_datFilePath, gfl2::fs::Define::OpenMode::WRITE );
  if( file == NULL ) {
    return false;
  }
  file->Write( data, dataSize );
  gfl2::fs::System::DestroyDirectSdmcFile( file );
  return true;
}

/**
 * 録画データのロード
 */
bool DebugBattleSaveData::LoadRecordData( u32 idx, void* dst, u32 dataSize )
{
  setupRecFilePath( m_datFilePath, idx );
  gfl2::fs::DirectSdmcFile* file = gfl2::fs::System::CreateDirectSdmcFile( m_heap, m_datFilePath, gfl2::fs::Define::OpenMode::READ );
  if( file == NULL ) {
    return false;
  }

  size_t readSize;
  file->Read( &readSize, dst, dataSize, dataSize );
  gfl2::fs::System::DestroyDirectSdmcFile( file );
  return true;
}



void DebugBattleSaveData::StartSave( u32 slotIdx )
{
  GFL_ASSERT(slotIdx < SLOT_MAX);
  setupDatFilePath( m_datFilePath, slotIdx );

  gfl2::fs::DirectSdmcFile* file = gfl2::fs::System::CreateDirectSdmcFile( m_heap, m_datFilePath, gfl2::fs::Define::OpenMode::WRITE );
  file->Write( static_cast<const void*>( m_pokeData ),           POKEDATASIZE );
  file->Write( static_cast<const void*>( m_defalutPowerUpDesc ), DEFAULT_POWERUP_DESC_SIZE );
  file->Write( static_cast<const void*>( &m_param ),             sizeof(m_param) );
  gfl2::fs::System::DestroyDirectSdmcFile( file );
}

bool DebugBattleSaveData::WaitSave( void )
{
  return true;
}

void DebugBattleSaveData::StartLoad( u32 slotIdx )
{
  GFL_ASSERT(slotIdx < SLOT_MAX);
  setupDatFilePath( m_datFilePath, slotIdx );
}

bool DebugBattleSaveData::WaitLoad( bool* bSucceed )
{
  gfl2::fs::DirectSdmcFile* file = gfl2::fs::System::CreateDirectSdmcFile( m_heap, m_datFilePath, gfl2::fs::Define::OpenMode::READ );
  if( file == NULL ) {
    *bSucceed = false;
    return true;
  }

  size_t readSize;
  file->Read( &readSize, static_cast<void*>( m_pokeData ),           POKEDATASIZE,              0,                                        POKEDATASIZE );
  file->Read( &readSize, static_cast<void*>( m_defalutPowerUpDesc ), DEFAULT_POWERUP_DESC_SIZE, POKEDATASIZE,                             DEFAULT_POWERUP_DESC_SIZE );
  file->Read( &readSize, static_cast<void*>( &m_param ),             sizeof(m_param),           POKEDATASIZE + DEFAULT_POWERUP_DESC_SIZE, sizeof(m_param) );
  gfl2::fs::System::DestroyDirectSdmcFile( file );

  *bSucceed = true;
  return true;
}


void DebugBattleSaveData::setupDatFilePath( char* dst, u32 slotIdx )
{
  const char  baseFilepath[] = "debug_btl_0.dat";
  setupFilePathCore( dst, slotIdx, baseFilepath );
}
void DebugBattleSaveData::setupRecFilePath( char* dst, u32 slotIdx )
{
  const char  baseFilepath[] = "debug_rec_0.dat";
  setupFilePathCore( dst, slotIdx, baseFilepath );
}

void DebugBattleSaveData::setupFilePathCore( char* dst, u32 idx, const char* baseFilePath )
{
  const char* p = baseFilePath;
  while( *p != L'\0' ){
    *dst = *p;
    if( *dst == L'0' ){
      (*dst) += idx;
    }
    ++dst;
    ++p;
  }
}


GFL_NAMESPACE_END(battle)
GFL_NAMESPACE_END(debug)

#endif // PM_DEBUG
