#ifndef __GFL_G3DERRORCHECK_H__
#define __GFL_G3DERRORCHECK_H__


//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dErrorCheck.h
 *	@brief  3Dエラーチェック
 *	@author	Koji Kawada
 *	@date		2011.08.04
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
//NintendoWare\CTR\tools\Viewer\sources\g3d\include\ErrorCheck.h

/*---------------------------------------------------------------------------*
  Project:  NW4C - Viewer - build - viewer - g3d
  File:     ErrorCheck.h

  Copyright (C)2005-2009 Nintendo  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.6 $
 *---------------------------------------------------------------------------*/

//を元に作成。
#endif


// CTR_SDK
#include <nn/gx.h>

// NW4C
#include <nw/gfx.h>


namespace gfl {
namespace grp {
namespace g3d {
namespace ec {


class ErrorCheck
{
  GFL_FORBID_COPY_AND_ASSIGN(ErrorCheck);

  friend class gfl::grp::g3d::Resource;

private:
#if 0
    static void CheckModelTextureCombiner( nw::gfx::Model* model );
#endif
#if 0
    static void CheckModelTextureReference( nw::gfx::Model* model );
#endif
#if 0
    static void CheckModelLookupTableReference( nw::gfx::Model* model );
    static void CheckModelFogIndexBoundary( nw::gfx::Model* model );
    static void CheckModelFogIndexReference( nw::gfx::Model* model );
    static void CheckModelLightSetIndexBoundary( nw::gfx::Model* model );
    static void CheckModelLightSetIndexReference( nw::gfx::Model* model );
    static void CheckModelCameraReferenceIndexBoundary( nw::gfx::Model* model );
    static void CheckModelCameraReferenceIndexReference( nw::gfx::Model* model );
    static void CheckCameraNear( nw::gfx::Camera* camera );
    static void CheckCameraNearFar( nw::gfx::Camera* camera );
    static void CheckCameraPosition( nw::gfx::Camera* camera );
    static void CheckCameraUpVectorZero( nw::gfx::Camera* camera );
    static void CheckCameraUpVectorParallel( nw::gfx::Camera* camera );
    static void CheckLightLookupTableReference( nw::gfx::Light* light );
    static void CheckSceneEnvironmentSetting( nw::gfx::SceneEnvironmentSetting* scene );
    static void Error_VertexLight_Disalbe();
    static void Error_VRAM_Alloc();
#endif

private:
    // friendクラスからアクセスするメンバ関数
    static void CheckModelTextureReference( const nw::gfx::res::ResModel res_model );
    static void CheckModelLookupTableReference( const nw::gfx::res::ResModel res_model );
    static void CheckModelShaderReference( const nw::gfx::res::ResModel res_model );

private:
    // friendクラスからアクセスしないメンバ関数
    static void
    CheckLookupTableReference( 
        bool isValid,
        nw::gfx::res::ResReferenceLookupTable refLut,
        const char* model_name,
        int material_index,
        const char* matName,
        const char* message
    );
};


}   // namespace ec
}   // namespace g3d
}   // namespace grp
}   // namespace gfl


#endif  // #ifndef __GFL_G3DERRORCHECK_H__
