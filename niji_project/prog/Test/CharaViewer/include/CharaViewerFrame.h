//======================================================================
/**
 * @file CharaViewerFrame.h
 * @date 2015/07/21 20:10:42
 * @author araki_syo
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined CHARA_VIEWER_FRAME_H_INCLUDED
#define CHARA_VIEWER_FRAME_H_INCLUDED
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Frame/AppFrameListener.h>
#include <AppLib/include/Frame/AppFrameCellSuper.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(GameSys)
  class GameManager;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
  class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)
  class CharaViewerDataManager;
  class CharaViewerView;
  class CharaViewerCharaModelManager;
GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)


//==============================================================================
/**
 @class     CharaViewerFrameListener
 @brief     CharaViewerのフレームのリスナー
 */
//==============================================================================
class CharaViewerFrameListener : public applib::frame::AppFrameListener
{
  GFL_FORBID_COPY_AND_ASSIGN(CharaViewerFrameListener);

public:
  CharaViewerFrameListener(void);
  virtual ~CharaViewerFrameListener();

public:
  virtual void EndFuncFinish( applib::frame::CellSuper* pEndCell );
};




//==============================================================================
/**
 @class     CharaViewerFrameCell
 @brief     CharaViewerのフレームのセル
 */
//==============================================================================
class CharaViewerFrameCell : public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(CharaViewerFrameCell);

public:
  class Param
  {
  public:
    CharaViewerProcParam*                 param;                 // paramのポインタを保持するだけ。
                                                           // 呼び出し元はparamを破棄しないように。
    app::util::Heap*                appHeap;
    GameSys::GameManager*           gameManager;
    CharaViewerDataManager*               dataManager;
    applib::frame::Manager*         frameManager;
    app::util::AppRenderingManager* appRenderingManager;
    CharaViewerCharaModelManager*   charaModelManager;
  public:
    Param(void)
      : param(NULL),
        appHeap(NULL),
        gameManager(NULL),
        dataManager(NULL),
        frameManager(NULL),
        appRenderingManager(NULL),
        charaModelManager(NULL)
    {}
  };

public:
  CharaViewerFrameCell(void);
  virtual ~CharaViewerFrameCell();

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
  CharaViewerView*         m_view;
  bool               m_endReq;

};


GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)


#endif // CHARA_VIEWER_FRAME_H_INCLUDED

#endif // PM_DEBUG
