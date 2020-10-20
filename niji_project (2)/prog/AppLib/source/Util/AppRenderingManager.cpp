//=============================================================================================
/**
 * @file   AppRenderingManager.cpp
 * @brief  アプリ用のパイプラインを管理するクラス
 * @author k.ohno
 * @date   2015.2.28
 */
//=============================================================================================

// gflib2のインクルード
#include <thread/include/gfl2_ThreadStatic.h>
#include <Layout/include/gfl2_LytSys.h>
#include <Effect/include/gfl2_EffectSystem.h>
#include <Effect/include/gfl2_EffectRenderPath.h>

// PokeToolのインクルード
#include <PokeTool/include/PokeModelSystem.h>
#include <PokeTool/include/PokeModel.h>

// poke_3d_modelのインクルード
#include <model/include/gfl2_BaseCamera.h>
#include <model/include/gfl2_BaseModel.h>
#include <model/include/gfl2_CharaModel.h>
#include <model/include/gfl2_DressUpModel.h>

// nijiのインクルード
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Util/app_util_RenderTargetTexture.h>
#include <AppLib/include/Util/AppCamera.h>
#include <App/RaderChart/include/app_tool_RaderChart.h>
#include <App/RaderChart/include/app_tool_RaderChartRenderPath.h>
#include <System/include/GflUse.h>
#include <System/include/nijiAllocator.h>
#include <System/include/Camera/InFrameCamera.h>
#include <GameSys/include/GameManager.h>

#include "AppRenderingManagerPokeModelRenderer.h"
#include "AppRenderingManagerRaderChartRenderer.h"
#include "AppRenderingManagerEffectRenderer.h"

#include <RenderingEngine/include/Renderer/Util/gfl2_StretchBltFrameBufferPath.h>
#include <renderer/include/gfl2_DistortionPostRenderPath.h>




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


AppRenderingManager::AppRenderingManager(void)
  : mCreateState(CREATE_STATE_NONE),
    mDesc(),

    mpCreateHeap(NULL),
    mpCreateNijiAllocatorForEdgeMapTexture(NULL),
    mpAsyncFileManager(NULL),  // @fix cov_ctr[10539]: 未初期化をNULLで初期化

    mpOffScreenRenderTargetTexture( NULL ),

    mLayoutLayerNumUpper( 0 ),
    mLayoutLayerNumLower( 0 ),

    mpPokeModelRendererBuilder( NULL ),
  
    mpRaderChartRenderingPipeLineUpperLeft( NULL ),
    mpRaderChartRenderingPipeLineUpperRight( NULL ),
    mpRaderChartRenderingPipeLineLower( NULL ),
    mpColorBufferLiner( NULL ),
    mpColorBufferBlock( NULL )
{
  for(u32 i=0; i<LAYOUT_LAYER_NUM_MAX; ++i)
  {
    mpLayoutRenderingPipeLineUpperLeft[i]  = NULL;
    mpLayoutRenderingPipeLineUpperRight[i] = NULL;
    mpLayoutRenderingPipeLineLower[i]      = NULL;

    mLayoutBitFlagUpperLeft[i]  = LAYOUT_BIT_FLAG_NONE;
    mLayoutBitFlagUpperRight[i] = LAYOUT_BIT_FLAG_NONE;
    mLayoutBitFlagLower[i]      = LAYOUT_BIT_FLAG_NONE;
  }

  for(s32 renderPlace=0; renderPlace<RenderPlaceType::NUM; ++renderPlace)
  {
    mPokeModelLayerNum[renderPlace] = 0;
  }
  for(s32 renderPlace=0; renderPlace<RenderPlaceType::TOTAL_NUM; ++renderPlace)
  {
    for(u32 i=0; i<POKEMODEL_LAYER_NUM_MAX; ++i)
    {
      mpPokeModelRenderingPipeLine[renderPlace][i]  = NULL;
    }
  }

  for(s32 renderPlace=0; renderPlace<RenderPlaceType::NUM; ++renderPlace)
  {
    mEffectLayerNum[renderPlace] = 0;
  }
  for(s32 renderPlace=0; renderPlace<RenderPlaceType::TOTAL_NUM; ++renderPlace)
  {
    for(u32 i=0; i<EFFECT_LAYER_NUM_MAX; ++i)
    {
      mpEffectRenderingPipeLine[renderPlace][i]  = NULL;
    }
  }
}


AppRenderingManager::~AppRenderingManager()
{
  if( !(this->CanDestroy()) )
  {
    // 破棄してはいけない状態のとき
    GFL_ASSERT(0);  // 破棄してはいけないときに破棄しようとしている。ケア済みのASSERT。
  }
  u32 loopCount = 0;
  while( !(this->CanDestroy()) )
  {
    this->Update();
    gfl2::thread::ThreadStatic::CurrentSleep(1);
    ++loopCount;
    if(loopCount >= 30*120)  // 無限ループ対策
    {
      GFL_ASSERT(0);  // 大分経過したが破棄できる状態になっていない。ケアしようがないASSERT。もう破棄しにいくしかない。
      break;
    }
  }

  // ここまで来たら、破棄してもいい状態になったので破棄する

  {
    for(s32 renderPlace=0; renderPlace<RenderPlaceType::TOTAL_NUM; ++renderPlace)
    {
      for(u32 i=0; i<EFFECT_LAYER_NUM_MAX; ++i)
      {
        GFL_SAFE_DELETE(mpEffectRenderingPipeLine[renderPlace][i]);
      }
    }
  }

  GFL_SAFE_DELETE(mpRaderChartRenderingPipeLineLower);
  GFL_SAFE_DELETE(mpRaderChartRenderingPipeLineUpperRight);
  GFL_SAFE_DELETE(mpRaderChartRenderingPipeLineUpperLeft);

  if(mpPokeModelRendererBuilder)
  {
    for(s32 renderPlace=0; renderPlace<RenderPlaceType::TOTAL_NUM; ++renderPlace)
    {
      for(u32 i=0; i<POKEMODEL_LAYER_NUM_MAX; ++i)
      {
        mpPokeModelRendererBuilder->DestroyRenderingPipeLine(mpPokeModelRenderingPipeLine[renderPlace][i]);    mpPokeModelRenderingPipeLine[renderPlace][i] = NULL;
      }
    }
    GFL_SAFE_DELETE(mpPokeModelRendererBuilder);
  }
  
  for(u32 i=0; i<LAYOUT_LAYER_NUM_MAX; ++i)
  {
    GFL_SAFE_DELETE(mpLayoutRenderingPipeLineLower[i]);
    GFL_SAFE_DELETE(mpLayoutRenderingPipeLineUpperRight[i]);
    GFL_SAFE_DELETE(mpLayoutRenderingPipeLineUpperLeft[i]);
  }
}


void AppRenderingManager::StartAsyncCreate(
    const Description&          desc,
    app::util::Heap*            pHeap,
    System::nijiAllocator*      pNijiAllocatorForEdgeMapTexture,
    gfl2::fs::AsyncFileManager* pAsyncFileManager 
)
{
  if(mCreateState == CREATE_STATE_NONE)
  {
    mCreateState = CREATE_STATE_CREATING;
    mDesc = desc;

    // レイアウト描画パス
    {
      mLayoutLayerNumUpper = getLayoutLayerNum(mDesc.renderDesc[RenderPlaceType::UPPER_DISPLAY].renderOrder);
      for(u32 i=0; i<mLayoutLayerNumUpper; ++i)
      {
        mpLayoutRenderingPipeLineUpperLeft[i]  = GFL_NEW(pHeap->GetDeviceHeap()) LayoutRenderingPipeLine(pHeap->GetDeviceHeap(), 512, 256, mDesc.renderDesc[RenderPlaceType::UPPER_DISPLAY].lytWkMax);
        if(mDesc.isUpperDisplayStereo)
        {
          mpLayoutRenderingPipeLineUpperRight[i] = GFL_NEW(pHeap->GetDeviceHeap()) LayoutRenderingPipeLine(pHeap->GetDeviceHeap(), 512, 256, mDesc.renderDesc[RenderPlaceType::UPPER_DISPLAY].lytWkMax);
        }
      }
    }
    {
      mLayoutLayerNumLower = getLayoutLayerNum(mDesc.renderDesc[RenderPlaceType::LOWER_DISPLAY].renderOrder);
      for(u32 i=0; i<mLayoutLayerNumLower; ++i)
      {
        mpLayoutRenderingPipeLineLower[i]      = GFL_NEW(pHeap->GetDeviceHeap()) LayoutRenderingPipeLine(pHeap->GetDeviceHeap(), 512, 256, mDesc.renderDesc[RenderPlaceType::LOWER_DISPLAY].lytWkMax);
      }
    }

    // ポケモンモデル描画パス
    if(
           isTherePokeModel(mDesc.renderDesc[RenderPlaceType::UPPER_DISPLAY].renderOrder)
        || isTherePokeModel(mDesc.renderDesc[RenderPlaceType::LOWER_DISPLAY].renderOrder)
        || isTherePokeModel(mDesc.renderDesc[RenderPlaceType::OFF_SCREEN].renderOrder)
    )
    {
      mpPokeModelRendererBuilder = GFL_NEW(pHeap->GetDeviceHeap()) app_rendering_manager_internal::PokeModelRendererBuilder();
      mpPokeModelRendererBuilder->StartAsyncCreate(
          pHeap,
          pAsyncFileManager 
      );
    }

    mpCreateHeap                           = pHeap;
    mpCreateNijiAllocatorForEdgeMapTexture = pNijiAllocatorForEdgeMapTexture;
    mpAsyncFileManager                     = pAsyncFileManager;
    
    this->continueAsyncCreate();
  }
  else
  {
    GFL_ASSERT(0);  // 既に生成済み。開発中に気付かせるためのASSERT。
  }
}

void AppRenderingManager::continueAsyncCreate(void)
{
  if(mCreateState == CREATE_STATE_CREATING)
  {
    // ポケモンモデル描画パス
    if(
           isTherePokeModel(mDesc.renderDesc[RenderPlaceType::UPPER_DISPLAY].renderOrder)
        || isTherePokeModel(mDesc.renderDesc[RenderPlaceType::LOWER_DISPLAY].renderOrder)
        || isTherePokeModel(mDesc.renderDesc[RenderPlaceType::OFF_SCREEN].renderOrder)
    )
    {
      if( mpPokeModelRendererBuilder->IsCreated() )
      {
        for(s32 renderPlace=0; renderPlace<RenderPlaceType::NUM; ++renderPlace)
        {
          mPokeModelLayerNum[renderPlace] = getPokeModelLayerNum(mDesc.renderDesc[renderPlace].renderOrder);
          for(u32 i=0; i<mPokeModelLayerNum[renderPlace]; ++i)
          {
            mpPokeModelRenderingPipeLine[renderPlace][i]  = mpPokeModelRendererBuilder->CreateRenderingPipeLine(
                (renderPlace==RenderPlaceType::OFF_SCREEN)?(app_rendering_manager_internal::PokeModelRendererType::Mode::APP_ORIGINAL):(app_rendering_manager_internal::PokeModelRendererType::Mode::COMMON),
                mpCreateHeap,
                mpCreateNijiAllocatorForEdgeMapTexture,
                mDesc.renderDesc[renderPlace].pokeModelNumMax,
                mDesc.renderDesc[renderPlace].modelNumMax,
                mDesc.renderDesc[renderPlace].charaModelNumMax,
                mDesc.renderDesc[renderPlace].dressUpModelNumMax,
                mDesc.renderDesc[renderPlace].useDistortionPath
            );
            if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mDesc.isUpperDisplayStereo )
            {
              mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][i] = mpPokeModelRendererBuilder->CreateRenderingPipeLine(
                  app_rendering_manager_internal::PokeModelRendererType::Mode::COMMON,
                  mpCreateHeap,
                  mpCreateNijiAllocatorForEdgeMapTexture,
                  mDesc.renderDesc[renderPlace].pokeModelNumMax,
                  mDesc.renderDesc[renderPlace].modelNumMax,
                  mDesc.renderDesc[renderPlace].charaModelNumMax,
                  mDesc.renderDesc[renderPlace].dressUpModelNumMax,
                  mDesc.renderDesc[renderPlace].useDistortionPath
              );
            }
          }
        }
      }
      else
      {
        return;
      }
    }
 
    // レーダーチャート描画パス
    if( isThereRaderChart(mDesc.renderDesc[RenderPlaceType::UPPER_DISPLAY].renderOrder) )
    {
      mpRaderChartRenderingPipeLineUpperLeft  = GFL_NEW(mpCreateHeap->GetDeviceHeap()) app_rendering_manager_internal::RaderChartRenderingPipeLine(mpCreateHeap);
      if(mDesc.isUpperDisplayStereo)
      {
        mpRaderChartRenderingPipeLineUpperRight  = GFL_NEW(mpCreateHeap->GetDeviceHeap()) app_rendering_manager_internal::RaderChartRenderingPipeLine(mpCreateHeap);
      }
    }
    if( isThereRaderChart(mDesc.renderDesc[RenderPlaceType::LOWER_DISPLAY].renderOrder) )
    {
      mpRaderChartRenderingPipeLineLower      = GFL_NEW(mpCreateHeap->GetDeviceHeap()) app_rendering_manager_internal::RaderChartRenderingPipeLine(mpCreateHeap);
    }
 
    // エフェクト描画パス
    if(
           isThereEffect(mDesc.renderDesc[RenderPlaceType::UPPER_DISPLAY].renderOrder)
        || isThereEffect(mDesc.renderDesc[RenderPlaceType::LOWER_DISPLAY].renderOrder)
    )
    {
      for(s32 renderPlace=0; renderPlace<RenderPlaceType::NUM; ++renderPlace)
      {
        mEffectLayerNum[renderPlace] = getEffectLayerNum(mDesc.renderDesc[renderPlace].renderOrder);
        for(u32 i=0; i<mEffectLayerNum[renderPlace]; ++i)
        {
          mpEffectRenderingPipeLine[renderPlace][i]  = GFL_NEW(mpCreateHeap->GetDeviceHeap()) app_rendering_manager_internal::EffectRenderingPipeLine();
          if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mDesc.isUpperDisplayStereo )
          {
            mpEffectRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][i] = GFL_NEW(mpCreateHeap->GetDeviceHeap()) app_rendering_manager_internal::EffectRenderingPipeLine();
          }
        }
      }
    }

    // もう使わないのでNULLにしておく
    mpCreateHeap                           = NULL;
    mpCreateNijiAllocatorForEdgeMapTexture = NULL;
    mpAsyncFileManager                     = NULL;

    mCreateState = CREATE_STATE_CREATED;
  }
}


void AppRenderingManager::SyncCreate(
    const Description&          desc,
    app::util::Heap*            pHeap,
    System::nijiAllocator*      pNijiAllocatorForEdgeMapTexture,
    gfl2::fs::AsyncFileManager* pAsyncFileManager 
)
{
  if(mCreateState == CREATE_STATE_NONE)
  {
    mCreateState = CREATE_STATE_CREATING;
    mDesc = desc;

    // レイアウト描画パス
    {
      mLayoutLayerNumUpper = getLayoutLayerNum(mDesc.renderDesc[RenderPlaceType::UPPER_DISPLAY].renderOrder);
      for(u32 i=0; i<mLayoutLayerNumUpper; ++i)
      {
        mpLayoutRenderingPipeLineUpperLeft[i]  = GFL_NEW(pHeap->GetDeviceHeap()) LayoutRenderingPipeLine(pHeap->GetDeviceHeap(), 512, 256, mDesc.renderDesc[RenderPlaceType::UPPER_DISPLAY].lytWkMax);
        if(mDesc.isUpperDisplayStereo)
        {
          mpLayoutRenderingPipeLineUpperRight[i] = GFL_NEW(pHeap->GetDeviceHeap()) LayoutRenderingPipeLine(pHeap->GetDeviceHeap(), 512, 256, mDesc.renderDesc[RenderPlaceType::UPPER_DISPLAY].lytWkMax);
        }
      }
    }
    {
      mLayoutLayerNumLower = getLayoutLayerNum(mDesc.renderDesc[RenderPlaceType::LOWER_DISPLAY].renderOrder);
      for(u32 i=0; i<mLayoutLayerNumLower; ++i)
      {
        mpLayoutRenderingPipeLineLower[i]      = GFL_NEW(pHeap->GetDeviceHeap()) LayoutRenderingPipeLine(pHeap->GetDeviceHeap(), 512, 256, mDesc.renderDesc[RenderPlaceType::LOWER_DISPLAY].lytWkMax);
      }
    }

    // ポケモンモデル描画パス
    if(
           isTherePokeModel(mDesc.renderDesc[RenderPlaceType::UPPER_DISPLAY].renderOrder)
        || isTherePokeModel(mDesc.renderDesc[RenderPlaceType::LOWER_DISPLAY].renderOrder)
        || isTherePokeModel(mDesc.renderDesc[RenderPlaceType::OFF_SCREEN].renderOrder)
    )
    {
      mpPokeModelRendererBuilder = GFL_NEW(pHeap->GetDeviceHeap()) app_rendering_manager_internal::PokeModelRendererBuilder();
      mpPokeModelRendererBuilder->SyncCreate(
          pHeap,
          pAsyncFileManager 
      );

      for(s32 renderPlace=0; renderPlace<RenderPlaceType::NUM; ++renderPlace)
      {
        mPokeModelLayerNum[renderPlace] = getPokeModelLayerNum(mDesc.renderDesc[renderPlace].renderOrder);
        for(u32 i=0; i<mPokeModelLayerNum[renderPlace]; ++i)
        {
          mpPokeModelRenderingPipeLine[renderPlace][i]  = mpPokeModelRendererBuilder->CreateRenderingPipeLine(
              (renderPlace==RenderPlaceType::OFF_SCREEN)?(app_rendering_manager_internal::PokeModelRendererType::Mode::APP_ORIGINAL):(app_rendering_manager_internal::PokeModelRendererType::Mode::COMMON),
              pHeap,
              pNijiAllocatorForEdgeMapTexture,
              mDesc.renderDesc[renderPlace].pokeModelNumMax,
              mDesc.renderDesc[renderPlace].modelNumMax,
              mDesc.renderDesc[renderPlace].charaModelNumMax,
              mDesc.renderDesc[renderPlace].dressUpModelNumMax,
              mDesc.renderDesc[renderPlace].useDistortionPath
          );
          if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mDesc.isUpperDisplayStereo )
          {
            mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][i] = mpPokeModelRendererBuilder->CreateRenderingPipeLine(
                app_rendering_manager_internal::PokeModelRendererType::Mode::COMMON,
                pHeap,
                pNijiAllocatorForEdgeMapTexture,
                mDesc.renderDesc[renderPlace].pokeModelNumMax,
                mDesc.renderDesc[renderPlace].modelNumMax,
                mDesc.renderDesc[renderPlace].charaModelNumMax,
                mDesc.renderDesc[renderPlace].dressUpModelNumMax,
                mDesc.renderDesc[renderPlace].useDistortionPath
            );
          }
        }
      }
    }

    // レーダーチャート描画パス
    if( isThereRaderChart(mDesc.renderDesc[RenderPlaceType::UPPER_DISPLAY].renderOrder) )
    {
      mpRaderChartRenderingPipeLineUpperLeft  = GFL_NEW(pHeap->GetDeviceHeap()) app_rendering_manager_internal::RaderChartRenderingPipeLine(pHeap);
      if(mDesc.isUpperDisplayStereo)
      {
        mpRaderChartRenderingPipeLineUpperRight  = GFL_NEW(pHeap->GetDeviceHeap()) app_rendering_manager_internal::RaderChartRenderingPipeLine(pHeap);
      }
    }
    if( isThereRaderChart(mDesc.renderDesc[RenderPlaceType::LOWER_DISPLAY].renderOrder) )
    {
      mpRaderChartRenderingPipeLineLower      = GFL_NEW(pHeap->GetDeviceHeap()) app_rendering_manager_internal::RaderChartRenderingPipeLine(pHeap);
    }

    // エフェクト描画パス
    if(
           isThereEffect(mDesc.renderDesc[RenderPlaceType::UPPER_DISPLAY].renderOrder)
        || isThereEffect(mDesc.renderDesc[RenderPlaceType::LOWER_DISPLAY].renderOrder)
    )
    {
      for(s32 renderPlace=0; renderPlace<RenderPlaceType::NUM; ++renderPlace)
      {
        mEffectLayerNum[renderPlace] = getEffectLayerNum(mDesc.renderDesc[renderPlace].renderOrder);
        for(u32 i=0; i<mEffectLayerNum[renderPlace]; ++i)
        {
          mpEffectRenderingPipeLine[renderPlace][i]  = GFL_NEW(pHeap->GetDeviceHeap()) app_rendering_manager_internal::EffectRenderingPipeLine();
          if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mDesc.isUpperDisplayStereo )
          {
            mpEffectRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][i] = GFL_NEW(pHeap->GetDeviceHeap()) app_rendering_manager_internal::EffectRenderingPipeLine();
          }
        }
      }
    }

    mCreateState = CREATE_STATE_CREATED;
  }
  else
  {
    GFL_ASSERT(0);  // 既に生成済み。開発中に気付かせるためのASSERT。
  }
}


bool AppRenderingManager::IsCreated(void) const
{
  return ( mCreateState == CREATE_STATE_CREATED );
}


bool AppRenderingManager::CanDestroy(void) const
{
  return ( mCreateState == CREATE_STATE_NONE || mCreateState == CREATE_STATE_CREATED );
}


void AppRenderingManager::SetOffScreenRenderTargetTexture(RenderPlaceType::Tag renderPlace, app::util::RenderTargetTexture* renderTargetTexture)
{
  if(renderTargetTexture)
  {
    if(renderPlace == RenderPlaceType::OFF_SCREEN)
    {
      mpOffScreenRenderTargetTexture = renderTargetTexture;
      
      // オフスクリーンのときは外部からデプスステンシルテクスチャをセット/アンセットするが、セットはここでは行わず初回使用時に行う。
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるASSERT
    }
  }
  else
  {
    GFL_ASSERT(0);  // NULLのセットはUnset...関数で。開発中に気付かせるためのASSERT
  }
}
void AppRenderingManager::UnsetOffScreenRenderTargetTexture(RenderPlaceType::Tag renderPlace)
{
  if(renderPlace == RenderPlaceType::OFF_SCREEN)
  {
    // @fix GFNMCat[3636] アプリに出るポケモンモデル：CTR版でＩＤエッジが表示されていない
    // オフスクリーンのときは外部からデプスステンシルテクスチャをセット/アンセットする。
    app_rendering_manager_internal::PokeModelRenderingPipeLine* pPokeModelRenderingPipeLine = mpPokeModelRenderingPipeLine[RenderPlaceType::OFF_SCREEN][0];
    if(pPokeModelRenderingPipeLine) pPokeModelRenderingPipeLine->UnsetOutLinePathOuterEdgeMapTexture();

    mpOffScreenRenderTargetTexture = NULL;
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるASSERT
  }
}


void AppRenderingManager::AddDrawableLytWkForOneFrame(RenderPlaceType::Tag renderPlace, gfl2::lyt::LytWk* pLytWk, u32 layerNo, u8 order)
{
  ////////////////////////////////////////////////////////////////
  // ポーズのときはDrawだけがまわるので描画後にクリアすることにしたが、描画後にクリアし損ねたものもあるかもしれないのでここでそれらをクリアする。
  ////////////////////////////////////////////////////////////////
  for(u32 i=0; i<LAYOUT_LAYER_NUM_MAX; ++i)
  {
    if(mLayoutBitFlagUpperLeft[i] & LAYOUT_BIT_FLAG_DRAW_SOME)
    { 
      if(mpLayoutRenderingPipeLineUpperLeft[i]) mpLayoutRenderingPipeLineUpperLeft[i]->GetLayoutRenderPath()->ClearDrawableLytWk();
      mLayoutBitFlagUpperLeft[i] = LAYOUT_BIT_FLAG_NONE;
    }
    if(mLayoutBitFlagUpperRight[i] & LAYOUT_BIT_FLAG_DRAW_SOME)
    {
      if(mpLayoutRenderingPipeLineUpperRight[i]) mpLayoutRenderingPipeLineUpperRight[i]->GetLayoutRenderPath()->ClearDrawableLytWk();
      mLayoutBitFlagUpperRight[i] = LAYOUT_BIT_FLAG_NONE;
    }
    if(mLayoutBitFlagLower[i] & LAYOUT_BIT_FLAG_DRAW_SOME)
    {
      if(mpLayoutRenderingPipeLineLower[i]) mpLayoutRenderingPipeLineLower[i]->GetLayoutRenderPath()->ClearDrawableLytWk();
      mLayoutBitFlagLower[i] = LAYOUT_BIT_FLAG_NONE;
    }
  }

  ////////////////////////////////////////////////////////////////
  // Addする。
  ////////////////////////////////////////////////////////////////
  if(renderPlace == RenderPlaceType::UPPER_DISPLAY)
  {
    if(layerNo < mLayoutLayerNumUpper)
    {
      mpLayoutRenderingPipeLineUpperLeft[layerNo]->GetLayoutRenderPath()->AddDrawableLytWk(pLytWk, gfl2::lyt::DISPLAY_UPPER, order);
      mLayoutBitFlagUpperLeft[layerNo] = mLayoutBitFlagUpperLeft[layerNo] | LAYOUT_BIT_FLAG_ADD_MINE;

      // 右目描画オフなら右目には積まない。
      if( isUpperDisplayModeStereo() )
      {
        if(mpLayoutRenderingPipeLineUpperRight[layerNo])
        {
          mpLayoutRenderingPipeLineUpperRight[layerNo]->GetLayoutRenderPath()->AddDrawableLytWk(pLytWk, gfl2::lyt::DISPLAY_UPPER_RIGHT, order);
          mLayoutBitFlagUpperRight[layerNo] = mLayoutBitFlagUpperRight[layerNo] | LAYOUT_BIT_FLAG_ADD_MINE;
        }
      }
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるASSERT
    }
  }
  else if(renderPlace == RenderPlaceType::LOWER_DISPLAY)
  {
    if(layerNo < mLayoutLayerNumLower)
    {
      mpLayoutRenderingPipeLineLower[layerNo]->GetLayoutRenderPath()->AddDrawableLytWk(pLytWk, gfl2::lyt::DISPLAY_LOWER, order);
      mLayoutBitFlagLower[layerNo] = mLayoutBitFlagLower[layerNo] | LAYOUT_BIT_FLAG_ADD_MINE;
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるASSERT
    }
  }
}


void AppRenderingManager::AddPokeModel(RenderPlaceType::Tag renderPlace, PokeTool::PokeModel* pPokeModel, u32 layerNo)
{
  if(renderPlace < RenderPlaceType::NUM)
  {
    if(layerNo < mPokeModelLayerNum[renderPlace])
    {
      mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetModelRenderPath()->AddNode(pPokeModel->GetModelInstanceNode());
      if(pPokeModel->GetShadowModel()) mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetModelRenderPath()->AddNode(pPokeModel->GetShadowModel()->GetModelInstanceNode());
      mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetEdgeMapPath()->AddEdgeRenderingTarget(pPokeModel->GetModelInstanceNode());

      if( pPokeModel->GetDistortionModel() )
      {
        if( mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetDistortionPath() )
        {
          mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetDistortionPath()->AddDistortionRenderingTarget( pPokeModel->GetDistortionModel()->GetModelInstanceNode() );
        }
        else
        {
          GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。ゆがみシェーダーが必要なのにつくっていない。
        }
      }
    
      if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo] )  // mDesc.isUpperDisplayStereo判定だけでなく、システムとして右目描画オフなら右目には積まないようにしたい。ここだけでなく他のところにも言えることだが。
      {
        mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetModelRenderPath()->AddNode(pPokeModel->GetModelInstanceNode());
        if(pPokeModel->GetShadowModel()) mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetModelRenderPath()->AddNode(pPokeModel->GetShadowModel()->GetModelInstanceNode());
        mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetEdgeMapPath()->AddEdgeRenderingTarget(pPokeModel->GetModelInstanceNode());

        if( pPokeModel->GetDistortionModel() )
        {
          if( mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetDistortionPath() )
          {
            mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetDistortionPath()->AddDistortionRenderingTarget( pPokeModel->GetDistortionModel()->GetModelInstanceNode() );
          }
          else
          {
            GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。ゆがみシェーダーが必要なのにつくっていない。
          }
        }
      }
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるASSERT
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるASSERT
  }
}
void AppRenderingManager::RemovePokeModel(RenderPlaceType::Tag renderPlace, PokeTool::PokeModel* pPokeModel, u32 layerNo)
{
  if(renderPlace < RenderPlaceType::NUM)
  {
    if(layerNo < mPokeModelLayerNum[renderPlace])
    {
      mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetModelRenderPath()->RemoveNode(pPokeModel->GetModelInstanceNode());
      if(pPokeModel->GetShadowModel()) mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetModelRenderPath()->RemoveNode(pPokeModel->GetShadowModel()->GetModelInstanceNode());
      mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetEdgeMapPath()->RemoveEdgeRenderingTarget(pPokeModel->GetModelInstanceNode());

      if( pPokeModel->GetDistortionModel() )
      {
        if( mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetDistortionPath() )
        {
          mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetDistortionPath()->RemoveDistortionRenderingTarget( pPokeModel->GetDistortionModel()->GetModelInstanceNode() );
        }
      }

      if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo] )
      {
        mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetModelRenderPath()->RemoveNode(pPokeModel->GetModelInstanceNode());
        if(pPokeModel->GetShadowModel()) mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetModelRenderPath()->RemoveNode(pPokeModel->GetShadowModel()->GetModelInstanceNode());
        mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetEdgeMapPath()->RemoveEdgeRenderingTarget(pPokeModel->GetModelInstanceNode());

        if( pPokeModel->GetDistortionModel() )
        {
          if( mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetDistortionPath() )
          {
            mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetDistortionPath()->AddDistortionRenderingTarget( pPokeModel->GetDistortionModel()->GetModelInstanceNode() );
          }
        }
      }
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるASSERT
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるASSERT
  }
}

void AppRenderingManager::AddModel(RenderPlaceType::Tag renderPlace, poke_3d::model::BaseModel* pModel, u32 layerNo, bool enableEdgeRenderPath)
{
  if(renderPlace < RenderPlaceType::NUM)
  {
    if(layerNo < mPokeModelLayerNum[renderPlace])
    {
      mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetModelRenderPath()->AddNode(pModel->GetModelInstanceNode());
      if( enableEdgeRenderPath )
      {
        mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetEdgeMapPath()->AddEdgeRenderingTarget(pModel->GetModelInstanceNode());
      }
    
      if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo] )
      {
        mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetModelRenderPath()->AddNode(pModel->GetModelInstanceNode());
        if( enableEdgeRenderPath )
        {
          mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetEdgeMapPath()->AddEdgeRenderingTarget(pModel->GetModelInstanceNode());
        }
      }
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるASSERT
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるASSERT
  }
}
void AppRenderingManager::RemoveModel(RenderPlaceType::Tag renderPlace, poke_3d::model::BaseModel* pModel, u32 layerNo)
{
  if(renderPlace < RenderPlaceType::NUM)
  {
    if(layerNo < mPokeModelLayerNum[renderPlace])
    {
      mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetModelRenderPath()->RemoveNode(pModel->GetModelInstanceNode());
      mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetEdgeMapPath()->RemoveEdgeRenderingTarget(pModel->GetModelInstanceNode());
    
      if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo] )
      {
        mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetModelRenderPath()->RemoveNode(pModel->GetModelInstanceNode());
        mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetEdgeMapPath()->RemoveEdgeRenderingTarget(pModel->GetModelInstanceNode());
      }
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるASSERT
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるASSERT
  }
}

void AppRenderingManager::AddCharaModel(RenderPlaceType::Tag renderPlace, poke_3d::model::CharaModel* pCharaModel, u32 layerNo)
{
  if(renderPlace < RenderPlaceType::NUM)
  {
    if(layerNo < mPokeModelLayerNum[renderPlace])
    {
      mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetModelRenderPath()->AddNode(pCharaModel->GetModelInstanceNode());
      mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetEdgeMapPath()->AddEdgeRenderingTarget(pCharaModel->GetModelInstanceNode());
    
      if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo] )
      {
        mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetModelRenderPath()->AddNode(pCharaModel->GetModelInstanceNode());
        mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetEdgeMapPath()->AddEdgeRenderingTarget(pCharaModel->GetModelInstanceNode());
      }
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるASSERT
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるASSERT
  }
}
void AppRenderingManager::RemoveCharaModel(RenderPlaceType::Tag renderPlace, poke_3d::model::CharaModel* pCharaModel, u32 layerNo)
{
  if(renderPlace < RenderPlaceType::NUM)
  {
    if(layerNo < mPokeModelLayerNum[renderPlace])
    {
      mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetModelRenderPath()->RemoveNode(pCharaModel->GetModelInstanceNode());
      mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetEdgeMapPath()->RemoveEdgeRenderingTarget(pCharaModel->GetModelInstanceNode());
    
      if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo] )
      {
        mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetModelRenderPath()->RemoveNode(pCharaModel->GetModelInstanceNode());
        mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetEdgeMapPath()->RemoveEdgeRenderingTarget(pCharaModel->GetModelInstanceNode());
      }
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるASSERT
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるASSERT
  }
}

void AppRenderingManager::AddDressUpModel(RenderPlaceType::Tag renderPlace, poke_3d::model::DressUpModel* pDressUpModel, u32 layerNo)
{
  if(renderPlace < RenderPlaceType::NUM)
  {
    if(layerNo < mPokeModelLayerNum[renderPlace])
    {
      this->pokeModelRenderingPipeLineSingleAddDressUpModel(mpPokeModelRenderingPipeLine[renderPlace][layerNo], pDressUpModel, renderPlace==RenderPlaceType::OFF_SCREEN);
    
      if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo] )
      {
        this->pokeModelRenderingPipeLineSingleAddDressUpModel(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo], pDressUpModel, renderPlace==RenderPlaceType::OFF_SCREEN);
      }
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるASSERT
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるASSERT
  }
}
void AppRenderingManager::RemoveDressUpModel(RenderPlaceType::Tag renderPlace, poke_3d::model::DressUpModel* pDressUpModel, u32 layerNo)
{
  if(renderPlace < RenderPlaceType::NUM)
  {
    if(layerNo < mPokeModelLayerNum[renderPlace])
    {
      this->pokeModelRenderingPipeLineSingleRemoveDressUpModel(mpPokeModelRenderingPipeLine[renderPlace][layerNo], pDressUpModel, renderPlace==RenderPlaceType::OFF_SCREEN);
    
      if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo] )
      {
        this->pokeModelRenderingPipeLineSingleRemoveDressUpModel(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo], pDressUpModel, renderPlace==RenderPlaceType::OFF_SCREEN);
      }
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるASSERT
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるASSERT
  }
}


#define APP_UTIL_APPRENDERINGMANAGER_CAMERA_FUNCTIONS(CameraName,CameraType)                                                                \
void AppRenderingManager::Set##CameraName(RenderPlaceType::Tag renderPlace, CameraType* pCamera, u32 layerNo)                               \
{                                                                                                                                           \
  if(pCamera)                                                                                                                               \
  {                                                                                                                                         \
    if(renderPlace < RenderPlaceType::NUM)                                                                                                  \
    {                                                                                                                                       \
      if(layerNo < mPokeModelLayerNum[renderPlace])                                                                                         \
      {                                                                                                                                     \
        mpPokeModelRenderingPipeLine[renderPlace][layerNo]->Set##CameraName(pCamera);                                                       \
                                                                                                                                            \
        if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo] )  \
        {                                                                                                                                   \
          mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->Set##CameraName(pCamera);                            \
        }                                                                                                                                   \
      }                                                                                                                                     \
      else                                                                                                                                  \
      {                                                                                                                                     \
        GFL_ASSERT(0);  /* 開発中に気付かせるASSERT */                                                                                      \
      }                                                                                                                                     \
    }                                                                                                                                       \
    else                                                                                                                                    \
    {                                                                                                                                       \
      GFL_ASSERT(0);  /* 開発中に気付かせるASSERT */                                                                                        \
    }                                                                                                                                       \
  }                                                                                                                                         \
}                                                                                                                                           \
void AppRenderingManager::Unset##CameraName(RenderPlaceType::Tag renderPlace, u32 layerNo)                                                  \
{                                                                                                                                           \
  if(renderPlace < RenderPlaceType::NUM)                                                                                                    \
  {                                                                                                                                         \
    if(layerNo < mPokeModelLayerNum[renderPlace])                                                                                           \
    {                                                                                                                                       \
      mpPokeModelRenderingPipeLine[renderPlace][layerNo]->Unset##CameraName();                                                              \
                                                                                                                                            \
      if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo] )    \
      {                                                                                                                                     \
        mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->Unset##CameraName();                                   \
      }                                                                                                                                     \
    }                                                                                                                                       \
    else                                                                                                                                    \
    {                                                                                                                                       \
      GFL_ASSERT(0);  /* 開発中に気付かせるASSERT */                                                                                        \
    }                                                                                                                                       \
  }                                                                                                                                         \
  else                                                                                                                                      \
  {                                                                                                                                         \
    GFL_ASSERT(0);  /* 開発中に気付かせるASSERT */                                                                                          \
  }                                                                                                                                         \
}                                                                                                                                           \
const CameraType* AppRenderingManager::Get##CameraName( RenderPlaceType::Tag renderPlace, u32 layerNo )                                     \
{                                                                                                                                           \
  if(renderPlace < RenderPlaceType::NUM)                                                                                                    \
  {                                                                                                                                         \
    if(layerNo < mPokeModelLayerNum[renderPlace])                                                                                           \
    {                                                                                                                                       \
      return mpPokeModelRenderingPipeLine[renderPlace][layerNo]->Get##CameraName();                                                         \
    }                                                                                                                                       \
    else                                                                                                                                    \
    {                                                                                                                                       \
      GFL_ASSERT(0);  /* 開発中に気付かせるASSERT */                                                                                        \
    }                                                                                                                                       \
  }                                                                                                                                         \
  else                                                                                                                                      \
  {                                                                                                                                         \
    GFL_ASSERT(0);  /* 開発中に気付かせるASSERT */                                                                                          \
  }                                                                                                                                         \
  return  NULL;                                                                                                                             \
}

APP_UTIL_APPRENDERINGMANAGER_CAMERA_FUNCTIONS(InFrameCamera, System::Camera::InFrameCamera)
APP_UTIL_APPRENDERINGMANAGER_CAMERA_FUNCTIONS(AppCamera, app::util::AppCamera)                      
APP_UTIL_APPRENDERINGMANAGER_CAMERA_FUNCTIONS(Camera, poke_3d::model::BaseCamera)                    

#undef APP_UTIL_APPRENDERINGMANAGER_CAMERA_FUNCTIONS


void AppRenderingManager::AddDrawEnv(RenderPlaceType::Tag renderPlace, gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode, u32 layerNo)
{
  gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( pDrawEnvNode );

  if(renderPlace < RenderPlaceType::NUM)
  {
    if(layerNo < mPokeModelLayerNum[renderPlace])
    {
      mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetModelRenderPath()->AddDrawEnv( pDrawEnvNode );
      
      if( mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetDistortionPath() )
      {
        mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetDistortionPath()->AddDrawEnv( pDrawEnvNode );
      }
    
      if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo] )
      {
        mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetModelRenderPath()->AddDrawEnv( pDrawEnvNode );

        if( mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetDistortionPath() )
        {
          mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetDistortionPath()->AddDrawEnv( pDrawEnvNode );
        }
      }
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるASSERT
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるASSERT
  }
}
void AppRenderingManager::RemoveDrawEnv(RenderPlaceType::Tag renderPlace, gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode, u32 layerNo)
{
  gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild( pDrawEnvNode );

  if(renderPlace < RenderPlaceType::NUM)
  {
    if(layerNo < mPokeModelLayerNum[renderPlace])
    {
      mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetModelRenderPath()->RemoveDrawEnv( pDrawEnvNode );
   
      if( mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetDistortionPath() )
      {
        mpPokeModelRenderingPipeLine[renderPlace][layerNo]->GetDistortionPath()->RemoveDrawEnv( pDrawEnvNode );
      }

      if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo] )
      {
        mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetModelRenderPath()->RemoveDrawEnv( pDrawEnvNode );

        if( mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetDistortionPath() )
        {
          mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->GetDistortionPath()->RemoveDrawEnv( pDrawEnvNode );
        }
      }
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるASSERT
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるASSERT
  }
}

void AppRenderingManager::SetPokeModelSystem(RenderPlaceType::Tag renderPlace, PokeTool::PokeModelSystem* pokeModelSys, u32 layerNo)
{
  if(pokeModelSys)                                                                                                                          
  {                                                                                                                                         
    if(renderPlace < RenderPlaceType::NUM)                                                                                                  
    {                                                                                                                                       
      if(layerNo < mPokeModelLayerNum[renderPlace])                                                                                         
      {                                                                                                                                     
        mpPokeModelRenderingPipeLine[renderPlace][layerNo]->SetPokeModelSystem(pokeModelSys);                                               
                                                                                                                                            
        if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo] )  
        {                                                                                                                                   
          mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->SetPokeModelSystem(pokeModelSys);                    
        }                                                                                                                                   
      }                                                                                                                                     
      else                                                                                                                                  
      {                                                                                                                                     
        GFL_ASSERT(0);  /* 開発中に気付かせるASSERT */                                                                                      
      }                                                                                                                                     
    }                                                                                                                                       
    else                                                                                                                                    
    {                                                                                                                                       
      GFL_ASSERT(0);  /* 開発中に気付かせるASSERT */                                                                                        
    }                                                                                                                                       
  }                                                                                                                                         
}
void AppRenderingManager::UnsetPokeModelSystem(RenderPlaceType::Tag renderPlace, u32 layerNo)
{
  if(renderPlace < RenderPlaceType::NUM)                                                                                                    
  {                                                                                                                                         
    if(layerNo < mPokeModelLayerNum[renderPlace])                                                                                           
    {                                                                                                                                       
      mpPokeModelRenderingPipeLine[renderPlace][layerNo]->UnsetPokeModelSystem();                                                           
                                                                                                                                            
      if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo] )    
      {                                                                                                                                     
        mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->UnsetPokeModelSystem();                                
      }                                                                                                                                     
    }                                                                                                                                       
    else                                                                                                                                    
    {                                                                                                                                       
      GFL_ASSERT(0);  /* 開発中に気付かせるASSERT */                                                                                        
    }                                                                                                                                       
  }                                                                                                                                         
  else                                                                                                                                      
  {                                                                                                                                         
    GFL_ASSERT(0);  /* 開発中に気付かせるASSERT */                                                                                          
  } 
}

void AppRenderingManager::SetRaderChart(RenderPlaceType::Tag renderPlace, app::tool::RaderChart* pRaderChart)
{
  if(pRaderChart)
  {
    if(renderPlace == RenderPlaceType::UPPER_DISPLAY)
    {
      mpRaderChartRenderingPipeLineUpperLeft->GetRaderChartRenderPath()->SetRaderChart(pRaderChart);
      if(mpRaderChartRenderingPipeLineUpperRight) mpRaderChartRenderingPipeLineUpperRight->GetRaderChartRenderPath()->SetRaderChart(pRaderChart);
    }
    else if(renderPlace == RenderPlaceType::LOWER_DISPLAY)
    {
      mpRaderChartRenderingPipeLineLower->GetRaderChartRenderPath()->SetRaderChart(pRaderChart);
    }
  }
  else
  {
    GFL_ASSERT(0);  // NULLのセットはUnset...関数で。開発中に気付かせるためのASSERT
  }
}
void AppRenderingManager::UnsetRaderChart(RenderPlaceType::Tag renderPlace)
{
  if(renderPlace == RenderPlaceType::UPPER_DISPLAY)
  {
    mpRaderChartRenderingPipeLineUpperLeft->GetRaderChartRenderPath()->UnsetRaderChart();
    if(mpRaderChartRenderingPipeLineUpperRight) mpRaderChartRenderingPipeLineUpperRight->GetRaderChartRenderPath()->UnsetRaderChart();
  }
  else if(renderPlace == RenderPlaceType::LOWER_DISPLAY)
  {
    mpRaderChartRenderingPipeLineLower->GetRaderChartRenderPath()->UnsetRaderChart();
  }
}

void AppRenderingManager::SetEffectRenderPathConfig(RenderPlaceType::Tag renderPlace, const gfl2::Effect::EffectRenderPath::Config& effectRenderPathConfig, u32 layerNo)
{
  if(renderPlace < RenderPlaceType::NUM)
  {
    if(layerNo < mEffectLayerNum[renderPlace])
    {
      mpEffectRenderingPipeLine[renderPlace][layerNo]->SetEffectRenderPathConfig( effectRenderPathConfig );
    
      if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpEffectRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo] )
      {
        mpEffectRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->SetEffectRenderPathConfig( effectRenderPathConfig  );
      }
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるASSERT
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるASSERT
  }
}
void AppRenderingManager::UnsetEffectRenderPathConfig(RenderPlaceType::Tag renderPlace, u32 layerNo)
{
  if(renderPlace < RenderPlaceType::NUM)
  {
    if(layerNo < mEffectLayerNum[renderPlace])
    {
      mpEffectRenderingPipeLine[renderPlace][layerNo]->UnsetEffectRenderPathConfig();
    
      if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpEffectRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo] )
      {
        mpEffectRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->UnsetEffectRenderPathConfig();
      }
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるASSERT
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるASSERT
  }
}


#define APP_UTIL_APPRENDERINGMANAGER_EFFECT_CAMERA_FUNCTIONS(CameraName,CameraType)                                                      \
void AppRenderingManager::SetEffect##CameraName(RenderPlaceType::Tag renderPlace, CameraType* pCamera, u32 layerNo)                      \
{                                                                                                                                        \
  if(pCamera)                                                                                                                            \
  {                                                                                                                                      \
    if(renderPlace < RenderPlaceType::NUM)                                                                                               \
    {                                                                                                                                    \
      if(layerNo < mEffectLayerNum[renderPlace])                                                                                         \
      {                                                                                                                                  \
        mpEffectRenderingPipeLine[renderPlace][layerNo]->Set##CameraName(pCamera);                                                       \
                                                                                                                                         \
        if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpEffectRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo] )  \
        {                                                                                                                                \
          mpEffectRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->Set##CameraName(pCamera);                            \
        }                                                                                                                                \
      }                                                                                                                                  \
      else                                                                                                                               \
      {                                                                                                                                  \
        GFL_ASSERT(0);  /* 開発中に気付かせるASSERT */                                                                                   \
      }                                                                                                                                  \
    }                                                                                                                                    \
    else                                                                                                                                 \
    {                                                                                                                                    \
      GFL_ASSERT(0);  /* 開発中に気付かせるASSERT */                                                                                     \
    }                                                                                                                                    \
  }                                                                                                                                      \
}                                                                                                                                        \
void AppRenderingManager::UnsetEffect##CameraName(RenderPlaceType::Tag renderPlace, u32 layerNo)                                         \
{                                                                                                                                        \
  if(renderPlace < RenderPlaceType::NUM)                                                                                                 \
  {                                                                                                                                      \
    if(layerNo < mEffectLayerNum[renderPlace])                                                                                           \
    {                                                                                                                                    \
      mpEffectRenderingPipeLine[renderPlace][layerNo]->Unset##CameraName();                                                              \
                                                                                                                                         \
      if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpEffectRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo] )    \
      {                                                                                                                                  \
        mpEffectRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][layerNo]->Unset##CameraName();                                   \
      }                                                                                                                                  \
    }                                                                                                                                    \
    else                                                                                                                                 \
    {                                                                                                                                    \
      GFL_ASSERT(0);  /* 開発中に気付かせるASSERT */                                                                                     \
    }                                                                                                                                    \
  }                                                                                                                                      \
  else                                                                                                                                   \
  {                                                                                                                                      \
    GFL_ASSERT(0);  /* 開発中に気付かせるASSERT */                                                                                       \
  }                                                                                                                                      \
}                                                                                                                                        \
const CameraType* AppRenderingManager::GetEffect##CameraName( RenderPlaceType::Tag renderPlace, u32 layerNo )                            \
{                                                                                                                                        \
  if(renderPlace < RenderPlaceType::NUM)                                                                                                 \
  {                                                                                                                                      \
    if(layerNo < mEffectLayerNum[renderPlace])                                                                                           \
    {                                                                                                                                    \
      return mpEffectRenderingPipeLine[renderPlace][layerNo]->Get##CameraName();                                                         \
    }                                                                                                                                    \
    else                                                                                                                                 \
    {                                                                                                                                    \
      GFL_ASSERT(0);  /* 開発中に気付かせるASSERT */                                                                                     \
    }                                                                                                                                    \
  }                                                                                                                                      \
  else                                                                                                                                   \
  {                                                                                                                                      \
    GFL_ASSERT(0);  /* 開発中に気付かせるASSERT */                                                                                       \
  }                                                                                                                                      \
  return  NULL;                                                                                                                          \
}                                                                                                                                        
                                                                                                                                         
APP_UTIL_APPRENDERINGMANAGER_EFFECT_CAMERA_FUNCTIONS(AppCamera, app::util::AppCamera)                                                              
APP_UTIL_APPRENDERINGMANAGER_EFFECT_CAMERA_FUNCTIONS(Camera, poke_3d::model::BaseCamera)

#undef APP_UTIL_APPRENDERINGMANAGER_EFFECT_CAMERA


void AppRenderingManager::CreateDefaultCamera(app::util::Heap* pHeap,
                                              gfl2::renderingengine::scenegraph::instance::DrawEnvNode** pDrawEnvNodeOfCamera,
                                              poke_3d::model::BaseCamera** pCamera, RenderPlaceType::Tag renderPlace)
{
  // DrawEnvNode
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* drawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(pHeap->GetDevGLHeapAllocator());

  // CameraNode
  {
    gfl2::renderingengine::scenegraph::instance::CameraNode* cameraNode;
    cameraNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateCameraNode(pHeap->GetDevGLHeapAllocator());
    cameraNode->SetNodeName( "Camera" );
    // 追加
    drawEnvNode->AddCamera( cameraNode );  // 追加しておけば親のdrawEnvNodeが削除されるときに一緒に削除される
  }

  // BaseCamera
  poke_3d::model::BaseCamera* baseCamera = GFL_NEW(pHeap->GetDeviceHeap()) poke_3d::model::BaseCamera;
  {
    baseCamera->Create(pHeap->GetDevGLHeapAllocator(), pHeap->GetDeviceHeap(), drawEnvNode, 0);
    baseCamera->SetNear( 1.0f );
    baseCamera->SetFar( 12000.0f );
    baseCamera->SetFovy( gfl2::math::ConvDegToRad(30.0f) );  // 30.0f度

    if(renderPlace==RenderPlaceType::UPPER_DISPLAY){
      baseCamera->SetAspectRatio( 400.0f/240.0f );
    }
    else{
      baseCamera->SetAspectRatio( 320.0f/240.0f );
    }
    baseCamera->SetPosition( gfl2::math::Vector3(0,0,1000) );
  }

  *pDrawEnvNodeOfCamera = drawEnvNode;
  *pCamera              = baseCamera;
}

gfl2::renderingengine::scenegraph::instance::DrawEnvNode* AppRenderingManager::CreateDefaultDrawEnv(app::util::Heap* pHeap, PresetType::Tag presetType, u32 a_lightSetNo)
{
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* drawEnvNode = NULL;

  switch(presetType)
  {
  case PresetType::DEFAULT:
    {
      u32 lightSetNo = 0;
      if(a_lightSetNo != DEFAULT_LIGHT_SET_NO)
      {
        lightSetNo = a_lightSetNo;
      }

      // DrawEnvNode 
      drawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(pHeap->GetDevGLHeapAllocator()); 
   
      gfl2::math::AABB envSpace(gfl2::math::Vector4(-500000.0f, -500000.0f, -500000.0f), gfl2::math::Vector4(500000.0f, 500000.0f, 500000.0f)); 
      drawEnvNode->SetAABB(envSpace); 
   
      // ライト 
      { 
        // ライトセット 
        gfl2::renderingengine::scenegraph::instance::LightSetNode* lightSetNode; 
        lightSetNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(pHeap->GetDevGLHeapAllocator()); 
        lightSetNode->SetSetNo( lightSetNo );  // ライトセット番号はAddLightSetしたときのものから変更できないので注意すること。
   
        // ライト 
        gfl2::renderingengine::scenegraph::instance::LightNode* directionalLightNode; 
        directionalLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(pHeap->GetDevGLHeapAllocator()); 
        directionalLightNode->SetNodeName( "DirectionalLight" ); 
        gfl2::util::FnvHash nameHash( reinterpret_cast<const u8*>(directionalLightNode->GetNodeName()) ); 
        directionalLightNode->SetNodeNameHash( nameHash.value ); 
        directionalLightNode->SetLightType( gfl2::renderingengine::LightType::Directional ); 
        gfl2::math::Vector3 pos = gfl2::math::Vector3(-10000000.0f, 10000000.0f, 10000000.0f); 
        directionalLightNode->SetPosition( pos ); 
        gfl2::math::Vector3 direction = -pos.Normalize(); 
        directionalLightNode->SetDirection( direction ); 
        directionalLightNode->SetColor( gfl2::math::Vector3( 0.7f, 0.7f, 0.7f ) ); 
   
        // 追加 
        lightSetNode->AddLight( directionalLightNode ); 
        drawEnvNode->AddLightSet( lightSetNode ); 
      } 
      break;
    }
  case PresetType::POKEMON: GFL_ASSERT(0); break;  // まだつくっていない
  case PresetType::TRAINER:
    {
      static const u32 TRAINER_LIGHT_SET_NO = 2;
      
      u32 lightSetNo = TRAINER_LIGHT_SET_NO;
      if(a_lightSetNo != DEFAULT_LIGHT_SET_NO)
      {
        lightSetNo = a_lightSetNo;
      }

      // DrawEnvNode 
      drawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(pHeap->GetDevGLHeapAllocator()); 
   
      gfl2::math::AABB envSpace(gfl2::math::Vector4(-500000.0f, -500000.0f, -500000.0f), gfl2::math::Vector4(500000.0f, 500000.0f, 500000.0f)); 
      drawEnvNode->SetAABB(envSpace); 
   
      // ライト 
      { 
        // ライトセット 
        gfl2::renderingengine::scenegraph::instance::LightSetNode* lightSetNode; 
        {
          lightSetNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(pHeap->GetDevGLHeapAllocator()); 
          lightSetNode->SetSetNo( lightSetNo ); 
        }

        // ライト 
        gfl2::renderingengine::scenegraph::instance::LightNode* directionalLightNode; 
        {
          directionalLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(pHeap->GetDevGLHeapAllocator()); 
          directionalLightNode->SetNodeName( "DirectionalLight" ); 
          gfl2::util::FnvHash nameHash( reinterpret_cast<const u8*>(directionalLightNode->GetNodeName()) ); 
          directionalLightNode->SetNodeNameHash( nameHash.value ); 
          directionalLightNode->SetLightType( gfl2::renderingengine::LightType::Directional ); 
          gfl2::math::Vector3 pos = gfl2::math::Vector3(0.0f, 0.0f, 0.0f); 
          directionalLightNode->SetPosition( pos ); 
          gfl2::math::Vector3 direction = gfl2::math::Vector3(-0.466435f, -0.171028f, -0.867834f); 
          directionalLightNode->SetDirection( direction ); 
          directionalLightNode->SetColor( gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ) ); 
        }
        
        // ライト 
        gfl2::renderingengine::scenegraph::instance::LightNode* ambientLightNode; 
        {
          ambientLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(pHeap->GetDevGLHeapAllocator()); 
          ambientLightNode->SetNodeName( "AmbientLight" ); 
          gfl2::util::FnvHash nameHash( reinterpret_cast<const u8*>(ambientLightNode->GetNodeName()) ); 
          ambientLightNode->SetNodeNameHash( nameHash.value ); 
          ambientLightNode->SetLightType( gfl2::renderingengine::LightType::Ambient ); 
          ambientLightNode->SetColor( gfl2::math::Vector3( 0.2f, 0.2f, 0.2f ) ); 
        } 

        // 追加 
        lightSetNode->AddLight( directionalLightNode ); 
        lightSetNode->AddLight( ambientLightNode ); 
        drawEnvNode->AddLightSet( lightSetNode ); 
      } 

      break;
    }
  }

  return drawEnvNode; 
}

void AppRenderingManager::Update(void)
{
  if(mCreateState == CREATE_STATE_CREATING)
  {
    this->continueAsyncCreate();
  }

  if( mpPokeModelRendererBuilder )
  {
    mpPokeModelRendererBuilder->Update();
  }

  for(s32 renderPlace=0; renderPlace<RenderPlaceType::NUM; ++renderPlace)
  {
    for(u32 i=0; i<mPokeModelLayerNum[renderPlace]; ++i)
    {
      if(mpPokeModelRenderingPipeLine[renderPlace][i])  mpPokeModelRenderingPipeLine[renderPlace][i]->Update();
      if( renderPlace == RenderPlaceType::UPPER_DISPLAY && mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][i] )
      {
        if(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][i])  mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][i]->Update();
      }
    }
  }

  if(mpRaderChartRenderingPipeLineUpperLeft)  mpRaderChartRenderingPipeLineUpperLeft->Update();
  if(mpRaderChartRenderingPipeLineUpperRight) mpRaderChartRenderingPipeLineUpperRight->Update();
  if(mpRaderChartRenderingPipeLineLower)      mpRaderChartRenderingPipeLineLower->Update();

  if(
       isThereGfxObject(mDesc.renderDesc[RenderPlaceType::UPPER_DISPLAY].renderOrder)
    || isThereGfxObject(mDesc.renderDesc[RenderPlaceType::LOWER_DISPLAY].renderOrder)
    || isThereGfxObject(mDesc.renderDesc[RenderPlaceType::OFF_SCREEN].renderOrder)
  )
  {
    if(mDesc.callGlobalTraverseModel)
    {
      gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();
    }
  }
}

void AppRenderingManager::PreDraw(void)
{
  // RenderPlaceType::OFF_SCREEN
  switch(mDesc.renderDesc[RenderPlaceType::OFF_SCREEN].renderOrder)
  {
  case RenderOrderType::POKEMODEL: this->drawOnOffScreen_PokeModel(RenderPlaceType::OFF_SCREEN); break;  // 今対応しているのはオフスクリーン0でのポケモンモデル描画だけ。
  }
}

