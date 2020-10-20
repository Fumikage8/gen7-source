#if !defined(GFL2_DEBUGLOGWINDOW_H_INCLUDED)
#define GFL2_DEBUGLOGWINDOW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   gfl2_DebugLogWindow.h
 * @date   12/12/7
 * @date   2016/01/26 Tue. 15:15:14
 * @author ariizumi
 * @author muto_yu
 * @brief  デバッグログウィンドウ
 * @note   旧作より移植
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <base/include/gfl2_Singleton.h>
#include  <heap/include/gfl2_Heap.h>
#include  <str/include/gfl2_Str.h>
#include  <debug/include/gfl2_Assert.h>
#include  <debug/include/gfl2_DebugPrint.h>
#include  <fs/include/gfl2_FsDirectSdmcFile.h>
#include  <base/include/gfl2_Bit.h>
#include  <gfx/include/gfl2_types.h>
#include  <gfx/include/gfl2_GLMemory.h>

// =============================================================================
/**/
//==============================================================================
#if GFL_DEBUG
  #define USE_DEBUG_LOG_WIN 1
#else
  #define USE_DEBUG_LOG_WIN 0
#endif


// =============================================================================
/**/
//==============================================================================
namespace gfl2{ namespace fs{   class DirectSdmcFile; }}
namespace gfl2{ namespace util{ class UtilTextRenderingPipeLine; }}
namespace gfl2{ namespace util{ class DrawUtilText; }}


namespace gfl2  {
namespace debug {
// =============================================================================
/**/
//==============================================================================

//ログインデックス
//  それぞれの値がかぶらないようにここに書くこと
enum  LogWinID
{
  LOGWIN_ID_POKE_MEMORY,
  LOGWIN_ID_POKE_FISHING,
  LOGWIN_ID_POKE_KINOMI,
  LOGWIN_ID_POKE_SAVE,
  LOGWIN_ID_WEATHER,
  LOGWIN_ID_SAKASA_SHINKA,
  LOGWIN_ID_BATTLE_VIEW,
  LOGWIN_ID_NEX,
  LOGWIN_ID_PGL,
  LOGWIN_ID_VALIDATION,  //以下テスト用ダミー
  LOGWIN_ID_DUMMY_8,

  LOGWIN_ID_MAX,    //最大値定義
  LOGWIN_ID_INVALID /**<  無効値  */
};


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
extern void InitDebugLogWindow(gfl2::heap::HeapBase *heap, gfl2::util::DrawUtilText* pTextUtil , gfl2::util::UtilTextRenderingPipeLine* pPipeLine );


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
extern void LogWindow_SetEnable(bool flg, const LogWinID logIdx = LOGWIN_ID_INVALID);


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
extern void LogWindow_SetDispFlg(bool flg, const LogWinID logIdx);

//----------------------------------------------------------------------------
/**
 *  @brief  ログウィンドウへメッセージをセット
 *  @param  u16 logIdx          表示制御のためのログインデックス
 *  @param  const wchar_t *msg  表示メッセージ(可変長引数対応)
 */
//-----------------------------------------------------------------------------
extern void LogWindow_AddMsg(const LogWinID logIdx,const wchar_t *msg, ... );


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
extern void LogWindow_SetFont();


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
extern void LogWindow_SetFont(gfl2::str::Font* pFont);


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
extern void LogWindow_SetFontScale(const f32 scaleH, const f32 scaleV);


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
extern void LogWindow_SetFontScaleDefault(void);



//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/01/26 Tue. 17:38:56
  */
//==============================================================================
#if USE_DEBUG_LOG_WIN
class DebugLogWindow:public gfl2::base::Singleton<DebugLogWindow>
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugLogWindow);
public:

  DebugLogWindow(gfl2::heap::HeapBase* pHeap ,gfl2::util::DrawUtilText* pTextUtil , gfl2::util::UtilTextRenderingPipeLine* pPipeLine );
  ~DebugLogWindow();

  void      Update(void);
//  void      Draw(gfl2::grp::GraphicsSystem *grpSys );
  void      Draw( gfl2::gfx::CtrDisplayNo dispNo , gfl2::gfx::DisplayDeviceHandle handle );

  void      AddMsg( const LogWinID logIdx, const wchar_t *msg , ...);

  wchar_t*  GetTempBuf(void){return mTempBuf;}

  static const int    MSG_BUF_LEN        = 64;                   /**<  行配列サイズ          */
  static const int    MSG_BUF_CHR_MAX    = MSG_BUF_LEN-1;        /**<  行あたりの文字数最大  */
  static const int    MSG_BUF_NUM        =  8;                   /**<  行数                  */
  static const int    MSG_DISP_TIME      = 30*5;                 /**<  ?                    */



  void  ClearLog(void);

  void  SetIsEnable(const bool flg)   {mIsEnable = flg;}

  int*  GetLifeTimePointer(void)      {return &mLifeTime;}
  int*  GetDispPosPointer(void)       {return &mDispPos;}
  bool* GetIsEnablePointer(void)      {return &mIsEnable;}

  bool  GetIsDispFlg( const LogWinID idx );
  void  SetIsDispFlg( const LogWinID idx , bool flg );


  /*  font  */
  void  SetFont(gfl2::str::Font* pFont)   {m_FontInfo.pFont  = pFont;}
  void  SetFontScale(const f32 scaleH = 1.0f, const f32 scaleV = 1.0f)
        {
          m_FontInfo.scaleH = scaleH;
          m_FontInfo.scaleV = scaleV;
        }


  //SD出力用
  //出力グループの設定・取得
  bool  GetIsOutputSdFlg( const LogWinID idx );
  void  SetIsOutputSdFlg( const LogWinID idx , bool flg );
  //SD出力の切り替え(Updateが1回呼ばれないと有効になりません！)
  void  SetIsEnableSdOutput(bool flg)             {mIsEnableOutptSd = flg;}
  //ログの出力(グループのチェックはAddMSgで)
  void  OutputSdLog( const wchar_t *msg , ... );
  //時刻の表示設定
  void  SetOutputSdLogTime(bool flg)              {mOutputSdTime = flg;}

  //デバッグウィンドウ用
  int* GetDebugWinPageIdx(void)                   {return &mPageIdx;}
//  const wchar_t* GetDebugWinMsgData(const LogWinID idx);
  bool* GetOutputSdLogTime(void)                  {return &mOutputSdTime;}
  bool* GetEnableOutputSd(void)                   {return &mIsEnableOutptSd;}


private:

  /** 行情報  */
  typedef struct
  {
    wchar_t   str[MSG_BUF_LEN];
    int       cnt;
    int       strLen;   /**<  行描画幅  */
    int       strV;     /**<  行描画高  */
    LogWinID  logIdx;
  }MsgBuffer;

  gfl2::heap::HeapBase*       mHeapMem;

  MsgBuffer                   mBuf[MSG_BUF_NUM];
  int                         mDispStart;
  int                         mDispEnd;

  bool                        mIsEnable;
  int                         mLifeTime;
  int                         mDispPos;



  /*  font  */
  struct FontInfo
  {
    gfl2::str::Font*  pFont;
    f32               scaleH;
    f32               scaleV;
    
    FontInfo(void)
      : pFont(NULL)
      , scaleH(1.0f)
      , scaleV(1.0f)
    {}
  }m_FontInfo;

  /*  render  */
  gfl2::util::DrawUtilText*               m_pTextUtil;


  wchar_t                     mTempBuf[MSG_BUF_LEN];

  gfl2::base::Bit             mFlgBit;

  //SD用
  
  gfl2::fs::DirectSdmcFile*   mSdFile;
  gfl2::base::Bit             mFlgSdBit;
  bool                        mIsEnableOutptSd;
  bool                        mOutputSdTime;

  //デバッグウィンドウ用
  int                         mPageIdx;
//  gfl2::str::MsgData*         mMsgData;
  gfl2::str::StrBuf           mStrBuf;
    gfl2::util::UtilTextRenderingPipeLine*  m_pTextRenderPipe;
};
#endif //USE_DEBUG_LOG_WIN




} /*  namespace debug */
} /*  namespace gfl2  */
#if USE_DEBUG_LOG_WIN
SINGLETON_INSTANCE_DECLARATION( gfl2::debug::DebugLogWindow );
#endif
#endif  /*  #if !defined(GFL2_DEBUGLOGWINDOW_H_INCLUDED)  */
