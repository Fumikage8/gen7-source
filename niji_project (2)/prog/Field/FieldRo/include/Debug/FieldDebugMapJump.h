//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldDebugMapJump.h
 *	@brief  マップジャンプ
 *	@author	tomoya takahashi
 *	@date		2015.04.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#if PM_DEBUG

#if !defined( __FIELD_DEBUG_MAPJUMP_H__ )
#define __FIELD_DEBUG_MAPJUMP_H__
#pragma once

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <Fs/include/gfl2_FsAsyncFileManager.h>


GFL_NAMESPACE_BEGIN(Field);

class Fieldmap;

GFL_NAMESPACE_BEGIN(Debug);


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class DebugMapJump
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugMapJump); //コピーコンストラクタ＋代入禁止
public:
  DebugMapJump();
  ~DebugMapJump();

  void Initialize( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 zoneID, Fieldmap* pFieldmap, gfl2::str::MsgData* pMsgData );
  void Terminate();

  const char* GetZoneName( u32 index ) const;
  const gfl2::str::STRCODE* GetZonePlaceName( u32 index );
  const gfl2::str::STRCODE* GetZonePlaceNameFollow( u32 index );
  inline u32 GetZoneMax() const { return m_ZoneMax; }

  inline const char* GetSelectZoneName() const { return GetZoneName(m_ZoneIndex); }
  inline const char* GetSelectZoneNameForFilter() const { return GetZoneName(m_ZoneIndexForFilter); }
  inline u32* GetSelectZoneId(){ return &m_ZoneIndex; }
  inline u32* GetSelectZoneFilterId(){ return &m_ZoneFilterId; }
  inline u32* GetSelectZoneIdForFilter(){ return &m_ZoneIndexForFilter; }

  void CreateDebugMenuGroup( gfl2::debug::DebugWinGroup* root );

  u32 GetNowZoneID();
  gfl2::heap::HeapBase *GetHeap(){ return m_pHeap; }

  inline gfl2::str::MsgData* GetMessageData( void ) { return m_pMsgData; } 

  void SetZoneIndexForFilterTop( u32 startZone = 0, s32 add = 1, b32 firstSkip = false );

  s32 GetZoneSkipNext( s32 startZone, const char* pFilter, s32 dir = 1 );

  b32 IsFilterHit( u32 filterIndex, s32 zoneId ) const;

private:
  static const int ONE_ZONE_CHAR_NUM = 16; // 1Zone名の文字数


private:

  gfl2::debug::DebugWinGroup* m_pGroup;
  gfl2::heap::HeapBase* m_pHeap;
  char* m_pZoneName;
  size_t m_ZoneNameDataSize;

  Fieldmap* m_pFieldmap;

  u32 m_ZoneFilterId;
  u32 m_ZoneMax;

  u32 m_ZoneIndex;
  u32 m_ZoneIndexForFilter;

  gfl2::str::MsgData* m_pMsgData;
  gfl2::str::StrBuf*  m_pStrBuffPlaceName;
  gfl2::str::StrBuf*  m_pStrBuffPlaceNameFollow;

};



GFL_NAMESPACE_END(Debug);
GFL_NAMESPACE_END(Field);

#endif	// __FIELD_DEBUG_MAPJUMP_H__

#endif // PM_DEBUG
