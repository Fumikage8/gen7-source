//======================================================================
/**
 * @file AppToolCharaSimpleModel.h
 * @date 2015/09/01 20:23:54
 * @author uchida_yuto
 * @brief キャラのモデル表示を簡易版
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 * @note
 *      現状バトル用のキャラモデル読み込みしか対応していない
 */
//======================================================================

#if !defined __APP_TOOL_CHARA_SIMPLE_MODEL_H_INCLUDED__
#define __APP_TOOL_CHARA_SIMPLE_MODEL_H_INCLUDED__
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <model/include/gfl2_CharaModelFactory.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_std_string.h>

//  nijiのインクルード
#include "AppLib/include/Util/AppRenderingManager.h"

#include "System/include/DressUp.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class Heap;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(System)

  class nijiAllocator;

GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

  //-------------------------------------------------------------------------
  /**
    * @brief キャラモデル
    */
  //-------------------------------------------------------------------------
  class CharaSimpleModel
  {
  public:
    enum HeapSize
    {
      //  クラス使用時の最低限のヒープサイズ（外部からモデルクラスを使うのに最低でもこれだけ必要）
      //  モデルのインスタンス生成と作業用のメモリ確保
      //  allcateにも使っている
      USE_ALLOCATE_HEAP_SIZE  = System::DressUp::BATTLE_HEAP_SIZE_ALLOCATOR,
      USE_HEAP_SIZE    = ( 1024 * 256 ) + System::DressUp::BATTLE_HEAP_SIZE_INSTANCE + USE_ALLOCATE_HEAP_SIZE,

      //  3Dのキャラモデル1体のヒープサイズ
      ONE_MODEL_BIN_HEAP_SIZE       = System::DressUp::BATTLE_HEAP_SIZE_ANIM,
      //  キャラモデル1体のパーツヒープサイズ
      ONE_MODEL_DRESS_BIN_HEAP_SIZE = System::DressUp::BATTLE_HEAP_SIZE_MODEL,

      //  クラスを使用時に必要なヒープフルサイズ（クラス内部でキャラリソース作成を行う場合はこれだけ必要）
      USE_FULL_HEAP_SIZE  = USE_HEAP_SIZE + ONE_MODEL_BIN_HEAP_SIZE + ONE_MODEL_DRESS_BIN_HEAP_SIZE
    };

    enum StateType
    {
      STATE_TYPE_IDLE   = 0,
      STATE_TYPE_LOAD,
      STATE_TYPE_UPDATE,
      STATE_TYPE_END,
    };

    enum
    {
      CHARA_ID_NULL   = 0xffffffff,
      ANIME_ID_NULL   = 0xffffffff
    };

    //  セットアップパラメータ
    struct SETUP_PARAM
    {
      SETUP_PARAM()
      {
        pUseHeap                      = NULL;
        pRenderingManager             = NULL;

        {
          u32 loopNum = GFL_NELEMS( aRenderPlaceType );
          for( u32 i = 0; i < loopNum; ++i )
          {
            aRenderPlaceType[ i ] = app::util::AppRenderingManager::RenderPlaceType::NUM;
            layerNo[ i ] = 0;
          }
        }
      };

      gfl2::heap::HeapBase*                                 pUseHeap;
      app::util::AppRenderingManager*                       pRenderingManager;
      app::util::AppRenderingManager::RenderPlaceType::Tag  aRenderPlaceType[ app::util::AppRenderingManager::RenderPlaceType::NUM ];
      u32 layerNo[ app::util::AppRenderingManager::RenderPlaceType::NUM ];
    };

    //  モデル生成パラメータ
    struct SETUP_CREATE_MODEL_PARAM
    {
      SETUP_CREATE_MODEL_PARAM()
      {
        pCharaModelFactory            = NULL;
        pDressUpModelResourceManager  = NULL;
      };

      poke_3d::model::CharaModelFactory*                    pCharaModelFactory;
      poke_3d::model::DressUpModelResourceManager*          pDressUpModelResourceManager;
    };

    //  性別からバトルのキャラid取得
    static u32 GetCharaBattleCharaId( const u32 sex );

    //  性別からバトルのキャラの待機アニメid取得
    static u32 GetCharaBattltAnimeWaitId( const u32 sex );
    
    //-------------------------------------------------------------------------
    /**
      * @brief コンストラクタ
      */
    //-------------------------------------------------------------------------
    CharaSimpleModel();

    //-------------------------------------------------------------------------
    /**
      * @brief デストラクタ
      */
    //-------------------------------------------------------------------------
    ~CharaSimpleModel();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      セットアップ
    * @param[in]  rSetupParam:  セットアップに必要なパラメータ
    * @param[in]  pFileManager: モデルロード時のファイル管理
    * @param[in]  modelArcId:   キャラモデルのarcID
    * @param[in]  dressupArcIdList: 着せ替えモデルのarcIDリスト
    * @note
    *   モデル生成に必要なパラメータは内部で確保
    */
    //--------------------------------------------------------------------------------------------
    void Setup( SETUP_PARAM& rSetupParam, gfl2::fs::AsyncFileManager* pFileManager, const gfl2::fs::ArcFile::ARCID modelArcId, s32* dressupArcIdList );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      セットアップ
    * @param[in]  rSetupParam:            セットアップに必要なパラメータ
    * @param[in]  rSetupCreateModelParam: モデル生成の外部設定パラメータ
    * @note
    *   この関数はモデル生成に必要なパラメータを外部設定用に用意
    */
    //--------------------------------------------------------------------------------------------
    void Setup( SETUP_PARAM& rSetupParam, SETUP_CREATE_MODEL_PARAM& rSetupCreateModelParam );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      ロード
    */
    //--------------------------------------------------------------------------------------------
    void StartLoad( const s32 charaId, const poke_3d::model::DressUpParam& rDressUpParam );

    //------------------------------------------------------------------
    /**
      * @brief   終了処理
      */
    //------------------------------------------------------------------
    void Terminate();

    //-------------------------------------------------------------------------
    /**
      * @brief 破棄可能か
      */
    //-------------------------------------------------------------------------
    bool IsDelete();

    //-------------------------------------------------------------------------
    /**
      * @brief 更新処理
      * @note
      *   使う時は毎フレーム呼び出す
      */
    //-------------------------------------------------------------------------
    void Update();

    //------------------------------------------------------------------
    /**
      * @brief    ステート取得
      * @return   現在のステート
      */
    //------------------------------------------------------------------
    StateType GetState() const;

    //------------------------------------------------------------------
    /**
      * @brief   破棄させる
      * @note
      *         Terminate()のように完全破棄ではなくメモリなど一部破棄する
      */
    //------------------------------------------------------------------
    void RequestFree();

    //------------------------------------------------------------------
    /**
      * @brief   アニメ再生停止
      */
    //------------------------------------------------------------------
    void SetStopAnime( const bool bFlg );

    //------------------------------------------------------------------
    /**
      * @brief   モーションアニメ再生設定
      */
    //------------------------------------------------------------------
    void SetMotionAnime( const s32 animeId, const u32 frameNum, const bool bLoop );

    //------------------------------------------------------------------
    /**
      * @brief   目パチ設定
      */
    //------------------------------------------------------------------
    void SetEye( const poke_3d::model::CharaModel::EyeIndex eyeIndex );

    //------------------------------------------------------------------
    /**
      * @brief   表示有効設定
      */
    //------------------------------------------------------------------
    void SetVisible( const bool bFlg );

    //------------------------------------------------------------------
    /**
      * @brief   使える準備ができているかどうか
      */
    //------------------------------------------------------------------
    bool IsReady();

    //------------------------------------------------------------------
    /**
      * @brief   目パチアニメ再生
      */
    //------------------------------------------------------------------
    void PlayEyeAnime();

    //------------------------------------------------------------------
    /**
      * @brief   現在の目パチ取得
      */
    //------------------------------------------------------------------
    poke_3d::model::CharaModel::EyeIndex GetEye();

    //------------------------------------------------------------------
    /**
      * @brief   セットアップパラメータ取得
      */
    //------------------------------------------------------------------
    const SETUP_PARAM&  GetSetupParam() const { return m_setupParam; }

    //------------------------------------------------------------------
    /**
      * @brief   ロードしようとしているキャラIdを取得
      * @note
      *         CHARA_ID_NULL値以外が入っているとロードの準備待ちと考える
      */
    //------------------------------------------------------------------
    u32 GetLoadCharaId() const;

    //------------------------------------------------------------------
    /**
      * @brief   キャラのきせかえデータ取得
      */
    //------------------------------------------------------------------
    const poke_3d::model::DressUpParam& GetDressUpParam() const { return m_loadDressUpParam; }

    //------------------------------------------------------------------
    /**
      * @brief   座標設定
      */
    //------------------------------------------------------------------
    void SetPos( const gfl2::math::Vector3& rPos );

    //------------------------------------------------------------------
    /**
      * @brief   回転のラジアン設定
      */
    //------------------------------------------------------------------
    void SetRot( const gfl2::math::Vector3& rRot );

    //  モデルの座標と回転を取得
    const gfl2::math::Vector3& GetPos() const { return m_pos; }
    const gfl2::math::Vector3& GetRot() const { return m_rot; }

    //  モデルが再生しているモーションアニメとフレーム数を取得
    u32 GetMotionAnimeId();
    u32 GetNowMotionAnimeFrame();

    //------------------------------------------------------------------
    /**
      * @brief   きせかえのリソースコア取得
      */
    //------------------------------------------------------------------
    poke_3d::model::DressUpModelResourceManagerCore* GetDressUpModelResourceManagerCore( const poke_3d::model::DressUpParam::Sex sex );

    //  きせかえの組み合わせ可能かチェック
    bool CheckDressUpCombination( const poke_3d::model::DressUpParam& rDressUpParam );

    //  着せ替えの組み合わせで問題ないデータを出力
    void OutPutDressUpCombination( poke_3d::model::DressUpParam* pDressUpParam );

    //  基本のモデルクラスを取得
    poke_3d::model::BaseModel& GetBaseModel();

  private:
    //------------------------------------------------------------------
    /**
      * @brief    共通セットアップ
      * @return   成功: true / 失敗: false
      */
    //------------------------------------------------------------------
    bool _SetupBase( SETUP_PARAM& rSetupParam );

    //------------------------------------------------------------------
    /**
      * @brief   状態設定
      */
    //------------------------------------------------------------------
    void _SetState( const StateType newStateType );

    //------------------------------------------------------------------
    /**
      * @brief   モーションアニメフレーム設定
      */
    //------------------------------------------------------------------
    void _SetMotionAnimeFrame( const s32 animeId, const u32 frameNum, const bool bLoop );

    //------------------------------------------------------------------
    /**
      * @brief   目パチアニメ再生
      */
    //------------------------------------------------------------------
    void _PlayEyeAnime();

    //  変数クリア
    void _Clear()
    {
      m_arcId               = gfl2::fs::ArcFile::ARCID_NULL;

      m_state               = STATE_TYPE_IDLE;
      m_charaId             = CHARA_ID_NULL;
      m_motionAnimeId       = ANIME_ID_NULL;
      m_eyeIndex            = poke_3d::model::CharaModel::EYE_INDEX_NONE;
      m_motionAnimeFrameNum = 0;
      m_subStep             = 0;
      m_seqDelete           = 0;

      m_bTerminate        = false;
      m_bVisible          = false;
      m_bAnimeLoop        = false;
      m_bCreateModelRes   = false;
      m_bAnimeLoop        = false;
      m_bAnimeStop        = false;
      m_bAutoEyeAnime     = false;
      m_bUnloadCharaModel = true;
      m_bDestroyModel     = true;
      m_bRemoveModel      = true;
      m_bUnloadParts      = true;
      m_bLoad             = false;

      m_loadCharId  = CHARA_ID_NULL;
      m_pHeapBase           = NULL;
      m_pAllocateHeap       = NULL;
      m_pModelAllocator     = NULL;

      m_pModelFactoryHeap         = NULL;
      m_pModelResourceManagerHeap = NULL;

      m_delCnt  = 0;
    }

    gfl2::fs::ArcFile::ARCID                        m_arcId;

    StateType                                       m_state;

    s32                                             m_charaId;
    s32                                             m_motionAnimeId;
    u32                                             m_motionAnimeFrameNum;
    poke_3d::model::CharaModel::EyeIndex            m_eyeIndex;

    u32                                             m_seqDelete;

    u8                                              m_subStep;


    bool                                            m_bTerminate;
    bool                                            m_bVisible;
    bool                                            m_bAnimeLoop;
    bool                                            m_bCreateModelRes;
    bool                                            m_bAnimeStop;
    bool                                            m_bAutoEyeAnime;
    bool                                            m_bUnloadCharaModel;
    bool                                            m_bDestroyModel;
    bool                                            m_bRemoveModel;
    bool                                            m_bUnloadParts;
    bool                                            m_bLoad;

    //  モデル座標
    gfl2::math::Vector3                             m_pos;
    //  モデル回転
    gfl2::math::Vector3                             m_rot;
    
    s32                                             m_loadCharId;
    poke_3d::model::DressUpParam                    m_loadDressUpParam;

    //  内部で使うヒープ(内部で作成してメモリを割り当て)
    gfl2::heap::HeapBase*               m_pHeapBase;
    gfl2::heap::HeapBase*               m_pAllocateHeap;

    //  モデル
    poke_3d::model::DressUpModel        m_dressUpModel;
    poke_3d::model::DressUpParam        m_dressUpParam;

    //  モデル生成で使用するアロケータ
    gfl2::gfx::IGLAllocator*            m_pModelAllocator;

    u32                                 m_delCnt;

    SETUP_PARAM                         m_setupParam;
    SETUP_CREATE_MODEL_PARAM            m_setupCreateModelParam;

    gfl2::heap::HeapBase*               m_pModelFactoryHeap;
    gfl2::heap::HeapBase*               m_pModelResourceManagerHeap;
  };

GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

#endif // __APP_TOOL_CHARA_SIMPLE_MODEL_H_INCLUDED__