//=============================================================================
/**
 * @file BattleUIKoukaIconPartsLayout.h
 * @brief パーツレイアウト効果アイコンの制御
 * @author yuto_uchida
 * @date 2015.04.10
 */
//=============================================================================
#include "Battle/source/btlv/app/parts/BattleUIKoukaIconPartsLayout.h"

#include "AppLib/include/Util/app_util_2d.h"

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( parts )

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIKoukaIconPartsLayout::BattleUIKoukaIconPartsLayout( const INIT_PARAM& param )
  {
    GFL_ASSERT( param.p_g2d_util );
    GFL_ASSERT( param.p_parts_pane );

    m_initParam = param;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIKoukaIconPartsLayout::~BattleUIKoukaIconPartsLayout()
  {
    GFL_PRINT( "die : BattleUIKoukaIconPartsLayout\n" );
  }

  //-----------------------------------------------------------------------------------------
  /**
    * @brief  表示設定
    */
  //-----------------------------------------------------------------------------------------
  void BattleUIKoukaIconPartsLayout::SetVisible( const bool bFlg )
  {
    m_initParam.p_g2d_util->SetPaneVisible( m_initParam.p_parts_pane, bFlg );
  }

  //-----------------------------------------------------------------------------------------
  /**
    * @brief  効果設定
    */
  //-----------------------------------------------------------------------------------------
  void BattleUIKoukaIconPartsLayout::SetType( const BtlTypeAffAbout type )
  {
  /*
    簡易タイプ相性

    TYPEAFF_ABOUT_NONE,         //  こうかなし
    TYPEAFF_ABOUT_NORMAL,       //  ふつう
    TYPEAFF_ABOUT_ADVANTAGE,    //  ばつぐん
    TYPEAFF_ABOUT_DISADVANTAGE, //  もうひとつ
  */
    const s32 aAnimeIdxMapTable[ 5 ][ 2 ] =
    {
      { pml::battle::TypeAffinity::TYPEAFF_ABOUT_NONE,         m_initParam.kouka_none_anime_idx        },
      { pml::battle::TypeAffinity::TYPEAFF_ABOUT_ADVANTAGE,    m_initParam.kouka_batsugun_anime_idx    },
      { pml::battle::TypeAffinity::TYPEAFF_ABOUT_DISADVANTAGE, m_initParam.kouka_imahitotsu_anime_idx  },
      { pml::battle::TypeAffinity::TYPEAFF_ABOUT_NORMAL,       m_initParam.kouka_normal_anime_idx      },
      { -1, -1 },
    };

    for( s32 i = 0; aAnimeIdxMapTable[ i ][ 0 ] != -1; ++i )
    {
      if( aAnimeIdxMapTable[ i ][ 0 ] == type )
      {
        m_initParam.p_g2d_util->StartAnime( m_initParam.layout_id, aAnimeIdxMapTable[ i ][ 1 ] );
        break;
      }
    }
  }

GFL_NAMESPACE_END( parts )
GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )
