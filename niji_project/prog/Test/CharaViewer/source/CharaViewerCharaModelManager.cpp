//======================================================================
/**
 * @file CharaViewerCharaModelManager.cpp
 * @date 2015/07/24 20:07:20
 * @author takebe_makoto
 * @brief キャラビューア：キャラモデル管理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include "../include/CharaViewerCharaModelManager.h"

#include <ctype.h>

#include <System/include/HeapDefine.h>
#include <AppLib/include/Util/app_util_heap.h>

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameProcManager.h>

#include <model/include/gfl2_CharaModel.h>
#include <model/include/gfl2_CharaModelFactory.h>

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

#include <fs/include/gfl2_BinLinkerAccessor.h>
// garc
#include <arc_def_index/arc_def.h>
#include <arc_index/debug_viewer_chara_model_battle.gaix>
#include <arc_index/btlv_resource.gaix>
#include <niji_conv_header/battle/trainer_extra_data_struct.h>
#include <niji_conv_header/battle/battle_data_pack.h>

// 着せ替えのインデックス
#include <System/include/DressUp.h>

// スクリーンショット
#include <System/include/Screenshot.h>

// アプリケーション用データの参照用
#include <Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h>

#include <System/include/DressUp.h>

#if defined(GF_PLATFORM_WIN32)
#define CHARA_VIEWER_SPRINTF ::sprintf_s
#else
#define CHARA_VIEWER_SPRINTF ::std::snprintf
#endif

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)

using namespace poke_3d::model;

static const s32 CHARA_VIEWER_CHARA_MODEL_MANAGER_MEM_PROC_DEV_MEM_SIZE     = 0x00E00000;

static const s32 CHARA_VIEWER_CHARA_MODEL_MANAGER_MEM_DRESSUP_ANIM_SIZE     = 0x00200000;
static const s32 CHARA_VIEWER_CHARA_MODEL_MANAGER_MEM_DRESSUP_MODEL_SIZE    = 0x00200000;
static const s32 CHARA_VIEWER_CHARA_MODEL_MANAGER_MEM_DRESSUP_INSTANCE_SIZE = 0x00100000;
static const s32 CHARA_VIEWER_CHARA_MODEL_MANAGER_MEM_DRESSUP_ALLOCATOR_SIZE = 0x00100000;

static const s32 CHARA_VIEWER_CHARA_MODEL_MANAGER_MEASURE_DUMP_BUFFER_SIZE = 40 * 1024; // 40KB 最大のCSVが32KBだったので

static s32 KISEKAE_ARCID_PARTS_LIST_BATTLE_HERO[] =
{
  ARCID_CHARA_MODEL_BATTLE_HERO_COLOR,
  ARCID_CHARA_MODEL_BATTLE_HERO_FACE,
  ARCID_CHARA_MODEL_BATTLE_HERO_HAIR,
  -1,
  ARCID_CHARA_MODEL_BATTLE_HERO_ACCEYE,
  -1,
  ARCID_CHARA_MODEL_BATTLE_HERO_BAG,
  ARCID_CHARA_MODEL_BATTLE_HERO_BNGL,
  -1,
  ARCID_CHARA_MODEL_BATTLE_HERO_BOTTOMS,
  ARCID_CHARA_MODEL_BATTLE_HERO_HAT,
  ARCID_CHARA_MODEL_BATTLE_HERO_LEGS,
  ARCID_CHARA_MODEL_BATTLE_HERO_SHOES,
  ARCID_CHARA_MODEL_BATTLE_HERO_TOPS,
  ARCID_CHARA_MODEL_BATTLE_HERO_SITU,
};
static s32 KISEKAE_ARCID_PARTS_LIST_BATTLE_HEROINE[] =
{
  ARCID_CHARA_MODEL_BATTLE_HEROINE_COLOR,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_FACE,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_HAIR,
  -1,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_ACCEYE,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_ACCHAIR,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_BAG,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_BNGL,
  -1,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_BOTTOMS,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_HAT,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_LEGS,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_SHOES,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_TOPS,
  ARCID_CHARA_MODEL_BATTLE_HEROINE_SITU,
};

static s32 KISEKAE_ARCID_PARTS_LIST_FIELD_HERO[] =
{
  ARCID_CHARA_MODEL_FIELD_HERO_COLOR,
  ARCID_CHARA_MODEL_FIELD_HERO_FACE,
  ARCID_CHARA_MODEL_FIELD_HERO_HAIR,
  -1,
  ARCID_CHARA_MODEL_FIELD_HERO_ACCEYE,
  -1,
  ARCID_CHARA_MODEL_FIELD_HERO_BAG,
  ARCID_CHARA_MODEL_FIELD_HERO_BNGL,
  -1,
  ARCID_CHARA_MODEL_FIELD_HERO_BOTTOMS,
  ARCID_CHARA_MODEL_FIELD_HERO_HAT,
  ARCID_CHARA_MODEL_FIELD_HERO_LEGS,
  ARCID_CHARA_MODEL_FIELD_HERO_SHOES,
  ARCID_CHARA_MODEL_FIELD_HERO_TOPS,
  ARCID_CHARA_MODEL_FIELD_HERO_SITU,
};
static s32 KISEKAE_ARCID_PARTS_LIST_FIELD_HEROINE[] =
{
  ARCID_CHARA_MODEL_FIELD_HEROINE_COLOR,
  ARCID_CHARA_MODEL_FIELD_HEROINE_FACE,
  ARCID_CHARA_MODEL_FIELD_HEROINE_HAIR,
  -1,
  ARCID_CHARA_MODEL_FIELD_HEROINE_ACCEYE,
  ARCID_CHARA_MODEL_FIELD_HEROINE_ACCHAIR,
  ARCID_CHARA_MODEL_FIELD_HEROINE_BAG,
  ARCID_CHARA_MODEL_FIELD_HEROINE_BNGL,
  -1,
  ARCID_CHARA_MODEL_FIELD_HEROINE_BOTTOMS,
  ARCID_CHARA_MODEL_FIELD_HEROINE_HAT,
  ARCID_CHARA_MODEL_FIELD_HEROINE_LEGS,
  ARCID_CHARA_MODEL_FIELD_HEROINE_SHOES,
  ARCID_CHARA_MODEL_FIELD_HEROINE_TOPS,
  ARCID_CHARA_MODEL_FIELD_HEROINE_SITU,
};

static const char* KISEKAE_PARTS_HEADER_LIST_HERO[] =
{
  "dressup_hero_face_parts_id",
  "dressup_hero_hair_parts_id",
  "dressup_hero_accbadge_parts_id",
  "dressup_hero_acceye_parts_id",
  NULL,
  "dressup_hero_bag_parts_id",
  "dressup_hero_bngl_parts_id",
  NULL,
  "dressup_hero_bottoms_parts_id",
  "dressup_hero_hat_parts_id",
  "dressup_hero_legs_parts_id",
  "dressup_hero_shoes_parts_id",
  "dressup_hero_tops_parts_id",
  "dressup_hero_situ_parts_id",
};

static const char* KISEKAE_PARTS_HEADER_LIST_HEROINE[] =
{
  "dressup_heroine_face_parts_id",
  "dressup_heroine_hair_parts_id",
  "dressup_heroine_accbadge_parts_id",
  "dressup_heroine_acceye_parts_id",
  "dressup_heroine_acchair_parts_id",
  "dressup_heroine_bag_parts_id",
  "dressup_heroine_bngl_parts_id",
  "dressup_heroine_body_parts_id",
  "dressup_heroine_bottoms_parts_id",
  "dressup_heroine_hat_parts_id",
  "dressup_heroine_legs_parts_id",
  "dressup_heroine_shoes_parts_id",
  "dressup_heroine_tops_parts_id",
  "dressup_heroine_situ_parts_id",
};

static const char* KISEKAE_ITEM_HEADER_LIST_HERO[] =
{
  "dressup_hero_face_item_id",
  "dressup_hero_hair_item_id",
  "dressup_hero_accbadge_item_id",
  "dressup_hero_acceye_item_id",
  NULL,
  "dressup_hero_bag_item_id",
  "dressup_hero_bngl_item_id",
  NULL,
  "dressup_hero_bottoms_item_id",
  "dressup_hero_hat_item_id",
  "dressup_hero_legs_item_id",
  "dressup_hero_shoes_item_id",
  "dressup_hero_tops_item_id",
  "dressup_hero_situ_item_id",
};

static const char* KISEKAE_ITEM_HEADER_LIST_HEROINE[] =
{
  "dressup_heroine_face_item_id",
  "dressup_heroine_hair_item_id",
  "dressup_heroine_accbadge_item_id",
  "dressup_heroine_acceye_item_id",
  "dressup_heroine_acchair_item_id",
  "dressup_heroine_bag_item_id",
  "dressup_heroine_bngl_item_id",
  "dressup_heroine_body_item_id",
  "dressup_heroine_bottoms_item_id",
  "dressup_heroine_hat_item_id",
  "dressup_heroine_legs_item_id",
  "dressup_heroine_shoes_item_id",
  "dressup_heroine_tops_item_id",
  "dressup_heroine_situ_item_id",
};

static const char* PARTS_NONE_NAME = "-- NONE --";

static const char* sc_dressUpModelNameList_hero[] =
{
  "p1_base",
  "p1_base_fi",
  "pc0001_00",
  "pc0001_00_fi",
};
static const char* sc_dressUpModelNameList_heroine[] =
{
  "p2_base",
  "p2_base_fi",
  "pc0002_00",
  "pc0002_00_fi",
};

// バッファクリア設定、初期値
static const GameSys::GameManager::BUFFER_CLEAR_SETTING sc_BufferClearDefaultSettingTable[] = 
{
  // 0.75
  {
    gfl2::gfx::Color( 0.75f, 0.75f, 0.75f, 1.0f ),
    1.0f, 
    255.0f,
    GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL
  },
  // 0.5
  {
    gfl2::gfx::Color( 0.5f, 0.5f, 0.5f, 1.0f ),
    1.0f, 
    255.0f,
    GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL
  },
  // 0.3
  {
    gfl2::gfx::Color( 0.3f, 0.3f, 0.3f, 1.0f ),
    1.0f, 
    255.0f,
    GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL
  },
  // 0.0
  {
    gfl2::gfx::Color( 0.0f, 0.0f, 0.0f, 1.0f ),
    1.0f, 
    255.0f,
    GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_COLOR | GameSys::GameManager::BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL
  },
};

static void Rot_Deg( f32 &x , f32 &z , const f32 deg )
{
  f32 sin,cos;
  const f32 wx = x;
  const f32 wz = z;
  gfl2::math::SinCosDeg(&sin, &cos, deg);
  x = wx*cos - wz*sin;
  z = wx*sin + wz*cos;
}

static u32 Get255Color(const gfl2::math::Vector3& v, u8 alpha = 255)
{
  u8 r = v.x * 255;
  u8 g = v.y * 255;
  u8 b = v.z * 255;
  u8 a = alpha;

  u32 rgba = (r << 24) | (g << 16) | (b << 8) | a;
  return rgba;
}

static s64 GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_TYPE type)
{
  s64 tick = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(type);

  //// 100以下はこまか過ぎて要らない
  //tick /= 100;
  //tick *= 100;

  return tick;
}

class GridModel
{
public:

  static GridModel* Create(gfl2::fs::AsyncFileManager* pFileManager, gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pHeap, u32 arcId)
  {
    GridModel* pGridModel = GFL_NEW(pHeap) GridModel();

    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
      req.arcId				= arcId;
      req.heapForFile	= pHeap->GetLowerHandle();
      req.heapForReq	= pHeap->GetLowerHandle();

      pFileManager->SyncArcFileOpen(req);
    }

    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
      req.arcId      = arcId;
      req.datId      = GARC_debug_viewer_chara_model_battle_grid_wingl_GFBMDLP;
#if defined(GF_PLATFORM_CTR)
      req.datId      = GARC_debug_viewer_chara_model_battle_grid_ctr_GFBMDLP;
#endif
      req.ppBuf      = &pGridModel->m_pData;
      req.heapForBuf = pHeap;
      req.align      = 128; // テクスチャは128バイトアラインメント
      req.heapForReq = pHeap->GetLowerHandle();
      req.heapForCompressed = NULL;

      pFileManager->SyncArcFileLoadData(req);
    }

    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack mdlDataPack;
      mdlDataPack.SetModelData((c8*)pGridModel->m_pData);

      pGridModel->m_pModelResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(pGfxAllocator, &mdlDataPack);
      pGridModel->m_pModelInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(pGfxAllocator, pGridModel->m_pModelResourceNode);
    }

    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
      req.arcId				= arcId;
      req.heapForReq	= pHeap->GetLowerHandle();

      pFileManager->SyncArcFileClose(req);
    }

    // 1フレ遅れ解放チェックは不要
    pGridModel->m_pModelInstanceNode->SetDrawCheckListener(false);

    return pGridModel;
  }

  ~GridModel()
  {
    GFL_SAFE_DELETE(m_pModelInstanceNode);
    GFL_SAFE_DELETE(m_pModelResourceNode);
    GflHeapSafeFreeMemory(m_pData);
  }

  bool IsVisible() const
  {
    return GFL_BOOL_CAST(m_pModelInstanceNode->IsVisible());
  }

  void SetVisible(bool visible)
  {
    m_pModelInstanceNode->SetVisible(visible);
  }

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* GetModelInstanceNode()
  {
    return m_pModelInstanceNode;
  }

private:

  GridModel() :
    m_pData(NULL),
    m_pModelResourceNode(NULL),
    m_pModelInstanceNode(NULL)
  {
  }

private:

  void* m_pData;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pModelResourceNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pModelInstanceNode;
};

// 性別ごとのパラメータ
class SexParam
{
public:
  SexParam()
  {
  }

  void Initialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pHeap, s32* arcIdList, const HeaderIdMap* pHeaderIdMap, const char** pPartsNameList, const char** pItemNameList, const char* pColorName)
  {
    // 着せ替えリソースマネージャ初期化
    for(u32 i = 0; i < CharaViewerCharaModelManager::MODEL_TAB_LIMIT; ++i)
    {
      m_DressUpModelResourceManager[i].Initialize(pFileManager, pHeap, arcIdList);
    }

    for (u32 i = 0; i < DressUpParam::ITEM_CATEGORY_COUNT; ++i)
    {
      m_pPartsIdList[i] = (pPartsNameList[i] != NULL)
        ? pHeaderIdMap->Find(pPartsNameList[i])
        : NULL;

      m_pItemIdList[i] = (pItemNameList[i] != NULL)
        ? pHeaderIdMap->Find(pItemNameList[i])
        : NULL;
    }

    m_pColorIdList = (pColorName != NULL)
      ? pHeaderIdMap->Find(pColorName)
      : NULL;

    u32* countList[] =
    {
      &m_ColorSkinCount,
      &m_ColorHairCount,
      &m_ColorEyeCount,
      &m_ColorEyeBlowCount,
      &m_ColorLipCount,
    };
    u32* beginList[] =
    {
      &m_ColorSkinBeginIndex,
      &m_ColorHairBeginIndex,
      &m_ColorEyeBeginIndex,
      &m_ColorEyeBlowBeginIndex,
      &m_ColorLipBeginIndex,
    };
    const char* nameList[] =
    {
      "dressup_color_id_skin_count",
      "dressup_color_id_hair_count",
      "dressup_color_id_eye_count",
      "dressup_color_id_eyeblow_count",
      "dressup_color_id_lip_count",
    };

    u32 type = 0;
    for (u32 i = 0; i < m_pColorIdList->GetDataCount(); ++i)
    {
      const HeaderId* pHeaderId = m_pColorIdList->GetData(i);
      if (::strcmp(pHeaderId->pName, nameList[type]) == 0)
      {
        *countList[type] = pHeaderId->id; 
        *beginList[type] = i - pHeaderId->id; 

        ++type;
        if (type >= sizeof(nameList) / sizeof(nameList[0]))
        {
          break;
        }
      }
    }
  }

  void Finalize()
  {
    // 着せ替えリソースマネージャ破棄
    for(u32 i = 0; i < CharaViewerCharaModelManager::MODEL_TAB_LIMIT; ++i)
    {
      m_DressUpModelResourceManager[i].Finalize();
    }
  }

  poke_3d::model::DressUpModelResourceManagerCore* GetDressUpModelResourceManager(u32 modelTabIndex)
  {
    return &m_DressUpModelResourceManager[modelTabIndex];
  }

  const poke_3d::model::DressUpModelResourceManagerCore* GetDressUpModelResourceManager(u32 modelTabIndex) const
  {
    return &m_DressUpModelResourceManager[modelTabIndex];
  }

  const HeaderIdList* GetPartsIdList(DressUpParam::ItemCategory itemCategory) const
  {
    return m_pPartsIdList[itemCategory];
  }
  const HeaderIdList* GetItemIdList(DressUpParam::ItemCategory itemCategory) const
  {
    return m_pItemIdList[itemCategory];
  }
  const u32 GetColorIdCount(DressUpParam::BodyCategory bodyCategory) const
  {
    u32 countList[] =
    {
      0, //BODY_CATEGORY_SEX,                    // 性別
      m_ColorSkinCount, //BODY_CATEGORY_BODY_COLOR,             // 体の色
      m_ColorEyeCount, //BODY_CATEGORY_EYE_COLOR,              // 目の色
      m_ColorHairCount, //BODY_CATEGORY_HAIR_COLOR,             // 髪の色
      m_ColorLipCount, //BODY_CATEGORY_FEMALE_LIP_COLOR,       // リップの色
    };

    return countList[bodyCategory];
  }
  const char* GetColorIdName(DressUpParam::BodyCategory bodyCategory, u32 index) const
  {
    u32 beginList[] =
    {
      0, //BODY_CATEGORY_SEX,                    // 性別
      m_ColorSkinBeginIndex, //BODY_CATEGORY_BODY_COLOR,             // 体の色
      m_ColorEyeBeginIndex, //BODY_CATEGORY_EYE_COLOR,              // 目の色
      m_ColorHairBeginIndex, //BODY_CATEGORY_HAIR_COLOR,             // 髪の色
      m_ColorLipBeginIndex, //BODY_CATEGORY_FEMALE_LIP_COLOR,       // リップの色
    };

    if (index >= this->GetColorIdCount(bodyCategory))
    {
      return NULL;
    }

    u32 headerIndex = beginList[bodyCategory] + index;
    return m_pColorIdList->GetData(headerIndex)->pName;
  }

private:
  poke_3d::model::DressUpModelResourceManagerCore m_DressUpModelResourceManager[CharaViewerCharaModelManager::MODEL_TAB_LIMIT];
  const HeaderIdList* m_pPartsIdList[DressUpParam::ITEM_CATEGORY_COUNT];
  const HeaderIdList* m_pItemIdList[DressUpParam::ITEM_CATEGORY_COUNT];
  const HeaderIdList* m_pColorIdList;

  u32 m_ColorSkinCount;
  u32 m_ColorSkinBeginIndex;

  u32 m_ColorHairCount;
  u32 m_ColorHairBeginIndex;

  u32 m_ColorEyeCount;
  u32 m_ColorEyeBeginIndex;

  u32 m_ColorEyeBlowCount;
  u32 m_ColorEyeBlowBeginIndex;

  u32 m_ColorLipCount;
  u32 m_ColorLipBeginIndex;
};

class FactoryParam
{
private:

  // ソート用の作業用名前クラス
  class TmpSortName
  {
  public:

    TmpSortName(const HeaderId* pHeaderId) :
      m_pHeaderId(pHeaderId)
    {
    }

    bool IsStartWith_bttr() const
    {
      return this->IsStartWith("bttr");
    }
    bool IsStartWith_tr() const
    {
      return this->IsStartWith("tr");
    }
    bool IsStartWith_pm() const
    {
      return this->IsStartWith("pm");
    }

  private:

    bool IsStartWith(const char* pString) const
    {
      const char* pName = m_pHeaderId->GetCharaName();

      // ::strstrは一致した先頭のポインタを返すので
      // 自分自身と同じポインタかを比較すればこの文字列で始まっているかが分かる
      return ::strstr(pName, pString) == pName;
    }

  private:

    const HeaderId* m_pHeaderId;
  };

public:

  static bool SortMotionFunc(const HeaderId* a, const HeaderId* b)
  {
    TmpSortName nameA(a);
    TmpSortName nameB(b);

    // バトルトレーナー優先
    if (nameA.IsStartWith_bttr() && !nameB.IsStartWith_bttr())
    {
      return true;
    }
    else if (!nameA.IsStartWith_bttr() && nameB.IsStartWith_bttr())
    {
      return false;
    }

    // フィールドトレーナー優先
    if (nameA.IsStartWith_tr() && !nameB.IsStartWith_tr())
    {
      return true;
    }
    else if (!nameA.IsStartWith_tr() && nameB.IsStartWith_tr())
    {
      return false;
    }

    // ポケモン優先
    if (nameA.IsStartWith_pm() && !nameB.IsStartWith_pm())
    {
      return true;
    }
    else if (!nameA.IsStartWith_pm() && nameB.IsStartWith_pm())
    {
      return false;
    }

    // 普通に名前比較
    return ::strcmp(a->GetCharaName(), b->GetCharaName()) < 0;
  }

  FactoryParam() :
    m_ModelType(CharaViewerCharaModelManager::MODEL_TYPE_BATTLE),
    m_pHeaderIdMap(NULL),
    m_pCharaIdList(NULL),
    m_pDyAnimIdList(NULL),
    m_ppSortedCharaIdList(NULL)
  {
  }

  void Initialize(
   CharaViewerCharaModelManager::ModelType modelType,
   gfl2::fs::AsyncFileManager* pFileManager,
   gfl2::heap::HeapBase* pHeap,
   gfl2::heap::HeapBase* pAnimHeap,
   gfl2::heap::HeapBase* pModelHeap,
   u32 arcId,
   s32** dressupArcIdList,
   u32 debugArcId,
   const char*** partsNameList,
   const char*** itemNameList,
   const char* colorName)
  {
    // モデルタイプ
    m_ModelType = modelType;

    // キャラモデルファクトリー初期化、フィールド用かバトル用のARCIDを指定する
    m_CharaModelFactory.Initialize(pFileManager, pAnimHeap, arcId);

    // 乱数用の関数セット
    m_CharaModelFactory.SetupRandomFunc(System::GflUse::GetPublicRand);

    // ヘッダー読み込み
    m_pHeaderIdMap = HeaderIdMap::Create(pFileManager, pHeap, debugArcId, GARC_debug_viewer_chara_model_battle_debug_viewer_chara_model_battle_BINL);
    m_pCharaIdList = m_pHeaderIdMap->Find("chara_model_id");

    // 名前でソートする
    u32 charaIdListCount = m_pCharaIdList->GetDataCount();
    m_ppSortedCharaIdList = GFL_NEW_ARRAY(pHeap) const HeaderId*[charaIdListCount];
    for (u32 i = 0; i < charaIdListCount; ++i)
    {
      m_ppSortedCharaIdList[i] = m_pCharaIdList->GetData(i);
    }
    // 着せ替えモデルは先頭がいいので２からソートを始める
    ::std::sort(&m_ppSortedCharaIdList[2], &m_ppSortedCharaIdList[charaIdListCount], SortMotionFunc);

    // 動的アニメのヘッダー読み込み
    m_pDyAnimIdList = m_pHeaderIdMap->Find("chara_model_dynamic_motion");

    // 性別ごとのパラメータ初期化
    for (u32 sex = 0; sex < DressUpParam::SEX_COUNT; ++sex)
    {
      SexParam* pSexParam = &m_SexParamList[sex];
      pSexParam->Initialize(pFileManager, pModelHeap, dressupArcIdList[sex], m_pHeaderIdMap, partsNameList[sex], itemNameList[sex], colorName);
    }
  }

  void Finalize()
  {
    // キャラモデルファクトリを破棄
    m_CharaModelFactory.Finalize();

    // 性別ごとのパラメータ破棄
    for (u32 sex = 0; sex < DressUpParam::SEX_COUNT; ++sex)
    {
      SexParam* pSexParam = &m_SexParamList[sex];
      pSexParam->Finalize();
    }

    // ヘッダー文字列破棄
    GFL_SAFE_DELETE(m_pHeaderIdMap);

    // ソート用バッファ破棄
    GFL_SAFE_DELETE_ARRAY(m_ppSortedCharaIdList);
  }

  void GetDressUpParamDefaultHero(poke_3d::model::DressUpParam* pDressUpParam) const
  {
    pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_BODY_COLOR] = DRESSUP_COLOR_ID_SKIN_SYE;                      // 体の色      
    pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_EYE_COLOR] = DRESSUP_COLOR_ID_EYE_EBK;                       // 目の色
    pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_HAIR_COLOR] = DRESSUP_COLOR_ID_HAIR_HBK;                      // 髪の色
    pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = DRESSUP_COLOR_ID_LIP_LOF;                       // リップの色

    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_FACE] = DRESSUP_ITEM_ID_P1_FACE01;                   // 顔
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAIR_STYLE] = DRESSUP_ITEM_ID_P1_HAIR_MIDIUM;        // 髪型
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCBADGE] = -1;        // バッジアクセサリ
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCEYE] = -1;        // アイアクセサリ
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCHAIR] = -1;        // ヘアアクセサリ
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BAG] = DRESSUP_ITEM_ID_P1_BAG_DRUCK01;        // バッグ
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BNGL] = -1;        // バングル
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BODY] = -1;        // ボディ
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BOTTOMS] = DRESSUP_ITEM_ID_P1_BOTTOMS_DPAN01_DRD;        // ボトムス
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAT] = DRESSUP_ITEM_ID_P1_HAT_DCAP01;        // 帽子
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_LEGS] = DRESSUP_ITEM_ID_P1_LEGS_BAREFOOT01;        // レッグ
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_SHOES] = DRESSUP_ITEM_ID_P1_SHOES_DSHOES01;        // シューズ
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_TOPS] = DRESSUP_ITEM_ID_P1_TOPS_STRIPE01_NB;        // トップス
  }

  void GetDressUpParamDefaultHeroine(poke_3d::model::DressUpParam* pDressUpParam) const
  {
    pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_BODY_COLOR] = DRESSUP_COLOR_ID_SKIN_SYE;                      // 体の色      
    pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_EYE_COLOR] = DRESSUP_COLOR_ID_EYE_EBK;                       // 目の色
    pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_HAIR_COLOR] = DRESSUP_COLOR_ID_HAIR_HBK;                      // 髪の色
    pDressUpParam->bodyParams[DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = DRESSUP_COLOR_ID_LIP_LOF;                       // リップの色

    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_FACE] = DRESSUP_ITEM_ID_P2_FACE01;                  // 顔
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAIR_STYLE] = DRESSUP_ITEM_ID_P2_HAIR_BOB;                // 髪型
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCBADGE] = -1;                                             // バッジアクセサリ
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCEYE] = -1;                                             // アイアクセサリ
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_ACCHAIR] = -1;                                             // ヘアアクセサリ
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BAG] = DRESSUP_ITEM_ID_P2_BAG_DBAG01;                  // バッグ
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BNGL] = -1;          // バングル
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BODY] = -1;                                             // ボディ
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_BOTTOMS] = DRESSUP_ITEM_ID_P2_BOTTOMS_DHOTPANTS01_GR;      // ボトムス
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_HAT] = DRESSUP_ITEM_ID_P2_HAT_DHAT01;                  // 帽子
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_LEGS] = DRESSUP_ITEM_ID_P2_LEGS_BAREFOOT01;        // レッグ
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_SHOES] = DRESSUP_ITEM_ID_P2_SHOES_DSHOES01;              // シューズ
    pDressUpParam->itemParams[DressUpParam::ITEM_CATEGORY_TOPS] = DRESSUP_ITEM_ID_P2_TOPS_DCUT01_DBE;             // トップス
  }

  poke_3d::model::DressUpModelResourceManagerCore* GetDressUpModelResourceManager(CharaViewerCharaModelManager::DressUpType dressupType, u32 modelTabIndex)
  {
    if (dressupType == CharaViewerCharaModelManager::DRESS_UP_TYPE_NONE)
    {
      return NULL;
    }

    u32 sexIndex = (dressupType == CharaViewerCharaModelManager::DRESS_UP_TYPE_HERO) ? 0 : 1;
    return m_SexParamList[sexIndex].GetDressUpModelResourceManager(modelTabIndex);
  }

  const poke_3d::model::DressUpModelResourceManagerCore* GetDressUpModelResourceManager(CharaViewerCharaModelManager::DressUpType dressupType, u32 modelTabIndex) const
  {
    if (dressupType == CharaViewerCharaModelManager::DRESS_UP_TYPE_NONE)
    {
      return NULL;
    }

    u32 sexIndex = (dressupType == CharaViewerCharaModelManager::DRESS_UP_TYPE_HERO) ? 0 : 1;
    return m_SexParamList[sexIndex].GetDressUpModelResourceManager(modelTabIndex);
  }

  poke_3d::model::CharaModelFactory* GetCharaModelFactory()
  {
    return &m_CharaModelFactory;
  }

  CharaViewerCharaModelManager::ModelType GetModelType() const
  {
    return m_ModelType;
  }

  const HeaderIdMap* GetHeaderIdMap() const
  {
    return m_pHeaderIdMap;
  }

  u32 GetCharaIdListDataCount() const
  {
    return m_pCharaIdList->GetDataCount();
  }

  const HeaderId* GetCharaIdListData(u32 index) const
  {
    return m_ppSortedCharaIdList[index];
  }

  const HeaderIdList* GetDyAnimIdList() const
  {
    return m_pDyAnimIdList;
  }

  const HeaderIdList* GetPartsIdList(CharaViewerCharaModelManager::DressUpType dressupType, DressUpParam::ItemCategory itemCategory) const
  {
    if (dressupType == CharaViewerCharaModelManager::DRESS_UP_TYPE_NONE)
    {
      return NULL;
    }

    u32 sexIndex = (dressupType == CharaViewerCharaModelManager::DRESS_UP_TYPE_HERO) ? 0 : 1;
    return m_SexParamList[sexIndex].GetPartsIdList(itemCategory);
  }

  const HeaderIdList* GetItemIdList(CharaViewerCharaModelManager::DressUpType dressupType, DressUpParam::ItemCategory itemCategory) const
  {
    if (dressupType == CharaViewerCharaModelManager::DRESS_UP_TYPE_NONE)
    {
      return NULL;
    }

    u32 sexIndex = (dressupType == CharaViewerCharaModelManager::DRESS_UP_TYPE_HERO) ? 0 : 1;
    return m_SexParamList[sexIndex].GetItemIdList(itemCategory);
  }

  u32 GetColorIdCount(DressUpParam::BodyCategory bodyCategory) const
  {
    return m_SexParamList[0].GetColorIdCount(bodyCategory);
  }
  const char* GetColorIdName(DressUpParam::BodyCategory bodyCategory, u32 index) const
  {
    return m_SexParamList[0].GetColorIdName(bodyCategory, index);
  }

private:
  CharaViewerCharaModelManager::ModelType m_ModelType;
  poke_3d::model::CharaModelFactory m_CharaModelFactory;
  SexParam m_SexParamList[DressUpModel::SEX_COUNT];
  const HeaderIdMap* m_pHeaderIdMap;
  const HeaderIdList* m_pCharaIdList;
  const HeaderIdList* m_pDyAnimIdList;
  const HeaderId** m_ppSortedCharaIdList;
};

CharaViewerCharaModelManager::CharaViewerCharaModelManager() :
  m_pDeviceHeap(NULL),
  m_pHeap(NULL),
  m_pHeap2(NULL),
  m_pHeap3(NULL),
  m_pNijiAllocator(NULL),
  m_pDressUpAllocator(NULL),
  m_StartFreeSize(0),
  m_InitSeq(0),
  m_UpdateSeq(0),
  m_EndSeq(0),
  m_pRenderingPipeLine(NULL),
  m_pDrawEnvNode(NULL),
  m_pDirectionalLightNode(NULL),
  m_pGameManager(NULL),
  m_CharaId(0),
  m_ClothColorSelectedItemCategory(DressUpParam::ITEM_CATEGORY_HAT),
  m_IsControllModeCamera(true),
  m_pGridModel(NULL),
  m_LoadModelType(MODEL_TYPE_BATTLE),
  m_SelectedModel(0),
  m_AllPlayFlag(false),
  m_UpdateType(UPDATE_TYPE_NORMAL),
  m_CaptureAnimationId(0),
  m_pCheckPartsList(NULL),
  m_BattleTrainerExtraDataBuf(NULL),
  m_FieldDebugModeEnabled(false),
  m_DebugMenuReloadRequest(false),
  m_pDressupGroup(NULL),
  m_pSaveDataItem(NULL)
{
  this->ResetCameraPosition();
  this->ResetCameraFovyNearFar();
  this->ResetLightPosition();
  this->MakeDressUpParamDefault(DRESS_UP_TYPE_NONE, NULL);
}

CharaViewerCharaModelManager::~CharaViewerCharaModelManager()
{
}

void CharaViewerCharaModelManager::Initialize(GameSys::GameManager* pGameManager, gfl2::heap::HeapBase* pDeviceHeap, b32 fieldDebugModeEnabled, b32 measureModeEnabled)
{
  m_pGameManager = pGameManager;
  m_pDeviceHeap = pDeviceHeap;
  m_FieldDebugModeEnabled = fieldDebugModeEnabled;
  m_MeasureModeEnabled = measureModeEnabled;

  // 画面のクリアカラー設定
  m_ClearColorType = 1;
  for (u32 i = 0; i < gfl2::gfx::CtrDisplayNo::DOWN; ++i)
  {
    m_pGameManager->GetBufferClearSetting((gfl2::gfx::CtrDisplayNo)i, &m_oldBufferClearSettingList[i]);
    m_pGameManager->SetBufferClearSetting((gfl2::gfx::CtrDisplayNo)i, sc_BufferClearDefaultSettingTable[m_ClearColorType]);
  }

  m_pHeap = GFL_CREATE_LOCAL_HEAP(pDeviceHeap, CHARA_VIEWER_CHARA_MODEL_MANAGER_MEM_PROC_DEV_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  GFL_ASSERT( m_pHeap != NULL );
  m_StartFreeSize = m_pHeap->GetTotalFreeSize();

  m_pHeap2 = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG); // メモリ足りないので追加で

  // PC接続時だけ大量のメモリが使える
  m_pHeap3 = NULL;
  u32 heapIdDebbuger = HEAPID_DEBUG_DEBUGGER;
  if (gfl2::heap::Manager::IsExistHeapId(heapIdDebbuger))
  {
    m_pHeap3 = gfl2::heap::Manager::GetHeapByHeapId(heapIdDebbuger); // メモリ足りないので追加で
  }

  if (m_MeasureModeEnabled)
  {
    m_pDressUpAnimHeap = GFL_CREATE_LOCAL_HEAP(m_pHeap, CHARA_VIEWER_CHARA_MODEL_MANAGER_MEM_DRESSUP_ANIM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
    m_pDressUpModelHeap = GFL_CREATE_LOCAL_HEAP(m_pHeap, CHARA_VIEWER_CHARA_MODEL_MANAGER_MEM_DRESSUP_MODEL_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
    m_pDressUpInstanceHeap =  GFL_CREATE_LOCAL_HEAP(m_pHeap, CHARA_VIEWER_CHARA_MODEL_MANAGER_MEM_DRESSUP_INSTANCE_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
    m_pDressUpAllocatorHeap =  GFL_CREATE_LOCAL_HEAP(m_pHeap, CHARA_VIEWER_CHARA_MODEL_MANAGER_MEM_DRESSUP_ALLOCATOR_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  }
  else
  {
    m_pDressUpAnimHeap = m_pHeap;
    m_pDressUpModelHeap = m_pHeap;
    m_pDressUpInstanceHeap =  m_pHeap2;
    m_pDressUpAllocatorHeap = m_pHeap2;

    // PC接続時だけ大量のメモリが使える
    if (m_pHeap3 != NULL)
    {
      m_pDressUpAnimHeap = m_pHeap3;
    }
  }

  m_pNijiAllocator = GFL_NEW(m_pHeap) System::nijiAllocator(m_pHeap);
  if (m_MeasureModeEnabled)
  {
    m_pDressUpAllocator = GFL_NEW(m_pDressUpAllocatorHeap) System::nijiAllocator(m_pDressUpAllocatorHeap);
  }
  else
  {
    m_pDressUpAllocator = m_pNijiAllocator;
  }

  m_pRenderingPipeLine = GFL_NEW(m_pHeap) CharaViewerRenderingPipeline();
  m_pRenderingPipeLine->Initialize(pGameManager->GetAsyncFileManager(), m_pHeap, m_pNijiAllocator);

  m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(m_pNijiAllocator);
  // ライト2
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode		*pLightSetNode;
    gfl2::renderingengine::scenegraph::instance::LightNode			*pDirectionalLightNode;
    gfl2::renderingengine::scenegraph::instance::LightNode			*pAmbientLightNode;

    pLightSetNode			= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(m_pNijiAllocator);
    pDirectionalLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(m_pNijiAllocator);
    pAmbientLightNode		= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(m_pNijiAllocator);

    {
      gfl2::math::Vector3 lightPos(0.0f, 10000000.0f, 10000000.0f);

      pDirectionalLightNode->SetNodeName( "DirectionalLight02" );
      gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(pDirectionalLightNode->GetNodeName()) );
      pDirectionalLightNode->SetNodeNameHash( nameHash.value );
      pDirectionalLightNode->SetLightType( gfl2::renderingengine::LightType::Directional );
      pDirectionalLightNode->SetPosition( lightPos );
      gfl2::math::Vector3 direction = -lightPos.Normalize();
      pDirectionalLightNode->SetDirection( direction );
      pDirectionalLightNode->SetColor( gfl2::math::Vector3( 0.7f, 0.7f, 0.7f ) );
    }

    {
      pAmbientLightNode->SetNodeName( "AmbientLight02" );
      gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(pAmbientLightNode->GetNodeName()) );
      pAmbientLightNode->SetNodeNameHash( nameHash.value );
      pAmbientLightNode->SetLightType( gfl2::renderingengine::LightType::Ambient );
      pAmbientLightNode->SetColor( gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ) );
    }

    pLightSetNode->SetSetNo( 2 );
    pLightSetNode->AddLight( pDirectionalLightNode );//LightSetNodeの子となる
    pLightSetNode->AddLight( pAmbientLightNode );//LightSetNodeの子となる
    m_pDrawEnvNode->AddLightSet( pLightSetNode );//DrawEnvNodeの子となる

    m_pDirectionalLightNode = pDirectionalLightNode;
  }

  gfl2::math::AABB envSpace(gfl2::math::Vector4(-500000.0f, -500000.0f, -500000.0f), gfl2::math::Vector4(500000.0f, 500000.0f, 500000.0f));
  m_pDrawEnvNode->SetAABB(envSpace);

  gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild(m_pDrawEnvNode);
  m_pRenderingPipeLine->AddDrawEnv(m_pDrawEnvNode);

  // グリッド用のモデル読み込み
  m_pGridModel = GridModel::Create(pGameManager->GetAsyncFileManager(), m_pNijiAllocator, m_pHeap, ARCID_DEBUG_VIEWER_CHARA_MODEL_BATTLE);
  m_pRenderingPipeLine->AddObject(m_pGridModel->GetModelInstanceNode(), false);

  static u32 sc_arcIdList[] =
  {
    ARCID_CHARA_MODEL_BATTLE_NORMAL,
    ARCID_CHARA_MODEL_FIELD_NORMAL,
  };
  static s32* sc_dressupArcIdListBattle[] =
  {
    KISEKAE_ARCID_PARTS_LIST_BATTLE_HERO,
    KISEKAE_ARCID_PARTS_LIST_BATTLE_HEROINE,
  };
  static s32* sc_dressupArcIdListField[] =
  {
    KISEKAE_ARCID_PARTS_LIST_FIELD_HERO,
    KISEKAE_ARCID_PARTS_LIST_FIELD_HEROINE,
  };
  static s32** sc_dressupArcIdList[] =
  {
    sc_dressupArcIdListBattle,
    sc_dressupArcIdListField,
  };
  static u32 sc_arcIdDebugList[] =
  {
    ARCID_DEBUG_VIEWER_CHARA_MODEL_BATTLE,
    ARCID_DEBUG_VIEWER_CHARA_MODEL_FIELD,
  };
  static const char** sc_partsNameList[] =
  {
    KISEKAE_PARTS_HEADER_LIST_HERO,
    KISEKAE_PARTS_HEADER_LIST_HEROINE,
  };
  static const char** sc_itemNameList[] =
  {
    KISEKAE_ITEM_HEADER_LIST_HERO,
    KISEKAE_ITEM_HEADER_LIST_HEROINE,
  };
  static const char* sc_colorName = "dressup_color_id";

  // タイプ毎の初期化
  for (u32 i = 0; i < MODEL_TYPE_COUNT; ++i)
  {
    m_pFactoryParamList[i] = GFL_NEW(m_pHeap) FactoryParam;
    
    m_pFactoryParamList[i]->Initialize(
      static_cast<ModelType>(i),
      pGameManager->GetAsyncFileManager(),
      m_pHeap,
      m_pDressUpAnimHeap,
      m_pDressUpModelHeap,
      sc_arcIdList[i],
      sc_dressupArcIdList[i],
      sc_arcIdDebugList[i],
      sc_partsNameList,
      sc_itemNameList,
      sc_colorName);
  }

  for(u32 i = 0; i < MODEL_TAB_LIMIT; ++i)
  {
    // アニメーション初期化
    {
      u32 datIdList[] =
      {
        GARC_debug_viewer_chara_model_battle_pc0001_00_ba_kisekae_GFBMOT,
        GARC_debug_viewer_chara_model_battle_pc0001_00_fi_kisekae_GFBMOT,
        GARC_debug_viewer_chara_model_battle_pc0002_00_ba_kisekae_GFBMOT,
        GARC_debug_viewer_chara_model_battle_pc0002_00_fi_kisekae_GFBMOT,
      };
      m_LoadedModel[i].m_Animation.Initialize(pGameManager->GetAsyncFileManager(), m_pNijiAllocator, m_pHeap, ARCID_DEBUG_VIEWER_CHARA_MODEL_BATTLE, datIdList);
    }
  }

  // めり込み確認用のパーツリスト読み込み
  // open
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.arcId				= ARCID_DEBUG_VIEWER_CHARA_MODEL_BATTLE;
    req.heapForFile	= m_pHeap;
    req.heapForReq	= m_pHeap;

    pGameManager->GetAsyncFileManager()->SyncArcFileOpen(req);
  }

  // load
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    req.arcId      = ARCID_DEBUG_VIEWER_CHARA_MODEL_BATTLE;
    req.datId      = GARC_debug_viewer_chara_model_battle_viewer_setting_BIN;
    req.ppBuf      = (void**)&m_pCheckPartsList;
    req.heapForBuf = m_pHeap;
    req.align      = 128; // テクスチャは128バイトアラインメント
    req.heapForReq = m_pHeap;
    req.heapForCompressed = NULL;

    pGameManager->GetAsyncFileManager()->SyncArcFileLoadData(req);
  }

  // close
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    req.arcId				= ARCID_DEBUG_VIEWER_CHARA_MODEL_BATTLE;
    req.heapForReq	= m_pHeap;

    pGameManager->GetAsyncFileManager()->SyncArcFileClose(req);
  }

  // バトル小物用のトレーナー拡張データ読み込み
  // open
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.arcId				= ARCID_BTLV_RESOURCE;
    req.heapForFile	= m_pHeap;
    req.heapForReq	= m_pHeap;

    pGameManager->GetAsyncFileManager()->SyncArcFileOpen(req);
  }

  // load
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    req.arcId      = ARCID_BTLV_RESOURCE;
    req.datId      = GARC_btlv_resource_battle_data_pack_BIN;
    req.ppBuf      = (void**)&m_BattleDataPackBuf;
    req.heapForBuf = m_pHeap;
    req.align      = 128; // テクスチャは128バイトアラインメント
    req.heapForReq = m_pHeap;
    req.heapForCompressed = NULL;

    pGameManager->GetAsyncFileManager()->SyncArcFileLoadData(req);

    gfl2::fs::BinLinkerAccessor binLinker;
    binLinker.Initialize(m_BattleDataPackBuf);

    m_BattleTrainerExtraDataBuf = binLinker.GetData(BL_IDX_BATTLE_DATA_PACK_TRAINER_EXTRA_DATA_BIN);

  }

  // close
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    req.arcId				= ARCID_BTLV_RESOURCE;
    req.heapForReq	= m_pHeap;

    pGameManager->GetAsyncFileManager()->SyncArcFileClose(req);
  }

#if defined(GF_PLATFORM_CTR)
  // シングルコマンドリストモードに設定
  gfl2::gfx::GFGL::SetSigleCommandListMode(true);
#endif

  if (m_MeasureModeEnabled)
  {
    m_DressUpAnimHeapBeginSize = m_pDressUpAnimHeap->GetTotalFreeSize();
    m_DressUpModelHeapBeginSize = m_pDressUpModelHeap->GetTotalFreeSize();
    m_DressUpInstanceHeapBeginSize = m_pDressUpInstanceHeap->GetTotalFreeSize();
    m_DressUpAllocatorHeapBeginSize = m_pDressUpAllocatorHeap->GetTotalFreeSize();
  }

  // フィールドデバッグ用に起動された
  if (m_FieldDebugModeEnabled)
  {
    this->SetLoadModelType(MODEL_TYPE_FIELD);

    // MyStatusを参照してフィールド用の着せ替えモデル読み込み
    Savedata::MyStatus* pMyStatus = m_pGameManager->GetGameData()->GetSaveData()->GetMyStatus();
    if (pMyStatus->GetSex() == PM_FEMALE)
    {
      this->SlideCharaSelector(true);
    }
    m_DressUpParam[m_SelectedModel] = pMyStatus->GetDressup();

    DressUpType dressUpType = this->GetCurrentCharaDressUpType();
    FactoryParam* pFactoryParam = this->GetCurrentFactoryParam();
    this->LoadCharactorCore(m_CharaId, dressUpType, m_DressUpParam[m_SelectedModel], pFactoryParam);
  }

  // デバッグメニュー初期化
  this->InitializeDebugMenu();
}

void CharaViewerCharaModelManager::Finalize()
{
  // デバッグメニュー破棄
  this->FinalizeDebugMenu();

  // フィールドデバッグ用に起動された
  if (m_FieldDebugModeEnabled)
  {
    // MyStatusに着せ替えを書き戻す
    Savedata::MyStatus* pMyStatus = m_pGameManager->GetGameData()->GetSaveData()->GetMyStatus();
    pMyStatus->SetDressup(m_DressUpParam[m_SelectedModel]);
  }

#if defined(GF_PLATFORM_CTR)
  // シングルコマンドリストモード終了
  gfl2::gfx::GFGL::SetSigleCommandListMode(false);
#endif

  // バトル小物用のトレーナー拡張データ破棄
  GflHeapSafeFreeMemory(m_BattleDataPackBuf);

  // めり込み確認用パーツリスト破棄
  GflHeapSafeFreeMemory(m_pCheckPartsList);

  // アニメーション破棄
  for(u32 i = 0; i < MODEL_TAB_LIMIT; ++i)
  {
    m_LoadedModel[i].m_Animation.Finalize();
  }

  // 読み込んだモデルを破棄
  for( u32 i = 0; i < MODEL_TAB_LIMIT; ++i)
  {
    m_SelectedModel = i;
    this->UnloadCharactor();
  }
  m_SelectedModel = 0;

  for (u32 i = 0; i < MODEL_TYPE_COUNT; ++i)
  {
    m_pFactoryParamList[i]->Finalize();
    GFL_SAFE_DELETE(m_pFactoryParamList[i]);
  }

  // グリッドモデル破棄
  GFL_SAFE_DELETE(m_pGridModel);


  // レンダリングパイプライン削除
  m_pRenderingPipeLine->Finalize();
  GFL_SAFE_DELETE(m_pRenderingPipeLine);

  // DrawEnvNode削除
  // この中でLightNodeなどDrawEnvNodeにぶら下がっているものも殺されるらしい
  GFL_SAFE_DELETE(m_pDrawEnvNode);

  // アロケータ破棄
  GFL_SAFE_DELETE(m_pNijiAllocator);

  // 画面のクリアカラー設定
  for (u32 i = 0; i < gfl2::gfx::CtrDisplayNo::DOWN; ++i)
  {
    m_pGameManager->SetBufferClearSetting((gfl2::gfx::CtrDisplayNo)i, m_oldBufferClearSettingList[i]);
  }

  if (m_MeasureModeEnabled)
  {
    GFL_SAFE_DELETE(m_pDressUpAllocator);
    GFL_DELETE_HEAP(m_pDressUpAnimHeap);
    GFL_DELETE_HEAP(m_pDressUpModelHeap);
    GFL_DELETE_HEAP(m_pDressUpInstanceHeap);
    GFL_DELETE_HEAP(m_pDressUpAllocatorHeap);
  }

  // メモリリークチェック
  {
    s32 endFreeSize = m_pHeap->GetTotalFreeSize();
    if (m_StartFreeSize != endFreeSize)
    {
      m_pHeap->Dump();
      GFL_ASSERT(m_StartFreeSize == endFreeSize);
    }

    GFL_DELETE_HEAP(m_pHeap);
  }
}

static wchar_t* CharaToWChara(char* buf8, u32 bufSize)
{
  static wchar_t buf16[256];

  // 大文字化
  for (u32 i = 0; i < bufSize; ++i)
  {
    buf8[i] = ::toupper(buf8[i]);
  }

  // テキトーに2バイト文字に変換
  for (u32 i = 0; i < bufSize; ++i)
  {
    buf16[i] = buf8[i];
  }

  return buf16;
}

template <poke_3d::model::DressUpParam::BodyCategory bodyCategory>
void CharaViewerCharaModelManager::DebugMenuBodyUpdate(void* pUserWork, gfl2::debug::DebugWinItem* pItem)
{
  CharaViewerCharaModelManager* pSelf = reinterpret_cast<CharaViewerCharaModelManager*>(pUserWork);
  
  LoadedModel* pLoadedModel = pSelf->GetLoadedModel();
  DressUpModelResourceManagerCore* pDressUpModelResourceManager = pLoadedModel->m_pDressUpModelResourceManager;

  // 読み込まれていないなら無視
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return;
  }
  if (pLoadedModel->m_DressUpType == DRESS_UP_TYPE_NONE)
  {
    return;
  }

  // 現在のボディパラメータ取得
  s16 bodyIndex = pSelf->m_DressUpParam[pSelf->m_SelectedModel].bodyParams[bodyCategory];
  s16 nextBodyIndex = bodyIndex;

  s32 minLimit = 0;
  s32 maxLimit = pLoadedModel->m_pFactoryParam->GetColorIdCount(bodyCategory) - 1;

  gfl2::ui::Button* pButton = pSelf->m_pGameManager->GetUiDeviceManager()->GetButton(0);

  s16 step = 1;
  if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    step *= 10;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    step *= 100;
  }

  if (pButton->IsRepeat(gfl2::ui::DIR_LEFT))
  {
    if (nextBodyIndex == minLimit)
    {
      nextBodyIndex = maxLimit;
    }
    else if (nextBodyIndex - step < minLimit)
    {
      nextBodyIndex = minLimit;
    }
    else
    {
      nextBodyIndex -= step;
    }
  }
  else if (pButton->IsRepeat(gfl2::ui::DIR_RIGHT))
  {
    if (nextBodyIndex == maxLimit)
    {
      nextBodyIndex = minLimit;
    }
    else if (nextBodyIndex + step > maxLimit)
    {
      nextBodyIndex = maxLimit;
    }
    else
    {
      nextBodyIndex += step;
    }
  }

  if (bodyIndex != nextBodyIndex)
  {
    pSelf->m_DressUpParam[pSelf->m_SelectedModel].bodyParams[bodyCategory] = nextBodyIndex;

    // リロード予約
    pSelf->m_DebugMenuReloadRequest = true;
  }
}

template <poke_3d::model::DressUpParam::BodyCategory bodyCategory>
wchar_t* CharaViewerCharaModelManager::DebugMenuBodyDraw(void* pUserWork, gfl2::debug::DebugWinItem* pItem)
{
  static const char* sc_pCategoryNameList[] =
  {
    "Color_Sex",  // BODY_CATEGORY_SEX,                    // 性別
    "Color_Body", // BODY_CATEGORY_BODY_COLOR,             // 体の色
    "Color_Eye",  // BODY_CATEGORY_EYE_COLOR,              // 目の色
    "Color_Hair", // BODY_CATEGORY_HAIR_COLOR,             // 髪の色
    "Color_Lip",  // BODY_CATEGORY_FEMALE_LIP_COLOR,       // リップの色
  };
  const char* pCategoryName = sc_pCategoryNameList[bodyCategory];

  char buf8[256];

  CharaViewerCharaModelManager* pSelf = reinterpret_cast<CharaViewerCharaModelManager*>(pUserWork);
  
  LoadedModel* pLoadedModel = pSelf->GetLoadedModel();
  DressUpModelResourceManagerCore* pDressUpModelResourceManager = pLoadedModel->m_pDressUpModelResourceManager;

  CHARA_VIEWER_SPRINTF(buf8, sizeof(buf8), "%s[-1]:-- NONE --\n", pCategoryName);

  // 読み込まれていないなら無視
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return CharaToWChara(buf8, sizeof(buf8));
  }
  if (pLoadedModel->m_DressUpType == DRESS_UP_TYPE_NONE)
  {
    return CharaToWChara(buf8, sizeof(buf8));
  }

  // 現在のボディパラメータ取得
  s16 bodyIndex = pSelf->m_DressUpParam[pSelf->m_SelectedModel].bodyParams[bodyCategory];
  if (bodyIndex < 0)
  {
    return CharaToWChara(buf8, sizeof(buf8));
  }

  // アイテム名取得
  const char* pBodyName = pLoadedModel->m_pFactoryParam->GetColorIdName(bodyCategory, bodyIndex) + 17; // 先頭の dressup_color_id_ は要らない

  CHARA_VIEWER_SPRINTF(buf8, sizeof(buf8), "%s[%d]:%s\n", pCategoryName, bodyIndex, pBodyName);

  return CharaToWChara(buf8, sizeof(buf8));
}

template <poke_3d::model::DressUpParam::ItemCategory itemCategory>
void CharaViewerCharaModelManager::DebugMenuItemUpdate(void* pUserWork, gfl2::debug::DebugWinItem* pItem)
{
  CharaViewerCharaModelManager* pSelf = reinterpret_cast<CharaViewerCharaModelManager*>(pUserWork);
  
  LoadedModel* pLoadedModel = pSelf->GetLoadedModel();
  DressUpModelResourceManagerCore* pDressUpModelResourceManager = pLoadedModel->m_pDressUpModelResourceManager;

  // 読み込まれていないなら無視
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return;
  }
  if (pLoadedModel->m_DressUpType == DRESS_UP_TYPE_NONE)
  {
    return;
  }

  // 現在のアイテムパラメータ取得
  s16 itemIndex = pSelf->m_DressUpParam[pSelf->m_SelectedModel].itemParams[itemCategory];
  s16 nextItemIndex = itemIndex;

  s32 minLimit = -1;
  s32 maxLimit = pDressUpModelResourceManager->GetDressUpItemCount(itemCategory) - 1;

  gfl2::ui::Button* pButton = pSelf->m_pGameManager->GetUiDeviceManager()->GetButton(0);

  s16 step = 1;
  if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    step *= 10;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    step *= 100;
  }

  if (pButton->IsRepeat(gfl2::ui::DIR_LEFT))
  {
    if (nextItemIndex == minLimit)
    {
      nextItemIndex = maxLimit;
    }
    else if (nextItemIndex - step < minLimit)
    {
      nextItemIndex = minLimit;
    }
    else
    {
      nextItemIndex -= step;
    }
  }
  else if (pButton->IsRepeat(gfl2::ui::DIR_RIGHT))
  {
    if (nextItemIndex == maxLimit)
    {
      nextItemIndex = minLimit;
    }
    else if (nextItemIndex + step > maxLimit)
    {
      nextItemIndex = maxLimit;
    }
    else
    {
      nextItemIndex += step;
    }
  }

  if (itemIndex != nextItemIndex)
  {
    // アイテムの組み合わせ修正
    pSelf->m_DressUpParam[pSelf->m_SelectedModel].itemParams[itemCategory] = nextItemIndex;
    pSelf->FixPartsCombination(itemCategory, nextItemIndex, pDressUpModelResourceManager);

    // リロード予約
    pSelf->m_DebugMenuReloadRequest = true;
  }
}

template <poke_3d::model::DressUpParam::ItemCategory itemCategory>
wchar_t* CharaViewerCharaModelManager::DebugMenuItemDraw(void* pUserWork, gfl2::debug::DebugWinItem* pItem)
{
  static const char* sc_pCategoryNameList[] =
  {
    "Face", //ITEM_CATEGORY_FACE,                   // 顔
    "Hair", //ITEM_CATEGORY_HAIR_STYLE,             // 髪型
    "AccB", //ITEM_CATEGORY_ACCBADGE,               // バッジアクセサリ
    "AccE", //ITEM_CATEGORY_ACCEYE,                 // アイアクセサリ
    "AccH", //ITEM_CATEGORY_ACCHAIR,                // ヘアアクセサリ
    "Bag ", //ITEM_CATEGORY_BAG,                    // バッグ
    "Bngl", //ITEM_CATEGORY_BNGL,                   // バングル
    "Body", //ITEM_CATEGORY_BODY,                   // ボディ
    "Bott", //ITEM_CATEGORY_BOTTOMS,                // ボトムス
    "Hat ", //ITEM_CATEGORY_HAT,                    // 帽子
    "Legs", //ITEM_CATEGORY_LEGS,                   // レッグ
    "Shoe", //ITEM_CATEGORY_SHOES,                  // シューズ
    "Tops", //ITEM_CATEGORY_TOPS,                   // トップス
    "Situ", //ITEM_CATEGORY_SITU,                   // シチュエーション着替え
  };
  const char* pCategoryName = sc_pCategoryNameList[itemCategory];

  char buf8[256];

  CharaViewerCharaModelManager* pSelf = reinterpret_cast<CharaViewerCharaModelManager*>(pUserWork);
  
  LoadedModel* pLoadedModel = pSelf->GetLoadedModel();
  DressUpModelResourceManagerCore* pDressUpModelResourceManager = pLoadedModel->m_pDressUpModelResourceManager;

  CHARA_VIEWER_SPRINTF(buf8, sizeof(buf8), "%s[-1]:-- NONE --\n", pCategoryName);

  // 読み込まれていないなら無視
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return CharaToWChara(buf8, sizeof(buf8));
  }
  if (pLoadedModel->m_DressUpType == DRESS_UP_TYPE_NONE)
  {
    return CharaToWChara(buf8, sizeof(buf8));
  }

  // 現在のアイテムパラメータ取得
  s16 itemIndex = pSelf->m_DressUpParam[pSelf->m_SelectedModel].itemParams[itemCategory];
  if (itemIndex < 0)
  {
    return CharaToWChara(buf8, sizeof(buf8));
  }

  // アイテム名取得
  const HeaderIdList* pItemIdList = pLoadedModel->m_pFactoryParam->GetItemIdList(pLoadedModel->m_DressUpType, itemCategory);
  const char* pItemName = pItemIdList->GetData(itemIndex)->pName + 16; // 先頭の dressup_item_id_ は要らない

  CHARA_VIEWER_SPRINTF(buf8, sizeof(buf8), "%s[%d]:%s\n", pCategoryName, itemIndex, pItemName);

  return CharaToWChara(buf8, sizeof(buf8));
}

void CharaViewerCharaModelManager::InitializeDebugMenu()
{
  gfl2::debug::DebugWinGroup* pRootGroup = gfl2::debug::DebugWin_GetTopGroup();
  m_pDressupGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"Dressup", pRootGroup);

  m_pSaveDataItem = gfl2::debug::DebugWin_AddItemValueU32(pRootGroup, m_pHeap, L"SaveDataVersion", &System::DressUpParamSaveData::s_debugVersion, 0, System::DressUpParamSaveData::DRESSUP_VERSION_COUNT - 1);

  //BODY_CATEGORY_BODY_COLOR,             // 体の色
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_pDressupGroup, m_pHeap, L"ColorBody", this, 
    DebugMenuBodyUpdate<poke_3d::model::DressUpParam::BODY_CATEGORY_BODY_COLOR>,
    DebugMenuBodyDraw<poke_3d::model::DressUpParam::BODY_CATEGORY_BODY_COLOR>);
  //BODY_CATEGORY_EYE_COLOR,              // 目の色
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_pDressupGroup, m_pHeap, L"ColorEye", this, 
    DebugMenuBodyUpdate<poke_3d::model::DressUpParam::BODY_CATEGORY_EYE_COLOR>,
    DebugMenuBodyDraw<poke_3d::model::DressUpParam::BODY_CATEGORY_EYE_COLOR>);
  //BODY_CATEGORY_HAIR_COLOR,             // 髪の色
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_pDressupGroup, m_pHeap, L"ColorHair", this, 
    DebugMenuBodyUpdate<poke_3d::model::DressUpParam::BODY_CATEGORY_HAIR_COLOR>,
    DebugMenuBodyDraw<poke_3d::model::DressUpParam::BODY_CATEGORY_HAIR_COLOR>);
  //BODY_CATEGORY_FEMALE_LIP_COLOR,       // リップの色
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_pDressupGroup, m_pHeap, L"ColorLip", this, 
    DebugMenuBodyUpdate<poke_3d::model::DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR>,
    DebugMenuBodyDraw<poke_3d::model::DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR>);

  //ITEM_CATEGORY_FACE,                   // 顔
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_pDressupGroup, m_pHeap, L"Face", this, 
    DebugMenuItemUpdate<poke_3d::model::DressUpParam::ITEM_CATEGORY_FACE>,
    DebugMenuItemDraw<poke_3d::model::DressUpParam::ITEM_CATEGORY_FACE>);
  //ITEM_CATEGORY_HAIR_STYLE,             // 髪型
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_pDressupGroup, m_pHeap, L"Hair", this, 
    DebugMenuItemUpdate<poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE>, 
    DebugMenuItemDraw<poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE>);
  //ITEM_CATEGORY_ACCEYE,                 // アイアクセサリ
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_pDressupGroup, m_pHeap, L"AccEye", this, 
    DebugMenuItemUpdate<poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE>, 
    DebugMenuItemDraw<poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE>);
  //ITEM_CATEGORY_ACCHAIR,                // ヘアアクセサリ
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_pDressupGroup, m_pHeap, L"AccHair", this, 
    DebugMenuItemUpdate<poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR>, 
    DebugMenuItemDraw<poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR>);
  //ITEM_CATEGORY_BAG,                    // バッグ
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_pDressupGroup, m_pHeap, L"Bag", this, 
    DebugMenuItemUpdate<poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG>, 
    DebugMenuItemDraw<poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG>);
  //ITEM_CATEGORY_BNGL,                   // バングル
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_pDressupGroup, m_pHeap, L"Bngl", this, 
    DebugMenuItemUpdate<poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL>, 
    DebugMenuItemDraw<poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL>);
  //ITEM_CATEGORY_BOTTOMS,                // ボトムス
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_pDressupGroup, m_pHeap, L"Bottoms", this, 
    DebugMenuItemUpdate<poke_3d::model::DressUpParam::ITEM_CATEGORY_BOTTOMS>, 
    DebugMenuItemDraw<poke_3d::model::DressUpParam::ITEM_CATEGORY_BOTTOMS>);
  //ITEM_CATEGORY_HAT,                    // 帽子
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_pDressupGroup, m_pHeap, L"Hat", this, 
    DebugMenuItemUpdate<poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT>, 
    DebugMenuItemDraw<poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT>);
  //ITEM_CATEGORY_LEGS,                   // レッグ
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_pDressupGroup, m_pHeap, L"Legs", this, 
    DebugMenuItemUpdate<poke_3d::model::DressUpParam::ITEM_CATEGORY_LEGS>, 
    DebugMenuItemDraw<poke_3d::model::DressUpParam::ITEM_CATEGORY_LEGS>);
  //ITEM_CATEGORY_SHOES,                  // シューズ
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_pDressupGroup, m_pHeap, L"Shoes", this, 
    DebugMenuItemUpdate<poke_3d::model::DressUpParam::ITEM_CATEGORY_SHOES>, 
    DebugMenuItemDraw<poke_3d::model::DressUpParam::ITEM_CATEGORY_SHOES>);
  //ITEM_CATEGORY_TOPS,                   // トップス
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_pDressupGroup, m_pHeap, L"Tops", this, 
    DebugMenuItemUpdate<poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS>, 
    DebugMenuItemDraw<poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS>);
  //ITEM_CATEGORY_SITU,                   // シチュエーション着替え
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_pDressupGroup, m_pHeap, L"Situ", this, 
    DebugMenuItemUpdate<poke_3d::model::DressUpParam::ITEM_CATEGORY_SITU>, 
    DebugMenuItemDraw<poke_3d::model::DressUpParam::ITEM_CATEGORY_SITU>);
}

void CharaViewerCharaModelManager::FinalizeDebugMenu()
{
  if (m_pDressupGroup)
  {
    gfl2::debug::DebugWin_RemoveGroup(m_pDressupGroup);
    m_pDressupGroup = NULL;
  }

  if (m_pSaveDataItem)
  {
    gfl2::debug::DebugWin_GetTopGroup()->RemoveItem(m_pSaveDataItem);
    m_pSaveDataItem = NULL;
  }
}

void CharaViewerCharaModelManager::Update()
{
  // デバッグウィンドウが開いている間は更新しない
  if (gfl2::debug::DebugWin_IsOpen())
  {
    return;
  }
  // デバッグウィンドウからのリロード予約
  if (m_DebugMenuReloadRequest)
  {
    this->ReloadCharactor();
    m_DebugMenuReloadRequest = false;
  }

  if (m_MeasureModeEnabled)
  {
    gfl2::ui::Button* pButton = m_pGameManager->GetUiDeviceManager()->GetButton(0);

    if (pButton->IsHold(gfl2::ui::BUTTON_L) && pButton->IsTrigger(gfl2::ui::BUTTON_R))
    {
      gfl2::renderingengine::scenegraph::SceneGraphManager::Dump();

      GFL_PRINT("animHeap      : 0x%08X\n", m_DressUpAnimHeapBeginSize - m_pDressUpAnimHeap->GetTotalFreeSize());
      GFL_PRINT("modelHeap     : 0x%08X\n", m_DressUpModelHeapBeginSize - m_pDressUpModelHeap->GetTotalFreeSize());
      GFL_PRINT("instanceHeap  : 0x%08X\n", m_DressUpInstanceHeapBeginSize - m_pDressUpInstanceHeap->GetTotalFreeSize());
      GFL_PRINT("allocatorHeap : 0x%08X\n", m_DressUpAllocatorHeapBeginSize - m_pDressUpAllocatorHeap->GetTotalFreeSize());
    }
  }

  switch (m_UpdateType)
  {
  case UPDATE_TYPE_NORMAL:
    this->UpdateNormal();
    break;
  case UPDATE_TYPE_CAPTURE_ALL:
    this->UpdateCaptureAll();
    break;
  case UPDATE_TYPE_MEASURE_NORMAL_ALL:
    this->UpdateMeasureNormalAll();
    break;
  case UPDATE_TYPE_MEASURE_DRESSUP_ALL:
    this->UpdateMeasureDressupAll();
    break;
  }

  // シーンのトラバース
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();
}

void CharaViewerCharaModelManager::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  if (displayNo == gfl2::gfx::CtrDisplayNo::DOWN)
  {
    return;
  }

  gfl2::gfx::DisplayDeviceHandle displayHandle = System::GflUse::m_HandleList[displayNo];
  u32 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth(displayHandle);
  u32 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight(displayHandle);
  gfl2::gfx::Surface* pBackBuffer = gfl2::gfx::GFGL::GetBackBuffer(System::GflUse::m_HandleList[displayNo]);
  gfl2::gfx::Surface* pDepthBuffer = gfl2::gfx::GFGL::GetDepthStencilBuffer(System::GflUse::m_HandleList[displayNo]);

  // 射影変換行列(パースペクティブ(透視法)射影)
  gfl2::math::Matrix44 projMatrix = gfl2::math::Matrix44::GetPerspective(
      gfl2::math::ConvDegToRad(m_CameraFovy),		// 視野角
      (float)displayWidth / (float)displayHeight,	// アスペクト比
      m_CameraNear,							// 前方投影面までの距離
      m_CameraFar,							// 後方投影面までの距離
      true);                // 画面の向きに合わせる

  m_pRenderingPipeLine->SetProjectionMatrix(projMatrix);

  // ビュー変換行列
  gfl2::math::Vector3 eyePosition(m_CameraPosition);  // 視点(カメラの位置)
  gfl2::math::Vector3 focusPosition(m_CameraTarget);  // 注視点
  gfl2::math::Vector3 upDirection(0.0f, 1.0f,  0.0f);     // カメラの上方向

  gfl2::math::Matrix34 viewMatrix = gfl2::math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

  m_pRenderingPipeLine->SetViewMatrix(viewMatrix);

  // 描画
  m_pRenderingPipeLine->SetRenderTarget(pBackBuffer);
  m_pRenderingPipeLine->SetDepthStencil(pDepthBuffer);
  m_pRenderingPipeLine->SetDisplaySize(displayWidth, displayHeight);
  m_pRenderingPipeLine->Update();
  m_pRenderingPipeLine->Execute();
}

void CharaViewerCharaModelManager::UpdateNormal()
{
  // カメラ更新
  this->UpdateCameraPosition();

  // ライト更新
  this->UpdateLightPosition();

  if (GetLoadedModel()->m_pCharaModel != NULL)
  {
    // モデル更新
    this->UpdateModelPosition();

    // スケール設定
    f32 scale = this->GetCurrentCharaScale();
    GetLoadedModel()->m_pCharaModel->SetScale(scale, scale, scale);
  }

  // アニメーション更新
  for( u32 i = 0; i < MODEL_TAB_LIMIT; ++i)
  {
    if(m_LoadedModel[i].m_pCharaModel != NULL)
    {
      m_LoadedModel[i].m_Animation.Update();

      // 歩き位置加算
      m_LoadedModel[i].m_pCharaModel->SetPosition(
        m_LoadedModel[i].m_Position + m_LoadedModel[i].m_Animation.GetWalkPosition()
        );

    }
  }
}

void CharaViewerCharaModelManager::UpdateCaptureAll()
{
  switch (m_UpdateSeq)
  {
  case 0: // 読み込み
    {
      // キャラ読み込み
      this->LoadCharactor();

      // 次へ
      ++m_UpdateSeq;
    }
    break;

  case 1: // 1フレーム待機
    {
      // 次へ
      ++m_UpdateSeq;
    }
    break;

  case 2: // キャプチャ
    {
      // キャプチャ実行
      this->Capture();

      // 次のキャラへ
      ++m_CharaId;

      // 最後のキャラならキャプチャシーケンス終了
      u32 charaMax = this->GetCurrentFactoryParam()->GetCharaIdListDataCount();
      if (static_cast<u32>(m_CharaId) >= charaMax)
      {
        m_UpdateType = UPDATE_TYPE_NORMAL;
        m_CharaId = 0;

        // 解放
        this->UnloadCharactor();

        GFL_PRINT("captureAll end\n");
        break;
      }

      // 最初へ戻る
      m_UpdateSeq = 0;
    }
    break;
  }
}

void CharaViewerCharaModelManager::UpdateMeasureNormalAll()
{
  switch (m_UpdateSeq)
  {
  case 0: // 読み込み
    {
      // キャラ読み込み
      this->LoadCharactor();

      // モーション選択：右
      this->SlideMotionSelector(0, true);
      this->GetLoadedModel()->m_Animation.Update();

      // 次へ
      ++m_UpdateSeq;
    }
    break;

  case 1: // モーション選択
    {
      // モーション選択：右
      this->SlideMotionSelector(0, true);
      this->GetLoadedModel()->m_Animation.Update();

      // モーションループ
      if (!this->IsMotionLoop())
      {
        this->ToggleLoopFlag();
      }

      // 次へ
      ++m_UpdateSeq;
    }
    break;

  // 安定させるまで適当なフレーム数待機
  default:
    {
      this->GetLoadedModel()->m_Animation.Update();

      // 次へ
      ++m_UpdateSeq;
    }
    break;

  case 30: // 計測
    {
      // 計測実行
      this->MeasureNormal();

      // 次のキャラへ
      ++m_CharaId;

      // 最後のキャラならシーケンス終了
      u32 charaMax = this->GetCurrentFactoryParam()->GetCharaIdListDataCount();
      if (static_cast<u32>(m_CharaId) >= charaMax)
      {
        m_UpdateType = UPDATE_TYPE_NORMAL;
        m_CharaId = 0;

        // 解放
        this->UnloadCharactor();

        // csvを出力
        s32 csvSize = CHARA_VIEWER_CHARA_MODEL_MANAGER_MEASURE_DUMP_BUFFER_SIZE - m_MeasureDumpRemainSize;
        m_pMeasureDumpBuffer -= csvSize;

        // HIOでファイルへ書き込み
        //gfl2::fs::PcUtil::WriteFile(m_pMeasureCSVFileName, m_pMeasureDumpBuffer, csvSize);
        // ReleaseビルドだとHIO無効にされていた、仕方ないのでコンソール出力からコピペか何かする

        // ダンプ用バッファ破棄
        GFL_DELETE_ARRAY(m_pMeasureDumpBuffer);
        m_MeasureDumpRemainSize = 0;
        m_pMeasureCSVFileName = NULL;

        // 通常モードで再起動
        {
          GameSys::GameManager* pGameManager = m_pGameManager;
          gfl2::heap::HeapBase* pDeviceHeap = m_pDeviceHeap;
          b32 fieldDebugModeEnabled = m_FieldDebugModeEnabled;

          this->Finalize();
          this->Initialize(pGameManager, pDeviceHeap, fieldDebugModeEnabled, false);
        }

        break;
      }

      // 最初へ戻る
      m_UpdateSeq = 0;
    }
    break;
  }
}

void CharaViewerCharaModelManager::UpdateMeasureDressupAll()
{
  switch (m_UpdateSeq)
  {
  case 0: // 読み込み
    {
      while (m_MeasureDressupItemCategory >= 0) // baseは無視
      {
        this->SelectNextParts(static_cast<DressUpParam::ItemCategory>(m_MeasureDressupItemCategory), true);
        if (m_DressUpParam[m_SelectedModel].itemParams[m_MeasureDressupItemCategory] >= 0)
        {
          break;
        }

        // 次のパーツカテゴリ
        ++m_MeasureDressupItemCategory;
        if (m_MeasureDressupItemCategory >= DressUpParam::ITEM_CATEGORY_COUNT)
        {
          m_UpdateType = UPDATE_TYPE_NORMAL;

          // 解放
          this->UnloadCharactor();

          // csvを出力
          s32 csvSize = CHARA_VIEWER_CHARA_MODEL_MANAGER_MEASURE_DUMP_BUFFER_SIZE - m_MeasureDumpRemainSize;
          m_pMeasureDumpBuffer -= csvSize;

          // HIOでファイルへ書き込み
          //gfl2::fs::PcUtil::WriteFile(m_pMeasureCSVFileName, m_pMeasureDumpBuffer, csvSize);
          // ReleaseビルドだとHIO無効にされていた、仕方ないのでコンソール出力からコピペか何かする

          // ダンプ用バッファ破棄
          GFL_DELETE_ARRAY(m_pMeasureDumpBuffer);
          m_MeasureDumpRemainSize = 0;
          m_pMeasureCSVFileName = NULL;

          // 通常モードで再起動
          {
            GameSys::GameManager* pGameManager = m_pGameManager;
            gfl2::heap::HeapBase* pDeviceHeap = m_pDeviceHeap;
            b32 fieldDebugModeEnabled = m_FieldDebugModeEnabled;

            this->Finalize();
            this->Initialize(pGameManager, pDeviceHeap, fieldDebugModeEnabled, false);
          }

          // 終了
          return;
        }
      }

      // キャラ読み込み
      this->ReloadCharactor();

      // モーション選択：右
      this->SlideMotionSelector(0, true);
      this->GetLoadedModel()->m_Animation.Update();

      // 次へ
      ++m_UpdateSeq;
    }
    break;

  case 1: // モーション選択
    {
      // モーション選択：右
      this->SlideMotionSelector(0, true);
      this->GetLoadedModel()->m_Animation.Update();

      // モーション選択：右
      this->SlideMotionSelector(0, false);
      this->GetLoadedModel()->m_Animation.Update();

      // モーションループ
      if (!this->IsMotionLoop())
      {
        this->ToggleLoopFlag();
      }

      // 次へ
      ++m_UpdateSeq;
    }
    break;

  // 安定させるまで適当なフレーム数待機
  default:
    {
      this->GetLoadedModel()->m_Animation.Update();

      // 次へ
      ++m_UpdateSeq;
    }
    break;

  case 30: // 計測
    {
      // 計測実行
      this->MeasureDressup();

      // baseならカテゴリを進める
      if (m_MeasureDressupItemCategory < 0)
      {
        ++m_MeasureDressupItemCategory;
      }

      // アイテム番号を進める
      ++m_MeasureDressupItemNo;

      this->SlideMotionSelector(0, false);
      this->GetLoadedModel()->m_Animation.Update();

      // 最初へ戻る
      m_UpdateSeq = 0;
    }
    break;
  }
}

void CharaViewerCharaModelManager::ResetCameraPosition()
{
  m_CameraPosition.Set(0, 0, 0);
  m_CameraTarget.Set(0, 80, 0);
  m_CameraRotate.Set(8.2f, 0, 0);
  m_CameraLength = 300;
}

void CharaViewerCharaModelManager::ResetLightPosition()
{
  m_LightRotate.Set(0, 0, 0);
}

void CharaViewerCharaModelManager::ResetCameraFovyNearFar()
{
  this->SetCameraFovyType(FOVY_TYPE_FIELD);
  this->SetCameraNearFarType(NEAR_FAR_TYPE_FIELD_NEAR);
}

void CharaViewerCharaModelManager::UpdateCameraPosition()
{
  if (m_IsControllModeCamera)
  {
    gfl2::ui::VectorDevice* pStick = m_pGameManager->GetUiDeviceManager()->GetStick(0);
    gfl2::ui::Button* pButton = m_pGameManager->GetUiDeviceManager()->GetButton(0);

    // 左スティックで回転
    if (!pButton->IsHold(gfl2::ui::BUTTON_R))
    {
      static const f32 STIC_MIN_VAL = 0.3f;
      static const f32 STIC_MID_VAL = 0.8f;
      static const f32 STIC_ROT_SPD = 2.0f;
      static const f32 STIC_ROT_X_MAX = 80.0f;

      f32 rotX = 0;
      f32 rotY = 0;
      if (gfl2::math::Abs(pStick->GetX()) > STIC_MIN_VAL)
      {
        if (gfl2::math::Abs(pStick->GetX()) > STIC_MID_VAL)
        {
          rotY += pStick->GetX() * 3;
        }
        else
        {
          rotY += pStick->GetX();
        }
      }
      if (gfl2::math::Abs(pStick->GetY()) > STIC_MIN_VAL)
      {
        if (gfl2::math::Abs(pStick->GetY()) > STIC_MID_VAL)
        {
          rotX += pStick->GetY() * 3;
        }
        else
        {
          rotX += pStick->GetY();
        }
      }

      m_CameraRotate.x += rotX * STIC_ROT_SPD;
      m_CameraRotate.x = gfl2::math::Clamp(m_CameraRotate.x, -STIC_ROT_X_MAX, STIC_ROT_X_MAX);
      m_CameraRotate.y += rotY * STIC_ROT_SPD;
    }

    // 距離の操作
    if (pButton->IsHold(gfl2::ui::BUTTON_R))
    {
      if (pButton->IsHold(gfl2::ui::DIR_DOWN))
      {
        m_CameraLength += 5;
      }
      if (pButton->IsHold(gfl2::ui::DIR_UP))
      {
        m_CameraLength -= 5;
        if (m_CameraLength < 10)
        {
          m_CameraLength = 10;
        }
      }
    }
    else
    {
      // 注視点の操作
      if (pButton->IsHold(gfl2::ui::DIR_UP))
      {
        m_CameraTarget.y += 5;
      }
      if (pButton->IsHold(gfl2::ui::DIR_DOWN))
      {
        m_CameraTarget.y -= 5;
      }
      f32 slideValX = 0;
      if (pButton->IsHold(gfl2::ui::DIR_LEFT))
      {
        slideValX = 5;
      }
      else
      if (pButton->IsHold(gfl2::ui::DIR_RIGHT))
      {
        slideValX = -5;
      }
      
      if (slideValX != 0)
      {
        f32 slideValZ = 0;
        
        Rot_Deg( slideValX , slideValZ , m_CameraRotate.y );
        
        m_CameraTarget.x += slideValX;
        m_CameraTarget.z += slideValZ;
      }
    }

    // リセット
    if (!pButton->IsHold(gfl2::ui::BUTTON_R) && pButton->IsTrigger(gfl2::ui::BUTTON_X))
    {
      this->ResetCameraPosition();
    }
  }

  gfl2::math::Vector3 camPos(0, 0, m_CameraLength);

  Rot_Deg( camPos.z , camPos.y , m_CameraRotate.x );
  Rot_Deg( camPos.x , camPos.z , m_CameraRotate.y );

  camPos += m_CameraTarget;
  
  m_CameraPosition = camPos;
}

void CharaViewerCharaModelManager::UpdateLightPosition()
{
  gfl2::ui::VectorDevice* pStick = m_pGameManager->GetUiDeviceManager()->GetStick(0);
  gfl2::ui::Button* pButton = m_pGameManager->GetUiDeviceManager()->GetButton(0);

  // Rボタンが押されていなかったら何もしない
  if (pButton->IsHold(gfl2::ui::BUTTON_R))
  {
    // 左スティックで回転
    {
      static const f32 STIC_MIN_VAL = 0.3f;
      static const f32 STIC_MID_VAL = 0.8f;
      static const f32 STIC_ROT_SPD = 2.0f;
      static const f32 STIC_ROT_X_MAX = 80.0f;

      f32 rotX = 0;
      f32 rotY = 0;
      if (gfl2::math::Abs(pStick->GetX()) > STIC_MIN_VAL)
      {
        if (gfl2::math::Abs(pStick->GetX()) > STIC_MID_VAL)
        {
          rotY += pStick->GetX() * 3;
        }
        else
        {
          rotY += pStick->GetX();
        }
      }
      if (gfl2::math::Abs(pStick->GetY()) > STIC_MIN_VAL)
      {
        if (gfl2::math::Abs(pStick->GetY()) > STIC_MID_VAL)
        {
          rotX += pStick->GetY() * 3;
        }
        else
        {
          rotX += pStick->GetY();
        }
      }

      m_LightRotate.x += rotX * STIC_ROT_SPD;
      m_LightRotate.x = gfl2::math::Clamp(m_LightRotate.x, -STIC_ROT_X_MAX, STIC_ROT_X_MAX);
      m_LightRotate.y += rotY * STIC_ROT_SPD;
    }

    // リセット
    if (pButton->IsTrigger(gfl2::ui::BUTTON_X))
    {
      this->ResetLightPosition();
    }
  }

  gfl2::math::Vector3 lightPos(0, 0, 1000000);

  Rot_Deg( lightPos.z , lightPos.y , m_LightRotate.x );
  Rot_Deg( lightPos.x , lightPos.z , m_LightRotate.y );

  m_pDirectionalLightNode->SetPosition(lightPos);
  gfl2::math::Vector3 direction = -lightPos.Normalize();
  m_pDirectionalLightNode->SetDirection(direction);
}

void CharaViewerCharaModelManager::UpdateModelPosition()
{
  if (GetLoadedModel()->m_pCharaModel == NULL)
  {
    return;
  }

  if (!m_IsControllModeCamera)
  {
    gfl2::ui::VectorDevice* pStick = m_pGameManager->GetUiDeviceManager()->GetStick(0);
    gfl2::ui::Button* pButton = m_pGameManager->GetUiDeviceManager()->GetButton(0);

    // 左スティックで回転
    {
      static const f32 STIC_MIN_VAL = 0.3f;
      static const f32 STIC_MID_VAL = 0.8f;
      static const f32 STIC_ROT_SPD = 0.1f;

      f32 rotY = 0.0f;
      if (gfl2::math::Abs(pStick->GetX()) > STIC_MIN_VAL)
      {
        if (gfl2::math::Abs(pStick->GetX()) > STIC_MID_VAL)
        {
          rotY += pStick->GetX() * 3;
        }
        else
        {
          rotY += pStick->GetX();
        }
      }

      rotY *= STIC_ROT_SPD;
      rotY += GetLoadedModel()->m_Rotation.y;
      GetLoadedModel()->m_Rotation.Set(0, rotY, 0);
    }

    // 移動
    gfl2::math::Vector3 pos = GetLoadedModel()->m_Position;
    static const f32 MOVE_SPD = 5.0f;
    if (pButton->IsHold(gfl2::ui::BUTTON_R))
    {
      if (pButton->IsHold(gfl2::ui::DIR_DOWN))
      {
        pos.y -= MOVE_SPD;
      }
      if (pButton->IsHold(gfl2::ui::DIR_UP))
      {
        pos.y += MOVE_SPD;
      }
    }
    else
    {
      if (pButton->IsHold(gfl2::ui::DIR_DOWN))
      {
        pos.z += MOVE_SPD;
      }
      if (pButton->IsHold(gfl2::ui::DIR_UP))
      {
        pos.z -= MOVE_SPD;
      }
    }

    if (pButton->IsHold(gfl2::ui::DIR_RIGHT))
    {
      pos.x += MOVE_SPD;
    }
    if (pButton->IsHold(gfl2::ui::DIR_LEFT))
    {
      pos.x -= MOVE_SPD;
    }
    GetLoadedModel()->m_Position = pos;

    // リセット
    if (pButton->IsTrigger(gfl2::ui::BUTTON_X))
    {
      this->LocateOrigin();
    }
  }
}

// 1ページ目
void CharaViewerCharaModelManager::SetLoadModelType(ModelType modelType)
{
  m_LoadModelType = modelType;
  m_CharaId = 0;
}

CharaViewerCharaModelManager::ModelType CharaViewerCharaModelManager::GetLoadModelType() const
{
  return m_LoadModelType;
}

u32 CharaViewerCharaModelManager::GetCurrentModel() const
{
  return m_SelectedModel;
}

void CharaViewerCharaModelManager::GetCurrentCharaName(const char** out_pCharaName, s32* out_pCharaId) const
{
  *out_pCharaName = this->GetCurrentFactoryParam()->GetCharaIdListData(m_CharaId)->GetCharaName();
  *out_pCharaId = m_CharaId;
}

f32 CharaViewerCharaModelManager::GetCurrentCharaScale() const
{
  return GetLoadedModel()->m_ModelScale / 100.0f;
}

CharaViewerCharaModelManager::DressUpType CharaViewerCharaModelManager::GetCurrentCharaDressUpType() const
{
  const char* pCharaName = NULL;
  s32 charaId = 0;
  this->GetCurrentCharaName(&pCharaName, &charaId);

  for (u32 i = 0; i < sizeof(sc_dressUpModelNameList_hero) / sizeof(sc_dressUpModelNameList_hero[0]); ++i)
  {
    if (::std::strcmp(pCharaName, sc_dressUpModelNameList_hero[i]) == 0)
    {
      return CharaViewerCharaModelManager::DRESS_UP_TYPE_HERO;
    }
  }
  for (u32 i = 0; i < sizeof(sc_dressUpModelNameList_heroine) / sizeof(sc_dressUpModelNameList_heroine[0]); ++i)
  {
    if (::std::strcmp(pCharaName, sc_dressUpModelNameList_heroine[i]) == 0)
    {
      return CharaViewerCharaModelManager::DRESS_UP_TYPE_HEROINE;
    }
  }

  return CharaViewerCharaModelManager::DRESS_UP_TYPE_NONE;
}

void CharaViewerCharaModelManager::MakeDressUpParamDefault(CharaViewerCharaModelManager::DressUpType dressUpType, const FactoryParam* pFactoryParam)
{
  if (dressUpType == DRESS_UP_TYPE_NONE)
  {
    for (u32 i = 0; i < DressUpParam::BODY_CATEGORY_COUNT; ++i)
    {
      m_DressUpParam[m_SelectedModel].bodyParams[i] = -1;
    }
    for (u32 i = 0; i < DressUpParam::ITEM_CATEGORY_COUNT; ++i)
    {
      m_DressUpParam[m_SelectedModel].itemParams[i] = -1;
    }
    return;
  }

  if (dressUpType == DRESS_UP_TYPE_HERO)
  {
    System::DressUp::GetDressUpParamDefaultHero(&m_DressUpParam[m_SelectedModel]);
  }
  else
  {
    System::DressUp::GetDressUpParamDefaultHeroine(&m_DressUpParam[m_SelectedModel]);
  }
}

bool CharaViewerCharaModelManager::IsCurrentCharaDressUpModel() const
{
  return (this->GetCurrentCharaDressUpType() != CharaViewerCharaModelManager::DRESS_UP_TYPE_NONE);
}

void CharaViewerCharaModelManager::SlideModelSelector(bool isForward)
{
  m_SelectedModel = (m_SelectedModel + ( isForward ? 1 : -1) ) % MODEL_TAB_LIMIT;
}

void CharaViewerCharaModelManager::SlideCharaSelector(bool isForward)
{
  m_CharaId += this->GetSlideValue(isForward);

  u32 charaMax = this->GetCurrentFactoryParam()->GetCharaIdListDataCount();
  if (m_CharaId < 0)
  {
    m_CharaId = charaMax - 1;
  }
  else if (static_cast<u32>(m_CharaId) >= charaMax)
  {
    m_CharaId = 0;
  }
}

void CharaViewerCharaModelManager::LoadCharactor()
{
  DressUpType dressUpType = this->GetCurrentCharaDressUpType();
  FactoryParam* pFactoryParam = this->GetCurrentFactoryParam();
  this->MakeDressUpParamDefault(dressUpType, pFactoryParam);

  this->LoadCharactorCore(m_CharaId, dressUpType, m_DressUpParam[m_SelectedModel], pFactoryParam);

  // 2体目以降は初期位置を右にずらす
  GetLoadedModel()->m_Position = gfl2::math::Vector3(m_SelectedModel * 150, 0, 0);
}

void CharaViewerCharaModelManager::ReloadCharactor()
{
  // 何も読み込まれていなければリロード処理はできない
  LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return;
  }

  u32 charaId = GetLoadedModel()->m_CharaId;
  DressUpType dressUpType = GetLoadedModel()->m_DressUpType;
  FactoryParam* pFactoryParam = GetLoadedModel()->m_pFactoryParam;

  // アニメーションの再生状態を退避
  CharaViewerCharaModelManager_Animation::StoreValue storeValue;
  GetLoadedModel()->m_Animation.Store(&storeValue);

  // ビーズON/OFF, 証ON/OFFを退避
  bool isBeadsOn = this->IsBeadsOn();
  bool isCharmOn = this->IsCharmOn();

  // 位置と回転を退避
  gfl2::math::Vector3 pos = GetLoadedModel()->m_Position;
  gfl2::math::Vector3 rot = GetLoadedModel()->m_Rotation;

  // リロード
  this->LoadCharactorCore(charaId, dressUpType, m_DressUpParam[m_SelectedModel], pFactoryParam);

  // 退避させていたアニメーションを復元
  GetLoadedModel()->m_Animation.Restore(&storeValue);

  // 退避させていたビーズON/OFF, 証ON/OFFを復元
  this->SetBeadsOnOff(isBeadsOn);
  this->SetCharmOnOff(isCharmOn);

  // 退避させていた位置と回転を復元
  GetLoadedModel()->m_Position = pos;
  GetLoadedModel()->m_Rotation = rot;
}

void CharaViewerCharaModelManager::LoadCharactorCore(u32 charaId, DressUpType dressUpType, const poke_3d::model::DressUpParam& dressUpParam, FactoryParam* pFactoryParam)
{
  // 既に読み込み済みなら古いモデルを解放
  LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel != NULL)
  {
    this->UnloadCharactor();
  }

  pLoadedModel->m_CharaId = charaId;
  pLoadedModel->m_DressUpType = dressUpType;
  pLoadedModel->m_pFactoryParam = pFactoryParam;
  pLoadedModel->m_pDressUpModelResourceManager = NULL;

  u32 charaModelId = pFactoryParam->GetCharaIdListData(charaId)->id;

  CharaModelFactory* pCharaModelFactory = pFactoryParam->GetCharaModelFactory();

  u32 refCounter = pLoadedModel->m_pFactoryParam->GetCharaIdListData(pLoadedModel->m_CharaId)->refCounter;
  if (refCounter == 0)
  {
    // モデル読み込みとセットアップ
    pCharaModelFactory->LoadModelSync(charaModelId);
    pCharaModelFactory->SetupModel(m_pDressUpAllocator, charaModelId);

    if (!m_MeasureModeEnabled)
    {
      // 動的アニメ読み込み
      for (u32 i = 0; i < pFactoryParam->GetDyAnimIdList()->GetDataCount(); ++i)
      {
        const HeaderId* pHeaderId = pFactoryParam->GetDyAnimIdList()->GetData(i);
        gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId(pHeaderId->id);

        // 対応するキャラモデルではないなら次へ
        if (packedDynamicAnimationId.charaId != charaModelId)
        {
          continue;
        }

        // 動的アニメ読み込みとセットアップ
        pCharaModelFactory->LoadDynamicAnimationSync(pHeaderId->id);
        pCharaModelFactory->SetupDynamicAnimation(m_pDressUpAllocator, m_pDressUpInstanceHeap, pHeaderId->id);
      }
    }

  }
  else
  {
    // すでに読み込み済み
    // 同じモデルからリソースサイズを取得
    for (u32 i = 0; i < MODEL_TAB_LIMIT; ++i)
    {
      if (i == m_SelectedModel)
      {
        continue;
      }

      if (m_LoadedModel[i].m_CharaId != pLoadedModel->m_CharaId)
      {
        continue;
      }
    }
  }

  // 着せ替えパーツの読み込みとセットアップ
  // ※ 着せ替えは1体ずつリソース管理するので参照カウントにかかわらず必ず読み込む
  u32 dressUpModelResSize = 0;
  u32 dressUpAnimResSize = 0;
  u32 dressUpTotalResSize = 0;
  if (dressUpType != DRESS_UP_TYPE_NONE)
  {
    // 着せ替えパーツ読み込み
    DressUpModelResourceManagerCore* pDressUpModelResourceManager = pFactoryParam->GetDressUpModelResourceManager(dressUpType, m_SelectedModel);
    pLoadedModel->m_pDressUpModelResourceManager = pDressUpModelResourceManager;
    pDressUpModelResourceManager->LoadDressUpPartsSync(dressUpParam);

    // 読み込んだ着せ替えパーツをセットアップ
    pDressUpModelResourceManager->SetupDressUpParts(m_pDressUpAllocator, dressUpParam);

    if (!m_MeasureModeEnabled)
    {
      // 動的アニメ読み込み
      for (u32 i = 0; i < pFactoryParam->GetDyAnimIdList()->GetDataCount(); ++i)
      {
        const HeaderId* pHeaderId = pFactoryParam->GetDyAnimIdList()->GetData(i);
        gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId(pHeaderId->id);

        // 対応するキャラモデルではないなら次へ
        if (packedDynamicAnimationId.charaId != charaModelId)
        {
          continue;
        }

        // 着せ替え用の動的アニメ読み込みとセットアップ
        pDressUpModelResourceManager->LoadDressUpDynamicAnimationSync(dressUpParam, pHeaderId->id);
        pDressUpModelResourceManager->SetupDressUpDynamicAnimation(m_pDressUpAllocator, m_pDressUpInstanceHeap, dressUpParam, pHeaderId->id);
      }
    }

    // サイズ取得
    dressUpModelResSize = pDressUpModelResourceManager->GetDressUpModelDataSize(dressUpParam);
    dressUpAnimResSize = pDressUpModelResourceManager->GetDressUpAnimationDataSize(dressUpParam);
    dressUpTotalResSize = pDressUpModelResourceManager->GetDressUpTotalDataSize(dressUpParam);
  }

  const_cast<HeaderId*>(pLoadedModel->m_pFactoryParam->GetCharaIdListData(pLoadedModel->m_CharaId))->IncrementRefCounter();

  u32 instanceBeginSize = m_pDressUpInstanceHeap->GetTotalFreeSize();
  u32 allocatorBeginSize = m_pDressUpAllocatorHeap->GetTotalFreeSize();
  if (dressUpType != DRESS_UP_TYPE_NONE)
  {
    // 着せ替えモデルのインスタンス作成
    DressUpModel* pDressUpModel = GFL_NEW(m_pDressUpInstanceHeap) poke_3d::model::DressUpModel();
    pLoadedModel->m_pCharaModel = pDressUpModel;

    // 読み込んだリソースから着せ替えモデルを作成
    DressUpModelResourceManagerCore* pDressUpModelResourceManager = pFactoryParam->GetDressUpModelResourceManager(dressUpType, m_SelectedModel);
    pCharaModelFactory->CreateDressUpModel(m_pDressUpAllocator, m_pDressUpInstanceHeap, pDressUpModel, pDressUpModelResourceManager, charaModelId);

    // 描画登録
    for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
    {
      m_pRenderingPipeLine->AddObject(pDressUpModel->GetPartsModelInstanceNode(static_cast<poke_3d::model::DressUpModel::PartsModel>(i)), true);
    }
  }
  else
  {
    // キャラモデルのインスタンス作成
    pLoadedModel->m_pCharaModel = GFL_NEW(m_pDressUpInstanceHeap) poke_3d::model::CharaModel();

    // 読み込んだリソースからモデルを作成
    pCharaModelFactory->CreateModel(m_pDressUpAllocator, m_pDressUpInstanceHeap, pLoadedModel->m_pCharaModel, charaModelId);

    // 描画登録
    m_pRenderingPipeLine->AddObject(pLoadedModel->m_pCharaModel->GetModelInstanceNode(), true);
  }

  pLoadedModel->m_SubObjectId = -1;
  char jointName[Field::MoveModel::SUBOBJECT_JOINT_NAME_SIZE] = "";

  // 小物のアタッチ
  if (pFactoryParam->GetModelType() == MODEL_TYPE_FIELD)
  {
    // フィールドモデル用

    const Field::MoveModel::ApplicationDataForCommon* pCommonData = static_cast<const Field::MoveModel::ApplicationDataForCommon*>(pCharaModelFactory->GetApplicationData(charaModelId));
    if (pCommonData->type == Field::MoveModel::TYPE_NPC)
    {
      // NPCにアタッチするサブオブジェクトが設定されているかどうか
      const Field::MoveModel::ApplicationDataForNPC* pNpcData = reinterpret_cast<const Field::MoveModel::ApplicationDataForNPC*>(pCommonData);
      pLoadedModel->m_SubObjectId = pNpcData->subobjectId;
    }
  }
  else
  {
    // バトルモデル用
    TrainerExtraInfoStruct_file_headder* pFileHeader = reinterpret_cast<TrainerExtraInfoStruct_file_headder*>(m_BattleTrainerExtraDataBuf);
    int num = pFileHeader->mNumOfElem;
    int dsize = pFileHeader->mElemSize;

    u8* ptr = reinterpret_cast<u8*>(m_BattleTrainerExtraDataBuf);

    ptr += sizeof(TrainerExtraInfoStruct_file_headder);
    ptr += (dsize * charaModelId);

    TrainerExtraInfoStruct* pExtraInfo = reinterpret_cast<TrainerExtraInfoStruct*>(ptr);

    // NPCにアタッチするサブオブジェクトが設定されているかどうか
    pLoadedModel->m_SubObjectId = pExtraInfo->mAccessoryID;

    if (pLoadedModel->m_SubObjectId > 0)
    {
      STATIC_ASSERT(Field::MoveModel::SUBOBJECT_JOINT_NAME_SIZE >= sizeof(pExtraInfo->mJointStr));

      for (u32 i = 0; i < Field::MoveModel::SUBOBJECT_JOINT_NAME_SIZE; ++i)
      {
        jointName[i] = pExtraInfo->mJointStr[i];
      }
    }
  }

  if (pLoadedModel->m_SubObjectId > 0)
  {
    u32 subRefCounter = pLoadedModel->m_pFactoryParam->GetCharaIdListData(pLoadedModel->m_SubObjectId)->refCounter;

    // サブオブジェクトを読み込み
    if (subRefCounter == 0)
    {
      pCharaModelFactory->LoadModelSync(pLoadedModel->m_SubObjectId);
      pCharaModelFactory->SetupModel(m_pDressUpAllocator, pLoadedModel->m_SubObjectId);

      if (!m_MeasureModeEnabled)
      {
        // 動的アニメ読み込み
        for (u32 i = 0; i < pFactoryParam->GetDyAnimIdList()->GetDataCount(); ++i)
        {
          const HeaderId* pHeaderId = pFactoryParam->GetDyAnimIdList()->GetData(i);
          gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId(pHeaderId->id);

          // 対応するキャラモデルではないなら次へ
          if (packedDynamicAnimationId.charaId != pLoadedModel->m_SubObjectId)
          {
            continue;
          }

          // 動的アニメ読み込みとセットアップ
          pCharaModelFactory->LoadDynamicAnimationSync(pHeaderId->id);
          pCharaModelFactory->SetupDynamicAnimation(m_pDressUpAllocator, m_pDressUpInstanceHeap, pHeaderId->id);
        }
      }
    }

    const_cast<HeaderId*>(pLoadedModel->m_pFactoryParam->GetCharaIdListData(pLoadedModel->m_SubObjectId))->IncrementRefCounter();

    // フィールドモデルならアプリ用データ領域にアタッチするジョイント名
    if (pFactoryParam->GetModelType() == MODEL_TYPE_FIELD)
    {
      const Field::MoveModel::ApplicationDataForSubobject* pSubObjectData = static_cast<const Field::MoveModel::ApplicationDataForSubobject*>(pCharaModelFactory->GetApplicationData(pLoadedModel->m_SubObjectId));

      for (u32 i = 0; i < Field::MoveModel::SUBOBJECT_JOINT_NAME_SIZE; ++i)
      {
        jointName[i] = static_cast<char>(pSubObjectData->jointName[i]);
      }
    }

    // サブオブジェクトのインスタンス作成
    pLoadedModel->m_pSubObjectModel = GFL_NEW(m_pDressUpInstanceHeap) poke_3d::model::CharaModel();
    pCharaModelFactory->CreateModel(m_pDressUpAllocator, m_pDressUpInstanceHeap, pLoadedModel->m_pSubObjectModel, pLoadedModel->m_SubObjectId);

    s32 jointIndex = pLoadedModel->m_pCharaModel->GetJointIndex(jointName);
    pLoadedModel->m_pSubObjectModel->SetParent(pLoadedModel->m_pCharaModel, jointIndex);

    // サブオブジェクトを描画登録
    m_pRenderingPipeLine->AddObject(pLoadedModel->m_pSubObjectModel->GetModelInstanceNode(), true);

    // 1フレ遅れ解放チェックは不要
    pLoadedModel->m_pSubObjectModel->SetDestroyCheckEnable(false);
  }

  u32 instanceEndSize = m_pDressUpInstanceHeap->GetTotalFreeSize();
  u32 allocatorEndSize = m_pDressUpAllocatorHeap->GetTotalFreeSize();
  u32 instanceSize = instanceBeginSize - instanceEndSize;

  if (m_pDressUpInstanceHeap != m_pDressUpAllocatorHeap)
  {
    u32 allocatorSize = allocatorBeginSize - allocatorEndSize;
    instanceSize += allocatorSize;
  }

  // リソースサイズとインスタンスサイズ設定
  pLoadedModel->m_NormalResModelSize = pCharaModelFactory->GetModelDataSize(charaId);
  pLoadedModel->m_NormalResAnimSize = pCharaModelFactory->GetAnimationDataSize(charaId);
  pLoadedModel->m_NormalResTotalSize = pCharaModelFactory->GetTotalDataSize(charaId);
  if (pLoadedModel->m_SubObjectId > 0)
  {
    pLoadedModel->m_NormalResModelSize += pCharaModelFactory->GetModelDataSize(pLoadedModel->m_SubObjectId);
    pLoadedModel->m_NormalResAnimSize += pCharaModelFactory->GetAnimationDataSize(pLoadedModel->m_SubObjectId);
    pLoadedModel->m_NormalResTotalSize += pCharaModelFactory->GetTotalDataSize(pLoadedModel->m_SubObjectId);
  }

  pLoadedModel->m_DressUpResModelSize = dressUpModelResSize;
  pLoadedModel->m_DressUpResAnimSize = dressUpAnimResSize;
  pLoadedModel->m_DressUpResTotalSize = dressUpTotalResSize;
  pLoadedModel->m_InstanceSize = instanceSize;

  // 1フレ遅れ解放チェックは不要
  pLoadedModel->m_pCharaModel->SetDestroyCheckEnable(false);

  // アニメーション管理用のキャラモデル変更
  const char* pCharaName = NULL;
  s32 tmpCharaId = 0;
  this->GetCurrentCharaName(&pCharaName, &tmpCharaId);

  const HeaderIdList* pMotionIdList = pFactoryParam->GetHeaderIdMap()->Find(pCharaName);
  if ((pMotionIdList == NULL) && (dressUpType == DRESS_UP_TYPE_NONE))
  {
    // バトルは先頭にBTが付いている可能性
    if (pFactoryParam->GetModelType() == MODEL_TYPE_BATTLE)
    {
      pMotionIdList = pFactoryParam->GetHeaderIdMap()->Find(&pCharaName[2]);
    }
  }
  else if ((pMotionIdList == NULL) && (dressUpType != DRESS_UP_TYPE_NONE))
  {
    // 自機はモーション名とIDが一致していない
    if (dressUpType == DRESS_UP_TYPE_HEROINE)
    {
      for (u32 i = 0; i < sizeof(sc_dressUpModelNameList_heroine) / sizeof(sc_dressUpModelNameList_heroine[0]); ++i)
      {
        pMotionIdList = pFactoryParam->GetHeaderIdMap()->Find(sc_dressUpModelNameList_heroine[i]);
        if (pMotionIdList != NULL)
        {
          break;
        }
      }
    }
    else if (dressUpType == DRESS_UP_TYPE_HERO)
    {
      for (u32 i = 0; i < sizeof(sc_dressUpModelNameList_hero) / sizeof(sc_dressUpModelNameList_hero[0]); ++i)
      {
        pMotionIdList = pFactoryParam->GetHeaderIdMap()->Find(sc_dressUpModelNameList_hero[i]);
        if (pMotionIdList != NULL)
        {
          break;
        }
      }
    }
  }

  // 通常アニメ数
  u32 motionIdListCount = 0;
  if (pMotionIdList != NULL)
  {
    ++motionIdListCount;
  }

  // 動的アニメ数
  for (u32 i = 0; i < pFactoryParam->GetDyAnimIdList()->GetDataCount(); ++i)
  {
    const HeaderId* pHeaderId = pFactoryParam->GetDyAnimIdList()->GetData(i);
    gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId(pHeaderId->id);

    // 対応するキャラモデルではないなら次へ
    if (packedDynamicAnimationId.charaId != charaModelId)
    {
      continue;
    }

    // ヘッダーが見つからなければ次へ
    const HeaderIdList* pDyAnimIdList = pFactoryParam->GetHeaderIdMap()->Find(pHeaderId->pName);
    if (pDyAnimIdList == NULL)
    {
      continue;
    }

    ++motionIdListCount;
  }

  pLoadedModel->m_MotionIdListCount = motionIdListCount;
  pLoadedModel->m_pMotionIdList = NULL;

  if (motionIdListCount > 0)
  {
    pLoadedModel->m_pMotionIdList = GFL_NEW_ARRAY(m_pDressUpInstanceHeap) const HeaderIdList*[motionIdListCount];

    u32 tmpIndex = 0;

    // 通常アニメ
    if (pMotionIdList != NULL)
    {
      pLoadedModel->m_pMotionIdList[tmpIndex++] = pMotionIdList;
    }

    // 動的アニメ
    for (u32 i = 0; i < pFactoryParam->GetDyAnimIdList()->GetDataCount(); ++i)
    {
      const HeaderId* pHeaderId = pFactoryParam->GetDyAnimIdList()->GetData(i);
      gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId(pHeaderId->id);

      // 対応するキャラモデルではないなら次へ
      if (packedDynamicAnimationId.charaId != charaModelId)
      {
        continue;
      }

      // ヘッダーが見つからなければ次へ
      const HeaderIdList* pDyAnimIdList = pFactoryParam->GetHeaderIdMap()->Find(pHeaderId->pName);
      if (pDyAnimIdList == NULL)
      {
        continue;
      }

      pLoadedModel->m_pMotionIdList[tmpIndex++] = pFactoryParam->GetHeaderIdMap()->Find(pHeaderId->pName);
    }
  }

  // アニメーション管理クラスに設定する
  pLoadedModel->m_Animation.ChangeCharaModel(m_pDressUpInstanceHeap, pLoadedModel->m_pCharaModel, pLoadedModel->m_pSubObjectModel, pLoadedModel->m_pMotionIdList, pLoadedModel->m_MotionIdListCount);

  if (m_MeasureModeEnabled)
  {
    // アニメーション用のメモリ確保
    if (m_DressUpParam[m_SelectedModel].itemParams[DressUpParam::ITEM_CATEGORY_FACE] >= 0)
    {
      this->SlideEyeSelector(true);
      pLoadedModel->m_pCharaModel->UpdateEye();
    }
    u32 resCount = pLoadedModel->m_pCharaModel->GetAnimationPackList()->GetAnimationPack(0)->GetResourceCount();
    bool smoothFlag = (pLoadedModel->m_pFactoryParam->GetModelType() == MODEL_TYPE_FIELD);
    for (u32 i = 0; i < resCount; ++i)
    {
      pLoadedModel->m_pCharaModel->AllocateAnimation(m_pDressUpInstanceHeap, smoothFlag, i);
    }
  }
}

void CharaViewerCharaModelManager::UnloadCharactor()
{
  // 何も読み込まれていないなら無視
  LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return;
  }
  
  FactoryParam* pFactoryParam = pLoadedModel->m_pFactoryParam;
  CharaModelFactory* pCharaModelFactory = pFactoryParam->GetCharaModelFactory();

  u32 charaModelId = pFactoryParam->GetCharaIdListData(pLoadedModel->m_CharaId)->id;
  u32 refCounter = pFactoryParam->GetCharaIdListData(pLoadedModel->m_CharaId)->refCounter;

  DressUpType dressUpType = pLoadedModel->m_DressUpType;

  // 描画解除
  if (dressUpType != DRESS_UP_TYPE_NONE)
  {
    DressUpModel* pDressUpModel = static_cast<DressUpModel*>(pLoadedModel->m_pCharaModel);
    for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
    {
      m_pRenderingPipeLine->RemoveObject(pDressUpModel->GetPartsModelInstanceNode(static_cast<poke_3d::model::DressUpModel::PartsModel>(i)));
    }
  }
  else
  {
    m_pRenderingPipeLine->RemoveObject(pLoadedModel->m_pCharaModel->GetModelInstanceNode());
  }

  // サブオブジェクト破棄
  if (pLoadedModel->m_pSubObjectModel != NULL)
  {
    // サブオブジェクト描画解除
    m_pRenderingPipeLine->RemoveObject(pLoadedModel->m_pSubObjectModel->GetModelInstanceNode());

    const_cast<HeaderId*>(pFactoryParam->GetCharaIdListData(pLoadedModel->m_SubObjectId))->DecrementRefCounter();
    u32 subRefCounter = pFactoryParam->GetCharaIdListData(pLoadedModel->m_SubObjectId)->refCounter;

    pLoadedModel->m_pSubObjectModel->Destroy();
    GFL_SAFE_DELETE(pLoadedModel->m_pSubObjectModel);

    // サブオブジェクトリソース解放
    if (subRefCounter == 0)
    {
      pCharaModelFactory->UnloadModel(pLoadedModel->m_SubObjectId);
    }

    pLoadedModel->m_SubObjectId = -1;
  }

  // モデル破棄
  pLoadedModel->m_pCharaModel->Destroy();
  GFL_SAFE_DELETE(pLoadedModel->m_pCharaModel);

  if(--refCounter == 0)
  {
    // モデル解放
    pCharaModelFactory->UnloadModel(charaModelId);

    // 動的アニメ解放
    for (u32 i = 0; i < pFactoryParam->GetDyAnimIdList()->GetDataCount(); ++i)
    {
      const HeaderId* pHeaderId = pFactoryParam->GetDyAnimIdList()->GetData(i);
      gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId(pHeaderId->id);

      // 対応するキャラモデルではないなら次へ
      if (packedDynamicAnimationId.charaId != charaModelId)
      {
        continue;
      }

      pCharaModelFactory->UnloadDynamicAnimation(pHeaderId->id);
    }
  }

  // 読み込んだ着せ替えリソースの開放
  // ※ 着せ替えは1体ずつリソース管理するので参照カウントにかかわらず必ず解放する
  if (dressUpType != DRESS_UP_TYPE_NONE)
  {
    DressUpModelResourceManagerCore* pDressUpModelResourceManager = pLoadedModel->m_pDressUpModelResourceManager;
    pDressUpModelResourceManager->UnloadDressUpParts(m_DressUpParam[m_SelectedModel]);

    // 動的アニメ解放
    for (u32 i = 0; i < pFactoryParam->GetDyAnimIdList()->GetDataCount(); ++i)
    {
      const HeaderId* pHeaderId = pFactoryParam->GetDyAnimIdList()->GetData(i);
      gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId(pHeaderId->id);

      // 対応するキャラモデルではないなら次へ
      if (packedDynamicAnimationId.charaId != charaModelId)
      {
        continue;
      }

      // 着せ替え用の動的アニメ解放
      pDressUpModelResourceManager->UnloadDressUpDynamicAnimation(m_DressUpParam[m_SelectedModel], pHeaderId->id);
    }
  }

  const_cast<HeaderId*>(pFactoryParam->GetCharaIdListData(pLoadedModel->m_CharaId))->DecrementRefCounter();

  pLoadedModel->m_DressUpType = DRESS_UP_TYPE_NONE;
  pLoadedModel->m_CharaId = -1;
  pLoadedModel->m_pFactoryParam = NULL;
  pLoadedModel->m_pDressUpModelResourceManager = NULL;
  pLoadedModel->m_MotionIdListCount = 0;
  GFL_SAFE_DELETE_ARRAY(pLoadedModel->m_pMotionIdList);

  // アニメーション管理用のキャラモデル変更
  pLoadedModel->m_Animation.ChangeCharaModel(NULL, NULL, NULL, NULL, 0);
}

void CharaViewerCharaModelManager::LocateOrigin()
{
  // 作成されていないなら無視
  LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return;
  }

  pLoadedModel->m_Position.Set(0, 0, 0);
  pLoadedModel->m_Rotation.Set(0, 0, 0);
}

void CharaViewerCharaModelManager::SlideScaleSelector(bool isForward)
{
  GetLoadedModel()->m_ModelScale += (isForward) ? 1 : -1;
}

u32 CharaViewerCharaModelManager::GetPolygonCount() const
{
  const LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return 0;
  }

  if (pLoadedModel->m_DressUpType == DRESS_UP_TYPE_NONE)
  {
    return pLoadedModel->m_pCharaModel->GetPolygonCount();
  }

  u32 polygonCount = 0;
  const DressUpModel* pDressUpModel = static_cast<const DressUpModel*>(pLoadedModel->m_pCharaModel);
  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    const BaseModel* pPartsModel = pDressUpModel->GetPartsModel(static_cast<DressUpModel::PartsModel>(i));
    if (pPartsModel == NULL)
    {
      continue;
    }

    polygonCount += pPartsModel->GetPolygonCount();
  }

  return polygonCount;
}

u32 CharaViewerCharaModelManager::GetMaterialCount() const
{
  const LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return 0;
  }

  if (pLoadedModel->m_DressUpType == DRESS_UP_TYPE_NONE)
  {
    return pLoadedModel->m_pCharaModel->GetMaterialCount();
  }

  u32 materialCount = 0;
  const DressUpModel* pDressUpModel = static_cast<const DressUpModel*>(pLoadedModel->m_pCharaModel);
  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    const BaseModel* pPartsModel = pDressUpModel->GetPartsModel(static_cast<DressUpModel::PartsModel>(i));
    if (pPartsModel == NULL)
    {
      continue;
    }

    materialCount += pPartsModel->GetMaterialCount();
  }

  return materialCount;
}

u32 CharaViewerCharaModelManager::GetMeshCount() const
{
  const LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return 0;
  }

  if (pLoadedModel->m_DressUpType == DRESS_UP_TYPE_NONE)
  {
    return pLoadedModel->m_pCharaModel->GetMeshCount();
  }

  u32 meshCount = 0;
  const DressUpModel* pDressUpModel = static_cast<const DressUpModel*>(pLoadedModel->m_pCharaModel);
  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    const BaseModel* pPartsModel = pDressUpModel->GetPartsModel(static_cast<DressUpModel::PartsModel>(i));
    if (pPartsModel == NULL)
    {
      continue;
    }

    meshCount += pPartsModel->GetMeshCount();
  }

  return meshCount;
}

u32 CharaViewerCharaModelManager::GetVertexCount() const
{
  const LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return 0;
  }

  if (pLoadedModel->m_DressUpType == DRESS_UP_TYPE_NONE)
  {
    return pLoadedModel->m_pCharaModel->GetVertexCount();
  }

  u32 vertexCount = 0;
  const DressUpModel* pDressUpModel = static_cast<const DressUpModel*>(pLoadedModel->m_pCharaModel);
  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    const BaseModel* pPartsModel = pDressUpModel->GetPartsModel(static_cast<DressUpModel::PartsModel>(i));
    if (pPartsModel == NULL)
    {
      continue;
    }

    vertexCount += pPartsModel->GetVertexCount();
  }

  return vertexCount;
}

u32 CharaViewerCharaModelManager::GetTextureCount() const
{
  const LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return 0;
  }

  if (pLoadedModel->m_DressUpType == DRESS_UP_TYPE_NONE)
  {
    return pLoadedModel->m_pCharaModel->GetTextureCount();
  }

  u32 textureCount = 0;
  const DressUpModel* pDressUpModel = static_cast<const DressUpModel*>(pLoadedModel->m_pCharaModel);
  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    const BaseModel* pPartsModel = pDressUpModel->GetPartsModel(static_cast<DressUpModel::PartsModel>(i));
    if (pPartsModel == NULL)
    {
      continue;
    }

    textureCount += pPartsModel->GetTextureCount();
  }

  return textureCount;
}

u32 CharaViewerCharaModelManager::GetBoneCount() const
{
  const LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return 0;
  }

  if (pLoadedModel->m_DressUpType == DRESS_UP_TYPE_NONE)
  {
    return pLoadedModel->m_pCharaModel->GetJointCount();
  }

  u32 boneCount = 0;
  const DressUpModel* pDressUpModel = static_cast<const DressUpModel*>(pLoadedModel->m_pCharaModel);
  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    const BaseModel* pPartsModel = pDressUpModel->GetPartsModel(static_cast<DressUpModel::PartsModel>(i));
    if (pPartsModel == NULL)
    {
      continue;
    }

    // ボーンはアニメーションさせるモデルだけカウントする
    if (!pPartsModel->HasAnimationController())
    {
      continue;
    }

    // 顔は要らない
    if (i == DressUpModel::PARTS_MODEL_FACE)
    {
      continue;
    }

    boneCount += pPartsModel->GetJointCount();
  }

  return boneCount + pDressUpModel->GetJointCount();
}

u32 CharaViewerCharaModelManager::GetResModelSize() const
{
  const LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return 0;
  }

  return pLoadedModel->m_NormalResModelSize + pLoadedModel->m_DressUpResModelSize;
}

u32 CharaViewerCharaModelManager::GetResAnimSize() const
{
  const LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return 0;
  }

  return pLoadedModel->m_NormalResAnimSize + pLoadedModel->m_DressUpResAnimSize;
}

u32 CharaViewerCharaModelManager::GetResTotalSize() const
{
  const LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return 0;
  }

  return pLoadedModel->m_NormalResTotalSize + pLoadedModel->m_DressUpResTotalSize;
}

u32 CharaViewerCharaModelManager::GetInstanceSize() const
{
  const LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return 0;
  }

  return pLoadedModel->m_InstanceSize;
}


// 2ページ目
bool CharaViewerCharaModelManager::IsClothColorHat() const
{
  return (m_ClothColorSelectedItemCategory == DressUpParam::ITEM_CATEGORY_HAT);
}

bool CharaViewerCharaModelManager::IsClothColorAccHair() const
{
  return (m_ClothColorSelectedItemCategory == DressUpParam::ITEM_CATEGORY_ACCHAIR);
}

bool CharaViewerCharaModelManager::IsClothColorAccEye() const
{
  return (m_ClothColorSelectedItemCategory == DressUpParam::ITEM_CATEGORY_ACCEYE);
}

bool CharaViewerCharaModelManager::IsClothColorTops() const
{
  return (m_ClothColorSelectedItemCategory == DressUpParam::ITEM_CATEGORY_TOPS);
}

bool CharaViewerCharaModelManager::IsClothColorOnepiece() const
{
  return (m_ClothColorSelectedItemCategory == DressUpParam::ITEM_CATEGORY_BODY);
}

bool CharaViewerCharaModelManager::IsClothColorBottoms() const
{
  return (m_ClothColorSelectedItemCategory == DressUpParam::ITEM_CATEGORY_BOTTOMS);
}

bool CharaViewerCharaModelManager::IsClothColorLegs() const
{
  return (m_ClothColorSelectedItemCategory == DressUpParam::ITEM_CATEGORY_LEGS);
}

bool CharaViewerCharaModelManager::IsClothColorShoes() const
{
  return (m_ClothColorSelectedItemCategory == DressUpParam::ITEM_CATEGORY_SHOES);
}

bool CharaViewerCharaModelManager::IsClothColorBag() const
{
  return (m_ClothColorSelectedItemCategory == DressUpParam::ITEM_CATEGORY_BAG);
}

bool CharaViewerCharaModelManager::IsClothColorBangle() const
{
  return (m_ClothColorSelectedItemCategory == DressUpParam::ITEM_CATEGORY_BNGL);
}

void CharaViewerCharaModelManager::SetClothColorHat()
{
  m_ClothColorSelectedItemCategory = DressUpParam::ITEM_CATEGORY_HAT;
}

void CharaViewerCharaModelManager::SetClothColorAccHair()
{
  m_ClothColorSelectedItemCategory = DressUpParam::ITEM_CATEGORY_ACCHAIR;
}

void CharaViewerCharaModelManager::SetClothColorAccEye()
{
  m_ClothColorSelectedItemCategory = DressUpParam::ITEM_CATEGORY_ACCEYE;
}

void CharaViewerCharaModelManager::SetClothColorTops()
{
  m_ClothColorSelectedItemCategory = DressUpParam::ITEM_CATEGORY_TOPS;
}

void CharaViewerCharaModelManager::SetClothColorOnepiece()
{
  m_ClothColorSelectedItemCategory = DressUpParam::ITEM_CATEGORY_BODY;
}

void CharaViewerCharaModelManager::SetClothColorBottoms()
{
  m_ClothColorSelectedItemCategory = DressUpParam::ITEM_CATEGORY_BOTTOMS;
}

void CharaViewerCharaModelManager::SetClothColorLegs()
{
  m_ClothColorSelectedItemCategory = DressUpParam::ITEM_CATEGORY_LEGS;
}

void CharaViewerCharaModelManager::SetClothColorShoes()
{
  m_ClothColorSelectedItemCategory = DressUpParam::ITEM_CATEGORY_SHOES;
}

void CharaViewerCharaModelManager::SetClothColorBag()
{
  m_ClothColorSelectedItemCategory = DressUpParam::ITEM_CATEGORY_BAG;
}

void CharaViewerCharaModelManager::SetClothColorBangle()
{
  m_ClothColorSelectedItemCategory = DressUpParam::ITEM_CATEGORY_BNGL;
}

void CharaViewerCharaModelManager::SetClothColorSituation()
{
  m_ClothColorSelectedItemCategory = DressUpParam::ITEM_CATEGORY_SITU;
}

u32 CharaViewerCharaModelManager::GetSkinColor(u32 index) const
{
  return Get255Color(GetDefaultDressUpResourceManager()->GetDressUpColorSkin(index));
}

u32 CharaViewerCharaModelManager::GetHairColor(u32 index) const
{
  return Get255Color(GetDefaultDressUpResourceManager()->GetDressUpColorHair(index));
}

u32 CharaViewerCharaModelManager::GetClothColor(u32 index) const
{
  u8 alpha = 0;

  // 着せ替えモデルが読み込まれて居たら対応する色だけ表示する
  const LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel != NULL)
  {
    DressUpModelResourceManagerCore* pDressUpModelResourceManager = pLoadedModel->m_pDressUpModelResourceManager;

    s32 itemIndex = m_DressUpParam[m_SelectedModel].itemParams[m_ClothColorSelectedItemCategory];
    if (itemIndex >= 0)
    {
      s32 partsIndex = pDressUpModelResourceManager->GetDressUpPartsIndex(m_ClothColorSelectedItemCategory, itemIndex);
      if (pDressUpModelResourceManager->IsDressUpPartsColorEnabled(m_ClothColorSelectedItemCategory, partsIndex, index))
      {
        // 対応する色
        alpha = 255;
      }
    }
  }

  return Get255Color(GetDefaultDressUpResourceManager()->GetDressUpColorCloth(index), alpha);
}

void CharaViewerCharaModelManager::ChangeSkinColor(u32 index)
{
  m_DressUpParam[m_SelectedModel].bodyParams[DressUpParam::BODY_CATEGORY_BODY_COLOR] = index;
  this->ReloadCharactor();
}

void CharaViewerCharaModelManager::ChangeHairColor(u32 index)
{
  m_DressUpParam[m_SelectedModel].bodyParams[DressUpParam::BODY_CATEGORY_HAIR_COLOR] = index;
  this->ReloadCharactor();
}

void CharaViewerCharaModelManager::ChangeClothColor(u32 index)
{
  s32 itemIndex = m_DressUpParam[m_SelectedModel].itemParams[m_ClothColorSelectedItemCategory];
  if (itemIndex < 0)
  {
    return;
  }
  LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return;
  }

  DressUpModelResourceManagerCore* pDressUpModelResourceManager = pLoadedModel->m_pDressUpModelResourceManager;
  u32 partsIndex = pDressUpModelResourceManager->GetDressUpPartsIndex(m_ClothColorSelectedItemCategory, itemIndex);
  
  if (!pDressUpModelResourceManager->IsDressUpPartsColorEnabled(m_ClothColorSelectedItemCategory, partsIndex, index))
  {
    // 対応しない色
    return;
  }
  m_DressUpParam[m_SelectedModel].itemParams[m_ClothColorSelectedItemCategory] = pDressUpModelResourceManager->GetDressUpItemIndex(m_ClothColorSelectedItemCategory, partsIndex, index);

  this->ReloadCharactor();
}

s32 CharaViewerCharaModelManager::GetSkinColorIndex() const
{
  return m_DressUpParam[m_SelectedModel].bodyParams[DressUpParam::BODY_CATEGORY_BODY_COLOR];
}

s32 CharaViewerCharaModelManager::GetHairColorIndex() const
{
  return m_DressUpParam[m_SelectedModel].bodyParams[DressUpParam::BODY_CATEGORY_HAIR_COLOR];
}

s32 CharaViewerCharaModelManager::GetClothColorIndex() const
{
  s32 itemIndex = m_DressUpParam[m_SelectedModel].itemParams[m_ClothColorSelectedItemCategory];
  if (itemIndex < 0)
  {
    return -1;
  }
  const LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return -1;
  }

  DressUpModelResourceManagerCore* pDressUpModelResourceManager = pLoadedModel->m_pDressUpModelResourceManager;
  return pDressUpModelResourceManager->GetDressUpColorIndex(m_ClothColorSelectedItemCategory, itemIndex);
}

void CharaViewerCharaModelManager::GetCurrentPartsNameFace(const char** out_ppPartsName, s32* out_pPartsIndex) const
{
  this->GetCurrentPartsName(DressUpParam::ITEM_CATEGORY_FACE, out_ppPartsName, out_pPartsIndex);
}

void CharaViewerCharaModelManager::GetCurrentPartsNameHairFront(const char** out_ppPartsName, s32* out_pPartsIndex) const
{
  *out_ppPartsName = PARTS_NONE_NAME;
  *out_pPartsIndex = -1;
}

void CharaViewerCharaModelManager::GetCurrentPartsNameHairStyle(const char** out_ppPartsName, s32* out_pPartsIndex) const
{
  this->GetCurrentPartsName(DressUpParam::ITEM_CATEGORY_HAIR_STYLE, out_ppPartsName, out_pPartsIndex);
}

void CharaViewerCharaModelManager::GetCurrentPartsNameHat(const char** out_ppPartsName, s32* out_pPartsIndex) const
{
  this->GetCurrentPartsName(DressUpParam::ITEM_CATEGORY_HAT, out_ppPartsName, out_pPartsIndex);
}

void CharaViewerCharaModelManager::GetCurrentPartsNameAccHair(const char** out_ppPartsName, s32* out_pPartsIndex) const
{
  this->GetCurrentPartsName(DressUpParam::ITEM_CATEGORY_ACCHAIR, out_ppPartsName, out_pPartsIndex);
}

void CharaViewerCharaModelManager::GetCurrentPartsNameAccEye(const char** out_ppPartsName, s32* out_pPartsIndex) const
{
  this->GetCurrentPartsName(DressUpParam::ITEM_CATEGORY_ACCEYE, out_ppPartsName, out_pPartsIndex);
}

void CharaViewerCharaModelManager::GetCurrentPartsNameTops(const char** out_ppPartsName, s32* out_pPartsIndex) const
{
  this->GetCurrentPartsName(DressUpParam::ITEM_CATEGORY_TOPS, out_ppPartsName, out_pPartsIndex);
}

void CharaViewerCharaModelManager::GetCurrentPartsNameOnePiece(const char** out_ppPartsName, s32* out_pPartsIndex) const
{
  *out_ppPartsName = PARTS_NONE_NAME;
  *out_pPartsIndex = -1;
}

void CharaViewerCharaModelManager::GetCurrentPartsNameBottoms(const char** out_ppPartsName, s32* out_pPartsIndex) const
{
  this->GetCurrentPartsName(DressUpParam::ITEM_CATEGORY_BOTTOMS, out_ppPartsName, out_pPartsIndex);
}

void CharaViewerCharaModelManager::GetCurrentPartsNameSocks(const char** out_ppPartsName, s32* out_pPartsIndex) const
{
  this->GetCurrentPartsName(DressUpParam::ITEM_CATEGORY_LEGS, out_ppPartsName, out_pPartsIndex);
}

void CharaViewerCharaModelManager::GetCurrentPartsNameShoes(const char** out_ppPartsName, s32* out_pPartsIndex) const
{
  this->GetCurrentPartsName(DressUpParam::ITEM_CATEGORY_SHOES, out_ppPartsName, out_pPartsIndex);
}

void CharaViewerCharaModelManager::GetCurrentPartsNameBag(const char** out_ppPartsName, s32* out_pPartsIndex) const
{
  this->GetCurrentPartsName(DressUpParam::ITEM_CATEGORY_BAG, out_ppPartsName, out_pPartsIndex);
}

void CharaViewerCharaModelManager::SlideSkinSelector(bool isForward)
{
  this->SelectNextParts(DressUpParam::ITEM_CATEGORY_FACE, isForward);
}

void CharaViewerCharaModelManager::SlideFrontHairSelector(bool isForward)
{
  // あとで
}

void CharaViewerCharaModelManager::SlideHairStyleSelector(bool isForward)
{
  this->SelectNextParts(DressUpParam::ITEM_CATEGORY_HAIR_STYLE, isForward);
}

void CharaViewerCharaModelManager::SlideHeadWearSelector(bool isForward)
{
  this->SelectNextParts(DressUpParam::ITEM_CATEGORY_HAT, isForward);
  this->SetClothColorHat();
}

void CharaViewerCharaModelManager::SlideHairAccessorySelector(bool isForward)
{
  this->SelectNextParts(DressUpParam::ITEM_CATEGORY_ACCHAIR, isForward);
  this->SetClothColorAccHair();
}

void CharaViewerCharaModelManager::SlideEyeWearSelector(bool isForward)
{
  this->SelectNextParts(DressUpParam::ITEM_CATEGORY_ACCEYE, isForward);
  this->SetClothColorAccEye();
}

void CharaViewerCharaModelManager::SlideTopsSelector(bool isForward)
{
  this->SelectNextParts(DressUpParam::ITEM_CATEGORY_TOPS, isForward);
  this->SetClothColorTops();
}

void CharaViewerCharaModelManager::SlideOnepieceSelector(bool isForward)
{
  // あとで
}

void CharaViewerCharaModelManager::SlideBottomsSelector(bool isForward)
{
  this->SelectNextParts(DressUpParam::ITEM_CATEGORY_BOTTOMS, isForward);
  this->SetClothColorBottoms();
}

void CharaViewerCharaModelManager::SlideSocksSelector(bool isForward)
{
  this->SelectNextParts(DressUpParam::ITEM_CATEGORY_LEGS, isForward);
  this->SetClothColorLegs();
}

void CharaViewerCharaModelManager::SlideShoesSelector(bool isForward)
{
  this->SelectNextParts(DressUpParam::ITEM_CATEGORY_SHOES, isForward);
  this->SetClothColorShoes();
}

void CharaViewerCharaModelManager::SlideBagSelector(bool isForward)
{
  this->SelectNextParts(DressUpParam::ITEM_CATEGORY_BAG, isForward);
  this->SetClothColorBag();
}


// 3ページ目
u32 CharaViewerCharaModelManager::GetEyeblowColor(u32 index) const
{
  return Get255Color(GetDefaultDressUpResourceManager()->GetDressUpColorEyeblow(index));
}

u32 CharaViewerCharaModelManager::GetEyeColor(u32 index) const
{
  return Get255Color(GetDefaultDressUpResourceManager()->GetDressUpColorEye(index));
}

u32 CharaViewerCharaModelManager::GetLipColor(u32 index) const
{
  return Get255Color(GetDefaultDressUpResourceManager()->GetDressUpColorLip(index));
}

void CharaViewerCharaModelManager::ChangeEyeblowColor(u32 index)
{
  m_DressUpParam[m_SelectedModel].bodyParams[DressUpParam::BODY_CATEGORY_HAIR_COLOR] = index;
  this->ReloadCharactor();
}

void CharaViewerCharaModelManager::ChangeEyeColor(u32 index)
{
  m_DressUpParam[m_SelectedModel].bodyParams[DressUpParam::BODY_CATEGORY_EYE_COLOR] = index;
  this->ReloadCharactor();
}

void CharaViewerCharaModelManager::ChangeLipColor(u32 index)
{
  m_DressUpParam[m_SelectedModel].bodyParams[DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = index;
  this->ReloadCharactor();
}

u32 CharaViewerCharaModelManager::GetEyeblowColorIndex() const
{
  return m_DressUpParam[m_SelectedModel].bodyParams[DressUpParam::BODY_CATEGORY_HAIR_COLOR];
}

u32 CharaViewerCharaModelManager::GetEyeColorIndex() const
{
  return m_DressUpParam[m_SelectedModel].bodyParams[DressUpParam::BODY_CATEGORY_EYE_COLOR];
}

u32 CharaViewerCharaModelManager::GetLipColorIndex() const
{
  return m_DressUpParam[m_SelectedModel].bodyParams[DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR];
}

void CharaViewerCharaModelManager::GetCurrentPartsNameBngl(const char** out_ppPartsName, s32* out_pPartsIndex) const
{
  this->GetCurrentPartsName(DressUpParam::ITEM_CATEGORY_BNGL, out_ppPartsName, out_pPartsIndex);
}

void CharaViewerCharaModelManager::GetCurrentPartsNameCharm(const char** out_ppPartsName, s32* out_pPartsIndex) const
{
  *out_ppPartsName = PARTS_NONE_NAME;
  *out_pPartsIndex = -1;
}

void CharaViewerCharaModelManager::GetCurrentPartsNameLip(const char** out_ppPartsName, s32* out_pPartsIndex) const
{
  *out_ppPartsName = PARTS_NONE_NAME;
  *out_pPartsIndex = -1;
}

void CharaViewerCharaModelManager::GetCurrentPartsNameSituation(const char** out_ppPartsName, s32* out_pPartsIndex) const
{
  this->GetCurrentPartsName(DressUpParam::ITEM_CATEGORY_SITU, out_ppPartsName, out_pPartsIndex);
}

void CharaViewerCharaModelManager::SlideBangleSelector(bool isForward)
{
  this->SelectNextParts(DressUpParam::ITEM_CATEGORY_BNGL, isForward);
  this->SetClothColorBangle();
}

void CharaViewerCharaModelManager::SlideCharmSelector(bool isForward)
{
}

void CharaViewerCharaModelManager::SlideLipSelector(bool isForward)
{
}

void CharaViewerCharaModelManager::SlideSituationSelector(bool isForward)
{
  this->SelectNextParts(DressUpParam::ITEM_CATEGORY_SITU, isForward);
  this->SetClothColorSituation();
}

void CharaViewerCharaModelManager::SlideEyeSelector(bool isForward)
{
  const LoadedModel* pLoadedModel = this->GetLoadedModel();

  // 何も読み込まれていない
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return;
  }

  s32 eyeIndex = pLoadedModel->m_pCharaModel->GetEyeIndex();
  eyeIndex += (isForward) ? 1 : -1;
  if (eyeIndex < 0)
  {
    eyeIndex = poke_3d::model::CharaModel::EYE_INDEX_UNIQUE6;
  }
  else if (eyeIndex > poke_3d::model::CharaModel::EYE_INDEX_UNIQUE6)
  {
    eyeIndex = 0;
  }

  // 自動目パチ無効
  pLoadedModel->m_pCharaModel->SetAutoBlinkMode(poke_3d::model::CharaModel::AUTO_BLINK_MODE_NONE);

  pLoadedModel->m_pCharaModel->SetEyeIndex(static_cast<poke_3d::model::CharaModel::EyeIndex>(eyeIndex));
}

void CharaViewerCharaModelManager::SlideMouthSelector(bool isForward)
{
  const LoadedModel* pLoadedModel = this->GetLoadedModel();

  // 何も読み込まれていない
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return;
  }

  s32 mouthIndex = pLoadedModel->m_pCharaModel->GetMouthIndex();
  mouthIndex += (isForward) ? 1 : -1;
  if (mouthIndex < 0)
  {
    mouthIndex = poke_3d::model::CharaModel::MOUTH_INDEX_UNIQUE6;
  }
  else if (mouthIndex > poke_3d::model::CharaModel::MOUTH_INDEX_UNIQUE6)
  {
    mouthIndex = 0;
  }
  pLoadedModel->m_pCharaModel->SetMouthIndex(static_cast<poke_3d::model::CharaModel::MouthIndex>(mouthIndex));
}

const char* CharaViewerCharaModelManager::GetCurrentEyeName() const
{
  static const char* sc_EyeNameList[] =
  {
    "--NONE--",                           ///< 指定無し、再生中のアニメーションに連動

    "OPEN",                               ///< 全開き
    "HALF_OPEN",                          ///< 半開き
    "CLOSE",                              ///< 閉じる

    "0",                                  ///< ユニーク0
    "1",                                  ///< ユニーク1
    "2",                                  ///< ユニーク2
    "3",                                  ///< ユニーク3
    "4",                                  ///< ユニーク4
    "5",                                  ///< ユニーク5
    "6",                                  ///< ユニーク6
  };

  const LoadedModel* pLoadedModel = this->GetLoadedModel();

  // 何も読み込まれていない
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return sc_EyeNameList[0];
  }

  return sc_EyeNameList[pLoadedModel->m_pCharaModel->GetEyeIndex()];
}

const char* CharaViewerCharaModelManager::GetCurrentMouthName() const
{
  static const char* sc_MouthNameList[] =
  {
    "--NONE--",                           ///< 指定無し、再生中のアニメーションに連動

    "CLOSE",                              ///< 閉じる
    "HALF_OPEN",                          ///< 半開き
    "OPEN",                               ///< 開く

    "0",                                  ///< ユニーク0
    "1",                                  ///< ユニーク1
    "2",                                  ///< ユニーク2
    "3",                                  ///< ユニーク3
    "4",                                  ///< ユニーク4
    "5",                                  ///< ユニーク5
    "6",                                  ///< ユニーク6
  };

  const LoadedModel* pLoadedModel = this->GetLoadedModel();

  // 何も読み込まれていない
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return sc_MouthNameList[0];
  }

  return sc_MouthNameList[pLoadedModel->m_pCharaModel->GetMouthIndex()];
}

void CharaViewerCharaModelManager::ToggleAutoBlink()
{
  LoadedModel* pLoadedModel = this->GetLoadedModel();

  // 何も読み込まれていない
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return;
  }

  if (this->IsAutoBlinkOn())
  {
    // 自動目パチ無効にする
    pLoadedModel->m_pCharaModel->SetAutoBlinkMode(poke_3d::model::CharaModel::AUTO_BLINK_MODE_NONE);
    return;
  }

  // 自動目パチ有効にする

  // 目インデックス無効
  pLoadedModel->m_pCharaModel->SetEyeIndex(poke_3d::model::CharaModel::EYE_INDEX_NONE);

  if (pLoadedModel->m_pFactoryParam->GetModelType() == MODEL_TYPE_BATTLE)
  {
    // バトル用目パチ
    pLoadedModel->m_pCharaModel->SetAutoBlinkMode(poke_3d::model::CharaModel::AUTO_BLINK_MODE_BATTLE);
  }
  else
  {
    // フィールド用目パチ
    pLoadedModel->m_pCharaModel->SetAutoBlinkMode(poke_3d::model::CharaModel::AUTO_BLINK_MODE_FIELD);
  }
}

bool CharaViewerCharaModelManager::IsAutoBlinkOn() const
{
  const LoadedModel* pLoadedModel = this->GetLoadedModel();

  // 何も読み込まれていない
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return false;
  }

  return (pLoadedModel->m_pCharaModel->GetAutoBlinkMode() != poke_3d::model::CharaModel::AUTO_BLINK_MODE_NONE);
}

void CharaViewerCharaModelManager::ToggleBeadsOnOff()
{
  // ビーズ表示をトグルする
  this->SetBeadsOnOff(!this->IsBeadsOn());
}

void CharaViewerCharaModelManager::SetBeadsOnOff(bool on)
{
  LoadedModel* pLoadedModel = this->GetLoadedModel();

  // 読み込みされてなければ終了
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return;
  }

  // 着せ替えモデルでなければ終了
  if (pLoadedModel->m_DressUpType == DRESS_UP_TYPE_NONE)
  {
    return;
  }

  DressUpModel* pDressUpModel = static_cast<DressUpModel*>(pLoadedModel->m_pCharaModel);
  pDressUpModel->SetZenBeadsVisible(on);
}

bool CharaViewerCharaModelManager::IsBeadsOn() const
{
  const LoadedModel* pLoadedModel = this->GetLoadedModel();

  // 読み込みされてなければ終了
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return false;
  }

  // 着せ替えモデルでなければ終了
  if (pLoadedModel->m_DressUpType == DRESS_UP_TYPE_NONE)
  {
    return false;
  }

  const DressUpModel* pDressUpModel = static_cast<const DressUpModel*>(pLoadedModel->m_pCharaModel);
  return GFL_BOOL_CAST(pDressUpModel->IsZenBeadsVisible());
}

void CharaViewerCharaModelManager::ToggleCharmOnOff()
{
  // 証表示をトグルする
  this->SetCharmOnOff(!this->IsCharmOn());
}

void CharaViewerCharaModelManager::SetCharmOnOff(bool on)
{
  LoadedModel* pLoadedModel = this->GetLoadedModel();

  // 読み込みされてなければ終了
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return;
  }

  // 着せ替えモデルでなければ終了
  if (pLoadedModel->m_DressUpType == DRESS_UP_TYPE_NONE)
  {
    return;
  }

  DressUpModel* pDressUpModel = static_cast<DressUpModel*>(pLoadedModel->m_pCharaModel);
  pDressUpModel->SetBagCharmVisible(on);
}

bool CharaViewerCharaModelManager::IsCharmOn() const
{
  const LoadedModel* pLoadedModel = this->GetLoadedModel();

  // 読み込みされてなければ終了
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return false;
  }

  // 着せ替えモデルでなければ終了
  if (pLoadedModel->m_DressUpType == DRESS_UP_TYPE_NONE)
  {
    return false;
  }

  const DressUpModel* pDressUpModel = static_cast<const DressUpModel*>(pLoadedModel->m_pCharaModel);
  return GFL_BOOL_CAST(pDressUpModel->IsBagCharmVisible());
}

void CharaViewerCharaModelManager::LoadCollisionCheckParts()
{
  LoadedModel* pLoadedModel = GetLoadedModel();

  // 何も読まれていなければ何もしない
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return;
  }

  // 着せ替えモデル以外は無視
  if (pLoadedModel->m_DressUpType == CharaViewerCharaModelManager::DRESS_UP_TYPE_NONE)
  {
    return;
  }

  // チェック用パーツに変更
  if (pLoadedModel->m_DressUpType == DRESS_UP_TYPE_HERO)
  {
    System::DressUp::GetDressUpParamDefaultHero(&m_DressUpParam[m_SelectedModel]);

    for (u32 i = 0; i < DressUpParam::ITEM_CATEGORY_COUNT; ++i)
    {
      m_DressUpParam[m_SelectedModel].itemParams[i] = m_pCheckPartsList[i];
    }
  }
  else
  {
    System::DressUp::GetDressUpParamDefaultHeroine(&m_DressUpParam[m_SelectedModel]);

    for (u32 i = 0; i < DressUpParam::ITEM_CATEGORY_COUNT; ++i)
    {
      m_DressUpParam[m_SelectedModel].itemParams[i] = m_pCheckPartsList[i + DressUpParam::ITEM_CATEGORY_COUNT];
    }
  }

  // 再読み込み
  this->ReloadCharactor();
}

void CharaViewerCharaModelManager::LoadDataSizeCheckParts()
{
  LoadedModel* pLoadedModel = GetLoadedModel();

  // 何も読まれていなければ何もしない
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return;
  }

  // 着せ替えモデル以外は無視
  if (pLoadedModel->m_DressUpType == CharaViewerCharaModelManager::DRESS_UP_TYPE_NONE)
  {
    return;
  }

  // チェック用パーツに変更
  const DressUpModelResourceManagerCore* pDressUpModelResourceManager = pLoadedModel->m_pFactoryParam->GetDressUpModelResourceManager(pLoadedModel->m_DressUpType, m_SelectedModel);
  pDressUpModelResourceManager->GetMaxDataSizeDressUpParam(&m_DressUpParam[m_SelectedModel]);

  // 再読み込み
  this->ReloadCharactor();
}

// 4ページ目
bool CharaViewerCharaModelManager::IsFpsMode30() const
{
  return GetLoadedModel()->m_Animation.IsFpsMode30();
}
void CharaViewerCharaModelManager::GetCurrentMotionName(u32 index, const char** out_ppMotionName, s32* out_pListIndex, s32* out_pMotionIndex) const
{
  GetLoadedModel()->m_Animation.GetCurrentMotionName(index, out_ppMotionName, out_pListIndex, out_pMotionIndex);
}
u32 CharaViewerCharaModelManager::GetCurrentMotionTotalFrame(u32 index) const
{
  return GetLoadedModel()->m_Animation.GetCurrentMotionTotalFrame(index);
}
u32 CharaViewerCharaModelManager::GetCurrentMotionFrame(u32 index) const
{
  return GetLoadedModel()->m_Animation.GetCurrentMotionFrame(index);
}
bool CharaViewerCharaModelManager::IsMotionEnabled(u32 index) const
{
  return GetLoadedModel()->m_Animation.IsMotionEnabled(index);
}
bool CharaViewerCharaModelManager::IsMotionLoop() const
{
  return GetLoadedModel()->m_Animation.IsMotionLoop();
}
void CharaViewerCharaModelManager::ToggleLoopFlag()
{
  if(m_AllPlayFlag)
  {
    GetLoadedModel()->m_Animation.ToggleLoopFlag();
    bool loopFlag = GetLoadedModel()->m_Animation.IsMotionLoop();

    for(u32 i = 0; i < MODEL_TAB_LIMIT; ++i)
    {
      if(m_LoadedModel[i].m_pCharaModel != NULL)
      {
        m_LoadedModel[i].m_Animation.SetLoopFlag(loopFlag);
      }
    }
  }
  else
  {
    GetLoadedModel()->m_Animation.ToggleLoopFlag();
  }
}
void CharaViewerCharaModelManager::PlaySingleFrame(bool isForward)
{
  if(m_AllPlayFlag)
  {
    for(u32 i = 0; i < MODEL_TAB_LIMIT; ++i)
    {
      if(m_LoadedModel[i].m_pCharaModel != NULL)
      {
        m_LoadedModel[i].m_Animation.PlaySingleFrame(isForward);
      }
    }
  }
  else
  {
    GetLoadedModel()->m_Animation.PlaySingleFrame(isForward);
  }
}
bool CharaViewerCharaModelManager::IsMotionPlaying() const
{
  return GetLoadedModel()->m_Animation.IsMotionPlaying();
}
void CharaViewerCharaModelManager::PlayMotion()
{
  if(m_AllPlayFlag)
  {
    for(u32 i = 0; i < MODEL_TAB_LIMIT; ++i)
    {
      if(m_LoadedModel[i].m_pCharaModel != NULL)
      {
        m_LoadedModel[i].m_pCharaModel->SetAnimationFrame(0);
        m_LoadedModel[i].m_Animation.PlayMotion();
      }
    }
  }
  else
  {
    GetLoadedModel()->m_Animation.PlayMotion();
  }
}
void CharaViewerCharaModelManager::SlideMotionSelector(u8 index, bool isForward)
{
  s32 slideValue = this->GetSlideValue(isForward);

  if(m_AllPlayFlag)
  {
    for(u32 i = 0; i < MODEL_TAB_LIMIT; ++i)
    {
      if(m_LoadedModel[i].m_pCharaModel != NULL)
      {
        m_LoadedModel[i].m_Animation.SlideMotionSelector(index, slideValue);
      }
    }
  }
  else
  {
    GetLoadedModel()->m_Animation.SlideMotionSelector(index, slideValue);
  }
}
void CharaViewerCharaModelManager::ToggleMotion(u8 index)
{
  GetLoadedModel()->m_Animation.ToggleMotion(index);
}
void CharaViewerCharaModelManager::PlayTPose()
{
  GetLoadedModel()->m_Animation.PlayTPose();
}
void CharaViewerCharaModelManager::PlayStackCheckMotion()
{
  LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return;
  }

  FactoryParam* pFactoryParam = pLoadedModel->m_pFactoryParam;

  if (pLoadedModel->m_DressUpType == DRESS_UP_TYPE_NONE)
  {
    return;
  }

  CharaViewerCharaModelManager_Animation::CheckMotionType type;
  if (pLoadedModel->m_DressUpType == DRESS_UP_TYPE_HERO)
  {
    if (pFactoryParam->GetModelType() == MODEL_TYPE_BATTLE)
    {
      type = CharaViewerCharaModelManager_Animation::CHECK_MOTION_TYPE_BATTLE_MALE;
    }
    else
    {
      type = CharaViewerCharaModelManager_Animation::CHECK_MOTION_TYPE_FIELD_MALE;
    }
  }
  else
  {
    if (pFactoryParam->GetModelType() == MODEL_TYPE_BATTLE)
    {
      type = CharaViewerCharaModelManager_Animation::CHECK_MOTION_TYPE_BATTLE_FEMALE;
    }
    else
    {
      type = CharaViewerCharaModelManager_Animation::CHECK_MOTION_TYPE_FIELD_FEMALE;
    }
  }
  pLoadedModel->m_Animation.PlayStackCheckMotion(type);
}
void CharaViewerCharaModelManager::ToggleAllPlayFlag()
{
  m_AllPlayFlag = !m_AllPlayFlag;

  if (m_AllPlayFlag)
  {
    bool loopFlag = GetLoadedModel()->m_Animation.IsMotionLoop();

    for(u32 i = 0; i < MODEL_TAB_LIMIT; ++i)
    {
      if(m_LoadedModel[i].m_pCharaModel != NULL)
      {
        m_LoadedModel[i].m_Animation.SetLoopFlag(loopFlag);
      }
    }
  }
}
bool CharaViewerCharaModelManager::GetTranslateFlag() const
{
  return GetLoadedModel()->m_Animation.GetTranslateFlag();
}
void CharaViewerCharaModelManager::ToggleTranslateFlag()
{
  GetLoadedModel()->m_Animation.ToggleTranslateFlag();
}
void CharaViewerCharaModelManager::SlideFpsSelector(bool isForward)
{
  GetLoadedModel()->m_Animation.SlideFpsSelector(isForward);
}

// 5ページ目
bool CharaViewerCharaModelManager::IsControllModeCamera() const
{
  return m_IsControllModeCamera;
}

void CharaViewerCharaModelManager::ToggleCameraControllMode()
{
  m_IsControllModeCamera = !m_IsControllModeCamera;
}

bool CharaViewerCharaModelManager::IsGridVisible() const
{
  return m_pGridModel->IsVisible();
}

void CharaViewerCharaModelManager::ToggleGrid()
{
  bool isVisible = m_pGridModel->IsVisible();
  m_pGridModel->SetVisible(!isVisible);
}

void CharaViewerCharaModelManager::SlideCameraAngleSelector(bool isForward)
{
  m_CameraFovy += (isForward) ? 1 : -1;
  if (m_CameraFovy < 5)
  {
    m_CameraFovy = 5;
  }
  if (m_CameraFovy > 90)
  {
    m_CameraFovy = 90;
  }
}

u32 CharaViewerCharaModelManager::GetCameraFovy() const
{
  return m_CameraFovy;
}

void CharaViewerCharaModelManager::SetCameraFovyType(CharaViewerCharaModelManager::FovyType type)
{
  m_CameraFovyType = type;

  if (type == FOVY_TYPE_BATTLE)
  {
    m_CameraFovy = 30;
  }
  else
  {
    m_CameraFovy = 35;
  }
}

CharaViewerCharaModelManager::FovyType CharaViewerCharaModelManager::GetCameraFovyType() const
{
  return m_CameraFovyType;
}

void CharaViewerCharaModelManager::SetCameraNearFarType(CharaViewerCharaModelManager::NearFarType type)
{
  m_CameraNearFarType = type;
  if (type == NEAR_FAR_TYPE_BATTLE)
  {
    m_CameraNear = 32;
    m_CameraFar = 6000;
  }
  else
  {
    m_CameraNear = 30;
    m_CameraFar = 30000;
  }
}

CharaViewerCharaModelManager::NearFarType CharaViewerCharaModelManager::GetCameraNearFarType() const
{
  return m_CameraNearFarType;
}

void CharaViewerCharaModelManager::SetClearColorType(u32 clearColorType)
{
  m_ClearColorType = clearColorType;
  for (u32 i = 0; i < gfl2::gfx::CtrDisplayNo::DOWN; ++i)
  {
    m_pGameManager->SetBufferClearSetting((gfl2::gfx::CtrDisplayNo)i, sc_BufferClearDefaultSettingTable[m_ClearColorType]);
  }
}

u32 CharaViewerCharaModelManager::GetClearColorType() const
{
  return m_ClearColorType;
}

void CharaViewerCharaModelManager::Capture()
{
#if GFL_FS_HIO_ENABLE

  LoadedModel* pLoadedModel = this->GetLoadedModel();

  const char* pCharaName = (pLoadedModel->m_pCharaModel == NULL)
    ? "name_none"
    : pLoadedModel->m_pFactoryParam->GetCharaIdListData(pLoadedModel->m_CharaId)->GetCharaName();

  s32 charaId = (pLoadedModel->m_pCharaModel == NULL)
    ? 9999
    : pLoadedModel->m_CharaId;

  char buf[256];
  CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%04d_%s.bmp", charaId, pCharaName);
  GFL_PRINT("capture : %s\n", buf);
  System::Screenshot::PrintScreen(buf, gfl2::gfx::DisplayMode::CTR_LEFT);

#else

  // HIOが無効なら計測の方を実行
  this->MeasureNormalAll();

#endif
}

void CharaViewerCharaModelManager::CaptureAll()
{
#if GFL_FS_HIO_ENABLE

  m_UpdateType = UPDATE_TYPE_CAPTURE_ALL;
  m_CharaId = 0;
  m_UpdateSeq = 0;

  LoadedModel* pLoadedModel = this->GetLoadedModel();
  m_CaptureAnimationId = (pLoadedModel->m_pCharaModel == NULL)
    ? 0
    : m_CaptureAnimationId = pLoadedModel->m_Animation.GetCurrentAnimationId();

  GFL_PRINT("captureAll begin\n");

#else

  // HIOが無効なら計測の方を実行
  this->MeasureDressupAll();

#endif
}

void CharaViewerCharaModelManager::MeasureNormal()
{
  const char* pName = NULL;
  s32 charaId = -1;
  this->GetCurrentCharaName(&pName, &charaId);

  s32 updateTick = GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPDATE) - m_BaseUpdateTick;
  s32 upnextTick = GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPNEXT) - m_BaseUpnextTick;
  s32 drawTick = GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_DRAW) - m_BaseDrawTick;
  s32 gpuTick = GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_GPU) - m_BaseGpuTick;

  u32 animHeapSize = m_DressUpAnimHeapBeginSize - m_pDressUpAnimHeap->GetTotalFreeSize();
  u32 modelHeapSize = m_DressUpModelHeapBeginSize - m_pDressUpModelHeap->GetTotalFreeSize();
  u32 instanceHeapSize = m_DressUpInstanceHeapBeginSize - m_pDressUpInstanceHeap->GetTotalFreeSize();
  u32 allocatorHeapSize = m_DressUpAllocatorHeapBeginSize - m_pDressUpAllocatorHeap->GetTotalFreeSize();

  if (updateTick < 0)
  {
    updateTick = 0;
  }
  if (drawTick < 0)
  {
    drawTick = 0;
  }
  if (gpuTick < 0)
  {
    gpuTick = 0;
  }

  s32 writeSize = CHARA_VIEWER_SPRINTF(m_pMeasureDumpBuffer, m_MeasureDumpRemainSize, "%d, %s, %d, %d, %d, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u\n",
    charaId,
    pName,
    updateTick + upnextTick,
    drawTick,
    gpuTick,
    this->GetResModelSize(),
    this->GetResAnimSize(),
    this->GetResTotalSize(),
    animHeapSize + modelHeapSize,
    instanceHeapSize,
    allocatorHeapSize,
    this->GetMaterialCount(),
    this->GetTextureCount(),
    this->GetMeshCount(),
    this->GetVertexCount(),
    this->GetPolygonCount(),
    this->GetBoneCount());

  GFL_RELEASE_PRINT("%s", m_pMeasureDumpBuffer);
  m_MeasureDumpRemainSize -= writeSize;
  m_pMeasureDumpBuffer += writeSize;
}

void CharaViewerCharaModelManager::MeasureNormalAll()
{
  // 計測モードで再起動
  {
    GameSys::GameManager* pGameManager = m_pGameManager;
    gfl2::heap::HeapBase* pDeviceHeap = m_pDeviceHeap;
    b32 fieldDebugModeEnabled = m_FieldDebugModeEnabled;

    this->Finalize();
    this->Initialize(pGameManager, pDeviceHeap, fieldDebugModeEnabled, true);
  }

  m_UpdateType = UPDATE_TYPE_MEASURE_NORMAL_ALL;
  m_CharaId = 0;
  m_UpdateSeq = 0;

  m_BaseUpdateTick = GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPDATE);
  m_BaseUpnextTick = GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPNEXT);
  m_BaseDrawTick = GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_DRAW);
  m_BaseGpuTick = GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_GPU);

  // csvダンプ用のメモリ確保
  m_pMeasureDumpBuffer = GFL_NEW_ARRAY(m_pHeap) char[CHARA_VIEWER_CHARA_MODEL_MANAGER_MEASURE_DUMP_BUFFER_SIZE];
  m_MeasureDumpRemainSize = CHARA_VIEWER_CHARA_MODEL_MANAGER_MEASURE_DUMP_BUFFER_SIZE;
  m_pMeasureCSVFileName = (m_LoadModelType == MODEL_TYPE_BATTLE)
    ? "load_measurement_bt_normal.csv"
    : "load_measurement_fl_normal.csv";

  s32 writeSize = CHARA_VIEWER_SPRINTF(m_pMeasureDumpBuffer, m_MeasureDumpRemainSize, "モデル番号, モデルラベル, アニメ負荷, 描画負荷, GPU負荷, res:モデル, res:アニメ, res:合計, mem:anim+model, mem:instance, mem:allocator, マテリアル数, テクスチャ枚数, メッシュ数, 頂点数, ポリゴン数, ボーン数\n");
  GFL_RELEASE_PRINT("%s", m_pMeasureDumpBuffer);
  m_MeasureDumpRemainSize -= writeSize;
  m_pMeasureDumpBuffer += writeSize;
}

void CharaViewerCharaModelManager::MeasureDressup()
{
  static const char* s_itemTextList[] =
  {
    "face",       // 顔
    "hair",       // 髪型
    "accbadge",       // バッジアクセサリ
    "acceye",       // アイアクセサリ
    "acchair",       // ヘアアクセサリ
    "bag",       // バッグ
    "bngl",       // バングル
    "body",       // ボディ
    "bottoms",       // ボトムス
    "hat",       // 帽子
    "legs",       // レッグ
    "shoes",       // シューズ
    "tops",       // トップス
    "situ",       // シチュエーション着替え
  };

  const char* pName = "base";
  s32 partsIndex = -1;

  const char* pPartsType = "base";

  if (m_MeasureDressupItemCategory >= 0)
  {
    this->GetCurrentPartsName(static_cast<DressUpParam::ItemCategory>(m_MeasureDressupItemCategory), &pName, &partsIndex);
    pPartsType = s_itemTextList[m_MeasureDressupItemCategory];
  }

  MeasureDressupParam param;
  param.updateTick = GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPDATE) - m_BaseUpdateTick;
  param.updateTick += GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPNEXT) - m_BaseUpnextTick;

  param.drawTick = GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_DRAW) - m_BaseDrawTick;
  param.gpuTick = GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_GPU) - m_BaseGpuTick;
  param.resModelSize = this->GetResModelSize();
  param.resAnimSize = this->GetResAnimSize();
  param.resTotalSize = this->GetResTotalSize();
  param.animHeapSize = m_DressUpAnimHeapBeginSize - m_pDressUpAnimHeap->GetTotalFreeSize();
  param.modelHeapSize = m_DressUpModelHeapBeginSize - m_pDressUpModelHeap->GetTotalFreeSize();
  param.instanceHeapSize = m_DressUpInstanceHeapBeginSize - m_pDressUpInstanceHeap->GetTotalFreeSize();
  param.allocatorHeapSize = m_DressUpAllocatorHeapBeginSize - m_pDressUpAllocatorHeap->GetTotalFreeSize();
  param.materialCount = this->GetMaterialCount();
  param.textureCount = this->GetTextureCount();
  param.meshCount = this->GetMeshCount();
  param.vertexCount = this->GetVertexCount();
  param.polygonCount = this->GetPolygonCount();
  param.boneCount = this->GetBoneCount();

  if (m_MeasureDressupItemCategory < 0)
  {
    m_MeasureDressupBaseParam = param;
  }
  else
  {
    param.updateTick -= m_MeasureDressupBaseParam.updateTick;
    param.drawTick -= m_MeasureDressupBaseParam.drawTick;
    param.gpuTick -= m_MeasureDressupBaseParam.gpuTick;
    param.resModelSize -= m_MeasureDressupBaseParam.resModelSize;
    param.resAnimSize -= m_MeasureDressupBaseParam.resAnimSize;
    param.resTotalSize -= m_MeasureDressupBaseParam.resTotalSize;
    param.animHeapSize -= m_MeasureDressupBaseParam.animHeapSize;
    param.modelHeapSize -= m_MeasureDressupBaseParam.modelHeapSize;
    param.instanceHeapSize -= m_MeasureDressupBaseParam.instanceHeapSize;
    param.allocatorHeapSize -= m_MeasureDressupBaseParam.allocatorHeapSize;
    param.materialCount -= m_MeasureDressupBaseParam.materialCount;
    param.textureCount -= m_MeasureDressupBaseParam.textureCount;
    param.meshCount -= m_MeasureDressupBaseParam.meshCount;
    param.vertexCount -= m_MeasureDressupBaseParam.vertexCount;
    param.polygonCount -= m_MeasureDressupBaseParam.polygonCount;
    param.boneCount -= m_MeasureDressupBaseParam.boneCount;
  }

  if (param.updateTick < 0)
  {
    param.updateTick = 0;
  }
  if (param.drawTick < 0)
  {
    param.drawTick = 0;
  }
  if (param.gpuTick < 0)
  {
    param.gpuTick = 0;
  }

  s32 writeSize = CHARA_VIEWER_SPRINTF(m_pMeasureDumpBuffer, m_MeasureDumpRemainSize, "%u, %s, %s, %d, %d, %d, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u\n",
    m_MeasureDressupItemNo,
    pPartsType,
    pName,
    param.updateTick,
    param.drawTick,
    param.gpuTick,
    param.resModelSize,
    param.resAnimSize,
    param.resTotalSize,
    param.animHeapSize,
    param.modelHeapSize,
    param.instanceHeapSize,
    param.allocatorHeapSize,
    param.materialCount,
    param.textureCount,
    param.meshCount,
    param.vertexCount,
    param.polygonCount,
    param.boneCount);

  GFL_RELEASE_PRINT("%s", m_pMeasureDumpBuffer);
  m_MeasureDumpRemainSize -= writeSize;
  m_pMeasureDumpBuffer += writeSize;
}

void CharaViewerCharaModelManager::MeasureDressupAll()
{
  if (this->GetCurrentCharaDressUpType() == DRESS_UP_TYPE_NONE)
  {
    // 着せ替えモデルでは無いので無視
    return;
  }

  // 計測モードで再起動
  {
    GameSys::GameManager* pGameManager = m_pGameManager;
    gfl2::heap::HeapBase* pDeviceHeap = m_pDeviceHeap;
    b32 fieldDebugModeEnabled = m_FieldDebugModeEnabled;

    this->Finalize();
    this->Initialize(pGameManager, pDeviceHeap, fieldDebugModeEnabled, true);
  }

  m_UpdateType = UPDATE_TYPE_MEASURE_DRESSUP_ALL;
  m_UpdateSeq = 0;
  m_MeasureDressupItemCategory = -1;
  m_MeasureDressupItemNo = 0;

  // csvダンプ用のメモリ確保
  m_pMeasureDumpBuffer = GFL_NEW_ARRAY(m_pHeap) char[CHARA_VIEWER_CHARA_MODEL_MANAGER_MEASURE_DUMP_BUFFER_SIZE];
  m_MeasureDumpRemainSize = CHARA_VIEWER_CHARA_MODEL_MANAGER_MEASURE_DUMP_BUFFER_SIZE;
  if (this->GetCurrentCharaDressUpType() == DRESS_UP_TYPE_HERO)
  {
    m_pMeasureCSVFileName = (m_LoadModelType == MODEL_TYPE_BATTLE)
      ? "load_measurement_bt_dessup_hero.csv"
      : "load_measurement_fl_dressup_hero.csv";
  }
  else
  {
    m_pMeasureCSVFileName = (m_LoadModelType == MODEL_TYPE_BATTLE)
      ? "load_measurement_bt_dessup_heroine.csv"
      : "load_measurement_fl_dressup_heroine.csv";
  }

  m_BaseUpdateTick = GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPDATE);
  m_BaseUpnextTick = GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPNEXT);
  m_BaseDrawTick = GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_DRAW);
  m_BaseGpuTick = GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_GPU);

  s32 writeSize = CHARA_VIEWER_SPRINTF(m_pMeasureDumpBuffer, m_MeasureDumpRemainSize, "アイテム番号, アイテムタイプ, アイテム名, アニメ負荷, 描画負荷, GPU負荷, res:モデル, res:アニメ, res:合計, mem:anim, mem:model, mem:instance, mem:allocator, マテリアル数, テクスチャ枚数, メッシュ数, 頂点数, ポリゴン数, ボーン数\n");
  GFL_RELEASE_PRINT("%s", m_pMeasureDumpBuffer);
  m_MeasureDumpRemainSize -= writeSize;
  m_pMeasureDumpBuffer += writeSize;

  this->LoadCharactor();

  for (u32 i = 0; i < DressUpParam::ITEM_CATEGORY_COUNT; ++i)
  {
    m_DressUpParam[m_SelectedModel].itemParams[i] = -1;
  }
}

void CharaViewerCharaModelManager::DressUpRand()
{
  LoadedModel* pLoadedModel = GetLoadedModel();

  // 何も読まれていなければ何もしない
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return;
  }

  // 着せ替えモデル以外は無視
  if (pLoadedModel->m_DressUpType == CharaViewerCharaModelManager::DRESS_UP_TYPE_NONE)
  {
    return;
  }

  // 性別以外を乱数で選択
  poke_3d::model::DressUpParam* pParam = &m_DressUpParam[m_SelectedModel];

  u32 sex = (pLoadedModel->m_DressUpType == CharaViewerCharaModelManager::DRESS_UP_TYPE_HERO)
    ? PM_MALE
    : PM_FEMALE;
  System::DressUp::Debug_GetDressUpParamRandom(sex, pLoadedModel->m_pDressUpModelResourceManager, pParam);

  // 再読み込み
  this->ReloadCharactor();
}

void CharaViewerCharaModelManager::DressUpDump()
{
  LoadedModel* pLoadedModel = GetLoadedModel();

  // 何も読まれていなければ何もしない
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return;
  }

  // 着せ替えモデル以外は無視
  if (pLoadedModel->m_DressUpType == CharaViewerCharaModelManager::DRESS_UP_TYPE_NONE)
  {
    return;
  }

  static const char* s_bodyTextList[] =
  {
    "BODY_CATEGORY_SEX              = ",       // 性別
    "BODY_CATEGORY_BODY_COLOR       = ",       // 体の色
    "BODY_CATEGORY_EYE_COLOR        = ",       // 目の色
    "BODY_CATEGORY_HAIR_COLOR       = ",       // 髪の色
    "BODY_CATEGORY_FEMALE_LIP_COLOR = ",       // リップの色
  };
  static const char* s_itemTextList[] =
  {
    "ITEM_CATEGORY_FACE             = ",       // 顔
    "ITEM_CATEGORY_HAIR_STYLE       = ",       // 髪型
    "ITEM_CATEGORY_ACCBADGE         = ",       // バッジアクセサリ
    "ITEM_CATEGORY_ACCEYE           = ",       // アイアクセサリ
    "ITEM_CATEGORY_ACCHAIR          = ",       // ヘアアクセサリ
    "ITEM_CATEGORY_BAG              = ",       // バッグ
    "ITEM_CATEGORY_BNGL             = ",       // バングル
    "ITEM_CATEGORY_BODY             = ",       // ボディ
    "ITEM_CATEGORY_BOTTOMS          = ",       // ボトムス
    "ITEM_CATEGORY_HAT              = ",       // 帽子
    "ITEM_CATEGORY_LEGS             = ",       // レッグ
    "ITEM_CATEGORY_SHOES            = ",       // シューズ
    "ITEM_CATEGORY_TOPS             = ",       // トップス
    "ITEM_CATEGORY_SITU             = ",       // シチュエーション着替え
  };

  static const char* s_sexTextList[] =
  {
    "MALE",
    "FEMALE",
  };
  static const char* s_colorTextList[] =
  {
    "",                                 // 性別
    "dressup_color_id_skin_",           // 体の色
    "dressup_color_id_eye_",            // 目の色
    "dressup_color_id_hair_",           // 髪の色
    "dressup_color_id_lip_",            // リップの色
  };
  
  GFL_PRINT("ds_dump_begin ---------------------------------------------\n");
  poke_3d::model::DressUpParam* pParam = &m_DressUpParam[m_SelectedModel];

  const HeaderIdMap* pHeaderIdMap = pLoadedModel->m_pFactoryParam->GetHeaderIdMap();
  const HeaderIdList* pColorIdList = pHeaderIdMap->Find("dressup_color_id");

  for (u32 i = 0; i < poke_3d::model::DressUpParam::BODY_CATEGORY_COUNT; ++i)
  {
    GFL_PRINT(s_bodyTextList[i]);

    poke_3d::model::DressUpParam::BodyCategory bodyCategory = static_cast<poke_3d::model::DressUpParam::BodyCategory>(i);
    s16 bodyId = pParam->bodyParams[bodyCategory];

    if (bodyId < 0)
    {
      GFL_PRINT("NONE\n");
      continue;
    }

    const char* pBodyName = NULL;
    if (bodyCategory == poke_3d::model::DressUpParam::BODY_CATEGORY_SEX)
    {
      pBodyName = s_sexTextList[bodyId];
    }
    else
    {
      u32 colorStartIndex = 0;
      for (u32 j = 0; j < pColorIdList->GetDataCount(); ++j)
      {
        if (::strstr(pColorIdList->GetData(j)->pName, s_colorTextList[bodyCategory]) != NULL)
        {
          colorStartIndex = j;
          break;
        }
      }

      pBodyName = pColorIdList->GetData(colorStartIndex + bodyId)->pName;
    }

    char buf[256] = "";

    // 大文字化
    u32 nameLength = ::strlen(pBodyName);
    GFL_ASSERT(sizeof(buf) > nameLength);

    for (u32 j = 0; j < nameLength; ++j)
    {
      buf[j] = ::toupper(pBodyName[j]);
    }

    GFL_PRINT(buf);
    GFL_PRINT("\n");
  }

  for (u32 i = 0; i < poke_3d::model::DressUpParam::ITEM_CATEGORY_COUNT; ++i)
  {
    GFL_PRINT(s_itemTextList[i]);

    poke_3d::model::DressUpParam::ItemCategory itemCategory = static_cast<poke_3d::model::DressUpParam::ItemCategory>(i);
    s16 itemId = pParam->itemParams[itemCategory];

    if (itemId < 0)
    {
      GFL_PRINT("NONE\n");
      continue;
    }

    // アイテム名取得
    const HeaderIdList* pItemIdList = pLoadedModel->m_pFactoryParam->GetItemIdList(pLoadedModel->m_DressUpType, itemCategory);
    const char* pItemName = pItemIdList->GetData(itemId)->pName;
    
    char buf[256] = "";

    // 大文字化
    u32 nameLength = ::strlen(pItemName);
    GFL_ASSERT(sizeof(buf) > nameLength);

    for (u32 j = 0; j < nameLength; ++j)
    {
      buf[j] = ::toupper(pItemName[j]);
    }

    GFL_PRINT(buf);
    GFL_PRINT("\n");
  }
  GFL_PRINT("ds_dump_end -----------------------------------------------\n");
}

void CharaViewerCharaModelManager::ResetAllParts()
{
  LoadedModel* pLoadedModel = GetLoadedModel();

  // 何も読まれていなければ何もしない
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return;
  }

  // 着せ替えモデル以外は無視
  if (pLoadedModel->m_DressUpType == CharaViewerCharaModelManager::DRESS_UP_TYPE_NONE)
  {
    return;
  }

  // デフォルトに戻す
  if (pLoadedModel->m_DressUpType == DRESS_UP_TYPE_HERO)
  {
    System::DressUp::GetDressUpParamDefaultHero(&m_DressUpParam[m_SelectedModel]);
  }
  else
  {
    System::DressUp::GetDressUpParamDefaultHeroine(&m_DressUpParam[m_SelectedModel]);
  }

  // 再読み込み
  this->ReloadCharactor();
}

void CharaViewerCharaModelManager::SaveCurrentPartsSetting()
{
  // 着せ替えモデルじゃなければ終了
  if (GetLoadedModel()->m_DressUpType == DRESS_UP_TYPE_NONE)
  {
    return;
  }

  GameSys::GameData* pGameData = m_pGameManager->GetGameData();

  // セーブデータ読み込み
  pGameData->LoadStart(m_pHeap);

  // セーブデータ読み込み完了待ち
  bool loadEnd = false;
  while (!loadEnd)
  {
    loadEnd = (pGameData->LoadMain() != gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE);
  }

  // 現在の着せ替え設定を保存
  pGameData->GetPlayerStatus()->SetDressup(m_DressUpParam[m_SelectedModel]);

  // セーブデータ書き込み開始
  pGameData->SaveStart(m_pHeap);

  // セーブデータ書き込み終了待ち
  bool saveEnd = false;
  while (!saveEnd)
  {
    saveEnd = (pGameData->SaveMain() != gfl2::fs::FsCtrBackupSystem::RESULT_CONTINUE);
  }
}

void CharaViewerCharaModelManager::FixPartsCombination(poke_3d::model::DressUpParam::ItemCategory itemCategory, s32 nextItemIndex, poke_3d::model::DressUpModelResourceManagerCore* pDressUpModelResourceManager)
{
  if (nextItemIndex < 0)
  {
    return;
  }

  // シチュエーション着替えは特別
  if (itemCategory == DressUpParam::ITEM_CATEGORY_SITU)
  {
    // face, bngl, situ以外全て外す
    for (u32 i = 0; i < DressUpParam::ITEM_CATEGORY_COUNT; ++i)
    {
      if ((i == DressUpParam::ITEM_CATEGORY_FACE) || (i == DressUpParam::ITEM_CATEGORY_BNGL) || (i == DressUpParam::ITEM_CATEGORY_SITU))
      {
        continue;
      }

      m_DressUpParam[m_SelectedModel].itemParams[i] = -1;
    }

    return;
  }

  // situを外す
  if ((itemCategory != DressUpParam::ITEM_CATEGORY_FACE) && (itemCategory != DressUpParam::ITEM_CATEGORY_BNGL))
  {
    m_DressUpParam[m_SelectedModel].itemParams[DressUpParam::ITEM_CATEGORY_SITU] = -1;
  }

  // hat -> hair
  // hat -> acchair
  if (itemCategory == DressUpParam::ITEM_CATEGORY_HAT)
  {
    // 帽子をかぶれる髪型か？
    if (!pDressUpModelResourceManager->CanDressUpHairWearHat(m_DressUpParam[m_SelectedModel].itemParams[DressUpParam::ITEM_CATEGORY_HAIR_STYLE]))
    {
      // hairを外す
      m_DressUpParam[m_SelectedModel].itemParams[DressUpParam::ITEM_CATEGORY_HAIR_STYLE] = -1;
    }

    // acchairを外す
    m_DressUpParam[m_SelectedModel].itemParams[DressUpParam::ITEM_CATEGORY_ACCHAIR] = -1;
  }
  // acchair -> hat
  else if (itemCategory == DressUpParam::ITEM_CATEGORY_ACCHAIR)
  {
    // hatを外す
    m_DressUpParam[m_SelectedModel].itemParams[DressUpParam::ITEM_CATEGORY_HAT] = -1;
  }
  // hair -> hat
  else if (itemCategory == DressUpParam::ITEM_CATEGORY_HAIR_STYLE)
  {
    // 帽子をかぶれる髪型か？
    if (!pDressUpModelResourceManager->CanDressUpHairWearHat(nextItemIndex))
    {
      // hatを外す
      m_DressUpParam[m_SelectedModel].itemParams[DressUpParam::ITEM_CATEGORY_HAT] = -1;
    }
  }
}

void CharaViewerCharaModelManager::SelectNextParts(poke_3d::model::DressUpParam::ItemCategory itemCategory, bool isForward)
{
  LoadedModel* pLoadedModel = GetLoadedModel();
  DressUpModelResourceManagerCore* pDressUpModelResourceManager = pLoadedModel->m_pDressUpModelResourceManager;

  // 読み込まれていないなら無視
  if (GetLoadedModel()->m_pCharaModel == NULL)
  {
    return;
  }

  // 現在のアイテムパラメータ取得
  s32 itemIndex = m_DressUpParam[m_SelectedModel].itemParams[itemCategory];
  s32 partsIndex = (itemIndex >= 0)
    ? (s32)pDressUpModelResourceManager->GetDressUpPartsIndex(itemCategory, itemIndex)
    : -1;

  // 次のパーツを選択
  s32 step = this->GetSlideValue(isForward);
  s32 nextPartsIndex = partsIndex + step;

  s32 minLimit = -1;
  u32 partsCountLimit = pDressUpModelResourceManager->GetDressUpPartsCount(itemCategory);
  if (nextPartsIndex < minLimit)
  {
    nextPartsIndex = partsCountLimit - 1;
  }
  else if ((u32)nextPartsIndex >= partsCountLimit)
  {
    nextPartsIndex = minLimit;
  }

  // アイテムのインデックス取得
  s32 nextItemIndex = (nextPartsIndex >= 0)
    ? pDressUpModelResourceManager->GetDressUpItemIndexDefault(itemCategory, nextPartsIndex)
    : -1;

  m_DressUpParam[m_SelectedModel].itemParams[itemCategory] = nextItemIndex;

  // アイテムの組み合わせ修正
  this->FixPartsCombination(itemCategory, nextItemIndex, pDressUpModelResourceManager);

  // 再読み込み
  this->ReloadCharactor();
}

const char* CharaViewerCharaModelManager::GetPartsName(poke_3d::model::DressUpParam::ItemCategory itemCategory, u32 index) const
{
  const LoadedModel* pLoadedModel = GetLoadedModel();
  if (pLoadedModel->m_pCharaModel == NULL)
  {
    return PARTS_NONE_NAME;
  }

  const FactoryParam* pFactoryParam = pLoadedModel->m_pFactoryParam;
  const HeaderIdList* pPartsIdList = pFactoryParam->GetPartsIdList(pLoadedModel->m_DressUpType, itemCategory);
  if (pPartsIdList == NULL)
  {
    return PARTS_NONE_NAME;
  }

  return pPartsIdList->GetData(index)->GetPartsName();
}

void CharaViewerCharaModelManager::GetCurrentPartsName(poke_3d::model::DressUpParam::ItemCategory itemCategory, const char** out_ppPartsName, s32* out_pPartsIndex) const
{
  *out_ppPartsName = PARTS_NONE_NAME;
  *out_pPartsIndex = -1;

  if (GetLoadedModel()->m_pCharaModel == NULL)
  {
    return;
  }
  if (m_DressUpParam[m_SelectedModel].itemParams[itemCategory] < 0)
  {
    return;
  }

  const LoadedModel* pLoadedModel = GetLoadedModel();
  DressUpModelResourceManagerCore* pDressUpModelResourceManager = pLoadedModel->m_pDressUpModelResourceManager;

  s32 itemIndex = m_DressUpParam[m_SelectedModel].itemParams[itemCategory];
  u32 partsIndex = pDressUpModelResourceManager->GetDressUpPartsIndex(itemCategory, itemIndex);

  *out_ppPartsName = this->GetPartsName(itemCategory, partsIndex);
  *out_pPartsIndex = partsIndex;
}

FactoryParam* CharaViewerCharaModelManager::GetCurrentFactoryParam()
{
  return m_pFactoryParamList[m_LoadModelType];
}

const FactoryParam* CharaViewerCharaModelManager::GetCurrentFactoryParam() const
{
  return m_pFactoryParamList[m_LoadModelType];
}

poke_3d::model::DressUpModelResourceManagerCore* CharaViewerCharaModelManager::GetDefaultDressUpResourceManager()
{
  return m_pFactoryParamList[MODEL_TYPE_BATTLE]->GetDressUpModelResourceManager(DRESS_UP_TYPE_HERO, 0);
}
const poke_3d::model::DressUpModelResourceManagerCore* CharaViewerCharaModelManager::GetDefaultDressUpResourceManager() const
{
  return m_pFactoryParamList[MODEL_TYPE_BATTLE]->GetDressUpModelResourceManager(DRESS_UP_TYPE_HERO, 0);
}

s32 CharaViewerCharaModelManager::GetSlideValue(bool isForward) const
{
  gfl2::ui::Button* pButton = m_pGameManager->GetUiDeviceManager()->GetButton(0);
  s32 slideValue = (pButton->IsHold(gfl2::ui::BUTTON_L) || pButton->IsHold(gfl2::ui::BUTTON_R)) ? 10 : 1;

  if (!isForward)
  {
    slideValue = -slideValue;
  }

  return slideValue;
}

void CharaViewerCharaModelManager::GetMaxInstanceSizeParts(s16* out_pInstanceIndex, s16* out_pAllocatorIndex, poke_3d::model::DressUpParam::ItemCategory itemCategory, bool hatEnable)
{
  *out_pInstanceIndex = -1;
  *out_pAllocatorIndex = -1;

  if (GetLoadedModel()->m_pCharaModel == NULL)
  {
    return;
  }
  if (GetLoadedModel()->m_DressUpType == DRESS_UP_TYPE_NONE)
  {
    return;
  }

  DressUpParam* pParam = &m_DressUpParam[m_SelectedModel];
  for (u32 i = 0; i < DressUpParam::ITEM_CATEGORY_COUNT; ++i)
  {
    pParam->itemParams[i] = -1;
  }
  this->ReloadCharactor();
  
  u32 maxInstanceSize = 0;
  u32 maxAllocatorSize = 0;

  do {
    this->SelectNextParts(itemCategory, true);

    if (!hatEnable && (itemCategory == DressUpParam::ITEM_CATEGORY_HAIR_STYLE))
    {
      bool canHat = GetLoadedModel()->m_pFactoryParam->GetDressUpModelResourceManager(GetLoadedModel()->m_DressUpType, 0)->CanDressUpHairWearHat(pParam->itemParams[itemCategory]);
      if (!canHat)
      {
        continue;
      }
    }

    u32 currentInstanceSize = m_DressUpInstanceHeapBeginSize - m_pDressUpInstanceHeap->GetTotalFreeSize();
    if (maxInstanceSize < currentInstanceSize)
    {
      maxInstanceSize = currentInstanceSize;
      *out_pInstanceIndex = pParam->itemParams[itemCategory];
    }
    u32 currentAllocatorSize = m_DressUpAllocatorHeapBeginSize - m_pDressUpAllocatorHeap->GetTotalFreeSize();
    if (maxAllocatorSize < currentAllocatorSize)
    {
      maxAllocatorSize = currentAllocatorSize;
      *out_pAllocatorIndex = pParam->itemParams[itemCategory];
    }
  } while(pParam->itemParams[itemCategory] != -1);
}

GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
