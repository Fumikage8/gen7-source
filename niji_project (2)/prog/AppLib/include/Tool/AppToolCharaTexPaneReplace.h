//======================================================================
/**
 * @file AppToolCharaTexPaneReplace.h
 * @date 2015/08/06 16:13:31
 * @author uchida_yuto
 * @brief キャラテクスチャを画像ペインに貼り付ける
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __APP_TOOL_CHARA_TEX_PANE_REPLACE_H_INCLUDED__
#define __APP_TOOL_CHARA_TEX_PANE_REPLACE_H_INCLUDED__
#pragma once

// gflib2のインクルード
#include <Layout/include/gfl2_Layout.h>
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include <model/include/gfl2_BaseCamera.h>
#include <model/include/gfl2_CharaModelFactory.h>

//  nijiのインクルード
#include "AppLib/include/tool/AppToolCharaSimpleModel.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class RenderTargetTexture;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class AppRenderingManager;
  class Heap;
  class G2DUtil;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(System)

  class nijiAllocator;

GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  /**
   * @class AppToolCharaTexPaneReplace
   * @brief キャラモデルからテクスチャを作成して画像ペインに貼り付ける
   */
  class AppToolCharaTexPaneReplace
  {
    GFL_FORBID_COPY_AND_ASSIGN(AppToolCharaTexPaneReplace);
  public:
    //  貼り付け通知データ
    struct REQUEST_REPLACE_DATA
    {
      REQUEST_REPLACE_DATA()
      {
        charaId = 0;
        animeId = 0;
        animeFrameNum = 0;
        eyeIndex  = poke_3d::model::CharaModel::EYE_INDEX_NONE;

        camFovy       = 0.0f;
        pReplacePane  = NULL;
      }

      //  貼り付けるペイン
      gfl2::lyt::LytPicture* pReplacePane;

      //  トレーナー情報
      //  トレーナーID
      s32 charaId;
      //  着せ替え
      poke_3d::model::DressUpParam dressUpParam;

      //  モーションid
      s32 animeId;
      //  モーションフレーム値
      u32 animeFrameNum;
      //  キャラの顔情報
      poke_3d::model::CharaModel::EyeIndex  eyeIndex;

      //  カメラ情報
      //  カメラ座標
      gfl2::math::Vector3 camPos;
      //  カメラの画角
      f32 camFovy;

      //  キャラRT情報
      gfl2::math::Vector3 modelPos;
      gfl2::math::Vector3 modelRot;
    };

    //  @brief  使用するVRAMサイズを取得
    static u32  GetUseVramAllocatorSize();

    //  @brief  クラスが使用するシステムヒープサイズ
    static u32 GetUseSystemHeapSize();

    //  @brief  クラスが使用するデバイスヒープサイズ
    static u32 GetUseDeviceHeapSize();

    //  @brief  モデルで使うメモリサイズ
    static u32 GetUseModelHeapSize();


    //--------------------------------------------------------------------------------------------
    /**
    * @brief      コンストラクタ
    * @param[in]  pRenderingManager:   内部でオフスクリーンを使うのに必要( 内部で参照しているのでアドレス先を破棄してはいけない )
    * @param[in]  pHeap:               内部で使用するヒープ( 内部で参照しているのでアドレス先を破棄してはいけない )
    * @param[in]  pFileManager:        モデル読みこみのファイル管理
    * @param[in]  pVramAllocator:      レンダーテクスチャのVRAMアロケータ(レンダーテクスチャーに必要なメモリを作成 GetUseVramAllocatorSize() からサイズ取得可能 )
    * @note
    *     内部でキャラモデル管理クラスを生成する
    *     メモリを数MB以上使う
    */
    //--------------------------------------------------------------------------------------------
    AppToolCharaTexPaneReplace( 
      app::util::AppRenderingManager* pRenderinManager, app::util::Heap* pHeap, 
      gfl2::fs::AsyncFileManager* pFileManager, System::nijiAllocator* pVramAllocator );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      コンストラクタ
    * @param[in]  pRenderingManager:   内部でオフスクリーンを使うのに必要( 内部で参照しているのでアドレス先を破棄してはいけない )
    * @param[in]  pHeap:               内部で使用するヒープ( 内部で参照しているのでアドレス先を破棄してはいけない )
    * @param[in]  pCharaModelFactory:  モデル制御に使う管理クラス
    * @param[in]  pVramAllocator:      レンダーテクスチャのVRAMアロケータ(レンダーテクスチャーに必要なメモリを作成 GetUseVramAllocatorSize() からサイズ取得可能 )
    * @note
    *   poke_3d::model::CharaModelFactoryのインスタンスで外部で作成して設定する
    *   なぜこれを用意したのかはメモリ節約が目的
    *   このクラスの処理を複数で平行で処理する場合は poke_3d::model::CharaModelFactory インスタンスは数分必要だが
    *   平行でなく単体のみの処理の時は poke_3d::model::CharaModelFactory インスタンスは一つだけ用意してい使いまわせばメモリ節約する場合は
    *   この関数を用意
    */
    //--------------------------------------------------------------------------------------------
    AppToolCharaTexPaneReplace( 
      app::util::AppRenderingManager* pRenderinManager, app::util::Heap* pHeap, 
      poke_3d::model::CharaModelFactory* pCharaModelFactory,
      poke_3d::model::DressUpModelResourceManager* pDressUpModelResourceManager, System::nijiAllocator* pVramAllocator );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      デストラクタ
    */
    //--------------------------------------------------------------------------------------------
    ~AppToolCharaTexPaneReplace();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      クラス使用終了
    *             クラスを破棄する時必ずこれを実行
    */
    //--------------------------------------------------------------------------------------------
    void Terminate();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      クラス使用終了可能か
    * @return     実行中 false / 実行終了(クラスの破棄可能に) true
    * @note
    *             必ず Terminate() を呼んでから実行
    */
    //--------------------------------------------------------------------------------------------
    bool IsDelete();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      キャラテクスチャ画像貼り付け依頼
    * @param[in]  rRequestData:  貼り付け時の依頼データ
    * @return     依頼成功 true / 依頼失敗 false
    */
    //--------------------------------------------------------------------------------------------
    bool RequestReplace( const REQUEST_REPLACE_DATA& rRequestData );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      キャラテクスチャを貼り付けているか
    * @param[in]  pReplacePane: 指定ペインにテクスチャが貼り付けているか
    * @return     貼り付けている true / 貼り付けていない false
    */
    //--------------------------------------------------------------------------------------------
    bool IsReplacePane( const gfl2::lyt::LytPicture* pReplacePane );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      貼り付け処理が動いているか
    */
    //--------------------------------------------------------------------------------------------
    bool IsReplaceRun();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      更新
    * @note
    *             必ず毎フレーム実行しないといけない
    */
    //--------------------------------------------------------------------------------------------
    void Update();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      切り替えリソース管理取得
    */
    //--------------------------------------------------------------------------------------------
    poke_3d::model::DressUpModelResourceManager*  GetDressupModelResManger() { return m_pDressUpModelResourceManager; }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      テクスチャーインフォ取得
    */
    //--------------------------------------------------------------------------------------------
    gfl2::lyt::LytResTextureInfo& GetTexInfo() { return m_texInfo; }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      テクスチャーバッファ取得
    */
    //--------------------------------------------------------------------------------------------
    void* GetTextureBuffer();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      テクスチャー貼り付けするか
    */
    //--------------------------------------------------------------------------------------------
    void SetEnableReplaceTex( const bool bFlg ) { m_bReplaceTex = bFlg; }

  private:
    //  クラスを動かすのに必要なヒープメモリ数
    enum
    {
      //  クラス内のインスタンス確保のヒープサイズ
      DEV_HEAP_SIZE              = 1024 * 400,

      //  システムヒープサイズ
      SYS_HEAP_SIZE = 2048
    };

    //  貼り付けデータ
    struct REPLACE_DATA
    {
      //  貼り付つける依頼データ
      REQUEST_REPLACE_DATA          requestData;

      bool                          bReplaceStart;
      CharaSimpleModel*             pCharaModel;
    };

    //  @brief  テクスチャのピクセルカラーフォーマット
    static gfl2::gfx::Format _GetTexturePixelColorFormat();
    //  @brief  テクスチャのサイズ
    static gfl2::math::Vector2  _GetTextureSize();

    //  @brief クラスのコンストラクタで必ず呼び出す
    void _Constructor( app::util::AppRenderingManager* pRenderinManager, app::util::Heap* pHeap, System::nijiAllocator* pVramAllocator );

    //  全変数の初期化
    void _Clear()
    {
      m_pRenderTargetTexture      = NULL;
      m_pVramAllocator            = NULL;
      m_pRenderinManager          = NULL;
      m_pHeap                     = NULL;
      m_pInReplacePane            = NULL;
      m_pCamera                   = NULL;
      m_pDrawEnvNodeCamera        = NULL;
      m_pModelFactoryHeap         = NULL;
      m_pCharaModelFactory        = NULL;
      m_pModelResourceManagerHeap = NULL;
      m_pReplaceData              = NULL;  
      m_pDressUpModelResourceManager = NULL;
      m_terminateSeq              = 0;
      m_replaceSeq                = 0;
      m_bTerminate                = false;
      m_bCreateCharaModelFactor   = false;
      m_bReplaceTex               = true;
      m_camTargetLength           = 0.0f;
    }

    //  描画テクスチャ
    app::util::RenderTargetTexture*                           m_pRenderTargetTexture;
    //  描画テクスチャをVRAM使用する時
    System::nijiAllocator*                                    m_pVramAllocator;

    app::util::AppRenderingManager*                           m_pRenderinManager;     //  内部で参照しているのでアドレス先を破棄してはいけない
    app::util::Heap*                                          m_pHeap;                //  内部で参照しているのでアドレス先を破棄してはいけない
    gfl2::heap::HeapBase*                                     m_pModelFactoryHeap;
    gfl2::heap::HeapBase*                                     m_pModelResourceManagerHeap;

    //  テクスチャを貼り付けたペイン
    gfl2::lyt::LytPicture*                                    m_pInReplacePane;

    REPLACE_DATA                                              m_replaceDataBuffer;

    REPLACE_DATA*                                             m_pReplaceData;

    //  カメラ情報
    poke_3d::model::BaseCamera*                               m_pCamera;
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvNodeCamera;

    //  キャラモデル管理
    poke_3d::model::CharaModelFactory*                        m_pCharaModelFactory;
    bool                                                      m_bCreateCharaModelFactor;
    poke_3d::model::DressUpModelResourceManager*              m_pDressUpModelResourceManager;
    bool                                                      m_bCreateDressUpModelResourceManager;

    gfl2::lyt::LytResTextureInfo  m_texInfo;

    f32   m_camTargetLength;

    u32   m_replaceSeq;
    u32   m_terminateSeq;
    bool  m_bTerminate;
    bool  m_bReplaceTex;
  };

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

#endif // __APP_TOOL_CHARA_TEX_PANE_REPLACE_H_INCLUDED__