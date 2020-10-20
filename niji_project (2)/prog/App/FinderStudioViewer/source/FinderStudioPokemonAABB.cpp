//======================================================================
/**
* @file FinderStudioPokemonAABB.cpp
* @date 2016/12/14
* @author harada_takumi
* @brief ファインダースタジオポケモンAABB
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#include "FinderStudioPokemonAABB.h"

// PokeTool
#include "PokeTool/include/PokeModel.h"
#include "PokeTool/include/PokeModelSystem.h"

// FinderStudioStatic
#include "App/FinderStudioStatic/include/FinderStudioViewerDefine.h"
#include "App/FinderStudioStatic/include/FinderStudioDefine.h"

// niji_conv_header
#include <niji_conv_header/poke_lib/monsno_def.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/FinderStudioPokeMotionAABB.gaix>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)

const u32 PokemonAABB::MOTION_TYPE_NO_TABLE[NUM_MOTION_AABB][2] =
{
  {POKE_MOTION_TYPE_BATTLE,PokeTool::MODEL_ANIME_BT_WAIT_A},
  {POKE_MOTION_TYPE_BATTLE,PokeTool::MODEL_ANIME_BT_WAIT_B},
  {POKE_MOTION_TYPE_BATTLE,PokeTool::MODEL_ANIME_BT_WAIT_C},
  {POKE_MOTION_TYPE_BATTLE,PokeTool::MODEL_ANIME_BUTURI1},
  {POKE_MOTION_TYPE_BATTLE,PokeTool::MODEL_ANIME_BUTURI2},
  {POKE_MOTION_TYPE_BATTLE,PokeTool::MODEL_ANIME_BUTURI3},
  {POKE_MOTION_TYPE_BATTLE,PokeTool::MODEL_ANIME_BUTURI4},
  {POKE_MOTION_TYPE_BATTLE,PokeTool::MODEL_ANIME_TOKUSHU1},
  {POKE_MOTION_TYPE_BATTLE,PokeTool::MODEL_ANIME_TOKUSHU2},
  {POKE_MOTION_TYPE_BATTLE,PokeTool::MODEL_ANIME_TOKUSHU3},
  {POKE_MOTION_TYPE_BATTLE,PokeTool::MODEL_ANIME_TOKUSHU4},

  {POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_KW_WAIT},
  {POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_DROWSE_A},
  {POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_DROWSE_B},
  {POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_DROWSE_C},
  {POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_HATE,},
  {POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_QUESTION},
  {POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_HAPPY_A},
  {POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_HAPPY_B},
  {POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_HAPPY_C},
  {POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_MOVE_A},
  {POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_MOVE_B},
  {POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_MOVE_C},
  {POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_MOVE_D},
  {POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_PLAY_A},
  {POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_PLAY_B},
  {POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_MAD},
};

PokemonAABB::PokemonAABB():
  m_pBuffer(NULL)
  ,m_pPokeModel(NULL)
{}


/// @brief アーカイブからの読み込み
/// @param pHeap ヒープ
/// @param pWorkHeap 一時ヒープ
/// @param simpleParam ポケモン見た目データ
/// @param pPokeModel ポケモンモデル
/// @param pPokeModelSystem ポケモンモデルシステム（データインデックス算出に使用）
void PokemonAABB::StartLoadData(gfl2::heap::HeapBase* pHeap,
                                  gfl2::heap::HeapBase* pWorkHeap,
                                  const PokeTool::SimpleParam& simpleParam,
                                  PokeTool::PokeModel* pPokeModel,
                                  PokeTool::PokeModelSystem* pPokeModelSystem)
{
  m_pPokeModel = pPokeModel;

  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  const u32 ARC_ID = ARCID_FINDER_STUDIO_POKE_MOTION_AABB;

  // ARCファイルオープン
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARC_ID;
    openReq.heapForFile = pWorkHeap->GetLowerHandle();
    openReq.heapForReq = pWorkHeap->GetLowerHandle();
    fileManager->SyncArcFileOpen(openReq);
  }

  // 読み込み開始
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
    loadReq.arcId = ARC_ID;
    loadReq.align = 4;
    loadReq.heapForBuf = pHeap;
    loadReq.heapForReq = pWorkHeap->GetLowerHandle();

    u32 dataId = pPokeModelSystem->GetDataIdx(simpleParam.monsNo,simpleParam.formNo,simpleParam.sex);
    
    // 不正チェック
    if(dataId >= GARC_FinderStudioPokeMotionAABB_DATA_NUM)
    {
      GFL_ASSERT_STOP_MSG(0,"dataIdが不正です\n");
    }

    // モデルシェアチェック
    const PokeTool::PokeModelSystem::POKE_FLG_DATA* flagData = pPokeModelSystem->GetFlgData(simpleParam.monsNo,simpleParam.formNo,simpleParam.sex);
    if(flagData->mFlgData & PokeTool::PokeModelSystem::POKE_DATA_FLG_SHARE_MODEL)
    {
      // モデルは同じなので元フォルムのAABBを使用する
      dataId = pPokeModelSystem->GetDataIdx(simpleParam.monsNo,flagData->mSubFrom,simpleParam.sex);
    }


    loadReq.datId = dataId;
    loadReq.ppBuf = &m_pBuffer;

    fileManager->AddArcFileLoadDataReq(loadReq);
  }
}

bool PokemonAABB::IsFinishLoadData()
{
  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  if(fileManager->IsArcFileLoadDataFinished(&m_pBuffer))
  {
    // 読み込み完了したらClose
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = ARCID_FINDER_STUDIO_POKE_MOTION_AABB;
    fileManager->SyncArcFileClose(closeReq);

    // 中身チェック
    if(m_pBuffer == NULL)
    {
      GFL_ASSERT_STOP(0);
      return false;
    }

    f32* pFloat = reinterpret_cast<f32*>(m_pBuffer);
    for(u32 i = 0;i < NUM_MOTION_AABB;i++)
    {
      f32 min1 = *pFloat;
      pFloat++;
      f32 min2 = *pFloat;
      pFloat++;
      f32 min3 = *pFloat;
      pFloat++;
      f32 max1 = *pFloat;
      pFloat++;
      f32 max2 = *pFloat;
      pFloat++;
      f32 max3 = *pFloat;
      pFloat++;
      m_AABBData[i].SetMinMax(gfl2::math::Vector3(min1,min2,min3),gfl2::math::Vector3(max1,max2,max3));
    }

    return true;
  }

  return false;
}

/// @brief アンロード処理
void PokemonAABB::UnloadData()
{
  GflHeapFreeMemory(m_pBuffer);
  m_pBuffer = NULL;
}

#if PM_DEBUG
/// @brief データ決めうち用関数
void PokemonAABB::DebugSetData(u32 monsNo,u32 formNo)
{
  // 初期化
  for(u32 i = 0;i < NUM_MOTION_AABB;i++)
  {
    gfl2::math::Vector3 min(0,0,0);
    gfl2::math::Vector3 max(0,0,0);
    m_AABBData[i].SetMinMax(min,max);
  }

  // ピカチュウなら決めうち
  if(monsNo == MONSNO_PIKATYUU)
  {
    for(u32 i = 0;i < NUM_MOTION_AABB;i++)
    {
      // 待機（共通）
      if( (MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_BATTLE &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_BT_WAIT_A) ||
        (MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_KAWAIGARI &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_KW_WAIT) )
      {
        gfl2::math::Vector3 min(-15.816141f,-0.017638f,-28.744623f);
        gfl2::math::Vector3 max(26.819006f,47.355995f,14.577482f);
        m_AABBData[i].SetMinMax(min,max);
      }

      // 待機派生
      else if(MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_BATTLE &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_BT_WAIT_B)
      {
        gfl2::math::Vector3 min(-20.655785f,-0.043614f,-32.195599f);
        gfl2::math::Vector3 max(31.176912f,52.832596f,15.674466f);
        m_AABBData[i].SetMinMax(min,max);
      }

      // 物理攻撃１
      else if(MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_BATTLE &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_BUTURI1)
      {
        gfl2::math::Vector3 min(-19.257172f,-4.391941f,-48.122936f);
        gfl2::math::Vector3 max(31.326584f,89.350769f,45.052170f);
        m_AABBData[i].SetMinMax(min,max);
      }

      // 物理攻撃２
      else if(MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_BATTLE &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_BUTURI2)
      {
        gfl2::math::Vector3 min(-19.184008f,-0.323206f,-50.553299f);
        gfl2::math::Vector3 max(31.500843f,67.509598f,30.913618f);
        m_AABBData[i].SetMinMax(min,max);
      }

      // 特殊攻撃１
      else if(MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_BATTLE &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_TOKUSHU1)
      {
        gfl2::math::Vector3 min(-19.640163f,-0.051628f,-43.718178f);
        gfl2::math::Vector3 max(30.915398f,85.021744f,23.787498f);
        m_AABBData[i].SetMinMax(min,max);
      }

      // 特殊攻撃２
      else if(MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_BATTLE &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_TOKUSHU2)
      {
        gfl2::math::Vector3 min(-19.017908f,-0.258849f,-48.217445f);
        gfl2::math::Vector3 max(26.654564f,60.306110f,33.805710f);
        m_AABBData[i].SetMinMax(min,max);
      }

      // 怒り
      else if(MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_KAWAIGARI &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_HATE)
      {
        gfl2::math::Vector3 min(-24.729942f,-0.014749f,-39.127777f);
        gfl2::math::Vector3 max(39.284416f,51.647850f,29.995888f);
        m_AABBData[i].SetMinMax(min,max);
      }

      // クエスチョン
      else if(MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_KAWAIGARI &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_QUESTION)
      {
        gfl2::math::Vector3 min(-28.726414f,-0.014680f,-32.178459f);
        gfl2::math::Vector3 max(36.894783f,50.291367f,14.491013f);
        m_AABBData[i].SetMinMax(min,max);
      }

      // ハッピーA
      else if(MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_KAWAIGARI &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_HAPPY_A)
      {
        gfl2::math::Vector3 min(-16.632387f,-0.014827f,-27.784794f);
        gfl2::math::Vector3 max(26.820507f,49.017670f,14.606764f);
        m_AABBData[i].SetMinMax(min,max);
      }

      // ハッピーB
      else if(MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_KAWAIGARI &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_HAPPY_B)
      {
        gfl2::math::Vector3 min(-20.159271f,-1.378635f,-33.350334f);
        gfl2::math::Vector3 max(31.802586f,48.197483f,24.659107f);
        m_AABBData[i].SetMinMax(min,max);
      }

      // ハッピーC
      else if(MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_KAWAIGARI &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_HAPPY_C)
      {
        gfl2::math::Vector3 min(-19.662807f,-0.261675f,-30.969189f);
        gfl2::math::Vector3 max(36.036842f,56.204536f,21.141689f);
        m_AABBData[i].SetMinMax(min,max);
      }

      // 動作A
      else if(MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_KAWAIGARI &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_MOVE_A)
      {
        gfl2::math::Vector3 min(-21.172375f,-0.014720f,-34.443592f);
        gfl2::math::Vector3 max(28.397486f,50.888367f,29.730743f);
        m_AABBData[i].SetMinMax(min,max);
      }

      // 動作B
      else if(MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_KAWAIGARI &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_MOVE_B)
      {
        gfl2::math::Vector3 min(-23.750038f,-0.014680f,-34.904716f);
        gfl2::math::Vector3 max(29.182732f,49.603165f,23.195541f);
        m_AABBData[i].SetMinMax(min,max);
      }

      // 動作C
      else if(MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_KAWAIGARI &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_MOVE_C)
      {
        gfl2::math::Vector3 min(-23.788118f,-0.014778f,-27.442099f);
        gfl2::math::Vector3 max(36.980488f,52.028492f,15.145010f);
        m_AABBData[i].SetMinMax(min,max);
      }

      // 戯れるA
      else if(MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_KAWAIGARI &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_PLAY_A)
      {
        gfl2::math::Vector3 min(-17.171135f,-0.387944f,-29.445879f);
        gfl2::math::Vector3 max(34.736084f,52.123852f,15.040631f);
        m_AABBData[i].SetMinMax(min,max);
      }

      // 戯れるB
      else if(MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_KAWAIGARI &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_PLAY_B)
      {
        gfl2::math::Vector3 min(-20.237591f,-1.039887f,-34.112694f);
        gfl2::math::Vector3 max(38.088413f,58.247250f,17.409580f);
        m_AABBData[i].SetMinMax(min,max);
      }

      // 興奮
      else if(MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_KAWAIGARI &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_MAD)
      {
        gfl2::math::Vector3 min(-28.099154f,-1.557189f,-40.571434f);
        gfl2::math::Vector3 max(35.037495f,51.159195f,27.127087f);
        m_AABBData[i].SetMinMax(min,max);
      }

      // 眠り
      else if(MOTION_TYPE_NO_TABLE[i][0] == POKE_MOTION_TYPE_KAWAIGARI &&
        MOTION_TYPE_NO_TABLE[i][1] == PokeTool::MODEL_ANIME_DROWSE_A)
      {
        // 眠りは開始、ループ、終了で合成した１つのAABBを使用する
        gfl2::math::AABB drowseA,drowseB,drowseC;
        gfl2::math::Vector3 min(-21.689297f,-0.014803f,-27.442097f);
        gfl2::math::Vector3 max(36.939209f,48.830730f,26.142931f);
        drowseA.SetMinMax(min,max);
        min.Set(-22.359335f,-0.014811f,-26.370872f);
        max.Set(37.107555f,49.813980f,26.308363f);
        drowseB.SetMinMax(min,max);
        min.Set(-21.942995f,-2.276956f,-33.072186f);
        max.Set(36.939209f,49.423832f,26.142931f);
        drowseC.SetMinMax(min,max);

        drowseB = drowseA.Merge(drowseB);
        m_AABBData[i] = drowseB.Merge(drowseC);
      }

      else
      {
        // 存在しないモーション
      }
    }
  }
}
#endif

/// @brief AABBデータの取得
/// @param motionType バトルかかわいがりか
/// @param motionNo アニメーションNo
/// @return パラメータに応じたAABB 見つからなかった場合は空のAABBを返す
gfl2::math::AABB PokemonAABB::GetData(PokeMotionType motionType,u32 motionNo) const
{
  // 眠りならABCを合わせたものを返す
  if((motionType == POKE_MOTION_TYPE_KAWAIGARI &&
    motionNo == PokeTool::MODEL_ANIME_DROWSE_A) ||
    (motionType == POKE_MOTION_TYPE_KAWAIGARI &&
    motionNo == PokeTool::MODEL_ANIME_DROWSE_B) ||
    (motionType == POKE_MOTION_TYPE_KAWAIGARI &&
    motionNo == PokeTool::MODEL_ANIME_DROWSE_C))
  {
    gfl2::math::AABB drowseAABB = getData(POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_DROWSE_A);
    drowseAABB = drowseAABB.Merge(getData(POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_DROWSE_B));
    drowseAABB = drowseAABB.Merge(getData(POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_DROWSE_C));
    return drowseAABB;
  }
  else
  {
    return getData(motionType,motionNo);
  }
}

gfl2::math::AABB PokemonAABB::getData(PokeMotionType motionType,u32 motionNo) const
{
  for(u32 i = 0;i < NUM_MOTION_AABB;i++)
  {
    if(MOTION_TYPE_NO_TABLE[i][0] == motionType &&
      MOTION_TYPE_NO_TABLE[i][1] == motionNo)
    {
      gfl2::math::AABB retAABB = m_AABBData[i];
      if(m_pPokeModel)
      {
        gfl2::math::Vector4 min = retAABB.GetMin() * m_pPokeModel->GetAdjustScale();
        gfl2::math::Vector4 max = retAABB.GetMax() * m_pPokeModel->GetAdjustScale();
        retAABB.SetMinMax(min,max);
      }
      else
      {
        GFL_ASSERT_MSG(0,"PokeModelが設定されていません");
      }
      // 最小サイズで矯正する（UI上で16）
      const f32 UI_MIN_SIZE = 16.0f;
      const f32 MIN_SIZE = UI_MIN_SIZE / Static::MOVE_DIAMETER_RATE;
      this->correctAABB(MIN_SIZE,&retAABB);
      return retAABB;
    }
  }

#if PM_DEBUG
  GFL_ASSERT_MSG(0,"AABBデータが見つかりませんでした\n");
#else
  GFL_ASSERT_STOP_MSG(0,"AABBデータが見つかりませんでした\n");
#endif

  return gfl2::math::AABB();
}

/// @brief 指定したサイズ以下のとき、そのサイズに矯正する
/// @param minSize 最低サイズ
/// @param pAABB 矯正するAABB
/// @return 矯正したかどうか
bool PokemonAABB::correctAABB(f32 minSize,gfl2::math::AABB* pAABB) const
{
  GFL_ASSERT_STOP(pAABB);

  gfl2::math::Vector4 max = pAABB->GetMax();
  gfl2::math::Vector4 min = pAABB->GetMin();

  bool isCorrect = false;

  f32 xSize = max.x - min.x;
  if(xSize < minSize)
  {
    f32 diff = minSize - xSize;
    max.x += diff / 2.0f;
    min.x -= diff / 2.0f;
    isCorrect = true;
  }
  f32 zSize = max.z - min.z;
  if(zSize < minSize)
  {
    f32 diff = minSize - zSize;
    max.z += diff / 2.0f;
    min.z -= diff / 2.0f;
    isCorrect = true;
  }

  if(isCorrect)
  {
    pAABB->SetMinMax(min,max);
    return true;
  }

  return false;
}

/// @brief ファインダースタジオで使用可能なモーションか判定
/// @param motionType バトルかかわいがりか
/// @param motionNo アニメーションNo
bool PokemonAABB::IsCanUseMotion(PokeMotionType motionType,u32 motionNo)
{
  for(u32 i = 0;i < NUM_MOTION_AABB;i++)
  {
    if(MOTION_TYPE_NO_TABLE[i][0] == motionType &&
      MOTION_TYPE_NO_TABLE[i][1] == motionNo)
    {
      return true;
    }
  }

  return false;
}

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)