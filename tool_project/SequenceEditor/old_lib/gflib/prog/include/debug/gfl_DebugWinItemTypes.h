//======================================================================
/**
 * @file  gfl_DebugwinItemTypes.h
 * @brief デバッグウィンドウ用アイテム(項目)の拡張定義
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

//クラス定義
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
 * @brief デバッグメニュー項目：数値操作用 基底テンプレートクラス
 * @tparam  T
 * @note  整数型と小数型で表示が異なるから一段基底をおく
 */
//---------------------------------------------------------
template<typename T> class DebugWinItemValue : public DebugWinItem
{
public:
  //数値更新時に呼ばれる関数の型
  typedef void(*ChangedValueFunc)( void *work );

  //コンストラクタ
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
  
  //項目選択時の更新のメイン部分
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
  
  //項目選択時の更新
  virtual void UpdateFunc(DebugWin *debSys)
  {
    UpdateValueFunc<T>( debSys,mTarget , mMin , mMax , mStep);
  }
  
  //型によって代わるので純粋仮想化
  virtual const wchar_t* GetDispName(DebugWin *debSys) = 0;

  ///変更通知を受け取るコールバックの登録
  inline void SetChangedValueFunc( ChangedValueFunc func , void *work )
  {
    mFunc = func;
    mWork = work;
  }


protected:
  //変更を通知する
  virtual void ChangedValue(void)
  {
    if( mFunc )
    {
      mFunc( mWork );
    }
  }


  T *mTarget;   ///<操作対象
  T mMin;       ///<最小値
  T mMax;       ///<最大値
  T mStep;      ///<変更量

  ChangedValueFunc mFunc; ///<変更通知を受け取るコールバック関数
  void *mWork;            ///<変更通知を受け取るコールバック関数のワーク

};

