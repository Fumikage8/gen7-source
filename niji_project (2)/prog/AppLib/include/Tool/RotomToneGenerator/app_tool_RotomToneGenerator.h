#if !defined(APP_TOOL_ROTOMTONEGENERATOR_H_INCLUDED)
#define APP_TOOL_ROTOMTONEGENERATOR_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   app_tool_RotomToneGenerator.h
 * @date   2017/04/26 Wed. 16:37:40
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


#include  <PokeTool/include/PokeToolRotomSound.h>

namespace app {
namespace tool {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2017/04/26 Wed. 16:37:44
  */
//==============================================================================
class RotomToneGenerator
{
  GFL_FORBID_COPY_AND_ASSIGN(RotomToneGenerator);

public:
  RotomToneGenerator(void);       /**<  ctor  */
  virtual ~RotomToneGenerator();  /**<  dtor  */

  void  Update(void);             /**<  状態更新              */
  bool  IsStable(void) const;     /**<  動作が完了しているか  */
  void  Request(const MonsNo monsNo, const pml::FormNo formNo, const pml::Sex gender);

  void  Abort(void);              /**<  強制停止  */

  /**
    MMcat[614]   図鑑上のトーン音信号の再生で、マップ上のBGMと干渉し認識率が大きく下がってしまう。
      音量調整機能を実装
  */
  void  SetBGMVolume(const f32 volume_SeqIn, const f32 volume_SeqOut, const f32 fadeFrame = 1.0f)  {m_BGMVolumeSeqIn = volume_SeqIn; m_BGMVolumeSeqOut = volume_SeqOut; m_BGMVolumeFadeFrame  = fadeFrame; SetBGMVolumeEnable(true);}
  void  SetBGMVolumeEnable(const bool bEnable)                                                     {m_BGMBolumeEnable  = bEnable;}


  /**
    MMcat
  */

#if PM_DEBUG
  class RotomToneGeneratorDebug;
#endif

protected:
  void  Reset(void);              /**<  状態のリセット    */

  u32   m_State;                  /**<  内部状態          */
  u32   m_RemainFrames;           /**<  残フレーム        */
  u32   m_Sequence;               /**<  内部シーケンス    */
  /**
    MMcat[614]   図鑑上のトーン音信号の再生で、マップ上のBGMと干渉し認識率が大きく下がってしまう。
      音量調整機能を実装
  */
  f32   m_BGMVolumeSeqIn;         /**<  リクエスト実行時のBGM音量   */
  f32   m_BGMVolumeSeqOut;        /**<  リクエスト終了時のBGM音量   */
  f32   m_BGMVolumeFadeFrame;     /**<  BGM音量変更Frame            */
  bool  m_BGMBolumeEnable;        /**<  BGM音量調整を有効にする     */

  enum  
  {
    REQUESTINFO_Current,
    REQUESTINFO_Next,

    REQUESTINFO_MAX
  };

  struct RequestInfo
  {
    MonsNo      monsNo;
    pml::FormNo formNo;
    pml::Sex    gender;
    
    
    RequestInfo(void)
      : monsNo(MONSNO_NULL)
      , formNo(0)
      , gender(pml::SEX_MALE)
    {
    }
    
    bool  IsValid(void) const
    {
      return (
        ((MONSNO_NULL < monsNo) && (monsNo <= MONSNO_END))
#if PM_DEBUG
        || (monsNo == 0xffff)
#endif
      );
    }
    
    void  Invalidate(void)
    {
      monsNo = MONSNO_NULL;
    }
  }m_Request[REQUESTINFO_MAX];

#if PM_DEBUG
  RotomToneGeneratorDebug*  m_pDebug;
#endif
};


}  /*  namespace tool  */
}  /*  namespace app  */
#endif  /*  #if !defined(APP_TOOL_ROTOMTONEGENERATOR_H_INCLUDED)  */
