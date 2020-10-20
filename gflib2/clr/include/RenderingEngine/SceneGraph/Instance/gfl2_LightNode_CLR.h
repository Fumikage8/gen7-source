#if !defined(GFLIB2_CLR_RENDERINGENGINE_SCENEGRAPH_INSTANCE_LIGHTNODE_CLR_H_INCLUDED)
#define GFLIB2_CLR_RENDERINGENGINE_SCENEGRAPH_INSTANCE_LIGHTNODE_CLR_H_INCLUDED
#pragma once

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LightNode.h>
#include <clr/include/math/gfl2_Vector_CLR.h>
#include <clr/include/math/gfl2_Matrix_CLR.h>
#include <clr/include/RenderingEngine/gfl2_RenderingEngineEnums_CLR.h>
#include <clr/include/gfx/gfl2_enums_CLR.h>

namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace instance {

public ref class LightNode
{
public:
	LightNode(gfl2::renderingengine::scenegraph::instance::LightNode* pNative);
	~LightNode()
	{
		this->!LightNode();
	}
	!LightNode();

	gfl2::renderingengine::scenegraph::instance::LightNode* GetNative()
	{
		return m_pNative;
	}

	//! @name 各種パラメータ設定
	//@{
    void SetNodeName(System::String^ name);
		void SetNodeNameHash( u32 hash );
    void SetLightType( gfl2::clr::renderingengine::LightType::Enum type );	//!< タイプの設定
		void SetColor( clr::math::Vector^ color );  			                //!< カラーの設定
		void SetIntensity( f32 intensity );					      		            //!< 強度の設定
		void SetPosition(  clr::math::Vector^ pos );	  	                //!< ポジションの設定
		void SetDirection(  clr::math::Vector^ dir );		                  //!< 方向の設定
		void SetDecayRate( f32 decayRate );							                  //!< 減衰率の設定
		void SetConeAngle( f32 coneAngle );							                  //!< 円錐の角度設定
		void SetPriority( s32 priority );								                  //!< プライオリティの設定
	//@}

	//! @name 各種パラメータ取得
	//@{
		u32 GetNodeNameHash();
    gfl2::clr::renderingengine::LightType::Enum GetLightType();	//!< タイプの取得
		clr::math::Vector^ GetColor();    					          //!< カラーの取得
		f32 GetIntensity();								    			          //!< 強度の取得
		clr::math::Vector^ GetPosition();	    			          //!< ポジションの取得
		clr::math::Vector^ GetDirection();    			          //!< 方向の取得
		f32 GetDecayRate();										    	          //!< 減衰率の取得
		f32 GetConeAngle();											              //!< 円錐の角度取得
		s32 GetPriority();											              //!< プライオリティの取得
		
		clr::math::Matrix^ GetMatrix();					//! Position, Direction等からマトリクスを求める
	//@}

    void Release();

private:
	gfl2::renderingengine::scenegraph::instance::LightNode* m_pNative;
};

}}}}}

#endif