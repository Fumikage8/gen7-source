//======================================================================
/**
 * @file  gfl_DebugwinItemTypes.h
 * @brief �f�o�b�O�E�B���h�E�p�A�C�e��(����)�̊g����`
 * @author  ariizumi
 * @data  11/03/29
 */
//======================================================================

#ifndef __GFL_DEBUGWIN_ITEM_TYPES_H__
#define __GFL_DEBUGWIN_ITEM_TYPES_H__
#pragma once

#include <stdio.h>
#include <wchar.h>

//#include <gfl_Grp.h>
#include <gfl_Ui.h>
#include <debug/gfl_DebugWinSystem.h>
#include <debug/gfl_DebugWinItem.h>
#include <debug/gfl_DebugWinLocal.h>

namespace gfl
{

//�N���X��`
namespace grp
{
namespace g3d
{
  class Camera;
  class Model;
  class Light;
}
}

namespace debug
{

#if GFL_DEBUGWIN_USE

//---------------------------------------------------------
/**
 * @brief �f�o�b�O���j���[���ځF���l����p ���e���v���[�g�N���X
 * @tparam  T
 * @note  �����^�Ə����^�ŕ\�����قȂ邩���i��������
 */
//---------------------------------------------------------
template<typename T> class DebugWinItemValue : public DebugWinItem
{
public:
  //���l�X�V���ɌĂ΂��֐��̌^
  typedef void(*ChangedValueFunc)( void *work );

  //�R���X�g���N�^
  DebugWinItemValue( DebugWinGroup *parent , const wchar_t *name , T *target , T min , T max , T step = 1 )
    :DebugWinItem( parent )
    ,mTarget(target)
    ,mMin(min)
    ,mMax(max)
    ,mStep(step)
    ,mFunc( NULL )
    ,mWork( NULL )
  {
    SetName(name);
  }
  
  //���ڑI�����̍X�V�̃��C������
  template<typename SubT>void UpdateValueFunc(DebugWin *debSys, SubT *target , SubT min , SubT max , SubT step )
  {
    gfl::ui::Button *uiButton = this->GetSysUiButton();
    gfl::ui::VectorDevice *uiKey = this->GetSysUiCrossKey();
    
    if( uiButton->IsHold( gfl::ui::BUTTON_R ) )
    {
      step *= 10;
    }
    if( uiButton->IsHold( gfl::ui::BUTTON_L ) )
    {
      step *= 100;
    }
    
    if( uiKey->IsRepeat( gfl::ui::BUTTON_LEFT ) )
    {
      if( *target == min )
      {
        *target = max;
      }
      else
      if( *mTarget - step < min )
      {
        *target = min;
      }
      else
      {
        *target -= step;
      }
      debSys->SoundCallBack(SE_CHANGE_VALUE);
      ChangedValue();
    }
    else
    if( uiKey->IsRepeat( gfl::ui::BUTTON_RIGHT ) )
    {
      if( *target == max )
      {
        *target = min;
      }
      else
      if( *target + step > max )
      {
        *target = max;
      }
      else
      {
        *target += step;
      }
      debSys->SoundCallBack(SE_CHANGE_VALUE);
      ChangedValue();
    }
  }
  
  //���ڑI�����̍X�V
  virtual void UpdateFunc(DebugWin *debSys)
  {
    UpdateValueFunc<T>( debSys,mTarget , mMin , mMax , mStep);
  }
  
  //�^�ɂ���đ���̂ŏ������z��
  virtual const wchar_t* GetDispName(DebugWin *debSys) = 0;

  ///�ύX�ʒm���󂯎��R�[���o�b�N�̓o�^
  inline void SetChangedValueFunc( ChangedValueFunc func , void *work )
  {
    mFunc = func;
    mWork = work;
  }


protected:
  //�ύX��ʒm����
  virtual void ChangedValue(void)
  {
    if( mFunc )
    {
      mFunc( mWork );
    }
  }


  T *mTarget;   ///<����Ώ�
  T mMin;       ///<�ŏ��l
  T mMax;       ///<�ő�l
  T mStep;      ///<�ύX��

  ChangedValueFunc mFunc; ///<�ύX�ʒm���󂯎��R�[���o�b�N�֐�
  void *mWork;            ///<�ύX�ʒm���󂯎��R�[���o�b�N�֐��̃��[�N

};

//---------------------------------------------------------
/**
 * @brief �f�o�b�O���j���[���ځF���l����p�e���v���[�g�N���X�����^
 */
//---------------------------------------------------------
template<typename T> class DebugWinItemValueU64 : public DebugWinItemValue<T>
{
public:
  DebugWinItemValueU64( DebugWinGroup *parent , const wchar_t *name , T *target , T min , T max , T step = 1 )
    :DebugWinItemValue<T>( parent , name , target , min , max , step )
  {
  }

  virtual const wchar_t* GetDispName(DebugWin *debSys)
  {
    wchar_t* workStr = this->GetSysWorkStr();
    swprintf( workStr ,DEBUGWIN_NAME_LEN, L"%ls[%llu]",DebugWinItemValue<T>::GetName() , *DebugWinItemValue<T>::mTarget);
    
    return workStr;
  }
};

//---------------------------------------------------------
/**
 * @brief �f�o�b�O���j���[���ځF���l����p�e���v���[�g�N���X�����^
 */
//---------------------------------------------------------
template<typename T> class DebugWinItemValueU32 : public DebugWinItemValue<T>
{
public:
  DebugWinItemValueU32( DebugWinGroup *parent , const wchar_t *name , T *target , T min , T max , T step = 1 )
    :DebugWinItemValue<T>( parent , name , target , min , max , step )
  {
  }

  virtual const wchar_t* GetDispName(DebugWin *debSys)
  {
    wchar_t* workStr = this->GetSysWorkStr();
    swprintf( workStr ,DEBUGWIN_NAME_LEN, L"%ls[%u]",DebugWinItemValue<T>::GetName() , *DebugWinItemValue<T>::mTarget);

    return workStr;
  }
};

//---------------------------------------------------------
/**
 * @brief �f�o�b�O���j���[���ځF���l����p�e���v���[�g�N���X�����^
 */
//---------------------------------------------------------
template<typename T> class DebugWinItemValueU8 : public DebugWinItemValue<T>
{
public:
  DebugWinItemValueU8( DebugWinGroup *parent , const wchar_t *name , T *target , T min , T max , T step = 1 )
    :DebugWinItemValue<T>( parent , name , target , min , max , step )
  {
  }

  virtual const wchar_t* GetDispName(DebugWin *debSys)
  {
    wchar_t* workStr = this->GetSysWorkStr();
    swprintf( workStr ,DEBUGWIN_NAME_LEN, L"%ls[%d]",DebugWinItemValue<T>::GetName() , *DebugWinItemValue<T>::mTarget);

    return workStr;
  }
};

//---------------------------------------------------------
/**
 * @brief �f�o�b�O���j���[���ځF���l����p�e���v���[�g�N���X�����^
 */
//---------------------------------------------------------
template<typename T> class DebugWinItemValueInt : public DebugWinItemValue<T>
{
public:
  DebugWinItemValueInt( DebugWinGroup *parent , const wchar_t *name , T *target , T min , T max , T step = 1 )
    :DebugWinItemValue<T>( parent , name , target , min , max , step )
  {
  }

  virtual const wchar_t* GetDispName(DebugWin *debSys)
  {
    wchar_t* workStr = this->GetSysWorkStr();
    swprintf( workStr ,DEBUGWIN_NAME_LEN, L"%ls[%d]",DebugWinItemValue<T>::GetName() , *DebugWinItemValue<T>::mTarget);

    return workStr;
  }
};
//---------------------------------------------------------
/**
 * @brief �f�o�b�O���j���[���ځF���l����p�e���v���[�g�N���X�����^
 */
//---------------------------------------------------------
template<typename T> class DebugWinItemValueFloat : public DebugWinItemValue<T>
{
public:

  DebugWinItemValueFloat( DebugWinGroup *parent , const wchar_t *name , T *target , T min , T max , T step = 1 )
    :DebugWinItemValue<T>( parent , name , target , min , max , step )
  {
  }

  virtual const wchar_t* GetDispName(DebugWin *debSys)
  {
    wchar_t* workStr = this->GetSysWorkStr();
    swprintf( workStr ,DEBUGWIN_NAME_LEN, L"%ls[%.2f]",DebugWinItemValue<T>::GetName() , *DebugWinItemValue<T>::mTarget);
    
    return workStr;
  }
};
//---------------------------------------------------------
/**
 * @brief �f�o�b�O���j���[���ځF���l����p�N���XVec3�^(�Ǝ������ǉ�)
 */
//---------------------------------------------------------
class DebugWinItemValueVec3 : public DebugWinItemValue<f32>
{
public:
  typedef void(*ChangedValueFunc)( void *work );

