#if PM_DEBUG
//======================================================================
/**
 * @file   ncl_font_viewer.cpp
 * @brief	 フォントの全文字表示デバッグモード
 * @author ichibayashi_hironori@Nintendo
 * @date   2013/05/07
 *
 * @note    sangoから移植
 * @author  Hiroyuki Nakamura
 * @date    2016/06/13
 */
//======================================================================

#ifndef __NCL_FONT_VIEWER_H__
#define __NCL_FONT_VIEWER_H__

#include <str/include/gfl2_Str.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameProc.h"
#include "AppLib/include/Util/app_util_Heap.h"


GFL_NAMESPACE_BEGIN(print)
class WordSet;
GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class G2DUtil;
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(Debug)

class FontViewer : public GameSys::GameProc
{
public:
  FontViewer();
  virtual ~FontViewer();

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
  virtual void PreDrawFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

//  virtual gamesystem::FrameMode GetFrameMode( void ) const;

private:
  enum LytID
  {
    LYTID_UPPER = 0,
    LYTID_LOWER,
    LYTID_MAX
  };

  // 文字インデックスが表す文字を決定するモード
  typedef enum
  {
    INDEXMODE_DEFINED = 0,  // kujiraで定義されている文字
    INDEXMODE_USERINPUT,    // ユーザー入力可能な文字
    INDEXMODE_MAX = INDEXMODE_USERINPUT,
  } IndexMode;

  IndexMode getIndexMode();
  void setIndexMode(IndexMode new_mode);

  int getIndexCount();
  void setIndex(int new_begin);
  void moveIndex(int diff);

  gfl2::str::STRCODE indexToCode(int index);

  void updateText();


  gfl2::heap::HeapBase * mpManagerHeap;    //!< マネージャ用ヒープ

  app::util::Heap mHeap;   //!< ローカルヒープ

  gfl2::heap::HeapBase*  mHeapMem;
  gfl2::heap::HeapBase*  mDeviceMem;
  //各種アロケータ
  gfl2::heap::NwAllocator* mHeapAllocater;
  gfl2::heap::NwAllocator* mDeviceAllocater;

  app::util::AppRenderingManager * mpRenderingManager;   //!< 描画マネージャ

  app::util::G2DUtil * mpG2DUtil;   //!< G2DUtil

  void * mpLytResBuff;  //!< レイアウトリソースバッファ

  // UI
  gfl2::ui::DeviceManager*  mDeviceManager;

  bool mIsUpdateMessage;

  gfl2::str::MsgData* mMsgData;

  gfl2::str::StrBuf *mStrBuf;
  gfl2::str::StrBuf *mExpandBuf;
  print::WordSet *mWordSet;

  IndexMode m_IndexMode;
  int       m_IndexBegin;    // 現在表示している文字の始点
  int       m_IndexEnd;      // 現在表示している文字の終点（含まず）
};


GFL_NAMESPACE_END(Debug)


#endif //__NCL_FONT_VIEWER_H__

#endif // PM_DEBUG
