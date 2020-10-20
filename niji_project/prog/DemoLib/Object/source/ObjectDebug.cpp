//======================================================================
/**
 * @file  ObjectDebug.cpp
 * @brief オブジェクトデバッグ用
 * @author  ariizumi
 * @data  15/03/16
 */
//======================================================================

#if PM_DEBUG
#include <debug/include/gfl2_DebugWinItemTypes.h>
#include <DemoLib/Object/include/Object.h>

#include <DemoLib/Object/include/ObjectDebug.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

void DebuwinObjectCallback_Visible( void *work )
{
  Object *obj = static_cast<Object*>(work);
  //フラグ直操作なので設定しなおす
  obj->SetVisible(obj->GetVisible());
}

void DebuwinObjectCallback_Update( void *work )
{
  Object *obj = static_cast<Object*>(work);
  obj->SetIsUpdate(true);
}

gfl2::debug::DebugWinGroup* AddDebugWin_Object(gfl2::heap::HeapBase *heap, gfl2::debug::DebugWinGroup* parent, Object *obj,wchar_t *name)
{
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData* pMsgData = devWork->GetMessageData();

  gfl2::debug::DebugWinGroup* group = gfl2::debug::DebugWin_AddGroup(heap,name,parent);

  gfl2::debug::DebugWinItem* item;
  item = gfl2::debug::DebugWin_AddItemValueVector3(group,heap, pMsgData,msg_dmenu_battle_obj_position,obj->GetPositionPointer(),-5000.0f,5000.0f);
  item->SetChangedValueFunc(DebuwinObjectCallback_Update,obj);
  item = gfl2::debug::DebugWin_AddItemValueVector3(group,heap, pMsgData,msg_dmenu_battle_obj_offset,obj->GetOffsetPointer(),-1000.0f,1000.0f);
  item->SetChangedValueFunc(DebuwinObjectCallback_Update,obj);
  item = gfl2::debug::DebugWin_AddItemValueVector3(group,heap, pMsgData,msg_dmenu_battle_obj_scale,obj->GetScalePointer(),-1000.0f,1000.0f,0.1f);
  item->SetChangedValueFunc(DebuwinObjectCallback_Update,obj);
  item = gfl2::debug::DebugWin_AddItemValueVector3(group,heap, pMsgData,msg_dmenu_battle_obj_rotate,obj->GetRotatePointer(),-360.0f,360.0f,0.1f);
  item->SetChangedValueFunc(DebuwinObjectCallback_Update,obj);
  item = gfl2::debug::DebugWin_AddItemValueVector3(group,heap, pMsgData,msg_dmenu_battle_obj_ads_scale,obj->GetAdjustScalePointer(),-1000.0f,1000.0f,0.1f);
  item->SetChangedValueFunc(DebuwinObjectCallback_Update,obj);
  item = gfl2::debug::DebugWin_AddItemValueVector3(group,heap, pMsgData,msg_dmenu_battle_obj_rot_ofs,obj->GetRotateOffsetPointer(),-1000.0f,1000.0f,0.1f);
  item->SetChangedValueFunc(DebuwinObjectCallback_Update,obj);

  item = gfl2::debug::DebugWin_AddItemValueBool(group,heap, pMsgData,msg_dmenu_battle_obj_visible,obj->GetVisiblePointer());
  item->SetChangedValueFunc(DebuwinObjectCallback_Visible,obj);

  return group;
}


GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)

#endif //PM_DEBUG