  DebugWinItemValueVec3( DebugWinGroup *parent , const wchar_t *name , gfl::math::VEC3 *target , f32 min , f32 max , f32 step = 1 )
    :DebugWinItemValue<f32>( parent , name , &target->x , min , max , step )
    ,mVecTarget( target )
  {
  }

  virtual const wchar_t* GetDispName(DebugWin *debSys);
  virtual void UpdateFunc(DebugWin *debSys);

protected:

private:
  gfl::math::VEC3 *mVecTarget;
};
//---------------------------------------------------------
/**
 * @brief �f�o�b�O���j���[���ځF���l����p�N���XVec4�^(�Ǝ������ǉ�)
 */
//---------------------------------------------------------
class DebugWinItemValueVec4 : public DebugWinItemValue<f32>
{
public:
  typedef void(*ChangedValueFunc)( void *work );

  DebugWinItemValueVec4( DebugWinGroup *parent , const wchar_t *name , gfl::math::VEC4 *target , f32 min , f32 max , f32 step = 1 )
    :DebugWinItemValue<f32>( parent , name , &target->x , min , max , step )
    ,mVecTarget( target )
  {
  }

  virtual const wchar_t* GetDispName(DebugWin *debSys);
  virtual void UpdateFunc(DebugWin *debSys);

protected:

private:
  gfl::math::VEC4 *mVecTarget;
};
//---------------------------------------------------------
/**
 * @brief �f�o�b�O���j���[���ځF���l����p�N���XColorF32�^(�Ǝ������ǉ�)
 */
//---------------------------------------------------------
class DebugWinItemValueColorF32 : public DebugWinItemValue<f32>
{
public:
  typedef void(*ChangedValueFunc)( void *work );

