//======================================================================
/**
 * @file  ObjectManager.cpp
 * @brief オブジェクトマネージャー
 * @author  ariizumi
 * @data  15/03/16
 */
//======================================================================

#include <DemoLib/Object/include/ObjectManager.h>
GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

ObjectManager::ObjectManager(gfl2::heap::HeapBase* heap, u32 bufferSize )
:mList(heap,bufferSize)
{

}

ObjectManager::~ObjectManager()
{
}

//オブジェクトの更新
void ObjectManager::Update(int step)
{
  gfl2::util::List<Object*>::Iterator it = mList.Begin();
  while( (*it) != NULL )
  {
    Object *obj = (*it);
    obj->Update(step);
    ++it;
  }
}

void ObjectManager::DrawLayout(gfl2::gfx::CtrDisplayNo displayNo)
{
  gfl2::util::List<Object*>::Iterator it = mList.Begin();
  while( (*it) != NULL )
  {
    Object *obj = (*it);
    obj->DrawLayout(displayNo);
    ++it;
  }
}

//オブジェクトの追加・削除
void ObjectManager::AddObject(Object *obj, int pri )
{
  obj->SetPriority(pri);
  gfl2::util::List<Object*>::Iterator it = mList.Begin();
  while( (*it) != NULL && (*it)->GetPriority() <= pri )
  {
    ++it;
  }

  if( it != NULL )
  {
    mList.Insert( it , obj );
  }
  else
  {
    mList.PushBack(obj);
  }
}

void ObjectManager::RemoveObject(Object *obj)
{
  mList.Remove(obj);
}

//ObjectにセットしたObjIDで取得
Object* ObjectManager::GetObjectById(int id)
{
  gfl2::util::List<Object*>::Iterator it = mList.Begin();
  while( (*it) != NULL )
  {
    Object *obj = (*it);
    if( obj->GetObjId() == id )
    {
      return obj;
    }
    ++it;
  }
  return NULL;
}


GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)