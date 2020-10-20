//======================================================================
/**
 * @file BtlvUIRotom.h
 * @date 2016/12/01 12:19:15
 * @author ariizumi_nobuhiko
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BTLV_UI_ROTOMO_H__
#define __BTLV_UI_ROTOMO_H__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include <AppLib/include/Util/app_util_heap.h>

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)
class Object_Layout;
GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)


GFL_NAMESPACE_BEGIN(btl)

class BattleViewSystem;

GFL_NAMESPACE_BEGIN(app)

class BtlvUiRotom
{
  //GFL_FORBID_COPY_AND_ASSIGN(BtlvUiRotom);
public:
  BtlvUiRotom(BattleViewSystem *core);

  void StartLoad(void);
  bool IsFinishLoad(void);
  void RemoveLayout(void);
  void Delete(void);

  void StartAnimation(int type);
  void SetData(const BTL_POKEPARAM *bpp);

  void Draw(gfl2::gfx::CtrDisplayNo displayNo);

private:

  void Create(void);

  BattleViewSystem *mBtlvCore;

  bool mIsLoad;
  bool mIsCreate;

  void *mDataBuf;
  ::app::util::Heap *mAppHeap;
  DemoLib::Obj::Object_Layout *mLyt;

};

GFL_NAMESPACE_END(app)
GFL_NAMESPACE_END(btl)

#endif // __BTLV_UI_ROTOMO_H__