//---------------------------------------------------------
/**
 * @brief デバッグメニュー項目：数値操作用テンプレートクラス整数型
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
 * @brief デバッグメニュー項目：数値操作用テンプレートクラス整数型
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
 * @brief デバッグメニュー項目：数値操作用テンプレートクラス整数型
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
 * @brief デバッグメニュー項目：数値操作用テンプレートクラス整数型
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
 * @brief デバッグメニュー項目：数値操作用テンプレートクラス小数型
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
 * @brief デバッグメニュー項目：数値操作用クラスVec3型(独自処理追加)
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
 * @brief デバッグメニュー項目：数値操作用クラスVec4型(独自処理追加)
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
 * @brief デバッグメニュー項目：数値操作用クラスColorF32型(独自処理追加)
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
 * @brief デバッグメニュー項目：数値操作用クラス整数型MsgData版(独自処理追加)
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
//汎用追加用

//数値系
//----------------------------------------------------------------------------
/**
 *  @brief  整数型操作項目追加
 *
 *  @param  DebugWinGroup *parent 親グループ(NULLならTopグループだけど非推奨)
 *  @param  const wchar_t *name   表示名(名前[数値])と展開されます
 *  @param  int *target           操作対象
 *  @param  int min               最小値
 *  @param  int max               最大値
 *  @param  int step              移動幅(R同時押しで10倍、L同時押しで100倍、LR同時押しで1000倍で操作)
 *
 *  @return DebugWinItemValueInt<int>* 戻り値に対してSetChangedValueFuncを呼ぶと数値変動時の関数を設定できます
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueU64<u64>* DebugWin_AddItemValueU64( DebugWinGroup *parent , const wchar_t *name , u64 *target , u64 min , u64 max , u64 step = 1 , heap::HeapBase *heap = NULL );
extern DebugWinItemValueU64<u64>* DebugWin_AddItemValueU64( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , u64 *target , u64 min , u64 max , u64 step = 1 , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  整数型操作項目追加
 *
 *  @param  DebugWinGroup *parent 親グループ(NULLならTopグループだけど非推奨)
 *  @param  const wchar_t *name   表示名(名前[数値])と展開されます
 *  @param  int *target           操作対象
 *  @param  int min               最小値
 *  @param  int max               最大値
 *  @param  int step              移動幅(R同時押しで10倍、L同時押しで100倍、LR同時押しで1000倍で操作)
 *
 *  @return DebugWinItemValueInt<int>* 戻り値に対してSetChangedValueFuncを呼ぶと数値変動時の関数を設定できます
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueU32<u32>* DebugWin_AddItemValueU32( DebugWinGroup *parent , const wchar_t *name , u32 *target , u32 min , u32 max , u32 step = 1 , heap::HeapBase *heap = NULL );
extern DebugWinItemValueU32<u32>* DebugWin_AddItemValueU32( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , u32 *target , u32 min , u32 max , u32 step = 1 , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  整数型操作項目追加
 *
 *  @param  DebugWinGroup *parent 親グループ(NULLならTopグループだけど非推奨)
 *  @param  const wchar_t *name   表示名(名前[数値])と展開されます
 *  @param  int *target           操作対象
 *  @param  int min               最小値
 *  @param  int max               最大値
 *  @param  int step              移動幅(R同時押しで10倍、L同時押しで100倍、LR同時押しで1000倍で操作)
 *
 *  @return DebugWinItemValueInt<int>* 戻り値に対してSetChangedValueFuncを呼ぶと数値変動時の関数を設定できます
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueU8<u8>* DebugWin_AddItemValueU8( DebugWinGroup *parent , const wchar_t *name , u8 *target , u8 min , u8 max , u8 step = 1 , heap::HeapBase *heap = NULL );
extern DebugWinItemValueU8<u8>* DebugWin_AddItemValueU8( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , u8 *target , u8 min , u8 max , u8 step = 1 , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  整数型操作項目追加
 *
 *  @param  DebugWinGroup *parent 親グループ(NULLならTopグループだけど非推奨)
 *  @param  const wchar_t *name   表示名(名前[数値])と展開されます
 *  @param  int *target           操作対象
 *  @param  int min               最小値
 *  @param  int max               最大値
 *  @param  int step              移動幅(R同時押しで10倍、L同時押しで100倍、LR同時押しで1000倍で操作)
 *
 *  @return DebugWinItemValueInt<int>* 戻り値に対してSetChangedValueFuncを呼ぶと数値変動時の関数を設定できます
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueInt<int>* DebugWin_AddItemValueInt( DebugWinGroup *parent , const wchar_t *name , int *target , int min , int max , int step = 1 , heap::HeapBase *heap = NULL );
extern DebugWinItemValueInt<int>* DebugWin_AddItemValueInt( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , int *target , int min , int max , int step = 1 , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  小数型操作項目追加
 *
 *  @param  DebugWinGroup *parent 親グループ(NULLならTopグループだけど非推奨)
 *  @param  const wchar_t *name   表示名(名前[数値])と展開されます
 *  @param  f32 *target           操作対象
 *  @param  f32 min               最小値
 *  @param  f32 max               最大値
 *  @param  f32 step              移動幅(R同時押しで10倍、L同時押しで100倍、LR同時押しで1000倍で操作)
 *
 *  @return DebugWinItemValueFloat<f32>* 戻り値に対してSetChangedValueFuncを呼ぶと数値変動時の関数を設定できます
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueFloat<f32>* DebugWin_AddItemValueFloat( DebugWinGroup *parent , const wchar_t *name , f32 *target , f32 min , f32 max , f32 step = 1 , heap::HeapBase *heap = NULL );
extern DebugWinItemValueFloat<f32>* DebugWin_AddItemValueFloat( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , f32 *target , f32 min , f32 max , f32 step = 1 , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  bool型操作項目追加
 *
 *  @param  DebugWinGroup *parent 親グループ(NULLならTopグループだけど非推奨)
 *  @param  const wchar_t *name   表示名(名前[数値])と展開されます
 *  @param  bool *target           操作対象
 *  @param  int min               最小値
 *  @param  int max               最大値
 *  @param  int step              移動幅(R同時押しで10倍、L同時押しで100倍、LR同時押しで1000倍で操作)
 *
 *  @return DebugWinItemValueInt<bool>* 戻り値に対してSetChangedValueFuncを呼ぶと数値変動時の関数を設定できます
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueInt<bool>* DebugWin_AddItemValueBool( DebugWinGroup *parent , const wchar_t *name , bool *target , heap::HeapBase *heap = NULL );
extern DebugWinItemValueInt<bool>* DebugWin_AddItemValueBool( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , bool *target , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  VEC3型操作項目追加
 *
 *  @param  DebugWinGroup *parent   親グループ(NULLならTopグループだけど非推奨)
 *  @param  const wchar_t *name     表示名(名前[数値])と展開されます
 *  @param  gfl::math::VEC3 *target 操作対象
 *  @param  f32 min                 最小値
 *  @param  f32 max                 最大値
 *  @param  f32 step                移動幅(R同時押しで10倍、L同時押しで100倍、LR同時押しで1000倍で操作)
 *
 *  @return DebugWinItemValueVec3* 戻り値に対してSetChangedValueFuncを呼ぶと数値変動時の関数を設定できます
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueVec3* DebugWin_AddItemValueVec3( DebugWinGroup *parent , const wchar_t *name , gfl::math::VEC3 *target , f32 min , f32 max , f32 step = 1 , heap::HeapBase *heap = NULL );
extern DebugWinItemValueVec3* DebugWin_AddItemValueVec3( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::math::VEC3 *target , f32 min , f32 max , f32 step = 1 , heap::HeapBase *heap = NULL );

//----------------------------------------------------------------------------
/**
 *  @brief  VEC4型操作項目追加
 *
 *  @param  DebugWinGroup *parent   親グループ(NULLならTopグループだけど非推奨)
 *  @param  const wchar_t *name     表示名(名前[数値])と展開されます
 *  @param  gfl::math::VEC4 *target 操作対象
 *  @param  f32 min                 最小値
 *  @param  f32 max                 最大値
 *  @param  f32 step                移動幅(R同時押しで10倍、L同時押しで100倍、LR同時押しで1000倍で操作)
 *
 *  @return DebugWinItemValueVec4* 戻り値に対してSetChangedValueFuncを呼ぶと数値変動時の関数を設定できます
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueVec4* DebugWin_AddItemValueVec4( DebugWinGroup *parent , const wchar_t *name , gfl::math::VEC4 *target , f32 min , f32 max , f32 step = 1 , heap::HeapBase *heap = NULL );
extern DebugWinItemValueVec4* DebugWin_AddItemValueVec4( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::math::VEC4 *target , f32 min , f32 max , f32 step = 1 , heap::HeapBase *heap = NULL );

//----------------------------------------------------------------------------
/**
 *  @brief  ColorF32型操作項目追加
 *
 *  @param  DebugWinGroup *parent   親グループ(NULLならTopグループだけど非推奨)
 *  @param  const wchar_t *name     表示名(名前[数値])と展開されます
 *  @param  gfl::math::VEC4 *target 操作対象
 *  @param  f32 min                 最小値(  0)
 *  @param  f32 max                 最大値(  1)
 *  @param  f32 step                移動幅(0.1)(R同時押しで10倍、L同時押しで100倍、LR同時押しで1000倍で操作)
 *
 *  @return DebugWinItemValueColorF32* 戻り値に対してSetChangedValueFuncを呼ぶと数値変動時の関数を設定できます
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueColorF32* DebugWin_AddItemValueColorF32( DebugWinGroup *parent , const wchar_t *name , gfl::grp::ColorF32 *target , f32 min = 0 , f32 max = 1, f32 step = 0.1 , heap::HeapBase *heap = NULL );
extern DebugWinItemValueColorF32* DebugWin_AddItemValueColorF32( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::grp::ColorF32 *target , f32 min = 0 , f32 max = 1, f32 step = 0.1 , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  整数型操作項目追加(Msg表示)
 *
 *  @param  DebugWinGroup *parent 親グループ(NULLならTopグループだけど非推奨)
 *  @param  const wchar_t *name   表示名(名前[数値])と展開されます
 *  @param  int *target           操作対象
 *  @param  int min               最小値
 *  @param  int max               最大値(-1で自動的にメッセージの個数を設定します)
 *  @param  int msgOfs            メッセージオフセット(表示位置をずらす この値＋targetの値のStrIdが表示されます)
 *
 *  @return DebugWinItemValueIntMsg* 戻り値に対してSetChangedValueFuncを呼ぶと数値変動時の関数を設定できます
 *          DebugWinItemValueInt<int>* でも受け取れます。
 */