  DebugWinItemValueColorF32( DebugWinGroup *parent , const wchar_t *name , gfl::grp::ColorF32 *target , f32 min = 0, f32 max = 1, f32 step = 0.1 )
    :DebugWinItemValue<f32>( parent , name , &target->r , min , max , step )
    ,mVecTarget( target )
  {
  }

  virtual const wchar_t* GetDispName(DebugWin *debSys);
  virtual void UpdateFunc(DebugWin *debSys);

protected:

private:
  gfl::grp::ColorF32 *mVecTarget;
};
//---------------------------------------------------------
/**
 * @brief �f�o�b�O���j���[���ځF���l����p�N���X�����^MsgData��(�Ǝ������ǉ�)
 */
//---------------------------------------------------------
class DebugWinItemValueIntMsg : public DebugWinItemValueInt<int>
{
GFL_FORBID_COPY_AND_ASSIGN(DebugWinItemValueIntMsg);
public:
  typedef void(*ChangedValueFunc)( void *work );

  DebugWinItemValueIntMsg( DebugWinGroup *parent , const wchar_t *name , int *target , fs::ArcFile::ARCID arcID, u32 datID , int min, int max , int msgOfs , heap::HeapBase *heap );
  ~DebugWinItemValueIntMsg();

  virtual const wchar_t* GetDispName(DebugWin *debSys);
  
protected:

private:
  
