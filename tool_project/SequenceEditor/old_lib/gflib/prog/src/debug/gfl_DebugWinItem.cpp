//======================================================================
/**
 * @file	gfl_DebugwinItem.cpp
 * @brief デバッグウィンドウ用アイテム(項目)
 * @author	ariizumi
 * @data	11/03/29
 */
//======================================================================

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <gflib.h>
#include <debug/gfl_DebugWinItem.h>
#include <debug/gfl_DebugWinSystem.h>

#if GFL_DEBUGWIN_USE

namespace gfl
{
namespace debug
{
  
DebugWinItem::DebugWinItem( DebugWinGroup *parent )
:mParentGroup(parent)
{
  SetName( L"DummyItem" );
}

DebugWinItem::DebugWinItem( DebugWinGroup *parent , const wchar_t *name )
:mParentGroup(parent)
{
  SetName( name );
}

DebugWinItem::~DebugWinItem()
{
  //DebugWinで解放処理(現在グループだったり、選択アイテムだったりとかの解除)
  GFL_SINGLETON_INSTANCE(DebugWin)->RemoveItemFunc(this);
  //親から離脱(Topグループのみ親が居ない)
  if( mParentGroup != NULL )
  {
    mParentGroup->GetItemList()->erase(this);
  }
}


void DebugWinItem::SetName( const wchar_t *name )
{
  //一応クリア
  std::MemClear( mName , sizeof(wchar_t)*DEBUGWIN_NAME_LEN );
  ::std::wcsncpy( mName , name , DEBUGWIN_NAME_LEN );
}

void DebugWinItem::SetNameV( wchar_t *name , ... )
{
  //一応クリア
  std::MemClear( mName , sizeof(wchar_t)*DEBUGWIN_NAME_LEN );

  va_list VA;
  va_start( VA, name );
  vswprintf( mName, DEBUGWIN_NAME_LEN, name, VA );
  va_end( VA );

}

///SEの再生
void DebugWinItem::PlaySound( DEBUGWIN_SOUND_TYPE type )
{
  GFL_SINGLETON_INSTANCE(DebugWin)->PlaySound(type);
}

wchar_t * DebugWinItem::GetSysWorkStr( void )
{
  return GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->GetWorkStr();
}

gfl::ui::Button * DebugWinItem::GetSysUiButton()
{
  return GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->GetUiButton();
}

gfl::ui::VectorDevice * DebugWinItem::GetSysUiCrossKey()
{
  return GFL_SINGLETON_INSTANCE(gfl::debug::DebugWin)->GetUiCrossKey();
}


//---------------------------------------------------------
DebugWinGroup::DebugWinGroup( DebugWinGroup *parent , const wchar_t *name )
:DebugWinItem(parent)
,mGroupWork(NULL)
,mOpenGroupFunc(NULL)
{
  SetName( name );
}

DebugWinGroup::~DebugWinGroup()
{
  //グループ管理リストから離脱
  GFL_SINGLETON_INSTANCE(DebugWin)->RemoveGroupList(this);
  
  //子をすべて破棄
  DebugWinItemIt it = mItemList.begin();
  while( it != mItemList.end() )
  {
    DebugWinItem *item = &(*it);
    it++;
    
    GFL_DELETE item;
  }
  //ワークがあれば解放
  if( mGroupWork )
  {
    GflHeapFreeMemory( mGroupWork );
  }
}

void DebugWinGroup::AddItem( DebugWinItem *item )
{
  //子の追加
  mItemList.push_back( item );
}

void DebugWinGroup::RemoveItem( DebugWinItem *item )
{
  //子の削除
  GFL_DELETE item;
}

void DebugWinGroup::UpdateFunc(DebugWin *debSys)
{
  gfl::ui::Button *uiButton = debSys->GetDeviceManager()->GetButton(0);
  //ボタンが押されたら、グループに入る
  if( uiButton->IsTrigger( gfl::ui::BUTTON_A ) )
  {
    debSys->SetCurrentGroup( this , NULL );
    if( mOpenGroupFunc )
    {
      mOpenGroupFunc( mGroupWork );
    }
    debSys->SoundCallBack(SE_GROUP_IN);
  }
}

void* DebugWinGroup::CreateGroupWork( const size_t size , heap::HeapBase *heap )
{
  if( mGroupWork )
  {
    GFL_ASSERT_MSG( mGroupWork == NULL ,"DebugWinGroup:Work already exists!" ); //@check
    return mGroupWork;
  }
  
  //グループが保持するワークを作成
  //主にライブラリで用意した簡易セットのワークを保存
  mGroupWork = GflHeapAllocMemory( (heap ? heap : GFL_SINGLETON_INSTANCE(DebugWin)->GetHeap() ) , size );
  return mGroupWork;
}

//---------------------------------------------------------
DebugWinFunc::DebugWinFunc( DebugWinGroup *parent , const wchar_t *name , void *userWork , DebugWinUpdateFunc updateFunc , DebugWinDrawFunc drawFunc )
  :DebugWinItem( parent , name )
  ,mUpdateFunc(updateFunc)
  ,mDrawFunc(drawFunc)
  ,mUserWork(userWork)
{
  
}

void DebugWinFunc::UpdateFunc(DebugWin *debSys)
{
  GFL_UNUSED( debSys );
  if( mUpdateFunc )
  {
    //更新
    mUpdateFunc( mUserWork , this );
  }
}

const wchar_t* DebugWinFunc::GetDispName(DebugWin *debSys)
{
  if( mDrawFunc )
  {
    //描画
    //中でSetNameVとかで設定してもらう。
    return mDrawFunc( mUserWork , this );
  }
  
  return DebugWinItem::GetDispName(debSys);
}

}//namespace debug
}//namespace gfl

#endif //GFL_DEBUGWIN_USE
