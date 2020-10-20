//==============================================================================
/**
 * @file   app_tool_RotomToneGenerator.cpp
 * @date   2017/04/26 Wed. 16:35:10
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <debug/include/gfl2_Assert.h>
#include  <System/include/HeapDefine.h>

#include  <Sound/include/Sound.h>

#include  "../../../include/Tool/RotomToneGenerator/app_tool_RotomToneGenerator.h"


#if PM_DEBUG
  #include  "../../../source/Tool/RotomToneGenerator/Debug/RotomToneGeneratorDebug.h"
#endif


namespace app {
namespace tool {
// =============================================================================
/**/
//==============================================================================
/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_app_tool_RotomToneGenerator
namespace FILELOCAL {
  /**
    MMcat[614]   図鑑上のトーン音信号の再生で、マップ上のBGMと干渉し認識率が大きく下がってしまう。
      音量調整機能を実装
  */
  static const f32  BGMVolume_SeqIn_Default   = 0.2f;
  static const f32  BGMVolume_SeqOut_Default  = 1.0f;

  static const u32  TONE_ID_MAX     = 4;
  static const u32  TONE_ID_INVALID = UINT_MAX;

  static const u32  TONE_INTERVAL_100msec =  3;   /**< 30frame * 100msec / 1000msec = 3frame   */
  static const u32  TONE_INTERVAL_133msec =  4;   /**< 30frame * 133msec / 1000msec = 4frame   */
  static const u32  TONE_INTERVAL_166msec =  5;   /**< 30frame * 166msec / 1000msec = 5frame   */
  static const u32  TONE_INTERVAL         = TONE_INTERVAL_133msec;

  /** tone シーケンス定義  */
  static const struct
  {

    u32   toneID;   /**<  PokeToolRotomSoundに渡すIndex  */
    u32   frames;   /**<  待機フレーム数                 */

    /**   */
    void  Tone(const MonsNo monsNo, const pml::FormNo formNo, const pml::Sex gender) const  {if(IsValid()) PokeTool::PlayRotomSound(monsNo, formNo, gender, toneID);}
    bool  IsValid(void) const                                                               {return (toneID < TONE_ID_MAX);}
  }toneSequenceTable[] = {
    {TONE_ID_INVALID, TONE_INTERVAL},
    {0, TONE_INTERVAL},
    {1, TONE_INTERVAL},
    {2, TONE_INTERVAL},
    {3, TONE_INTERVAL},
    {TONE_ID_INVALID, TONE_INTERVAL},
  };

  static const u32  toneSequenceTableElems  = GFL_NELEMS(toneSequenceTable);


  /** 内部状態  */
  enum
  {
    STATE_Idle,
    STATE_StartSequence,
    STATE_DoSequence,
  };


}


// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    ctor
  */
//------------------------------------------------------------------
RotomToneGenerator::RotomToneGenerator(void)
  : m_State(FILELOCAL::STATE_Idle)
  , m_RemainFrames(0)
  , m_Sequence(0)
  , m_BGMVolumeSeqIn(FILELOCAL::BGMVolume_SeqIn_Default)      /**<  リクエスト実行時のBGM音量   */
  , m_BGMVolumeSeqOut(FILELOCAL::BGMVolume_SeqOut_Default)    /**<  リクエスト終了時のBGM音量   */
  , m_BGMVolumeFadeFrame(1.0f)                                /**<  BGM音量変更Frame            */
  , m_BGMBolumeEnable(false)                                  /**<  BGM音量調整を有効にする     */
#if PM_DEBUG
 , m_pDebug(NULL)
#endif
{
#if PM_DEBUG
  {
    gfl2::heap::HeapBase* pDebugHeapBase = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);

    m_pDebug = GFL_NEW(pDebugHeapBase)  RotomToneGeneratorDebug(this);
  }
#endif
}


//------------------------------------------------------------------
/**
  *  @brief    dtor
  */
//------------------------------------------------------------------
RotomToneGenerator::~RotomToneGenerator()
{
  GFL_ASSERT(IsStable());
#if PM_DEBUG
  GFL_SAFE_DELETE(m_pDebug);
#endif
}


//------------------------------------------------------------------
/**
  *  @brief    状態更新
  */
