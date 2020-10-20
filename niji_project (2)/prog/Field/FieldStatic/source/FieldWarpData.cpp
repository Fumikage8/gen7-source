//==============================================================================
/**
 * @file        FieldWarpData.cpp
 * @brief       ワープデータを読み込み、提供
  
    sango より移植.
 * 
 * @author      N.Takeda
 * @date        2015/07/15(水) 20:02:28
 */

//==============================================================================

#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>

#include "System/include/HeapDefine.h"
#include "GameSys/include/GameManager.h"
#include "Field/FieldStatic/include/FieldWarpData.h"
#include "Savedata/include/EventWork.h"

#include <arc_def_index/arc_def.h>
#include "niji_conv_header/field/field_resident/field_resident_pack.h"

namespace Field{

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          実装
*/
//-----------------------------------------------------------------------------

struct WARPDATA
{
  u32 sky_flag_id;       ///< 空を飛ぶフラグID
//  u32 locator_id;           ///< ロケーターID(まだ準備できてないので、いったんゼロ)

  u16 revoer_zone_id;       ///< 全滅時の戻り先ZoneID
  u16 warp_zone_id;         ///< ワープ先のZoneID

  u32 IsTeleportPos:1;      ///< フラグの格納領域 4byte境界に合わせるので32bit分
  u32 AutoSetArriveFlag:1;
  u32 RecverType:2;         ///< 0:ポケセン　1:自宅　2:救急車
  u32 :28;

