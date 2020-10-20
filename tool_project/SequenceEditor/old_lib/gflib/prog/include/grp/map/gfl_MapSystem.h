#ifndef __GFL_MAPSYSTEM_H__
#define __GFL_MAPSYSTEM_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_MapSystem.h
 *	@brief  マップ管理システム
 *	@author	tomoya takahashi
 *	@date		2011.01.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once


#include <gfl_Common.h>
#include <gfl_Heap.h>
#include <gfl_Math.h>
#include <gfl_Fs.h>
#include <grp/g3d/gfl_G3dScene.h>
#include <grp/g3d/gfl_G3dResource.h>
#include <gfl_Base.h>

#include <grp/map/gfl_MapFileControl.h>
#include <grp/map/gfl_MapDivLoad.h>


namespace gfl{
namespace grp{
namespace map{


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *    基底クラス
 */
//-----------------------------------------------------------------------------
class System
{
  GFL_FORBID_COPY_AND_ASSIGN( System );

public:

  enum ModelMode{
    MODEL_DEFAULT,    ///< gfl::g3d::Modelクラスを使用
    MODEL_H3D,        ///< gfl::g3d::H3dModelクラスを使用
  };
  
  //-------------------------------------
  ///	環境データ
  //=====================================
  struct SetUpParam{
    u32 mapDataHeapSize;                  // マップデータバッファByteSize
    u32 texVramSize;                      // テクスチャVRAM ByteSize　（VRAMに配置しない＝０）
    u32 plttVramSize;                      // テクスチャVRAM ByteSize　（VRAMに配置しない＝０）
    const FileControlArray* cpFileControls;  // ファイルコントロール配列
    u32 divLoadSize;                      // 分割読み込み　1回の読み込みByteSize
    DivLoad::Mode divMode;                // 分割読み込みモード
    gfl::base::Thread::Priority threadPri;// スレッド優先順位
    ModelMode     modelMode;
  };

  enum LoadStatus{
    LOAD_STATE_IDLING = 0,        // 待機中
    LOAD_STATE_START,             // 読み込み開始
    LOAD_STATE_LOAD,              // 読み込み中
    LOAD_STATE_SETUP,             // セットアップ中
    LOAD_STATE_END,               // 完了（データが入っている状態）

    ENUM_FORCE_DWORD(LoadStatus)
  };


  //-------------------------------------
  ///	登録可能　グローバルテクスチャリソース数
  //=====================================
  static const u32 GLOBAL_RESOURCE_MAX = 8;


public:

  //----------------------------------------------------------------------------
  /**
   *	@brief  コンストラクタ
   */
  //-----------------------------------------------------------------------------
  System( void );
    
  //----------------------------------------------------------------------------
  /**
   *	@brief  コンストラクタ　（初期化こみ）
   *
   *	@param	param         環境データ
   *	@param  p_scene       シーンシステム
   *	@param  p_sysHeap     システム制御に使用するヒープ
   *	@param  p_deviceHeap  デバイスヒープ
   */
  //-----------------------------------------------------------------------------
  System( const SetUpParam& param, gfl::grp::g3d::Scene* p_scene, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap );

  //----------------------------------------------------------------------------
  /**
   *	@brief  デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~System();
  

  //----------------------------------------------------------------------------
  /**
   *	@brief  初期化
   *
   *	@param	param         環境データ
   *	@param  p_scene       シーンシステム
   *	@param  p_sysHeap     システム制御に使用するヒープ
   *	@param  p_deviceHeap  デバイスヒープ
   */
  //-----------------------------------------------------------------------------
  virtual void Initialize( const SetUpParam& param, gfl::grp::g3d::Scene* p_scene, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap );

  //----------------------------------------------------------------------------
  /**
   *	@brief  破棄
   */
  //-----------------------------------------------------------------------------
  virtual void Finalize( void );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  更新処理
   */
  //-----------------------------------------------------------------------------
  virtual void Update( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  描画更新処理
   *
   *	Lodを入れるなど。
   */
  //-----------------------------------------------------------------------------
  virtual void UpdateDraw( gfl::grp::g3d::Camera* p_camera );

  //----------------------------------------------------------------------------
  /**
   *	@brief  読み込みリクエスト
   *
   *	@param	dataID        データID
   *	@param	divLoadFlag   分割読み込みするか。　（true=分割　false=一括）
   *
   *	@param  true  リクエスト成功
   *	@param  false リクエスト失敗
   */
  //-----------------------------------------------------------------------------
  virtual bool ReqLoadData( u32 dataID, b32 divLoadFlag );

  //----------------------------------------------------------------------------
  /**
   *	@brief  読み込みリクエスト  圧縮データの読み込みリクエスト
   *
   *	@param	dataID        データID
   *	@param	divLoadFlag   分割読み込みするか。　（true=分割　false=一括）
   *	@param  p_tmp_heap    テンポラリ確保用のバッファ
   *
   *	@param  true  リクエスト成功
   *	@param  false リクエスト失敗
   */
  //-----------------------------------------------------------------------------
  virtual bool ReqLoadCompData( u32 dataID, b32 divLoadFlag, gfl::heap::HeapBase* p_tmp_buff );

  //----------------------------------------------------------------------------
  /**
   *	@brief  読み込みリセット
   */
  //-----------------------------------------------------------------------------
  virtual bool ResetLoadData( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  読み込み状態の取得
   *
   *	@retval  LOAD_STATE_IDLING = 0,        // 待機中
   *	@retval  LOAD_STATE_START,             // 読み込み開始
   *	@retval  LOAD_STATE_LOAD,              // 読み込み中
   *	@retval  LOAD_STATE_SETUP,             // セットアップ中
   *	@retval  LOAD_STATE_END,               // 完了
   */
  //-----------------------------------------------------------------------------
  gfl::grp::map::System::LoadStatus GetLoadStatus( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  読み込んできたデータを取得
   *
   *	@return 読み込んできたデータ(gflib_cpp/tools/binlinker.rbを使用して生成する。）
   */
  //-----------------------------------------------------------------------------
  const gfl::base::BinLinkerAccessor& GetLoadMemoryPointer(void) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  読み込んできたデータIDを取得
   *
   *	@return データID
   */
  //-----------------------------------------------------------------------------
  u32 GetLoadDataID( void ) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief  アーカイブデータ設定
   *
   *	@param	  cp_path   アーカイブデータのパス
   *  @param    p_heap    ヒープ
   */
  //-----------------------------------------------------------------------------
  void ResistArc( const wchar_t* cp_path, gfl::heap::HeapBase* p_heap );

  //----------------------------------------------------------------------------
  /**
   *	@brief  アーカイブデータ設定  IDバージョン
   *
   *	@param	  arc_id      アーカイブデータのID
   *  @param    p_heap      ヒープ
   *  @param    file_on_pc  パソコンからの読み込みの場合true, ROMの場合false
   */
  //-----------------------------------------------------------------------------
  void ResistArc( gfl::fs::ArcFile::ARCID arc_id, gfl::heap::HeapBase* p_heap, bool file_on_pc = false  );

  //----------------------------------------------------------------------------
  /**
   *	@brief  アーカイブデータ設定
   *
   *	@param	 p_arc 
   */
  //-----------------------------------------------------------------------------
  void ResistArc( gfl::fs::ArcFile* p_arc );


  //----------------------------------------------------------------------------
  /**
   *	@brief  アーカイブ設定完了待ち
   *
   *  @retval true    完了
   *  @retval false   途中（アーカイブのファーストモードセットアップ中）
   */
  //-----------------------------------------------------------------------------
  bool IsRegistArcFinished( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  アーカイブデータ破棄
   */
  //-----------------------------------------------------------------------------
  void ReleaseArc( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  アーカイブデータ取得
   *
   */
  //-----------------------------------------------------------------------------
  gfl::fs::ArcFile* GetArc( void ) const;
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  グローバルテクスチャリソース　設定
   *  
   *	@param	  p_globalTex   グローバルテクスチャ
   *	@param    index         登録インデックス
   */
  //-----------------------------------------------------------------------------
  void ResistGlobalTexResource( gfl::grp::g3d::Resource* p_globalTex, u32 index = 0 );
#if GFL_GRP_G3D_H3D_USE
  void ResistGlobalTexResource( gfl::grp::g3d::H3dResource* p_globalTex, u32 index = 0 );
#endif // GFL_GRP_G3D_H3D_USE

  //----------------------------------------------------------------------------
  /**
   *	@brief  グローバルテクスチャリソース　クリア
   */
  //-----------------------------------------------------------------------------
  void ReleaseGlobalTexResource(void);
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  地形表示のON・OFF
   *
   *	@param	flag    true:ON false:OFF
   */
  //-----------------------------------------------------------------------------
  void SetVisible( b32 flag );

  //----------------------------------------------------------------------------
  /**
   *	@brief  地形表示のON・OFF　チェック
   *
   *	@retval true      ON
   *	@retval false     OFF
   */
  //-----------------------------------------------------------------------------
  b32 IsVisible( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  地形表示位置の設定
   *
   *	@param	  pos   位置
   */
  //-----------------------------------------------------------------------------
  void SetTrans( const gfl::math::VEC3& pos );

  //----------------------------------------------------------------------------
  /**
   *	@brief  地形表示位置の取得
   *
   *	@return 位置
   */
  //-----------------------------------------------------------------------------
  const gfl::math::VEC3& GetTrans( void ) const;

   
  //----------------------------------------------------------------------------
  /**
   *	@brief  アトリビュート取得
   *  
   *	@param	  p_attrInfo    アトリビュートインフォ
   *	@param    cp_pos        座標をあらわすワーク
   *	@param    cp_way        地面方向（Defaultは、(0.0f,-1.0f,0.0f)
   *	@param    map_width     マップブロック幅
   */
  //-----------------------------------------------------------------------------
  virtual void GetAttr( gfl::grp::map::AttrInfo* p_attrInfo, const void* cp_pos, const gfl::math::VEC3* cp_way, f32 map_width );


  //----------------------------------------------------------------------------
  /**
   *	@brief  地形モデル生成
   *  
   *  @param  p_allocator          アロケータ（デバイスメモリ）
   *  @param  p_model_memory       モデルリソースバッファ
   *  @param  p_tex_memory         テクスチャリソースバッファ
   *  @param  cp_res_desc           リソース展開設定
   *  @param  cp_model_desc       　モデル展開設定
   *  @param  common_resource_flag 使用する共通リソースがある場合はビットを立てておく
   */
  //-----------------------------------------------------------------------------
  virtual void CreateModel( gfl::heap::NwAllocator* p_allocator, void* p_model_memory, void* p_tex_memory, const gfl::grp::g3d::Resource::Description* cp_res_desc = NULL, const gfl::grp::g3d::Model::Description* cp_model_desc = NULL, u32 common_resource_flag = grp::g3d::Resource::COMMON_RESOURCE_NONE );

#if GFL_GRP_G3D_H3D_USE
  virtual void CreateH3dModel( gfl::heap::NwAllocator* p_allocator, void* p_model_memory, void* p_tex_memory, /* const gfl::grp::g3d::H3dResource::Description* cp_res_desc = NULL, const gfl::grp::g3d::H3dModel::Description* cp_model_desc = NULL, */ u32 common_resource_flag = grp::g3d::H3dResource::COMMON_RESOURCE_NONE );
#endif // GFL_GRP_G3D_H3D_USE


  //----------------------------------------------------------------------------
  /**
   *	@brief  地形モデル破棄
   */
  //-----------------------------------------------------------------------------
  virtual void DeleteModel( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  地形モデルの取得
   *
   *	@retval モデルオブジェクト
   */
  //-----------------------------------------------------------------------------
  gfl::grp::g3d::Model* GetModel( void ) const;
#if GFL_GRP_G3D_H3D_USE
  gfl::grp::g3d::H3dModel* GetH3dModel( void ) const;
#endif // GFL_GRP_G3D_H3D_USE

  //----------------------------------------------------------------------------
  /**
   *	@brief  地形モデルのモデルリソース取得
   *
   *	@retval モデルリソース
   */
  //-----------------------------------------------------------------------------
  const gfl::grp::g3d::Resource* GetResourceModel( void ) const;
#if GFL_GRP_G3D_H3D_USE
  const gfl::grp::g3d::H3dResource* GetH3dResourceModel( void ) const;
#endif // GFL_GRP_G3D_H3D_USE

  //----------------------------------------------------------------------------
  /**
   *	@brief  地形モデルのテクスチャリソース取得
   *
   *	@retval テクスチャリソース
   */
  //-----------------------------------------------------------------------------
  const gfl::grp::g3d::Resource* GetResourceTexture( void ) const;
#if GFL_GRP_G3D_H3D_USE
  const gfl::grp::g3d::H3dResource* GetH3dResourceTexture( void ) const;
#endif // GFL_GRP_G3D_H3D_USE


  //----------------------------------------------------------------------------
  /**
   *	@brief  読み込んだマップブロックのサイズを取得
   */
  //-----------------------------------------------------------------------------
  s32 GetLoadFileSize( void ) const;

protected:

  //----------------------------------------------------------------------------
  /**
   *	@brief  ワークのクリア処理  すべてのメンバの値を初期化します。
   */
  //-----------------------------------------------------------------------------
  virtual void Clear( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  マップバッファ　初期化
   *
   *	@param	param         パラメータ
   *	@param  p_sysHeap     システムメモリ用ヒープ
   *	@param  p_deviceHeap  デバイスメモリ用ヒープ
   */
  //-----------------------------------------------------------------------------
  virtual void InitializeMapBuffer( const SetUpParam& param, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap );
  //----------------------------------------------------------------------------
  /**
   *	@brief  マップバッファ　破棄
   */
  //-----------------------------------------------------------------------------
  virtual void FinalizeMapBuffer( void );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  モデルバッファ　初期化
   *
   *	@param	param         パラメータ
   *	@param  p_sysHeap     システムメモリ用ヒープ
   *	@param  p_deviceHeap  デバイスメモリ用ヒープ
   */
  //-----------------------------------------------------------------------------
  virtual void InitializeModelBuffer( const SetUpParam& param, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap );
  //----------------------------------------------------------------------------
  /**
   *	@brief  モデルバッファ 破棄
   */
  //-----------------------------------------------------------------------------
  virtual void FinalizeModelBuffer( void );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  モデルデータ用VRAM確保処理
   *
   *	@param	param         パラメータ
   *	@param  p_sysHeap     システムメモリ用ヒープ
   *	@param  p_deviceHeap  デバイスメモリ用ヒープ
   */
  //-----------------------------------------------------------------------------
  virtual void InitializeVramAddr( const SetUpParam& param, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap );
  //----------------------------------------------------------------------------
  /**
   *	@brief  VRAM確保領域のクリア
   */
  //-----------------------------------------------------------------------------
  virtual void FinalizeVramAddr( void );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  分割ロードシステムの初期化
   *
   *	@param	param         パラメータ
   *	@param  p_sysHeap     システムヒープ
   *	@param  p_deviceHeap  デバイスヒープ
   */
  //-----------------------------------------------------------------------------
  virtual void InitializeDivLoad( const SetUpParam& param, gfl::heap::HeapBase* p_sysHeap, gfl::heap::HeapBase* p_deviceHeap );
  //----------------------------------------------------------------------------
  /**
   *	@brief  分割ロードシステムの破棄
   */
  //-----------------------------------------------------------------------------
  virtual void FinalizeDivLoad( void );

  
  //----------------------------------------------------------------------------
  /**
   *	@brief  分割読み込みの状態遷移
   */
  //-----------------------------------------------------------------------------
  virtual void UpdateLoad( void );


  //----------------------------------------------------------------------------
  /**
   *	@brief  モデルモードの一致チェック
   */
  //-----------------------------------------------------------------------------
  bool isModelMode( ModelMode mode ) const ;

protected:
  /// g3d::Modelを使用する場合
  struct DefaultModelDatas
  {
    gfl::grp::g3d::Model* m_pModel;         // モデル
    gfl::grp::g3d::Resource* m_pMdlRes;     // モデルリソース
    gfl::grp::g3d::Resource* m_pTexRes;     // モデルテクスチャリソース
    gfl::grp::g3d::Resource* m_pGlobalTexRes[GLOBAL_RESOURCE_MAX]; // 共有テクスチャリソース
  };
  

#if GFL_GRP_G3D_H3D_USE
  /// g3d::H3dModelを使用する場合
  struct H3dModelDatas
  {
    gfl::grp::g3d::H3dModel* m_pModel;         // モデル
    gfl::grp::g3d::H3dResource* m_pMdlRes;     // モデルリソース
    gfl::grp::g3d::H3dResource* m_pTexRes;     // モデルテクスチャリソース
    gfl::grp::g3d::H3dResource* m_pGlobalTexRes[GLOBAL_RESOURCE_MAX]; // 共有テクスチャリソース
  };
#endif // GFL_GRP_G3D_H3D_USE

  /// Modelを使用する場合
  union ModelDatas
  {
    DefaultModelDatas def;
#if GFL_GRP_G3D_H3D_USE
    H3dModelDatas     h3d;
#endif // GFL_GRP_G3D_H3D_USE
  };

protected:

  // 初期化　有無
  b8 m_Initialize;
  b8 m_CreateModel;

  // モデルモード
  ModelMode   m_ModelMode;    ///< この値を見て、ModelDatasの中身を使い分ける。

  // 表示ON・OFF
  b8 m_Visible;
  

  // 描画登録シーンシステム
  gfl::grp::g3d::Scene* m_pScene;


  // 表示位置
  gfl::math::VEC3 m_Trans;

  // データ読み込みアーカイブファイル
  gfl::fs::ArcFile* m_pArcFile;
  bool m_ArcFileOriginal;
  // アーカイブデータID
  u32 m_DataID;

  // マップ情報読み込みバッファ
  void* m_pMapData;         // バッファポインタ
  u32 m_MapDataHeapSize;    // バッファサイズ
  
  // パックバイナリデータAccessor
  gfl::base::BinLinkerAccessor m_LinkData;

  // フィールドコントロール
  u32 m_FileType;               // ファイル識別　タイプ
  FileControl*  m_pFileControl;  // 今のファイルタイプに対応したファイルコントロール
  const FileControlArray* m_cpFileControls; // フィールドコントロールデータ
  

  // VRAM領域アドレス
  uptr m_VramAddrTex;     // テクスチャ　VRAMアドレス
  uptr m_VramAddrPltt;    // パレット　　VRAMアドレス
  u32 m_MapTexVramSize;   // テクスチャVRAMサイズ

  // 地形モデル情報
  ModelDatas      m_ModelDatas;

  // 分割読み込み管理
  DivLoad m_DivLoad;
  u32 m_DivLoadSize;        // 分割読み込み　１回の読み込みサイズ
  LoadStatus  m_LoadStatus; // 読み込み状態
};
 


} // namespace map
} // namespace grp
} // namespace gfl
#endif // __GFL_MAPSYSTEM_H__