//------------------------------------------------------------------
void  RotomToneGenerator::Update(void)
{
  bool  bMore = false;

  do
  {
    bMore = false;
    switch(m_State)
    {
      case  FILELOCAL::STATE_Idle:
        if(m_Request[REQUESTINFO_Next].IsValid())
        {
          m_Request[REQUESTINFO_Current]  = m_Request[REQUESTINFO_Next];
          m_Request[REQUESTINFO_Next].Invalidate();

          /**
            MMcat[614]   図鑑上のトーン音信号の再生で、マップ上のBGMと干渉し認識率が大きく下がってしまう。
              音量調整機能を実装
          */
          if(m_BGMBolumeEnable)
          {
            Sound::ChangeBGMVolume(m_BGMVolumeSeqIn, m_BGMVolumeFadeFrame);
          }
          
          m_Sequence      = 0;
          m_State         = FILELOCAL::STATE_StartSequence;
        }
        break;

      case  FILELOCAL::STATE_StartSequence:
        if(m_Sequence < FILELOCAL::toneSequenceTableElems)
        {
          const RequestInfo& rRequestInfo  = m_Request[REQUESTINFO_Current];

#if PM_DEBUG
          if(rRequestInfo.monsNo == 0xffff)
          {
            m_RemainFrames  = m_pDebug->GetRemainFrames();
            m_pDebug->Tone(FILELOCAL::toneSequenceTable[m_Sequence].toneID);
          }
          else
#endif
          {
            m_RemainFrames  = FILELOCAL::toneSequenceTable[m_Sequence].frames;
            FILELOCAL::toneSequenceTable[m_Sequence].Tone(rRequestInfo.monsNo, rRequestInfo.formNo, rRequestInfo.gender);
          }
#if PM_DEBUG
          m_pDebug->PrintInterval(m_Sequence);
#endif
         m_State         = FILELOCAL::STATE_DoSequence;
        }
        else
        {
          m_Request[REQUESTINFO_Current].Invalidate();
          /**
            MMcat[614]   図鑑上のトーン音信号の再生で、マップ上のBGMと干渉し認識率が大きく下がってしまう。
              音量調整機能を実装
          */
          if(m_BGMBolumeEnable)
          {
            Sound::ChangeBGMVolume(m_BGMVolumeSeqOut, m_BGMVolumeFadeFrame);
          }
          m_State = FILELOCAL::STATE_Idle;
        }
        break;

      case  FILELOCAL::STATE_DoSequence:
        if(m_RemainFrames)
        {
          --m_RemainFrames;
        }
        if(!m_RemainFrames)
        {
          ++m_Sequence;
          m_State = FILELOCAL::STATE_StartSequence;
          bMore   = true;   /*  同一Frameに処理する  */
        }
        break;

      default:
        GFL_ASSERT(false);
        break;
    }
  }while(bMore);
}


//------------------------------------------------------------------
/**
  *  @brief    動作が完了しているか
  */
//------------------------------------------------------------------
bool  RotomToneGenerator::IsStable(void)const
{
  return (m_State == FILELOCAL::STATE_Idle);
}


//------------------------------------------------------------------
/**
  *  @brief    発音リクエスト
  */
//------------------------------------------------------------------
void  RotomToneGenerator::Request(const MonsNo monsNo, const pml::FormNo formNo, const pml::Sex gender)
{
  m_Request[REQUESTINFO_Next].monsNo  = monsNo;
  m_Request[REQUESTINFO_Next].formNo  = formNo;
  m_Request[REQUESTINFO_Next].gender  = gender;
}


//------------------------------------------------------------------
/**
  *  @brief    強制中断
  */
//------------------------------------------------------------------
void  RotomToneGenerator::Abort(void)
{
  Reset();
}


//------------------------------------------------------------------
/**
  *  @brief    状態のリセット
  */
//------------------------------------------------------------------
void  RotomToneGenerator::Reset(void)
{
  m_State         = FILELOCAL::STATE_Idle;
  m_RemainFrames  = 0;
  m_Sequence      = 0;
  
  m_Request[REQUESTINFO_Current].Invalidate();
}




}  /*  namespace tool  */
}  /*  namespace app  */
