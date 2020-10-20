#if !defined(NIJI_PROJECT_APP_UTIL_EFFECTUTIL_H_INCLUDED)
#define NIJI_PROJECT_APP_UTIL_EFFECTUTIL_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    EffectUtil.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.12.14
 @brief   エフェクトを簡便に使うためのクラス
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <Effect/include/gfl2_EffectSystem.h>
#include <Effect/include/gfl2_EffectRenderPath.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


//==============================================================================
/**
 @class     EffectUtil
 @brief     エフェクトを簡便に使うためのクラス
 */
//==============================================================================
class EffectUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectUtil);

public:
  //! @brief  モード
  enum Mode
  {
    MODE_NORMAL,                    // 通常
    MODE_1RES_1EMITTER_SET_1GROUP,  // 1リソース1エミッターセット1グループモード
  };

  //! @brief  設定
  class Description
  {
  public:
    u32  layerNum;
    u32  effectResNumMax;
    Mode mode;
    u32* effectGroupIDNumMaxArray;  // u32 effectGroupIDNumMaxArray[layerNum];配列を渡して下さい。
    Description(void)
      : layerNum(0),
        effectResNumMax(0),
        mode(MODE_NORMAL),
        effectGroupIDNumMaxArray(NULL)
    {}
  };

  //! @brief  データ 
  class Data
  {
  public:
    gfl2::Effect::EffectRenderPath::Config  effectRenderPathConfig;  // EffectRenderPathに渡すConfig
    gfl2::util::List<u32>*                  effectGroupIDList;
    Data(void) :
      effectRenderPathConfig(),
      effectGroupIDList(NULL)
    {}
  };

  //! @brief  無効なID
  static const u32 INVALID_ID = 0xFFFFFFFF;

public:
  //! @brief  コンストラクタ
  //! @note  Createは呼ばれないので、呼び出し元からCreateを呼んで下さい
  EffectUtil(void);

  //! @brief  コンストラクタ
  //! @note  Createが呼ばれます。引数の説明はCreateの説明を見て下さい。
  EffectUtil(gfl2::heap::HeapBase* heap, const Description& desc);

  //! @brief  デストラクタ
  //! @note  Destroyが呼ばれます。
  //! @note  1フレ遅れ解放は呼び出し元でやって下さい。
  virtual ~EffectUtil();

  //! @brief  生成する
  //! @param[in]  heap  デバイスヒープ。Create後も使われ続けます。
  //! @param[in]  desc  設定(呼び出し元はdescに渡したものをCreate後に破棄して構いません)
  void Create(gfl2::heap::HeapBase* heap, const Description& desc);

  //! @brief  破棄する
  //! @note  デストラクタからも呼ばれます。
  //! @note  1フレ遅れ解放は呼び出し元でやって下さい。
  void Destroy(void);


  //! @name  リソース
  //@{

  //! @brief  リソースを登録する
  //! @param[in] resBuf  bptclファイルを読み込んだもの。128バイトアラインメント。
  //! @note  nw::eft::System::EntryResourceの説明を見て下さい。resource→resBuf。s32 resId→u32 resID。
  //! @note  Create時に渡したheapを使います。
  void EffectSystemEntryResource(void* resBuf, u32 resID, gfl2::Effect::EnvInfo* envInfo=NULL);
  
  //! @brief  指定IDのリソースをエフェクトシステムから破棄します。
  //! @note  nw::eft::System::ClearResourceの説明を見て下さい。s32 resId→u32 resID。
  //! @note  Create時に渡したheapを使います。
  void EffectSystemClearResource(u32 resID);
 
  //! @brief  リソースIDを得る
  //! @return  resBufが登録されていないときINVALID_IDを返す。
  u32 GetEffectResID(void* resBuf) const;
  
  //! @brief  リソースバッファを得る
  //! @return  resIDが登録されていないときNULLを返す。
  void* GetEffectResBuf(u32 resID) const;

  //@}


  //! @name  1リソース1エミッターセット1グループモード
  //@{
  
  //! @brief  全てのエミッタセットを生成する for MODE_1RES_1EMITTER_SET_1GROUP
  void CreateAllEffectEmitterSetForMode1Res1EmitterSet1Group(void);

  //! @brief  全てのエミッタを削除する for MODE_1RES_1EMITTER_SET_1GROUP
  void KillAllEmitterSetForMode1Res1EmitterSet1Group(void);
  
  //! @brief  更新 for MODE_1RES_1EMITTER_SET_1GROUP
  void UpdateForMode1Res1EmitterSet1Group(void);

  //! @brief  エフェクトハンドルをゲットする for MODE_1RES_1EMITTER_SET_1GROUP
  gfl2::Effect::Handle* GetEffectHandleForMode1Res1EmitterSet1Group(u32 resID) const;

  //@}


  //! @brief  エフェクトシステムをゲットする
  gfl2::Effect::System* GetEffectSystem(void) const;

  //! @brief  データをゲットする
  //! @param[in]  layerIndex  Descriptionで設定したlayerNum未満
  Data* GetData(u32 layerIndex) const;

private:
  Description            m_desc;
  gfl2::Effect::GFHeap*  m_effectHeap;
  gfl2::Effect::System*  m_effectSys;
  void**                 m_effectResBufArray;  // void* m_effectResBufArray[m_desc.effectResNumMax];配列
  Data*                  m_dataArray;          // Data m_dataArray[m_desc.layerNum];配列
  gfl2::Effect::Handle*  m_effectHandleArray;  // gfl2::Effect::Handle* m_effectHandleArray[m_desc.effectResNumMax];配列  // for MODE_1RES_1EMITTER_SET_1GROUP
};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_UTIL_EFFECTUTIL_H_INCLUDED

