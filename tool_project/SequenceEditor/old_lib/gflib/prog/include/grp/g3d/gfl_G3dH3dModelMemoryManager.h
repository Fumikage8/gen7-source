//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dModelMemoryManager.h
 *	@brief  H3D　モデルメモリ解放管理　マネージャ
 *	@author	tomoya takahashi
 *	@date		2012.08.30
 *
 *	@note H3Dモデルのキャッシュメモリを１フレーム待って
 *	　　　レンダリング終了後に削除するマネージャ
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __GFL_G3dH3DMODELMEMORYMANAGER_H__
#define __GFL_G3dH3DMODELMEMORYMANAGER_H__
#pragma once

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_Debug.h>
#include <gfl_fs.h>


#if GFL_GRP_G3D_H3D_USE


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class H3dModelMemoryManager
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dModelMemoryManager);

public:
  
  class IMemortObject{
  public:
    virtual ~IMemortObject(){}
  protected:
    IMemortObject(){}
    
  };

  // コンストラクタ・デストラクタ
  H3dModelMemoryManager(void);
  ~H3dModelMemoryManager(void);
  
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  マネージャ初期化
   *
   *	@param  p_heap        ヒープ
   *	@param  h3dmodel_max  モデル最大数
   */
  //-----------------------------------------------------------------------------
  void Initialize( gfl::heap::HeapBase* p_heap, u32 h3dmodel_max );

  //----------------------------------------------------------------------------
  /**
   *	@brief  マネージャ破棄　（登録されたメモリを一括で解放します）
   */
  //-----------------------------------------------------------------------------
  void Finalize( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  解放処理の更新    CalculateやSubmitViewのタイミングで呼ばれる想定である。
   *	                          １フレームに１回。
   */
  //-----------------------------------------------------------------------------
  void Update( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  破棄するメモリを登録    １回だけ登録するように注意
   */
  //-----------------------------------------------------------------------------
  //void SetDeleteMemory( void * p_state_heap, void * p_state_device );
  void SetDeleteMemory( IMemortObject* pObject );

  //----------------------------------------------------------------------------
  /**
   *	@brief    データの有無をチェック
   */
  //-----------------------------------------------------------------------------
  bool IsHeapIdMemoryExists( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief    heapIDから作られたメモリが存在しているかチェック
   *
   *	@retval true  存在している
   *	@retval false 存在していない
   */
  //-----------------------------------------------------------------------------
  bool IsHeapIdMemoryExists( gfl::heap::HEAPID heapID ) const;


private:

  struct Data
  {
    //void * p_state_heap;
    //void * p_state_device;
    IMemortObject*    pMemortObject;
    
    s8 count;
    b8 data_in;
    u8  pad[2];
    
    Data() : pMemortObject(NULL){}
  };
  

private:
  void FreeMemoryData( Data* p_data );

private:

  Data* m_pBuffer;
  s32   m_BufferMax;

};



}  // namespace g3d
}  // namespace grp
}  // namespace gfl


#endif  // GFL_GRP_G3D_H3D_USE


#endif	// __GFL_G3dH3DMODELMEMORYMANAGER_H__

