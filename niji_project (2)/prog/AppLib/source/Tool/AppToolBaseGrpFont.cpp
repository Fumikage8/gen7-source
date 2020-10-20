//======================================================================
/**
 * @file AppToolBaseGrpFont.cpp
 * @date 2015/08/28 12:37:33
 * @author uchida_yuto
 * @brief グラフィックフォントを扱う基本クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "AppLib/include/Tool/AppToolBaseGrpFont.h"
#include "AppLib/include/Tool/app_tool_LytTexReplaceSystem.h"

#include "AppLib/include/Util/AppUtilFileControlState.h"
#include "AppLib/include/Util/app_util_heap.h"

#include <arc_def_index/arc_def.h>

//  gfl2のインクルード
#include <Print/include/SystemFont.h>
#include <system/include/PokemonVersion.h>
#include <GameSys/include/GameManager.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  //  UVデータ一覧(現在は配列はcdatの構造体配列になっているが、もしかしたらbinaryファイルになるかも知れない)
  const AppToolBaseGrpFont::FONT_DATA AppToolBaseGrpFont::ms_aFontDataTable[ AppToolBaseGrpFont::FONT_TYPE_MAX ] =
  {
    //  common
    {
      ARCID_COMMON_GRP_ICON,
      CommonGrpIconUVDataTable::aDataTbl,
      GFL_NELEMS( CommonGrpIconUVDataTable::aDataTbl )
    },

    //  FONT_TYPE_BTL_VIDEO_PLAYER
    {
      ARCID_BTL_VIDEO_PLAYER_GRP_FONT,
      BtlVideoPlayerGrpFontUVDataTale::aDataTbl,
      GFL_NELEMS( BtlVideoPlayerGrpFontUVDataTale::aDataTbl )
    }
  };

  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  AppToolBaseGrpFont::AppToolBaseGrpFont()
  {
    _Clear();
  }

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  AppToolBaseGrpFont::~AppToolBaseGrpFont()
  {
  }

  //-----------------------------------------------------------------------------
  /**
   * @brief 初期化
   * @brief[in] pUtilHeap:  クラス内で使用するヒープ設定
   * @brief[in] pFontData:  フォントデータ(このクラスインスタンス生存中は、呼び出し元はfontDataを破棄せず保持しておいて下さい)
   * @brief[in] lang:       対象言語
   */
  //-----------------------------------------------------------------------------
  bool AppToolBaseGrpFont::Initialize( app::util::Heap* pUtilHeap, const FONT_DATA* pFontData, gfl2::fs::ToolDefine::ArcLangType::Tag lang )
  {
    if( m_pFileLoadState != NULL )
    {
      return  false;
    }

    m_pCurrentFontData  = pFontData;

    GFL_ASSERT( pUtilHeap );
    m_pUtilHeap = pUtilHeap;

    m_pFileLoadState  = GFL_NEW( m_pUtilHeap->GetDeviceHeap() ) app::util::AppUtilFileControlState( m_pCurrentFontData->arcId, 0, m_pUtilHeap->GetDeviceHeap(), true, 128, lang );

    //  レイアウトシステム作成
    {
      //  すげー適当
      gfl2::lyt::LytSysSetUp setup =
      {
        GFL_SINGLETON_INSTANCE(print::SystemFont)->GetRectDrawerShaderRes(),
        12,     // 管理リソース最大数
        1       // 管理レイアウトワーク最大数
      };

      m_pLytSys = GFL_NEW( m_pUtilHeap->GetDeviceHeap() )  gfl2::lyt::LytSys( &setup, m_pUtilHeap->GetSystemAllocator(), m_pUtilHeap->GetDeviceAllocator(), GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetDrawingLytWkManager() );
    }

    m_pFileLoadState->Start();

    return  true;
  }

  //-----------------------------------------------------------------------------
  /**
    * @brief 初期化
    * @brief[in] pUtilHeap:  クラス内で使用するヒープ設定
    * @brief[in] fontType:   フォントのタイプ
    * @brief[in] lang:       対象言語
    */
  //-----------------------------------------------------------------------------
  bool AppToolBaseGrpFont::Initialize( app::util::Heap* pUtilHeap, const FontType fontType, gfl2::fs::ToolDefine::ArcLangType::Tag lang )
  {
    return this->Initialize( pUtilHeap, &ms_aFontDataTable[ fontType ], lang );
  }

  //-----------------------------------------------------------------------------
  /**
    * @brief 終了
    */
  //-----------------------------------------------------------------------------
  void AppToolBaseGrpFont::Terminate()
  {
    if( m_pLytSys != NULL )
    {
      m_pLytSys->ReleaseLytRes( &m_loadTexMuiltResID );
    }

    m_pUtilHeap = NULL;

    GFL_SAFE_DELETE( m_pLytSys );
    GFL_SAFE_DELETE( m_pFileLoadState );
  }

  //-----------------------------------------------------------------------------
  /**
    * @func    IsReady
    * @brief   使用可能か
    * @date    2015.06.16
    *
    * @retval  false = 使用不可能
    * @retval  true  = 使用可能
    */
  //-----------------------------------------------------------------------------
  bool AppToolBaseGrpFont::IsReady()
  {
    if( m_pFileLoadState == NULL )
    {
      return  false;
    }

    m_pFileLoadState->Update();
    if( m_pFileLoadState->IsEnd() == true )
    {
      if( m_loadTexMuiltResID.IsEnable() == false )
      {
        //  テクスチャをロードしてレイアウトシステムにメモリ転送する
        m_loadTexMuiltResID = m_pLytSys->LoadLytRes( NULL, m_pUtilHeap->GetDeviceAllocator(), m_pFileLoadState->GetLoadDataBuffer() );
        m_pLytSys->TransferAllTexture( &m_loadTexMuiltResID );
      }

      return  true;
    }

    return  false;
  }

  //-----------------------------------------------------------------------------
  /**
    * @brief   ペインのテクスチャを変更
    *
    * @param   uvID      UVデータのID
    * @param   pane      ペイン
    * @param   material  マテリアルのインデックス
    * @param   texture   テクスチャのインデックス
    */
  //-----------------------------------------------------------------------------
  bool AppToolBaseGrpFont::ReplacePaneTexture( u32 uvID, gfl2::lyt::LytPicture * pane, u32 materialDataIdx, u32 textureDataIdx )
  {
    if( IsReady() == false )
    {
      return  false;
    }

    if( m_loadTexMuiltResID.IsEnable() == false )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    if( pane == NULL )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    if( m_pCurrentFontData->uiRectDataMax <= uvID )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    //  UVデータ取得
    gfl2::math::Vector2 uvPos   = gfl2::math::Vector2( (f32)m_pCurrentFontData->pUIRectData[ uvID ].x, (f32)m_pCurrentFontData->pUIRectData[ uvID ].y );
    gfl2::math::Vector2 uvRect  = gfl2::math::Vector2( (f32)m_pCurrentFontData->pUIRectData[ uvID ].width, (f32)m_pCurrentFontData->pUIRectData[ uvID ].height );

    gfl2::lyt::LytRes*  pRes  = m_pLytSys->GetResource( &m_loadTexMuiltResID );
    if( pRes == NULL )
    {
      GFL_ASSERT( 0 );
      return  false;
    }

    //  貼り付け
    LytTexReplaceSystem::ReplaceTextureStatic( pRes->GetTexture( 0, m_loadTexMuiltResID.GetMultiID() ), pane, uvPos, uvRect, materialDataIdx, textureDataIdx );

    //  ペインのサイズを貼り付けるテクスチャーサイズに調整
    nw::lyt::Size size( uvRect.GetX(), uvRect.GetY() );
    pane->SetSize( size );

    return  true;
  }

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)
