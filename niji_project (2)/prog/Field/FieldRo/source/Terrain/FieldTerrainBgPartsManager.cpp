/**
* @brief  フィールドの地形BGパーツ管理クラス
* @file   FieldTerrainBgPartsManager.cpp
* @author ikuta_junya
* @data   2015.09.04
*/

// field
#include "Field/FieldRo/include/Terrain/FieldTerrainBgPartsManager.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainFactory.h"
#include "Field/FieldRo/include/Terrain/Block/FieldTerrainBlockTypes.h"

#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );

// @fix GFCHECK[8230]:ワーニング対処。宣言したけど使われていない変数をコメントアウト、nijiではBGPartsをカリングしない。
// const static u32 INVISIBLE_CULLING = 0x01;
const static u32 INVISIBLE_DRAW_SWITCH = 0x01 << 1;
const static u32 INVISIBLE_DEBUG_DRAW_SWITCH = 0x01 << 2;


class TerrainBgParts
{
public:

  struct State
  {
    enum Value
    {
      LOADING = 0,
      UPDATE,
      UNLOADING,
      FINISH
    };

  };

  TerrainBgParts( u32 no, gfl2::heap::HeapBase* pDeviceHeap, TerrainFactory* pTerrainFactory, System::nijiAllocator* pDeviceAllocator ) :
    m_State( State::LOADING )
    ,m_No( 0 )
    ,m_InvisibleBit( 0 )
    ,m_UnLoadReq( 0 )
    ,m_pModel( NULL )
    ,m_pTerrainFactory( pTerrainFactory )
    ,m_pDeviceHeap( pDeviceHeap )
    ,m_pDeviceAllocator( pDeviceAllocator )
  {
    m_No = no;

    // ロードリクエスト発行
    m_pTerrainFactory->LoadBgPartsAsync( m_No, pDeviceHeap, pDeviceAllocator );
  }

  ~TerrainBgParts( void )
  {

  }

  /**
  * @brief 更新処理
  *
  * @param localAnimationFrame;      Localアニメの現在フレーム
  * @param oneDayAnimationFrame;     1日アニメの現在フレーム
  *
  * @return 無し
  */
  void Update( f32 localAnimationFrame, f32 oneDayAnimationFrame, const gfl2::math::Matrix44* pCullingMatrixTable )
  {
    UpdateResourceControl();
    
    switch( m_State )
    {
    case State::UPDATE:
      {
        // Visible制御
        // @fix GFCHECK[8230]:ワーニング対処、デバッグ非表示機能を追加し未使用警告を回避
#if PM_DEBUG
        if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::TERRAIN_BG_PARTS ) == 0 )
        {
          m_InvisibleBit = ( m_InvisibleBit | INVISIBLE_DEBUG_DRAW_SWITCH );
        }
        else
        {
          m_InvisibleBit = ( m_InvisibleBit & (~INVISIBLE_DEBUG_DRAW_SWITCH) );
        }
#endif
        UpdateVisibleImpl( !m_InvisibleBit );

        // モデル更新処理
        if( m_pModel && m_InvisibleBit == false )
        {
          m_pModel->SetAnimationFrame( localAnimationFrame, Block::AnimationSlotNo::LOCAL );
          m_pModel->SetAnimationFrame( oneDayAnimationFrame, Block::AnimationSlotNo::ONE_DAY );
          m_pModel->UpdateAnimation();
        }

        break;
      }
    } // switch
  }

  /**
  * @brief フィールド開始用更新処理
  *
  * @return true Setup継続 false 完了
  */
  void UpdateStartSetup( void )
  {
    UpdateResourceControl();
  }

  /**
  * @brief フィールド終了用更新処理
  *
  * @return true Setup継続 false 完了
  */
  void UpdateEndSetup( void )
  {
    UpdateResourceControl();
  }

  void SetVisible( b32 flag )
  {
    // モデル生成前でのbitを更新

    // bit更新
    if( flag )
    {
      m_InvisibleBit = ( m_InvisibleBit & (~INVISIBLE_DRAW_SWITCH) );
    }
    else
    {
      m_InvisibleBit = ( m_InvisibleBit | INVISIBLE_DRAW_SWITCH );
    }

    // モデルに反映
    switch( m_State )
    {
    case State::LOADING:
    case State::UPDATE:
      {
        UpdateVisibleImpl( !m_InvisibleBit );
        break;
      }

    case State::UNLOADING:
    case State::FINISH:
      {
        break;
      }
    }
  }

  u32 GetNo( void ) const
  {
    return m_No;
  }

  b32 IsLoading( void ) const
  {
    return ( m_State == State::LOADING );
  }

  // 自身を破棄する際にtrueを返す
  b32 IsFinish( void ) const
  {
    return ( m_State == State::FINISH );
  }

  // 破棄リクエスト
  void UnLoad( void )
  {
    m_UnLoadReq = 1;
  }

