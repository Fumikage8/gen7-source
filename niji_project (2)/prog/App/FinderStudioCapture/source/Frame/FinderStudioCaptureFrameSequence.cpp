//======================================================================
/**
 * @file    FinderStudioCaptureFrameSequence.cpp
 * @date    2016/11/16 15:10:08
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：フレームシーケンス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureFrameSequence.h"

#include <App/FinderStudioCapture/source/Debug/FinderStudioCaptureDebug.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

/**
 * @brief   次のフレームIDの取得
 *
 * @param[in]   nowFrameID    現在のフレームID
 * @param[in]   result        フレームリザルト
 *
 * @return    次のフレームID
 */
Frame::FrameID FrameSequence::GetNextFrameID( Frame::FrameID nowFrameID, Frame::Result result )
{
#if PM_DEBUG
  /// 自動撮影開始か？
  if( DebugClass::IsAutoCapture_Start() )
  {
    return Frame::FRAME_ID_DEBUG_AUTOCAP;
  }
  /// 写真差分機能か？
  if( DebugClass::IsPhotoCompare_Start() )
  {
    if( nowFrameID == Frame::FRAME_ID_SHOOT_MAIN && result == Frame::RES_NEXT )
    {
      return Frame::FRAME_ID_DEBUG_PHOTOCOMPARE;
    }
  }
  /// エージングか？
  if( DebugClass::IsAging_Start() )
  {
    return Frame::FRAME_ID_DEBUG_AGING;
  }
#endif

  if( nowFrameID >= Frame::FRAME_ID_NUM )
  {
    GFL_ASSERT( 0 );
    return Frame::FRAME_ID_NULL;
  }
  if( result >= Frame::RES_NUM )
  {
    GFL_ASSERT( 0 );
    return Frame::FRAME_ID_NULL;
  }

  const struct NextFrameParam
  {
    Frame::FrameID frameList[Frame::RES_NUM];
    /*
        Frame::RES_PREV,   //!< 前のフレームへ
        Frame::RES_NEXT,   //!< 次のフレームへ
        Frame::RES_FINISH, //!< 強制終了
        Frame::RES_ERROR,  //!< フェイタルエラー
        Frame::RES_NUM
    */
  } c_NextFrameParam[Frame::FRAME_ID_NUM] = {
    { // FRAME_ID_SDCARD_LOAD
      Frame::FRAME_ID_NULL,
      Frame::FRAME_ID_SELECT_DIFFICULTY,
      Frame::FRAME_ID_NULL,
      Frame::FRAME_ID_NULL,
    },
    { // FRAME_ID_SELECT_DIFFICULTY
      Frame::FRAME_ID_NULL,
      Frame::FRAME_ID_SHOOT_PREPARE,
      Frame::FRAME_ID_NULL,
      Frame::FRAME_ID_NULL,
    },
    { // FRAME_ID_SHOOT_PREPARE
      Frame::FRAME_ID_SELECT_DIFFICULTY,
      Frame::FRAME_ID_SHOOT_MAIN,
      Frame::FRAME_ID_NULL,
      Frame::FRAME_ID_NULL,
    },
    { // FRAME_ID_SHOOT_MAIN
      Frame::FRAME_ID_SHOOT_PREPARE,
      Frame::FRAME_ID_PHOTO_LIST,
      Frame::FRAME_ID_NULL,
      Frame::FRAME_ID_NULL,
    },
    { // FRAME_ID_PHOTO_LIST
      Frame::FRAME_ID_SDCARD_SAVE,
      Frame::FRAME_ID_SDCARD_SAVE,
      Frame::FRAME_ID_NULL,
      Frame::FRAME_ID_NULL,
    },
    { // FRAME_ID_SDCARD_SAVE
      Frame::FRAME_ID_SHOOT_PREPARE,
      Frame::FRAME_ID_SHOOT_MAIN,
      Frame::FRAME_ID_NULL,
      Frame::FRAME_ID_NULL,
    },
#if PM_DEBUG
    { // FRAME_ID_DEBUG_AUTOCAP
      Frame::FRAME_ID_SELECT_DIFFICULTY,
      Frame::FRAME_ID_SELECT_DIFFICULTY,
      Frame::FRAME_ID_NULL,
      Frame::FRAME_ID_NULL,
    },
    { // FRAME_ID_DEBUG_PHOTOCOMPARE
      Frame::FRAME_ID_SHOOT_MAIN,
      Frame::FRAME_ID_SHOOT_MAIN,
      Frame::FRAME_ID_NULL,
      Frame::FRAME_ID_NULL,
    },
    { // FRAME_ID_DEBUG_AGING
      Frame::FRAME_ID_SHOOT_PREPARE,
      Frame::FRAME_ID_SHOOT_PREPARE,
      Frame::FRAME_ID_NULL,
      Frame::FRAME_ID_NULL,
    },
#endif
  };

  Frame::FrameID frameID = c_NextFrameParam[nowFrameID].frameList[result];
  
  GFL_PRINT(
    "GetNextFrameID : NowFrameID=[%d] FrameResult=[%d] NextFrameID=[%d]\n"
    , nowFrameID, result, frameID );

  return frameID;
}

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
