//==============================================================================
/**
 @file    AppOutLinePostRenderPath.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.01.29
 @brief   AppOutLinePostRenderPath
 */
//==============================================================================

// poke_3d_libのインクルード
#include <renderer/include/gfl2_OutLinePostRenderPath.h>

// nijiのインクルード
#include <debug/include/gfl2_DebugPrint.h>
#include <GameSys/include/GameManager.h>

#include <System/include/RenderPath/OutLinePostRenderPath.h>

#include "AppOutLinePostRenderPath.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


AppOutLinePostRenderPath::AppOutLinePostRenderPath(gfl2::gfx::IGLAllocator *allocator)
  : System::NijiOutLinePostSceneRenderPath(allocator)
{
#if NIJI_APPLIB_UTIL_APP_OUT_LINE_POST_RENDER_PATH_DEBUG_TEX_SRT
  for(u32 texNo=0; texNo<TEX_NUM; ++texNo)
  {
    m_debugTexTranslateOriginalValGetFlag[texNo] = false;
    m_debugTexTranslateOriginalVal[texNo]        = gfl2::math::Vector2(0,0);
  }
  m_debugTexScaleVal     = gfl2::math::Vector2(1,1);
  m_debugTexTranslateVal = gfl2::math::Vector2(0,0);
#endif  // NIJI_APPLIB_UTIL_APP_OUT_LINE_POST_RENDER_PATH_DEBUG_TEX_SRT
}


AppOutLinePostRenderPath::~AppOutLinePostRenderPath()
{}


void AppOutLinePostRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
  // モード  // モードを用意しておき、こちらが想定したレンダーターゲットしか受け付けないことにする。
  enum Mode
  {
    MODE_CTR_UPPER_DISPLAY,
    MODE_CTR_LOWER_DISPLAY,
    MODE_OFF_SCREEN,
    MODE_NUM,
    MODE_DEFAULT            = MODE_CTR_UPPER_DISPLAY
  };
  gfl2::math::Vector2 TEX_SCALE[MODE_NUM] =
  {
    gfl2::math::Vector2(240.0f/256.0f, 400.0f/512.0f),
    gfl2::math::Vector2(240.0f/256.0f, 320.0f/512.0f),
    gfl2::math::Vector2(         1.0f,          1.0f),  // 直後に書き換え
  };
  gfl2::math::Vector2 TEX_TRANSLATE[MODE_NUM][TEX_NUM] =
  {
    { gfl2::math::Vector2(0,0), gfl2::math::Vector2(0,0), gfl2::math::Vector2(0,0), },  // 直後に書き換え
    { gfl2::math::Vector2(0,0), gfl2::math::Vector2(0,0), gfl2::math::Vector2(0,0), },  // 直後に書き換え
    { gfl2::math::Vector2(0,0), gfl2::math::Vector2(0,0), gfl2::math::Vector2(0,0), },  // 直後に書き換え
  };
  Mode mode = MODE_DEFAULT;



	//DisplayDeviceHandle				dispHandle = gfl2::gfx::GFGL::GetActiveDisplay();

	//m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID] = gfl2::gfx::GFGL::GetDepthStencilTexture( dispHandle );

	gfl2::gfx::GFGL::SetRenderTarget( rDrawContext.m_pRenderTarget );
	gfl2::gfx::GFGL::SetDepthStencil( NULL );

	gfl2::gfx::GFGL::BeginScene();

	const gfl2::gfx::Surface::SurfaceDesc			&rDesc = gfl2::gfx::GFGL::GetRenderTarget()->GetSurfaceDesc();



  // モード判定
  u32 normalWidth  = 0;
  u32 normalHeight = 0;
  u32 idWidth      = 0;
  u32 idHeight     = 0;
  {
#ifdef GF_PLATFORM_CTR
    // テクスチャの実際のサイズが想定したものになっているかチェックする
    if( this->IsEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal ) && m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal] )
    {
      normalWidth = (m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal]->GetSurface() != NULL)
        ? m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal]->GetSurface()->GetSurfaceDesc().bufferWidth
        : m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal]->GetTextureDesc().width;
      
      normalHeight = (m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal]->GetSurface() != NULL)
        ? m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal]->GetSurface()->GetSurfaceDesc().bufferHeight
        : m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal]->GetTextureDesc().height;
      
      if(normalWidth != 256 || normalHeight != 512)
      {
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。対応していないモード。抜けたらそのまま処理を進める。
      }
    }
 
    // テクスチャの実際のサイズが想定したものになっているかチェックする
    if( this->IsEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID ) && m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID] )
    {
      idWidth = (m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID]->GetSurface() != NULL)
        ? m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID]->GetSurface()->GetSurfaceDesc().bufferWidth
        : m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID]->GetTextureDesc().width;
     
        idHeight = (m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID]->GetSurface() != NULL)
        ? m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID]->GetSurface()->GetSurfaceDesc().bufferHeight
        : m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID]->GetTextureDesc().height;

      if(idWidth != 256 || idHeight != 512)
      {
        GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。対応していないモード。抜けたらそのまま処理を進める。
      }
    }

    // モード決定
    if(rDesc.width == rDesc.height)
    {
      mode = MODE_OFF_SCREEN;
    }
    else if(rDesc.width == 320 && rDesc.height == 240)
    {
      mode = MODE_CTR_LOWER_DISPLAY;
    }
    else if(rDesc.width == 400 && rDesc.height == 240)
    {
      mode = MODE_CTR_UPPER_DISPLAY;
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。対応していないモード。抜けたらMODE_DEFAULTで処理を進める。
    }
#endif  // GF_PLATFORM_CTR

#ifdef GF_PLATFORM_CTR
    // 想定したレンダーターゲットだったので、不確定だった値を確定させる
    TEX_SCALE[MODE_OFF_SCREEN] = gfl2::math::Vector2(rDesc.height/256.0f, rDesc.width/512.0f);
    
    TEX_TRANSLATE[MODE_CTR_UPPER_DISPLAY][0] = gfl2::math::Vector2(                 0,                0);
    TEX_TRANSLATE[MODE_CTR_UPPER_DISPLAY][1] = gfl2::math::Vector2(       1.0f/256.0f,                0);
    TEX_TRANSLATE[MODE_CTR_UPPER_DISPLAY][2] = gfl2::math::Vector2(                 0,      1.0f/512.0f);

    TEX_TRANSLATE[MODE_CTR_LOWER_DISPLAY][0] = gfl2::math::Vector2(                 0,                0);
    TEX_TRANSLATE[MODE_CTR_LOWER_DISPLAY][1] = gfl2::math::Vector2(       1.0f/256.0f,                0);
    TEX_TRANSLATE[MODE_CTR_LOWER_DISPLAY][2] = gfl2::math::Vector2(                 0,      1.0f/512.0f);
    
    TEX_TRANSLATE[MODE_OFF_SCREEN][0]        = gfl2::math::Vector2(                 0,                0);
    TEX_TRANSLATE[MODE_OFF_SCREEN][1]        = gfl2::math::Vector2( 1.0f/rDesc.height,                0);
    TEX_TRANSLATE[MODE_OFF_SCREEN][2]        = gfl2::math::Vector2(                 0, 1.0f/rDesc.width);
#endif  // GF_PLATFORM_CTR
  }

#if NIJI_APPLIB_UTIL_APP_OUT_LINE_POST_RENDER_PATH_DEBUG_TEX_SRT
  this->debugControlTexSRT();
