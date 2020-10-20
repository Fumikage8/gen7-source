#if !defined(NIJI_PROJECT_APPLIB_APP_TOOL_RADERCHART_H_INCLUDED)
#define NIJI_PROJECT_APPLIB_APP_TOOL_RADERCHART_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    app_tool_RaderChart.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.07
 @brief   レーダーチャート
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <math/include/gfl2_Vector2.h>
#include <math/include/gfl2_Vector4.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
  class AsyncFileManager;
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(renderingengine)
GFL_NAMESPACE_BEGIN(scenegraph)
GFL_NAMESPACE_BEGIN(resource)
  class ResourceNode;
GFL_NAMESPACE_END(resource)
GFL_NAMESPACE_END(scenegraph)
GFL_NAMESPACE_END(renderingengine)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(model)
  class BaseModel;
GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)








GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)


//==============================================================================
/**
 @class     RaderChart
 @brief     レーダーチャート
 */
//==============================================================================
class RaderChart
{
  GFL_FORBID_COPY_AND_ASSIGN(RaderChart);

  friend class RaderChartRenderPath;

public:
  //! @brief  図
  enum Figure
  {
    FIGURE_INSIDE,   //!< 内側チャート
    FIGURE_OUTSIDE,  //!< 外側チャート
    FIGURE_NUM
  };

  //! @brief  項目数
  static const u32 ITEM_NUM = 6;

private:
  static const f32 ADJUST_RADIUS_TIMES;  // 先細りして指定した半径に届かないので、これだけ拡大して届かせる。

public:
  RaderChart(void);
  virtual ~RaderChart();

  //! @name  生成/破棄
  //@{

  //! @brief  非同期生成　開始
  //! @note  Updateを毎フレーム呼んでいないと非同期生成処理は進みません。
  void StartAsyncCreate(
      app::util::Heap*            pHeap,             // 生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
      gfl2::fs::AsyncFileManager* pAsyncFileManager  // ファイルマネージャ
  );
  //! @brief  同期生成 
  void SyncCreate(
      app::util::Heap*            pHeap,             // 生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
      gfl2::fs::AsyncFileManager* pAsyncFileManager  // ファイルマネージャ
  );
  //! @brief  生成が完了しているか
  //! @return  生成が完了したらtrueを返す。
  //!          生成を開始していないときはfalseを返す。生成中はfalseを返す。
  //! @note  Updateを毎フレーム呼んでいないと非同期生成処理は進みません。
  bool IsCreated(void) const;
  
  //! @brief  破棄できるか
  //! @return  破棄できるときtrueを返す。
  //!          生成中は破棄できないのでfalseを返す。
  //! @caution  描画をオフにしてから1フレーム後に破棄しないと安全ではありません。
  //!             この関数は「破棄できるか否か」だけを返しており、「オフにして1フレーム空けたか否か」は判定していません。
  //!             「オフにして1フレーム空けたか否か」は呼び出し元で判定して下さい。
  //!             ↓
  //!             @fix GFNMCat[5379] ボックス:ランダムキー入力で上画面終了処理時に描画参照のままというASSERTで停止
  //!             ↓
  //!             これに対処するためにCanDestroyで「描画中のときは破棄できない」という判定をすることにした。
  //!             描画をオフにしてから1フレーム空けていないときはfalseを返します。
  bool CanDestroy(void) const;

  //@}

  void Update(void);

  //! @name  設定
  //@{

  //! @brief  中心位置を設定する
  //! @param[in] pos  レイアウト座標で指定する
  void SetPos(const gfl2::math::Vector2& pos);
  
  //! @brief  中心位置を取得する
  //! @return  中心位置(レイアウト座標)
  const gfl2::math::Vector2& GetPos(void) const;

  //! @brief  半径を設定する
  //! @param[in] insideRadius   内側チャートの半径。レイアウト座標で指定する。
  //! @param[in] outsideRadius  外側チャートの半径。レイアウト座標で指定する。
  virtual void SetRadius(f32 insideRadius, f32 outsideRadius);

  //! @brief  色を設定する
  //! @param[in] insideColor   内側チャートの色。(x=r,y=g,z=b,w=a)。0.0fから1.0fで指定する。
  //! @param[in] outsideColor  外側チャートの色。(x=r,y=g,z=b,w=a)。0.0fから1.0fで指定する。
  virtual void SetColor(const gfl2::math::Vector4& insideColor, const gfl2::math::Vector4& outsideColor);

  //! @brief  値を設定する
  //! @param[in] figure  内側チャートか外側チャートか。
  //! @param[in] item    項目番号。0<=val<ITEM_NUM。
  //! @param[in] val     0.0f<=val<=1.0f。1.0fのとき半径の位置。
  virtual void SetVal(Figure fig, u32 item, f32 val); 

  //@}

  //! @name  friendがアクセスする関数
  //@{
private:
  poke_3d::model::BaseModel* GetModel() const { return m_model; }
  //@}

private:
  void createModel(app::util::Heap* pHeap);  // 生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
  void setupMaterialIndex(void);
  void updateVal(void);

protected:
  struct HermitePoint
  {
    f32 frame;
    f32 value;
    f32 slopePrev;
    f32 slopeNext;
  };
  //! @brief  エルミートで補間した値を得る(NintendoWareのLayoutEditorのカーブと合わせてある)
  //! @param[in] frame              値を求めるフレーム
  //! @param[in] hermitePointArray  HermitePointの配列
  //! @param[in] hermitePointNum    hermitePointArrayの要素数
  static f32 GetHermiteValue(f32 frame, const HermitePoint* hermitePointArray, u32 hermitePointNum);

  //! @brief  m_val[fig][item]に掛ける調整値を得る
  //! @param[in] val m_val[FIGURE_NUM][ITEM_NUM]
  //! @param[in] fig
  //! @param[in] item
  //! @return  val[fig][item]に掛ける値
  virtual f32 GetAdjustVal(const f32 val[FIGURE_NUM][ITEM_NUM], u32 fig, u32 item) const { return 1.0f; }

private:
  enum CreateState
  {
    CREATE_STATE_NONE,
    CREATE_STATE_CREATING,
    CREATE_STATE_CREATED,
  };

private:
  CreateState                                                m_createState;
  app::util::Heap*                                           m_pHeap;              // 生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
  gfl2::fs::AsyncFileManager*                                m_pAsyncFileManager;  // ファイルマネージャ
  void*                                                      m_buf;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_resNode;
  poke_3d::model::BaseModel*                                 m_model;

  gfl2::math::Vector2                                        m_pos;
  f32                                                        m_insideRadius;
  f32                                                        m_outsideRadius;
  f32                                                        m_val[FIGURE_NUM][ITEM_NUM];

  s32                                                        m_insideMaterialIndex;
  s32                                                        m_outsideMaterialIndex;
};


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APPLIB_APP_TOOL_RADERCHART_H_INCLUDED

