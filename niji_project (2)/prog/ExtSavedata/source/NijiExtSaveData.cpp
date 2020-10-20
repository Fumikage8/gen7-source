//==============================================================================
/**
 * @file    NijiExtSaveData.cpp
 * @brief   拡張セーブデータの定義
 * @author  tamada
 * @date    2012.09.25
 */
//==============================================================================
#include "ExtSavedata/include/NijiExtSaveData.h"
#include "ExtSavedata/include/RegulationExtSaveData.h"
#include "ExtSavedata/include/BattleRecorderSaveData.h"
#include "ExtSavedata/include/FinderStudioExtSaveData.h"
#include "Battle/Regulation/include/Regulation.h"

#include "NetStatic/NetLib/include/NetDefine.h"

#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN(ExtSavedata)



//-----------------------------------------------------------------------------
/// 未来バージョンのために確保する生成ファイル数の係数
//-----------------------------------------------------------------------------
static const int ReservedFileCountRate = 3;

//-----------------------------------------------------------------------------
/// 未来バージョンのために確保する生成ディレクトリ数の係数
//-----------------------------------------------------------------------------
static const int ReservedDirectoryCountRate = 3;

//-----------------------------------------------------------------------------
/**
 * @brief 拡張セーブデータ構成データ
 */
//-----------------------------------------------------------------------------
const ExtSaveDataFormat NijiExtSaveDataFormat[EXTID_NUM] = {
  { //バトルビデオ
    L"btvideo",
    101,
    sizeof(ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA),
  },
  { //ファインダースタジオ
    L"finder_studio",
    1,
    ExtSavedata::FinderStudioExtSaveData::DATA_FILE_SIZE,
  },
  { //レギュレーション
    L"reg",
    ExtSavedata::RegulationExtSaveData::REGULATION_TYPE_NUM,
    Regulation::REGULATION_MAX_DATA_SIZE,
  },
  { //PSSアイコン　友達
    L"dummy2",
    100,
    3200,
  },
  { //トライアルハウス
    L"dummy3",
    1,
    1,
  },
};

//-----------------------------------------------------------------------------
/// 拡張セーブデータの分類数
//-----------------------------------------------------------------------------
const int NijiExtSaveDataFormatCount = GFL_NELEMS(NijiExtSaveDataFormat);


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
nn::fs::ExtSaveDataId NijiExtSaveData::GetExtSaveDataId( void )
{
  return GameSys::GameManager::APLICATION_UNIQUE_ID;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int NijiExtSaveData::GetMaxDirectoryCount( void )
{
  GFL_ASSERT_STOP( EXTID_NUM == NijiExtSaveDataFormatCount );
  return NijiExtSaveDataFormatCount * ReservedDirectoryCountRate;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int NijiExtSaveData::GetMaxFileCount( void )
{
  int count = 0;
  for (int i = 0; i < NijiExtSaveDataFormatCount; ++ i )
  {
    count += NijiExtSaveDataFormat[i].max_file_num;
  }
  // +1 はBOSS通信用
  return count * ReservedFileCountRate + 1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int NijiExtSaveData::GetSaveDataFormatCount( void )
{
  return NijiExtSaveDataFormatCount;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const ExtSaveDataFormat * NijiExtSaveData::GetSaveDataFormat( int id )
{
  GFL_ASSERT_STOP( id < NijiExtSaveDataFormatCount );
  return &NijiExtSaveDataFormat[id];
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
size_t NijiExtSaveData::GetSaveFileSize( int id )
{
  const ExtSaveDataFormat * form = GetSaveDataFormat( id );
  return form->max_file_size;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int NijiExtSaveData::GetSaveFileMaxNum( int id )
{
  const ExtSaveDataFormat * form = GetSaveDataFormat( id );
  return form->max_file_num;
}



GFL_NAMESPACE_END(ExtSavedata)
