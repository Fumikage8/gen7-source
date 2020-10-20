//======================================================================
/**
 * @file  gfl_DebugwinItem.h
 * @brief �f�o�b�O�E�B���h�E�p�A�C�e��(����)
 * @author  ariizumi
 * @date  11/03/29
 */
//======================================================================

#if !defined( __GFL_DEBUGWIN_ITEM_H__ )
#define __GFL_DEBUGWIN_ITEM_H__
#pragma once

#include <debug/gfl_DebugwinTypes.h>

#include <list>

//��`��gfl_DebugWinTypes.h �ɂ���܂��B
//#define GFL_DEBUGWIN_USE 1

namespace gfl
{

namespace ui
{
  class Button;
  class VectorDevice;
}

namespace debug
{

#if GFL_DEBUGWIN_USE

///���ڂ̕\�����O�̍ő�T�C�Y
static const int DEBUGWIN_NAME_LEN = 64;

class DebugWin;
class DebugWinGroup;

//---------------------------------------------------------
/**
 * @class DebugWinItem
 * @brief �f�o�b�O�E�B���h�E���ڊ��N���X
 */
//---------------------------------------------------------
class DebugWinItem
{
public:
  //�R���X�g���N�^
  DebugWinItem( DebugWinGroup *parent );
  DebugWinItem( DebugWinGroup *parent , const wchar_t *name );
  //�f�X�g���N�^
  virtual ~DebugWinItem();

  ///�J�[�\���A�N�e�B�u���̍X�V
  virtual inline void UpdateFunc(DebugWin *debSys){GFL_UNUSED(debSys);}
  ///�\�����擾
  virtual const wchar_t* GetDispName(DebugWin *debSys){GFL_UNUSED(debSys);return mName;}
  
  ///�e�O���[�v�̎擾
  inline DebugWinGroup* GetParentGroup(void){return mParentGroup;}
  ///���O�̐ݒ�
  void SetName( const wchar_t *name );
  void SetNameV( wchar_t *name , ... );
  ///���O�̑f�̕�����擾(�����ȂǗp)
  inline wchar_t* GetName( void ) {return mName;}
  
  ///SE�̍Đ�
  void PlaySound( DEBUGWIN_SOUND_TYPE type );
  
  nw::ut::LinkListNode mItemNode;

protected:
  ///�e�̐ݒ�
  inline void SetParentGroup( DebugWinGroup *parent ){mParentGroup = parent;}

  /// �f�o�b�O�E�B���h�E�V�X�e�����當���񏈗��p�̃��[�N���擾����
  wchar_t * GetSysWorkStr( void );
  /// �f�o�b�O�E�B���h�E�V�X�e������{�^�������擾����
  gfl::ui::Button * GetSysUiButton();
  /// �f�o�b�O�E�B���h�E�V�X�e������\���L�[�����擾����
  gfl::ui::VectorDevice * GetSysUiCrossKey();

private:
  ///�e�O���[�v�ւ̃|�C���^
  DebugWinGroup* mParentGroup;
  ///���O������
  wchar_t mName[DEBUGWIN_NAME_LEN];
  
};

typedef nw::ut::LinkList<DebugWinItem,offsetof(DebugWinItem,mItemNode)> DebugWinItemList;
typedef DebugWinItemList::Iterator DebugWinItemIt;

//std::list�p
//typedef ::std::list<DebugWinItem*> DebugWinItemList;
//typedef DebugWinItemList::iterator DebugWinItemIt;

//---------------------------------------------------------

//�O���[�v�N���X
typedef void(*OpenGroupFunc)( void *work );

//---------------------------------------------------------
/**
 * @class DebugWinGroup
 * @brief �f�o�b�O�E�B���h�E���ڃO���[�v�N���X
 */
//---------------------------------------------------------
class DebugWinGroup : public DebugWinItem
{
public:
  //�R���X�g���N�^
  DebugWinGroup( DebugWinGroup *parent , const wchar_t *name );
  //�f�X�g���N�^
  virtual ~DebugWinGroup();

  //�J�[�\���A�N�e�B�u���̍X�V
  virtual void UpdateFunc(DebugWin *debSys);
  
  //�q�̒ǉ�
  void AddItem( DebugWinItem *item );
  
  //�q�̍폜
  void RemoveItem( DebugWinItem *item );
  
  //�q���X�g�̎擾
  inline DebugWinItemList* GetItemList(){return &mItemList;}
  
  //�O���[�v�̃��[�N�̍쐬
  //�O���[�v�폜���ɉ������܂�
  void* CreateGroupWork( const size_t size , heap::HeapBase *heap = NULL );
  
  void SetOpenGroupFunc( OpenGroupFunc func ){mOpenGroupFunc = func;}
  
  nw::ut::LinkListNode mGrpNode;

private:
  //�q���X�g
  DebugWinItemList mItemList;
  //���[�N
  void *mGroupWork;
  
  OpenGroupFunc mOpenGroupFunc;
};

//---------------------------------------------------------

//���p�̍X�V���E�`�掞�N���X
typedef void (*DebugWinUpdateFunc)( void* userWork  , DebugWinItem *item );
typedef wchar_t* (*DebugWinDrawFunc)( void* userWork  , DebugWinItem *item );

//---------------------------------------------------------
/**
 * @class DebugWinFunc
 * @brief �X�V�����Ǝ��ݒ�p���ڃN���X
 */
//---------------------------------------------------------
class DebugWinFunc : public DebugWinItem
{
public:
  //�R���X�g���N�^
  DebugWinFunc( DebugWinGroup *parent , const wchar_t *name , void *userWork , DebugWinUpdateFunc updateFunc , DebugWinDrawFunc drawFunc = NULL );
  //�f�X�g���N�^
  virtual ~DebugWinFunc(){};
  
  //�X�V(DebugWinUpdateFunc)
  virtual void UpdateFunc(DebugWin *debSys);
  //�\�����̎擾(DebugWinDrawFunc)
  virtual const wchar_t* GetDispName(DebugWin *debSys);
  
private:
  DebugWinUpdateFunc mUpdateFunc;
  DebugWinDrawFunc mDrawFunc;
  void *mUserWork;
};

#endif //GFL_DEBUGWIN_USE

}//namespace debug
}//namespace gfl




#endif  //__GFL_DEBUGWIN_ITEM_H__

