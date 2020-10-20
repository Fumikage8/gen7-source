#ifndef __APP_RENDERING_MANAGER_H__
#define __APP_RENDERING_MANAGER_H__
#pragma once


//=============================================================================================
/**
 * @file   AppRenderingManager.h
 * @brief  アプリ用のパイプラインを管理するクラス
 * @author k.ohno
 * @date   2015.2.28
 */
//=============================================================================================


#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <System/include/AppRenderingPipeLine.h>
#include <layout/include/gfl2_LytSys.h>
#include <Effect/include/gfl2_EffectRenderPath.h>
#include "AppLib/include/Util/app_util_heap.h"


GFL_NAMESPACE_BEGIN(PokeTool)
class PokeModelSystem;
class PokeModel;
GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(model)
class BaseCamera;
class BaseModel;
class CharaModel;
class DressUpModel;
GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)

GFL_NAMESPACE_BEGIN(System)
class nijiAllocator;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(System)
GFL_NAMESPACE_BEGIN(Camera)
class InFrameCamera;
GFL_NAMESPACE_END(Camera)
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(app) 
GFL_NAMESPACE_BEGIN(tool)
class RaderChart;
GFL_NAMESPACE_END(tool) 
GFL_NAMESPACE_END(app) 

GFL_NAMESPACE_BEGIN(app) 
GFL_NAMESPACE_BEGIN(util) 
GFL_NAMESPACE_BEGIN(app_rendering_manager_internal) 
class PokeModelRendererBuilder;
class PokeModelRenderingPipeLine;
class RaderChartRenderingPipeLine;
class EffectRenderingPipeLine;
GFL_NAMESPACE_END(app_rendering_manager_internal) 
GFL_NAMESPACE_END(util) 
GFL_NAMESPACE_END(app) 
 
GFL_NAMESPACE_BEGIN(app) 
GFL_NAMESPACE_BEGIN(util) 
class RenderTargetTexture;
class AppCamera;
GFL_NAMESPACE_END(util) 
GFL_NAMESPACE_END(app) 
 



GFL_NAMESPACE_BEGIN(app) 
GFL_NAMESPACE_BEGIN(util) 


typedef AppRenderingPipeLine LayoutRenderingPipeLine;


class AppRenderingManager 
{ 
  GFL_FORBID_COPY_AND_ASSIGN(AppRenderingManager); //コピーコンストラクタ＋代入禁止


public:
  //! @brief  描画場所タイプ  // コメントアウトしているのはそのうち実装します。コメントアウトにもないものも必要ならば実装します。
  class RenderPlaceType
  {
  public:
    enum Tag
    {
      UPPER_DISPLAY,  //!< 上画面(上画面左、上画面右)
      LOWER_DISPLAY,  //!< 下画面
      OFF_SCREEN,     //!< オフスクリーン
      //OFF_SCREEN1,    //!< 【今後追加するならこんな感じ】オフスクリーン1(OFF_SCREENをOFF_SCREEN0にリネームすること)
      
      // 以下は制御用の定義。外部からは使用しないで下さい。
      NUM,
      //UPPER_LEFT_DISPLAY  = UPPER_DISPLAY,  // 2つ定義があるとややこしいのでコメントアウト。
      UPPER_RIGHT_DISPLAY = NUM,
      TOTAL_NUM           = UPPER_RIGHT_DISPLAY +1
    };
  };

  //! @brief  描画順タイプ  // コメントアウトしているのはそのうち実装します。コメントアウトにもないものも必要ならば実装します。
  class RenderOrderType
  {
  public:
    enum Tag
    {
      // POKEMODELは「ポケモンモデル、通常モデル、キャラモデル、着せ替えモデル」を描画できます。
      // MODELは「通常モデル」だけ描画できます。

