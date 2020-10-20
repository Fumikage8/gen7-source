#if !defined(BATTLEVIDEOPLAYERDEBUG_H_INCLUDED)
#define BATTLEVIDEOPLAYERDEBUG_H_INCLUDED
#pragma once
#if PM_DEBUG
//==============================================================================
/**
 * @file   BattleVideoPlayerDebug.h
 * @date   2017/05/24 Wed. 12:35:26
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <Debug/include/gfl2_DebugWinSystem.h>
#include  <Debug/include/gfl2_DebugWinItemTypes.h>
#include  <system/include/Date/gfl2_Date.h>

#include  <NetStatic/NetAppLib/include/Debug/NetAppDebugTextDrawer.h>


namespace NetAppLib {
namespace BattleVideoPlayer {
namespace Debug {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  
  *  @date   2017/05/24 Wed. 12:36:24
  */
//==============================================================================
class VideoPlayerDataDebug
{
  GFL_FORBID_COPY_AND_ASSIGN(VideoPlayerDataDebug);

public:
  VideoPlayerDataDebug(void);
  virtual ~VideoPlayerDataDebug();

  void  OnUpdate(void);
  void  OnDraw(const gfl2::gfx::CtrDisplayNo displayNo);

#if defined( GF_PLATFORM_CTR )
  void  OverwriteServerTimeByLocalTimeIfEnable(nn::nex::DateTime & rNexDateTime);
#endif


  void      OnUpdate_TimeVal(gfl2::debug::DebugWinItem* pWinItem, const u32 mode);
  wchar_t*  OnDraw_TimeVal(gfl2::debug::DebugWinItem* pWinItem, const u32 mode);
protected:
  enum  GroupID
  {
    GROUP_dummy,
    
    GROUP_MAX
  };


  gfl2::debug::DebugWinGroup*     m_pGroup_Root;
  gfl2::debug::DebugWinGroup*     m_pGroups[GROUP_MAX];

  /*  items  */
  enum  {
    VALITEM_OverwriteMetaCreateTimeByLocalTime,   /**<  ダウンロードしたVideoDataの登録日時をローカル時刻で上書きする  */
    
    VALITEM_MAX
  };

  struct  ItemInfo
  {
    VideoPlayerDataDebug*                   pThis;
    gfl2::debug::DebugWinItemValueU32<u32>* pViewItem;
    u32                                     value;
  };

  ItemInfo  m_ViewItemInfos[VALITEM_MAX];


  gfl2::system::Date    m_LocalTime;
  u32                   m_LocalTimeTarget;

//  NetAppLib::Debug::NetAppDebugTextDrawer m_Text;

private:
};


}  /* namespace Debug */
}  /*  namespace BattleVideoPlayer  */
}  /*  namespace NetAppLib  */
#endif  /*  #if PM_DEBUG                                     *//
#endif  /*  #if !defined(BATTLEVIDEOPLAYERDEBUG_H_INCLUDED)  */
