//======================================================================
/**
 * @file gfl2_DrawingLytWkManager.h
 * @date 2015/08/03 15:47:18
 * @author kawada_koji
 * @brief 描画中レイアウトワークのマネージャ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __GFL2_LAYOUT_DRAWINGLYTWKMANAGER_H_INCLUDED__
#define __GFL2_LAYOUT_DRAWINGLYTWKMANAGER_H_INCLUDED__
#pragma once


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)
class LytWk;
GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)




GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)


//==============================================================================
/**
 @class  DrawingLytWkManager
 @brief  描画中レイアウトワークのマネージャ
 */
class DrawingLytWkManager
{
  GFL_FORBID_COPY_AND_ASSIGN(DrawingLytWkManager);
 
  friend class LytWk;

public:
  //! @brief  コンストラクタ
  //! @param[in] heap         ヒープ(普通のメモリでもデバイスメモリでもどちらでもいい)
  //! @param[in] lytWkNumMax  管理するレイアウトワークの最大数
  DrawingLytWkManager(gfl2::heap::HeapBase* heap, u32 lytWkNumMax);
  //! @brief  デストラクタ
  virtual ~DrawingLytWkManager();

  //! @brief  リストをスワップし、
  //          以前のリスト（スワップしたのでカレントリストになっている）に登録されていたレイアウトワークの描画中カウントを下げる。
  void SwapList(void);

  //! @name  friendであるLytWkからしかアクセスしない
  //@{
private:
  //! @brief  レイアウトワークを登録する
  void AddLytWk(LytWk* lytWk);

  //! @brief  強制的にレイアウトワークを登録解除する
  //! @note  「LytWkが破棄される際にリストに残ったままである」というときに、リストから登録解除する。
  //!        このようなときは「レイアウト描画中に破棄」であるので、そのASSERTを抜けた後にこの関数が呼ばれる。
  void RemoveLytWkForce(LytWk* lytWk);
  //@}

private:
  enum
  {
    LIST_NUM = 2  // LIST_NUMは2固定。それ以外は想定していない。
  };

private:
  u32     m_lytWkNumMax;
  u32     m_currListIndex;
  u32     m_lytWkNum[LIST_NUM];
  LytWk** m_lytWkList[LIST_NUM];
};


GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)


#endif // __GFL2_LAYOUT_DRAWINGLYTWKMANAGER_H_INCLUDED__

