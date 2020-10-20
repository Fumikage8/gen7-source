//======================================================================
/**
 * @file  gfl2_DebugwinItem.cpp
 * @brief デバッグウィンドウ用アイテム(項目)
 * @author  ariizumi
 * @data  11/03/29
 */
//======================================================================

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <ui/include/gfl2_UI.h>
#include <str/include/gfl2_Str.h>

#include <debug/include/gfl2_DebugWinItem.h>
#include <debug/include/gfl2_DebugWinSystem.h>

#include <debug/include/gfl2_DebugWinLocal.h>

#if GFL_DEBUGWIN_USE

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(debug)


DebugWinItem::DebugWinItem( DebugWinGroup *parent )
:mFunc( NULL )
,mWork( NULL )
,mParentGroup(parent)
,m_iScrId( -1 )
,mIsSelectSkip(false)
{
  SetName( L"DummyItem" );
}

DebugWinItem::DebugWinItem( DebugWinGroup *parent , const wchar_t *name, const s32 iScrId )
:mParentGroup(parent)
,m_iScrId( iScrId )
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
    mParentGroup->GetItemList()->Remove(this);
  }
}


void DebugWinItem::SetName( const wchar_t *name )
{
  //一応クリア
  gfl2::std::MemClear( mName , sizeof(wchar_t)*DEBUGWIN_NAME_LEN );
  gfl2::str::StrNCopy( mName , name , DEBUGWIN_NAME_LEN );
}

void DebugWinItem::SetNameV( wchar_t *name , ... )
{
  //一応クリア
  gfl2::std::MemClear( mName , sizeof(wchar_t)*DEBUGWIN_NAME_LEN );

  va_list VA;
  va_start( VA, name );
  vswprintf( mName, DEBUGWIN_NAME_LEN, name, VA );
  va_end( VA );

}

///SEの再生
void DebugWinItem::PlaySystemSound( DEBUGWIN_SOUND_TYPE type )
{
  GFL_SINGLETON_INSTANCE(DebugWin)->PlaySystemSound(type);
}

wchar_t * DebugWinItem::GetSysWorkStr( void )
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStr();
}

gfl2::ui::Button * DebugWinItem::GetSysUiButton()
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetUiButton();
}

gfl2::ui::VectorDevice * DebugWinItem::GetSysUiCrossKey()
{
  return GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetUiCrossKey();
}

void DebugWinItem::ChangedValue(void)
{
  if( mFunc )
  {
    mFunc( mWork );
  }
}


//---------------------------------------------------------
DebugWinGroup::DebugWinGroup( DebugWinGroup *parent , const wchar_t *name , int itemNum , gfl2::heap::HeapBase *heap )
:DebugWinItem(parent)
,mItemList(heap,itemNum)
,mGroupWork(NULL)
,mOpenGroupFunc(NULL)
,mCreateGroupFunc(NULL)
,mDestroyGroupFunc(NULL)
,mCreateGroupAfterFunc(NULL)
,mWindowOpenGroupFunc(NULL)
,mWindowCloseGroupFunc(NULL)
,muserWork(NULL)
,mWindowUserWork(NULL)
{
  SetName( name );
}

DebugWinGroup::~DebugWinGroup()
{
  //グループ管理リストから離脱
  GFL_SINGLETON_INSTANCE(DebugWin)->RemoveGroupList(this);
  
  //子をすべて破棄
  DebugWinItemIt it = mItemList.Begin();
  while( it != mItemList.End() )
  {
    DebugWinItem *item = (*it);
    ++it;
    
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
  GFL_ASSERT_MSG(mItemList.Size() < mItemList.MaxSize() , "リストサイズがMAXです\n" );
  //子の追加
  mItemList.PushBack( item );
}

void DebugWinGroup::RemoveItem( DebugWinItem *item )
{
  mItemList.Remove( item );

  //子の削除
  GFL_DELETE item;
}

//子の空き数を取得
s32 DebugWinGroup::GetItemFreeSize( void ) const
{
  return mItemList.MaxSize() - mItemList.Size();
}

void DebugWinGroup::UpdateFunc(DebugWin *debSys)
{
  gfl2::ui::Button *uiButton = debSys->GetDeviceManager()->GetButton(0);
  //ボタンが押されたら、グループに入る
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    CallCreateGroupFunc();
    debSys->SetCurrentGroup( this , NULL );
    if( mOpenGroupFunc )
    {
      mOpenGroupFunc( mGroupWork );
    }
    debSys->SoundCallBack(SE_GROUP_IN);
    CallCreateGroupAfterFunc();
  }
}

//カーソルがタッチされたさいの更新
void DebugWinGroup::UpdateTouchFunc(DebugWin *debSys)
{
  CallCreateGroupFunc();
  debSys->SetCurrentGroup( this , NULL );
  if( mOpenGroupFunc )
  {
    mOpenGroupFunc( mGroupWork );
  }
  debSys->SoundCallBack(SE_GROUP_IN);
  CallCreateGroupAfterFunc();
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

void DebugWinGroup::CallCreateGroupFunc( void )
{
  if( mCreateGroupFunc )
  {
    mCreateGroupFunc( muserWork, this );
  }
}
void DebugWinGroup::CallCreateGroupAfterFunc( void )
{
  if( mCreateGroupAfterFunc )
  {
    mCreateGroupAfterFunc( muserWork, this );
  }
}
void DebugWinGroup::CallDestroyGroupFunc( void )
{
  if( mDestroyGroupFunc )
  {
    mDestroyGroupFunc( muserWork, this );
  }
}
void DebugWinGroup::CallWindowOpenGroupFunc( void )
{
  if( mWindowOpenGroupFunc )
  {
    mWindowOpenGroupFunc( mWindowUserWork, this );
  }
}
void DebugWinGroup::CallWindowCloseGroupFunc( void )
{
  if( mWindowCloseGroupFunc )
  {
    mWindowCloseGroupFunc( mWindowUserWork, this );
  }
}

void DebugWinGroup::ResizeItem( int itemNum , gfl2::heap::HeapBase *heap )
{
  RemoveAllItem();

  mItemList.Clear();
  mItemList.CreateBuffer( heap, itemNum );
}

void DebugWinGroup::RemoveAllItem( void )
{
  //子をすべて破棄
  DebugWinItemIt it = mItemList.Begin();
  while( it != mItemList.End() )
  {
    DebugWinItem *item = (*it);
    ++it;
    
    GFL_DELETE item;
  }
}

//---------------------------------------------------------
DebugWinFunc::DebugWinFunc( DebugWinGroup *parent , const wchar_t *name , void *userWork , DebugWinUpdateFunc updateFunc , DebugWinDrawFunc drawFunc, const s32 iScrId )
  :DebugWinItem( parent , name, iScrId )
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

GFL_NAMESPACE_END(debug)
GFL_NAMESPACE_END(gfl2)

#endif //GFL_DEBUGWIN_USE