  f32 recover_pos_x;
  f32 recover_pos_z;        ///< Yは吸着でいい  
  f32 warp_pos_x;
  f32 warp_pos_z;
};

struct WARPDATA2
{
  u16 zone_id;      ///< ZoneID
  u16 pad;
  u32 flag_id;       ///< 到着フラグID
  
};

struct FLY_DATA {
  u16 zone_id;
  u16 flag_ids[4];
};

struct FLY_ZONE_DATA {
  u16 zone_id;
  u8  auto_flag;
};

//--------------------------------------------------------------------------------------------
//  ワープとび先
//--------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *  @brief  ワープIDのゾーンIDを取得
 *
 *  @param  warp_id 
 *
 *  @return
 */
//-----------------------------------------------------------------------------
u16 WarpDataTools::GetWarpZoneID( int warp_id )
{
  warp_id = RegulateWarpID( warp_id );
  return m_pWarpData[ warp_id ].warp_zone_id;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief データをパスするか判定
 *
 * @param warp_id ワープID
 * @param p_gdata ゲームデータ
 *
 * @return true: スキップする, false: スキップしない
 */
/* ------------------------------------------------------------------------- */
b32 WarpDataTools::IsDataSkip( int warp_id, const GameSys::GameData* p_gdata)
{
  bool is_filter_pass = false;

  if (warp_id >= m_iWarpDataSize)
  {
    GFL_ASSERT(0);
    return true;
  }
  
#if 0
  if (m_pWarpData[warp_id].Filter == WARPDATA_FILTER_MALE) 
  {
    if (p_gdata->GetPlayerStatus()->GetSex() != PM_MALE)
    {
      //HOSAKA_PRINT("m_pWarpData skip warp_id=%d\n", warp_id);
      is_filter_pass = true;
    }
  }
  else if (m_pWarpData[warp_id].Filter == WARPDATA_FILTER_FEMALE) 
  {
    if (p_gdata->GetPlayerStatus()->GetSex() != PM_FEMALE)
    {
      //HOSAKA_PRINT("m_pWarpData skip warp_id=%d\n", warp_id);
      is_filter_pass = true;
    }
  }
#endif

  return is_filter_pass;
}

//----------------------------------------------------------------------------
/**
 *  @brief  復帰先でワープIDを取得
 *
 *  @param  zone_id 
 *
 *  @return ワープID
 */
//-----------------------------------------------------------------------------
int WarpDataTools::SearchByRoomID(ZoneID zone_id, const GameSys::GameData* p_gdata)
{
  int i;
  for (i = 0; i < m_iWarpDataSize; ++i)
  {
    if (m_pWarpData[i].revoer_zone_id == zone_id
        && m_pWarpData[i].IsTeleportPos && IsDataSkip(i, p_gdata) == false) {
      return i + 1;
    }
  }
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        GetRecoverType
 * @brief       戻り先タイプの取得.
 * @author      N.Takeda
 * @date        2015/09/15(火) 17:00:54
 *
 * @param[in]   warp_id .
 *
 * @return      int 
 */
//------------------------------------------------------------------------------
WarpDataTools::RecoverType WarpDataTools::GetRecoverType( int warp_id )
{
  return static_cast<WarpDataTools::RecoverType>(m_pWarpData[ warp_id ].RecverType);
}

//--------------------------------------------------------------------------------------------
//  到着フラグ
//--------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *  @brief  到着フラグ・フライフラグを設定
 *
 *  @param  gamedata    ゲームデータ
 *  @param  zone_id     ゾーンID
 */
//-----------------------------------------------------------------------------
void WarpDataTools::UpdateArrive( GameSys::GameData * gamedata, int zone_id )
{
  EventWork* p_eventwork = gamedata->GetEventWork();
  
  for (s32 ii = 0; ii < m_iWarpDataSize; ii++)
  {
    if (m_pWarpData[ii].warp_zone_id == zone_id && m_pWarpData[ii].AutoSetArriveFlag)
    {
      p_eventwork->SetEventFlag( m_pWarpData[ii].sky_flag_id );
      break;
    }
  }
  
  for (s32 ii = 0; ii < m_iWarpData2Size; ii++)
  {
    if (m_pWarpData2[ii].zone_id == zone_id)
    {
      p_eventwork->SetEventFlag( m_pWarpData2[ii].flag_id );
      break;
    }
  }
}

/* ------------------------------------------------------------------------- */
/**
 * @brief ゾーンをトリガーにフライフラグをセット(スクリプトコマンドから呼んでいるのみ2015/11/25現在)
 *
 * @param p_eventwork イベントワーク
 * @param zone_id ゾーンID
 */
/* ------------------------------------------------------------------------- */
void WarpDataTools::SetFlyFlags( EventWork * p_eventwork, int zone_id )
{
  for (s32 i = 0; i < m_iWarpDataSize; i++)
  {
    if (m_pWarpData[i].warp_zone_id == zone_id )
    {
      p_eventwork->SetEventFlag( m_pWarpData[i].sky_flag_id );
      break;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  到着フラグを取得する
 *
 *  @param  gamedata  ゲームデータ
 *  @param  warp_id   ワープID
 *
 *  @retval true  到着している
 *  @retval false 到着していない
 */
//-----------------------------------------------------------------------------
bool WarpDataTools::GetArriveFlag( const GameSys::GameData& gamedata, int warp_id )
{
  int reg_id = RegulateWarpID(warp_id);
  const EventWork* cp_eventwork = gamedata.GetEventWork();
  return cp_eventwork->CheckEventFlag( m_pWarpData[reg_id].sky_flag_id );
}


//-----------------------------------------------------------------------------
/**
 *  隠蔽関数
 */
//-----------------------------------------------------------------------------


// 
//----------------------------------------------------------------------------
/**
 *  @brief  ワープIDが正常化チェック
 *
 *  @param  warp_id   ワープID
 *
 *  @retval true  正常
 *  @retval false 不正
 */
//-----------------------------------------------------------------------------
bool WarpDataTools::IsWarpIDOK( int warp_id )
{
  if( (warp_id <= 0) || (warp_id > m_iWarpDataSize) )
  {
    GFL_ASSERT_MSG( 0, "不正なワープID(%d)です\n", warp_id );
    return false;
  }
  return true;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 自宅のワープIDを取得
 *
 * @param sex 性別
 *
 * @return ワープID
 */
/* ------------------------------------------------------------------------- */
int WarpDataTools::GetMyHomeWarpID( u32 sex ) 
{
  if (sex == PM_MALE)
  {
    return 1;
  }

  return 2;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ワープIDをデータテーブルのインデックスに編集
 *
 *  0オリジン→1オリジンに変換
 */
//-----------------------------------------------------------------------------
int WarpDataTools::RegulateWarpID( int warp_id )
{
  if( !IsWarpIDOK(warp_id) ){
    return 0;
    //warp_id = InitializeWarpID;  // 
  }

  -- warp_id;
  return warp_id;
} 

//------------------------------------------------------------------------------
/**
 * @func        GetRecoverPoint
 * @brief       Recoverの取得.
 * @author      N.Takeda
 * @date        2015/07/16(木) 11:46:39
 *
 * @param[in]   const s32             warp_id         .
 * @param[out]  ZoneID*               Receipt_zoneId  ID.
 * @param[out]  gfl2::math::Vector3*  Receipt_pos     .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  WarpDataTools::GetRecoverPoint( const s32 warp_id, ZoneID *Receipt_zoneId, gfl2::math::Vector3 *Receipt_pos )
{
  GFL_ASSERT( m_pWarpData );
  if( m_pWarpData )
  {
    *Receipt_zoneId = m_pWarpData[ warp_id ].revoer_zone_id;
    Receipt_pos->x = m_pWarpData[ warp_id ].recover_pos_x;
    Receipt_pos->z = m_pWarpData[ warp_id ].recover_pos_z;
  }
}

//------------------------------------------------------------------------------
/**
 * @func        GetFlyPoint
 * @brief       FlyPointの取得.
 * @author      N.Takeda
 * @date        2015/07/16(木) 11:46:39
 *
 * @param[in]   const s32             warp_id         .
 * @param[out]  ZoneID*               Receipt_zoneId  ID.
 * @param[out]  gfl2::math::Vector3*  Receipt_pos     .
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void  WarpDataTools::GetFlyPoint( const s32 warp_id, ZoneID *Receipt_zoneId, gfl2::math::Vector3 *Receipt_pos )
{
  GFL_ASSERT( m_pWarpData );
  if( m_pWarpData )
  {
    *Receipt_zoneId = m_pWarpData[ warp_id ].warp_zone_id;
    Receipt_pos->x = m_pWarpData[ warp_id ].warp_pos_x;
    Receipt_pos->z = m_pWarpData[ warp_id ].warp_pos_z;
  }
}

/**
 *  @brief  リソース初期化(読み込み開始）
 */
void WarpDataTools::Initialize( GameSys::GameManager* p_gman, gfl2::heap::HeapBase* pHeap )
{
  gfl2::fs::AsyncFileManager* pAsyncFileManager = p_gman->GetAsyncFileManager();
  m_pHeap = pHeap;
  gfl2::heap::HeapBase* tempHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE )->GetLowerHandle(); //この瞬間はまだ使っていないAppDeviceをテンポラリに使う

  //---------------------------------------------------
  // ARCファイルオープン
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_FIELD_RESIDENT;
    openReq.heapForFile = tempHeap;
    openReq.heapForArcSrc = tempHeap;
    openReq.heapForReq = tempHeap;
    pAsyncFileManager->AddArcFileOpenReq( openReq );
  }
  //---------------------------------------------------
  // ファイル読み込みリクエスト
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;

    loadReq.arcId = ARCID_FIELD_RESIDENT;
    loadReq.datId = 0;
    loadReq.ppBuf = &m_pBuffer;
    loadReq.heapForBuf = tempHeap;
    loadReq.heapForReq = tempHeap;
    loadReq.heapForCompressed = tempHeap;
    loadReq.pBufSize = NULL;
    loadReq.align = 128;
    pAsyncFileManager->AddArcFileLoadDataReq( loadReq );
  }
  
}

/**
 *  @brief  リソース初期化待ち（読み込み待ち）
 */
bool WarpDataTools::InitializeWait(GameSys::GameManager* p_gman)
{
  const s32 HEADER_SIZE = 4;

  gfl2::fs::AsyncFileManager* pAsyncFileManager = p_gman->GetAsyncFileManager();

  //読み込みが終了している
  if ( m_isInitialized )
  {
    return true;
  }
  
  // 読み込み完了待ち
  if( !pAsyncFileManager->IsArcFileOpenFinished( ARCID_FIELD_RESIDENT ) ){
    return false;
  }
  if( !pAsyncFileManager->IsArcFileLoadDataFinished( &m_pBuffer ) ){
    return false;
  }

  //データ解析
  {
    gfl2::fs::BinLinkerAccessor binAccessor;
    binAccessor.Initialize(m_pBuffer);
    {
      u32* pBin = reinterpret_cast<u32*>(binAccessor.GetData(BL_IDX_FIELD_RESIDENT__WARPDATA_BIN));
      s32 binSize = static_cast<s32>(binAccessor.GetDataSize(BL_IDX_FIELD_RESIDENT__WARPDATA_BIN)-HEADER_SIZE); ///< Coverity.11811
      m_pWarpData = reinterpret_cast<WARPDATA*>(GflHeapAllocMemory( m_pHeap, binSize ));
      m_iWarpDataSize = *pBin;
      gfl2::std::MemCopy( reinterpret_cast<void*>(pBin + 1), m_pWarpData, binSize);
    }
    {
      u32* pBin = reinterpret_cast<u32*>(binAccessor.GetData(BL_IDX_FIELD_RESIDENT__WARPDATA2_BIN));
      s32 binSize = static_cast<s32>(binAccessor.GetDataSize(BL_IDX_FIELD_RESIDENT__WARPDATA2_BIN)-HEADER_SIZE);  ///< Coverity.11811
      m_pWarpData2 = reinterpret_cast<WARPDATA2*>(GflHeapAllocMemory( m_pHeap, binSize ));
      m_iWarpData2Size = *pBin;
      gfl2::std::MemCopy( reinterpret_cast<void*>(pBin + 1), m_pWarpData2, binSize);
    }
    m_isInitialized = true;
    binAccessor.Initialize(NULL);
  }

  if( m_pBuffer ){
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId = ARCID_FIELD_RESIDENT;
      closeReq.heapForReq = m_pHeap->GetLowerHandle();
      pAsyncFileManager->SyncArcFileClose( closeReq );
    }
    GflHeapSafeFreeMemory( m_pBuffer );
  }

  return true;
}

/**
 *  @brief  リソース破棄
 */
void WarpDataTools::Terminate(void)
{
  GflHeapSafeFreeMemory( m_pWarpData );
  GflHeapSafeFreeMemory( m_pWarpData2 );
  m_iWarpDataSize = 0;
  m_iWarpData2Size = 0;
  m_isInitialized = false;
}

#if PM_DEBUG
/* ------------------------------------------------------------------------- */
/**
 * @brief ゾーンをトリガーにフライフラグをセット(スクリプトコマンドから呼んでいるのみ2015/11/25現在)
 *
 * @param p_eventwork イベントワーク
 * @param zone_id ゾーンID
 */
/* ------------------------------------------------------------------------- */
void WarpDataTools::DebugSetFlyFlags( EventWork * p_eventwork, bool set_f )
{
  if(set_f){
    for (s32 i = 0; i < m_iWarpDataSize; i++)
    {
      p_eventwork->SetEventFlag( m_pWarpData[i].sky_flag_id );
    }
  }else{
    for (s32 i = 0; i < m_iWarpDataSize; i++)
    {
      p_eventwork->ResetEventFlag( m_pWarpData[i].sky_flag_id );
    }
  }

}

#endif


} // namespace Field
