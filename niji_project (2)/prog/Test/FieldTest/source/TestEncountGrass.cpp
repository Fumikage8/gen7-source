﻿//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		TestEncountGrass.cpp
 *	@brief		エンカウント草テスト　クラス
 *	@author		tomoya takahashi 
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "test/FieldTest/include/TestEncountGrass.h"

#include "test/FieldTest/include/FieldTestUtility.h"

// リソース
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

// デバッグ

#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

#if PM_DEBUG
#include "test/FieldTest/include/Debug/FieldDebugTypes.h"
#include "test/FieldTest/include/Debug/FieldDebugInstanceMemoryCollection.h"

#endif

// arc
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include "arc_def_index/arc_def.h"
#include "arc_index/encgrass_gfmdl.gaix"



GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )


// グリッドクラス
class Grid
{
  GFL_FORBID_COPY_AND_ASSIGN(Grid); //コピーコンストラクタ＋代入禁止

public:
  enum
  {
    GRID_MAX = 40 * 40 + 1, // グリッド最大サイズ
  };

  Grid(){}
  ~Grid()
  {
    if( m_pOffsetArray ){
      GflHeapFreeMemory( m_pOffsetArray );
      m_pOffsetArray = NULL;
    }
  }

  /*
   *  @param  transformは左上
   *
   */
  void Setup( gfl2::heap::HeapBase* pHeap, u32 x, u32 z, f32 width, const gfl2::math::Matrix34& transform )
  {
    m_Xnum = x;
    m_Znum = z;
    m_width = width;
    m_Transform = transform;
    m_InvTransform = m_Transform.Inverse();

    GFL_ASSERT( (m_Xnum * m_Znum) < GRID_MAX );

    // オフセットテーブル生成
    m_pOffsetArray = (f32*)GflHeapAllocMemory( pHeap, m_Znum * sizeof(float) );
    for( u32 i=0; i<m_Znum; ++i ){
      m_pOffsetArray[i] = 0.0f;
    }
  }
 
  /*
   * @brief オフセット設定
   */
  void SetXOffset( u32 z, f32 offset_x )
  {
    if( z < m_Znum ){
      m_pOffsetArray[z] = offset_x;
    }
  }


  /*
   *  @param  交差判定
   */
  bool IsHit( const gfl2::math::Vector3& pos, u32* p_idx ) const
  {
    gfl2::math::Vector3 inv_pos;

    inv_pos = m_InvTransform * pos;

    s32 grid_z = static_cast<s32>(inv_pos.z / m_width);

    s32 grid_x = static_cast<s32>((inv_pos.x - m_pOffsetArray[grid_z]) / m_width);

    if( (grid_x >= 0) && (grid_x < m_Xnum) &&
        (grid_z >= 0) && (grid_z < m_Znum) )
    {
      *p_idx = (grid_z * m_Xnum) + grid_x;
      return true;
    }

    return false;
  }

  gfl2::math::Vector3 GetGridCenterPos( u32 index ) const
  {
    u32 z = index / m_Xnum;
    u32 x = index % m_Xnum;
    gfl2::math::Vector3 center_pos;

    center_pos.x = (x*m_width) + (m_width/2.0f) + m_pOffsetArray[ z ];
    center_pos.z = (z*m_width) + (m_width/2.0f);
    center_pos.y = 0;

    center_pos = m_Transform * center_pos;

    return center_pos;
  }

public:

  u32 m_Xnum;
  u32 m_Znum;
  f32 m_width;
  gfl2::math::Matrix34 m_Transform;
  gfl2::math::Matrix34 m_InvTransform;
  f32* m_pOffsetArray;

};

// エフェクト
class Effect
{
  GFL_FORBID_COPY_AND_ASSIGN(Effect); //コピーコンストラクタ＋代入禁止

public:
  // コンストラクタ / デストラクタ
  Effect(void) : 
    m_is_exist(false),
    m_status(ST_NONE),
    m_pEffectInstanceNode(NULL)
  {
  }

  ~Effect()
  {
    Finalize();
  }

  /**
   * @brief 初期化
   * @param pResource 
   * @param pHeap
   * @param pPipeLine
   */
  void Initialize( u32 idx, const gfl2::math::Vector3& pos, gfl2::renderingengine::scenegraph::resource::ResourceNode * pResource, gfl2::renderingengine::scenegraph::resource::ResourceNode * pMotionResource, gfl2::heap::HeapBase* pHeap, Test::FieldTest::MyRenderingPipeLine* pPipeLine )
  {
    if( !m_is_exist ){
      m_pPipeLine = pPipeLine;
      // インスタンス生成
      {
        //static int create_indtance_count = 0; char strbuff[128] = {0};
        //Utility::Sprintf_S( strbuff, 127, "EncountGrassEffect[%d]",  create_indtance_count ++ );
        //int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
        //FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
        m_pEffectInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( pResource );
        //m_pEffectInstanceNode->SetNodeName( strbuff );
        //FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
        
        m_pEffectInstanceNode->SetLocalTranslate( pos );
        m_pEffectInstanceNode->SetVisible( true );
        m_pPipeLine->AddEdgeRenderingTarget( m_pEffectInstanceNode );
      }

      // アニメーションコントローラー初期化
      {
        gfl2::animation::AnimationController::InitDescription desc;
        desc.jointSlotCount = 1;
        m_EffectAnimationControler.Initialize(m_pEffectInstanceNode , desc);
        gfl2::animation::JointAnimationSlot* pSlot0 = m_EffectAnimationControler.GetJointSlot(0);
        pSlot0->SetAnimationResource(pMotionResource);
        pSlot0->SetStepFrame(0.0f);
        pSlot0->SetLoop(false);
      }

      m_is_exist = true;
      m_status = ST_NONE;

      m_idx = idx;  // 識別IDX
    }
  }
  
  // 破棄
  void Finalize( void )
  {
    if( m_is_exist ){

      
      SetVisible(false);
      m_EffectAnimationControler.Finalize();
      GFL_DELETE ( m_pEffectInstanceNode );
      m_pEffectInstanceNode = NULL;
  
      m_is_exist = false;
    }
  }

  void SetVisible( bool flag )
  {
    if( flag ){
      if( m_pEffectInstanceNode->IsVisible() == false ){
        m_pEffectInstanceNode->SetVisible( flag );
        gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pEffectInstanceNode );
        m_pPipeLine->AddEdgeRenderingTarget( m_pEffectInstanceNode );
      }
    }
    else{
      if( m_pEffectInstanceNode->IsVisible() == true ){
        m_pEffectInstanceNode->SetVisible( flag );
        gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild( m_pEffectInstanceNode );
        m_pPipeLine->RemoveEdgeRenderingTarget( m_pEffectInstanceNode );
      }
    }
  }

  bool IsVisible( void ) const
  {
    return m_pEffectInstanceNode->IsVisible();
  }

  // 更新
  void Update( const gfl2::math::Vector3& pos )
  {
    if( !this->IsExist() ) { return ; } //
    //if( !this->IsVisible() ){ return ; }

    // 主人公位置に合わせて動作させる。
    gfl2::math::Vector3 mypos = m_pEffectInstanceNode->GetLocalTranslate();

    // myposと主人公の位置関係で、草のアニメーション及び、破棄を制御
    f32 dist = (mypos - pos).Length();
    static const f32 GRASS_ON_LEN = 75.0f;

    switch( m_status ){
    case ST_NONE:
      //if( dist < GRASS_ON_LEN )
      {
        // ONへ
        m_status = ST_ON;

        // フレーム制御
        gfl2::animation::JointAnimationSlot* pSlot0 = m_EffectAnimationControler.GetJointSlot(0);
        pSlot0->SetFrame( 0.0f );
        pSlot0->SetStepFrame(1.0f);
      }
      break;

    case ST_ON:
      if( dist >= GRASS_ON_LEN )
      {
        // OFFへ
        m_status = ST_OFF;

        // フレーム制御
        gfl2::animation::JointAnimationSlot* pSlot0 = m_EffectAnimationControler.GetJointSlot(0);
        pSlot0->SetFrame( 3.0f );
        pSlot0->SetStepFrame(1.0f);
      }
      else
      {
        gfl2::animation::JointAnimationSlot* pSlot0 = m_EffectAnimationControler.GetJointSlot(0);
        f32 frame = pSlot0->GetFrame();
        if( frame >= 3.0f ){
          pSlot0->SetStepFrame(0.0f);
        }
      }
      break;

    case ST_OFF:
      if( dist < GRASS_ON_LEN )
      {
        // ONへ
        m_status = ST_ON;

        // フレーム制御
        gfl2::animation::JointAnimationSlot* pSlot0 = m_EffectAnimationControler.GetJointSlot(0);
        pSlot0->SetFrame( 0.0f );
        pSlot0->SetStepFrame(1.0f);
      }
      else
      {
        gfl2::animation::JointAnimationSlot* pSlot0 = m_EffectAnimationControler.GetJointSlot(0);
        if( pSlot0->GetFrame() >= 6.0f )
        {
          // 消す
          SetVisible(false);
          m_status = ST_DELETE;
        }
      }
      break;

    case ST_DELETE:// 破棄
      Finalize();
      m_status = ST_NONE;
      break;

    }

    // アニメーション更新
    if( m_status != ST_NONE ){
      m_EffectAnimationControler.Update();
    }
  }

  // 動作中？
  inline bool IsExist( void ) const { return m_is_exist; }

  // 識別IDX取得
  inline u32 GetIdx( void ) const { return m_idx; }

