//======================================================================
/**
* @file DebugZoneNameLoader.cpp
* @date 2016/10/04 16:18:19
* @author ikuta_junya
* @brief デバッグ機能用、ゾーン名文字列をロードする機能
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if PM_DEBUG

// gfl
#include <util/include/gfl2_std_string.h>
#include <core/include/gfl2_Utility.h>

// niji
#include "Field/FieldStatic/include/Debug/DebugZoneNameLoader.h"

#include <arc_def_index/arc_def.h>
#include "GameSys/include/GameManager.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)


 ZoneNameLoader::ZoneNameLoader( void ) :
  m_pZoneNameBuf( NULL )
  ,m_ZoneNameDataSize( 0 )
{

}

ZoneNameLoader::~ZoneNameLoader()
{
  Delete();
}

/**
* @breaf データをロードする 
*
* @param pHeap ロード先を確保するためのヒープ
* @return 成否
*/
b32 ZoneNameLoader::Load( gfl2::heap::HeapBase* pHeap )
{
  // @todo ゾーン名をロード、デバッグマップジャンプからこぴぺ、共通化したい
  {
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      openReq.arcId = ARCID_ZONE_DATA_D;
      openReq.heapForFile = pHeap->GetLowerHandle();
      openReq.heapForArcSrc = pHeap->GetLowerHandle();
      openReq.heapForReq = pHeap->GetLowerHandle();
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->SyncArcFileOpen( openReq );
    }
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
      loadReq.arcId = ARCID_ZONE_DATA_D;
      loadReq.datId = 0;
      loadReq.ppBuf = reinterpret_cast<void**>(&m_pZoneNameBuf);
      loadReq.heapForBuf = pHeap;
      loadReq.pBufSize = &m_ZoneNameDataSize;
      loadReq.align = 4;
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->SyncArcFileLoadData( loadReq );
    }
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId = ARCID_ZONE_DATA_D;
      closeReq.heapForReq = pHeap;
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->SyncArcFileClose( closeReq );
    }
  }

  return true;
}

/**
* @breaf データを削除
*/
void ZoneNameLoader::Delete( void )
{
  GflHeapSafeFreeMemory( m_pZoneNameBuf );
  m_ZoneNameDataSize = 0;
}

/**
* @breaf ゾーンラベル文字列からゾーンIDを取得する
* @param pName      ゾーンラベル文字列
* @param pOut       ゾーンID格納先
* @return ゾーンIDが見当たらなかった場合、false
**/
b32 ZoneNameLoader::GetZoneId( const c8* pName, u32* pOut ) const
{
  if( !Exists() )
  {
    return false;
  }

  // @todo Z_F0102EV01_T0101、Z_F0102EV01_T0102が17文字で超えている。
  u32 zoneNameLengh = 16;
  u32 zoneMax = m_ZoneNameDataSize / zoneNameLengh;

  for( u32 i = 0; i < zoneMax; i++ )
  {
    if( gfl2::core::Utility::Strncmp( &m_pZoneNameBuf[i * zoneNameLengh], pName, zoneNameLengh ) == 0 )
    {
      *pOut = i;
      return true;
    }
  }
  return false;
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif