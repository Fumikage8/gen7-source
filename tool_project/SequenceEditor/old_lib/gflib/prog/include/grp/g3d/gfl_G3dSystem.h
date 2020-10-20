#ifndef __GFL_G3DSYSTEM_H__
#define __GFL_G3DSYSTEM_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSystem.h
 *	@brief  3Dシステム
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/ut.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>
#include <gfl_Base.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dObjectManager.h>
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dH3dResource.h>
#include <grp/g3d/gfl_G3dTexture.h>
#include <grp/g3d/gfl_G3dH3dModelMemoryManager.h>


namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	オブジェクトの最大数
//=====================================
enum
{
  SYSTEM_SCENE_ENVIRONMENT_SETTING_OBJECT_NUM_MAX     =  64,  // SceneEnvironmentSettingの最大数
  SYSTEM_SCENE_OBJECT_NUM_MAX                         =   8,  // Sceneの最大数
  SYSTEM_MODEL_OBJECT_NUM_MAX                         = 256,  // Modelの最大数
  SYSTEM_TEXTURE_OBJECT_NUM_MAX                       = 128,  // Textureの最大数
  SYSTEM_FOG_OBJECT_NUM_MAX                           =  64,  // Fogの最大数
  SYSTEM_CAMERA_OBJECT_NUM_MAX                        =  64,  // Cameraの最大数
  SYSTEM_STEREO_CAMERA_OBJECT_NUM_MAX                 =  64,  // StereoCameraの最大数
  SYSTEM_LIGHT_OBJECT_NUM_MAX                         = 256,  // Lightの最大数
  SYSTEM_RES_ANIM_OBJECT_NUM_MAX                      = 256,  // ResAnimの最大数
  SYSTEM_ANIMATION_OBJECT_NUM_MAX                     = 256,  // Animationの最大数
  SYSTEM_UNIT_ANIMATION_OBJECT_NUM_MAX                = 256,  // UnitAnimationの最大数
  SYSTEM_RESOURCE_OBJECT_NUM_MAX                      = 256,  // Resourceの最大数
  SYSTEM_PARTICLE_OBJECT_NUM_MAX                      = 256,  // Particleの最大数
  SYSTEM_H3D_MODEL_OBJECT_NUM_MAX                     = 256   // H3dModelの最大数
};


//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3Dシステムクラス
//=====================================
class System : public gfl::base::Singleton<System>
{
  GFL_FORBID_COPY_AND_ASSIGN(System);


public:
  // @brief  共通データの自動処理タイプ
  enum AutoCommonDataDeal
  {
    AUTO_COMMON_DATA_DEAL_NONE,  // 共通データを自動的に読み込まなし生成しない
    AUTO_COMMON_DATA_DEAL_ALL    // 共通データを自動的に読み込み生成する(共通データが常駐します)
  };

  //#######################################################################################
  //
  //            システムの設定内容
  // 
  //#######################################################################################
  //---------------------------------------------------------------------------
  /**
   *  @brief システムの設定内容構造体
   */
  //---------------------------------------------------------------------------
  struct Description  // maxと付いているのは管理する最大数、付いていないのはその個数生成
  {
    u32  resource_object_num_max;
    u32  scene_object_num_max;
    u32  model_object_num_max;
    u32  light_object_num_max;
    u32  camera_object_num_max;
    u32  stereo_camera_object_num_max;
    u32  fog_object_num_max;
    u32  scene_environment_setting_object_num_max;
    u32  res_anim_object_num_max; 
    u32  animation_object_num_max; 
    u32  unit_animation_object_num_max; 
    u32  texture_object_num_max;
    u32  particle_object_num_max;
    u32  h3d_model_object_num_max;
    AutoCommonDataDeal  auto_common_data_deal;  // 共通データの自動処理タイプ

    // @brief コンストラクタ
    Description(void)
      : resource_object_num_max                  (SYSTEM_RESOURCE_OBJECT_NUM_MAX),
        scene_object_num_max                     (SYSTEM_SCENE_OBJECT_NUM_MAX),
        model_object_num_max                     (SYSTEM_MODEL_OBJECT_NUM_MAX),
        light_object_num_max                     (SYSTEM_LIGHT_OBJECT_NUM_MAX),
        camera_object_num_max                    (SYSTEM_CAMERA_OBJECT_NUM_MAX),
        stereo_camera_object_num_max             (SYSTEM_STEREO_CAMERA_OBJECT_NUM_MAX),
        fog_object_num_max                       (SYSTEM_FOG_OBJECT_NUM_MAX),
        scene_environment_setting_object_num_max (SYSTEM_SCENE_ENVIRONMENT_SETTING_OBJECT_NUM_MAX),
        res_anim_object_num_max                  (SYSTEM_RES_ANIM_OBJECT_NUM_MAX),
        animation_object_num_max                 (SYSTEM_ANIMATION_OBJECT_NUM_MAX),
        unit_animation_object_num_max            (SYSTEM_UNIT_ANIMATION_OBJECT_NUM_MAX),
        texture_object_num_max                   (SYSTEM_TEXTURE_OBJECT_NUM_MAX),
        particle_object_num_max                  (SYSTEM_PARTICLE_OBJECT_NUM_MAX),
        h3d_model_object_num_max                 (SYSTEM_H3D_MODEL_OBJECT_NUM_MAX),
        auto_common_data_deal                    (AUTO_COMMON_DATA_DEAL_ALL)
    {}
  };


public:
  //#######################################################################################
  //
  //           共通データ
  //
  //#######################################################################################
  //---------------------------------------------------------------------------
  /**
   *  @brief  共通データ定義
   */
  //---------------------------------------------------------------------------
  enum CommonDataType
  {
    COMMON_DATA_TYPE_NONE,                     // 未定義。ただのバッファかもしれないし、gfl::grp::g3d::Cameraとかかもしれないし。
    
    // 共通データ 
    COMMON_DATA_TYPE_DEFAULT_SHADER,           // gflデフォルトシェーダ
    COMMON_DATA_TYPE_PARTICLE_DEFAULT_SHADER,  // gflパーティクルデフォルトシェーダ
    COMMON_DATA_TYPE_NW_DEFAULT_SHADER,        // NWデフォルトシェーダ
    COMMON_DATA_TYPE_SMALL_TEXTURE,            // 小さいテクスチャ
#if GFL_GRP_G3D_H3D_USE
    COMMON_DATA_TYPE_H3D_DEFAULT_SHADER,       // gfl h3dデフォルトシェーダ
    COMMON_DATA_TYPE_H3D_SMALL_TEXTURE,        // h3d小さいテクスチャ
#endif  // GFL_GRP_G3D_H3D_USE

    // Ex共通データ
    EX_COMMON_DATA_TYPE_BUFFER_RESOURCE_SHADER,   // バッファからリソースを生成するシェーダ
    EX_COMMON_DATA_TYPE_BUFFER_RESOURCE_TEXTURE,  // バッファからリソースを生成するテクスチャ
    EX_COMMON_DATA_TYPE_RESOURCE_SHADER,          // リソースになっているシェーダ
    EX_COMMON_DATA_TYPE_RESOURCE_TEXTURE          // リソースになっているテクスチャ
  };

  #if 0
/*    共通データ   = システムで持っている共通データ。COMMON_DATA, CommonData, common_dataという名前が付いている。
                   共通リソース(「COMMON_RESOURCE_BIT_名前」)となっているものもあり、それらはResourceのSetupで使用可能。
    Ex共通データ = ユーザが追加した共通データ。EX_COMMON_DATA, ExCommonData, ex_common_dataという名前が付いている。
                   共通リソース(「EX_COMMON_RESOURCE_BIT_名前」)にすることもでき(Ex共通リソースと呼ぶ)、そうした場合はResourceのSetupで使用可能。
                   CommonDataと同様の方法で生成、破棄、アクセスできるので、CommonDataという名前の付いた構造体や関数にも注目すること。*/
  #endif

  //---------------------------------------------------------------------------
  /**
  // @brief  共通データ生成に必要な引数をまとめた構造体
   */
  //---------------------------------------------------------------------------
  struct CommonDataCreateArg
  {
    CommonDataType type;
    void*          ptr;
    u32            param0;
    u32            param1;

  #if 0
    /*+---------------------------------------------+----------+---------------------------------------+-------------------+----------+
    | type                                        | ptr      | param0                                | param1            | 生成物   |
    +---------------------------------------------+----------+---------------------------------------+-------------------+----------+
    | COMMON_DATA_TYPE_NONE                       | NULL     | PARAM_NONE                            | PARAM_NONE        | NULL     |
    +---------------------------------------------+----------+---------------------------------------+-------------------+----------+
    | COMMON_DATA_TYPE_DEFAULT_SHADER             | buffer   | index_in_resource                     | PARAM_NONE        | Resource |
    | COMMON_DATA_TYPE_PARTICLE_DEFAULT_SHADER    | buffer   | index_in_resource                     | PARAM_NONE        | Resource |
    | COMMON_DATA_TYPE_NW_DEFAULT_SHADER          | buffer   | index_in_resource                     | PARAM_NONE        | Resource |
    | COMMON_DATA_TYPE_SMALL_TEXTURE              | NULL     | PARAM_NONE                            | PARAM_NONE        | Texture  |
    | COMMON_DATA_TYPE_H3D_DEFAULT_SHADER         | buffer   | ?                                     | ?                 | Resource |
    | COMMON_DATA_TYPE_H3D_SMALL_TEXTURE          | NULL     | PARAM_NONE                            | PARAM_NONE        | Texture  |
    +---------------------------------------------+----------+---------------------------------------+-------------------+----------+
    | EX_COMMON_DATA_TYPE_BUFFER_RESOURCE_SHADER  | buffer   | Resource::EX_COMMON_RESOURCE_BIT_番号 | index_in_resource | Resource |
    | EX_COMMON_DATA_TYPE_BUFFER_RESOURCE_TEXTURE | buffer   | Resource::EX_COMMON_RESOURCE_BIT_番号 | index_in_resource | Resource |
    | EX_COMMON_DATA_TYPE_RESOURCE_SHADER         | Resource | Resource::EX_COMMON_RESOURCE_BIT_番号 | index_in_resource | Resource |
    | EX_COMMON_DATA_TYPE_RESOURCE_TEXTURE        | Resource | Resource::EX_COMMON_RESOURCE_BIT_番号 | index_in_resource | Resource |
    +---------------------------------------------+----------+---------------------------------------+-------------------+----------+*/
  #endif
    
    // @brief  定数
    enum
    {
      PARAM_NONE   = 0xFFFFFFFF
    };

    // @brief  コンストラクタ
    CommonDataCreateArg(void)
      : type(COMMON_DATA_TYPE_NONE),
        ptr(NULL),
        param0(PARAM_NONE),
        param1(PARAM_NONE)
    {}

    // @brief  コンストラクタ
    CommonDataCreateArg(
        CommonDataType a_type,
        void*          a_ptr,
        u32            a_param0,
        u32            a_param1
    )
      : type(a_type),
        ptr(a_ptr),
        param0(a_param0),
        param1(a_param1)
    {}
  };

  //---------------------------------------------------------------------------
  /**
   *  @brief  共通データを生成する
   *  @retval  生成できたときtrueを返す
   */
  //---------------------------------------------------------------------------
  b32 CreateCommonData(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      CommonDataCreateArg&    common_data_create_arg
  );
  
  //---------------------------------------------------------------------------
  /**
   *  @brief  共通データを破棄する
   *          ptrが指すものそのものの解放は呼び出し元で行って下さい。
   *  @param[in]  ptr  CreateCommonDataで与えたCommonDataCreateArgのptr
   */
  //---------------------------------------------------------------------------
  void DestroyCommonData(
      void* ptr
  );

