//===========================~===========================================
/**
 * @file AppToolTrainerIconRendering.h
 * @date 2015/08/18 15:33:35
 * @author uchida_yuto
 * @brief トレーナーアイコン2D表現処理クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __APP_TOOL_TRAINER_ICON_RENDERING_H_INCLUDED__
#define __APP_TOOL_TRAINER_ICON_RENDERING_H_INCLUDED__
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
#include "AppLib/include/Util/app_util_2d.h"
#include "AppLib/include/Util/AppUtilFileControlState.h"

//  トレーナータイプ
#include "niji_conv_header/trainer/trtype_def.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(System)

  class nijiAllocator;

GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class AppRenderingManager;
  class Heap;
  class G2DUtil;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  //  ローカル用の前方定義
  GFL_NAMESPACE_BEGIN(localPlayerIconManager)

    class TrainerIconObject;

  GFL_NAMESPACE_END(localPlayerIconManager)

  /**
   * @class AppToolTrainerIconTexture
   * @brief トレイナーアイコン2Dテクスチャークラス
   * @note
   *        用途としてはロードしたテクスチャーデータを使う側が保存して再ロードしないようにするため
   *        １テクスチャーデータ分のメモリが必要
  */
  class AppToolTrainerIconTexture
  {
    GFL_FORBID_COPY_AND_ASSIGN(AppToolTrainerIconTexture);

  public:
    AppToolTrainerIconTexture( gfl2::heap::HeapBase* pHeap );
    ~AppToolTrainerIconTexture();

    //  画像ペインに作成したテクスチャーを設定
    bool ReplacePicPane( gfl2::lyt::LytPicture* pRqplacePane );

    //  データを転送
    void CopyData( void* pData, const u32 size );

    //  テクスチャーインフォからデータ生成
    void CreateTex( void* pTextureBuffer, gfl2::lyt::LytResTextureInfo& rTexInfo );

    //  作成済みか
    bool IsValid() { return m_bCreateData; }

    //  テクスチャデータ取得
    void* GetTexData() { return m_pTexData; }

  private:

    void _Clear()
    {
      m_pTexData    = NULL;
      m_bCreateData = false;
      m_memSize     = 0;
    }

    void*                         m_pTexData;
    gfl2::lyt::LytResTextureInfo  m_texInfo;  // テクスチャ情報

    u32                           m_memSize;
    bool  m_bCreateData;
  };

  /**
   * @class AppToolTrainerIconRendering
   * @brief トレイナーアイコン2D表現処理クラス
   *      使用の前提条件
   *         1:  使用メモリが大きい
   *             具体的なヒープサイズは下記static関数で取得してください。
   *               システム -> AppToolTrainerIconRendering::GetUseSystemHeapSize()
   *               デバイス -> AppToolTrainerIconRendering::GetUseDeviceHeapSize()
   *
   *               少なくとも今は最低でもデバイスヒープは10MByte以上は必要です。
   *               そして表示するアイコンが１つ増えるたびに追加で1Mbyteが必要と考えてください。
   *
   *               使用サイズが確定できません。
   *               理由としては各リソースのデータサイズが確定できないからです。
   *
   *         2:  使う側では用意してもらう AppRenderingManagerの設定について
   *             ・モデルのレンダーターゲットテクスチャを扱うにはオフスクリーンの設定が必要
   *               
   *               オフスクリーンターゲットのオーダーにモデル設定をする
   *               appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN].renderOrder  = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
   *
   *     使い方
   *        使用初回準備(インスタンス生成からインスタンスを使えるまでの流れ)
   *          1: AppToolTrainerIconRenderingのインスタンスを生成
   *          2: AppToolTrainerIconRendering::Initialize()関数に必要なパラメータを渡して呼ぶ
   *               アイコン表示する個数を設定をしてください。
   *               この関数は一度しか呼んではいけません。
   *               連続で呼ぶと動作保障できません。
   *               
   *          3: AppToolTrainerIconRendering::IsReady()関数を呼んで true が引けるまで呼び続ける
   *
   *        使用中
   *          ※ AppToolTrainerIconRendering::Update()関数は必ず毎フレーム呼ぶ
   *
   *         トレーナーアイコン表示設定
   *             AppToolTrainerIconRendering::SetIconObjectData()関数に必要なパラメータを渡すと貼り付けるデータのリソースを読み込んで画像ペインに貼り付けます
   *             ※リソース読み込み中はこちらで貼り付ける対象の画像ペインは非表示にしています。
   *
   *        使用破棄(後始末からインスタンス破棄までの流れ)
   *          1: AppToolTrainerIconRendering::Termiate()関数を一度呼ぶ
   *              この関数を呼んだ段階で破棄と見なします。
   *              そしてこれは一度しか呼んではいけません。
   *              連続で呼ぶと動作保障できません。
   *
   *          2: AppToolTrainerIconRendering::IsDelete()関数を呼んで true を引くまで呼ぶ続ける
   *
   *          3: インスタンスを破棄する
   *
   *     ※Updata()
   */
  class AppToolTrainerIconRendering
  {
    GFL_FORBID_COPY_AND_ASSIGN(AppToolTrainerIconRendering);

  public:
    //  主人公トレーナIDを取得
    static u32 GetHeroTrainerId( const u32 sex );

    //  トレーナーモデルのArcID取得
    static gfl2::fs::ArcFile::ARCID GetTrainerModelArcId();

    //  トレーナーIDからモデルリソースid取得
    static u32 ConvTrainerIdToModelResID( const u32 trainerId );

    //  トレーナーIDからモデルの待機モーションアニメid取得
    static u32 ConvTrainerIdToModelWaitMotionAnimeId( const u32 trainerId );

    //  主人公トレーナーアイコンのみのアイコンデータ設定
    struct HERO_TRAINER_ICON_DATA
    {
      HERO_TRAINER_ICON_DATA()
      {
        animeId       = 0;
        animeFrameNum = 0;
        camFovy       = 0.0f;
        eyeIndex      = 0;
        bDressDefault = false;
      }

      //  着せ替え
      poke_3d::model::DressUpParam dressUpParam;

      //  きせかえをデフォルトにするか
      bool  bDressDefault;

      //  モーションid
      s32 animeId;
      //  モーションフレーム値
      u32 animeFrameNum;

      //  キャラの顔情報
      u8  eyeIndex;

      //  カメラ情報
      //  カメラ座標
      gfl2::math::Vector3 camPos;
      //  カメラの画角
      f32 camFovy;

      //  キャラRT情報
      gfl2::math::Vector3 modelPos;
      gfl2::math::Vector3 modelRot;
    };

    //  アイコン設定データ
    struct ICON_OBJECT_DATA
    {
      ICON_OBJECT_DATA()
      {
        trainerId = 0;
        pReplacePane  = NULL;
        pSyncVisiblePane = NULL;
        pInLoadVisibleChangePane  = NULL;
        pTimerIconRootPane        = NULL;
      }

      //  貼り付けるペイン
      gfl2::lyt::LytPicture* pReplacePane;

      //  Visible状態を同期させるペイン
      gfl2::lyt::LytPane* pSyncVisiblePane;

      //  ロード中はvisibleをfalseにロード終了したらvisibleをtrueにしたいペイン
      gfl2::lyt::LytPane* pInLoadVisibleChangePane;

      //  タイマーアイコンのペインの親にするペイン
      //  このペインを使う時は親のペインの生存に注意する。
      gfl2::lyt::LytPane* pTimerIconRootPane;

      //  データをコンバート

      //  セーブデータのMyStatusからアイコン設定データをコンバート
      //  MyStatusのアイコンデータ変数を直接使って変換
      void ConvFromMyStatusIconData( const Savedata::MyStatus::ICON_DATA& rIcon );

      //  trainerId / heroTrainerDataの内容が更新
      void ConvFromMyStatus( const Savedata::MyStatus& rSavedataMyStatus );

      //  データからアイコンセーブデータへ変換
      void ConvToMyStatusIcon( Savedata::MyStatus::ICON_DATA* pOutPutSavedataMyStatusIcon );

      //  トレーナーID
      s32 trainerId;

      //  以下のデータは主人公のトレーナーの時のみ使う
      HERO_TRAINER_ICON_DATA  heroTrainerData;
    };

    //  MyStatus::ICON_DATAのデフォルトデータ作成
    static void OutputDefaultMyStatusIconData( const u32 sex, Savedata::MyStatus::ICON_DATA* pOutputData );

    //  ICON_OBJECT_DATAのデフォルトデータ作成
    static void OutputDefaultIconObjectData( const u32 trainerId, ICON_OBJECT_DATA* pOutputData );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      クラスが使うシステムヒープサイズ取得
    * @param[in]  drawIconNum: 表示するアイコン数
    * @param[in]  NPCアイコンのみのか
    * @return     クラスに必要なヒープサイズ
    */
    //--------------------------------------------------------------------------------------------
    static u32 GetUseSystemHeapSize( const u32 drawIconNum, const bool bNPCOnly = false );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      クラスが使うデバイスヒープサイズ取得
    * @param[in]  drawIconNum: 表示するアイコン数
    * @return     クラスに必要なヒープサイズ
    */
    //--------------------------------------------------------------------------------------------
    static u32 GetUseDeviceHeapSize( const u32 drawIconNum, const bool bNPCOnly = false );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      コンストラクタ
    */
    //--------------------------------------------------------------------------------------------
    AppToolTrainerIconRendering() { _Clear(); }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      デストラクタ
    */
    //--------------------------------------------------------------------------------------------
    ~AppToolTrainerIconRendering();

    //-----------------------------------------------------------------------------
    /**
     * @brief 初期化
     * @param[in]  pRenderingManager:  内部でオフスクリーンを使うのに必要( 内部で参照しているのでアドレス先を破棄してはいけない )
     * @param[in]  pHeap:              内部で使用するヒープ( 内部で参照しているのでアドレス先を破棄してはいけない )
     * @param[in]  pFileManager:       ファイルロード用( 内部で参照しているのでアドレス先を破棄してはいけない )
     * @param[in]  iconMax:            管理するアイコン最大数
     * @param[in]  displayType:        アイコンの描画先を指定
     * @note
     *      必ず最初に実行しないといけない
     */
    //-----------------------------------------------------------------------------
    void Initialize(
     app::util::AppRenderingManager* pRenderinManager, app::util::Heap* pHeap,
     gfl2::fs::AsyncFileManager* pFileManager, const u32 iconMax, app::util::G2DUtil::SETUP_DISPLAY_TYPE displayType );

    //  NPCアイコンの表示だけの場合はこちらを使うとモデルが使わないのメモリが少なくてすむ
    void InitializeByNPCOnly(
     app::util::AppRenderingManager* pRenderinManager, app::util::Heap* pHeap,
     gfl2::fs::AsyncFileManager* pFileManager, const u32 iconMax, app::util::G2DUtil::SETUP_DISPLAY_TYPE displayType );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      クラス使用終了通知
    * @note
    *             クラスを破棄する前には必ずこれを実行
    */
    //--------------------------------------------------------------------------------------------
    void Terminate();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      クラス使用可能か
    * @return     使用不可能 false / 使用可能 true
    * @note
    *             Initialize()関数を呼んだ後に true が返ってくるまで呼び続ける
    */
    //--------------------------------------------------------------------------------------------
    bool IsReady();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      クラス破棄可能か
    * @return     破棄不可能 false / 破棄可能 true
    * @note
    *             Terminate()関数を呼んだ後に true が返ってくるまで呼び続ける
    */
    //--------------------------------------------------------------------------------------------
    bool IsDelete();

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
    * @brief      描画
    */
    //--------------------------------------------------------------------------------------------
    void Draw( gfl2::gfx::CtrDisplayNo displayNo );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      タイマーアイコンオブジェクトのレイアウト設定
    * @param[in]  layoutOrder:     タイマーアイコンレイアウトの描画優先度
    * @param[in]  layoutNo:        タイマーアイコンレイアウトのNO
    */
    //--------------------------------------------------------------------------------------------
    void SetLayoutParamTimerIconObject( const u32 layoutOrder, const u32 layoutNo );
    
    //--------------------------------------------------------------------------------------------
    /**
    * @brief      アイコンオブジェクト設定
    * @param[in]  idx:                設定するアイコンオブジェクトidx
    * @param[in]  rObjectData:        オブジェクトデータ
    * @param[in]  bVisibleTimerIcon:  ロードする時にTimerIconを出すか
    */
    //--------------------------------------------------------------------------------------------
    void SetIconObjectData( const u32 idx, const ICON_OBJECT_DATA& rObjectData, bool bVisibleTimerIcon = true );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      アイコンオブジェクト設定
    * @param[in]  idx:                設定するアイコンオブジェクトidx
    * @param[in]  rObjectData:        オブジェクトデータ
    * @param[in]  rTimerIconPos:      タイマーアイコンの表示位置
    * @param[in]  bVisibleTimerIcon:  ロードする時にTimerIconを出すか
    * @note
    *             下記の設定したペインと位置を同期させずに rTimerIconPos引数で設定した固定位置にタイマーアイコンを表示させる
    *             rObjectData.pReplacePane
    */
    //--------------------------------------------------------------------------------------------
    void SetIconObjectData( const u32 idx, const ICON_OBJECT_DATA& rObjectData, const gfl2::math::Vector3& rTimerIconPos, bool bVisibleTimerIcon = true );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      アイコンオブジェクト設定
    * @param[in]  idx:                設定するアイコンオブジェクトidx
    * @param[in]  rObjectData:        オブジェクトデータ
    * @param[in]  bVisibleTimerIcon:  ロードする時にTimerIconを出すか
    * @param[out] pOutputTexture:     ロードした時のテクスチャーデータ出力
    * @note
    *       ロードしたテクスチャーデータを受け取る関数
    *       使用側で再ロードする必要がない時に使える(特にリスト関連は)
    */
    //--------------------------------------------------------------------------------------------
    void SetIconObjectDataByOutputTexture( const u32 idx, const ICON_OBJECT_DATA& rObjectData, bool bVisibleTimerIcon, AppToolTrainerIconTexture* pOutputTexture );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      アイコンオブジェクトロードが終っているか
    * @param[in]  idx: チェックするアイコンオブジェクトidx
    */
    //--------------------------------------------------------------------------------------------
    bool IsLoadedObject( const u32 idx );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      アイコンオブジェクトデータ取得
    * @param[in]  idx: 表示するアイコンオブジェクトidx
    * @return     アイコンオブジェクト
    */
    //--------------------------------------------------------------------------------------------
    ICON_OBJECT_DATA* GetIconObjectData( const u32 idx );

  private:
    void _SetIconObjectDataBasic( const u32 idx, const ICON_OBJECT_DATA& rObjectData, bool bVisibleTimerIcon, const bool bTimerIconFixPos );

    //  変数のクリア
    void _Clear()
    {
      m_pIconObjectArray                = NULL;
      m_pVramAllocator                  = NULL;
      m_pRenderinManager                = NULL;
      m_pFileManager                    = NULL;
      m_pLocalHeap                      = NULL;
      m_pTimerIconLayoutFileLoadState   = NULL;
      m_pModelFactoryHeap               = NULL;
      m_pModelResourceManagerHeap       = NULL;
      m_pCharaModelLightDrawEnv         = NULL;
      m_pVramHeap                       = NULL;

      m_iconObjectArrayNum        = 0;
      m_readySeq                  = 0;
      m_terminateSeq              = 0;

      m_bTerminate                = false;
      m_bReady                    = false;
      m_bNPCOnly                  = false;

      m_displayType               = app::util::G2DUtil::SETUP_LOWER;
    }

    //  アイコンオブジェクトリスト
    localPlayerIconManager::TrainerIconObject*                m_pIconObjectArray;
    u32                                                       m_iconObjectArrayNum;

    //  描画テクスチャをVRAM使用する時
    System::nijiAllocator*                                    m_pVramAllocator;

    app::util::AppRenderingManager*                           m_pRenderinManager;     //  内部で参照しているのでアドレス先を破棄してはいけない
    gfl2::fs::AsyncFileManager*                               m_pFileManager;
    app::util::Heap*                                          m_pLocalHeap;

    //  VRAM専用のヒープ
    gfl2::heap::HeapBase*                                     m_pVramHeap;

    //  タイマーアイコンのレイアウトロード
    app::util::AppUtilFileControlState*                       m_pTimerIconLayoutFileLoadState;

    //  モデル管理
    gfl2::heap::HeapBase*                                     m_pModelFactoryHeap;
    poke_3d::model::CharaModelFactory                         m_charaModelFactory;
    gfl2::heap::HeapBase*                                     m_pModelResourceManagerHeap;
    poke_3d::model::DressUpModelResourceManager               m_dressUpModelResourceManager;

    //  アイコンの描画先
    app::util::G2DUtil::SETUP_DISPLAY_TYPE                    m_displayType;

    //  キャラ用のライト
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pCharaModelLightDrawEnv;

    bool                                                      m_bReady;
    u32                                                       m_readySeq;

    bool                                                      m_bTerminate;
    u32                                                       m_terminateSeq;

    bool                                                      m_bNPCOnly;
  };

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

#endif // __APP_TOOL_TRAINER_ICON_RENDERING_H_INCLUDED__