//==============================================================================
/**
 * @file	JoinFestaFacilitiesModelResourceManager.cpp
 * @brief	ジョインフェスタ施設モデル
 * @author	ichiraku_katsuhiko
 * @date	2015/10/22　12:52
 */
// =============================================================================
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesModelResourceManager.h"

// ライブラリ
#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h> // リソース生成のため
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>

// niji
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"

// arc
#include "arc_def_index/arc_def.h"
#include "niji_conv_header/gimmick/join_festa/join_festa_ctr.h"
#include "niji_conv_header/gimmick/join_festa/join_festa_mot.h"
#include "niji_conv_header/gimmick/join_festa/gimmick_join_festa_garc_ctr.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

// コンストラクタ
JoinFestaFacilitiesModelResourceManager::JoinFestaFacilitiesModelResourceManager() :
  m_isLoaded(false)
  ,m_loadSeq(0)
  ,m_pResourceBuff(NULL)
{
}

// デストラクタ
JoinFestaFacilitiesModelResourceManager::~JoinFestaFacilitiesModelResourceManager()
{
}


//=============================================================================
/**
 *  publicメソッド
 */
//=============================================================================
/**
 * @brief   読み込み
 * @param   heap      ヒープ
 *
 * @return  trueで読み込み完了
 */
bool JoinFestaFacilitiesModelResourceManager::Load( gfl2::heap::HeapBase * heap )
{
  if( m_isLoaded == true )
  {
    return true;
  }

  switch( m_loadSeq ){
  case 0:   // ファイルオープン
    app::util::FileAccessor::FileOpen( ARCID_JOIN_FESTA_GIMMICK, heap, true );
    m_loadSeq++;
    /* FALL THROUGH */
  case 1:   // ファイルロード
    if( app::util::FileAccessor::IsFileOpen( ARCID_JOIN_FESTA_GIMMICK ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_JOIN_FESTA_GIMMICK,
      0,
      &m_pResourceBuff,
      heap,
      true, 128 );
    m_loadSeq++;
    /* FALL THROUGH */
  case 2:   // ファイルクローズ
    if( app::util::FileAccessor::IsFileLoad( &m_pResourceBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( ARCID_JOIN_FESTA_GIMMICK, heap );
    m_loadSeq++;
    /* FALL THROUGH */
  case 3:   // ファイル操作終了
    if( app::util::FileAccessor::IsFileClose( ARCID_JOIN_FESTA_GIMMICK ) == false )
    {
      break;
    }

    // リンカーに設定
    m_BinLinker.Initialize( m_pResourceBuff );
    m_BinLinker.Initialize( m_BinLinker.GetData( BL_IDX_GIMMICK_JOIN_FESTA_GARC_CTR_JOIN_FESTA_CTR_PACK ) );

#if PM_DEBUG
    {
      u32 dataMax = m_BinLinker.GetDataMax();
      ICHI_PRINT(">ICHI dataMax:%d \n", dataMax);
      for( int i=0; i<dataMax; i++ )
      {
        ICHI_PRINT(">ICHI dataSize[%d]:%d \n", i, m_BinLinker.GetDataSize(i) );
      }
    }
#endif
    // 読み込み完了
    m_isLoaded = true;

    // 終了
    m_loadSeq = 0;
    return true;

  default:
    GFL_ASSERT( 0 );
    break;
  }

  return false;
}

/**
 * @brief   破棄
 * @return  trueで破棄完了
 */
bool JoinFestaFacilitiesModelResourceManager::Terminate()
{
  if( m_isLoaded == true )
  {
    m_BinLinker.Initialize(NULL);
    GflHeapSafeFreeMemory(m_pResourceBuff);
  }
  return true;
}


/**
 * @brief   モデルリソース取得
 */
void* JoinFestaFacilitiesModelResourceManager::GetModel( JoinFestaScript::FacilitiesType type )
{
  if( m_isLoaded == false )
  {
    GFL_ASSERT(0);
    return NULL;
  }

  // FacilitiesType→リソースインデックス
  u32 idx=0;
  switch( type ){
  case JoinFestaScript::FACILITIES_TYPE_LOTTERY:       //! くじ引き屋
    idx = BL_IDX_JOIN_FESTA_CTR_JF_KUJI_CTR_GFBMDLP;
    break;
  case JoinFestaScript::FACILITIES_TYPE_AIR_TOY:      //! エア遊具
    idx = BL_IDX_JOIN_FESTA_CTR_JF_BARU_CTR_GFBMDLP;
    break;
  case JoinFestaScript::FACILITIES_TYPE_FUN_HOUSE:    //! ビックリハウス
    idx = BL_IDX_JOIN_FESTA_CTR_JF_BIKURI_CTR_GFBMDLP;
    break;
  case JoinFestaScript::FACILITIES_TYPE_STALL:      //! 屋台
	  idx = BL_IDX_JOIN_FESTA_CTR_JF_YATAI_CTR_GFBMDLP;
    break;
  case JoinFestaScript::FACILITIES_TYPE_GIFT_SHOP:     //! ギフトショップ
    idx = BL_IDX_JOIN_FESTA_CTR_JF_OMIYA_CTR_GFBMDLP;
    break;
  case JoinFestaScript::FACILITIES_TYPE_FORTUNE:     //! 占い
    idx = BL_IDX_JOIN_FESTA_CTR_JF_URA_CTR_GFBMDLP;
    break;
  case JoinFestaScript::FACILITIES_TYPE_DYEING:   //! 染物屋
    idx = BL_IDX_JOIN_FESTA_CTR_JF_SOME_CTR_GFBMDLP;
    break;
  case JoinFestaScript::FACILITIES_TYPE_TRADE:      //! こうかんじょ(momiji追加)
    idx = BL_IDX_JOIN_FESTA_CTR_JF_NEKO_CTR_GFBMDLP;
    break;
  }

  return m_BinLinker.GetData( idx );
}

/**
 * @brief   城のモデルリソース取得
 */
void* JoinFestaFacilitiesModelResourceManager::GetCastleModel()
{
  if( m_isLoaded == false )
  {
    GFL_ASSERT(0);
    return NULL;
  }
  return m_BinLinker.GetData( BL_IDX_JOIN_FESTA_CTR_JF_SHIRO_CTR_GFBMDLP );
}

/**
 * @brief   RR団の城のモデルリソース取得
 */
void* JoinFestaFacilitiesModelResourceManager::GetRRCastleModel()
{
  if( m_isLoaded == false )
  {
    GFL_ASSERT(0);
    return NULL;
  }
  return m_BinLinker.GetData( BL_IDX_JOIN_FESTA_CTR_JF_SHIRO_RR_CTR_GFBMDLP );
}

/**
 * @brief   通常時の地面のモデルリソース取得
 */
void* JoinFestaFacilitiesModelResourceManager::GetNormalGroundModel()
{
  if( m_isLoaded == false )
  {
    GFL_ASSERT(0);
    return NULL;
  }
  return m_BinLinker.GetData( BL_IDX_JOIN_FESTA_CTR_JF_GROUND_CTR_GFBMDLP );
}

/**
 * @brief   RR団時の地面のモデルリソース取得
 */
void* JoinFestaFacilitiesModelResourceManager::GetRRGroundModel()
{
  if( m_isLoaded == false )
  {
    GFL_ASSERT(0);
    return NULL;
  }
  return m_BinLinker.GetData( BL_IDX_JOIN_FESTA_CTR_JF_GROUND_RR_CTR_GFBMDLP );
}

/**
 * @brief   RR団イベント用の空のモデルリソース取得
 */
void* JoinFestaFacilitiesModelResourceManager::GetRRSkyModel()
{
  if( m_isLoaded == false )
  {
    GFL_ASSERT(0);
    return NULL;
  }
  return m_BinLinker.GetData( BL_IDX_JOIN_FESTA_CTR_JF_SKY_RR_CTR_GFBMDLP );
}

/**
 * @brief   アトラクション時の地面のモデルリソース取得
 */
void* JoinFestaFacilitiesModelResourceManager::GetAttractionGroundModel()
{
  if( m_isLoaded == false )
  {
    GFL_ASSERT(0);
    return NULL;
  }
  return m_BinLinker.GetData( BL_IDX_JOIN_FESTA_CTR_JF_CARPET_CTR_GFBMDLP );
}

/**
 * @brief   城のアニメID取得
 * @return  アニメID
 */
u32 JoinFestaFacilitiesModelResourceManager::GetCastleAnimeId()
{
  return JF_SHIRO_GFMOT;
}

/**
 * @brief   RR団の城のアニメID取得
 * @return  アニメID
 */
u32 JoinFestaFacilitiesModelResourceManager::GetRRCastleAnimeId()
{
  return JF_SHIRO_RR_GFMOT;
}

/**
 * @brief   城のテクスチャパターンアニメID取得
 * @return  アニメID
 */
u32 JoinFestaFacilitiesModelResourceManager::GetCastleTextureAnimeId()
{
  return JF_SHIRO_COLOR_GFMOT;
}

/**
 * @brief   RR団イベント用の空のアニメID取得
 */
u32 JoinFestaFacilitiesModelResourceManager::GetRRSkyAnimeId()
{
  return JF_SKY_RR_GFMOT;
}

/**
 * @brief   アニメーションデータ取得
 */
void* JoinFestaFacilitiesModelResourceManager::GetAnimationData()
{
  if( m_isLoaded == false )
  {
    GFL_ASSERT(0);
    return NULL;
  }
  return m_BinLinker.GetData( BL_IDX_JOIN_FESTA_CTR_JOIN_FESTA_GFBMOTP );
}


/**
 * @brief   施設のアニメID取得
 * @return  アニメID
 */
u32 JoinFestaFacilitiesModelResourceManager::GetAnimeId( JoinFestaScript::FacilitiesType type )
{
  // FacilitiesType→アニメID
  u32 idx=0;
  switch( type ){
  case JoinFestaScript::FACILITIES_TYPE_LOTTERY:       //! くじ引き屋
    idx = JF_KUJI_GFMOT;
    break;
  case JoinFestaScript::FACILITIES_TYPE_AIR_TOY:      //! エア遊具
    idx = JF_BARU_GFMOT;
    break;
  case JoinFestaScript::FACILITIES_TYPE_FUN_HOUSE:    //! ビックリハウス
    idx = JF_BIKURI_GFMOT;
    break;
  case JoinFestaScript::FACILITIES_TYPE_STALL:      //! 屋台
	  idx = JF_YATAI_GFMOT;
    break;
  case JoinFestaScript::FACILITIES_TYPE_GIFT_SHOP:     //! 土産屋
    idx = JF_OMIYA_GFMOT;
    break;
  case JoinFestaScript::FACILITIES_TYPE_FORTUNE:     //! 占い
    idx = JF_URA_GFMOT;
    break;
  case JoinFestaScript::FACILITIES_TYPE_DYEING:   //! 染物屋
    idx = JF_SOME_GFMOT;
    break;
  case JoinFestaScript::FACILITIES_TYPE_TRADE:      //! こうかんじょ(momiji追加)
    idx = 0; // アニメない
    break;
  }

  return idx;
}

/**
 * @brief   施設のテクスチャパターンアニメID取得
 * @return  アニメID
 */
u32 JoinFestaFacilitiesModelResourceManager::GetTextureAnimeId( JoinFestaScript::FacilitiesType type )
{
  // FacilitiesType→アニメID
  u32 idx=1000;
  switch( type ){
  case JoinFestaScript::FACILITIES_TYPE_LOTTERY:       //! くじ引き屋
    idx = JF_KUJI_COLOR_GFMOT;
    break;
  case JoinFestaScript::FACILITIES_TYPE_AIR_TOY:      //! エア遊具
    idx = JF_BARU_COLOR_GFMOT;
    break;
  case JoinFestaScript::FACILITIES_TYPE_FUN_HOUSE:    //! ビックリハウス
    idx = JF_BIKURI_COLOR_GFMOT;
    break;
  case JoinFestaScript::FACILITIES_TYPE_STALL:      //! 屋台
	  idx = JF_YATAI_COLOR_GFMOT;
    break;
  case JoinFestaScript::FACILITIES_TYPE_GIFT_SHOP:     //! 土産屋
    idx = JF_OMIYA_COLOR_GFMOT;
    break;
  case JoinFestaScript::FACILITIES_TYPE_FORTUNE:     //! 占い
    idx = JF_URA_COLOR_GFMOT;
    break;
  case JoinFestaScript::FACILITIES_TYPE_DYEING:   //! 染物屋
    idx = JF_SOME_COLOR_GFMOT;
    break;
  case JoinFestaScript::FACILITIES_TYPE_TRADE:      //! こうかんじょ(momiji追加)
    idx = JF_NEKO_COLOR_GFMOT;
    break;
  }

  return idx;
}

//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