private:

  void UpdateResourceControl( void )
  {
    switch( m_State )
    {
    case State::LOADING:
      {
        // ロード完了
        if( m_pTerrainFactory->CheckBgPartsUnitState( m_No ) == TerrainFactory::UnitState::LOAD_COMPLETION )
        {
          // 生成
          m_pModel = m_pTerrainFactory->CreateBgParts( m_No, m_pDeviceHeap, m_pDeviceAllocator );

          // 状態変更
          m_State = State::UPDATE;

          // 最新の状態を反映するため、そのまま更新に流す
          // break;
        }
        else
        {
          break;
        }

      }

    case State::UPDATE:
      {
        if( m_UnLoadReq )
        {
          // 破棄リクエストを受理
          this->UpdateVisibleImpl( false );
          
          // 状態変更
          m_State = State::UNLOADING;
        }

        break;
      }

    case State::UNLOADING:
      {
        if( m_pModel->CanDestroy() )
        {
          // 破棄を実行
          m_pTerrainFactory->DeleteModel( &m_pModel );
          m_pTerrainFactory->UnLoadBgParts( m_No );

          // 状態変更
          m_State = State::FINISH;
        }

        break;
      }
    
    case State::FINISH:
      {
        // State::FINISHになった直後に破棄されるので、ここにはこないはず
        // m_Noを覚えているが、State::FINISHになった直後に破棄されるので問題ない
        break;
      }
    }
  }


  /**
  * @brief 表示・非表示切り替え
  *
  * @param visible  true 表示 false 非表示
  *
  * @return 無し
  */
  void UpdateVisibleImpl( b32 visible )
  {
    // モデル生成前は無視
    if( m_pModel == NULL )
    {
      return ;
    }

    // 同じなら無視
    if( m_pModel->IsVisible() == visible )
    {
      return ;
    }

    // 反映
    m_pModel->SetVisible( visible );
  }

  State::Value m_State;
  u32 m_No;               // BgPartsNo
  u8  m_InvisibleBit;
  u8  m_UnLoadReq : 1;
  poke_3d::model::BaseModel* m_pModel;

  // 外部オブジェクト
  TerrainFactory*  m_pTerrainFactory;             // 地形生成機能
  gfl2::heap::HeapBase* m_pDeviceHeap;              // モデルのロード・生成に使用するヒープ(動的)
  System::nijiAllocator* m_pDeviceAllocator;
};


/**
* @brief コンストラクタ
*
* @return 無し
*/ 
TerrainBgPartsManager::TerrainBgPartsManager( void ) :
  m_CreateEnable( false )
  ,m_pBgPartsList( NULL )
  ,m_pTerrainFactory( NULL )
{
}

/**
* @brief デストラクタ
*
* @return 無し
*/
TerrainBgPartsManager::~TerrainBgPartsManager( void )
{
  // 非同期開放が必要なので、必ず開放処理で破棄する
}

