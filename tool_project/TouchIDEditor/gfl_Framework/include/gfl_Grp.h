#ifndef __GFL_GRP_H__
#define __GFL_GRP_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_grp.h
 *	@brief  描画環境 ライブラリ
 *	@date		2010.10.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once



#ifdef  __cplusplus
    #include <grp/gfl_GraphicsSystem.h>
    #include <grp/gfl_Camera.h>
    #include <grp/gfl_RenderTarget.h>
    #include <grp/gfl_Viewport.h>
    #include <grp/gfl_GraphicsType.h>
    #include <grp/gfl_Fade.h>
    #include <grp/g2d/gfl_G2dEnv.h>
    #include <grp/g2d/gfl_LytSys.h>
    #include <grp/g3d/gfl_G3dSceneEnvironmentSetting.h>
    #include <grp/g3d/gfl_G3dSceneNode.h>
    #include <grp/g3d/gfl_G3dModel.h>
    #include <grp/g3d/gfl_G3dCamera.h>
    #include <grp/g3d/gfl_G3dStereoCamera.h>
    #include <grp/g3d/gfl_G3dLight.h>
    #include <grp/g3d/gfl_G3dLightSet.h>
    #include <grp/g3d/gfl_G3dFog.h>
    #include <grp/g3d/gfl_G3dTexture.h>
    #include <grp/g3d/gfl_G3dResAnim.h>
    #include <grp/g3d/gfl_G3dAnimation.h>
    #include <grp/g3d/gfl_G3dPackAnim.h>
    #include <grp/g3d/gfl_G3dPackAnimConnector.h>
    #include <grp/g3d/gfl_G3dMultiPackAnimConnector.h>
    #include <grp/g3d/gfl_G3dParticle.h>
    #include <grp/g3d/gfl_G3dResource.h>
    #include <grp/g3d/gfl_G3dScene.h>
    #include <grp/g3d/gfl_G3dSystem.h>
    #include <grp/g3d/gfl_G3dSceneCalculateGroup.h>
    #include <grp/g3d/gfl_G3dSceneDrawGroup.h>
    #include <grp/g3d/gfl_G3dShadowSystem.h>
    #include <grp/g3d/gfl_G3dBloomFilter.h>
    #include <grp/g3d/gfl_G3dDefine.h>
    #include <grp/map/gfl_MapSystem.h>
    #include <grp/util/gfl_UtilDrawUtil.h>
    #include <grp/g3d/gfl_G3dH3dUtil.h>
    #include <grp/g3d/gfl_G3dH3dResAnim.h>
    #include <grp/g3d/gfl_G3dH3dResSkeletalAnim.h>
    #include <grp/g3d/gfl_G3dH3dResMaterialAnim.h>
    #include <grp/g3d/gfl_G3dH3dResVisibilityAnim.h>
    #include <grp/g3d/gfl_G3dH3dAnimationSlot.h>
    #include <grp/g3d/gfl_G3dH3dResTexture.h>
    #include <grp/g3d/gfl_G3dH3dDynamicTexture.h>
    #include <grp/g3d/gfl_G3dH3dResShader.h>
    #include <grp/g3d/gfl_G3dH3dCommand.h>
    #include <grp/g3d/gfl_G3dH3dEnvironment.h>
    #include <grp/g3d/gfl_G3dH3dModel.h>
    #include <grp/g3d/gfl_G3dH3dResource.h>
    #include <grp/g3d/gfl_G3dH3dUserData.h>

    // デバッグ
    #include <debug/gfl_DebugG3dCamera.h>
    #include <debug/gfl_DebugG3dCameraChase.h>
    #include <debug/gfl_DebugG3dStereoCamera.h>
    #include <debug/gfl_DebugG3dStereoCameraChase.h>
#endif // __cplusplus


// 以下のファイルは、インクルードしてはならない
#if 0
    #include <grp/g3d/gfl_G3dSceneSystem.h>
    #include <grp/g3d/gfl_G3dRenderSystem.h>
    #include <grp/g3d/gfl_G3dShadowGroup.h>
    #include <grp/g3d/gfl_G3dObject.h>
    #include <grp/g3d/gfl_G3dObjectManager.h>
#endif

#endif // __GFL_GRP_H__
