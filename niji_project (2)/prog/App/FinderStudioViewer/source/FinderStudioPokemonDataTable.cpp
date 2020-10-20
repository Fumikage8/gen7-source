//======================================================================
/**
* @file FinderStudioPokemonDataTable.cpp
* @date 2017/02/09
* @author harada_takumi
* @brief ファインダースタジオポケモンデータテーブルアクセサ
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#include "FinderStudioPokemonDataTable.h"

// PokeTool
#include "PokeTool/include/PokeModel.h"

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/FinderStudioPokeDataTable.gaix>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)

// エクセルデータの並びと同じにする
const u32 PokemonDataTable::MOTION_TYPE_NO_TABLE[NUM_MOTION_CHECK][2] =
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
  {POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_HATE},
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

PokemonDataTable::PokemonDataTable():
  m_pBuffer(NULL)
  ,m_IsLoaded(false)
  ,m_DataSize(0)
  ,m_DataNum(0)
  ,m_LastCheckMonsNo(0)
  ,m_LastCheckFormNo(0)
  ,m_LastCalcIndex(0)
{}


/// @brief アーカイブからの読み込み
/// @param pHeap ヒープ
/// @param pWorkHeap 一時ヒープ
void PokemonDataTable::StartLoadData(gfl2::heap::HeapBase* pHeap,gfl2::heap::HeapBase* pWorkHeap)
{
  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  const u32 ARC_ID = ARCID_FINDER_STUDIO_POKE_DATA_TABLE;
  
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
    loadReq.datId = GARC_FinderStudioPokeDataTable_pokemon_data_table_BIN;
    loadReq.align = 128;
    loadReq.pRealReadSize = &m_DataSize;
    loadReq.heapForBuf = pHeap;
    loadReq.heapForReq = pWorkHeap->GetLowerHandle();
    loadReq.ppBuf = &m_pBuffer;

    fileManager->AddArcFileLoadDataReq(loadReq);
  }
}

bool PokemonDataTable::IsFinishLoadData()
{
  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  if(fileManager->IsArcFileLoadDataFinished(&m_pBuffer))
  {
    // 読み込み完了したらClose
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = ARCID_FINDER_STUDIO_POKE_DATA_TABLE;
    fileManager->SyncArcFileClose(closeReq);

    // データの個数算出
    m_DataNum = m_DataSize / sizeof(PokemonData);

    m_IsLoaded = true;

    return true;
  }

  return false;
}

/// @brief 削除処理
void PokemonDataTable::DeleteData()
{
  GflHeapFreeMemory(m_pBuffer);
  m_pBuffer = NULL;

  m_IsLoaded = false;
}

/// @brief データの取得
/// @param monsNo 図鑑No formNo フォルムNo
const PokemonDataTable::PokemonData* PokemonDataTable::GetData(u32 monsNo,u32 formNo)
{
  u32 index = this->getIndex(monsNo,formNo);
  return this->getData(index);
}

/// @brief データの取得
/// @param インデックス
const PokemonDataTable::PokemonData* PokemonDataTable::getData(u32 index) const
{
  if(!m_IsLoaded || m_DataNum == 0 || index >= m_DataNum)
  {
    GFL_ASSERT_MSG(0,"ポケモンデータが存在しません\n");
    return NULL;
  }

  return static_cast<PokemonData*>(m_pBuffer)+index;
}

/// @brief インデックスの取得
/// @param monsNo 図鑑No formNo フォルムNo
/// @return データのインデックス
u32 PokemonDataTable::getIndex(u32 monsNo,u32 formNo)
{
  // MonsNo不正値チェック
  if(monsNo == 0 || monsNo > MONSNO_END)
  {
    GFL_ASSERT_MSG(0,"MonsNoが不正です。最初のデータを返します\n");
    return 0;
  }

  // 前と同じなら計算不要
  if(monsNo == m_LastCheckMonsNo && formNo == m_LastCheckFormNo) return m_LastCalcIndex;

  int result = -1;

  for(u32 i = 0;i < m_DataNum;i++)
  {
    const PokemonData* data = this->getData(i);
    if(data->monsNo == monsNo)
    {
      // MonsNoが見つかればそれを代入。一致するフォルムが見つかればそれに上書きする
      if(data->formNo == formNo || result == -1)
      {
        result = i;
      }
    }
  }

  if(result < 0)
  {
    GFL_ASSERT_STOP_MSG(0,"ポケモンデータが見つかりません！\n");
  }

  m_LastCheckMonsNo = monsNo;
  m_LastCheckFormNo = formNo;
  m_LastCalcIndex = static_cast<u32>(result);

  return m_LastCalcIndex;
}

/// @brief モーション使用可否判定
/// @param monsNo 図鑑No
/// @param formNo フォルムNo
/// @param motionType モーションタイプ
/// @param motionNo モーションNo
bool PokemonDataTable::IsCanUseMotion(u32 monsNo,u32 formNo,PokeMotionType motionType,u32 motionNo)
{
  const PokemonData* pPokemonData = this->GetData(monsNo,formNo);
  if(pPokemonData == NULL)
  {
    GFL_ASSERT_MSG(0,"ポケモンが見つかりません\n");
    return false;
  }

  // 眠りBCはAと同じにする
  if(motionType == POKE_MOTION_TYPE_KAWAIGARI)
  {
    if(motionNo == PokeTool::MODEL_ANIME_DROWSE_B ||
      motionNo == PokeTool::MODEL_ANIME_DROWSE_C)
    {
      motionNo = PokeTool::MODEL_ANIME_DROWSE_A;
    }
  }

  // モーションのインデックスを算出
  int motionIndex = -1;
  for(u32 i = 0;i < NUM_MOTION_CHECK;i++)
  {
    if(MOTION_TYPE_NO_TABLE[i][0] == motionType &&
      MOTION_TYPE_NO_TABLE[i][1] == motionNo)
    {
      motionIndex = i;
      break;
    }
  }

  // 見つからなければモーション使用不可
  if(motionIndex < 0)
  {
    GFL_ASSERT_MSG(0,"想定されていないモーションです\n");
    return false;
  }

  // ビット抽出
  u32 data = (pPokemonData->canUseMotion1 << 24) | (pPokemonData->canUseMotion2 << 16) | (pPokemonData->canUseMotion3 << 8) | (pPokemonData->canUseMotion4);
  u32 bit = 0x80000000 >> motionIndex;
  return data & bit;
}

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)