/**
* @brief 初期化処理
*
* @param  rInitDesc   初期化設定
*
* @return 無し
*/
void TerrainBgPartsManager::Initialize( const InitializeDescription& rInitDesc )
{
  // 記憶
  m_pTerrainFactory = rInitDesc.m_pTerrainFactory;
  
  // リスト生成
  m_pBgPartsList = GFL_NEW( rInitDesc.m_pSystemHeap ) BgPartsList( rInitDesc.m_pSystemHeap );

  // 設定
  m_CreateEnable = false;
}

/**
* @brief 終了処理
*
* @return 無し
*/
void TerrainBgPartsManager::Terminate( void )
{
  // リスト破棄
  GFL_SAFE_DELETE( m_pBgPartsList );

  // クリア
  m_pTerrainFactory = NULL;
}

/**
* @brief 更新処理(ロード状況を監視、リソースの生成)
*
* @return 無し
*/
void TerrainBgPartsManager::Update( f32 localAnimationFrame, f32 oneDayAnimationFrame, const gfl2::math::Matrix44* pCullingMatrixTable )
{
  // 各パーツを更新
  BgPartsListIterator it( m_pBgPartsList->Begin() );
  BgPartsListIterator end( m_pBgPartsList->End() );
  while( it != end )
  {
    (*it)->Update( localAnimationFrame, oneDayAnimationFrame, pCullingMatrixTable );

    if( (*it)->IsFinish() )
    {
      // 破棄
      TerrainBgParts* ptr = (*it);
      it = m_pBgPartsList->Erase( it );
      GFL_SAFE_DELETE( ptr );
    }
    else
    {
      it++;
    }
  }
}

/**
* @brief フィールド開始用更新処理
*
* @return true Setup継続 false 完了
*/
b32 TerrainBgPartsManager::UpdateStartSetup( void )
{
  // 各パーツを更新
  BgPartsListIterator it( m_pBgPartsList->Begin() );
  BgPartsListIterator end( m_pBgPartsList->End() );
  while( it != end )
  {
    (*it)->UpdateStartSetup();

    if( (*it)->IsFinish() )
    {
      // 破棄
      TerrainBgParts* ptr = (*it);
      it = m_pBgPartsList->Erase( it );
      GFL_SAFE_DELETE( ptr );
    }
    else
    {
      it++;
    }
  }

  return this->IsLoading();
}

/**
* @brief フィールド終了用更新処理
*
* @return true Setup継続 false 完了
*/
b32 TerrainBgPartsManager::UpdateEndSetup( void )
{
  // 各パーツを更新
  BgPartsListIterator it( m_pBgPartsList->Begin() );
  BgPartsListIterator end( m_pBgPartsList->End() );
  while( it != end )
  {
    (*it)->UpdateEndSetup();

    if( (*it)->IsFinish() )
    {
      // 破棄
      TerrainBgParts* ptr = (*it);
      it = m_pBgPartsList->Erase( it );
      GFL_SAFE_DELETE( ptr );
    }
    else
    {
      it++;
    }
  }

  // 読み込み中なら継続
  return this->IsLoading();
}

/**
* @brief 指定データ番号のBgPartsが有効かチェックする
*
* @param  no        データ番号
*
* @return true 有効   false 無効
*/
b32 TerrainBgPartsManager::CheckBgPartsNoVailed( u32 no ) const
{
  return ( m_pTerrainFactory->CheckBgPartsNoVailed( no ) );
}

/**
* @brief 指定データ番号のBgPartsの非同期読み込み開始
*
* @param  no                データ番号
* @param  pDeviceHeap       読み込み及びインスタンス生成に使用するヒープ
* @param  pDeviceAllocator  インスタンス生成に使用するアロケータ
*
* @return 成否( データ番号が不正値、既に存在する場合はfalseを返し何もしない )
*/
b32 TerrainBgPartsManager::LoadBgPartsAsync( u32 no, gfl2::heap::HeapBase* pDeviceHeap, System::nijiAllocator* pDeviceAllocator )
{
  // 受付不可状態
  if( m_CreateEnable == false )
  {
    return false;
  }

  // 無効No
  if( this->CheckBgPartsNoVailed( no ) == false )
  {
    return false;
  }

  // 既にある
  if( this->CheckBgPartsExist( no ) )
  {
    return false;
  }

  // 生成開始 + 管理リスト追加
  TerrainBgParts* ptr = GFL_NEW( pDeviceHeap ) TerrainBgParts( no, pDeviceHeap, m_pTerrainFactory, pDeviceAllocator );
  m_pBgPartsList->PushBack( ptr, pDeviceHeap );

  return true;
}