private:

  enum
  {
    ST_NONE,  // 何もしてない
    ST_ON,    // のった
    ST_OFF,   // 離れた 
    ST_DELETE,// 破棄

    ST_NUM,
  };

  bool m_is_exist;  // 動作中？
  u32  m_status;    // ステータス
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*   m_pEffectInstanceNode; // エフェクト最大：数
  gfl2::animation::AnimationController															m_EffectAnimationControler; // アニメーションコントロール
  Test::FieldTest::MyRenderingPipeLine* m_pPipeLine; //! パイプライン
  u32 m_idx;  // 識別IDX
};


//-----------------------------------------------------------------------------
/**
*					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------


/**
 *	@brief コンストラクタ
 */
TestEncountGrass::TestEncountGrass() : 
  m_pPipeLine(NULL),
  m_pEffectArray(NULL),
  m_EffectArrayNum(0)
{
  for( int i= 0; i< RES_NUM; ++i ){
    m_pResourceRootNode[i] = NULL;
    m_pResBuf[i] = NULL;
  }

  for( int i= 0; i< MDL_NUM; ++i ){
    m_pModelInstanceNode[i] = NULL;
  }

  for( int i= 0; i< GRID_NUM; ++i ){
    m_pGrid[i] = NULL;
  }
}

/**
 *	@brief デストラクタ
 */
TestEncountGrass::~TestEncountGrass()
{
	Finalize();
}


/**
 *	@brief 初期化
 *	@param	pParentHeap		親デバイスヒープ
 */
