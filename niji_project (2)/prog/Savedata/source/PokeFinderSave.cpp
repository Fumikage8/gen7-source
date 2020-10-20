//======================================================================
/**
 * @file PokeFinderSave.cpp
 * @date 2015/12/01 12:23:23
 * @author miyachi_soichi
 * @brief ポケファインダーデータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// gfl2
#include <util/include/gfl2_std_string.h>
// self
#include "Savedata/include/PokeFinderSave.h"

GFL_NAMESPACE_BEGIN(Savedata)

/**
 *  @class PokeFinderSave
 *  @brief  ポケファインダー用セーブデータ
 */
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// SaveDataInterface 実装
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  読込んだデータをセットする関数
 *  @param  pData   先頭のポインタ
 */
void PokeFinderSave::SetData( void* pData )
{
  gfl2::std::MemCopy( pData, reinterpret_cast<void*>(&(this->data)), sizeof(PokeFinderData) );
}

/**
 *  @brief  データの先頭ポインタを得る関数
 *  @return 先頭のポインタ
 */
void* PokeFinderSave::GetData( void )
{
  return reinterpret_cast<void*>( &(this->data) );
}

/**
 *  @brief  データサイズ
 *  @return バイト
 */
size_t PokeFinderSave::GetDataSize( void )
{
  return sizeof(PokeFinderData);
}

/**
 *  @brief  セーブデータのクリア
 *  @param  heap
 */
void PokeFinderSave::Clear( gfl2::heap::HeapBase* pHeap )
{
  this->InitData();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// 専用処理
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
//! データの初期化
void PokeFinderSave::InitData( void )
{
  // ファインダー全体で保存するデータ
  this->data.finder_level = 0;
  this->data.gyro_enable = POKE_FINDER_GYRO_ENABLE;
  this->data.snap_count = 0;
  this->data.value_count = 0;
  this->data.total_value = 0;
  this->data.high_value = 0;
  this->data.tutorial_flag = FINDER_TUTORIAL_CLEAR;
  this->data.reserve[0] = 0;
  this->data.reserve[1] = 0;
  this->data.reserve[2] = 0;
  this->data.reserve[3] = 0;
  this->data.reserve[4] = 0;
  // 各スポット毎で保存するデータ
  for( s32 i = 0; i < MAX_POKE_FINDER_SPOT; ++ i )
  {
    this->data.spot_data[i].flags       = FINDERSPOT_FLAG_CLEAR;
    for( s32 j = 0; j < 22; ++ j )
    {
      this->data.spot_data[i].reserve[j] = 0;
    }
  }
}
//! カメラアプリのバージョンを設定
void PokeFinderSave::SetCameraVersion( u16 ver )
{
  // 最大値を超えさせない
  if( MAX_POKE_FINDER_LEVEL < ver )
  {
    ver = MAX_POKE_FINDER_LEVEL;
  }
  this->data.finder_level = ver;
}
//! カメラアプリのバージョンを取得
u16 PokeFinderSave::GetCameraVersion( void )
{
  return this->data.finder_level;
}
//! ジャイロ状態切替
void PokeFinderSave::SetGyroFlag( bool flag )
{
  if( flag )
  {
    this->data.gyro_enable = POKE_FINDER_GYRO_ENABLE;
  }
  else
  {
    this->data.gyro_enable = POKE_FINDER_GYRO_DISABLE;
  }
}
//! ジャイロ状態取得
bool PokeFinderSave::GetGyroFlag( void )
{
  return this->data.gyro_enable == POKE_FINDER_GYRO_ENABLE;
}
//! 撮影回数の加算
void PokeFinderSave::AddSnapCount( u32 add )
{
  this->data.snap_count += add;
}
//! 撮影回数の取得
u32 PokeFinderSave::GetSnapCount( void )
{
  return this->data.snap_count;
}
//! 評価回数の加算
void PokeFinderSave::AddValueCount( u32 add )
{
  this->data.value_count += add;
}
//! 評価回数の取得
u32 PokeFinderSave::GetValueCount( void )
{
  return this->data.value_count;
}
//! 総評価得点の加算
void PokeFinderSave::AddTotalValue( u32 add )
{
  if( this->data.total_value + add <= MAX_POKE_FINDER_TOTALVALUE )
  {
    this->data.total_value += add;
  }
  else
  {
    this->data.total_value = MAX_POKE_FINDER_TOTALVALUE;
  }
  // 最高点の更新
  this->SetHighValue( add );
}
//! 総評価得点の取得
u32 PokeFinderSave::GetTotalValue( void )
{
  return this->data.total_value;
}
//! 最高評価点の設定
void PokeFinderSave::SetHighValue( u32 val )
{
  if( this->data.high_value < val )
  {
    this->data.high_value = val;
  }
}
//! 最高評価点の取得
u32 PokeFinderSave::GetHighValue( void )
{
  return this->data.high_value;
}
//! チュートリアル状況の設定
void PokeFinderSave::SetTutorialFlags( u16 flag, bool enable )
{
  if( enable )
  {
    this->data.tutorial_flag |= flag;
  }
  else
  {
    this->data.tutorial_flag &= ~(flag);
  }
}
//! チュートリアル状況の取得
u16 PokeFinderSave::GetTutorialFlags( void )
{
  return this->data.tutorial_flag;
}
//! 指定スポットの状態を操作
void PokeFinderSave::SetSpotFlags( u32 spot_id, u32 flag, bool enable )
{
  if( spot_id >= MAX_POKE_FINDER_SPOT ){ return; }
  if( enable )
  {
    this->data.spot_data[spot_id].flags |= flag;
  }
  else
  {
    this->data.spot_data[spot_id].flags &= ~(flag);
  }
}
//! 指定スポットの状態を取得
u32 PokeFinderSave::GetSpotFlags( u32 spot_id )
{
  if( spot_id >= MAX_POKE_FINDER_SPOT ){ return 0; }
  return this->data.spot_data[spot_id].flags;
}
GFL_NAMESPACE_END(Savedata)
