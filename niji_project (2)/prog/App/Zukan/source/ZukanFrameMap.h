#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEMAP_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEMAP_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanFrameMap.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の分布画面のフレーム
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Util/AppRenderingManager.h>

#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Frame/AppFrameListener.h>
#include <AppLib/include/Frame/AppFrameCellSuper.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanDataManager.h"
#include "ZukanFrameSceneBase.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TownMap)
class TownMapPartsDraw;
GFL_NAMESPACE_END(TownMap)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
class ZukanTownMapListener; 
class ZukanViewMap;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanFrameMapCell
 @brief     図鑑の分布画面のフレームのセル
 */
//==============================================================================
class ZukanFrameMapCell : public ZukanFrameSceneBaseCell
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanFrameMapCell);

public:
  ZukanFrameMapCell(void);
  virtual ~ZukanFrameMapCell();

  virtual applib::frame::Result InitFunc(void);
  virtual applib::frame::Result UpdateFunc(void);
  virtual void                  DrawFunc(gfl2::gfx::CtrDisplayNo no);
  virtual applib::frame::Result EndFunc(void);
 
public:
  virtual void Req(s32 reqId);

public:
  // Req関数に渡すリクエストID
  enum
  {
    REQ_ID_END
  };

private:
  static const u32 TOWN_MAP_PARTS_DRAW_HEAP_MEM_SIZE =  0x2A000;
  static const u32 TOWN_MAP_PARTS_DRAW_DEV_MEM_SIZE  = 0x3FF000;

protected:
  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  // メモリ
  app::util::Heap*  m_townMapPartsDrawAppHeap;

  // ビュー
  ZukanViewMap*     m_view;
  bool              m_endReq;

  // タウンマップ
  ZukanTownMapListener*             m_townMapListener;
  App::TownMap::TownMapPartsDraw*   m_townMapPartsDraw;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEMAP_H_INCLUDED

