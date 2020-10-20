//======================================================================
/**
 * @file	gfl_DebugwinItem.cpp
 * @brief �f�o�b�O�E�B���h�E�p�A�C�e��(����)
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
  //DebugWin�ŉ������(���݃O���[�v��������A�I���A�C�e����������Ƃ��̉���)
  GFL_SINGLETON_INSTANCE(DebugWin)->RemoveItemFunc(this);
  //�e���痣�E(Top�O���[�v�̂ݐe�����Ȃ�)
  if( mParentGroup != NULL )
  {
    mParentGroup->GetItemList()->erase(this);
  }
}


void DebugWinItem::SetName( const wchar_t *name )
{
  //�ꉞ�N���A
  std::MemClear( mName , sizeof(wchar_t)*DEBUGWIN_NAME_LEN );
  ::std::wcsncpy( mName , name , DEBUGWIN_NAME_LEN );
}

void DebugWinItem::SetNameV( wchar_t *name , ... )
{
  //�ꉞ�N���A
  std::MemClear( mName , sizeof(wchar_t)*DEBUGWIN_NAME_LEN );

  va_list VA;
  va_start( VA, name );
  vswprintf( mName, DEBUGWIN_NAME_LEN, name, VA );
  va_end( VA );

}

///SE�̍Đ�
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
  //�O���[�v�Ǘ����X�g���痣�E
  GFL_SINGLETON_INSTANCE(DebugWin)->RemoveGroupList(this);
  
  //�q�����ׂĔj��
  DebugWinItemIt it = mItemList.begin();
  while( it != mItemList.end() )
  {
    DebugWinItem *item = &(*it);
    it++;
    
    GFL_DELETE item;
  }
  //���[�N������Ή��
  if( mGroupWork )
  {
    GflHeapFreeMemory( mGroupWork );
  }
}

void DebugWinGroup::AddItem( DebugWinItem *item )
{
  //�q�̒ǉ�
  mItemList.push_back( item );
}

void DebugWinGroup::RemoveItem( DebugWinItem *item )
{
  //�q�̍폜
  GFL_DELETE item;
}

void DebugWinGroup::UpdateFunc(DebugWin *debSys)
{
  gfl::ui::Button *uiButton = debSys->GetDeviceManager()->GetButton(0);
  //�{�^���������ꂽ��A�O���[�v�ɓ���
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
  
  //�O���[�v���ێ����郏�[�N���쐬
  //��Ƀ��C�u�����ŗp�ӂ����ȈՃZ�b�g�̃��[�N��ۑ�
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
    //�X�V
    mUpdateFunc( mUserWork , this );
  }
}

const wchar_t* DebugWinFunc::GetDispName(DebugWin *debSys)
{
  if( mDrawFunc )
  {
    //�`��
    //����SetNameV�Ƃ��Őݒ肵�Ă��炤�B
    return mDrawFunc( mUserWork , this );
  }
  
  return DebugWinItem::GetDispName(debSys);
}

}//namespace debug
}//namespace gfl

#endif //GFL_DEBUGWIN_USE