#endif  // NIJI_APPLIB_UTIL_APP_OUT_LINE_POST_RENDER_PATH_DEBUG_TEX_SRT



	gfl2::gfx::GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);

	{
		gfl2::renderingengine::renderer::RenderState			renderState;
    gfl2::math::Matrix34		dummyMat34 = gfl2::math::Matrix34::GetIdentity();
		gfl2::math::Matrix44		dummyMat44 = gfl2::math::Matrix44::GetIdentity();
			
		renderState.SetProjectionMatrix( &dummyMat44 );
		renderState.SetViewMatrix( &dummyMat34 );
		renderState.SetWorldMatrix( &dummyMat34 );

    if( this->IsEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal ) )
		{
			gfl2::renderingengine::renderer::MeshDrawTag			*pTag = m_pModelInstanceNode[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal]->GetMeshDrawTag( 0 );

			for( u32 texNo = 0; texNo < 3; ++texNo )
      {
#ifdef GF_PLATFORM_CTR
        // CTRの下画面でエッジがずれることへの対応
        gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo* texInfo
            = m_pModelInstanceNode[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal]->GetMaterialInstanceNode(0)->GetTextureInfo(texNo);
        if(texInfo && m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal])
        {
          //u32 height = (m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal]->GetSurface() != NULL)
          //  ? m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal]->GetSurface()->GetSurfaceDesc().bufferHeight
          //  : m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal]->GetTextureDesc().height;
          //
          //texInfo->m_Attribute.m_ScaleV = (static_cast<f32>(rDesc.width)) / height;  // 例: 400/512

          texInfo->m_Attribute.m_ScaleV     = TEX_SCALE[mode].y;
          texInfo->m_Attribute.m_ScaleU     = TEX_SCALE[mode].x;
          texInfo->m_Attribute.m_TranslateV = TEX_TRANSLATE[mode][texNo].y;
          texInfo->m_Attribute.m_TranslateU = TEX_TRANSLATE[mode][texNo].x;

#if NIJI_APPLIB_UTIL_APP_OUT_LINE_POST_RENDER_PATH_DEBUG_TEX_SRT
          /*
          // m_debugTexScaleVal、m_debugTexTranslateValをIDエッジのデバッグ調整に使いたいのでこちらはコメントアウト。
          if(!m_debugTexTranslateOriginalValGetFlag[texNo])
          {
            // エッジを出すには3枚の画像をずらしてやらないといけない。SRT調整を行う際のずらし量には初期設定値を使うことにする。実際のずらし量は初期設定値とは異なりTEX_TRANSLATEを用いる。
            m_debugTexTranslateOriginalValGetFlag[texNo] = true;
            m_debugTexTranslateOriginalVal[texNo]        = gfl2::math::Vector2(texInfo->m_Attribute.m_TranslateU, texInfo->m_Attribute.m_TranslateV);
          }
          texInfo->m_Attribute.m_ScaleV     = m_debugTexScaleVal.y;
          texInfo->m_Attribute.m_ScaleU     = m_debugTexScaleVal.x;
          texInfo->m_Attribute.m_TranslateV = m_debugTexTranslateVal.y + m_debugTexTranslateOriginalVal[texNo].y;
          texInfo->m_Attribute.m_TranslateU = m_debugTexTranslateVal.x + m_debugTexTranslateOriginalVal[texNo].x;
          */
#endif  // NIJI_APPLIB_UTIL_APP_OUT_LINE_POST_RENDER_PATH_DEBUG_TEX_SRT

        }
#endif  // GF_PLATFORM_CTR
				pTag->SetTexture( texNo, m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal] );
      }

			pTag->Draw( renderState );
		}

    if( this->IsEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID ) && m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID] )
		{//IDエッジ
      static const u32                materialMax = 2;
      static const gfl2::util::FnvHash			materialName[materialMax] = {
        gfl2::util::FnvHash( reinterpret_cast<u8*>( const_cast<char*>("Comb2") ) ),
        gfl2::util::FnvHash( reinterpret_cast<u8*>( const_cast<char*>("Comb3") ) )
      };

      //メッシュとマテリアルは１対１
      GFL_ASSERT( m_pModelInstanceNode[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID]->GetMaterialNum() == m_pModelInstanceNode[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID]->GetMehsNum() );
      GFL_ASSERT( m_pModelInstanceNode[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID]->GetMaterialNum() == materialMax );

      gfl2::renderingengine::renderer::MeshDrawTag                     *pOT[materialMax] = { NULL, NULL };

      
      for( u32 pushCnt = 0; pushCnt < materialMax; ++pushCnt )
      {
        u32         checkValue = materialName[pushCnt].value;//Comb2 -> Comb3 の順に整えます。

        for( u32 materialNo = 0; materialNo < materialMax; ++materialNo )
			  {
				  gfl2::renderingengine::renderer::MeshDrawTag			                                                            *pTag = m_pModelInstanceNode[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID]->GetMeshDrawTag( materialNo );
          const gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode     *pMaterial = pTag->GetMaterialInstanceNode();

          if ( pMaterial->GetNameHash() == checkValue )
          {
            pOT[pushCnt] = pTag;
            break;
          }
        }
      }

      for( u32 materialNo = 0; materialNo < materialMax; ++materialNo )
			{
				gfl2::renderingengine::renderer::MeshDrawTag			                                                            *pTag = pOT[materialNo];
        const gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode     *pMaterial = pTag->GetMaterialInstanceNode();

				for( u32 texNo = 0; texNo < 3; ++texNo )
        {
#ifdef GF_PLATFORM_CTR
          // CTRの下画面でエッジがずれることへの対応
          gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo* texInfo = const_cast<gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo*>( pMaterial->GetTextureInfo(texNo) );

          if(texInfo)
          {
            //u32 width = (m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID]->GetSurface() != NULL)
            //  ? m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID]->GetSurface()->GetSurfaceDesc().bufferWidth
						//	: m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID]->GetTextureDesc().width;
            //
						//u32 height = (m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID]->GetSurface() != NULL)
            //  ? m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID]->GetSurface()->GetSurfaceDesc().bufferHeight
            //  : m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID]->GetTextureDesc().height;

						f32			fwidth = static_cast<f32>(rDesc.width);
						f32			fheight = static_cast<f32>(rDesc.height);

            //texInfo->m_Attribute.m_ScaleV = fwidth / height;  // 例: 400/512
						//texInfo->m_Attribute.m_ScaleU = fheight / width;  // 例: 240/256

            texInfo->m_Attribute.m_ScaleV = TEX_SCALE[mode].y;
            texInfo->m_Attribute.m_ScaleU = TEX_SCALE[mode].x;

						texInfo->m_Attribute.m_TranslateU = 0.0f;
						texInfo->m_Attribute.m_TranslateV = 0.0f;

            if ( materialNo == 0 )
            {//Comb2はＵ方向差分をレンダリング
						  switch( texNo ){
						  case 1:
							  texInfo->m_Attribute.m_TranslateU = 1.0f / fheight;
							  break;
						  case 2:
							  texInfo->m_Attribute.m_TranslateU = -(1.0f / fheight);
							  break;
						  }
            }
            else
            {//Comb3はＶ方向差分をレンダリング
              switch( texNo ){
						  case 1:
							  texInfo->m_Attribute.m_TranslateV = 1.0f / fwidth ;
							  break;
						  case 2:
							  texInfo->m_Attribute.m_TranslateV = -(1.0f / fwidth );
							  break;
						  }
            }

#if NIJI_APPLIB_UTIL_APP_OUT_LINE_POST_RENDER_PATH_DEBUG_TEX_SRT
            // m_debugTexScaleVal、m_debugTexTranslateValをIDエッジのデバッグ調整に使う。
            if(!m_debugTexTranslateOriginalValGetFlag[texNo])
            {
              // エッジを出すには3枚の画像をずらしてやらないといけない。SRT調整を行う際のずらし量には初期設定値を使うことにする。実際のずらし量は初期設定値とは異なりTEX_TRANSLATEを用いる。
              m_debugTexTranslateOriginalValGetFlag[texNo] = true;
              m_debugTexTranslateOriginalVal[texNo]        = gfl2::math::Vector2(texInfo->m_Attribute.m_TranslateU, texInfo->m_Attribute.m_TranslateV);
            }
            texInfo->m_Attribute.m_ScaleV     = m_debugTexScaleVal.y;
            texInfo->m_Attribute.m_ScaleU     = m_debugTexScaleVal.x;
            texInfo->m_Attribute.m_TranslateV = m_debugTexTranslateVal.y + m_debugTexTranslateOriginalVal[texNo].y;
            texInfo->m_Attribute.m_TranslateU = m_debugTexTranslateVal.x + m_debugTexTranslateOriginalVal[texNo].x;
#endif  // NIJI_APPLIB_UTIL_APP_OUT_LINE_POST_RENDER_PATH_DEBUG_TEX_SRT

          }
#endif  // GF_PLATFORM_CTR
					pTag->SetTexture( texNo, m_pTexture[poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID] );
        }

				pTag->Draw( renderState );
			}
		}
	}

	gfl2::gfx::GFGL::SetTexture( 0, NULL );//次のフレームでは最初にRTとして使用するので、バインドをはずしておく。
	gfl2::gfx::GFGL::SetTexture( 1, NULL );//次のフレームでは最初にRTとして使用するので、バインドをはずしておく。
	gfl2::gfx::GFGL::SetTexture( 2, NULL );//次のフレームでは最初にRTとして使用するので、バインドをはずしておく。

	gfl2::gfx::GFGL::EndScene();
	gfl2::gfx::GFGL::SetDepthStencil( rDrawContext.m_pDepthStencil );
}


