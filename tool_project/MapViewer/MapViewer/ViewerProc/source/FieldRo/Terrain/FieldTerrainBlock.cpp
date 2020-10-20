/**
* @brief  �t�B�[���h�̒n�`�u���b�N�N���X
* @file   TerrainBlock.cpp
* @author ikuta_junya
* @data   2015.04.19
*/

// field
#include "ViewerProc/include/FieldRo/Terrain/FieldTerrainBlock.h"
#include "ViewerProc/include/FieldRo/Terrain/FieldTerrainFactory.h"
///#include "Field/FieldRo/include/Terrain/Block/FieldTerrainBlockLodChangeStaticListener.h"
#include "ViewerProc/include/FieldStatic/Collision/CollisionSceneNone.h"
#include "ViewerProc/include/FieldStatic/Collision/StaticActor.h"
///#include "Field/FieldRo/include/KusaModel/FieldKusaModel.h"

// field debug
///#include "Field/FieldRo/include/Debug/FieldDebugTypes.h"

// �O���錾
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );

const static u32 INVISIBLE_CULLING = 0x01;
const static u32 INVISIBLE_DEBUG_DRAW_SWITCH = 0x01 << 1;

/**
* @brief �R���X�g���N�^
*
* @return ����
*/ 
TerrainBlock::TerrainBlock( void ) :

  // ����
  m_X( 0 )
  ,m_Z( 0 )
  ,m_Size( 0.0f )
  ,m_Pos( 0.0f ,0.0f, 0.0 )
  ///,m_RequestLodType( Block::LodType::MIDDLE )
  ///,m_LodType( Block::LodType::MIDDLE )
  ,m_InvisibleBit( 0 )
  ,m_IsLoading( false )
  ///,m_IsHighModel( false )

  // �I�u�W�F�N�g
  ,m_pCollisionModel( NULL )
  ,m_pStaticActor( NULL )
  ,m_pWallCollisionModel( NULL )
  ,m_pWallStaticActor( NULL )
  ///,m_ppLodChangeStaticListenerTable( NULL )
  ,m_LodChangeStaticListenerMax( 0 )
  
  // ���\�[�X�f�[�^
  // �O���I�u�W�F�N�g
  ,m_pTerrainFactory( NULL )
  ,m_pCollisionScene( NULL )
  ,m_pWallCollisionScene( NULL )
{
#if 0
  for( u32 i = 0; i < Block::LodType::MAX; i++ )
  {
    // m_pModelNodeTable[ i ] = NULL;
    m_pModelTable[ i ] = NULL;
    ///m_ppEncountGrassModelTable[i] = NULL;
  }
#endif //@saito_del
	m_pModelTable = NULL; //@satio_add
}

/**
* @brief �f�X�g���N�^
*
* @return ����
*/
TerrainBlock::~TerrainBlock( void )
{
  Terminate();
}

/**
* @brief ����������
*
* @param  rInitDesc   �������ݒ�
*
* @return ����
*/
void TerrainBlock::Initialize( const InitializeDescription& rInitDesc )
{
  m_Pos = rInitDesc.m_Pos;
  m_X = rInitDesc.m_X;
  m_Z = rInitDesc.m_Z;
  m_Size = rInitDesc.m_Size;
  m_pTerrainFactory = rInitDesc.pTerrainFactory;
  m_pCollisionScene = rInitDesc.pCollisionScene;
  m_pWallCollisionScene = rInitDesc.m_pWallCollisionScene;

  ///m_IsHighModel = m_pTerrainFactory->CheckHighModelDataVailed( m_X, m_Z );

  // MiddleMode����
  // m_pModelNodeTable[ Block::LodType::MIDDLE ] = m_pTerrainFactory->CreateMiddleModelNode( m_X, m_Z );
  ///m_pModelTable[ Block::LodType::MIDDLE ] = m_pTerrainFactory->CreateMiddleModel( m_X, m_Z );@saito_del
  m_pModelTable = m_pTerrainFactory->CreateMiddleModel( m_X, m_Z );//@saito_add
  /**
	// ���X�i�[�Ǘ��e�[�u���쐬
  m_LodChangeStaticListenerMax = rInitDesc.m_LodChangeStaticListenerMax;
  if( m_LodChangeStaticListenerMax > 0 )
  {
    m_ppLodChangeStaticListenerTable = GFL_NEW_ARRAY( rInitDesc.m_pSystemHeap ) Block::LodChangeStaticListener* [ m_LodChangeStaticListenerMax ];
    for( u32 i = 0; i < m_LodChangeStaticListenerMax; i++ )
    {
      m_ppLodChangeStaticListenerTable[ i ] = NULL;
    }
  }
	*///@saito_del
}

/**
* @brief �I������
*
* @return ����
*/
void TerrainBlock::Terminate( void )
{
  // ���X�i�[�Ǘ��e�[�u���j��
  ///GFL_SAFE_DELETE_ARRAY( m_ppLodChangeStaticListenerTable );
  m_LodChangeStaticListenerMax = 0;

  // �ǃR���W�����o�^�����E�j��
  {
    if( m_pWallStaticActor )
    {
      m_pWallCollisionScene->UnRegistStaticActor( m_pWallStaticActor );
      GFL_SAFE_DELETE( m_pWallStaticActor );
    }
    GFL_SAFE_DELETE( m_pWallCollisionModel );
  }

  // �����R���W�����o�^�����E�j��
  {
    if( m_pStaticActor )
    {
      m_pCollisionScene->UnRegistStaticActor( m_pStaticActor );
      GFL_SAFE_DELETE( m_pStaticActor );
    }
    GFL_SAFE_DELETE( m_pCollisionModel );
  }
	/**
  // �����f��
  {
    for( u32 i = 0; i < Block::LodType::MAX; i++ )
    {
      if( m_ppEncountGrassModelTable[ i ] ){
        for( int j=0; j<m_pTerrainFactory->GetEncountGrassModelTableNum(); ++j ){ GFL_SAFE_DELETE( m_ppEncountGrassModelTable[ i ][ j ] ); }
        GFL_SAFE_DELETE_ARRAY(m_ppEncountGrassModelTable[ i ]);
      }
    }
  }
	*///@saito_del
  // @todo 1�t���[���x��J��
  // @todo ���[�h������J��
  // @todo �R���W�������f���J��
  /**
	for( u32 i = 0; i < Block::LodType::MAX; i++ )
  {
    // GFL_SAFE_DELETE( m_pModelNodeTable[ i ] );
    GFL_SAFE_DELETE( m_pModelTable[ i ] );

  }
	*/ //@saito_del
	GFL_SAFE_DELETE( m_pModelTable );//@saito_add


  if( m_pTerrainFactory->CheckHighModelUnitState( m_X, m_Z ) == TerrainFactory::UnitState::LOAD_COMPLETION )
  {
    m_IsLoading = false;
    m_pTerrainFactory->UnLoadHighModel( m_X, m_Z );
  }
}
#if 0
/**
* @brief �X�V����
*
* @return ����
*/
void TerrainBlock::Update( void )
{
  // @todo ��ԕʏ����𐮗�

  // 1�t���[���x��āAhigh���f�����J������
#if 0
  if( m_pModelNodeTable[ Block::LodType::HIGH ] && m_LodType != Block::LodType::HIGH )
  {
    GFL_SAFE_DELETE( m_pModelNodeTable[ Block::LodType::HIGH ] );

    // �ǃR���W�����o�^�����E�j��
    {
      if( m_pWallStaticActor )
      {
        m_pWallCollisionScene->UnRegistStaticActor( m_pWallStaticActor );
        GFL_SAFE_DELETE( m_pWallStaticActor );
      }
      GFL_SAFE_DELETE( m_pWallCollisionModel );
    }

    // �����R���W�����o�^�����E�j��
    {
      if( m_pStaticActor )
      {
        m_pCollisionScene->UnRegistStaticActor( m_pStaticActor );
        GFL_SAFE_DELETE( m_pStaticActor );
      }
      GFL_SAFE_DELETE( m_pCollisionModel );
    }
    
    m_pTerrainFactory->UnLoadHighModel( m_X, m_Z );
  }
#endif

  if( m_pModelTable[ Block::LodType::HIGH ] && m_LodType != Block::LodType::HIGH )
  {
    GFL_SAFE_DELETE( m_pModelTable[ Block::LodType::HIGH ] );

    // �G���J�E���g���̔j��
    {
      if( m_ppEncountGrassModelTable[ Block::LodType::HIGH ] ){
      
        for( int i=0; i<m_pTerrainFactory->GetEncountGrassModelTableNum(); ++i ){ GFL_SAFE_DELETE( m_ppEncountGrassModelTable[ Block::LodType::HIGH ][i] ); }
        GFL_SAFE_DELETE_ARRAY(m_ppEncountGrassModelTable[ Block::LodType::HIGH ]);
      }
    }

    // �ǃR���W�����o�^�����E�j��
    {
      if( m_pWallStaticActor )
      {
        m_pWallCollisionScene->UnRegistStaticActor( m_pWallStaticActor );
        GFL_SAFE_DELETE( m_pWallStaticActor );
      }
      GFL_SAFE_DELETE( m_pWallCollisionModel );
    }

    // �����R���W�����o�^�����E�j��
    {
      if( m_pStaticActor )
      {
        m_pCollisionScene->UnRegistStaticActor( m_pStaticActor );
        GFL_SAFE_DELETE( m_pStaticActor );
      }
      GFL_SAFE_DELETE( m_pCollisionModel );
    }
    
    m_pTerrainFactory->UnLoadHighModel( m_X, m_Z );
  }

  // high���f�����[�h���̏���
  if( m_IsLoading )
  {
    // �����`�F�b�N
    if( m_pTerrainFactory->CheckHighModelUnitState( m_X, m_Z ) == TerrainFactory::UnitState::LOAD_COMPLETION )
    {
#if 0
      if( m_pModelNodeTable[ m_LodType ] )
      {
        m_pModelNodeTable[ m_LodType ]->SetVisible( false );
      }
      m_pModelNodeTable[ Block::LodType::HIGH ] = m_pTerrainFactory->CreateHighModelNode( m_X, m_Z );
#endif

      if( m_pModelTable[ m_LodType ] )
      {
        UpdateVisibleImpl( m_LodType, false );
      }
      m_pModelTable[ Block::LodType::HIGH ] = m_pTerrainFactory->CreateHighModel( m_X, m_Z );
      m_ppEncountGrassModelTable[ Block::LodType::HIGH ] = m_pTerrainFactory->CreateEncountGrassModelTable( m_X, m_Z );

      m_pCollisionModel = m_pTerrainFactory->CreateHeightCollisionModel( m_X, m_Z );
      m_LodType = Block::LodType::HIGH;
      this->ChangeLodNotificationListener( m_LodType );
      m_IsLoading = false;

      // �����R���W���������E�o�^
      {
        // �����Ƃ����邩��
        if( m_pCollisionModel )
        {
          m_pStaticActor = m_pCollisionScene->CreateStaticActor();
          m_pStaticActor->CreateMesh( gfl2::math::Vector3(0.0f,0.0f,0.0f), m_pCollisionModel );
        }

        if( m_pStaticActor )
        {
          m_pCollisionScene->RegistStaticActor( m_pStaticActor );
        }
      }

      // �ǃR���W���������E�o�^
      {
        m_pWallCollisionModel = m_pTerrainFactory->CreateWallCollisionModel( m_X, m_Z );

        // �����Ƃ����邩��
        if( m_pWallCollisionModel )
        {
          m_pWallStaticActor = m_pWallCollisionScene->CreateStaticActor();
          m_pWallStaticActor->CreateMesh( gfl2::math::Vector3(0.0f,0.0f,0.0f), m_pWallCollisionModel );
          m_pWallCollisionScene->RegistStaticActor( m_pWallStaticActor );
        }

      }
    }
    else
    {
      return;
    }
  }

  // �����ڐ؂�ւ�����
  if( m_RequestLodType != m_LodType )
  {
    // high���f���̓��[�h������̂œ���
    if( m_RequestLodType == Block::LodType::HIGH )
    {
      // ���[�h���N�G�X�g�𓊂���
      if( m_pTerrainFactory->GetHighModelUnitUnRunnigNum() > 0 )
      {
        m_pTerrainFactory->LoadHighModelAsync( m_X, m_Z );
        m_IsLoading = true;
      }
    }
    else
    {
#if 0
      if( m_pModelNodeTable[ m_LodType ] )
      {
        m_pModelNodeTable[ m_LodType ]->SetVisible( false );
      }
#endif
      if( m_pModelTable[ m_LodType ] )
      {
        UpdateVisibleImpl( m_LodType, false );
      }

      m_LodType = m_RequestLodType;
      this->ChangeLodNotificationListener( m_LodType );
    }
  }

#if 0
  // Visible�X�V����
  if( m_pModelNodeTable[ m_LodType ] )
  {
    m_pModelNodeTable[ m_LodType ]->SetVisible( true );
    m_pModelNodeTable[ m_LodType ]->SetLocalTranslate( m_Pos );  // @todo ������Ȃ�����
  }
#endif

  // Visible����
  {
#if PM_DEBUG
    // �f�o�b�O�X�C�b�`�ɂ���\��
    b32 debugVisible = true;
    switch( m_LodType )    // �����ł݂�͖̂{����State
    {
    case Block::LodType::MIDDLE: debugVisible = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::TERRAIN_LOD ); break;
    case Block::LodType::LOW: debugVisible = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::TERRAIN_LOD ); break;
    case Block::LodType::HIGH: debugVisible = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::TERRAIN_HIGH ); break;
    }

    if( debugVisible )
    {
      m_InvisibleBit = ( m_InvisibleBit & (~INVISIBLE_DEBUG_DRAW_SWITCH) );
    }
    else
    {
      m_InvisibleBit = ( m_InvisibleBit | INVISIBLE_DEBUG_DRAW_SWITCH );
    }
#endif

    UpdateVisibleImpl( m_LodType, !m_InvisibleBit );
  }

  // Visible�X�V����
  if( m_pModelTable[ m_LodType ] )
  {
    m_pModelTable[ m_LodType ]->SetPosition( m_Pos );  // @todo ������Ȃ�����
    m_pModelTable[ m_LodType ]->UpdateAnimation();
  }
}
#endif //@saito_del
#if 0
/**
* @brief LOD�ύX����
*
* @param  lodLevel �ύX���������
*
* @return ����
*/
void TerrainBlock::ChangeLod( Block::LodType::Value lodLevel )
{
  if( lodLevel == Block::LodType::HIGH )
  {
    // return;
  }
  // High���f�������B�B�B
  if( m_IsHighModel == false && lodLevel == Block::LodType::HIGH )
  {
    return;
  }

  if( m_RequestLodType == lodLevel )
  {
    return;
  }

  m_RequestLodType = lodLevel;
}

/**
* @brief LOD�ύX�ʒm���󂯎�郊�X�i�[��o�^
* @note  ���X�i�[�̎����͓o�^���ŊǗ����Ă�������
*
* @param  pListener  �o�^���郊�X�i�[
*
* @return ����
*/
b32 TerrainBlock::RegistListener( Block::LodChangeStaticListener* pListener )
{
  // �󂫃X�y�[�X�ɐݒ�
  Block::LodChangeStaticListener** ppWork = m_ppLodChangeStaticListenerTable;
  for( u32 i = 0; i < m_LodChangeStaticListenerMax; i++, ppWork++ )
  {
    if( *ppWork == NULL )
    {
      *ppWork = pListener;
      pListener->RegistAfter( m_LodType );
      return true;
    }

  }

  return false;
}

/**
* @brief LOD�ύX�ʒm���󂯎�郊�X�i�[��o�^�폜
* @note  ���X�i�[�̎����͓o�^���ŊǗ����Ă�������
*
* @param  pListener  �o�^�폜���郊�X�i�[
*
* @return ����
*/
b32 TerrainBlock::UnRegistListener( Block::LodChangeStaticListener* pListener )
{
  Block::LodChangeStaticListener** ppWork = m_ppLodChangeStaticListenerTable;
  for( u32 i = 0; i < m_LodChangeStaticListenerMax; i++, ppWork++ )
  {
    if( *ppWork == pListener )
    {
      *ppWork = NULL;
      return true;
    }

  }

  return false;
}
#endif //@saito_del
//
// private
//
#if 0
/**
* @brief LOD�ύX�����X�i�[�ɒʒm
*
* @param  lodType �ύX��̏��
*
* @return ����
*/
void TerrainBlock::ChangeLodNotificationListener( Block::LodType::Value lodType )
{
  Block::LodChangeStaticListener** ppWork = m_ppLodChangeStaticListenerTable;
  for( u32 i = 0; i < m_LodChangeStaticListenerMax; i++, ppWork++ )
  {
    if( *ppWork )
    {
      (*ppWork)->ChangeLod( lodType );
    }
  }
}

/**
* @brief �n�`���f���̕\���E��\���؂�ւ�
*
* @param lodType  �ΏۂƂȂ�LOD���f��
* @param visible  true �\�� false ��\��
*
* @return ����
*/
void TerrainBlock::UpdateVisibleImpl( Block::LodType::Value lodType, b32 visible )
{
  if( m_pModelTable[ lodType ] == NULL )
  {
    return;
  }

  if( m_pModelTable[ lodType ]->IsVisible() == visible )
  {
    return;
  }

  m_pModelTable[ lodType ]->SetVisible( visible );
  for( int i=0; i<m_pTerrainFactory->GetEncountGrassModelTableNum(); ++i )
  {
    if(m_ppEncountGrassModelTable[ lodType ] && m_ppEncountGrassModelTable[ lodType ][i])
    {
      m_ppEncountGrassModelTable[ lodType ][i]->SetVisible( visible ); 
    }
  }

#if 0
  // @todo �d�������H
  if( visible )
  {
    gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pModelInstanceNodeTable[ lod ] );
  }
  else
  {
    gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild( m_pModelInstanceNodeTable[ lod ] );
  }
#endif
}
#endif //@saito_del

GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );