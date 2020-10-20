#ifndef __GFL_TRANSPORTER_H__
#define __GFL_TRANSPORTER_H__
//======================================================================
/**
 * @file	gfl_Transporter.h
 * @brief	数値補間計算クラス
 * @author	ariizumi
 * @data	10/11/30
 */
//======================================================================

#pragma once

#include <debug/include/gfl2_DebugPrint.h>

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Vector3.h>
#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_MathTriangular.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(math)

//gfl_TransporterType.h にf32とVector3の定義がしてあります
  
  
//数値補間基底クラス
//Vector3については下にあるTransVecを使用してください
template<typename T> class TransBase
{
public:
  //----------------------------------------------------------------------------
  /**
   *  @brief  コンストラクタ
   *
   *  @param  start 開始値
   *  @param  end   終了値
   *  @param  step  ステップ数
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
   *  @brief  再スタート
   *          ステップ数や、終了フラグを初期化します
   *
   *  @param  start 開始値
   *  @param  end   終了値
   *  @param  step  ステップ数
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
   *  @brief  更新
   *          ステップ数を進め、更新された値を返します
   *
   *  @return  更新後の値
   */
  //-----------------------------------------------------------------------------
  virtual const T& Update(void) = 0;

  //----------------------------------------------------------------------------
  /**
   *  @brief  終了チェック
   *
   *  @return  終了したか？
   */
  //-----------------------------------------------------------------------------
  bool IsEnd(void) const {return mIsEnd;}

  //----------------------------------------------------------------------------
  /**
   *  @brief  値の取得
   *
   *  @return  値
   */
  //-----------------------------------------------------------------------------
  virtual const T& GetValue(void)const{return mNow;}
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  開始値の取得
   *
   *  @return  値
   */
  //-----------------------------------------------------------------------------
  virtual const T& GetStartValue(void)const{return mStart;}
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  終了値の取得
   *
   *  @return  値
   */
  //-----------------------------------------------------------------------------
  virtual const T& GetEndValue(void)const{return mEnd;}

  //----------------------------------------------------------------------------
  /**
   *  @brief  Update途中に強制終了させる
   *
   *  @return  値
   */
  //-----------------------------------------------------------------------------
  void Clear (void){ mStep = 0; mIsEnd = true; }
  
protected:

  //開始・終了・現在値
  T mStart;
  T mEnd;
  T mNow;
  
  //ステップ
  int mStep;
  int mStepMax;
  
  //終了フラグ(反復等のためにステップ比較では返さない)
  bool mIsEnd;
};

//線形補間
template<typename T> class TransLiner:public TransBase<T>
{
public:
  TransLiner(const T &start,const T &end , const u32 step )
    :TransBase<T>(start,end,step){}
  TransLiner():TransBase<T>(){}
  virtual ~TransLiner(){}
  //更新
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

//三角関数使って円の弧で補間(最初が早い ９時から１２時
template<typename T> class TransCurveFast:public TransBase<T>
{
public:
  TransCurveFast(const T &start,const T &end , const u32 step )
    :TransBase<T>(start,end,step){}
  TransCurveFast():TransBase<T>(){}
  virtual ~TransCurveFast(){}
  //更新
  virtual const T& Update(void)
  {
    if( this->mIsEnd == true )
    {
      return this->mNow;
    }
    
    this->mStep++;
    if( this->mStep < this->mStepMax )
    {
      //Index参照なので0〜256
      const f32 idx = (f32)(this->mStepMax-this->mStep)/(f32)this->mStepMax;
      this->mNow = this->mStart + ((this->mEnd-this->mStart)*CosRad(AsinRad(idx)));
    }
    else
    {
      this->mNow = this->mEnd;
      this->mIsEnd = true;
    }
    
    return this->mNow;
  };
};
//三角関数使って円の弧で補間(最初が遅い ６時から３時
template<typename T> class TransCurveSlow:public TransBase<T>
{
public:
  TransCurveSlow(const T &start,const T &end , const u32 step )
    :TransBase<T>(start,end,step){}
  TransCurveSlow():TransBase<T>(){}
  virtual ~TransCurveSlow(){}
  //更新
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
      this->mNow = this->mEnd-((this->mEnd-this->mStart)*SinRad(AcosRad(idx)));
    }
    else
    {
      this->mNow = this->mEnd;
      this->mIsEnd = true;
    }
    
    return this->mNow;
  };
};

//三角関数使って円の弧で補間(cos波の180〜360度。遅い→早い→遅い)
template<typename T> class TransCurveSFS:public TransBase<T>
{
public:
  TransCurveSFS(const T &start,const T &end , const u32 step )
    :TransBase<T>(start,end,step){}
  TransCurveSFS():TransBase<T>(){}
  virtual ~TransCurveSFS(){}
  //更新
  virtual const T& Update(void)
  {
    if( this->mIsEnd == true )
    {
      return this->mNow;
    }
    
    this->mStep++;
    if( this->mStep < this->mStepMax )
    {
      const f32 idx = (f32)this->mStep/(f32)this->mStepMax*PI+PI;
      const f32 con = (CosRad(idx)+1)/2;
      this->mNow = this->mStart+((this->mEnd-this->mStart)*con);
    }
    else
    {
      this->mNow = this->mEnd;
      this->mIsEnd = true;
    }
    
    return this->mNow;
  };
};
//三角関数使って曲線で補間
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

  //更新
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
  f32 mNowValue;  //mNowには角度が入っているので

};