#if NIJI_APPLIB_UTIL_APP_OUT_LINE_POST_RENDER_PATH_DEBUG_TEX_SRT
void AppOutLinePostRenderPath::debugControlTexSRT(void)
{
  const gfl2::ui::Button* button = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  bool changeFlag = false;
  f32* targetVal = NULL;

  if( button->IsHold(gfl2::ui::BUTTON_L) )
  {
    // scalse
    if( button->IsHold(gfl2::ui::BUTTON_X) )
    {
      // u
      targetVal = &(m_debugTexScaleVal.x);
    }
    else if( button->IsHold(gfl2::ui::BUTTON_Y) )
    {
      // v
      targetVal = &(m_debugTexScaleVal.y);
    }
  }
  else if( button->IsHold(gfl2::ui::BUTTON_R) )
  {
    // translate
    if( button->IsHold(gfl2::ui::BUTTON_X) )
    {
      // u
      targetVal = &(m_debugTexTranslateVal.x);
    }
    else if( button->IsHold(gfl2::ui::BUTTON_Y) )
    {
      // v
      targetVal = &(m_debugTexTranslateVal.y);
    }
  }

  if(targetVal)
  {
    if( button->IsRepeat(gfl2::ui::BUTTON_DOWN) )
    {
      *targetVal = *targetVal - 0.0001f;
      changeFlag = true;
    }
    else if( button->IsRepeat(gfl2::ui::BUTTON_UP) )
    {
      *targetVal = *targetVal + 0.0001f;
      changeFlag = true;
    }
    else if( button->IsRepeat(gfl2::ui::BUTTON_LEFT) )
    {
      *targetVal = *targetVal - 0.01f;
      changeFlag = true;
    }
    else if( button->IsRepeat(gfl2::ui::BUTTON_RIGHT) )
    {
      *targetVal = *targetVal + 0.01f;
      changeFlag = true;
    }
    else if( button->IsTrigger(gfl2::ui::BUTTON_A) )
    {
      // リセット
      *targetVal = 0.0f;
      changeFlag = true;
    }
  }

  if(changeFlag)
  {
    GFL_PRINT("Scale(%f, %f), Translate(%f, %f)\n",
        m_debugTexScaleVal.x, m_debugTexScaleVal.y, m_debugTexTranslateVal.x, m_debugTexTranslateVal.y);
  }
}
#endif  // NIJI_APPLIB_UTIL_APP_OUT_LINE_POST_RENDER_PATH_DEBUG_TEX_SRT


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)