      NONE,                                                   //!< 描画しない
      LAYOUT,                                                 //!< レイアウトだけ描画する
      POKEMODEL,                                              //!< ポケモンモデルだけ描画する
      LAYOUT_POKEMODEL,                                       //!< レイアウト→ポケモンモデル の順に描画する
      LAYOUT_RADERCHART,                                      //!< レイアウト→レーダーチャート の順に描画する
      LAYOUT_RADERCHART_POKEMODEL,                            //!< レイアウト→レーダーチャート→ポケモンモデル の順に描画する
      LAYOUT_RADERCHART_POKEMODEL0_POKEMODEL1,                //!< レイアウト→レーダーチャート→ポケモンモデル0→ポケモンモデル1 の順に描画する
      LAYOUT_RADERCHART_POKEMODEL0_POKEMODEL1_POKEMODEL2,     //!< レイアウト→レーダーチャート→ポケモンモデル0→ポケモンモデル1→ポケモンモデル2 の順に描画する
      LAYOUT_EFFECT,                                          //!< レイアウト→エフェクト の順に描画する
      LAYOUT0_POKEMODEL_LAYOUT1,                              //!< レイアウト0→ポケモンモデル→レイアウト1 の順に描画する
      POKEMODEL_LAYOUT,                                       //!< ポケモンモデル→レイアウト の順に描画する
      POKEMODEL0_LAYOUT_RADERCHART_POKEMODEL1,                //!< ポケモンモデル0→レイアウト→レーダーチャート→ポケモンモデル1 の順に描画する
      POKEMODEL_EFFECT_LAYOUT,                                //!< ポケモンモデル→エフェクト→レイアウト の順に描画する
      POKEMODEL0_LAYOUT_POKEMODEL1,                           //!< ポケモンモデル0→レイアウト→ポケモンモデル1 の順に描画する
      POKEMODEL0_EFFECT_LAYOUT_POKEMODEL1,                    //!< ポケモンモデル0→エフェクト→レイアウト→ポケモンモデル1 の順に描画する
      POKEMODEL0_POKEMODEL1_POKEMODEL2_EFFECT_LAYOUT,         //!< ポケモンモデル0→ポケモデル1→ポケモデル2→エフェクト→レイアウト の順に描画する
      LAYOUT0_POKEMODEL_LAYOUT1_LAYOUT2,                      //!< レイアウト0→ポケモンモデル→レイアウト1→レイアウト2 の順に描画する
      LAYOUT0_LAYOUT1,                                        //!< レイアウト0→レイアウト1 の順に描画する
      LAYOUT0_LAYOUT1_LAYOUT2,                                //!< レイアウト0→レイアウト1→レイアウト2 の順に描画する
      LAYOUT0_POKEMODEL_LAYOUT1_EFFECT,                       //!< レイアウト0→ポケモンモデル→レイアウト1→エフェクト の順に描画する
      POKEMODEL0_POKEMODEL1_POKEMODEL2_LAYOUT,                //!< ポケモンモデル0→ポケモデル1→ポケモデル2→レイアウト の順に描画する

      // ..._LAYER_NUM_MAXに注意！
      // LAYOUT_LAYER_NUM_MAX = 3;
      // POKEMODEL_LAYER_NUM_MAX = 3;
      // EFFECT_LAYER_NUM_MAX = 2;
    };
  };

  //! @brief  描画場所ごとの設定
  class RenderDescription
  {
  public:
    RenderOrderType::Tag renderOrder;         //!< 描画順タイプ
    u32                  lytWkMax;            //!< Addできるレイアウトワークの最大数
    u32                  pokeModelNumMax;     //!< Addできるポケモンモデルの最大数(通常モデル、キャラモデル、着せ替えモデルは含まない)
    u32                  modelNumMax;         //!< Addできる通常モデルの最大数(ポケモンモデル、キャラモデル、着せ替えモデルは含まない)
    u32                  charaModelNumMax;    //!< Addできるキャラモデルの最大数(ポケモンモデル、通常モデル、着せ替えモデルは含まない)
    u32                  dressUpModelNumMax;  //!< Addできる着せ替えモデルの最大数(ポケモンモデル、通常モデル、キャラモデルは含まない)
    bool                 useDistortionPath;   //!< ゆがみシェーダーを使用するときtrue(メガヤミラミを描画するときはtrue)
  public:
    RenderDescription(void)
      : renderOrder(RenderOrderType::NONE),
        lytWkMax(32),
        pokeModelNumMax(4),
        modelNumMax(16),
        charaModelNumMax(4),
        dressUpModelNumMax(4),
        useDistortionPath(false)
    {}
  };

  //! @brief  設定
  class Description
  {
  public:
    RenderDescription renderDesc[RenderPlaceType::NUM];  //!< 描画場所ごとの設定
    bool              isUpperDisplayStereo;              //!< 上画面を立体視にするときはtrue
    bool              callGlobalTraverseModel;           //!< gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelを呼ぶ場合はtrue、呼ばない場合はfalse
  public:
    Description(void)
      : isUpperDisplayStereo(false),
        callGlobalTraverseModel(true)
    {
      for(s32 i=0; i<RenderPlaceType::NUM; ++i)
      {
        renderDesc[i] = RenderDescription();
      }
    }
  };

public:
  //! @brief  プリセット
  class PresetType
  {
  public:
    enum Tag
    {
      DEFAULT,  //!< デフォルト
      POKEMON,  //!< ポケモン用
      TRAINER,  //!< トレーナー用
    };
  };
  //! @brief  デフォルトのライトセット番号
  static const u32 DEFAULT_LIGHT_SET_NO = 0xFFFFFFFF;

public:
  AppRenderingManager(void);
  virtual ~AppRenderingManager();

  //! @name  生成/破棄
  //@{
  
  //! @brief  非同期生成　開始
  //! @note  Updateを毎フレーム呼んでいないと非同期生成処理は進みません。
  void StartAsyncCreate(
      const Description&          desc,                             // 呼び出し元はdescインスタンスを破棄してもいい(descに書かれているポインタが指すものは破棄しないように)
      app::util::Heap*            pHeap,                            // 生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
      System::nijiAllocator*      pNijiAllocatorForEdgeMapTexture,  // VRAM限定。RenderOrderTypeにPOKEMODELを書いているときは必須、そうでないときはNULLでよい。
                                                                    // RenderDescriptionのuseDistortionPathがtrueのとき屈折テクスチャ用のVRAM確保もここから行う。
      gfl2::fs::AsyncFileManager* pAsyncFileManager                 // ファイルマネージャ
  );
  //! @brief  同期生成 
  void SyncCreate(
      const Description&          desc,                             // 呼び出し元はdescインスタンスを破棄してもいい(descに書かれているポインタが指すものは破棄しないように)
      app::util::Heap*            pHeap,                            // 生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
      System::nijiAllocator*      pNijiAllocatorForEdgeMapTexture,  // VRAM限定。RenderOrderTypeにPOKEMODELを書いているときは必須、そうでないときはNULLでよい。
                                                                    // RenderDescriptionのuseDistortionPathがtrueのとき屈折テクスチャ用のVRAM確保もここから行う。
      gfl2::fs::AsyncFileManager* pAsyncFileManager                 // ファイルマネージャ
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
  bool CanDestroy(void) const;
  
  //@}
 
  //! @name  設定
  //@{

  //! @brief  オフスクリーンレンダーターゲットとなるテクスチャをセットする
  //! @attention  一度SetしたらUnsetするまでSetされっぱなしです。
  void SetOffScreenRenderTargetTexture(RenderPlaceType::Tag renderPlace, app::util::RenderTargetTexture* renderTargetTexture); 
  //! @brief  オフスクリーンレンダーターゲットとなるテクスチャをアンセットする
  //! @attention  一度UnsetしたらSetするまでUnsetされっぱなしです。
  void UnsetOffScreenRenderTargetTexture(RenderPlaceType::Tag renderPlace);
  //! @brief  オフスクリーンレンダーターゲットにセットしているテクスチャのポインタを取得
  const app::util::RenderTargetTexture* GetOffScreenRenderTargetTexture() const { return mpOffScreenRenderTargetTexture; }
  //@}


public:
  //! @name  レイアウト描画パス
  //@{
 
  //! @brief  このフレームで描画するレイアウトを追加する
  //! @caution  毎フレームクリアされるので、毎フレームAddして下さい。
  //! @attention  【【【AddやSet関数の中でこれだけが毎フレーム行うものになります】】】
  void AddDrawableLytWkForOneFrame(RenderPlaceType::Tag renderPlace, gfl2::lyt::LytWk* pLytWk, u32 layerNo=0, u8 order = gfl2::lyt::ORDER_NORMAL);
  
  //@}


  //! @name  ポケモンモデル描画パス
  //@{
  
  //! @brief  ポケモンモデルをシーンに追加する
  //! @attention  一度AddしたらRemoveするまでAddされっぱなしです。
  void AddPokeModel(RenderPlaceType::Tag renderPlace, PokeTool::PokeModel* pPokeModel, u32 layerNo=0);
  //! @brief  ポケモンモデルをシーンから取り除く
  //! @attention  一度RemoveしたらAddするまでRemoveされっぱなしです。
  void RemovePokeModel(RenderPlaceType::Tag renderPlace, PokeTool::PokeModel* pPokeModel, u32 layerNo=0);
  
  //@}
  
  //! @name  通常モデル描画パス
  //@{
  
  //! @brief  通常モデルをシーンに追加する
  //! @attention  一度AddしたらRemoveするまでAddされっぱなしです。
  void AddModel(RenderPlaceType::Tag renderPlace, poke_3d::model::BaseModel* pModel, u32 layerNo=0, bool enableEdgeRenderPath=true);
  //! @brief  通常モデルをシーンから取り除く
  //! @attention  一度RemoveしたらAddするまでRemoveされっぱなしです。
  void RemoveModel(RenderPlaceType::Tag renderPlace, poke_3d::model::BaseModel* pModel, u32 layerNo=0);
  
  //@}

  //! @name  キャラモデル描画パス
  //@{
  
  //! @brief  キャラモデルをシーンに追加する
  //! @attention  一度AddしたらRemoveするまでAddされっぱなしです。
  void AddCharaModel(RenderPlaceType::Tag renderPlace, poke_3d::model::CharaModel* pCharaModel, u32 layerNo=0);
  //! @brief  キャラモデルをシーンから取り除く
  //! @attention  一度RemoveしたらAddするまでRemoveされっぱなしです。
  void RemoveCharaModel(RenderPlaceType::Tag renderPlace, poke_3d::model::CharaModel* pCharaModel, u32 layerNo=0);
  
  //@}

  //! @name  着せ替えモデル描画パス
  //@{
  
  //! @brief  着せ替えモデルをシーンに追加する
  //! @attention  一度AddしたらRemoveするまでAddされっぱなしです。
  void AddDressUpModel(RenderPlaceType::Tag renderPlace, poke_3d::model::DressUpModel* pDressUpModel, u32 layerNo=0);
  //! @brief  着せ替えモデルをシーンから取り除く
  //! @attention  一度RemoveしたらAddするまでRemoveされっぱなしです。
  void RemoveDressUpModel(RenderPlaceType::Tag renderPlace, poke_3d::model::DressUpModel* pDressUpModel, u32 layerNo=0);
  
  //@}

  //! @name  3Dオブジェクト共通
  //@{

  //! @note  カメラが複数ある場合の優先順位
  //         最優先 InFrameCamera > AppCamera > BaseCamera > DrawEnvNode

  //! @name  カメラをセットする(NULLのセットはUnset...関数で)
  //! @attention  一度SetしたらUnsetするまでSetされっぱなしです。
  void SetInFrameCamera(RenderPlaceType::Tag renderPlace, System::Camera::InFrameCamera* pCamera, u32 layerNo=0);
  //! @brief  カメラをアンセットする
  //! @attention  一度UnsetしたらSetするまでUnsetされっぱなしです。
  void UnsetInFrameCamera(RenderPlaceType::Tag renderPlace, u32 layerNo=0);
  //! @brief  設定しているカメラのポインタ取得
  const System::Camera::InFrameCamera* GetInFrameCamera( RenderPlaceType::Tag renderPlace, u32 layerNo=0 );
  
  //! @name  カメラをセットする(NULLのセットはUnset...関数で)
  //! @attention  一度SetしたらUnsetするまでSetされっぱなしです。
  //! @note  SetInFrameCameraでカメラがセットされている場合は、AppCameraは使わずInFrameCameraを優先して使う。
  void SetAppCamera(RenderPlaceType::Tag renderPlace, AppCamera* pCamera, u32 layerNo=0);
  //! @brief  カメラをアンセットする
  //! @attention  一度UnsetしたらSetするまでUnsetされっぱなしです。
  void UnsetAppCamera(RenderPlaceType::Tag renderPlace, u32 layerNo=0);
  //! @brief  設定しているカメラのポインタ取得
  const AppCamera* GetAppCamera( RenderPlaceType::Tag renderPlace, u32 layerNo=0 );

  //! @name  カメラをセットする(NULLのセットはUnset...関数で)
  //! @attention  一度SetしたらUnsetするまでSetされっぱなしです。
  //! @note  SetInFrameCamera, SetAppCameraでカメラがセットされている場合は、BaseCameraは使わずInFrameCamera, AppCameraを優先して使う。
  void SetCamera(RenderPlaceType::Tag renderPlace, poke_3d::model::BaseCamera* pCamera, u32 layerNo=0);
  //! @brief  カメラをアンセットする
  //! @attention  一度UnsetしたらSetするまでUnsetされっぱなしです。
  void UnsetCamera(RenderPlaceType::Tag renderPlace, u32 layerNo=0);
  //! @brief  設定しているカメラのポインタ取得
  const poke_3d::model::BaseCamera* GetCamera( RenderPlaceType::Tag renderPlace, u32 layerNo=0 );

  //! @brief  環境(ライトとか)をシーンに追加する 
  //! @attention  一度AddしたらRemoveするまでAddされっぱなしです。
  //! @note  SetInFrameCamera, SetAppCamera, SetCameraでカメラがセットされている場合は、DrawEnvNodeのカメラは使わずSetInFrameCamera, SetAppCamera, SetCameraのカメラを優先して使う。
  void AddDrawEnv(RenderPlaceType::Tag renderPlace, gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode, u32 layerNo=0); 
  //! @brief  環境(ライトとか)をシーンから取り除く 
  //! @attention  一度RemoveしたらAddするまでRemoveされっぱなしです。
  void RemoveDrawEnv(RenderPlaceType::Tag renderPlace, gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode, u32 layerNo=0); 

  //! @name  ポケモデルシステムをセットする(NULLのセットはUnset...関数で)
  //! @attention  一度SetしたらUnsetするまでSetされっぱなしです。
  //! @note  ポケモンの色変更を行う際は、ポケモデルシステムを渡して下さい。
  void SetPokeModelSystem(RenderPlaceType::Tag renderPlace, PokeTool::PokeModelSystem* pokeModelSys, u32 layerNo=0);
  //! @name  ポケモデルシステムをアンセットする
  //! @attention  一度UnsetしたらSetするまでUnsetされっぱなしです。
  void UnsetPokeModelSystem(RenderPlaceType::Tag renderPlace, u32 layerNo=0);

  //@}

  //! @name  レーダーチャート描画パス
  //@{
  
  //! @name  レーダーチャートをセットする(NULLのセットはUnset...関数で)
  //! @attention  一度SetしたらUnsetするまでSetされっぱなしです。
  void SetRaderChart(RenderPlaceType::Tag renderPlace, app::tool::RaderChart* pRaderChart);
  //! @brief  レーダーチャートをアンセットする
  //! @attention  一度UnsetしたらSetするまでUnsetされっぱなしです。
  void UnsetRaderChart(RenderPlaceType::Tag renderPlace);

  //@}

  //! @name  エフェクト描画パス
  //@{
  
  //! @brief  エフェクトをセットする(NULLのセットはUnset...関数で)
  //! @param[in]  effectRenderPathConfig  ここに渡したインスタンスは破棄してもいいですが、インスタンス内のポインタが指す先は破棄しないようにして下さい。
  //! @attention  一度SetしたらUnsetするまでSetされっぱなしです。
  void SetEffectRenderPathConfig(RenderPlaceType::Tag renderPlace, const gfl2::Effect::EffectRenderPath::Config& effectRenderPathConfig, u32 layerNo=0);
  //! @brief  エフェクトをアンセットする
  //! @attention  一度UnsetしたらSetするまでUnsetされっぱなしです。
  void UnsetEffectRenderPathConfig(RenderPlaceType::Tag renderPlace, u32 layerNo=0);

  //@}

  //! @name  エフェクト環境
  //@{

  //! @note  カメラが複数ある場合の優先順位
  //         最優先 InFrameCamera > AppCamera > BaseCamera > DrawEnvNode

  //! @name  カメラをセットする(NULLのセットはUnset...関数で)
  //! @attention  一度SetしたらUnsetするまでSetされっぱなしです。
  void SetEffectAppCamera(RenderPlaceType::Tag renderPlace, AppCamera* pCamera, u32 layerNo=0);
  //! @brief  カメラをアンセットする
  //! @attention  一度UnsetしたらSetするまでUnsetされっぱなしです。
  void UnsetEffectAppCamera(RenderPlaceType::Tag renderPlace, u32 layerNo=0);
  //! @brief  設定しているカメラのポインタ取得
  const AppCamera* GetEffectAppCamera( RenderPlaceType::Tag renderPlace, u32 layerNo=0 );

  //! @name  カメラをセットする(NULLのセットはUnset...関数で)
  //! @attention  一度SetしたらUnsetするまでSetされっぱなしです。
  //! @note  SetAppCameraでカメラがセットされている場合は、BaseCameraは使わずAppCameraを優先して使う。
  void SetEffectCamera(RenderPlaceType::Tag renderPlace, poke_3d::model::BaseCamera* pCamera, u32 layerNo=0);
  //! @brief  カメラをアンセットする
  //! @attention  一度UnsetしたらSetするまでUnsetされっぱなしです。
  void UnsetEffectCamera(RenderPlaceType::Tag renderPlace, u32 layerNo=0);
  //! @brief  設定しているカメラのポインタ取得
  const poke_3d::model::BaseCamera* GetEffectCamera( RenderPlaceType::Tag renderPlace, u32 layerNo=0 );

  //@}


  //! @name  便利関数
  //@{

  //! @brief  3Dオブジェクトを描画する際のデフォルトのカメラを生成する
  //! @param[in]   pHeap                 生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
  //! @param[out]  pDrawEnvNodeOfCamera  この関数で作成されるDrawEnvNode(CameraNodeの実体を持っている)を入れて返す。破棄はGFL_DELETEで。
  //! @param[out]  pCamera               この関数で作成されるBaseCamera(CameraNodeのポインタを持っている)を入れて返す。破棄はGFL_DELETEで。
  static void CreateDefaultCamera(app::util::Heap* pHeap, gfl2::renderingengine::scenegraph::instance::DrawEnvNode** pDrawEnvNodeOfCamera, poke_3d::model::BaseCamera** pCamera,RenderPlaceType::Tag renderPlace = RenderPlaceType::UPPER_DISPLAY);

  //! @brief  3Dオブジェクトを描画する際のデフォルトの環境(ライトとか)を生成する 
  //! @param[in]   pHeap         生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
  //! @param[in]   presetType    プリセット
  //! @param[in]   a_lightSetNo  ライトセット番号。DEFAULT_LIGHT_SET_NOのときはpresetTypeによって変わる。
  //! @return  pDrawEnvNode  この関数で作成されるDrawEnvNodeを入れて返す。破棄はGFL_DELETEで。 
  static gfl2::renderingengine::scenegraph::instance::DrawEnvNode* CreateDefaultDrawEnv(app::util::Heap* pHeap, PresetType::Tag presetType=PresetType::DEFAULT, u32 a_lightSetNo=DEFAULT_LIGHT_SET_NO);

  //@}


public: 
  void Update(void);
  void PreDraw(void);
  void Draw(gfl2::gfx::CtrDisplayNo displayNo);


private:
  // 描画
  void draw_Layout(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_PokeModel(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_Layout_PokeModel(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_Layout_RaderChart(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_Layout_RaderChart_PokeModel(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_Layout_RaderChart_PokeModel0_PokeModel1(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_Layout_RaderChart_PokeModel0_PokeModel1_PokeModel2(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_Layout_Effect(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_Layout0_PokeModel_Layout1(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_PokeModel_Layout(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_PokeModel0_Layout_RaderChart_PokeModel1(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_PokeModel_Effect_Layout(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_PokeModel0_Layout_PokeModel1(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_PokeModel0_Effect_Layout_PokeModel1(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_PokeModel0_PokeModel1_PokeModel2_Effect_Layout(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_Layout0_PokeModel_Layout1_Layout2(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_Layout0_Layout1(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_Layout0_Layout1_Layout2(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_Layout0_PokeModel_Layout1_Effect(gfl2::gfx::CtrDisplayNo displayNo);
  void draw_PokeModel0_PokeModel1_PokeModel2_Layout(gfl2::gfx::CtrDisplayNo displayNo);
  
  void drawOnOffScreen_PokeModel(RenderPlaceType::Tag renderPlace);

  // レイアウト描画パス
  void layoutRenderingPipeLineSingleExecute(LayoutRenderingPipeLine* pLayout, gfl2::gfx::DisplayDeviceHandle handle);
  
  // ポケモンモデル描画パス
  void pokeModelRenderingPipeLineSingleExecute(app_rendering_manager_internal::PokeModelRenderingPipeLine* pPokeModelRenderingPipeLine, gfl2::gfx::CtrDisplayNo displayNo);

  // レーダーチャート描画パス
  void raderChartRenderingPipeLineSingleExecute(app_rendering_manager_internal::RaderChartRenderingPipeLine* pRaderChartRenderingPipeLine, gfl2::gfx::CtrDisplayNo displayNo);

  // エフェクト描画パス
  void effectRenderingPipeLineSingleExecute(app_rendering_manager_internal::EffectRenderingPipeLine* pEffectRenderingPipeLine, gfl2::gfx::CtrDisplayNo displayNo);

  //! @brief  生成状態
  enum CreateState
  {
    CREATE_STATE_NONE,      //!< 生成していない
    CREATE_STATE_CREATING,  //!< 生成中
    CREATE_STATE_CREATED,   //!< 生成済み
  };

  // 非同期生成　続き
  void continueAsyncCreate(void);

  // 機能おまとめ
  void pokeModelRenderingPipeLineSingleAddDressUpModel(app_rendering_manager_internal::PokeModelRenderingPipeLine* pPokeModelRenderingPipeLine, poke_3d::model::DressUpModel* pDressUpModel, bool noEedge=false);
  void pokeModelRenderingPipeLineSingleRemoveDressUpModel(app_rendering_manager_internal::PokeModelRenderingPipeLine* pPokeModelRenderingPipeLine, poke_3d::model::DressUpModel* pDressUpModel, bool noEdge=false);

  // その他
  static u32  getLayoutLayerNum(RenderOrderType::Tag order);  // 0のときレイアウトなし。1以上のときレイアウトあり。
  static u32  getPokeModelLayerNum(RenderOrderType::Tag order);  // 0のときポケモンモデルなし。1以上のときポケモンモデルあり。
  static u32  getEffectLayerNum(RenderOrderType::Tag order);  // 0のときエフェクトなし。1以上のときエフェクトあり。
  static bool isTherePokeModel(RenderOrderType::Tag order);
  static bool isThereRaderChart(RenderOrderType::Tag order);
  static bool isThereGfxObject(RenderOrderType::Tag order);  // ポケモンモデル、通常モデル、キャラモデル、着せ替えモデル、レーダーチャートがあるか
  static bool isThereEffect(RenderOrderType::Tag order);

  static bool isUpperDisplayModeStereo(void);  // 上画面の表示モードが立体視になっているときtrue(実際に右目レンダリングを行っているときtrue)

public:
  void RequestTransferColorBuffer_Liner( void* buffer ) { mpColorBufferLiner = buffer; }
  void RequestTransferColorBuffer_Block( void* buffer ) { mpColorBufferBlock = buffer; }
private:
  void transferColorBuffer( gfl2::gfx::CtrDisplayNo no );

private:
  // 同じ種類のもののレイヤー数
  static const u32 LAYOUT_LAYER_NUM_MAX = 3;
  static const u32 POKEMODEL_LAYER_NUM_MAX = 3;
  static const u32 EFFECT_LAYER_NUM_MAX = 2;

  // ビットフラグ
  enum
  {
    LAYOUT_BIT_FLAG_NONE      = 0,
    LAYOUT_BIT_FLAG_ADD_MINE  = 1 << 0,  //!< このAddが呼ばれたときに立てる
    LAYOUT_BIT_FLAG_DRAW_SOME = 1 << 1,  //!< どれかのDrawが呼ばれたときに立てる(このDrawが呼ばれたときも立てる)
  };


private:
  CreateState                   mCreateState;
  Description                   mDesc;
  app::util::Heap*              mpCreateHeap;
  System::nijiAllocator*        mpCreateNijiAllocatorForEdgeMapTexture;
  gfl2::fs::AsyncFileManager*   mpAsyncFileManager;

  app::util::RenderTargetTexture* mpOffScreenRenderTargetTexture;

  // レイアウト描画パス
  u32                           mLayoutLayerNumUpper;
  u32                           mLayoutLayerNumLower;
  LayoutRenderingPipeLine*      mpLayoutRenderingPipeLineUpperLeft[LAYOUT_LAYER_NUM_MAX]; 
  LayoutRenderingPipeLine*      mpLayoutRenderingPipeLineUpperRight[LAYOUT_LAYER_NUM_MAX]; 
  LayoutRenderingPipeLine*      mpLayoutRenderingPipeLineLower[LAYOUT_LAYER_NUM_MAX]; 
  bit32                         mLayoutBitFlagUpperLeft[LAYOUT_LAYER_NUM_MAX];
  bit32                         mLayoutBitFlagUpperRight[LAYOUT_LAYER_NUM_MAX];
  bit32                         mLayoutBitFlagLower[LAYOUT_LAYER_NUM_MAX];

  // ポケモンモデル描画パス
  u32                           mPokeModelLayerNum[RenderPlaceType::NUM];
  app_rendering_manager_internal::PokeModelRendererBuilder*     mpPokeModelRendererBuilder;
  app_rendering_manager_internal::PokeModelRenderingPipeLine*   mpPokeModelRenderingPipeLine[RenderPlaceType::TOTAL_NUM][POKEMODEL_LAYER_NUM_MAX];

  // レーダーチャート描画パス
  app_rendering_manager_internal::RaderChartRenderingPipeLine*  mpRaderChartRenderingPipeLineUpperLeft;
  app_rendering_manager_internal::RaderChartRenderingPipeLine*  mpRaderChartRenderingPipeLineUpperRight;
  app_rendering_manager_internal::RaderChartRenderingPipeLine*  mpRaderChartRenderingPipeLineLower;

  // エフェクト描画パス
  u32                           mEffectLayerNum[RenderPlaceType::NUM];
  app_rendering_manager_internal::EffectRenderingPipeLine*   mpEffectRenderingPipeLine[RenderPlaceType::TOTAL_NUM][EFFECT_LAYER_NUM_MAX];

  // カラーバッファ
  void*   mpColorBufferLiner;
  void*   mpColorBufferBlock;
};


GFL_NAMESPACE_END(util) 
GFL_NAMESPACE_END(app) 


#endif //__APP_RENDERING_MANAGER_H__