//Vecter補間クラス(f32の補間クラスを渡し、その計算式でXYZをそれぞれ補間します)
//使用例
//gfl::math::TransVec<gfl::math::TransLiner<f32> > vecTrans(startVec,endVec,64,sysHeap);
template<class TransClass>class TransVec:public TransBase<gfl2::math::Vector3>
{
public:
  TransVec(const gfl2::math::Vector3 &start,const gfl2::math::Vector3 &end , const u32 step ,gfl2::heap::HeapBase *heapHandle)
    :TransBase<gfl2::math::Vector3>()
    ,mTransX(NULL)
    ,mTransY(NULL)
    ,mTransZ(NULL)
    {
      //初期化子後に来てほしい
      Restart(start,end,step,heapHandle);
    }
  TransVec()
    :TransBase<gfl2::math::Vector3>()
    ,mTransX(NULL)
    ,mTransY(NULL)
    ,mTransZ(NULL)
    {
    }
  virtual ~TransVec(void)
  {
    DeleteWork();
  }

  virtual void Restart(const gfl2::math::Vector3 &start,const gfl2::math::Vector3 &end , const u32 step ,gfl2::heap::HeapBase *heapHandle)
  {
    if( mTransX != NULL){GFL_SAFE_DELETE(mTransX);mTransX=NULL;}
    if( mTransY != NULL){GFL_SAFE_DELETE(mTransY);mTransY=NULL;}
    if( mTransZ != NULL){GFL_SAFE_DELETE(mTransZ);mTransZ=NULL;}
    TransBase::Restart(start,end,step);
    if( start.x != end.x ){mTransX = GFL_NEW(heapHandle) TransClass(start.x,end.x,step);}
    if( start.y != end.y ){mTransY = GFL_NEW(heapHandle) TransClass(start.y,end.y,step);}
    if( start.z != end.z ){mTransZ = GFL_NEW(heapHandle) TransClass(start.z,end.z,step);}

  }

  //更新
  virtual const gfl2::math::Vector3& Update(void)
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

//Vecter補間クラス(f32の補間クラスを渡し、その計算式でXYZをそれぞれ補間します)
//使用例
//gfl::math::TransVec<gfl::math::TransLiner<f32> > vecTrans(startVec,endVec,64,sysHeap);
template<class TransClass>class TransVec4:public TransBase<gfl2::math::Vector4>
{
public:
  TransVec4(const gfl2::math::Vector4 &start,const gfl2::math::Vector4 &end , const u32 step ,gfl2::heap::HeapBase *heapHandle)
    :TransBase<gfl2::math::Vector4>()
    ,mTransX(NULL)
    ,mTransY(NULL)
    ,mTransZ(NULL)
    ,mTransW(NULL)
    {
      //初期化子後に来てほしい
      Restart(start,end,step,heapHandle);
    }
  TransVec4()
    :TransBase<gfl2::math::Vector4>()
    ,mTransX(NULL)
    ,mTransY(NULL)
    ,mTransZ(NULL)
    ,mTransW(NULL)
    {
    }
  virtual ~TransVec4(void)
  {
    DeleteWork();
  }

  virtual void Restart(const gfl2::math::Vector4 &start,const gfl2::math::Vector4 &end , const u32 step ,gfl2::heap::HeapBase *heapHandle)
  {
    if( mTransX != NULL){GFL_SAFE_DELETE(mTransX);mTransX=NULL;}
    if( mTransY != NULL){GFL_SAFE_DELETE(mTransY);mTransY=NULL;}
    if( mTransZ != NULL){GFL_SAFE_DELETE(mTransZ);mTransZ=NULL;}
    if( mTransW != NULL){GFL_SAFE_DELETE(mTransW);mTransW=NULL;}
    TransBase::Restart(start,end,step);
    if( start.x != end.x ){mTransX = GFL_NEW(heapHandle) TransClass(start.x,end.x,step);}
    if( start.y != end.y ){mTransY = GFL_NEW(heapHandle) TransClass(start.y,end.y,step);}
    if( start.z != end.z ){mTransZ = GFL_NEW(heapHandle) TransClass(start.z,end.z,step);}
    if( start.w != end.w ){mTransW = GFL_NEW(heapHandle) TransClass(start.w,end.w,step);}

  }

  //更新
  virtual const gfl2::math::Vector4& Update(void)
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
      if( mTransW != NULL){this->mNow.w = mTransW->Update();}
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
    if( mTransW != NULL){GFL_DELETE mTransW;}
  }
  
private:
  TransBase<f32> *mTransX;
  TransBase<f32> *mTransY;
  TransBase<f32> *mTransZ;
  TransBase<f32> *mTransW;
};

GFL_NAMESPACE_END(math)
GFL_NAMESPACE_END(gfl2)
#endif // __GFL_TRANSPORTER_H__