  int mMsgOffset;
  
  int mDispStrNo;
  str::MsgData *mMsgData;
  str::StrBuf *mWorkStr;
};


//---------------------------------------------------------
//�ėp�ǉ��p

//���l�n
//----------------------------------------------------------------------------
/**
 *  @brief  �����^���썀�ڒǉ�
 *
 *  @param  DebugWinGroup *parent �e�O���[�v(NULL�Ȃ�Top�O���[�v�����ǔ񐄏�)
 *  @param  const wchar_t *name   �\����(���O[���l])�ƓW�J����܂�
 *  @param  int *target           ����Ώ�
 *  @param  int min               �ŏ��l
 *  @param  int max               �ő�l
 *  @param  int step              �ړ���(R����������10�{�AL����������100�{�ALR����������1000�{�ő���)
 *
 *  @return DebugWinItemValueInt<int>* �߂�l�ɑ΂���SetChangedValueFunc���ĂԂƐ��l�ϓ����̊֐���ݒ�ł��܂�
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueU64<u64>* DebugWin_AddItemValueU64( DebugWinGroup *parent , const wchar_t *name , u64 *target , u64 min , u64 max , u64 step = 1 , heap::HeapBase *heap = NULL );
extern DebugWinItemValueU64<u64>* DebugWin_AddItemValueU64( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , u64 *target , u64 min , u64 max , u64 step = 1 , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  �����^���썀�ڒǉ�
 *
 *  @param  DebugWinGroup *parent �e�O���[�v(NULL�Ȃ�Top�O���[�v�����ǔ񐄏�)
 *  @param  const wchar_t *name   �\����(���O[���l])�ƓW�J����܂�
 *  @param  int *target           ����Ώ�
 *  @param  int min               �ŏ��l
 *  @param  int max               �ő�l
 *  @param  int step              �ړ���(R����������10�{�AL����������100�{�ALR����������1000�{�ő���)
 *
 *  @return DebugWinItemValueInt<int>* �߂�l�ɑ΂���SetChangedValueFunc���ĂԂƐ��l�ϓ����̊֐���ݒ�ł��܂�
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueU32<u32>* DebugWin_AddItemValueU32( DebugWinGroup *parent , const wchar_t *name , u32 *target , u32 min , u32 max , u32 step = 1 , heap::HeapBase *heap = NULL );
extern DebugWinItemValueU32<u32>* DebugWin_AddItemValueU32( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , u32 *target , u32 min , u32 max , u32 step = 1 , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  �����^���썀�ڒǉ�
 *
 *  @param  DebugWinGroup *parent �e�O���[�v(NULL�Ȃ�Top�O���[�v�����ǔ񐄏�)
 *  @param  const wchar_t *name   �\����(���O[���l])�ƓW�J����܂�
 *  @param  int *target           ����Ώ�
 *  @param  int min               �ŏ��l
 *  @param  int max               �ő�l
 *  @param  int step              �ړ���(R����������10�{�AL����������100�{�ALR����������1000�{�ő���)
 *
 *  @return DebugWinItemValueInt<int>* �߂�l�ɑ΂���SetChangedValueFunc���ĂԂƐ��l�ϓ����̊֐���ݒ�ł��܂�
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueU8<u8>* DebugWin_AddItemValueU8( DebugWinGroup *parent , const wchar_t *name , u8 *target , u8 min , u8 max , u8 step = 1 , heap::HeapBase *heap = NULL );
extern DebugWinItemValueU8<u8>* DebugWin_AddItemValueU8( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , u8 *target , u8 min , u8 max , u8 step = 1 , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  �����^���썀�ڒǉ�
 *
 *  @param  DebugWinGroup *parent �e�O���[�v(NULL�Ȃ�Top�O���[�v�����ǔ񐄏�)
 *  @param  const wchar_t *name   �\����(���O[���l])�ƓW�J����܂�
 *  @param  int *target           ����Ώ�
 *  @param  int min               �ŏ��l
 *  @param  int max               �ő�l
 *  @param  int step              �ړ���(R����������10�{�AL����������100�{�ALR����������1000�{�ő���)
 *
 *  @return DebugWinItemValueInt<int>* �߂�l�ɑ΂���SetChangedValueFunc���ĂԂƐ��l�ϓ����̊֐���ݒ�ł��܂�
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueInt<int>* DebugWin_AddItemValueInt( DebugWinGroup *parent , const wchar_t *name , int *target , int min , int max , int step = 1 , heap::HeapBase *heap = NULL );
extern DebugWinItemValueInt<int>* DebugWin_AddItemValueInt( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , int *target , int min , int max , int step = 1 , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  �����^���썀�ڒǉ�
 *
 *  @param  DebugWinGroup *parent �e�O���[�v(NULL�Ȃ�Top�O���[�v�����ǔ񐄏�)
 *  @param  const wchar_t *name   �\����(���O[���l])�ƓW�J����܂�
 *  @param  f32 *target           ����Ώ�
 *  @param  f32 min               �ŏ��l
 *  @param  f32 max               �ő�l
 *  @param  f32 step              �ړ���(R����������10�{�AL����������100�{�ALR����������1000�{�ő���)
 *
 *  @return DebugWinItemValueFloat<f32>* �߂�l�ɑ΂���SetChangedValueFunc���ĂԂƐ��l�ϓ����̊֐���ݒ�ł��܂�
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueFloat<f32>* DebugWin_AddItemValueFloat( DebugWinGroup *parent , const wchar_t *name , f32 *target , f32 min , f32 max , f32 step = 1 , heap::HeapBase *heap = NULL );
extern DebugWinItemValueFloat<f32>* DebugWin_AddItemValueFloat( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , f32 *target , f32 min , f32 max , f32 step = 1 , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  bool�^���썀�ڒǉ�
 *
 *  @param  DebugWinGroup *parent �e�O���[�v(NULL�Ȃ�Top�O���[�v�����ǔ񐄏�)
 *  @param  const wchar_t *name   �\����(���O[���l])�ƓW�J����܂�
 *  @param  bool *target           ����Ώ�
 *  @param  int min               �ŏ��l
 *  @param  int max               �ő�l
 *  @param  int step              �ړ���(R����������10�{�AL����������100�{�ALR����������1000�{�ő���)
 *
 *  @return DebugWinItemValueInt<bool>* �߂�l�ɑ΂���SetChangedValueFunc���ĂԂƐ��l�ϓ����̊֐���ݒ�ł��܂�
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueInt<bool>* DebugWin_AddItemValueBool( DebugWinGroup *parent , const wchar_t *name , bool *target , heap::HeapBase *heap = NULL );
extern DebugWinItemValueInt<bool>* DebugWin_AddItemValueBool( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , bool *target , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  VEC3�^���썀�ڒǉ�
 *
 *  @param  DebugWinGroup *parent   �e�O���[�v(NULL�Ȃ�Top�O���[�v�����ǔ񐄏�)
 *  @param  const wchar_t *name     �\����(���O[���l])�ƓW�J����܂�
 *  @param  gfl::math::VEC3 *target ����Ώ�
 *  @param  f32 min                 �ŏ��l
 *  @param  f32 max                 �ő�l
 *  @param  f32 step                �ړ���(R����������10�{�AL����������100�{�ALR����������1000�{�ő���)
 *
 *  @return DebugWinItemValueVec3* �߂�l�ɑ΂���SetChangedValueFunc���ĂԂƐ��l�ϓ����̊֐���ݒ�ł��܂�
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueVec3* DebugWin_AddItemValueVec3( DebugWinGroup *parent , const wchar_t *name , gfl::math::VEC3 *target , f32 min , f32 max , f32 step = 1 , heap::HeapBase *heap = NULL );
extern DebugWinItemValueVec3* DebugWin_AddItemValueVec3( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::math::VEC3 *target , f32 min , f32 max , f32 step = 1 , heap::HeapBase *heap = NULL );

//----------------------------------------------------------------------------
/**
 *  @brief  VEC4�^���썀�ڒǉ�
 *
 *  @param  DebugWinGroup *parent   �e�O���[�v(NULL�Ȃ�Top�O���[�v�����ǔ񐄏�)
 *  @param  const wchar_t *name     �\����(���O[���l])�ƓW�J����܂�
 *  @param  gfl::math::VEC4 *target ����Ώ�
 *  @param  f32 min                 �ŏ��l
 *  @param  f32 max                 �ő�l
 *  @param  f32 step                �ړ���(R����������10�{�AL����������100�{�ALR����������1000�{�ő���)
 *
 *  @return DebugWinItemValueVec4* �߂�l�ɑ΂���SetChangedValueFunc���ĂԂƐ��l�ϓ����̊֐���ݒ�ł��܂�
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueVec4* DebugWin_AddItemValueVec4( DebugWinGroup *parent , const wchar_t *name , gfl::math::VEC4 *target , f32 min , f32 max , f32 step = 1 , heap::HeapBase *heap = NULL );
extern DebugWinItemValueVec4* DebugWin_AddItemValueVec4( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::math::VEC4 *target , f32 min , f32 max , f32 step = 1 , heap::HeapBase *heap = NULL );

//----------------------------------------------------------------------------
/**
 *  @brief  ColorF32�^���썀�ڒǉ�
 *
 *  @param  DebugWinGroup *parent   �e�O���[�v(NULL�Ȃ�Top�O���[�v�����ǔ񐄏�)
 *  @param  const wchar_t *name     �\����(���O[���l])�ƓW�J����܂�
 *  @param  gfl::math::VEC4 *target ����Ώ�
 *  @param  f32 min                 �ŏ��l(  0)
 *  @param  f32 max                 �ő�l(  1)
 *  @param  f32 step                �ړ���(0.1)(R����������10�{�AL����������100�{�ALR����������1000�{�ő���)
 *
 *  @return DebugWinItemValueColorF32* �߂�l�ɑ΂���SetChangedValueFunc���ĂԂƐ��l�ϓ����̊֐���ݒ�ł��܂�
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueColorF32* DebugWin_AddItemValueColorF32( DebugWinGroup *parent , const wchar_t *name , gfl::grp::ColorF32 *target , f32 min = 0 , f32 max = 1, f32 step = 0.1 , heap::HeapBase *heap = NULL );
extern DebugWinItemValueColorF32* DebugWin_AddItemValueColorF32( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::grp::ColorF32 *target , f32 min = 0 , f32 max = 1, f32 step = 0.1 , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  �����^���썀�ڒǉ�(Msg�\��)
 *
 *  @param  DebugWinGroup *parent �e�O���[�v(NULL�Ȃ�Top�O���[�v�����ǔ񐄏�)
 *  @param  const wchar_t *name   �\����(���O[���l])�ƓW�J����܂�
 *  @param  int *target           ����Ώ�
 *  @param  int min               �ŏ��l
 *  @param  int max               �ő�l(-1�Ŏ����I�Ƀ��b�Z�[�W�̌���ݒ肵�܂�)
 *  @param  int msgOfs            ���b�Z�[�W�I�t�Z�b�g(�\���ʒu�����炷 ���̒l�{target�̒l��StrId���\������܂�)
 *
 *  @return DebugWinItemValueIntMsg* �߂�l�ɑ΂���SetChangedValueFunc���ĂԂƐ��l�ϓ����̊֐���ݒ�ł��܂�
 *          DebugWinItemValueInt<int>* �ł��󂯎��܂��B
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueIntMsg* DebugWin_AddItemValueIntMsg( DebugWinGroup *parent , const wchar_t *name , int *target , fs::ArcFile::ARCID arcID, u32 datID , int min = 0 , int max = -1 , int msgOfs = 0 , heap::HeapBase *heap = NULL );
extern DebugWinItemValueIntMsg* DebugWin_AddItemValueIntMsg( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , int *target , fs::ArcFile::ARCID arcID, u32 datID , int min = 0 , int max = -1 , int msgOfs = 0 , heap::HeapBase *heap = NULL );

//�N���X�n
//----------------------------------------------------------------------------
/**
 *  @brief  �J�������썀�ڒǉ�
 *          �O���[�v���쐬���A���Ŏ��_�A�����_�Anear�Afar�̑��삪�ł��܂��B
 *          GetValue��I������ƌ��ݒl���擾�ł��܂��B
 *          �폜����ۂ�name���g���č폜�ł��܂�
 *
 *  @param  DebugWinGroup *parent         �e�O���[�v(NULL�Ȃ�Top�O���[�v�����ǔ񐄏�)
 *  @param  const wchar_t *name           �\����(���̖��O�̃O���[�v���쐬����܂�)
 *  @param  gfl::grp::g3d::Camera *target ����Ώ�
 */
//-----------------------------------------------------------------------------
extern void DebugWin_AddItemCamera( DebugWinGroup *parent , const wchar_t *name , gfl::grp::g3d::Camera *target , heap::HeapBase *heap = NULL );
extern void DebugWin_AddItemCamera( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::grp::g3d::Camera *target , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  ���f�����썀�ڒǉ�
 *          �O���[�v���쐬���A���ňʒu�A��]�A�X�P�[���A�A�j���t���[���A�A�j�����x�̑��삪�ł��܂�
 *          GetValue��I������ƌ��ݒl���擾�ł��܂����A��]�̂ݎ擾�ł��܂���B
 *          �폜����ۂ�name���g���č폜�ł��܂�
 *
 *  @param  DebugWinGroup *parent         �e�O���[�v(NULL�Ȃ�Top�O���[�v�����ǔ񐄏�)
 *  @param  const wchar_t *name           �\����(���̖��O�̃O���[�v���쐬����܂�)
 *  @param  gfl::grp::g3d::Model *target ����Ώ�
 */
//-----------------------------------------------------------------------------
extern void DebugWin_AddItemModel( DebugWinGroup *parent , const wchar_t *name , gfl::grp::g3d::Model *target , heap::HeapBase *heap = NULL );
extern void DebugWin_AddItemModel( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::grp::g3d::Model *target , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  ���C�g���썀�ڒǉ�
 *          �O���[�v���쐬���A���ňʒu�A�����A�e�F�̑��삪�ł��܂�
 *          �폜����ۂ�name���g���č폜�ł��܂�
 *
 *  @param  DebugWinGroup *parent         �e�O���[�v(NULL�Ȃ�Top�O���[�v�����ǔ񐄏�)
 *  @param  const wchar_t *name           �\����(���̖��O�̃O���[�v���쐬����܂�)
 *  @param  gfl::grp::g3d::Light *target ����Ώ�
 */
//-----------------------------------------------------------------------------
extern void DebugWin_AddItemLight( DebugWinGroup *parent , const wchar_t *name , gfl::grp::g3d::Light *target , heap::HeapBase *heap = NULL );
extern void DebugWin_AddItemLight( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::grp::g3d::Light *target , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  Haep�`�F�b�N���ڒǉ�
 *
 *  @param  DebugWinGroup *parent         �e�O���[�v(NULL�Ȃ�Top�O���[�v�����ǔ񐄏�)
 *  @param  const wchar_t *name           �\����(���̖��O�̃O���[�v���쐬����܂�)
 *  @param  gfl::heap::HeapBase *target   ����Ώ�
 */
//-----------------------------------------------------------------------------
extern void DebugWin_AddItemHeap( DebugWinGroup *parent , const wchar_t *name , gfl::heap::HeapBase *target , heap::HeapBase *heap = NULL );
extern void DebugWin_AddItemHeap( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::heap::HeapBase *target , heap::HeapBase *heap = NULL );

//���̑�
//----------------------------------------------------------------------------
/**
 *  @brief  �Ǝ��X�V�^���ڒǉ�
 *          �X�V�֐��Ɏ����ō�����֐���o�^���邱�Ƃ��ł��܂��B
 *          �X�V�֐��͍��ڂ��I������Ă���ԏ�ɌĂ΂�Ă���̂ŁA�{�^�����쓙�Ƒg�ݍ��킹�Ă��������B
 *
 *  @param  DebugWinGroup *parent   �e�O���[�v(NULL�Ȃ�Top�O���[�v�����ǔ񐄏�)
 *  @param  const wchar_t *name     �\����(���O[���l])�ƓW�J����܂�
 *  @param  void *work              ���[�U�[���[�N�B�X�V�֐��Ŏ󂯎��܂�
 *  @param  DebugWinUpdateFunc updateFunc �X�V�֐�
 *  @param  DebugWinDrawFunc drawFunc     �`�掞�X�V�֐��B����SetNameV�ȂǂŐ��l�\���Ɏg���܂��B
 */
//-----------------------------------------------------------------------------
extern DebugWinFunc* DebugWin_AddItemUpdateFunc( DebugWinGroup *parent , const wchar_t *name , void *work , DebugWinUpdateFunc updateFunc , DebugWinDrawFunc drawFunc = NULL , heap::HeapBase *heap = NULL );
extern DebugWinFunc* DebugWin_AddItemUpdateFunc( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , void *work , DebugWinUpdateFunc updateFunc , DebugWinDrawFunc drawFunc = NULL , heap::HeapBase *heap = NULL );

#else //GFL_DEBUGWIN_USE

/*
#define DebugWin_AddItemValueInt(parent,name,target,min,max,step) (NULL)
#define DebugWin_AddItemValueFloat(parent,name,target,min,max,step) (NULL)
#define DebugWin_AddItemValueVec3(parent,name,target,min,max,step) (NULL)
#define DebugWin_AddItemValueVec4(parent,name,target,min,max,step) (NULL)
#define DebugWin_AddItemCamera(parent,name,target) ((void)0)
#define DebugWin_AddItemModel(parent,name,target) ((void)0)
#define DebugWin_AddItemUpdateFunc(parent,name,work,updateFunc,drawFunc) ((void)0)
*/

class DebugWinItemGroup{};

template<typename T> class DebugWinItemValueU64{};
template<typename T> class DebugWinItemValueU32{};
template<typename T> class DebugWinItemValueU8{};
template<typename T> class DebugWinItemValueInt{};
template<typename T> class DebugWinItemValueFloat{};
class DebugWinItemValueVec3{};
class DebugWinItemValueVec4{};
class DebugWinItemValueColorF32{};

extern DebugWinItemValueInt<u64>* DebugWin_AddItemValueU64(...);
extern DebugWinItemValueInt<u32>* DebugWin_AddItemValueU32(...);
extern DebugWinItemValueInt<u8>* DebugWin_AddItemValueU8(...);
extern DebugWinItemValueInt<int>* DebugWin_AddItemValueInt(...);
extern DebugWinItemValueFloat<f32>* DebugWin_AddItemValueFloat(...);
extern DebugWinItemValueInt<bool>* DebugWin_AddItemValueBool(...);
extern DebugWinItemValueVec3* DebugWin_AddItemValueVec3(...);
extern DebugWinItemValueVec4* DebugWin_AddItemValueVec4(...);
extern DebugWinItemValueColorF32* DebugWin_AddItemValueColorF32(...);
extern void DebugWin_AddItemCamera(...);
extern void DebugWin_AddItemModel(...);
extern void DebugWin_AddItemLight(...);
extern void DebugWin_AddItemHeap(...);
extern void DebugWin_AddItemUpdateFunc(...);


#endif //GFL_DEBUGWIN_USE


}//namespace debug
}//namespace gfl




#endif  //__GFL_DEBUGWIN_ITEM_TYPES_H__