  //---------------------------------------------------------------------------
  /**
   *  @brief  全ての共通データを破棄する
   *          ptrが指すものそのものの解放は呼び出し元で行って下さい。
   *          CreateCommonDataで生成したものだけ破棄します。AutoCommonDataDealで生成したものは破棄しません。
   */
  //---------------------------------------------------------------------------
  void DestroyAllCommonData(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief  共通データを取得する
   *          CreateCommonDataで生成したものも、AutoCommonDataDealで生成したものも、どちらも取得できます。
   *  @retval       
   */
  //-----------------------------------------------------------------------------
  inline Resource*     GetCommonDataDefaultShaderResource(void) const;
  inline Resource*     GetCommonDataParticleDefaultShaderResource(void) const;
  inline Resource*     GetCommonDataNwDefaultShaderResource(void) const;
  //変更しましたinline Resource*     GetCommonDataSmallTextureResource(void) const;  // L4 8x8 白一色のテクスチャ
  inline Texture*      GetCommonDataSmallTexture(void) const;  // L4 8x8 白一色のテクスチャ
#if GFL_GRP_G3D_H3D_USE
  inline H3dResource*        GetCommonDataH3dDefaultShaderResource(void) const;
  inline H3dDynamicTexture*  GetCommonDataH3dSmallTexture(void) const;  // L4 8x8 白一色のテクスチャ
#endif  // GFL_GRP_G3D_H3D_USE
  //なくしましたinline Resource*     GetCommonDataMiddleTextureResource(void) const;  // 


  //-----------------------------------------------------------------------------
  /**
   *  @brief  共通データを得る
   *  @param[in]  type  CreateCommonDataで与えたCommonDataCreateArgのtypeです。同じtypeのデータが複数ある場合は、一番最初に生成したものを返します。
   *  @retval  生成されたデータ。typeに応じてキャストして使用して下さい。
   */
  //-----------------------------------------------------------------------------
  inline void* GetCommonData(CommonDataType type) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief  共通データを得る
   *  @param[in]  ptr  CreateCommonDataで与えたCommonDataCreateArgのptr
   *  @retval  生成されたデータ。typeに応じてキャストして使用して下さい。
   */
  //-----------------------------------------------------------------------------
  inline void* GetCommonData(void* ptr) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief  「EX_COMMON_DATA_TYPE_BUFFER_RESOURCE_名前」を得る
   *  @param[in]  param0  「CommonDataCreateArg」の「param0」に渡した「Resource::EX_COMMON_RESOURCE_BIT_番号」
   *  @retval  Resource
   */
  //-----------------------------------------------------------------------------
  inline Resource* GetExCommonDataExCommonResource(u32 param0) const;





  //#######################################################################################
  //
  //   g3dリソースデータ
  //   下記リソースをすべて外部指定することで、g3dライブラリ内部でのファイル読み込みをカットすることができる。
  //
  //#######################################################################################
  enum ResourceEnum{
    GFL_DEFAULT_SHADER_BCSDR,           // gflデフォルトシェーダ
    NWGFX_PARTICLEDEFAULTSHADER_BCSDR,  // gflパーティクルデフォルトシェーダ
    NWGFX_DEFAULTSHADER_BCSDR,          // NWデフォルトシェーダ
#if GFL_GRP_G3D_H3D_USE
    GFL_DEFAULTSHADER_BCHSDR,           // gfl h3dデフォルトシェーダ
#endif  // GFL_GRP_G3D_H3D_USE
    BLOOM_FILTER_HIGH_LUMINANCE_BOARD_BCRES,  // ブルームフィルター素材
    BLOOM_FILTER_BLOOM_BOARD_BCRES,           // ブルームフィルター素材
    TEXTURE_CHECK_BOARD_NO_ALPHA_BCRES,       // ブルームフィルター素材
    EDGE_FILLTER_BCRES,                       // エッジフィルター素材
    VCOLOR_EDGE_FILLTER_BCRES,                // エッジフィルター素材
    IDEDGE_FILLTER_BCRES,                     // エッジフィルター素材
    EDGE_FILLTER_NORMAL_BCRES,                // エッジフィルター素材
    INSIDE_EDGE_FILLTER_BCRES,                // エッジフィルター素材
    NORMAL_TEXTURE_BCTEX,                     // エッジフィルター素材
    NORMAL_TO_UV_SHADER_BCSDR,                // エッジフィルター素材
    EDGE_MAP_VCOLA_BCMDL,                     // エッジフィルター素材
    EDGE_MAP_ALPHAVCOLA_BCMDL,                // エッジフィルター素材
    EDGE_MAP_VCOLA_DISABLE_NORMAL_BCMDL,      // エッジフィルター素材
    EDGE_MAP_ALPHA_VCOLA_DISABLE_NORMAL_BCMDL,// エッジフィルター素材
    EDGE_MAP_DISABLE_NORMAL_VCOLA_BCMDL,      // エッジフィルター素材
    EDGE_MAP_ALPHA_DISABLE_NORMAL_VCOLA_BCMDL,// エッジフィルター素材
#if GFL_GRP_G3D_H3D_USE
    NORMAL_TEXTURE_BCTEX_H3D,                     // エッジフィルター素材
    NORMAL_TO_UV_SHADER_BCSDR_H3D,                // エッジフィルター素材
    EDGE_MAP_VCOLA_BCMDL_H3D,                 // エッジフィルター素材
    EDGE_MAP_ALPHAVCOLA_BCMDL_H3D,            // エッジフィルター素材
    EDGE_MAP_VCOLA_DISABLE_NORMAL_BCMDL_H3D,  // エッジフィルター素材
    EDGE_MAP_ALPHA_VCOLA_DISABLE_NORMAL_BCMDL_H3D,  // エッジフィルター素材
    EDGE_MAP_DISABLE_NORMAL_VCOLA_BCMDL_H3D,        // エッジフィルター素材
    EDGE_MAP_ALPHA_DISABLE_NORMAL_VCOLA_BCMDL_H3D,  // エッジフィルター素材
#endif // GFL_GRP_G3D_H3D_USE
#if GFL_DEBUG
    EDGE_DEBUG_RGB_BCRES,                     // エッジフィルター素材（デバッグ時のみ）
    EDGE_DEBUG_A_BCRES,                       // エッジフィルター素材（デバッグ時のみ）
    EDGE_DEBUG_R_BCRES,                       // エッジフィルター素材（デバッグ時のみ）
#endif // GFL_DEBUG

    RESOURCE_MAX,   // g3dシステムが要求するリソース数
  };

  //----------------------------------------------------------------------------
  /**
   *	@brief  g3dライブラリで使用するリソースを外部指定
   */
  //-----------------------------------------------------------------------------
  // 設定
  static void SetStaticResource( void * buff, ResourceEnum resource_type );
  // 取得
  static void* GetStaticResource( ResourceEnum resource_type );

  //----------------------------------------------------------------------------
  /**
   *	@brief  外部指定されたg3dライブラリで使用するリソースを保持ているテーブルをすべてクリア
   *	*メモリの解放は外部で。
   */
  //-----------------------------------------------------------------------------
  static void ClearStaticResourceAll( void );



public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *
   *  @param[in]     heap_allocator    ヒープメモリのアロケータ
   *  @param[in]     device_allocator  デバイスメモリのアロケータ
   *  @param[in]     description       設定内容
   */
  //-----------------------------------------------------------------------------
  System( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description* description = NULL );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   */
  //-----------------------------------------------------------------------------
  ~System();  // 継承は考えていない


  //----------------------------------------------------------------------------
  /**
   *	@brief  更新処理  （SceneのCalculateやSubmitViewの前に読んでください）
   */
  //-----------------------------------------------------------------------------
  void Update(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief        マネージャーを取得する
   *
   *  @retval       ObjectManager*       マネージャー
   */
  //-----------------------------------------------------------------------------
  ObjectManager*   GetSceneEnvironmentSettingManager(void)  {  return m_scene_environment_setting_manager;  }
  ObjectManager*   GetSceneManager(void)                    {  return m_scene_manager;  }
  ObjectManager*   GetModelManager(void)                    {  return m_model_manager;  }
  ObjectManager*   GetParticleManager(void)                 {  return m_particle_manager;  }
  ObjectManager*   GetTextureManager(void)                  {  return m_texture_manager;  }
  ObjectManager*   GetFogManager(void)                      {  return m_fog_manager;  }
  ObjectManager*   GetCameraManager(void)                   {  return m_camera_manager;  }
  ObjectManager*   GetStereoCameraManager(void)             {  return m_stereo_camera_manager;  }
  ObjectManager*   GetLightManager(void)                    {  return m_light_manager;  }
  ObjectManager*   GetResAnimManager(void)                  {  return m_res_anim_manager;  }
  ObjectManager*   GetAnimationManager(void)                {  return m_animation_manager;  }
  ObjectManager*   GetUnitAnimationManager(void)            {  return m_unit_animation_manager;  }
  ObjectManager*   GetResourceManager(void)                 {  return m_resource_manager;  }
  ObjectManager*   GetH3dModelManager(void)                 {  return m_h3d_model_manager;  }

#if GFL_GRP_G3D_H3D_USE
  //-----------------------------------------------------------------------------
  /**
   *  @brief        H3dメモリマネージャを取得する
   */
  //-----------------------------------------------------------------------------
  inline H3dModelMemoryManager* GetH3dModelMemoryManager(void) const { return m_h3d_model_memory_manager; }
#endif  // GFL_GRP_G3D_H3D_USE

  //-----------------------------------------------------------------------------
  /**
   *  @brief        描画環境を初期化する
   */
  //-----------------------------------------------------------------------------
  //使用禁止にしておくvoid ResetRenderState(void);

private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         初期化処理
   */
  //-----------------------------------------------------------------------------
  void initialize(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator); 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         終了処理
   */
  //-----------------------------------------------------------------------------
  void finalize(void); 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         共通データを自動的に読み込み生成する、破棄する
   */
  //-----------------------------------------------------------------------------
  void autoCreateCommonData(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);
  void autoDestroyCommonData(void);

  void autoCreateCommonDataDefaultShaderResource(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);
  void autoDestoryCommonDataDefaultShaderResource(void);
  void autoCreateCommonDataParticleDefaultShaderResource(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);
  void autoDestroyCommonDataParticleDefaultShaderResource(void);
  void autoCreateCommonDataNwDefaultShaderResource(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);
  void autoDestoryCommonDataNwDefaultShaderResource(void);
  void autoCreateCommonDataSmallTexture(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);  // L4 8x8 白一色のテクスチャ
  void autoDestoryCommonDataSmallTexture(void);  // L4 8x8 白一色のテクスチャ
#if GFL_GRP_G3D_H3D_USE
  void autoCreateCommonDataH3dDefaultShaderResource(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);
  void autoDestroyCommonDataH3dDefaultShaderResource(void);
  void autoCreateCommonDataH3dSmallTexture(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);  // L4 8x8 白一色のテクスチャ
  void autoDestoryCommonDataH3dSmallTexture(void);  // L4 8x8 白一色のテクスチャ
#endif  // GFL_GRP_G3D_H3D_USE

  
private:
  // 共通データの自動処理フラグ
  enum
  {
    AUTO_COMMON_DATA_DEAL_BIT_NONE                             = 0,
    AUTO_COMMON_DATA_DEAL_BIT_DEFAULT_SHADER_RESOURCE          = 1 <<  0,
    AUTO_COMMON_DATA_DEAL_BIT_PARTICLE_DEFAULT_SHADER_RESOURCE = 1 <<  1,
    AUTO_COMMON_DATA_DEAL_BIT_NW_DEFAULT_SHADER_RESOURCE       = 1 <<  2,
    AUTO_COMMON_DATA_DEAL_BIT_SMALL_TEXTURE                    = 1 <<  3
#if GFL_GRP_G3D_H3D_USE
    , AUTO_COMMON_DATA_DEAL_BIT_H3D_DEFAULT_SHADER_RESOURCE      = 1 <<  4 
    , AUTO_COMMON_DATA_DEAL_BIT_H3D_SMALL_TEXTURE                = 1 <<  5
#endif  // GFL_GRP_G3D_H3D_USE
  };

  // 共通データ要素
  struct CommonDataElement
  {
    CommonDataCreateArg arg;
    void*               data;  // 生成物

    CommonDataElement*  prev;  // 先頭要素のときはNULL
    CommonDataElement*  next;  // 末尾要素のときはNULL

    CommonDataElement(void)
      : arg(),
        data(NULL),
        prev(NULL),
        next(NULL)
    {}
  };


private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         共通データの生成、破棄
   *                 CommonDataElementの枠は、呼び出し元で生成、破棄して下さい。
   */
  //-----------------------------------------------------------------------------
  static void* createCommonDataSmallTextureBuffer(gfl::heap::NwAllocator* device_allocator);  // L4 8x8 白一色のテクスチャバッファ生成
  static void  destroyCommonDataSmallTextureBuffer(void* buffer);  // L4 8x8 白一色のテクスチャバッファ破棄
  
  static Texture* createCommonDataSmallTexture(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, void* buffer);  // L4 8x8 白一色のテクスチャ
#if GFL_GRP_G3D_H3D_USE
  static H3dDynamicTexture* createCommonDataH3dSmallTexture(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, void* buffer);  // L4 8x8 白一色のテクスチャ
#endif  // GFL_GRP_G3D_H3D_USE
  b32 createCommonData(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      CommonDataElement* elem
  );
  void destroyCommonData(CommonDataElement* elem);


private:
  
  ObjectManager*   m_scene_environment_setting_manager;
  ObjectManager*   m_scene_manager;
  ObjectManager*   m_model_manager;
  ObjectManager*   m_particle_manager;
  ObjectManager*   m_texture_manager;
  ObjectManager*   m_fog_manager;
  ObjectManager*   m_camera_manager;
  ObjectManager*   m_stereo_camera_manager;
  ObjectManager*   m_light_manager;
  ObjectManager*   m_res_anim_manager;
  ObjectManager*   m_animation_manager;
  ObjectManager*   m_unit_animation_manager;
  ObjectManager*   m_resource_manager;
  ObjectManager*   m_h3d_model_manager;

  b32    m_is_initialized;  // 初期化済みならtrue
  bit32  m_auto_common_data_deal_bit;

  void*         m_small_texture_buffer;  // L4 8x8 白一色のテクスチャバッファ
  
  Resource*     m_default_shader_resource;
  Resource*     m_particle_default_shader_resource;
  Resource*     m_nw_default_shader_resource;
  //Resource*     m_small_texture_resource;  // L4 8x8 白一色のテクスチャ
  Texture*      m_small_texture;  // L4 8x8 白一色のテクスチャ

#if GFL_GRP_G3D_H3D_USE
  void*               m_h3d_default_shader_buffer;
  b8                  m_h3d_default_shader_buffer_release_flag;
  H3dResource*        m_h3d_default_shader_resource;
  H3dDynamicTexture*  m_h3d_small_texture;  // L4 8x8 白一色のテクスチャ
  H3dModelMemoryManager* m_h3d_model_memory_manager;  // H3dのモデルメモリマネージャ
#endif  // GFL_GRP_G3D_H3D_USE

  CommonDataElement*  m_common_data_head_elem;  // 先頭要素を指す


  // ユーザー指定リソース
  static void * m_pStaticResourceTbl[RESOURCE_MAX];
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSYSTEM_H__