void AppRenderingManager::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  ////////////////////////////////////////////////////////////////
  // Drawする。
  ////////////////////////////////////////////////////////////////
  RenderOrderType::Tag order = RenderOrderType::NONE;
  if( displayNo == gfl2::gfx::CtrDisplayNo::LEFT || displayNo == gfl2::gfx::CtrDisplayNo::RIGHT )
  {
    order = mDesc.renderDesc[RenderPlaceType::UPPER_DISPLAY].renderOrder;
  }
  else if( displayNo == gfl2::gfx::CtrDisplayNo::DOWN )
  {
    order = mDesc.renderDesc[RenderPlaceType::LOWER_DISPLAY].renderOrder;
  }

  switch(order)
  {
  case RenderOrderType::LAYOUT:                                             this->draw_Layout(displayNo);                                             break;
  case RenderOrderType::POKEMODEL:                                          this->draw_PokeModel(displayNo);                                          break;
  case RenderOrderType::LAYOUT_POKEMODEL:                                   this->draw_Layout_PokeModel(displayNo);                                   break;
  case RenderOrderType::LAYOUT_RADERCHART:                                  this->draw_Layout_RaderChart(displayNo);                                  break;
  case RenderOrderType::LAYOUT_RADERCHART_POKEMODEL:                        this->draw_Layout_RaderChart_PokeModel(displayNo);                        break;
  case RenderOrderType::LAYOUT_RADERCHART_POKEMODEL0_POKEMODEL1:            this->draw_Layout_RaderChart_PokeModel0_PokeModel1(displayNo);            break;
  case RenderOrderType::LAYOUT_RADERCHART_POKEMODEL0_POKEMODEL1_POKEMODEL2: this->draw_Layout_RaderChart_PokeModel0_PokeModel1_PokeModel2(displayNo); break;
  case RenderOrderType::LAYOUT_EFFECT:                                      this->draw_Layout_Effect(displayNo);                                      break;
  case RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1:                          this->draw_Layout0_PokeModel_Layout1(displayNo);                          break;
  case RenderOrderType::POKEMODEL_LAYOUT:                                   this->draw_PokeModel_Layout(displayNo);                                   break;
  case RenderOrderType::POKEMODEL0_LAYOUT_RADERCHART_POKEMODEL1:            this->draw_PokeModel0_Layout_RaderChart_PokeModel1(displayNo);            break;
  case RenderOrderType::POKEMODEL_EFFECT_LAYOUT:                            this->draw_PokeModel_Effect_Layout(displayNo);                            break;
  case RenderOrderType::POKEMODEL0_POKEMODEL1_POKEMODEL2_EFFECT_LAYOUT:     this->draw_PokeModel0_PokeModel1_PokeModel2_Effect_Layout(displayNo);     break;
  case RenderOrderType::POKEMODEL0_LAYOUT_POKEMODEL1:                       this->draw_PokeModel0_Layout_PokeModel1(displayNo);                       break;
  case RenderOrderType::POKEMODEL0_EFFECT_LAYOUT_POKEMODEL1:                this->draw_PokeModel0_Effect_Layout_PokeModel1(displayNo);                break;
  case RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1_LAYOUT2:                  this->draw_Layout0_PokeModel_Layout1_Layout2(displayNo);                  break;
  case RenderOrderType::LAYOUT0_LAYOUT1:                                    this->draw_Layout0_Layout1(displayNo);                                    break;
  case RenderOrderType::LAYOUT0_LAYOUT1_LAYOUT2:                            this->draw_Layout0_Layout1_Layout2(displayNo);                            break;
  case RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1_EFFECT:                   this->draw_Layout0_PokeModel_Layout1_Effect(displayNo);                   break;
  case RenderOrderType::POKEMODEL0_POKEMODEL1_POKEMODEL2_LAYOUT:            this->draw_PokeModel0_PokeModel1_PokeModel2_Layout(displayNo);            break;
  }

  ////////////////////////////////////////////////////////////////
  // ポーズのときはDrawだけがまわるので描画後にクリアすることにした。
  ////////////////////////////////////////////////////////////////
  for(u32 i=0; i<LAYOUT_LAYER_NUM_MAX; ++i)
  {
    if(mLayoutBitFlagUpperLeft[i] & LAYOUT_BIT_FLAG_ADD_MINE)
    { 
      mLayoutBitFlagUpperLeft[i] |= LAYOUT_BIT_FLAG_DRAW_SOME;
    }
    if(mLayoutBitFlagUpperRight[i] & LAYOUT_BIT_FLAG_ADD_MINE)
    {
      mLayoutBitFlagUpperRight[i] |= LAYOUT_BIT_FLAG_DRAW_SOME;
    }
    if(mLayoutBitFlagLower[i] & LAYOUT_BIT_FLAG_ADD_MINE)
    {
      mLayoutBitFlagLower[i] |= LAYOUT_BIT_FLAG_DRAW_SOME;
    }
  }
  
  if( displayNo == gfl2::gfx::CtrDisplayNo::LEFT )
  {
    for(u32 i=0; i<LAYOUT_LAYER_NUM_MAX; ++i)
    {
      if(mpLayoutRenderingPipeLineUpperLeft[i]) mpLayoutRenderingPipeLineUpperLeft[i]->GetLayoutRenderPath()->ClearDrawableLytWk();
      mLayoutBitFlagUpperLeft[i] = LAYOUT_BIT_FLAG_NONE;
    }

    // 右目描画がないときは、左目のときに右目の分もクリアしておく。
    if( !isUpperDisplayModeStereo() )
    {
      for(u32 i=0; i<LAYOUT_LAYER_NUM_MAX; ++i)
      {
        if(mpLayoutRenderingPipeLineUpperRight[i]) mpLayoutRenderingPipeLineUpperRight[i]->GetLayoutRenderPath()->ClearDrawableLytWk();
        mLayoutBitFlagUpperRight[i] = LAYOUT_BIT_FLAG_NONE;
      }
    }
  }
  else if( displayNo == gfl2::gfx::CtrDisplayNo::RIGHT )
  {
    for(u32 i=0; i<LAYOUT_LAYER_NUM_MAX; ++i)
    {
      if(mpLayoutRenderingPipeLineUpperRight[i]) mpLayoutRenderingPipeLineUpperRight[i]->GetLayoutRenderPath()->ClearDrawableLytWk();
      mLayoutBitFlagUpperRight[i] = LAYOUT_BIT_FLAG_NONE;
    }
  }
  else if( displayNo == gfl2::gfx::CtrDisplayNo::DOWN )
  {
    for(u32 i=0; i<LAYOUT_LAYER_NUM_MAX; ++i)
    {
      if(mpLayoutRenderingPipeLineLower[i]) mpLayoutRenderingPipeLineLower[i]->GetLayoutRenderPath()->ClearDrawableLytWk();
      mLayoutBitFlagLower[i] = LAYOUT_BIT_FLAG_NONE;
    }
  }

  this->transferColorBuffer( displayNo );
}

void AppRenderingManager::draw_Layout(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  }
}
void AppRenderingManager::draw_PokeModel(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
    }
    break;
  }
}
void AppRenderingManager::draw_Layout_PokeModel(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
    }
    break;
  }
}
void AppRenderingManager::draw_Layout_RaderChart(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
      this->raderChartRenderingPipeLineSingleExecute(mpRaderChartRenderingPipeLineUpperLeft, displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
      this->raderChartRenderingPipeLineSingleExecute(mpRaderChartRenderingPipeLineUpperRight, displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
      this->raderChartRenderingPipeLineSingleExecute(mpRaderChartRenderingPipeLineLower, displayNo);
    }
    break;
  }
}
void AppRenderingManager::draw_Layout_RaderChart_PokeModel(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
      this->raderChartRenderingPipeLineSingleExecute(mpRaderChartRenderingPipeLineUpperLeft, displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
      this->raderChartRenderingPipeLineSingleExecute(mpRaderChartRenderingPipeLineUpperRight, displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
      this->raderChartRenderingPipeLineSingleExecute(mpRaderChartRenderingPipeLineLower, displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
    }
    break;
  }
}
void AppRenderingManager::draw_Layout_RaderChart_PokeModel0_PokeModel1(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
      this->raderChartRenderingPipeLineSingleExecute(mpRaderChartRenderingPipeLineUpperLeft, displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][1], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
      this->raderChartRenderingPipeLineSingleExecute(mpRaderChartRenderingPipeLineUpperRight, displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][1], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
      this->raderChartRenderingPipeLineSingleExecute(mpRaderChartRenderingPipeLineLower, displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][1], displayNo);
    }
    break;
  }
}
void AppRenderingManager::draw_Layout_RaderChart_PokeModel0_PokeModel1_PokeModel2(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
      this->raderChartRenderingPipeLineSingleExecute(mpRaderChartRenderingPipeLineUpperLeft, displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][1], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][2], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
      this->raderChartRenderingPipeLineSingleExecute(mpRaderChartRenderingPipeLineUpperRight, displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][1], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][2], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
      this->raderChartRenderingPipeLineSingleExecute(mpRaderChartRenderingPipeLineLower, displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][1], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][2], displayNo);
    }
    break;
  }
}

void AppRenderingManager::draw_Layout_Effect(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
      this->effectRenderingPipeLineSingleExecute(mpEffectRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
      this->effectRenderingPipeLineSingleExecute(mpEffectRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
      this->effectRenderingPipeLineSingleExecute(mpEffectRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
    }
    break;
  }
}

void AppRenderingManager::draw_Layout0_PokeModel_Layout1(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[1], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[1], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[1], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  }
}
void AppRenderingManager::draw_PokeModel_Layout(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  }
}
void AppRenderingManager::draw_PokeModel0_Layout_RaderChart_PokeModel1(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
      this->raderChartRenderingPipeLineSingleExecute(mpRaderChartRenderingPipeLineUpperLeft, displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][1], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
      this->raderChartRenderingPipeLineSingleExecute(mpRaderChartRenderingPipeLineUpperRight, displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][1], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
      this->raderChartRenderingPipeLineSingleExecute(mpRaderChartRenderingPipeLineLower, displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][1], displayNo);
    }
    break;
  }
}
void AppRenderingManager::draw_PokeModel_Effect_Layout(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
      this->effectRenderingPipeLineSingleExecute(mpEffectRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
      this->effectRenderingPipeLineSingleExecute(mpEffectRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
      this->effectRenderingPipeLineSingleExecute(mpEffectRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  }
}
void AppRenderingManager::draw_PokeModel0_Layout_PokeModel1(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][1], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][1], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][1], displayNo);
    }
    break;
  }
}
void AppRenderingManager::draw_PokeModel0_Effect_Layout_PokeModel1(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
      this->effectRenderingPipeLineSingleExecute(mpEffectRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][1], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
      this->effectRenderingPipeLineSingleExecute(mpEffectRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][1], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
      this->effectRenderingPipeLineSingleExecute(mpEffectRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][1], displayNo);
    }
    break;
  }
}
void AppRenderingManager::draw_PokeModel0_PokeModel1_PokeModel2_Effect_Layout(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][1], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][2], displayNo);
      this->effectRenderingPipeLineSingleExecute(mpEffectRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][1], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][2], displayNo);
      this->effectRenderingPipeLineSingleExecute(mpEffectRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][1], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][2], displayNo);
      this->effectRenderingPipeLineSingleExecute(mpEffectRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  }
}
void AppRenderingManager::draw_Layout0_PokeModel_Layout1_Layout2(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[1], System::GflUse::m_HandleList[displayNo]);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[2], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[1], System::GflUse::m_HandleList[displayNo]);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[2], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[1], System::GflUse::m_HandleList[displayNo]);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[2], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  }
}
void AppRenderingManager::draw_Layout0_Layout1(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[1], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[1], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[1], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  }
}
void AppRenderingManager::draw_Layout0_Layout1_Layout2(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[1], System::GflUse::m_HandleList[displayNo]);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[2], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[1], System::GflUse::m_HandleList[displayNo]);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[2], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[1], System::GflUse::m_HandleList[displayNo]);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[2], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  }
}

void AppRenderingManager::draw_Layout0_PokeModel_Layout1_Effect(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[1], System::GflUse::m_HandleList[displayNo]);
      this->effectRenderingPipeLineSingleExecute(mpEffectRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[1], System::GflUse::m_HandleList[displayNo]);
      this->effectRenderingPipeLineSingleExecute(mpEffectRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[1], System::GflUse::m_HandleList[displayNo]);
      this->effectRenderingPipeLineSingleExecute(mpEffectRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
    }
    break;
  }
}

void AppRenderingManager::draw_PokeModel0_PokeModel1_PokeModel2_Layout(gfl2::gfx::CtrDisplayNo displayNo)
{
  switch( displayNo )
  {
  case gfl2::gfx::CtrDisplayNo::LEFT:       // 左画面描画
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][0], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][1], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_DISPLAY][2], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperLeft[0], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::RIGHT:      // 右画面描画(立体視時のみ)
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][0], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][1], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::UPPER_RIGHT_DISPLAY][2], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineUpperRight[0], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  case gfl2::gfx::CtrDisplayNo::DOWN:       // 下画面描画
    {
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][0], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][1], displayNo);
      this->pokeModelRenderingPipeLineSingleExecute(mpPokeModelRenderingPipeLine[RenderPlaceType::LOWER_DISPLAY][2], displayNo);
      this->layoutRenderingPipeLineSingleExecute(mpLayoutRenderingPipeLineLower[0], System::GflUse::m_HandleList[displayNo]);
    }
    break;
  }
}

void AppRenderingManager::drawOnOffScreen_PokeModel(RenderPlaceType::Tag renderPlace)
{
  app_rendering_manager_internal::PokeModelRenderingPipeLine* pPokeModelRenderingPipeLine = NULL;
  gfl2::gfx::Surface*                                         pOffScreenRenderTarget      = NULL;
  gfl2::gfx::Surface*                                         pOffScreenDepthBuffer       = NULL;
  gfl2::gfx::Color                                            clearColor(0,0,0,0);

  if(renderPlace == RenderPlaceType::OFF_SCREEN)
  {
    pPokeModelRenderingPipeLine = mpPokeModelRenderingPipeLine[RenderPlaceType::OFF_SCREEN][0];

    //  NULLになるケースがあるのでチェック追加
    if( mpOffScreenRenderTargetTexture != NULL )
    {
      pOffScreenRenderTarget      = mpOffScreenRenderTargetTexture->GetColorSurface();
      pOffScreenDepthBuffer       = mpOffScreenRenderTargetTexture->GetDepthSurface();
      clearColor                  = mpOffScreenRenderTargetTexture->GetClearColor();

      // @fix GFNMCat[3636] アプリに出るポケモンモデル：CTR版でＩＤエッジが表示されていない
      // オフスクリーンのときは外部からデプスステンシルテクスチャをセット/アンセットする。
      if( pPokeModelRenderingPipeLine->GetOutLinePathOuterEdgeMapTexture() == NULL )
      {
        if( mpOffScreenRenderTargetTexture->GetDepthTexture() )
        {
          //pPokeModelRenderingPipeLine->SetOutLinePathOuterEdgeMapTexture( mpOffScreenRenderTargetTexture->GetDepthTexture() );  // @attention オフスクリーンのときうまく描画されないので今はコメントアウト。
                                                                                                                                  //            nijiで必須でないのでこのままコメントアウトしておく。
              // うまく描画されない事例
              //     IDエッジを使用しているポケモンモデル（シズクモとユニランでうまく出すための値が違っている）←コメントアウト部分を有効にし、それぞれ適切な値にしてやればうまく出る。
              //     IDエッジを使用していないバトル着せ替えモデル←コメントアウトのままにしておき無効にしておけばうまく出る。→だからコメントアウトしている。
        }
      }
    }
  }

  if(pPokeModelRenderingPipeLine && pOffScreenRenderTarget)  // pOffScreenDepthBufferはNULLでも見逃す
  {
    gfl2::gfx::DisplayDeviceHandle handle = System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT];  // ないバッファはここから借りる
 
    gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( handle );          // 元に戻すときにだけ使う
    gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( handle );  // 元に戻すときにだけ使う
    u32                 displayWidth  = pOffScreenRenderTarget->GetSurfaceDesc().width;
    u32                 displayHeight = pOffScreenRenderTarget->GetSurfaceDesc().height;

    {
      bool setMtxFlag = false;
      gfl2::math::Matrix44 projMtx;
      gfl2::math::Matrix34 viewMtx;
      
      if(pPokeModelRenderingPipeLine->GetInFrameCamera())
      {
        projMtx = pPokeModelRenderingPipeLine->GetInFrameCamera()->GetProjectionMatrix();
        viewMtx = pPokeModelRenderingPipeLine->GetInFrameCamera()->GetViewMatrix();
        setMtxFlag = true;
      }
      else if(pPokeModelRenderingPipeLine->GetCamera())
      {
        projMtx = pPokeModelRenderingPipeLine->GetCamera()->GetProjectionMatrix();
        viewMtx = pPokeModelRenderingPipeLine->GetCamera()->GetViewMatrix();
        setMtxFlag = true;
      }

      if(setMtxFlag)
      {
        pPokeModelRenderingPipeLine->SetProjectionMatrix( projMtx );
        pPokeModelRenderingPipeLine->SetViewMatrix( viewMtx );
      }
    }

    pPokeModelRenderingPipeLine->SetRenderTarget( pOffScreenRenderTarget );
    pPokeModelRenderingPipeLine->SetDepthStencil( pOffScreenDepthBuffer );
    pPokeModelRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );

    // コマンド発行
    {
      gfl2::gfx::GFGL::SetRenderTarget( pOffScreenRenderTarget );
      gfl2::gfx::GFGL::SetDepthStencil( pOffScreenDepthBuffer );

      gfl2::gfx::GFGL::BeginScene();

      // クリア
      gfl2::gfx::GFGL::SetViewport(0, 0, displayWidth, displayHeight);  // 縦横ひっくり返す？
      gfl2::gfx::GFGL::SetScissor(0, 0, displayWidth, displayHeight);   // スクリーンサイズが(w400,h240)だったらバッファサイズは(w512,h256)になっているが、バッファサイズで指定する？
      gfl2::gfx::GFGL::ClearRenderTargetDepthStencil( clearColor );

      // 設定
      gfl2::gfx::GFGL::SetViewport(0, 0, displayWidth, displayHeight);  // 縦横そのまま、スクリーンサイズ、での指定でいい。例: (w400,h240)
      gfl2::gfx::GFGL::SetScissor(0, 0, displayWidth, displayHeight);
#if defined(GF_PLATFORM_CTR)
      gfl2::gfx::GFGL::FlushAllDirtyCommand();  // ViewportとSissorのコマンドを発行してくれるClearRenderTargetを呼んでいないので、自分でコマンドを発行しておく。
#endif  // GF_PLATFORM_CTR

      gfl2::gfx::GFGL::EndScene();
    }

    pPokeModelRenderingPipeLine->Execute();

    // コマンド発行
    {
      gfl2::gfx::GFGL::SetRenderTarget( pOffScreenRenderTarget );
      gfl2::gfx::GFGL::SetDepthStencil( pOffScreenDepthBuffer );

      gfl2::gfx::GFGL::BeginScene();

      // デプスバッファだけクリア
      gfl2::gfx::GFGL::SetViewport(0, 0, displayWidth, displayHeight);  // 縦横ひっくり返す？
      gfl2::gfx::GFGL::SetScissor(0, 0, displayWidth, displayHeight);   // スクリーンサイズが(w400,h240)だったらバッファサイズは(w512,h256)になっているが、バッファサイズで指定する？
      gfl2::gfx::GFGL::ClearDepthStencil();
      
      gfl2::gfx::GFGL::EndScene();

      gfl2::gfx::GFGL::SetRenderTarget( pBackBuffer );
      gfl2::gfx::GFGL::SetDepthStencil( pDepthBuffer );
    }
  }
}

void AppRenderingManager::layoutRenderingPipeLineSingleExecute(LayoutRenderingPipeLine* pLayoutRenderingPipeLine, gfl2::gfx::DisplayDeviceHandle handle)
{
  if(pLayoutRenderingPipeLine)
  {
    gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( handle );
    gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( handle );
    u32                 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( handle );
    u32                 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( handle );
    pLayoutRenderingPipeLine->SetRenderTarget( pBackBuffer );
    pLayoutRenderingPipeLine->SetDepthStencil( pDepthBuffer );
    pLayoutRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );
    pLayoutRenderingPipeLine->Execute();
  }
}

void AppRenderingManager::pokeModelRenderingPipeLineSingleExecute(app_rendering_manager_internal::PokeModelRenderingPipeLine* pPokeModelRenderingPipeLine, gfl2::gfx::CtrDisplayNo displayNo)
{
  if(pPokeModelRenderingPipeLine)
  {
    gfl2::gfx::DisplayDeviceHandle handle = System::GflUse::m_HandleList[displayNo];
 
    gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( handle );
    gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( handle );
    u32                 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( handle );
    u32                 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( handle );

    {
      bool setMtxFlag = false;
      bool isOrtho    = false;
      gfl2::math::Matrix44 projMtx;
      gfl2::math::Matrix34 viewMtx;

      if(pPokeModelRenderingPipeLine->GetInFrameCamera())
      {
        projMtx = pPokeModelRenderingPipeLine->GetInFrameCamera()->GetProjectionMatrix();
        viewMtx = pPokeModelRenderingPipeLine->GetInFrameCamera()->GetViewMatrix();
        setMtxFlag = true;
      }
      else if(pPokeModelRenderingPipeLine->GetAppCamera())
      {
        isOrtho = (pPokeModelRenderingPipeLine->GetAppCamera()->GetProjectionType() == AppCamera::ProjectionType::ORTHO);
        projMtx = pPokeModelRenderingPipeLine->GetAppCamera()->GetProjectionMatrix();
        viewMtx = pPokeModelRenderingPipeLine->GetAppCamera()->GetViewMatrix();
        setMtxFlag = true;
      }
      else if(pPokeModelRenderingPipeLine->GetCamera())
      {
        projMtx = pPokeModelRenderingPipeLine->GetCamera()->GetProjectionMatrix();
        viewMtx = pPokeModelRenderingPipeLine->GetCamera()->GetViewMatrix();
        setMtxFlag = true;
      }

      if(setMtxFlag)
      {
        gfl2::math::Matrix44 stereoProjMtx[2];  // 0=Left; 1=Right;
        gfl2::math::Matrix34 stereoViewMtx[2];
        u32 idx = 0;

        if(isOrtho)
        {
          stereoProjMtx[0] = stereoProjMtx[1] = projMtx;
          stereoViewMtx[0] = stereoViewMtx[1] = viewMtx;
        }
        else
        {
#if defined(GF_PLATFORM_CTR)
          f32 depthLevel = 0.0f;
          f32 factor     = 0.0f;
          if(displayNo == gfl2::gfx::CtrDisplayNo::LEFT || displayNo == gfl2::gfx::CtrDisplayNo::RIGHT)
          {
            if(displayNo == gfl2::gfx::CtrDisplayNo::RIGHT) idx = 1;
            depthLevel = 4.0f;
            factor     = 1.0f;
          }
          else
          {
            depthLevel = 4.0f;
            factor     = 0.0f;
          }
          gfl2::gfx::GFGL::GetStereoMatrix
          (
            projMtx,
            viewMtx,
            depthLevel,
            factor, 
            &stereoProjMtx[0],
            &stereoViewMtx[0],
            &stereoProjMtx[1],
            &stereoViewMtx[1]
          );
#elif defined(GF_PLATFORM_WIN32)
          stereoProjMtx[0] = stereoProjMtx[1] = projMtx;
          stereoViewMtx[0] = stereoViewMtx[1] = viewMtx;
#endif  // GF_PLATFORM_CTR, GF_PLATFORM_WIN32
        }

        pPokeModelRenderingPipeLine->SetProjectionMatrix( stereoProjMtx[idx] );
        pPokeModelRenderingPipeLine->SetViewMatrix( stereoViewMtx[idx] );
      }
    }

    pPokeModelRenderingPipeLine->SetRenderTarget( pBackBuffer );
    pPokeModelRenderingPipeLine->SetDepthStencil( pDepthBuffer );
    pPokeModelRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );
 
    pPokeModelRenderingPipeLine->Execute();
  }
}

void AppRenderingManager::raderChartRenderingPipeLineSingleExecute(app_rendering_manager_internal::RaderChartRenderingPipeLine* pRaderChartRenderingPipeLine, gfl2::gfx::CtrDisplayNo displayNo)
{
  if(pRaderChartRenderingPipeLine)
  {
    gfl2::gfx::DisplayDeviceHandle handle = System::GflUse::m_HandleList[displayNo];
 
    gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( handle );
    gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( handle );
    u32                 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( handle );
    u32                 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( handle );

    gfl2::math::Matrix44 projMatrix = gfl2::math::Matrix44::GetIdentity();
    gfl2::math::Matrix34 viewMatrix = gfl2::math::Matrix34::GetIdentity();
    pRaderChartRenderingPipeLine->SetProjectionMatrix(projMatrix);
    pRaderChartRenderingPipeLine->SetViewMatrix(viewMatrix);
    
    pRaderChartRenderingPipeLine->SetRenderTarget( pBackBuffer );
    pRaderChartRenderingPipeLine->SetDepthStencil( pDepthBuffer );
    pRaderChartRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );
 
    pRaderChartRenderingPipeLine->Execute();
  }
}

