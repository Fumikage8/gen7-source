//======================================================================
/**
 * @file    FinderStudioDecorationFrameSequence.cpp
 * @date    2017/01/31 15:50:40
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：フレームシーケンス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationFrameSequence.h"

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

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
      Frame::FRAME_ID_DECORATION,
      Frame::FRAME_ID_NULL,
      Frame::FRAME_ID_NULL,
    },
    { // FRAME_ID_DECORATION
      Frame::FRAME_ID_NULL,
      Frame::FRAME_ID_SDCARD_SAVE,
      Frame::FRAME_ID_NULL,
      Frame::FRAME_ID_NULL,
    },
    { // FRAME_ID_SDCARD_SAVE
      Frame::FRAME_ID_NULL,
      Frame::FRAME_ID_NULL,
      Frame::FRAME_ID_NULL,
      Frame::FRAME_ID_NULL,
    },
  };

  Frame::FrameID frameID = c_NextFrameParam[nowFrameID].frameList[result];

  GFL_PRINT(
    "GetNextFrameID : NowFrameID=[%d] FrameResult=[%d] NextFrameID=[%d]\n"
    , nowFrameID, result, frameID );

  return frameID;
}

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
