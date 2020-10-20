//======================================================================
/**
 * @file FieldDebugDispInfo.h
 * @date 2015/09/16 13:16:27
 * @author saita_kazuki
 * @brief デバッグ：DispInfo 情報表示
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELDDEBUG_DISP_INFO_H_INCLUDED__
#define __FIELDDEBUG_DISP_INFO_H_INCLUDED__
#pragma once

#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Easing.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>

// 前方宣言
namespace gfl2 {
  namespace debug {
    class DebugWinGroup;
  }
  namespace fs {
    class AsyncFileManager;
  }
}

namespace Field {
  class Fieldmap;
  class ZoneDataLoader;
}

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

#if PM_DEBUG

/**
 * @class DebugDispInfo
 * @brief クラスの説明
 */
class DebugDispInfo
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugDispInfo);

public:
  /**
   *  @brief  コンストラクタ
   */
  DebugDispInfo( void );

  /**
   *  @brief  デストラクタ
   */
  ~DebugDispInfo( void );
  
  /**
   *  @brief  初期化
   */
  void Initialize(
    gfl2::heap::HeapBase *pHeap,
    gfl2::fs::AsyncFileManager* pAsyncFileReadManager,
    Fieldmap* pFieldmap,
    ZoneDataLoader* pZoneDataLoader,
    gfl2::str::MsgData* pMsgData
    );

  /**
   *  @brief  破棄
   */
  void Terminate( void );

  /**
   *  @brief  メニューの作成
   */
  void CreateDebugMenu( gfl2::debug::DebugWinGroup *root );

  /**
   *  @brief  ゾーン名取得
   */
  u32 GetNowZoneID();

  /**
   *  @brief  アトリビュート名取得
   */
  char* DebugDispInfo::GetAttributeName( u32 index );

  /**
   *  @brief  アトリビュートパターン名取得
   */
  char* DebugDispInfo::GetAttributePatternName( u32 index );

  /**
   *  @brief  近景ID名取得
   */
  char* DebugDispInfo::GetNearBgName( u32 index );

  /**
   *  @brief  遠景ID名取得
   */
  char* DebugDispInfo::GetFarBgName( u32 index );

  /**
   *  @brief  今のゾーンID取得
   */
  char* GetZoneName( u32 index );


  //! ヒープの取得
  gfl2::heap::HeapBase *GetHeap( void ){ return m_pHeap; }

  //! メッセージデータ取得
  inline gfl2::str::MsgData* GetMessageData( void ) { return m_pMsgData; }

  //! Fieldmapの取得
  Fieldmap* GetFieldmap() const { return m_pFieldmap; };

  //! ZoneDataLoaderの取得
  ZoneDataLoader* GetZoneDataLoader() const { return m_pZoneDataLoader; };

public:

  static const int ONE_ZONE_CHAR_NUM          = 16; // 1Zone名の文字数
  static const int ONE_ATTR_CHAR_NUM          = 24; // 1Attr名の文字数
  static const int ONE_ATTR_PATTERN_CHAR_NUM  = 24; // 1AttrPattern名の文字数
  static const int ONE_NEAR_BG_CHAR_NUM       = 24; // 1NearBG名の文字数
  static const int ONE_FAR_BG_CHAR_NUM        = 24; // 1FarBG名の文字数

  gfl2::heap::HeapBase*     m_pHeap;
  Fieldmap*                 m_pFieldmap;
  ZoneDataLoader*           m_pZoneDataLoader;
  gfl2::str::MsgData*       m_pMsgData;
  char*                     m_pZoneName;
  char*                     m_pAttrName;
  char*                     m_pAttrPatternName;
  char*                     m_pNearBgName;
  char*                     m_pFarBgName;
  u32                       m_attrPatternMax;

private:

  void SyncArcOpen( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID);
  void SyncArcClose( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID);
  void SyncArcLoad( gfl2::heap::HeapBase* pHeap, gfl2::fs::AsyncFileManager* pAsyncFileReadManager, u32 arcID, u32 datID, void** ppBuff, u32* pSize);

};

#endif // PM_DEBUG

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __FIELDDEBUG_DISP_INFO_H_INCLUDED__
