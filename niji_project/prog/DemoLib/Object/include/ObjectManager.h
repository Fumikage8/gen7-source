//======================================================================
/**
 * @file	ObjectManager.h
 * @brief	オブジェクトマネージャー
 * @author	ariizumi
 * @data	15/03/16
 */
//======================================================================
#if !defined( __OBJECT_MANAGER_H__ )
#define __OBJECT_MANAGER_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <util/include/gfl2_List.h>
#include <Heap/include/gfl2_Heap.h>

#include <DemoLib/Object/include/Object.h>

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)


class ObjectManager
{
public:
  //コンストラクタ
  //  gfl2::heap::HeapBase* heap  List用ヒープ
  //  u32 bufferSize              管理数
  ObjectManager(gfl2::heap::HeapBase* heap, u32 bufferSize );

  //デストラクタ
  virtual ~ObjectManager();

  //オブジェクトの更新
  void Update(int step=1);

  // Draw Layout
  void DrawLayout(gfl2::gfx::CtrDisplayNo displayNo);

  //オブジェクトの追加・削除
  void AddObject(Object *obj , int pri = OBJ_DEFAULT_PRIORITY );
  void RemoveObject(Object *obj);

  //ObjectにセットしたObjIDで取得
  Object *GetObjectById(int id);

private:

  gfl2::util::List<Object*> mList;
};

GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)


#endif //__OBJECT_MANAGER_H__