//-----------------------------------------------------------------------------
extern DebugWinItemValueIntMsg* DebugWin_AddItemValueIntMsg( DebugWinGroup *parent , const wchar_t *name , int *target , fs::ArcFile::ARCID arcID, u32 datID , int min = 0 , int max = -1 , int msgOfs = 0 , heap::HeapBase *heap = NULL );
extern DebugWinItemValueIntMsg* DebugWin_AddItemValueIntMsg( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , int *target , fs::ArcFile::ARCID arcID, u32 datID , int min = 0 , int max = -1 , int msgOfs = 0 , heap::HeapBase *heap = NULL );

//クラス系
//----------------------------------------------------------------------------
/**
 *  @brief  カメラ操作項目追加
 *          グループを作成し、中で視点、注視点、near、farの操作ができます。
 *          GetValueを選択すると現在値を取得できます。
 *          削除する際はnameを使って削除できます
 *
 *  @param  DebugWinGroup *parent         親グループ(NULLならTopグループだけど非推奨)
 *  @param  const wchar_t *name           表示名(この名前のグループが作成されます)
 *  @param  gfl::grp::g3d::Camera *target 操作対象
 */
//-----------------------------------------------------------------------------
extern void DebugWin_AddItemCamera( DebugWinGroup *parent , const wchar_t *name , gfl::grp::g3d::Camera *target , heap::HeapBase *heap = NULL );
extern void DebugWin_AddItemCamera( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::grp::g3d::Camera *target , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  モデル操作項目追加
 *          グループを作成し、中で位置、回転、スケール、アニメフレーム、アニメ速度の操作ができます
 *          GetValueを選択すると現在値を取得できますが、回転のみ取得できません。
 *          削除する際はnameを使って削除できます
 *
 *  @param  DebugWinGroup *parent         親グループ(NULLならTopグループだけど非推奨)
 *  @param  const wchar_t *name           表示名(この名前のグループが作成されます)
 *  @param  gfl::grp::g3d::Model *target 操作対象
 */
//-----------------------------------------------------------------------------
extern void DebugWin_AddItemModel( DebugWinGroup *parent , const wchar_t *name , gfl::grp::g3d::Model *target , heap::HeapBase *heap = NULL );
extern void DebugWin_AddItemModel( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::grp::g3d::Model *target , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  ライト操作項目追加
 *          グループを作成し、中で位置、方向、各色の操作ができます
 *          削除する際はnameを使って削除できます
 *
 *  @param  DebugWinGroup *parent         親グループ(NULLならTopグループだけど非推奨)
 *  @param  const wchar_t *name           表示名(この名前のグループが作成されます)
 *  @param  gfl::grp::g3d::Light *target 操作対象
 */
//-----------------------------------------------------------------------------
extern void DebugWin_AddItemLight( DebugWinGroup *parent , const wchar_t *name , gfl::grp::g3d::Light *target , heap::HeapBase *heap = NULL );
extern void DebugWin_AddItemLight( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::grp::g3d::Light *target , heap::HeapBase *heap = NULL );
//----------------------------------------------------------------------------
/**
 *  @brief  Haepチェック項目追加
 *
 *  @param  DebugWinGroup *parent         親グループ(NULLならTopグループだけど非推奨)
 *  @param  const wchar_t *name           表示名(この名前のグループが作成されます)
 *  @param  gfl::heap::HeapBase *target   操作対象
 */
//-----------------------------------------------------------------------------
extern void DebugWin_AddItemHeap( DebugWinGroup *parent , const wchar_t *name , gfl::heap::HeapBase *target , heap::HeapBase *heap = NULL );
extern void DebugWin_AddItemHeap( DebugWinGroup *parent , str::MsgData *msgData , u32 strId , gfl::heap::HeapBase *target , heap::HeapBase *heap = NULL );

//その他
//----------------------------------------------------------------------------
/**
 *  @brief  独自更新型項目追加
 *          更新関数に自分で作った関数を登録することができます。
 *          更新関数は項目が選択されている間常に呼ばれているので、ボタン操作等と組み合わせてください。
 *
 *  @param  DebugWinGroup *parent   親グループ(NULLならTopグループだけど非推奨)
 *  @param  const wchar_t *name     表示名(名前[数値])と展開されます
 *  @param  void *work              ユーザーワーク。更新関数で受け取れます
 *  @param  DebugWinUpdateFunc updateFunc 更新関数
 *  @param  DebugWinDrawFunc drawFunc     描画時更新関数。中でSetNameVなどで数値表示に使えます。
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