void AppRenderingManager::effectRenderingPipeLineSingleExecute(app_rendering_manager_internal::EffectRenderingPipeLine* pEffectRenderingPipeLine, gfl2::gfx::CtrDisplayNo displayNo)
{
  if(pEffectRenderingPipeLine && pEffectRenderingPipeLine->CanExecute())
  {
    gfl2::gfx::DisplayDeviceHandle handle = System::GflUse::m_HandleList[displayNo];
 
    gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( handle );
    gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( handle );
    u32                 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( handle );
    u32                 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( handle );

    {
      bool setMtxFlag = false;
      bool isOrtho    = false;
      gfl2::math::Matrix44 projMtx;
      gfl2::math::Matrix34 viewMtx;

      if(pEffectRenderingPipeLine->GetAppCamera())
      {
        isOrtho = (pEffectRenderingPipeLine->GetAppCamera()->GetProjectionType() == AppCamera::ProjectionType::ORTHO);
        projMtx = pEffectRenderingPipeLine->GetAppCamera()->GetProjectionMatrix();
        viewMtx = pEffectRenderingPipeLine->GetAppCamera()->GetViewMatrix();
        setMtxFlag = true;
      }
      else if(pEffectRenderingPipeLine->GetCamera())
      {
        projMtx = pEffectRenderingPipeLine->GetCamera()->GetProjectionMatrix();
        viewMtx = pEffectRenderingPipeLine->GetCamera()->GetViewMatrix();
        setMtxFlag = true;
      }

      if(setMtxFlag)
      {
        gfl2::math::Matrix44 stereoProjMtx[2];  // 0=Left; 1=Right;
        gfl2::math::Matrix34 stereoViewMtx[2];
        u32 idx = 0;
     
        if(isOrtho)
        {
          stereoProjMtx[0] = stereoProjMtx[1] = projMtx;
          stereoViewMtx[0] = stereoViewMtx[1] = viewMtx;
        }
        else
        {
#if defined(GF_PLATFORM_CTR)
          f32 depthLevel = 0.0f;
          f32 factor     = 0.0f;
          if(displayNo == gfl2::gfx::CtrDisplayNo::LEFT || displayNo == gfl2::gfx::CtrDisplayNo::RIGHT)
          {
            if(displayNo == gfl2::gfx::CtrDisplayNo::RIGHT) idx = 1;
            depthLevel = 4.0f;
            factor     = 1.0f;
          }
          else
          {
            depthLevel = 4.0f;
            factor     = 0.0f;
          }
          gfl2::gfx::GFGL::GetStereoMatrix
          (
            projMtx,
            viewMtx,
            depthLevel,
            factor, 
            &stereoProjMtx[0],
            &stereoViewMtx[0],
            &stereoProjMtx[1],
            &stereoViewMtx[1]
          );
#elif defined(GF_PLATFORM_WIN32)
          stereoProjMtx[0] = stereoProjMtx[1] = projMtx;
          stereoViewMtx[0] = stereoViewMtx[1] = viewMtx;
#endif  // GF_PLATFORM_CTR, GF_PLATFORM_WIN32
        }

        pEffectRenderingPipeLine->SetProjectionMatrix( stereoProjMtx[idx] );
        pEffectRenderingPipeLine->SetViewMatrix( stereoViewMtx[idx] );
      }
    }

    pEffectRenderingPipeLine->SetRenderTarget( pBackBuffer );
    pEffectRenderingPipeLine->SetDepthStencil( pDepthBuffer );
    pEffectRenderingPipeLine->SetDisplaySize( displayWidth, displayHeight );
 
    pEffectRenderingPipeLine->Execute();
  }
}

void AppRenderingManager::pokeModelRenderingPipeLineSingleAddDressUpModel(app_rendering_manager_internal::PokeModelRenderingPipeLine* pPokeModelRenderingPipeLine, poke_3d::model::DressUpModel* pDressUpModel, bool noEdge)
{
  for(s32 i=0; i<poke_3d::model::DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* partsModelInstanceNode = pDressUpModel->GetPartsModelInstanceNode(static_cast<poke_3d::model::DressUpModel::PartsModel>(i));
    if(partsModelInstanceNode)
    {
      pPokeModelRenderingPipeLine->GetModelRenderPath()->AddNode(partsModelInstanceNode);
      pPokeModelRenderingPipeLine->GetEdgeMapPath()->AddEdgeRenderingTarget(partsModelInstanceNode);
    }
  }
}
void AppRenderingManager::pokeModelRenderingPipeLineSingleRemoveDressUpModel(app_rendering_manager_internal::PokeModelRenderingPipeLine* pPokeModelRenderingPipeLine, poke_3d::model::DressUpModel* pDressUpModel, bool noEdge)
{
  for(s32 i=0; i<poke_3d::model::DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* partsModelInstanceNode = pDressUpModel->GetPartsModelInstanceNode(static_cast<poke_3d::model::DressUpModel::PartsModel>(i));
    if(partsModelInstanceNode)
    {
      pPokeModelRenderingPipeLine->GetModelRenderPath()->RemoveNode(partsModelInstanceNode);
      pPokeModelRenderingPipeLine->GetEdgeMapPath()->RemoveEdgeRenderingTarget(partsModelInstanceNode);
    }
  }
}

u32  AppRenderingManager::getLayoutLayerNum(RenderOrderType::Tag order)
{
  u32 num = 0;
  if(
       order == RenderOrderType::LAYOUT
    || order == RenderOrderType::LAYOUT_POKEMODEL
    || order == RenderOrderType::LAYOUT_RADERCHART
    || order == RenderOrderType::LAYOUT_RADERCHART_POKEMODEL
    || order == RenderOrderType::LAYOUT_RADERCHART_POKEMODEL0_POKEMODEL1
    || order == RenderOrderType::LAYOUT_RADERCHART_POKEMODEL0_POKEMODEL1_POKEMODEL2
    || order == RenderOrderType::LAYOUT_EFFECT
    || order == RenderOrderType::POKEMODEL_LAYOUT
    || order == RenderOrderType::POKEMODEL0_LAYOUT_RADERCHART_POKEMODEL1
    || order == RenderOrderType::POKEMODEL_EFFECT_LAYOUT
    || order == RenderOrderType::POKEMODEL0_LAYOUT_POKEMODEL1
    || order == RenderOrderType::POKEMODEL0_EFFECT_LAYOUT_POKEMODEL1
    || order == RenderOrderType::POKEMODEL0_POKEMODEL1_POKEMODEL2_EFFECT_LAYOUT
    || order == RenderOrderType::POKEMODEL0_POKEMODEL1_POKEMODEL2_LAYOUT
  )
  {
    num = 1;
  }
  else if(
       order == RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1
    || order == RenderOrderType::LAYOUT0_LAYOUT1
    || order == RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1_EFFECT
  )
  {
    num = 2;
  }
  else if(
       order == RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1_LAYOUT2
    || order == RenderOrderType::LAYOUT0_LAYOUT1_LAYOUT2
  )
  {
    num = 3;
  }
  return num;
}
u32  AppRenderingManager::getPokeModelLayerNum(RenderOrderType::Tag order)
{
  u32 num = 0;
  if(
       order == RenderOrderType::POKEMODEL
    || order == RenderOrderType::LAYOUT_POKEMODEL
    || order == RenderOrderType::LAYOUT_RADERCHART_POKEMODEL
    || order == RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1
    || order == RenderOrderType::POKEMODEL_LAYOUT
    || order == RenderOrderType::POKEMODEL_EFFECT_LAYOUT
    || order == RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1_LAYOUT2
    || order == RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1_EFFECT
  )
  {
    num = 1;
  }
  else if(
       order == RenderOrderType::LAYOUT_RADERCHART_POKEMODEL0_POKEMODEL1
    || order == RenderOrderType::POKEMODEL0_LAYOUT_RADERCHART_POKEMODEL1
    || order == RenderOrderType::POKEMODEL0_LAYOUT_POKEMODEL1
    || order == RenderOrderType::POKEMODEL0_EFFECT_LAYOUT_POKEMODEL1
  )
  {
    num = 2;
  }
  else if(
       order == RenderOrderType::LAYOUT_RADERCHART_POKEMODEL0_POKEMODEL1_POKEMODEL2
    || order == RenderOrderType::POKEMODEL0_POKEMODEL1_POKEMODEL2_EFFECT_LAYOUT
    || order == RenderOrderType::POKEMODEL0_POKEMODEL1_POKEMODEL2_LAYOUT
  )
  {
    num = 3;
  }
  return num;
}
u32  AppRenderingManager::getEffectLayerNum(RenderOrderType::Tag order)
{
  u32 num = 0;
  if(
       order == RenderOrderType::LAYOUT_EFFECT
    || order == RenderOrderType::POKEMODEL_EFFECT_LAYOUT
    || order == RenderOrderType::POKEMODEL0_EFFECT_LAYOUT_POKEMODEL1
    || order == RenderOrderType::POKEMODEL0_POKEMODEL1_POKEMODEL2_EFFECT_LAYOUT
    || order == RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1_EFFECT
  )
  {
    num = 1;
  }
  return num;
}

bool AppRenderingManager::isTherePokeModel(RenderOrderType::Tag order)
{
  return (
       order == RenderOrderType::POKEMODEL
    || order == RenderOrderType::LAYOUT_POKEMODEL
    || order == RenderOrderType::LAYOUT_RADERCHART_POKEMODEL
    || order == RenderOrderType::LAYOUT_RADERCHART_POKEMODEL0_POKEMODEL1
    || order == RenderOrderType::LAYOUT_RADERCHART_POKEMODEL0_POKEMODEL1_POKEMODEL2
    || order == RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1
    || order == RenderOrderType::POKEMODEL_LAYOUT
    || order == RenderOrderType::POKEMODEL0_LAYOUT_RADERCHART_POKEMODEL1
    || order == RenderOrderType::POKEMODEL_EFFECT_LAYOUT
    || order == RenderOrderType::POKEMODEL0_LAYOUT_POKEMODEL1
    || order == RenderOrderType::POKEMODEL0_EFFECT_LAYOUT_POKEMODEL1
    || order == RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1_LAYOUT2
    || order == RenderOrderType::POKEMODEL0_POKEMODEL1_POKEMODEL2_EFFECT_LAYOUT
    || order == RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1_EFFECT
    || order == RenderOrderType::POKEMODEL0_POKEMODEL1_POKEMODEL2_LAYOUT
  );
}
bool AppRenderingManager::isThereRaderChart(RenderOrderType::Tag order)
{
  return (
       order == RenderOrderType::LAYOUT_RADERCHART
    || order == RenderOrderType::LAYOUT_RADERCHART_POKEMODEL
    || order == RenderOrderType::LAYOUT_RADERCHART_POKEMODEL0_POKEMODEL1
    || order == RenderOrderType::LAYOUT_RADERCHART_POKEMODEL0_POKEMODEL1_POKEMODEL2
    || order == RenderOrderType::POKEMODEL0_LAYOUT_RADERCHART_POKEMODEL1
  );
}
bool AppRenderingManager::isThereGfxObject(RenderOrderType::Tag order)
{
  return (
       order == RenderOrderType::POKEMODEL
    || order == RenderOrderType::LAYOUT_POKEMODEL
    || order == RenderOrderType::LAYOUT_RADERCHART
    || order == RenderOrderType::LAYOUT_RADERCHART_POKEMODEL
    || order == RenderOrderType::LAYOUT_RADERCHART_POKEMODEL0_POKEMODEL1
    || order == RenderOrderType::LAYOUT_RADERCHART_POKEMODEL0_POKEMODEL1_POKEMODEL2
    || order == RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1
    || order == RenderOrderType::POKEMODEL_LAYOUT
    || order == RenderOrderType::POKEMODEL0_LAYOUT_RADERCHART_POKEMODEL1
    || order == RenderOrderType::POKEMODEL_EFFECT_LAYOUT
    || order == RenderOrderType::POKEMODEL0_LAYOUT_POKEMODEL1
    || order == RenderOrderType::POKEMODEL0_EFFECT_LAYOUT_POKEMODEL1
    || order == RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1_LAYOUT2
    || order == RenderOrderType::POKEMODEL0_POKEMODEL1_POKEMODEL2_EFFECT_LAYOUT
    || order == RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1_EFFECT
    || order == RenderOrderType::POKEMODEL0_POKEMODEL1_POKEMODEL2_LAYOUT
  );
}
bool AppRenderingManager::isThereEffect(RenderOrderType::Tag order)
{
  return (
       order == RenderOrderType::LAYOUT_EFFECT
    || order == RenderOrderType::POKEMODEL_EFFECT_LAYOUT
    || order == RenderOrderType::POKEMODEL0_EFFECT_LAYOUT_POKEMODEL1
    || order == RenderOrderType::POKEMODEL0_POKEMODEL1_POKEMODEL2_EFFECT_LAYOUT
    || order == RenderOrderType::LAYOUT0_POKEMODEL_LAYOUT1_EFFECT
  );
}

bool AppRenderingManager::isUpperDisplayModeStereo(void)  // 上画面の表示モードが立体視になっているときtrue(実際に右目レンダリングを行っているときtrue)
{
  bool ret = false;
#if defined(GF_PLATFORM_CTR)
  ret = !( nn::gx::CTR::IsStereoVisionAllowed() == false || gfl2::gfx::ctr::CTRGL::IsStereoModeEnable() == false );  // 右目レンダリング判定
#endif
  return ret;
}


void AppRenderingManager::transferColorBuffer( gfl2::gfx::CtrDisplayNo no )
{
  if( no == gfl2::gfx::CtrDisplayNo::LEFT )
  {
    if( mpColorBufferLiner )
    {
      gfl2::gfx::GFGL::AddTransferLinearColorBufferCommand( mpColorBufferLiner, true );
      mpColorBufferLiner = NULL;
    }
    if( mpColorBufferBlock )
    {
      gfl2::gfx::GFGL::AddTransferColorBufferCommand( mpColorBufferBlock );
      mpColorBufferBlock = NULL;
    }
  }
}

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

