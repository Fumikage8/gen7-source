#ifndef __GFL_TRANSPORTER_H__
#define __GFL_TRANSPORTER_H__
//======================================================================
/**
 * @file	gfl_Transporter.h
 * @brief	���l��Ԍv�Z�N���X
 * @author	ariizumi
 * @data	10/11/30
 */
//======================================================================

#pragma once
#include <heap/gfl_HeapBase.h>

namespace gfl
{
namespace math
{
//gfl_TransporterType.h ��f32��Vec�̒�`�����Ă���܂�
  
  
//���l��Ԋ��N���X
//VEC3�ɂ��Ă͉��ɂ���TransVec���g�p���Ă�������
template<typename T> class TransBase
{
public:
  //----------------------------------------------------------------------------
  /**
   *  @brief  �R���X�g���N�^
   *
   *  @param  start �J�n�l
   *  @param  end   �I���l
   *  @param  step  �X�e�b�v��
   */
  //-----------------------------------------------------------------------------
  TransBase(const T &start,const T &end , const u32 step )
  {
    Restart(start,end,step);
  }
  TransBase()
  {
    mStep = 0;
    mIsEnd = true;
  }
  virtual ~TransBase(){}

  //----------------------------------------------------------------------------
  /**
   *  @brief  �ăX�^�[�g
   *          �X�e�b�v����A�I���t���O�����������܂�
   *
   *  @param  start �J�n�l
   *  @param  end   �I���l
   *  @param  step  �X�e�b�v��
   */
  //-----------------------------------------------------------------------------
  virtual void Restart(const T &start,const T &end , const u32 step )
  {
    mStart = start;
    mNow = start;
    mEnd = end;
    mStepMax = step;
    mStep = 0;
    mIsEnd = false;
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  �X�V
   *          �X�e�b�v����i�߁A�X�V���ꂽ�l��Ԃ��܂�
   *
   *  @return  �X�V��̒l
   */
  //-----------------------------------------------------------------------------
  virtual const T& Update(void) = 0;

  //----------------------------------------------------------------------------
  /**
   *  @brief  �I���`�F�b�N
   *
   *  @return  �I���������H
   */
  //-----------------------------------------------------------------------------
  bool IsEnd(void) const {return mIsEnd;}

  //----------------------------------------------------------------------------
  /**
   *  @brief  �l�̎擾
   *
   *  @return  �l
   */
  //-----------------------------------------------------------------------------
  virtual const T& GetValue(void)const{return mNow;}
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  �J�n�l�̎擾
   *
   *  @return  �l
   */
  //-----------------------------------------------------------------------------
  virtual const T& GetStartValue(void)const{return mStart;}
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  �I���l�̎擾
   *
   *  @return  �l
   */
  //-----------------------------------------------------------------------------
  virtual const T& GetEndValue(void)const{return mEnd;}

  //----------------------------------------------------------------------------
  /**
   *  @brief  Update�r���ɋ����I��������
   *
   *  @return  �l
   */
  //-----------------------------------------------------------------------------
  void Clear (void){ mStep = 0; mIsEnd = true; }
  
protected:

  //�J�n�E�I���E���ݒl
  T mStart;
  T mEnd;
  T mNow;
  
  //�X�e�b�v
  int mStep;
  int mStepMax;
  
  //�I���t���O(�������̂��߂ɃX�e�b�v��r�ł͕Ԃ��Ȃ�)
  bool mIsEnd;
};

//���`���
template<typename T> class TransLiner:public TransBase<T>
{
public:
  TransLiner(const T &start,const T &end , const u32 step )
    :TransBase<T>(start,end,step){}
  TransLiner():TransBase<T>(){}
  virtual ~TransLiner(){}
  //�X�V
  virtual const T& Update(void)
  {
    if( this->mIsEnd == true )
    {
      return this->mNow;
    }
    
    this->mStep++;
    if( this->mStep < this->mStepMax )
    {
      this->mNow = this->mStart + ((this->mEnd-this->mStart)*this->mStep/this->mStepMax);
    }
    else
    {
      this->mNow = this->mEnd;
      this->mIsEnd = true;
    }
    
    return this->mNow;
  };
};

//�O�p�֐��g���ĉ~�̌ʂŕ��(�ŏ������� �X������P�Q��
template<typename T> class TransCurveFast:public TransBase<T>
{
public:
  TransCurveFast(const T &start,const T &end , const u32 step )
    :TransBase<T>(start,end,step){}
  TransCurveFast():TransBase<T>(){}
  virtual ~TransCurveFast(){}
  //�X�V
  virtual const T& Update(void)
  {
    if( this->mIsEnd == true )
    {
      return this->mNow;
    }
    
    this->mStep++;
    if( this->mStep < this->mStepMax )
    {
      //Index�Q�ƂȂ̂�0�`256
      const f32 idx = (f32)(this->mStepMax-this->mStep)/(f32)this->mStepMax;
      this->mNow = this->mStart + ((this->mEnd-this->mStart)*CosFIdx(AsinFIdx(idx)));
    }
    else
    {
      this->mNow = this->mEnd;
      this->mIsEnd = true;
    }
    
    return this->mNow;
  };
};
//�O�p�֐��g���ĉ~�̌ʂŕ��(�ŏ����x�� �U������R��
template<typename T> class TransCurveSlow:public TransBase<T>
{
public:
  TransCurveSlow(const T &start,const T &end , const u32 step )
    :TransBase<T>(start,end,step){}
  TransCurveSlow():TransBase<T>(){}
  virtual ~TransCurveSlow(){}
  //�X�V
  virtual const T& Update(void)
  {
    if( this->mIsEnd == true )
    {
      return this->mNow;
    }
    
    this->mStep++;
    if( this->mStep < this->mStepMax )
    {
      const f32 idx = (f32)this->mStep/(f32)this->mStepMax;
      this->mNow = this->mEnd-((this->mEnd-this->mStart)*SinFIdx(AcosFIdx(idx)));
    }
    else
    {
      this->mNow = this->mEnd;
      this->mIsEnd = true;
    }
    
    return this->mNow;
  };
};

//�O�p�֐��g���ĉ~�̌ʂŕ��(cos�g��180�`360�x�B�x�����������x��)
template<typename T> class TransCurveSFS:public TransBase<T>
{
public:
  TransCurveSFS(const T &start,const T &end , const u32 step )
    :TransBase<T>(start,end,step){}
  TransCurveSFS():TransBase<T>(){}
  virtual ~TransCurveSFS(){}
  //�X�V
  virtual const T& Update(void)
  {
    if( this->mIsEnd == true )
    {
      return this->mNow;
    }
    
    this->mStep++;
    if( this->mStep < this->mStepMax )
    {
      const f32 idx = (f32)this->mStep/(f32)this->mStepMax*128+128;
      const f32 con = (nn::math::CosFIdx(idx)+1)/2;
      this->mNow = this->mEnd-((this->mEnd-this->mStart)*con);
    }
    else
    {
      this->mNow = this->mEnd;
      this->mIsEnd = true;
    }
    
    return this->mNow;
  };
};
//�O�p�֐��g���ċȐ��ŕ��
class TransSin:public TransLiner<f32>
{
public:
  TransSin(const f32 startDec , const f32 endDec , const u32 step , const f32&rate , const f32& offset = 0)
  {
    Restart(startDec , endDec , step , rate , offset );
  }
  TransSin():TransLiner<f32>(){}
  virtual ~TransSin(){}

  void Restart(const f32 startDec , const f32 endDec , const u32 step , const f32& rate , const f32& offset = 0 )
  {
    TransLiner<f32>::Restart(startDec,endDec,step);
    mRate = rate;
    mOffset = offset;
    mNowValue = SinDeg( startDec ) * mRate + mOffset;
  }

  //�X�V
  virtual const f32& Update(void)
  {
    const f32 dec = TransLiner<f32>::Update();
    
    mNowValue = SinDeg( dec ) * mRate + mOffset;

    return mNowValue;
  };

  virtual const f32& GetValue(void)const{return mNowValue;}

private:
  f32 mRate;
  f32 mOffset;
  f32 mNowValue;  //mNow�ɂ͊p�x�������Ă���̂�

};

//Vecter��ԃN���X(f32�̕�ԃN���X��n���A���̌v�Z����XYZ�����ꂼ���Ԃ��܂�)
//�g�p��
//gfl::math::TransVec<gfl::math::TransLiner<f32> > vecTrans(startVec,endVec,64,sysHeap);

template<class TransClass>class TransVec:public TransBase<VEC3>
{
public:
  TransVec(const VEC3 &start,const VEC3 &end , const u32 step ,gfl::heap::HeapBase *heapHandle)
    :TransBase<VEC3>()
    ,mTransX(NULL)
    ,mTransY(NULL)
    ,mTransZ(NULL)
    {
      //�������q��ɗ��Ăق���
      Restart(start,end,step,heapHandle);
    }
  TransVec()
    :TransBase<VEC3>()
    ,mTransX(NULL)
    ,mTransY(NULL)
    ,mTransZ(NULL)
    {
    }
  virtual ~TransVec(void)
  {
    DeleteWork();
  }

  virtual void Restart(const VEC3 &start,const VEC3 &end , const u32 step ,gfl::heap::HeapBase *heapHandle)
  {
    if( mTransX != NULL){GFL_DELETE mTransX;mTransX=NULL;}
    if( mTransY != NULL){GFL_DELETE mTransY;mTransY=NULL;}
    if( mTransZ != NULL){GFL_DELETE mTransZ;mTransZ=NULL;}
    TransBase::Restart(start,end,step);
    if( start.x != end.x ){mTransX = GFL_NEW(heapHandle) TransClass(start.x,end.x,step);}
    if( start.y != end.y ){mTransY = GFL_NEW(heapHandle) TransClass(start.y,end.y,step);}
    if( start.z != end.z ){mTransZ = GFL_NEW(heapHandle) TransClass(start.z,end.z,step);}

  }

  //�X�V
  virtual const VEC3& Update(void)
  {
    if( this->mIsEnd == true )
    {
      return this->mNow;
    }
    
    this->mStep++;
    if( this->mStep < this->mStepMax )
    {
      if( mTransX != NULL){this->mNow.x = mTransX->Update();}
      if( mTransY != NULL){this->mNow.y = mTransY->Update();}
      if( mTransZ != NULL){this->mNow.z = mTransZ->Update();}
    }
    else
    {
      this->mNow = this->mEnd;
      this->mIsEnd = true;
    }
    return this->mNow;
  };
  
  virtual void DeleteWork(void)
  {
    if( mTransX != NULL){GFL_DELETE mTransX;}
    if( mTransY != NULL){GFL_DELETE mTransY;}
    if( mTransZ != NULL){GFL_DELETE mTransZ;}
  }
  
private:
  TransBase<f32> *mTransX;
  TransBase<f32> *mTransY;
  TransBase<f32> *mTransZ;
};

}//namespace gfl
}//namespace math
#endif // __GFL_TRANSPORTER_H__
