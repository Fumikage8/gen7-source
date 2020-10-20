//======================================================================
/**
 * @file NamiModelSet.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief 波のモデルセット
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __NAMIMODEL_SET_H_INCLUDED__
#define __NAMIMODEL_SET_H_INCLUDED__
#pragma once

#include "../Model/ModelNamiEnd.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
class MantainSurfAppManager;
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Nami)

class NamiModelSet
{
  GFL_FORBID_COPY_AND_ASSIGN(NamiModelSet);

public:

  enum NAMI_END_PARTS_POS
  {
    END_PARTS_FRONT,    // 前方の波終端
    END_PARTS_MAX
  };

  enum
  {
    NAMI_BASE_NUM = 1,                                  // ベース波の数
    NAMI_MAX_NUM = NAMI_BASE_NUM + END_PARTS_MAX * 2    // 波モデルの最大数
  };


  struct MapData
  {
    gfl2::math::Vector3   m_mapInitPos;           // マップ表示する部分の初期位置
    gfl2::math::Vector3   m_playerInitPos;        // プレイヤーの初期位置
    f32                   m_mapDepth;             // マップの奥行き
    f32                   m_mapWidth;             // マップの幅
    f32                   m_horizonHeight;        // 水平面の高さ
  };
  
public:

  NamiModelSet();
  virtual ~NamiModelSet();

  void Update(const gfl2::math::Vector3 &playerPosition);
  void UpdateAfterTraverse();

  void UpdateTimeZone(s32 timeZone);

  void Create(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pHeap, Resource::ResourceManager *pResourceManager, Effect::EffectManager* pEffectManager, MantainSurfAppManager* pAppManager, const MapData &mapData);

  void Destory();

  void AddToRenderingPipeLine(MantainSurfRenderingPipeline* pRenderingPipeLine, bool edgeFlag);

  void SetHeightAnimationFrame(f32 heigthRate);
  void ResetZPosition(const f32 resetLength);

  gfl2::math::Vector3 GetTopPosition();

  void StopEffect();
  void ResumeEffect();

private:

  void CreateCommon(Model::ModelNami* pNamiModel, gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pHeap, Resource::ResourceManager *pResourceManager, Effect::EffectManager* pEffectManager);

  void UpdatePosition(const gfl2::math::Vector3 &playerPosition);

private:

  MantainSurfAppManager* m_pAppManager;

  MapData           m_MapData;
  Model::ModelNami* m_pNamiBaseModels[NAMI_BASE_NUM];

  struct NamiEndParts
  {
    Model::ModelNami*     m_pNamiModel;     // 長さ可変部分
    Model::ModelNamiEnd*  m_pNamiEndModel;  // 波終了部分

    NamiEndParts()
    {
      m_pNamiModel = NULL;
      m_pNamiEndModel = NULL;
    }

    void Terminate()
    {
      if(m_pNamiModel)    m_pNamiModel->Terminate();
      if(m_pNamiEndModel) m_pNamiEndModel->Terminate();
    }
  };
  NamiEndParts m_NamiEndParts[END_PARTS_MAX];

  Model::ModelNami* m_pAllNamiModels[NAMI_MAX_NUM];
};

GFL_NAMESPACE_END(Nami)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __NAMIMODEL_SET_H_INCLUDED__
