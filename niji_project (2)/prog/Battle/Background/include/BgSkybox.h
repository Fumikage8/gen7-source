//======================================================================
/**
 * @file	BgSkybox.h
 * @brief	戦闘背景 天球パーツ
 * @author	anai
 * @data	15/07/28
 * 
 * BgSystemクラス経由で呼ばれる実装をめざす
 * 現状、AnaiTestProcでのテストでのみ使用可能
 */
//======================================================================
#if !defined( __BG_SKYBOX_H__ )
#define __BG_SKYBOX_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>

#include <Heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <system/include/nijiAllocator.h>
#include <arc_def_index/arc_def.h>
#include <arc_index/background.gaix>
#include <System/include/HeapDefine.h>
#include <GameSys/include/GameProcManager.h>
#include <Debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_std_string.h>


#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryEnvData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfEnvData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNode.h>

#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/AppRenderingManager.h"

#include <Animation/include/gfl2_AnimationController.h>
#include <Animation/include/gfl2_JointController.h>

#include <Battle/Background/include/BgSystem.h>


#include <arc_index/islandground.gaix> 

#include "System/include/SkyBox/Skybox.h"


GFL_NAMESPACE_BEGIN(gfl2)
  GFL_NAMESPACE_BEGIN(util)
    class GLHeapAllocator;
  GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
  class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(btl)

//戦闘背景天球パーツ
class BgSkybox
{
public:
  BgSkybox();
  ~BgSkybox();
  void Create(::app::util::Heap* appHeap , gfl2::fs::AsyncFileManager* afm);

  //ここから----------
  void Delete(void);
  void SetVisible(bool flg);
  void PlayAnimeCore(int anmType);
  void StopAnimeCore(int anmType);
  //------ここまで空関数


  bool isReadySkybox();
  bool isReadySkyboxResource();
  void SetWeather(System::Skybox::Skybox::SkyType s_type);
  void PreDraw();
  void Draw(gfl2::gfx::CtrDisplayNo displayNo);
  void Update(const gfl2::math::Vector3& pos, s32 hour, s32 minute, s32 second);


private:
  static const int READ_DATA_NUM = GARC_islandground_DATA_NUM;
  ::app::util::FileAccessor::RequestData*                     m_pFrs[READ_DATA_NUM];
  int                                                         mFileSeq;
  System::Skybox::Skybox*                                     mpSkybox;
  System::nijiAllocator*                                      m_pNijiAllocator;  //レンダリングに必要なアロケータ
  ::app::util::AppRenderingManager*                           m_appRenderingManager;    ///APPレンダリングマネージャー
  ::app::util::AppRenderingManager::Description               m_Description;  //APPレンダリングマネージャーの設定
  ::app::util::Heap*                                          mpAppHeap;

  poke_3d::model::BaseCamera*                                 m_camera;       // カメラ
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*   m_drawEnvNode;  // カメラを含む環境
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*   m_pDrawEnvNode;  // カメラを含む環境

  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pDrawEnvResourceRootNode;
  void*                                                       mpResourceBuffer[GARC_islandground_DATA_NUM];   //読み込んだ素材管理バッファ

  gfl2::fs::AsyncFileManager*                                 mpAsyncFileManager;
  void fileReadStructSetup(::app::util::FileAccessor::RequestData* pfrs, int arcid, int dataid, ::app::util::Heap* pAppHeap, void** pReadBuffAddr);
  bool fileReadSequece(void);

};
GFL_NAMESPACE_END(btl)


#endif //__BG_SKYBOX_H__