/**
* @brief 全てのBgPartsを破棄する
*
* @return 無し
*/
void TerrainBgPartsManager::UnLoadBgPartsAll( void )
{
  BgPartsListIterator it( m_pBgPartsList->Begin() );
  BgPartsListIterator end( m_pBgPartsList->End() );
  while( it != end )
  {
    (*it)->UnLoad();
    it++;
  }
}

/**
* @brief 指定データ番号のBgPartsを破棄する
*
* @param  no        データ番号
*
* @return 成否
*/
b32 TerrainBgPartsManager::UnLoadBgParts( u32 no )
{
  TerrainBgParts* pParts = SearchBgParts( no );
  if( pParts )
  {
    pParts->UnLoad();
    return true;
  }

  return false;
}

/**
* @brief 管理しているBgParts数を取得する
*        ロード中、破棄中を含む
*
* @return BgParts数
*/
u32 TerrainBgPartsManager::CountBgParts( void ) const
{
  return m_pBgPartsList->Size();
}

/**
* @brief BGパーツを読み込んでいる最中か取得する
*
* @return true 読み込んでいる最中
*/
b32 TerrainBgPartsManager::IsLoading( void ) const
{
  BgPartsListIterator it( m_pBgPartsList->Begin() );
  BgPartsListIterator end( m_pBgPartsList->End() );
  while( it != end )
  {
    if( (*it)->IsLoading() )
    {
      return true;
    }
    it++;
  }

  return false;
}

/**
* @brief 指定データ番号のBgPartsを表示フラグを設定する
*
* @param  no        データ番号
* @param  flag      true: 表示 false: 非表示
*
* @return 無し
*/
void TerrainBgPartsManager::SetCreateEnable( b32 flag )
{
  m_CreateEnable = flag;
}

/**
* @brief 指定データ番号のBgPartsを表示フラグを設定する
*
* @param  no        データ番号
* @param  flag      true: 表示 false: 非表示
*
* @return 無し
*/
void TerrainBgPartsManager::SetVisible( u32 no, b32 flag )
{
  TerrainBgParts* pParts = SearchBgParts( no );
  if( pParts )
  {
    pParts->SetVisible( flag );
  }
}


/**
* @brief 指定データ番号のBgPartsが既にあるか取得する
*
* @param  no        データ番号
*
* @return true ある   false ない
*/
b32 TerrainBgPartsManager::CheckBgPartsExist( u32 no ) const
{
  return ( SearchBgParts( no ) != NULL );
}

/**
* @brief 指定データ番号のBgPartsを取得する
*
* @param  no        データ番号
*
* @retval NULL 無い
* @return BgParts
*/
TerrainBgParts* TerrainBgPartsManager::SearchBgParts( u32 no )
{
  BgPartsListIterator it( m_pBgPartsList->Begin() );
  BgPartsListIterator end( m_pBgPartsList->End() );
  while( it != end )
  {
    if( (*it)->GetNo() == no )
    {
      return (*it);
    }
    it++;
  }

  return NULL;
}

/**
* @brief 指定データ番号のBgPartsを取得する
*
* @param  no        データ番号
*
* @retval NULL 無い
* @return BgParts
*/
const TerrainBgParts* TerrainBgPartsManager::SearchBgParts( u32 no ) const
{
  BgPartsListIterator it( m_pBgPartsList->Begin() );
  BgPartsListIterator end( m_pBgPartsList->End() );
  while( it != end )
  {
    if( (*it)->GetNo() == no )
    {
      return (*it);
    }
    it++;
  }

  return NULL;
}

GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );