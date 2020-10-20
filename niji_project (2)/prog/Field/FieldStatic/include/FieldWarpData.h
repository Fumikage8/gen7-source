//==============================================================================
/**
 * @file        FieldWarpData.h
 * @brief       ワープデータを読み込み、提供
  
    sango より移植.
 * 
 * @author      N.Takeda
 * @date        2015/07/15(水) 20:03:07
 */
//==============================================================================
#ifndef __FIELDWARPDATA_H__
#define __FIELDWARPDATA_H__

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <heap/include/gfl2_heap.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>

#include "GameSys/include/GameData.h"

#include "Field/FieldStatic/include/FieldLocation.h"

namespace GameSys{
  class GameManager;
}

namespace Field
{


//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          構造体宣言
*/
//-----------------------------------------------------------------------------
struct WARPDATA;
struct WARPDATA2;

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
 * @class       WarpDataTools
 * @brief       
 * @author      N.Takeda
 * @date        2015/07/16(木) 11:37:40
 */
//------------------------------------------------------------------------------
class WarpDataTools
{
public:
  WarpDataTools()
    : m_pWarpData( NULL )
    , m_pWarpData2( NULL )
    , m_iWarpDataSize( 0 )
    , m_iWarpData2Size( 0 )
  {}

  enum RecoverType{
    RECOVER_TYPE_POKECEN,
    RECOVER_TYPE_MYHOME,
    RECOVER_TYPE_RESCUE,
  };
public:

  //--------------------------------------------------------------------------------------------
  //  ワープとび先
  //--------------------------------------------------------------------------------------------
  // 空を飛ぶように設定されたZoneIDを取得
  u16 GetWarpZoneID( int warp_id );
  // 復帰先でワープIDを取得
  int SearchByRoomID(ZoneID zone_id, const GameSys::GameData* p_gdata);
  /// 戻り先タイプの取得.
  RecoverType GetRecoverType( int warp_id );

  void GetRecoverPoint( const s32 warp_id, ZoneID *Receipt_zoneId, gfl2::math::Vector3 *Receipt_pos );
  void GetFlyPoint( const s32 warp_id, ZoneID *Receipt_zoneId, gfl2::math::Vector3 *Receipt_pos );
  
  //--------------------------------------------------------------------------------------------
  // 到達による到達フラグ・フライフラグの設定
  //--------------------------------------------------------------------------------------------
  void UpdateArrive( GameSys::GameData * gamedata, int zone_id );

  //--------------------------------------------------------------------------------------------
  // 手動設定のFLYFLAGをセットする
  //--------------------------------------------------------------------------------------------
  void SetFlyFlags( EventWork * p_eventwork, int zone_id );
  
  //--------------------------------------------------------------------------------------------
  //  到着フラグ取得
  //--------------------------------------------------------------------------------------------
  bool GetArriveFlag( const GameSys::GameData& gamedata, int warp_id );

  // ワープIDが正常化チェック
  bool IsWarpIDOK( int warp_id );

  // 自宅のワープIDを取得
  int GetMyHomeWarpID( u32 sex );


  //--------------------------------------------------------------------------------------------
  //  リソース初期化
  //--------------------------------------------------------------------------------------------
  void Initialize( GameSys::GameManager* p_gman, gfl2::heap::HeapBase* pHeap );
  bool InitializeWait(GameSys::GameManager* p_gman);
  void Terminate(void);

  //--------------------------------------------------------------------------------------------
  //  デバッグ
  //--------------------------------------------------------------------------------------------
#if PM_DEBUG
  void DebugSetFlyFlags( EventWork * p_eventwork, bool set_f );
#endif

private:

  // ワープIDをデータテーブルのインデックスに編集
  int RegulateWarpID( int warp_id );

  b32 IsDataSkip( int warp_id, const GameSys::GameData* p_gdata);

  WARPDATA    *m_pWarpData;
  WARPDATA2   *m_pWarpData2;
  s32         m_iWarpDataSize;
  s32         m_iWarpData2Size;
  bool        m_isInitialized;
  gfl2::heap::HeapBase* m_pHeap;
  void *m_pBuffer;
};


} // Field

#endif  // __FIELDWARPDATA_H__