void TestEncountGrass::Initialize( gfl2::heap::HeapBase* pParentHeap, gfl2::fs::AsyncFileManager * pAsyncFileReadManager, Test::FieldTest::MyRenderingPipeLine* pPipeLine  )
{
  m_pPipeLine = pPipeLine;
  m_pHeap = pParentHeap;

  //---------------------------------------------------
  // ARCでモデルデータ読み込み
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_ENCGRASS_TEST;
    openReq.heapForFile = pParentHeap;
    openReq.heapForArcSrc = pParentHeap;
    openReq.heapForReq = pParentHeap;
    pAsyncFileReadManager->SyncArcFileOpen( openReq );
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;

    static const u32 sc_LoadDataIdx[] = {
      RES_GRASS0, // アニメーションするモデル
      RES_ENCGRASS01, // 計測：高負荷上物草
      RES_ENCGRASS02, // 計測：普通負荷上物草
      RES_ENCMAP01, // 計測：高負荷　下草
      RES_ENCMAP02, // 計測：普通負荷　下草
      RES_MOT,
    };

    for( int i= 0; i< GFL_NELEMS(sc_LoadDataIdx); ++i ){
      u32 idx = sc_LoadDataIdx[i];
      loadReq.arcId = ARCID_ENCGRASS_TEST;
      loadReq.datId = idx;
      loadReq.ppBuf = &m_pResBuf[idx];
      loadReq.heapForBuf = pParentHeap;
      loadReq.pBufSize = &m_ResSize[idx];
      loadReq.align = 128;
      pAsyncFileReadManager->SyncArcFileLoadData( loadReq );
    }
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = ARCID_ENCGRASS_TEST;
    closeReq.heapForReq = pParentHeap;
    pAsyncFileReadManager->SyncArcFileClose( closeReq );
  }

  // HIGH LOW の配置位置
  gfl2::math::Vector3 highpos( 320, -368, 3900 );
  gfl2::math::Vector3 lowpos( 320, -368, 3900 );
  {
    static const u32 sc_ModelResource[] = 
    {
      RES_GRASS0, // アニメーションするモデル

      RES_ENCGRASS01, // 計測：高負荷上物草
      RES_ENCGRASS02, // 計測：普通負荷上物草

      RES_ENCMAP01, // 計測：高負荷　下草
      RES_ENCMAP02, // 計測：普通負荷　下草
    };
    for( int i= 0; i< GFL_NELEMS(sc_ModelResource); ++i )
    {
      // モデルリソース生成
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
        resourceData.SetModelData((c8*)m_pResBuf[sc_ModelResource[i]]);

        char strbuff[128] = {0};
        Utility::Sprintf_S( strbuff, 127, "EncGrass Resource[%d]",  i );
        int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
        m_pResourceRootNode[sc_ModelResource[i]] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData);
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
      }
    }

    static const u32 sc_ModelInstance[] = 
    {
      RES_ENCGRASS01, // 計測：高負荷上物草
      RES_ENCGRASS02, // 計測：普通負荷上物草

      RES_ENCMAP01, // 計測：高負荷　下草
      RES_ENCMAP02, // 計測：普通負荷　下草
    };

    GFL_ASSERT( GFL_NELEMS(sc_ModelInstance) == MDL_NUM );
    for( int i= 0; i< MDL_NUM; ++i )
    {
      // インスタンス生成
      {
        char strbuff[128] = {0};
        Utility::Sprintf_S( strbuff, 127, "EncountGrassModel[%d]",  i );
        int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
        m_pModelInstanceNode[i] = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pResourceRootNode[sc_ModelInstance[i]]);
        m_pModelInstanceNode[i]->SetNodeName( strbuff );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);

        m_pModelInstanceNode[i]->SetVisible(false);
        gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild( m_pModelInstanceNode[i] );
        //m_pPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode[i] );

        {
          int l_mesh_num = m_pModelInstanceNode[i]->GetMehsNum();
          int l_material_num = m_pModelInstanceNode[i]->GetMaterialNum();
          int l_joint_num = m_pModelInstanceNode[i]->GetJointNum();
          GFL_PRINT( "mdl[%d] mesh[%d] material[%d] joint[%d]\n", i, l_mesh_num, l_material_num, l_joint_num );
        }
      }
    }

    m_pModelInstanceNode[MDL_HIGH_UPPER]->SetLocalTranslate( highpos );
    m_pModelInstanceNode[MDL_LOW_UPPER]->SetLocalTranslate( lowpos );
    m_pModelInstanceNode[MDL_HIGH_LOWER]->SetLocalTranslate( highpos );
    m_pModelInstanceNode[MDL_LOW_LOWER]->SetLocalTranslate( lowpos );
  }

  // モーション適応
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData motionResourceData;
    motionResourceData.SetMotionData( (c8*)m_pResBuf[RES_MOT]/*, m_ResSize[RES_MOT]*/ );

    char strbuff[128] = {0};
    Utility::Sprintf_S( strbuff, 127, "EncGrass MotionResource[%d]",  0 );
    int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
    m_pResourceRootNode[RES_MOT] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &motionResourceData );
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);

#if 0
    gfl2::animation::AnimationController::InitDescription desc;
    desc.jointSlotCount = 1;
    m_AnimationControler.Initialize(m_pModelInstanceNode[MDL_GRASS0] , desc);
    gfl2::animation::JointAnimationSlot* pSlot0 = m_AnimationControler.GetJointSlot(0);
    pSlot0->SetAnimationResource(m_pResourceRootNode[RES_MOT]);
    pSlot0->SetLoop(true);
#endif
  }


  // グリッドクラス生成
  // 範囲１
  {
    gfl2::math::Vector3 g_highpos(highpos);

    static const u32 GRID_X = 9;
    static const u32 GRID_Z = 4;
    m_pGrid[0] = GFL_NEW(pParentHeap) Grid();
    gfl2::math::Matrix34 trans = gfl2::math::Matrix34::GetIdentity();

    trans.SetElem( 0, 3, g_highpos.GetX() );
    trans.SetElem( 1, 3, g_highpos.GetY() );
    trans.SetElem( 2, 3, g_highpos.GetZ() );
    m_pGrid[0]->Setup( m_pHeap, GRID_X, GRID_Z, 75.0f, trans );
    for( u32 i=0; i<GRID_Z; i+=2 ){
      m_pGrid[0]->SetXOffset( i, 37.50f );  // 半サイズずれている
    }
  }

  // 範囲２
  {
    gfl2::math::Vector3 g_lowpos(lowpos);
    
    static const u32 GRID_X = 32;
    static const u32 GRID_Z = 32;
    m_pGrid[1] = GFL_NEW(pParentHeap) Grid();

    gfl2::math::Matrix34 trans = gfl2::math::Matrix34::GetIdentity();

    trans.SetElem( 0, 3, g_lowpos.GetX() );
    trans.SetElem( 1, 3, g_lowpos.GetY() );
    trans.SetElem( 2, 3, g_lowpos.GetZ() );
    m_pGrid[1]->Setup( m_pHeap, GRID_X, GRID_Z, 75.0f, trans );
    for( u32 i=0; i<GRID_Z; i+=2 ){
      m_pGrid[1]->SetXOffset( i, 37.50f );  // 半サイズずれている
    }
  }

  // エフェクト配列生成
  m_pEffectArray = GFL_NEW(pParentHeap) Effect[EFF_GRASS_MAX];
  m_EffectArrayNum = EFF_GRASS_MAX;

}

/**
 *	@brief 破棄
 */
void TestEncountGrass::Finalize( void )
{
  // エフェクト破棄
  for( int i=0; i<m_EffectArrayNum; ++i ){
    m_pEffectArray->Finalize();
  }
  GFL_DELETE_ARRAY(m_pEffectArray);
  m_pEffectArray = NULL;
  m_EffectArrayNum = 0;

  // グリッド破棄
  for( int i=0; i<GRID_NUM; ++i )
  {
    GFL_DELETE m_pGrid[i];
    m_pGrid[i] = NULL;
  }

  // アニメーション
  //m_AnimationControler.Finalize();

  for( int i= 0; i< MDL_NUM; ++i ){
    
    // モデルインスタンス破棄
    if( m_pModelInstanceNode[i] ){

      m_pPipeLine->RemoveEdgeRenderingTarget( m_pModelInstanceNode[i] );
      GFL_DELETE m_pModelInstanceNode[i];
      m_pModelInstanceNode[i] = NULL;
    }
  }

  for( int i= 0; i< RES_NUM; ++i ){
    // リソースを破棄
    if( m_pResourceRootNode[i] ){
      GFL_DELETE m_pResourceRootNode[i];
      m_pResourceRootNode[i] = NULL;
    }
    if(m_pResBuf[i]){
      GflHeapSafeFreeMemory( m_pResBuf[i] );
      m_pResBuf[i] = NULL;
    }
  }

}


/**
 *	@brief 更新処理
 */
void TestEncountGrass::Update( const gfl2::math::Vector3& pos )
{

  // アニメーション更新
  //m_AnimationControler.Update();

  // エンカウント草の領域判定
  // グリッドを算出、
  // そこの草が動作しているかチェック
  // 動作していなければ、そこに草を生成
#if 0
  u32 index;
  for( int i=0; i<GRID_NUM; ++i ){

#if PM_DEBUG
    // フラグがたっていなかったら生成しない
    if( !Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_ENCOUNT_HIGH + i ] ){
      continue;
    }
#endif

    if( m_pGrid[i]->IsHit( pos, &index ) )
    {
      bool is_exist = false;

      u32 id = index + (i * Grid::GRID_MAX);
      // グリッドオリジナルのインデックス値に変換

      // その位置のデータが存在しているかチェック
      for( int j=0; j<m_EffectArrayNum; ++j )
      {
        if( !m_pEffectArray[j].IsExist() ){ continue; }

        if( m_pEffectArray[j].GetIdx() == id )
        {
          is_exist = true;
          break;
        }
      }
      if( !is_exist )
      {
        gfl2::math::Vector3 eff_pos = m_pGrid[i]->GetGridCenterPos( index );
        for( int j=0; j<m_EffectArrayNum; ++j )
        {
          if( !m_pEffectArray[j].IsExist() ){
            m_pEffectArray[j].Initialize( id, eff_pos, 
              m_pResourceRootNode[RES_GRASS0], m_pResourceRootNode[RES_MOT],
              m_pHeap, m_pPipeLine );
            break;
          }
        }
      }
    }
  }
  
  // GRASS1の領域にプレイヤーが入ったら、
  // 草を生成してアニメーションさせる。
  for( int i=0; i<m_EffectArrayNum; ++i )
  {
    m_pEffectArray[i].Update(pos);
  }
#endif

#if PM_DEBUG
  ControlVisible( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_ENCOUNT_HIGH ],
    m_pModelInstanceNode[MDL_HIGH_UPPER], m_pPipeLine );

  ControlVisible( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_ENCOUNT_MIDDLE ],
    m_pModelInstanceNode[MDL_LOW_UPPER], m_pPipeLine );

  ControlVisible( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_ENCOUNT_GROUND ],
    m_pModelInstanceNode[MDL_LOW_LOWER], m_pPipeLine );

  ControlVisible( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_ENCOUNT_GROUND ],
    m_pModelInstanceNode[MDL_HIGH_LOWER], m_pPipeLine );
#endif
}


GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )

