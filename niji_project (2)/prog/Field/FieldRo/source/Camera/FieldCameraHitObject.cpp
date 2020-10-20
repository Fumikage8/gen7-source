/**
* @brief  フィールドカメラを管理するクラス
* @file   FieldCameraHitObject.cpp
* @author ikuta_junya
* @data   2016.03.03
*/

// field
#include "Field/FieldRo/include/Camera/FieldCameraHitObject.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

static const u32 USE_CONSTANT_COLOR( 5 );



// 設定
CameraHitObject::Setting s_HitObjectSetting =
{
  6,
  gfl2::math::Easing::LINEAR,
  10,
  gfl2::math::Easing::LINEAR,
};

// 操作前に覚えておくパラメーター
struct CameraHitObject::FaceMemoryData
{
  f32 alpha;
};

/**
* @brief コンストラクタ
*
* @return 無し
*/ 
CameraHitObject::CameraHitObject( void ) :

  // 制御
  m_Mode( Mode::VISIBLE )
  ,m_IsHit( false )
  ,m_IsHideStop( false )
  ,m_OneTimeHideReq( false )
  ,m_State( State::DEF )
  ,m_pMemoryData( NULL )
  ,m_pModelInstanceNode( NULL )
{
  for( u32 i = 0; i < MeshType::MAX; i++ )
  {
    m_pMeshLink[ i ] = NULL;
  }
  m_ValueControllerForAlpha.Clear();
}

/**
* @brief デストラクタ
*
* @return 無し
*/
CameraHitObject::~CameraHitObject( void )
{
  Terminate();
}

/**
* @brief 初期化処理
*
* @param  rInitDesc   初期化設定
*
* @return 無し
*/
void CameraHitObject::Initialize( const InitializeDescription& rInitDesc )
{
  m_pMeshLink[ MeshType::BASE ] = rInitDesc.m_pMeshLink;
  m_pModelInstanceNode = rInitDesc.m_pModelInstanceNode;
  m_ValueControllerForAlpha.Set( 1.0f );

  // アルファ遷移モードのみマテリアル操作を行う
  m_Mode = rInitDesc.m_Mode;
  if( m_Mode == Mode::ALPHA )
  {
    m_pMeshLink[ MeshType::ALPHA ] = rInitDesc.m_pMeshLinkAfter;
    m_pMemoryData = GFL_NEW_ARRAY( rInitDesc.m_pHeap ) CameraHitObject::FaceMemoryData [ m_pMeshLink[ MeshType::ALPHA ]->GetFaceNum() ];
    MemoryMaterialDefault();

    // 透過前のメッシュのみを表示状態にする
    SetVisible( MeshType::BASE, true );
    SetVisible( MeshType::ALPHA, false );
  }
}

/**
* @brief 終了処理
*
* @return 無し
*/
void CameraHitObject::Terminate( void )
{
  GFL_SAFE_DELETE( m_pMemoryData );
  for( u32 i = 0; i < MeshType::MAX; i++ )
  {
    m_pMeshLink[ i ] = NULL;
  }
}

/**
* @brief 更新処理
*
* @return 無し
*/
void CameraHitObject::Update( void )
{
  switch( m_Mode )
  {
  case Mode::VISIBLE:
    {
      UpdateModeVisible();
      break;
    }

  case Mode::ALPHA:
    {
      UpdateModeAlpha();
      break;
    }
  }

  // リクエスト破棄
  m_OneTimeHideReq = false;
}


/**
* @brief 更新事前処理
*
* @return 無し
*/
void CameraHitObject::PreUpdate( void )
{
  m_IsHit = false;
}

/**
* @brief Hitした際に実行する
*
* @return 無し
*/
void CameraHitObject::Hit( void )
{
  // @fix GFMcat[4831]イベント中に透過メッシュが不要に消える、対処。透過機能をOFFする機能を追加。スクリプトに公開
  if( m_IsHideStop )
  {
    return;
  }

  m_IsHit = true;
}

/**
* @brief カメラ接触でメッシュの非表示にする機能を有効化する
* @fix GFMcat[4831]イベント中に透過メッシュが不要に消える、対処。透過機能をOFFする機能を追加。スクリプトに公開
*
* @param oneTime true:透過遷移を挟さまない false:透過遷移を挟む
*/
void CameraHitObject::HideOn( b32 oneTime )
{
  m_IsHideStop = false;
  if( oneTime )
  {
    m_OneTimeHideReq = true;
  }
}

/**
* @brief カメラ接触でメッシュの非表示にする機能を無効化する
* @fix GFMcat[4831]イベント中に透過メッシュが不要に消える、対処。透過機能をOFFする機能を追加。スクリプトに公開
*
* @param oneTime true:透過遷移を挟さまない false:透過遷移を挟む
*/
void CameraHitObject::HideOff( b32 oneTime )
{
  m_IsHideStop = true;

  // 透過中なら非透過に一瞬で戻す
  if( oneTime && m_State != State::DEF )
  {
    switch( m_Mode )
    {
    case Mode::VISIBLE:
      {
        SetVisible( MeshType::BASE, true );
        m_State = State::DEF;
        break;
      }

    case Mode::ALPHA:
      {
        SetupAlphaUpEnd();
        ApplyAlphaToMaterial();
        break;
      }
    }
  }
}

#if PM_DEBUG
/**
* @brief 設定構造体のポインタを取得
*
* @return ポインタ
*/
CameraHitObject::Setting* CameraHitObject::GetSettingPtr( void )
{
  return &s_HitObjectSetting;
}

/**
* @brief 情報出力 
*
* @return 無し
*/
void CameraHitObject::Dump( void ) const
{
  if( m_pModelInstanceNode == NULL )
  {
    return;
  }

  GFL_PRINT( "-- CameraHitObject::Dump -- \n" );

  // @note GetNodeNameだと名前が取れないっぽい
  // GFL_PRINT( "ModelInstanceNodeName[%s] \n", m_pModelInstanceNode->GetNodeName() );   
  switch( m_Mode )
  {
  case Mode::VISIBLE:
    {
      GFL_PRINT( "Type[VISIBLE]\n" );
      DumpMesh( m_pMeshLink[ MeshType::BASE ] );
      break;
    }

  case Mode::ALPHA:
    {
      GFL_PRINT( "Type[ALPHA]\n" );
      DumpMesh( m_pMeshLink[ MeshType::BASE ] );
      DumpMesh( m_pMeshLink[ MeshType::ALPHA ] );
      break;
    }
  }
  GFL_PRINT( "--------------------------- \n" );
}

#endif

//
// private:
//

/**
* @brief 更新処理(visibleの切り替えのみ)
*
* @return 無し
*/
void CameraHitObject::UpdateModeVisible( void )
{
  switch( m_State )
  {
  case State::DEF:
    {
      if( m_IsHit )
      {
        SetVisible( MeshType::BASE, false );
        m_State = State::HIDE;
      }
      break;
    }

  case State::HIDE:
    {
      if( !m_IsHit )
      {
        SetVisible( MeshType::BASE, true );
        m_State = State::DEF;
      }
      break;
    }
  }
}

/**
* @brief 更新処理(アルファ遷移)
*
* @return 無し
*/
void CameraHitObject::UpdateModeAlpha( void )
{
  if( m_IsHit && m_OneTimeHideReq )
  {
    // 透過完了、非表示状態にする
    SetupAlphaDownEnd();
    ApplyAlphaToMaterial();

    return;
  }

  switch( m_State )
  {
  case State::DEF:
    {
      if( m_IsHit )
      {        
        SetupAlphaDownStart( 1.0f, true );
        ApplyAlphaToMaterial();
      }
      break;
    }

  case State::DOWN_ALPHA:
    {
      if( m_IsHit )
      {
        m_ValueControllerForAlpha.Update();
        if( m_ValueControllerForAlpha.IsEnd() )
        {
          // 透過完了、非表示状態にする
          SetupAlphaDownEnd();
          ApplyAlphaToMaterial();
        }
        else
        {
          ApplyAlphaToMaterial();
        }
      }
      else
      {
        // 復帰状態にする
        SetupAlphaUpStart( m_ValueControllerForAlpha.GetValue(), false );
        ApplyAlphaToMaterial();
      }

      break;
    }

  case State::UP_ALPHA:
    {
      if( m_IsHit )
      {
        // 透過状態にする
        SetupAlphaDownStart( m_ValueControllerForAlpha.GetValue(), false );
        ApplyAlphaToMaterial();
      }
      else
      {
        m_ValueControllerForAlpha.Update();
        if( m_ValueControllerForAlpha.IsEnd() )
        {
          // 復帰終了、通常状態に戻す
          SetupAlphaUpEnd();
          ApplyAlphaToMaterial();
        }
        else
        {
          ApplyAlphaToMaterial();
        }
      }

      break;
    }

  case State::HIDE:
    {
      if( !m_IsHit )
      {
        // 復帰
        SetupAlphaUpStart( 0.0f, true );
        ApplyAlphaToMaterial();
      }

      break;
    }
  }
}

/**
* @brief 半透明へ遷移する設定を行う
*
* @param  nowValue        開始時のアルファ
* @param  updateMaterial  マテリアル設定を半透明に書き換えるか
*
* @return 無し
*/
void CameraHitObject::SetupAlphaDownStart( f32 nowValue, b32 updateMaterial )
{
  m_ValueControllerForAlpha.Move(
    nowValue,
    0.0f,
    s_HitObjectSetting.downFrame,
    static_cast< gfl2::math::Easing::EaseFunc >( s_HitObjectSetting.downEaseFunc )
    );

  m_State = State::DOWN_ALPHA;
  
  SetVisible( MeshType::ALPHA, true );
  SetVisible( MeshType::BASE, false );

  if( updateMaterial )
  {
    // SetupMaterialBlendOn();
  }
}

/**
* @brief 透過遷移完了処理(Down完了)
*
* @return 無し
*/
void CameraHitObject::SetupAlphaDownEnd( void )
{
  SetVisible( MeshType::ALPHA, false );
  SetVisible( MeshType::BASE, false );

  m_State = State::HIDE;
}

/**
* @brief 透過遷移完了処理(UP完了)
*
* @return 無し
*/
void CameraHitObject::SetupAlphaUpEnd( void )
{
  SetVisible( MeshType::ALPHA, false );
  SetVisible( MeshType::BASE, true );
  
  SetupMaterialDefault();
  m_State = State::DEF;
}

/**
* @brief 不透明へ遷移する設定を行う
*
* @param  nowValue        開始時のアルファ
* @param  updateMaterial  マテリアル設定を半透明に書き換えるか
*
* @return 無し
*/
void CameraHitObject::SetupAlphaUpStart( f32 nowValue, b32 updateMaterial )
{
  m_ValueControllerForAlpha.Move(
    nowValue,
    1.0f,
    s_HitObjectSetting.upFrame,
    static_cast< gfl2::math::Easing::EaseFunc >( s_HitObjectSetting.upEaseFunc )
    );

  m_State = State::UP_ALPHA;
  
  SetVisible( MeshType::ALPHA, true );
  SetVisible( MeshType::BASE, false );

  if( updateMaterial )
  {
    // SetupMaterialBlendOn();
  }
}

/**
* @brief 現在のマテリアル設定を通常設定として保存する
*
* @return 無し
*/
void CameraHitObject::MemoryMaterialDefault( void )
{
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMeshLink = m_pMeshLink[ MeshType::ALPHA ];

  for( u32 f = 0; f < pMeshLink->GetFaceNum(); f++ )
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMat = m_pModelInstanceNode->GetMaterialInstanceNodeByNameHash( pMeshLink->GetFaceData( f )->m_MaterialNameHash );
    m_pMemoryData[ f ].alpha = pMat->GetAttributeParam()->m_ConstantColor[ USE_CONSTANT_COLOR ].a;
  }
}

/**
* @brief マテリアル設定を通常設定にする
*
* @return 無し
*/
void CameraHitObject::SetupMaterialDefault( void )
{
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMeshLink = m_pMeshLink[ MeshType::ALPHA ];
  
  const CameraHitObject::FaceMemoryData* pMemoryData = &m_pMemoryData[0];
  for( u32 f = 0; f < pMeshLink->GetFaceNum(); f++, pMemoryData++ )
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMat = m_pModelInstanceNode->GetMaterialInstanceNodeByNameHash( pMeshLink->GetFaceData( f )->m_MaterialNameHash );
    pMat->GetAttributeParam()->m_ConstantColor[ USE_CONSTANT_COLOR ].a = pMemoryData->alpha;
  }
}

/**
* @brief 内部制御のアルファ値をマテリアルに反映する
*
* @return 無し
*/
void CameraHitObject::ApplyAlphaToMaterial(void )
{
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMeshLink = m_pMeshLink[ MeshType::ALPHA ];
  
  const CameraHitObject::FaceMemoryData* pMemoryData = &m_pMemoryData[0];
  f32 t = m_ValueControllerForAlpha.GetValue();

  for( u32 f = 0; f < pMeshLink->GetFaceNum(); f++, pMemoryData++ )
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMat = m_pModelInstanceNode->GetMaterialInstanceNodeByNameHash( pMeshLink->GetFaceData( f )->m_MaterialNameHash );
    pMat->GetAttributeParam()->m_ConstantColor[ USE_CONSTANT_COLOR ].a = pMemoryData->alpha * t; // 0～255
  }
}

/**
* @brief 表示・非表示を設定にする
*
* @param meshType 操作対象メッシュ
* @param flag true表示、false非表示
*
* @return 無し
*/
void CameraHitObject::SetVisible( MeshType::Value meshType, b32 flag )
{
  if( m_pMeshLink[ meshType ] && m_pMeshLink[ meshType ]->IsVisible() != flag )
  {
    m_pMeshLink[ meshType ]->SetVisible( flag );
  }
}

#if PM_DEBUG

/**
* @brief メッシュ情報出力
*
* @param 対象メッシュ
*
* @return 無し
*/
void CameraHitObject::DumpMesh( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMesh ) const
{
  if( pMesh )
  {
    GFL_PRINT( "MeshName[%s]\n", pMesh->GetName() );

    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* pMeshLink = pMesh;

    for( u32 f = 0; f < pMeshLink->GetFaceNum(); f++ )
    {
      gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMat = m_pModelInstanceNode->GetMaterialInstanceNodeByNameHash( pMeshLink->GetFaceData( f )->m_MaterialNameHash );
      GFL_PRINT( "-MaterialName[%s]\n", pMat->GetName() );
    }
  }
}

#endif


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );