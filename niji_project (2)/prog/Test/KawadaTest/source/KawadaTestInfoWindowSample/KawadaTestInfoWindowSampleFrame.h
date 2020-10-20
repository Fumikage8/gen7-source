#if !defined(NIJI_PROJECT_TEST_KAWADATESTINFOWINDOWSAMPLEFRAME_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATESTINFOWINDOWSAMPLEFRAME_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    KawadaTestInfoWindowSampleFrame.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.07.01
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Frame/AppFrameListener.h>
#include <AppLib/include/Frame/AppFrameCellSuper.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)
  class MsgData; 
  class StrBuf;
GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(GameSys)
  class GameManager;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
  class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
  class MenuWindow;
  class InfoWindowSheetType;
  class InfoWindowBookType;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(print)
  class WordSet;
GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)
  class KawadaTestInfoWindowSampleDataManager;
  class KawadaTestInfoWindowSampleView;
  class KawadaTestInfoWindowSampleViewListener;
  class KawadaTestInfoWindowSampleMenuWindowListener;
  class KawadaTestInfoWindowSampleInfoWindowSheetTypeListener;
  class KawadaTestInfoWindowSampleInfoWindowBookTypeListener;
GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
/**
 @class     KawadaTestInfoWindowSampleFrameListener
 @brief     サンプルのフレームのリスナー
 */
//==============================================================================
class KawadaTestInfoWindowSampleFrameListener : public applib::frame::AppFrameListener
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestInfoWindowSampleFrameListener);

public:
  KawadaTestInfoWindowSampleFrameListener(void);
  virtual ~KawadaTestInfoWindowSampleFrameListener();

public:
  virtual void EndFuncFinish( applib::frame::CellSuper* pEndCell );
};




//==============================================================================
/**
 @class     KawadaTestInfoWindowSampleFrameCell
 @brief     サンプルのフレームのセル
 */
//==============================================================================
class KawadaTestInfoWindowSampleFrameCell : public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestInfoWindowSampleFrameCell);

public:
  class Param
  {
  public:
    app::util::Heap*                       appHeap;
    GameSys::GameManager*                  gameManager;
    KawadaTestInfoWindowSampleDataManager* dataManager;
    applib::frame::Manager*                frameManager;
    app::util::AppRenderingManager*        appRenderingManager;

  public:
    Param(void)
      : appHeap(NULL),
        gameManager(NULL),
        dataManager(NULL),
        frameManager(NULL),
        appRenderingManager(NULL)
    {}
  };

public:
  KawadaTestInfoWindowSampleFrameCell(void);
  virtual ~KawadaTestInfoWindowSampleFrameCell();

  virtual applib::frame::Result InitFunc(void);
  virtual applib::frame::Result UpdateFunc(void);
  virtual void                  DrawFunc(gfl2::gfx::CtrDisplayNo no);
  virtual applib::frame::Result EndFunc(void);
 
public:
  //! @brief  引数を渡す
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  void Setup(const Param& param);

  //! @brief  ビューから呼び出される終了リクエスト
  void ReqEnd(void);
  //! @brief  ビューから呼び出されるMenuWindowオープンリクエスト
  void ReqMenuWindowOpen(void);
  //! @brief  ビューから呼び出されるInfoWindowSheetTypeオープンリクエスト
  void ReqInfoWindowSheetTypeOpen(const gfl2::math::Vector2& touchPos);
  //! @brief  ビューから呼び出されるBookWindowSheetTypeオープンリクエスト
  void ReqInfoWindowBookTypeOpen(const gfl2::math::Vector2& touchPos);
  
  //! @brief  MenuWindowビューから呼び出されるクローズリクエスト
  void ReqMenuWindowClose(u32 selectId);
  //! @brief  InfoWindowSheetTypeビューから呼び出されるクローズリクエスト
  void ReqInfoWindowSheetTypeClose(void);
  //! @brief  InfoWindowBookTypeビューから呼び出されるクローズリクエスト
  void ReqInfoWindowBookTypeClose(void);

  KawadaTestInfoWindowSampleView* GetView(void) const               { return m_view; }
  app::tool::InfoWindowBookType*  GetInfoWindowBookType(void) const { return m_infoWindowBookType; }
  print::WordSet*                 GetWordSet(void) const            { return m_wordSet; }
  gfl2::str::StrBuf*              GetStrBuf(u32 i) const            { return m_strBuf[i]; }
  gfl2::str::MsgData*             GetMsgData(void) const            { return m_msgData; }

private:
  void changeSeq(int seq);

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  Param  m_param;

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  // フェード色
  gfl2::math::Vector4  m_fadeColor;

  // seq
  u32   m_seqCount;

  // ビュー
  KawadaTestInfoWindowSampleView*         m_view;
  KawadaTestInfoWindowSampleViewListener* m_viewListener;
  bool                                    m_endReq;

  // MenuWindowビュー
  app::tool::MenuWindow*                        m_menuWindow;
  KawadaTestInfoWindowSampleMenuWindowListener* m_menuWindowListener;
  bool                                          m_menuWindowOpenReq;
  bool                                          m_menuWindowCloseReq;
  u32                                           m_menuWindowSelectId;

  // InfoWindowSheetTypeビュー
  app::tool::InfoWindowSheetType*                         m_infoWindowSheetType;
  KawadaTestInfoWindowSampleInfoWindowSheetTypeListener*  m_infoWindowSheetTypeListener;
  gfl2::math::Vector2                                     m_touchPosForInfoWindowSheetType;  // タッチパネル座標
  bool                                                    m_infoWindowSheetTypeOpenReq;
  bool                                                    m_infoWindowSheetTypeCloseReq;
  
  // InfoWindowBookTypeビュー
  app::tool::InfoWindowBookType*                          m_infoWindowBookType;
  KawadaTestInfoWindowSampleInfoWindowBookTypeListener*   m_infoWindowBookTypeListener;
  gfl2::math::Vector2                                     m_touchPosForInfoWindowBookType;  // タッチパネル座標
  bool                                                    m_infoWindowBookTypeOpenReq;
  bool                                                    m_infoWindowBookTypeCloseReq;
 
  // メッセージ
  static const u32 STR_BUF_NUM          = 14;
  static const u32 STR_BUF_CHAR_NUM_MAX = 512;
  print::WordSet*      m_wordSet;
  gfl2::str::StrBuf*   m_strBuf[STR_BUF_NUM];
  gfl2::str::MsgData*  m_msgData;

  ////////////////////////////////////////////////fileTest
  // ファイル読み込みテスト
  s32 m_fileTestStep;
  void* m_fileTestBuf[8];
  ////////////////////////////////////////////////fileTest
};


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


#endif  // NIJI_PROJECT_TEST_KAWADATESTINFOWINDOWSAMPLEFRAME_H_INCLUDED

