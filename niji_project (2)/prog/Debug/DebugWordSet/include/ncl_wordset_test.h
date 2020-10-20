#if PM_DEBUG

//======================================================================
/**
 * @file   ncl_wordset_test.h
 * @brief	 EffectViewer
 * @author ichibayashi_hironori@Nintendo
 * @date   2013/01/08
 */
//======================================================================

#ifndef __NCL_MSG_TEST_H__
#define __NCL_MSG_TEST_H__

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include "GameSys/include/GameProc.h"
#include "GameSys/include/GameManager.h"
#include "AppLib/include/ui/UIView.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)

  class MsgData;

GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class Heap;
  class AppRenderingManager;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN( test )
GFL_NAMESPACE_BEGIN( ncl )

class WordsetTestView;

class WordsetTest : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(WordsetTest);

public:
  WordsetTest();

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);
  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメインを行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);
  virtual void DrawFunc(gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo);

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

private:
  enum TestType
  {
    TESTTYPE_ITEMNAME,
    TESTTYPE_TRTYPEANDNAME,
    TESTTYPE_TRTYPENAME,
    TESTTYPE_POKEMONNAME,
    TESTTYPE_TYPENAME,
    TESTTYPE_ARTICLE_01,
    TESTTYPE_ARTICLE_02,
    TESTTYPE_ARTICLE_03,
    TESTTYPE_ARTICLE_04,
    TESTTYPE_ARTICLE_05,
    TESTTYPE_RIVALNAME,
    TESTTYPE_DIGITS,
    TESTTYPE_COUNT,
  };

  static int between(int value, int minValue, int maxValue);
  void updateText();
  void registerWords();
  void registerWords_TrTypeAndName();
  void registerWords_ItemName();
  
  static int testWordMax[TESTTYPE_COUNT];
  
  gfl2::heap::HeapBase*  mDeviceMem;

  app::util::Heap*       mUtilHeap;

  // UI
  gfl2::ui::DeviceManager*  mDeviceManager;

  WordsetTestView*  mUIView;

  bool mIsUpdateMessage;
  
  gfl2::str::MsgData* mMsgData;
  gfl2::str::MsgData* mMsgDataItem;
  gfl2::str::MsgData* mMsgDataBag;

  gfl2::str::StrBuf *mStrBuf;
  gfl2::str::StrBuf *mExpandBuf;
  print::WordSet *mWordSet;

  app::util::AppRenderingManager* mpAppRenderingManager;

  void* mLytBuf;
  print::LYT_TEXT_EX_WORK* mLytTextWork;  //!<テキスト表示用ワーク

  int mWordIndex;
  TestType mTestType;
};

GFL_NAMESPACE_END( test )
GFL_NAMESPACE_END( ncl )

#endif //__NCL_MSG_TEST_H__

#endif // PM_DEBUG
