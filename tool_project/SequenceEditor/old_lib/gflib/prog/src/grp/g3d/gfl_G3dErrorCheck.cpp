//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dErrorCheck.cpp
 *	@brief  3D�G���[�`�F�b�N
 *	@author	Koji Kawada
 *	@date		2011.08.04
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
//NintendoWare\CTR\tools\Viewer\sources\g3d\src\ErrorCheck.cpp

/*---------------------------------------------------------------------------*
  Project:  NW4C - Viewer - build - viewer - g3d
  File:     ErrorCheck.cpp

  Copyright (C)2005-2009 Nintendo  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.6 $
 *---------------------------------------------------------------------------*/

//�����ɍ쐬�B
#endif


namespace gfl {
namespace grp {
namespace g3d {
namespace ec {

enum CameraSubKey {
    ErrorCameraNear = 0,
    ErrorCameraNearFar,
    ErrorCameraPosition,
    ErrorCameraUpVectorZero,
    ErrorCameraUpVectorParallel
};

enum ModelLutSubKey {
    ErrorModelRefRSampler = 0,
    ErrorModelRefGSampler,
    ErrorModelRefBSampler,
    ErrorModelDis0Sampler,
    ErrorModelDis1Sampler,
    ErrorModelFresnelSampler
};

enum ModelMaterialSubKey {
    ErrorModelFogUpperBoundary = 0,
    ErrorModelFogLowerBoundary,
    ErrorModelFogReference,
    ErrorModelLightSetUpperBoundary,
    ErrorModelLightSetLowerBoundary,
    ErrorModelLightSetReference
};

enum ModelTextureCoordinatorSubKey {
    ErrorModelCameraReferenceIndexUpperBoundary = 0,
    ErrorModelCameraReferenceIndexLowerBoundary,
    ErrorModelCameraReferenceIndexReference
};

enum SceneEnvironmentSettingSubKey {
    ErrorSceneSettingFogUpperBoundary = 0,
    ErrorSceneSettingFogLowerBoundary,
    ErrorSceneSettingCameraUpperBoundary,
    ErrorSceneSettingCameraLowerBoundary,
    ErrorSceneSettingLightSetUpperBoundary,
    ErrorSceneSettingLightSetLowerBoundary
};

enum LightLutSubKey {
    ErrorLightDistAttenuationSampler = 0,
    ErrorLightAngleSampler
};

#if 0
static ModelElem*
GetModelElem( nw::gfx::Model* model )
{
    SceneManager::ModelList::iterator i = SceneManager::GetInstance()->FindModel( model );
    if( i != SceneManager::GetInstance()->GetModelsEnd() )
    {
        return &(*i);
    }
    return 0;
}

static LightElem*
GetLightElem( nw::gfx::Light* light )
{
    SceneManager::LightList::iterator i = SceneManager::GetInstance()->FindLight( light );
    if( i != SceneManager::GetInstance()->GetLightsEnd() )
    {
        return &(*i);
    }
    return 0;
}

static SceneEnvironmentSettingElem*
GetSceneEnvironmentSettingElem( nw::gfx::SceneEnvironmentSetting* scene )
{
    SceneManager::SceneEnvironmentSettingList::iterator i = SceneManager::GetInstance()->FindSceneEnvironmentSetting( scene );
    if( i != SceneManager::GetInstance()->GetSceneEnvironmentSettingsEnd() )
    {
        return &(*i);
    }
    return 0;
}

static u32
GetHashID( nw::gfx::Camera* camera )
{
    SceneManager::CameraList::iterator i = SceneManager::GetInstance()->FindCamera( camera );
    if( i != SceneManager::GetInstance()->GetCamerasEnd() )
    {
        ResourceSet* res = i->GetResSource();
        return res->GetHashID();
    }
    return 0;
}

static u32
GetHashID( nw::anim::res::ResAnim anim )
{
    SceneManager::AnimList::iterator i = SceneManager::GetInstance()->FindAnim( anim );
    if( i != SceneManager::GetInstance()->GetAnimsEnd() )
    {
        ResourceSet* res = i->GetResSource();
        return res->GetHashID();
    }
    return 0;
}

static const char*
GetName( nw::gfx::Camera* camera )
{
    SceneManager::CameraList::iterator i = SceneManager::GetInstance()->FindCamera( camera );
    if( i != SceneManager::GetInstance()->GetCamerasEnd() )
    {
        return i->GetName();
    }
    return 0;
}

static void
OutputError(
    bool isError,
    vw::system::LogSystem::TYPE type,
    const char* name,
    const char* message,
    unsigned int hashID,
    void* mainKey,
    unsigned int subKey
) {
    // ��U�폜
    vw::system::LogSystem::GetInstance()->Clear( mainKey, subKey );

    // �G���[�̏ꍇ���ߒǉ�
    if ( isError ) 
    {
        CharArray log(vw::system::Memory_GetSysAllocator());
        log = name;
        log += ": ";
        log += message;
        if ( type == vw::system::LogSystem::ERROR ) {
            vw::system::LogSystem::GetInstance()->AddError(
                log.Get(),
                hashID,
                mainKey,
                subKey
            );
        } else {
            vw::system::LogSystem::GetInstance()->AddWarning(
                log.Get(),
                hashID,
                mainKey,
                subKey
            );
        }
    } 
}
#endif

#if 0
static void
CheckTextureReference( 
    bool isValid,
    nw::gfx::res::ResReferenceTexture refTex,
    const char* name,
    const char* message,
    unsigned int hashID,
    void* mainKey,
    unsigned int subKey
) {

    bool error = false;
    if ( isValid ) {
        void* pDefaultTex = 
            ResourceManager::GetInstance()->GetDefaultTexture().ptr();
        if ( !refTex.IsValid() || refTex.ref().toTargetTexture.to_ptr() == pDefaultTex ) error = true;
    } 
    OutputError( 
        error,
        vw::system::LogSystem::WARNING,
        name, message, hashID,
        mainKey, subKey
    );
}
#endif

#if 0
static void
CheckLookupTableReference( 
    bool isValid,
    nw::gfx::res::ResReferenceLookupTable refLut,
    const char* name,
    const char* message,
    unsigned int hashID,
    void* mainKey,
    unsigned int subKey
) {

    bool error = false;
    if ( isValid ) {
        void* pDefaultLut = 
            ResourceManager::GetInstance()->GetDefaultLookupTable().ptr();
        if ( !refLut.IsValid() || refLut.ref().toTargetLut.to_ptr() == pDefaultLut ) error = true;
    } 
    OutputError( 
        error,
        vw::system::LogSystem::ERROR,
        name, message, hashID,
        mainKey, subKey
    );
}

void
ErrorCheck::CheckModelTextureCombiner( nw::gfx::Model* model )
{
    if (!model) return;

    ModelElem* modelElem = GetModelElem( model );
    if (!modelElem) return;

    nw::gfx::res::ResTextureCombiner::Source defaultValue = 
        nw::gfx::res::ResTextureCombiner::SOURCE_CONSTANT;
    unsigned int hashID = modelElem->GetResSource()->GetHashID();
    for ( int i = 0; i < model->GetMaterialCount(); ++i )
    {
        // �}�e���A�������擾
        const char* matName = model->GetMaterial( i )->GetOriginal().GetName();

        // �}�e���A�����擾
        nw::gfx::res::ResMaterial res_material[] = {
            model->GetMaterial( i )->GetBuffer(), model->GetMaterial( i )->GetOriginal()
        };
        if ( !res_material[ 0 ].IsValid() ) return;

        // �t���O�����g�V�F�[�_�[�̎擾
        nw::gfx::res::ResFragmentShader res_shader[] = {
            res_material[ 0 ].GetFragmentShader(), res_material[ 1 ].GetFragmentShader()
        };
        if ( !res_shader[ 0 ].IsValid() ) return;

        // 0�i�̃e�N�X�`���R���o�C�i�̎擾
        nw::gfx::res::ResTextureCombiner res_combiner[] = {
            res_shader[ 0 ].GetTextureCombiners( 0 ), res_shader[ 1 ].GetTextureCombiners( 0 ) 
        };
        if ( !res_combiner[ 0 ].IsValid() ) return;

        {
            char message[128];
            const nw::gfx::res::ResTextureCombiner::Source errorList[] = {
                nw::gfx::res::ResTextureCombiner::SOURCE_PREVIOUS,
                nw::gfx::res::ResTextureCombiner::SOURCE_PREVIOUS_BUFFER,
            };
            const char* errorMessage[] = { "[Previous]", "[Previous Buffer]" };

            // �v�f���̌v�Z
            const int errorNum = 
                sizeof(errorList) / sizeof(nw::gfx::res::ResTextureCombiner::Source);

            // SourceRgb�̒���
            for ( int j = 0; j < 3; ++j ) 
            {
                for ( int k = 0; k < errorNum; ++k )
                {
                    bool error = ( res_combiner[ 0 ].GetSourceRgb( j ) == errorList[k] );
                    if ( error ) {
                        res_combiner[ 0 ].SetSourceRgb( j, defaultValue );
                        res_combiner[ 1 ].SetSourceRgb( j, defaultValue );
                    }

                    const int subKey = errorNum * j + k;
                    sprintf( message, "Combiner0 Rgb[%d] is %s", j, errorMessage[k] );

                    // �G���[�o��
                    OutputError(
                        error,
                        vw::system::LogSystem::WARNING,
                        matName, message,
                        hashID, res_combiner[ 0 ].ptr(), subKey
                    );
                }
            }

            // SourceAlpha�̒���
            for ( int j = 0; j < 3; ++j ) 
            {
                for ( int k = 0; k < errorNum; ++k )
                {
                    bool error = ( res_combiner[ 0 ].GetSourceAlpha( j ) == errorList[k] );
                    if ( error ) {
                        res_combiner[ 0 ].SetSourceAlpha( j, defaultValue );
                        res_combiner[ 1 ].SetSourceAlpha( j, defaultValue );
                    }

                    const int subKey = errorNum * j + k + ( 3 * errorNum );
                    sprintf( message, "Combiner0 Alpha[%d] is %s", j, errorMessage[k] );

                    // �G���[�o��
                    OutputError(
                        error,
                        vw::system::LogSystem::WARNING,
                        matName, message,
                        hashID, res_combiner[ 0 ].ptr(), subKey
                    );
                }
            }
        }
    }
}
#endif

#if 0
void
ErrorCheck::CheckModelTextureReference( nw::gfx::Model* model )
{
    if (!model) return;

    ModelElem* modelElem = GetModelElem( model );
    if (!modelElem) return;

    unsigned int hashID = modelElem->GetResSource()->GetHashID();
    for ( int i = 0; i < model->GetMaterialCount(); ++i )
    {
        // �}�e���A�������擾
        const char* matName = model->GetMaterial( i )->GetOriginal().GetName();

        // �}�e���A�����擾
        nw::gfx::res::ResMaterial res_material = model->GetMaterial( i )->GetBuffer();
        if ( !res_material.IsValid() ) return;

        // �e�N�X�`���Q�Ƃ̃`�F�b�N
        {
            // �C���[�W�e�N�X�`���̊O���Q�Ƃ��`�F�b�N
            for ( int j = 0; j < res_material.GetTextureMappersCount(); ++j )
            {
                // �e�N�X�`���}�b�p�[�f�[�^���擾
                nw::gfx::res::ResPixelBasedTextureMapper res_mapper = res_material.GetTextureMappers( j );
                if ( res_mapper.IsValid() ) 
                {
                    // �e�N�X�`���}�b�p�[�̊O���Q�Ɛ�𒲍�
                    nw::gfx::res::ResReferenceTexture res_ref;
                    {
                        res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceTexture>(
                            res_mapper.GetTexture()
                        );
                    }
                    char message[64];
                    sprintf( message, "Texture[%d] is not assinged", j );
                    CheckTextureReference(
                        true,
                        res_ref,
                        matName, message,
                        hashID, res_mapper.ptr(), 0
                    );
                }
            }

            // �v���V�[�W�����e�N�X�`���̊O���Q�Ƃ��`�F�b�N
            {
                // �e�N�X�`���}�b�p�[�f�[�^���擾
                nw::gfx::res::ResProceduralTextureMapper res_mapper = res_material.GetProceduralTextureMapper();
                if ( res_mapper.IsValid() )
                {
                    // �e�N�X�`���}�b�p�[�̊O���Q�Ɛ�𒲍�
                    nw::gfx::res::ResReferenceTexture res_ref;
                    {
                        res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceTexture>(
                            res_mapper.GetTexture()
                        );
                    }
                    CheckTextureReference(
                        true,
                        res_ref,
                        matName, "Proc Texture is not assigned",
                        hashID, res_mapper.ptr(), 0
                    );
                }
            }
        }
    }
}
#endif

#if 0
void
ErrorCheck::CheckModelLookupTableReference( nw::gfx::Model* model )
{
    if (!model) return;

    ModelElem* modelElem = GetModelElem( model );
    if (!modelElem) return;

    unsigned int hashID = modelElem->GetResSource()->GetHashID();
    for ( int i = 0; i < model->GetMaterialCount(); ++i )
    {
        // �}�e���A�������擾
        const char* matName = model->GetMaterial( i )->GetOriginal().GetName();

        // �}�e���A�����擾
        nw::gfx::res::ResMaterial res_material = model->GetMaterial( i )->GetBuffer();
        if ( !res_material.IsValid() ) return;

        // �Q�ƃe�[�u���̊O���Q�Ƃ��`�F�b�N
        {
            // �t���O�����g�V�F�[�_�f�[�^���擾
            nw::gfx::res::ResFragmentShader res_shader = res_material.GetFragmentShader();
            if ( !res_shader.IsValid() ) return;

            // ���b�N�A�b�v�e�[�u���f�[�^���擾                    
            nw::gfx::res::ResFragmentLightingTable res_table = res_shader.GetFragmentLightingTable();
            if ( !res_table.IsValid() ) return;
            
            // ���C�e�B���O�f�[�^���擾
            nw::gfx::res::ResFragmentLighting res_lighting = res_shader.GetFragmentLighting();
            if ( !res_lighting.IsValid() ) return;

            // ���C�e�B���O�t���O���擾
            bool isDis0Enbaled = 
                nw::ut::CheckFlag(res_lighting.GetFlags(), nw::gfx::res::ResFragmentLightingData::FLAG_DISTRIBUTION0_ENABLED);
            bool isDis1Enbaled = 
                nw::ut::CheckFlag(res_lighting.GetFlags(), nw::gfx::res::ResFragmentLightingData::FLAG_DISTRIBUTION1_ENABLED);
            bool isRefEnabled = 
                nw::ut::CheckFlag(res_lighting.GetFlags(), nw::gfx::res::ResFragmentLightingData::FLAG_REFLECTION_ENABLED);
            bool isFresnelEnabled = 
                (res_lighting.GetFresnelConfig() != nw::gfx::res::ResFragmentLighting::CONFIG_NO_FRESNEL);

            // ���b�N�A�b�v�e�[�u���̊O���Q�Ɛ�𒲍�
            bool isSamplerValid;
            nw::gfx::res::ResReferenceLookupTable res_ref;
            {
                // ReflectanceRSampler�̒���
                {
                    isSamplerValid = res_table.GetReflectanceRSampler().IsValid();
                    if ( isSamplerValid )
                    {
                        res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceLookupTable>(
                            res_table.GetReflectanceRSampler().GetSampler()
                        );
                    }
                    CheckLookupTableReference(
                        isSamplerValid && isRefEnabled,
                        res_ref,
                        matName, "RefR Lut is not assigned",
                        hashID, res_table.ptr(), ErrorModelRefRSampler
                    );
                }

                // ReflectanceGSampler�̒���
                {
                    isSamplerValid = res_table.GetReflectanceGSampler().IsValid();
                    if ( isSamplerValid )
                    {                
                        res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceLookupTable>(
                            res_table.GetReflectanceGSampler().GetSampler()
                        );
                    }
                    CheckLookupTableReference(
                        isSamplerValid && isRefEnabled,
                        res_ref,
                        matName, "RefG Lut is not assigned",
                        hashID, res_table.ptr(), ErrorModelRefGSampler
                    );
                }

                // ReflectanceBSampler�̒���
                {
                    isSamplerValid = res_table.GetReflectanceBSampler().IsValid();
                    if ( isSamplerValid )
                    {
                        res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceLookupTable>(
                            res_table.GetReflectanceBSampler().GetSampler()
                        );
                    }
                    CheckLookupTableReference(
                        isSamplerValid && isRefEnabled,
                        res_ref,
                        matName, "RefB Lut is not assigned",
                        hashID, res_table.ptr(), ErrorModelRefBSampler
                    );
                }

                // Distribution0Sampler�̒���
                {
                    isSamplerValid = res_table.GetDistribution0Sampler().IsValid();
                    if ( isSamplerValid )
                    {
                        res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceLookupTable>(
                            res_table.GetDistribution0Sampler().GetSampler()
                        );
                    }
                    CheckLookupTableReference(
                        isSamplerValid && isDis0Enbaled,
                        res_ref,
                        matName, "Dis0 Lut is not assigned",
                        hashID, res_table.ptr(), ErrorModelDis0Sampler
                    );
                }

                // Distribution1Sampler�̒���
                {
                    isSamplerValid = res_table.GetDistribution1Sampler().IsValid();
                    if ( isSamplerValid )
                    {
                        res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceLookupTable>(
                            res_table.GetDistribution1Sampler().GetSampler()
                        );
                    }
                    CheckLookupTableReference(
                        isSamplerValid && isDis1Enbaled,
                        res_ref,
                        matName, "Dis1 Lut is not assigned",
                        hashID, res_table.ptr(), ErrorModelDis1Sampler
                    );
                }

                // FresnelSampler�̒���
                {
                    isSamplerValid = res_table.GetFresnelSampler().IsValid();
                    if ( isSamplerValid )
                    {
                        res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceLookupTable>(
                            res_table.GetFresnelSampler().GetSampler()
                        );
                    }
                    CheckLookupTableReference(
                        isSamplerValid && isFresnelEnabled,
                        res_ref,
                        matName, "Fresnel Lut is not assigned",
                        hashID, res_table.ptr(), ErrorModelFresnelSampler
                    );
                }
            }
        }    
    }
}

void
ErrorCheck::CheckModelFogIndexBoundary( nw::gfx::Model* model )
{
    if (!model) return;

    ModelElem* modelElem = GetModelElem( model );
    if (!modelElem) return;

    nw::gfx::SceneEnvironment& sceneEnvironment = 
        vw::system::Video_GetRenderContext()->GetSceneEnvironment();

    nw::gfx::Fog* defaultFog = 
        SceneManager::GetInstance()->GetVoidFog();

    unsigned int hashID = modelElem->GetResSource()->GetHashID();
    for ( int i = 0; i < model->GetMaterialCount(); ++i )
    {
        // �}�e���A�������擾
        const char* matName = model->GetMaterial( i )->GetOriginal().GetName();

        // �}�e���A�����擾
        nw::gfx::res::ResMaterial res_material[] = {
            model->GetMaterial( i )->GetBuffer(), model->GetMaterial( i )->GetOriginal()
        };
        if ( !res_material[ 0 ].IsValid() ) return;

        // �t�H�O�t���O���擾
        bool isFogEnbaled = 
            nw::ut::CheckFlag(res_material[ 0 ].GetFlags(), nw::gfx::res::ResMaterialData::FLAG_FOG_ENABLED);

        // �t�H�O�C���f�b�N�X�̏���`�F�b�N
        {
            // �G���[�`�F�b�N
            int fogIndex = res_material[ 0 ].GetFogIndex();
            int maxFog = nw::gfx::RenderContext::DEFAULT_MAX_FOGS;
            bool error = 
                isFogEnbaled &&
                fogIndex >= maxFog;
            if (error) {
                res_material[ 0 ].SetFogIndex( maxFog - 1 );
                res_material[ 1 ].SetFogIndex( maxFog - 1 );
            }

            // �G���[�o��
            char str[128];
            sprintf( str, "Fog index(%d) >= Viewer max fog index(%d)", fogIndex, maxFog - 1 );
            OutputError(
                error,
                vw::system::LogSystem::WARNING,
                matName, str,
                hashID, res_material[ 0 ].ptr(), ErrorModelFogUpperBoundary
            );
        }

        // �t�H�O�C���f�b�N�X�̉����`�F�b�N
        {
            // �G���[�`�F�b�N
            int fogIndex = res_material[ 0 ].GetFogIndex();
            bool error = 
                isFogEnbaled &&
                fogIndex < 0;
            if (error) {
                res_material[ 0 ].SetFogIndex( 0 );
                res_material[ 1 ].SetFogIndex( 0 );
            }

            // �G���[�o��
            char str[128];
            sprintf( str, "Fog index(%d) < 0", fogIndex );
            OutputError(
                error,
                vw::system::LogSystem::WARNING,
                matName, str,
                hashID, res_material[ 0 ].ptr(), ErrorModelFogLowerBoundary
            );
        }
    }
}

void
ErrorCheck::CheckModelFogIndexReference( nw::gfx::Model* model )
{
    if (!model) return;

    ModelElem* modelElem = GetModelElem( model );
    if (!modelElem) return;

    nw::gfx::SceneEnvironment& sceneEnvironment = 
        vw::system::Video_GetRenderContext()->GetSceneEnvironment();

    nw::gfx::Fog* defaultFog = 
        SceneManager::GetInstance()->GetVoidFog();

    unsigned int hashID = modelElem->GetResSource()->GetHashID();
    for ( int i = 0; i < model->GetMaterialCount(); ++i )
    {
        // �}�e���A�������擾
        const char* matName = model->GetMaterial( i )->GetOriginal().GetName();

        // �}�e���A�����擾
        nw::gfx::res::ResMaterial res_material = model->GetMaterial( i )->GetBuffer();
        if ( !res_material.IsValid() ) return;

        // �t�H�O�t���O���擾
        bool isFogEnbaled = 
            nw::ut::CheckFlag(res_material.GetFlags(), nw::gfx::res::ResMaterialData::FLAG_FOG_ENABLED);

        // �t�H�O�Q�Ɛ�̃`�F�b�N
        {
            // �G���[�`�F�b�N
            int fogIndex = res_material.GetFogIndex();
            bool error = 
                isFogEnbaled &&
                ( sceneEnvironment.GetFog(fogIndex) == NULL ||
                  sceneEnvironment.GetFog(fogIndex) == defaultFog );
            if (error) sceneEnvironment.SetFog( fogIndex, defaultFog ); 

            // �G���[�o��
            char str[128];
            sprintf( str, "Invalid fog index(%d)", fogIndex );
            OutputError(
                error,
                vw::system::LogSystem::WARNING,
                matName, str,
                hashID, res_material.ptr(), ErrorModelFogReference
            );
        }
    }
}

void
ErrorCheck::CheckModelLightSetIndexBoundary( nw::gfx::Model* model )
{
    if (!model) return;

    ModelElem* modelElem = GetModelElem( model );
    if (!modelElem) return;

    nw::gfx::SceneEnvironment& sceneEnvironment = 
        vw::system::Video_GetRenderContext()->GetSceneEnvironment();

    nw::gfx::LightSet* defaultLightSet = 
        SceneManager::GetInstance()->GetVoidLightSet();

    unsigned int hashID = modelElem->GetResSource()->GetHashID();
    for ( int i = 0; i < model->GetMaterialCount(); ++i )
    {
        // �}�e���A�������擾
        const char* matName = model->GetMaterial( i )->GetOriginal().GetName();

        // �}�e���A�����擾
        nw::gfx::res::ResMaterial res_material[] = {
            model->GetMaterial( i )->GetBuffer(), model->GetMaterial( i )->GetOriginal()
        };
        if ( !res_material[ 0 ].IsValid() ) return;

        // ���C�g�Z�b�g�C���f�b�N�X�̏���`�F�b�N
        {
            // �G���[�`�F�b�N
            int lightSetIndex = res_material[ 0 ].GetLightSetIndex();
            int maxLightSet = nw::gfx::RenderContext::DEFAULT_MAX_LIGHT_SETS;
            bool error = lightSetIndex >= maxLightSet;
            if (error) {
                res_material[ 0 ].SetLightSetIndex( maxLightSet - 1 );
                res_material[ 1 ].SetLightSetIndex( maxLightSet - 1 );
            }

            // �G���[�o��
            char str[128];
            sprintf( 
                str,
                "Light set index(%d) >= Viewer max light set index(%d)",
                lightSetIndex,
                maxLightSet - 1
            );
            OutputError(
                error,
                vw::system::LogSystem::WARNING,
                matName, str,
                hashID, res_material[ 0 ].ptr(), ErrorModelLightSetUpperBoundary
            );
        }

        // ���C�g�Z�b�g�C���f�b�N�X�̉����`�F�b�N
        {
            // �G���[�`�F�b�N
            int lightSetIndex = res_material[ 0 ].GetLightSetIndex();
            bool error = lightSetIndex < 0;
            if (error) {
                res_material[ 0 ].SetLightSetIndex( 0 );
                res_material[ 1 ].SetLightSetIndex( 0 );
            }

            // �G���[�o��
            char str[128];
            sprintf( str, "Light set index(%d) < 0", lightSetIndex );
            OutputError(
                error,
                vw::system::LogSystem::WARNING,
                matName, str,
                hashID, res_material[ 0 ].ptr(), ErrorModelLightSetLowerBoundary
            );
        }
    }
}

void
ErrorCheck::CheckModelLightSetIndexReference( nw::gfx::Model* model )
{
    if (!model) return;

    ModelElem* modelElem = GetModelElem( model );
    if (!modelElem) return;

    nw::gfx::SceneEnvironment& sceneEnvironment = 
        vw::system::Video_GetRenderContext()->GetSceneEnvironment();

    nw::gfx::LightSet* defaultLightSet = 
        SceneManager::GetInstance()->GetVoidLightSet();

    unsigned int hashID = modelElem->GetResSource()->GetHashID();
    for ( int i = 0; i < model->GetMaterialCount(); ++i )
    {
        // �}�e���A�������擾
        const char* matName = model->GetMaterial( i )->GetOriginal().GetName();

        // �}�e���A�����擾
        nw::gfx::res::ResMaterial res_material = model->GetMaterial( i )->GetBuffer();
        if ( !res_material.IsValid() ) return;

        // ���C�g�Z�b�g�Q�Ɛ�̃`�F�b�N
        {
            // �G���[�`�F�b�N
            int lightSetIndex = res_material.GetLightSetIndex();
            bool error = 
                sceneEnvironment.GetLightSet(lightSetIndex) == NULL ||
                sceneEnvironment.GetLightSet(lightSetIndex) == defaultLightSet;
            if (error) sceneEnvironment.SetLightSet( lightSetIndex, defaultLightSet ); 

            // �G���[�o��
            char str[128];
            sprintf( str, "Invalid light set index(%d)", lightSetIndex );
            OutputError(
                error,
                vw::system::LogSystem::WARNING,
                matName, str,
                hashID, res_material.ptr(), ErrorModelLightSetReference
            );
        }
    }
}

void
ErrorCheck::CheckModelCameraReferenceIndexBoundary( nw::gfx::Model* model )
{
    if (!model) return;

    ModelElem* modelElem = GetModelElem( model );
    if (!modelElem) return;

    nw::gfx::SceneEnvironment& sceneEnvironment = 
        vw::system::Video_GetRenderContext()->GetSceneEnvironment();

    nw::gfx::Camera* defaultCamera = 
        SceneManager::GetInstance()->GetDefaultCamera()->GetSceneNode();

    unsigned int hashID = modelElem->GetResSource()->GetHashID();
    for ( int i = 0; i < model->GetMaterialCount(); ++i )
    {
        // �}�e���A�������擾
        const char* matName = model->GetMaterial( i )->GetOriginal().GetName();

        // �}�e���A�����擾
        nw::gfx::res::ResMaterial res_material[] = {
            model->GetMaterial( i )->GetBuffer(), model->GetMaterial( i )->GetOriginal()
        };
        if ( !res_material[ 0 ].IsValid() ) return;

        // �Q�ƃJ�����̃C���f�b�N�X���`�F�b�N
        int coordinatorsCount = res_material[ 0 ].GetActiveTextureCoordinatorsCount();
        for (int j = 0; j < coordinatorsCount; ++j)
        {
            nw::gfx::res::ResTextureCoordinator res_coord[] = {
                res_material[ 0 ].GetTextureCoordinators( j ), res_material[ 1 ].GetTextureCoordinators( j )
            };
            if ( !res_coord[ 0 ].IsValid() ) return;

            // �J�����C���f�b�N�X�̏���`�F�b�N
            {
                // �G���[�`�F�b�N
                int cameraIndex = res_coord[ 0 ].GetReferenceCamera();
                int maxCamera = nw::gfx::RenderContext::DEFAULT_MAX_CAMERAS;
                bool error = 
                    res_coord[ 0 ].GetMappingMethod() == nw::gfx::res::ResTextureCoordinator::MAPPINGMETHOD_PROJECTION &&
                    cameraIndex >= maxCamera;
                if (error) {
                    res_coord[ 0 ].SetReferenceCamera( maxCamera - 1 );
                    res_coord[ 1 ].SetReferenceCamera( maxCamera - 1 );
                    res_coord[ 0 ].SetDirty( true );
                    res_coord[ 1 ].SetDirty( true );
                }

                // �G���[�o��
                char str[128];
                sprintf( 
                    str,
                    "TexCoord[%d] camera ref index(%d) >= Viewer max camera index(%d)",
                    j,
                    cameraIndex,
                    maxCamera - 1
                );
                OutputError(
                    error,
                    vw::system::LogSystem::WARNING,
                    matName, str,
                    hashID, res_coord[ 0 ].ptr(), ErrorModelCameraReferenceIndexUpperBoundary
                );
            }
        }
    }
}

void
ErrorCheck::CheckModelCameraReferenceIndexReference( nw::gfx::Model* model )
{
    if (!model) return;

    ModelElem* modelElem = GetModelElem( model );
    if (!modelElem) return;

    nw::gfx::SceneEnvironment& sceneEnvironment = 
        vw::system::Video_GetRenderContext()->GetSceneEnvironment();

    nw::gfx::Camera* defaultCamera = 
        SceneManager::GetInstance()->GetDefaultCamera()->GetSceneNode();

    unsigned int hashID = modelElem->GetResSource()->GetHashID();
    for ( int i = 0; i < model->GetMaterialCount(); ++i )
    {
        // �}�e���A�������擾
        const char* matName = model->GetMaterial( i )->GetOriginal().GetName();

        // �}�e���A�����擾
        nw::gfx::res::ResMaterial res_material = model->GetMaterial( i )->GetBuffer();
        if ( !res_material.IsValid() ) return;

        // �Q�ƃJ�����̃C���f�b�N�X���`�F�b�N
        int coordinatorsCount = res_material.GetActiveTextureCoordinatorsCount();
        for (int j = 0; j < coordinatorsCount; ++j)
        {
            nw::gfx::res::ResTextureCoordinator res_coord = res_material.GetTextureCoordinators( j );
            if ( !res_coord.IsValid() ) return;

            // �J�����Q�Ɛ�̃`�F�b�N
            {
                // �G���[�`�F�b�N
                int cameraIndex = res_coord.GetReferenceCamera();
                bool error = 
                    cameraIndex >= 0 && 
                    res_coord.GetMappingMethod() == nw::gfx::res::ResTextureCoordinator::MAPPINGMETHOD_PROJECTION &&
                    ( sceneEnvironment.GetCamera(cameraIndex) == NULL ||
                      sceneEnvironment.GetCamera(cameraIndex) == defaultCamera );
                if (error) sceneEnvironment.SetCamera( cameraIndex, defaultCamera ); 

                // �G���[�o��
                char str[128];
                sprintf( 
                    str,
                    "TexCoord[%d] has invalid camera ref index(%d)",
                    j,
                    cameraIndex
                );
                OutputError(
                    error,
                    vw::system::LogSystem::ERROR,
                    matName, str,
                    hashID, res_coord.ptr(), ErrorModelCameraReferenceIndexReference
                );
            }
        }
    }
}

void
ErrorCheck::CheckCameraNear( nw::gfx::Camera* camera )
{
    if (!camera) return;

    bool error = false;
    if (camera->GetResCamera().GetProjectionType() == nw::gfx::ResCamera::PROJTYPE_PERSPECTIVE)
    {
        nw::gfx::ResPerspectiveProjectionUpdater resProjectionUpdater =
            nw::gfx::ResDynamicCast<nw::gfx::ResPerspectiveProjectionUpdater>(
            camera->GetProjectionUpdater()->GetResource());

        f32 near = resProjectionUpdater.GetNear();
        if ( near == 0.0f )
        {
            error = true;
            resProjectionUpdater.SetNear( 0.01f );
        }
    }
    else if(camera->GetResCamera().GetProjectionType() == nw::gfx::ResCamera::PROJTYPE_ORTHO)
    {
        nw::gfx::ResOrthoProjectionUpdater resOrthoUpdater =
            nw::gfx::ResDynamicCast<nw::gfx::ResOrthoProjectionUpdater>(
            camera->GetProjectionUpdater()->GetResource());

        f32 near = resOrthoUpdater.GetNear();
        if ( near == 0.0f )
        {
            error = true;
            resOrthoUpdater.SetNear( 0.01f );
        }
    }

    // �G���[�o��
    if (camera->GetAnimGroup() && camera->GetAnimObject()) {
        if (error) {
            nw::gfx::AnimEvaluator* eval = 
                nw::ut::DynamicCast<nw::gfx::AnimEvaluator*>(camera->GetAnimObject());
            nw::anim::res::ResAnim anim = eval->GetAnimData();
            char str[128];
            sprintf( 
                str,
                "Camera Near is zero at Frame[%.1f]",
                eval->GetFrame()
            );
            OutputError(
                error,
                vw::system::LogSystem::ERROR,
                anim.GetName(), str,
                GetHashID( anim ), eval, ErrorCameraNear
            );
        }
    } else {
        OutputError(
            error,
            vw::system::LogSystem::ERROR,
            GetName( camera ), "Camera Near is zero",
            GetHashID( camera ), camera, ErrorCameraNear
        );
    }
}

void
ErrorCheck::CheckCameraNearFar( nw::gfx::Camera* camera )
{
    if (!camera) return;

    bool error = false;
    if (camera->GetResCamera().GetProjectionType() == nw::gfx::ResCamera::PROJTYPE_PERSPECTIVE)
    {
        nw::gfx::ResPerspectiveProjectionUpdater resProjectionUpdater =
            nw::gfx::ResDynamicCast<nw::gfx::ResPerspectiveProjectionUpdater>(
            camera->GetProjectionUpdater()->GetResource());

        f32 Near = resProjectionUpdater.GetNear();
        f32 Far  = resProjectionUpdater.GetFar();
        if ( Near >= Far )
        {
            error = true;
            resProjectionUpdater.SetFar( Near + 0.01f );
        }
    }
    else if(camera->GetResCamera().GetProjectionType() == nw::gfx::ResCamera::PROJTYPE_ORTHO)
    {
        nw::gfx::ResOrthoProjectionUpdater resOrthoUpdater =
            nw::gfx::ResDynamicCast<nw::gfx::ResOrthoProjectionUpdater>(
            camera->GetProjectionUpdater()->GetResource());

        f32 Near = resOrthoUpdater.GetNear();
        f32 Far  = resOrthoUpdater.GetFar();
        if ( Near >= Far )
        {
            error = true;
            resOrthoUpdater.SetFar( Near + 0.01f );
        }
    }

    // �G���[�o��
    if (camera->GetAnimGroup() && camera->GetAnimObject()) {
        if (error) {
            nw::gfx::AnimEvaluator* eval = 
                nw::ut::DynamicCast<nw::gfx::AnimEvaluator*>(camera->GetAnimObject());
            nw::anim::res::ResAnim anim = eval->GetAnimData();
            char str[128];
            sprintf( 
                str,
                "Camera Near >= Far at Frame[%.1f]",
                eval->GetFrame()
            );
            OutputError(
                error,
                vw::system::LogSystem::ERROR,
                anim.GetName(), str,
                GetHashID( anim ), eval, ErrorCameraNearFar
            );
        }
    } else {
        OutputError(
            error,
            vw::system::LogSystem::ERROR,
            GetName( camera ), "Camera Near >= Far",
            GetHashID( camera ), camera, ErrorCameraNearFar
        );
    }
}

void
ErrorCheck::CheckCameraPosition( nw::gfx::Camera* camera )
{
    if (!camera) return;

    nw::gfx::ResCameraViewUpdater       resViewUpdater             = camera->GetViewUpdater()->GetResource();
    nw::gfx::ResAimTargetViewUpdater    resAimTargetViewUpdater    = nw::gfx::ResDynamicCast<nw::gfx::ResAimTargetViewUpdater>(resViewUpdater);
    nw::gfx::ResLookAtTargetViewUpdater resLookAtTargetViewUpdater = nw::gfx::ResDynamicCast<nw::gfx::ResLookAtTargetViewUpdater>(resViewUpdater);

    nw::math::VEC3 viewPos;
    camera->Transform().GetTranslate( &viewPos );
    bool error = false;
    if( resAimTargetViewUpdater.IsValid() )
    {
        // �J�����̈ʒu�ƒ����_��������
        nw::math::VEC3 targetPos = resAimTargetViewUpdater.GetTargetPosition();
        u32 nFlags = resAimTargetViewUpdater.GetFlags();
        if( nFlags & nw::gfx::res::ResAimTargetViewUpdaterData::FLAG_INHERITING_TARGET_TRANSLATE )
        {
            if( targetPos == nw::math::VEC3(0,0,0) )
            {
                error = true;
            }
        }
        else
        {
            if( targetPos == viewPos )
            {
                error = true;
            }
        }
        if( error )
        {
            targetPos.z -= 1.0f;
            resAimTargetViewUpdater.SetTargetPosition( targetPos );
        }
    }
    else if( resLookAtTargetViewUpdater.IsValid() )
    {
        // �J�����̈ʒu�ƒ����_��������
        nw::math::VEC3 targetPos = resLookAtTargetViewUpdater.GetTargetPosition();
        if( viewPos == targetPos )
        {
            targetPos.z -= 1.0f;
            resLookAtTargetViewUpdater.SetTargetPosition( targetPos );
            error = true;
        }
    }

    // �G���[�o��
    if (camera->GetAnimGroup() && camera->GetAnimObject()) {
        if (error) {
            nw::gfx::AnimEvaluator* eval = 
                nw::ut::DynamicCast<nw::gfx::AnimEvaluator*>(camera->GetAnimObject());
            nw::anim::res::ResAnim anim = eval->GetAnimData();
            char str[128];
            sprintf( 
                str,
                "CameraPos == TargetPos at Frame[%.1f]",
                eval->GetFrame()
            );
            OutputError(
                error,
                vw::system::LogSystem::ERROR,
                anim.GetName(), str,
                GetHashID( anim ), eval, ErrorCameraPosition
            );
        }
    } else {
        OutputError(
            error,
            vw::system::LogSystem::ERROR,
            GetName( camera ), "CameraPos == TargetPos",
            GetHashID( camera ), camera, ErrorCameraPosition
        );
    }
}

void
ErrorCheck::CheckCameraUpVectorZero( nw::gfx::Camera* camera )
{
    if (!camera) return;

    nw::gfx::ResCameraViewUpdater resViewUpdater = camera->GetViewUpdater()->GetResource();
    nw::gfx::ResLookAtTargetViewUpdater resLookAtTargetViewUpdater = nw::gfx::ResDynamicCast<nw::gfx::ResLookAtTargetViewUpdater>(resViewUpdater);

    bool error = false;
    if( resLookAtTargetViewUpdater.IsValid() )
    {
        // �A�b�v�x�N�^�[�̃T�C�Y���O���m�F
        nw::math::VEC3 UpwardVector = resLookAtTargetViewUpdater.GetUpwardVector();
        if( UpwardVector.x == 0.0f && UpwardVector.y == 0.0f && UpwardVector.z == 0.0f )
        {
            error = true;
            resLookAtTargetViewUpdater.SetUpwardVector( 0.0f, 1.0f, 0.0f );
        }
    }

    // �G���[�o��
    if (camera->GetAnimGroup() && camera->GetAnimObject()) {
        if (error) {
            nw::gfx::AnimEvaluator* eval = 
                nw::ut::DynamicCast<nw::gfx::AnimEvaluator*>(camera->GetAnimObject());
            nw::anim::res::ResAnim anim = eval->GetAnimData();
            char str[128];
            sprintf( 
                str,
                "Camera UpwardVector is zero at Frame[%.1f]",
                eval->GetFrame()
            );
            OutputError(
                error,
                vw::system::LogSystem::ERROR,
                anim.GetName(), str,
                GetHashID( anim ), eval, ErrorCameraUpVectorZero
            );
        }
    } else {
        OutputError(
            error,
            vw::system::LogSystem::ERROR,
            GetName( camera ), "Camera UpwardVector is zero",
            GetHashID( camera ), camera, ErrorCameraUpVectorZero
        );
    }
}

void
ErrorCheck::CheckCameraUpVectorParallel( nw::gfx::Camera* camera )
{
    if (!camera) return;

    nw::gfx::ResCameraViewUpdater resViewUpdater = camera->GetViewUpdater()->GetResource();
    nw::gfx::ResLookAtTargetViewUpdater resLookAtTargetViewUpdater = nw::gfx::ResDynamicCast<nw::gfx::ResLookAtTargetViewUpdater>(resViewUpdater);

    bool error = false;
    if( resLookAtTargetViewUpdater.IsValid() )
    {
        // �����x�N�g���ƃA�b�v�x�N�g�������s�ł͂����Ȃ�
        nw::math::VEC3 cameraPosition;
        camera->Transform().GetTranslate( &cameraPosition );
        nw::math::VEC3 targetPos = resLookAtTargetViewUpdater.GetTargetPosition();
        nw::math::VEC3 UpwardVector = resLookAtTargetViewUpdater.GetUpwardVector();
        if( nw::math::VEC3().Cross(UpwardVector, targetPos-cameraPosition).IsZero() )
        {
            error = true;
            nw::math::VEC3Normalize( &UpwardVector, &UpwardVector );
            if( UpwardVector.x == 0.0f && UpwardVector.y == 1.0f && UpwardVector.z == 0.0f )
            {
                resLookAtTargetViewUpdater.SetUpwardVector( 0.0f, 0.0f, 1.0f );
            }
            else
            {
                resLookAtTargetViewUpdater.SetUpwardVector( 0.0f, 1.0f, 0.0f );
            }
        }
    }

    // �G���[�o��
    if (camera->GetAnimGroup() && camera->GetAnimObject()) {
        if (error) {
            nw::gfx::AnimEvaluator* eval = 
                nw::ut::DynamicCast<nw::gfx::AnimEvaluator*>(camera->GetAnimObject());
            nw::anim::res::ResAnim anim = eval->GetAnimData();
            char str[128];
            sprintf( 
                str,
                "Camera Vector is parallel at Frame[%.1f]",
                eval->GetFrame()
            );
            OutputError(
                error,
                vw::system::LogSystem::ERROR,
                anim.GetName(), str,
                GetHashID( anim ), eval, ErrorCameraUpVectorParallel
            );
        }
    } else {
        OutputError(
            error,
            vw::system::LogSystem::ERROR,
            GetName( camera ), "Camera Vector is parallel",
            GetHashID( camera ), camera, ErrorCameraUpVectorParallel
        );
    }
}

void
ErrorCheck::CheckLightLookupTableReference( nw::gfx::Light* light )
{
    if (!light) return;

    LightElem* lightElem = GetLightElem( light );
    if (!lightElem) return;

    const char* name = lightElem->GetName();
    unsigned int hashID = lightElem->GetResSource()->GetHashID();

    // �t���O�����g���C�g�f�[�^���擾
    nw::gfx::res::ResFragmentLight res_light =
        nw::ut::ResDynamicCast<nw::gfx::res::ResFragmentLight>(light->GetResSceneObject());
    if ( !res_light.IsValid() ) return;

    // ���b�N�A�b�v�e�[�u���̊O���Q�Ɛ�𒲍�
    nw::gfx::res::ResReferenceLookupTable res_ref;
    {
        // ���C�e�B���O�t���O���擾
        bool isSpotLight = ( res_light.GetLightKind() == nw::gfx::res::ResLight::KIND_SPOT );
        bool isDistAttenuationEnbaled = 
            nw::ut::CheckFlag(res_light.GetFlags(), nw::gfx::res::ResFragmentLightData::FLAG_DISTANCE_ATTENUATION_ENABLED);

        // DistanceSampler�̒���
        {
            res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceLookupTable>(
                res_light.GetDistanceSampler()
            );
            CheckLookupTableReference(
                isDistAttenuationEnbaled,
                res_ref,
                name, "Dist Lut is not assigned",
                hashID, res_light.ptr(), ErrorLightDistAttenuationSampler
            );
        }

        // AngleSampler�̒���
        {
            bool isSamplerValid = res_light.GetAngleSampler().IsValid();
            if ( isSamplerValid )
            {
                res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceLookupTable>(
                    res_light.GetAngleSampler().GetSampler()
                );
            }
            CheckLookupTableReference(
                isSamplerValid && isSpotLight,
                res_ref,
                name, "Angle Lut is not assigned",
                hashID, res_light.ptr(), ErrorLightAngleSampler
            );
        }
    }
}

void
ErrorCheck::CheckSceneEnvironmentSetting( nw::gfx::SceneEnvironmentSetting* scene )
{
    if (!scene) return;

    SceneEnvironmentSettingElem* sceneElem = GetSceneEnvironmentSettingElem( scene );
    if (!sceneElem) return;

    const char* name = sceneElem->GetName();
    unsigned int hashID = sceneElem->GetResSource()->GetHashID();

    // �V�[�����f�[�^���擾
    nw::gfx::res::ResSceneEnvironmentSetting res_scene =
        nw::ut::ResDynamicCast<nw::gfx::res::ResSceneEnvironmentSetting>(scene->GetResSceneObject());
    if ( !res_scene.IsValid() ) return;

    // �t�H�O�C���f�b�N�X�̒���
    for ( int i = 0; i < res_scene.GetFogsCount(); ++ i )
    {
        nw::gfx::res::ResReferenceSceneObject res_fog = res_scene.GetFogs( i );

        // �t�H�O�C���f�b�N�X�̏���`�F�b�N
        {
            // �G���[�`�F�b�N
            int fogIndex = res_fog.GetIndex();
            int maxFog = nw::gfx::RenderContext::DEFAULT_MAX_FOGS;
            bool error = fogIndex >= maxFog;
            if (error) res_fog.SetIndex( maxFog - 1 ); 

            // �G���[�o��
            char str[128];
            sprintf( str, "Fog index(%d) >= Viewer max fog index(%d)", fogIndex, maxFog - 1 );
            OutputError(
                error,
                vw::system::LogSystem::WARNING,
                name, str,
                hashID, res_fog.ptr(), ErrorSceneSettingFogUpperBoundary
            );
        }

        // �t�H�O�C���f�b�N�X�̉����`�F�b�N
        {
            // �G���[�`�F�b�N
            int fogIndex = res_fog.GetIndex();
            bool error = fogIndex < 0;
            if (error) res_fog.SetIndex( 0 ); 

            // �G���[�o��
            char str[128];
            sprintf( str, "Fog index(%d) < 0", fogIndex );
            OutputError(
                error,
                vw::system::LogSystem::WARNING,
                name, str,
                hashID, res_fog.ptr(), ErrorSceneSettingFogLowerBoundary
            );
        }
    }

    // �J�����C���f�b�N�X�̒���
    for ( int i = 0; i < res_scene.GetCamerasCount(); ++ i )
    {
        nw::gfx::res::ResReferenceSceneObject res_camera = res_scene.GetCameras( i );

        // �J�����C���f�b�N�X�̏���`�F�b�N
        {
            // �G���[�`�F�b�N
            int cameraIndex = res_camera.GetIndex();
            int maxCamera = nw::gfx::RenderContext::DEFAULT_MAX_CAMERAS;
            bool error = cameraIndex >= maxCamera;
            if (error) res_camera.SetIndex( maxCamera - 1 ); 

            // �G���[�o��
            char str[128];
            sprintf( str, "Camera index(%d) >= Viewer max camera index(%d)", cameraIndex, maxCamera - 1 );
            OutputError(
                error,
                vw::system::LogSystem::WARNING,
                name, str,
                hashID, res_camera.ptr(), ErrorSceneSettingCameraUpperBoundary
            );
        }

        // �J�����C���f�b�N�X�̉����`�F�b�N
        {
            // �G���[�`�F�b�N
            int cameraIndex = res_camera.GetIndex();
            bool error = cameraIndex < 0;
            if (error) res_camera.SetIndex( 0 ); 

            // �G���[�o��
            char str[128];
            sprintf( str, "Camera index(%d) < 0", cameraIndex );
            OutputError(
                error,
                vw::system::LogSystem::WARNING,
                name, str,
                hashID, res_camera.ptr(), ErrorSceneSettingCameraLowerBoundary
            );
        }
    }

    // ���C�g�Z�b�g�C���f�b�N�X�̒���
    for ( int i = 0; i < res_scene.GetLightSetsCount(); ++ i )
    {
        nw::gfx::res::ResLightSet res_lightSet = res_scene.GetLightSets( i );

        // ���C�g�Z�b�g�C���f�b�N�X�̏���`�F�b�N
        {
            // �G���[�`�F�b�N
            int lightSetIndex = res_lightSet.GetIndex();
            int maxLightSet = nw::gfx::RenderContext::DEFAULT_MAX_LIGHT_SETS;
            bool error = lightSetIndex >= maxLightSet;
            if (error) res_lightSet.SetIndex( maxLightSet - 1 ); 

            // �G���[�o��
            char str[128];
            sprintf( str, "Light set index(%d) >= Viewer max light set index(%d)", lightSetIndex, maxLightSet - 1 );
            OutputError(
                error,
                vw::system::LogSystem::WARNING,
                name, str,
                hashID, res_lightSet.ptr(), ErrorSceneSettingLightSetUpperBoundary
            );
        }

        // ���C�g�Z�b�g�C���f�b�N�X�̉����`�F�b�N
        {
            // �G���[�`�F�b�N
            int lightSetIndex = res_lightSet.GetIndex();
            bool error = lightSetIndex < 0;
            if (error) res_lightSet.SetIndex( 0 ); 

            // �G���[�o��
            char str[128];
            sprintf( str, "Light set index(%d) < 0", lightSetIndex );
            OutputError(
                error,
                vw::system::LogSystem::WARNING,
                name, str,
                hashID, res_lightSet.ptr(), ErrorSceneSettingLightSetLowerBoundary
            );
        }
    }
}

void ErrorCheck::Error_VertexLight_Disalbe()
{
    vw::system::LogSystem::GetInstance()->AddSystemLog("Viewer cannot handle Vertex Light");
}

void ErrorCheck::Error_VRAM_Alloc()
{
    vw::system::LogSystem::GetInstance()->AddSystemLog("VRAM over");
}
#endif


void
ErrorCheck::CheckModelTextureReference( const nw::gfx::res::ResModel res_model )
{
    // ���f�������擾
    const char* model_name = res_model.GetName();

    for ( int i = 0; i < res_model.GetMaterialsCount(); ++i )
    {
        // �}�e���A�����擾
        nw::gfx::res::ResMaterial res_material = res_model.GetMaterials( i );
        if ( !res_material.IsValid() ) continue;

        // �}�e���A�������擾
        const char* matName = res_material.GetName();

        // �e�N�X�`���Q�Ƃ̃`�F�b�N
        {
            // �C���[�W�e�N�X�`���̊O���Q�Ƃ��`�F�b�N
            for ( int j = 0; j < res_material.GetTextureMappersCount(); ++j )
            {
                // �e�N�X�`���}�b�p�[�f�[�^���擾
                nw::gfx::res::ResPixelBasedTextureMapper res_mapper = res_material.GetTextureMappers( j );
                if ( res_mapper.IsValid() ) 
                {
                    // �e�N�X�`���}�b�p�[�̊O���Q�Ɛ�𒲍�
                    nw::gfx::res::ResReferenceTexture res_ref;
                    {
                        res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceTexture>(
                            res_mapper.GetTexture()
                        );
                    }

                    if( res_ref.IsValid() )
                    {
                      // �e�N�X�`�����Q�Ƃ��Ă���
                      // �e�N�X�`�������擾
                      const char* texture_name = res_ref.GetPath();  // GetName�ł̓_��

                      if( !(res_ref.GetTargetTexture().IsValid()) )  // Dereference�̓_�����Ȃ�
                      {
                        // �e�N�X�`�����Q�Ƃ��Ă��邪�A���̂��Ȃ�
                        GFL_PRINT("Model=%s, Material[%d]=%s, Texture[%d]=%s���������Ă��܂���B\n",
                            model_name, i, matName, j, texture_name );
                      }
                    }
                }
            }

            // �v���V�[�W�����e�N�X�`���̊O���Q�Ƃ��`�F�b�N
            {
                // �e�N�X�`���}�b�p�[�f�[�^���擾
                nw::gfx::res::ResProceduralTextureMapper res_mapper = res_material.GetProceduralTextureMapper();
                if ( res_mapper.IsValid() )
                {
                    // �e�N�X�`���}�b�p�[�̊O���Q�Ɛ�𒲍�
                    nw::gfx::res::ResReferenceTexture res_ref;
                    {
                        res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceTexture>(
                            res_mapper.GetTexture()
                        );
                    }

                    if( res_ref.IsValid() )
                    {
                      // �e�N�X�`�����Q�Ƃ��Ă���
                      // �e�N�X�`�������擾
                      const char* texture_name = res_ref.GetPath();  // GetName�ł̓_��

                      if( !(res_ref.GetTargetTexture().IsValid()) )  // Dereference�̓_�����Ȃ�
                      {
                        // �e�N�X�`�����Q�Ƃ��Ă��邪�A���̂��Ȃ�
                        GFL_PRINT("Model=%s, Material[%d]=%s, ProceduralTexture=%s���������Ă��܂���B\n",
                            model_name, i, matName, texture_name );
                      }
                    }
                }
            }
        }
    }
}

void
ErrorCheck::CheckModelLookupTableReference( const nw::gfx::res::ResModel res_model )
{
    // ���f�������擾
    const char* model_name = res_model.GetName();

    for ( int i = 0; i < res_model.GetMaterialsCount(); ++i )
    {
        // �}�e���A�����擾
        nw::gfx::res::ResMaterial res_material = res_model.GetMaterials( i );
        if ( !res_material.IsValid() ) continue;

        // �}�e���A�������擾
        const char* matName = res_material.GetName();

        // �Q�ƃe�[�u���̊O���Q�Ƃ��`�F�b�N
        {
            // �t���O�����g�V�F�[�_�f�[�^���擾
            nw::gfx::res::ResFragmentShader res_shader = res_material.GetFragmentShader();
            if ( !res_shader.IsValid() ) continue;

            // ���b�N�A�b�v�e�[�u���f�[�^���擾                    
            nw::gfx::res::ResFragmentLightingTable res_table = res_shader.GetFragmentLightingTable();
            if ( !res_table.IsValid() ) continue;
            
            // ���C�e�B���O�f�[�^���擾
            nw::gfx::res::ResFragmentLighting res_lighting = res_shader.GetFragmentLighting();
            if ( !res_lighting.IsValid() ) continue;

            // ���C�e�B���O�t���O���擾
            bool isDis0Enbaled = 
                nw::ut::CheckFlag(res_lighting.GetFlags(), nw::gfx::res::ResFragmentLightingData::FLAG_DISTRIBUTION0_ENABLED);
            bool isDis1Enbaled = 
                nw::ut::CheckFlag(res_lighting.GetFlags(), nw::gfx::res::ResFragmentLightingData::FLAG_DISTRIBUTION1_ENABLED);
            bool isRefEnabled = 
                nw::ut::CheckFlag(res_lighting.GetFlags(), nw::gfx::res::ResFragmentLightingData::FLAG_REFLECTION_ENABLED);
            bool isFresnelEnabled = 
                (res_lighting.GetFresnelConfig() != nw::gfx::res::ResFragmentLighting::CONFIG_NO_FRESNEL);

            // ���b�N�A�b�v�e�[�u���̊O���Q�Ɛ�𒲍�
            bool isSamplerValid;
            nw::gfx::res::ResReferenceLookupTable res_ref;
            {
                // ReflectanceRSampler�̒���
                {
                    isSamplerValid = res_table.GetReflectanceRSampler().IsValid();
                    if ( isSamplerValid )
                    {
                        res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceLookupTable>(
                            res_table.GetReflectanceRSampler().GetSampler()
                        );
                    }
                    CheckLookupTableReference(
                        isSamplerValid && isRefEnabled,
                        res_ref,
                        model_name,
                        i,
                        matName, "RefR Lut is not assigned"
                    );
                }

                // ReflectanceGSampler�̒���
                {
                    isSamplerValid = res_table.GetReflectanceGSampler().IsValid();
                    if ( isSamplerValid )
                    {                
                        res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceLookupTable>(
                            res_table.GetReflectanceGSampler().GetSampler()
                        );
                    }
                    CheckLookupTableReference(
                        isSamplerValid && isRefEnabled,
                        res_ref,
                        model_name,
                        i,
                        matName, "RefG Lut is not assigned"
                    );
                }

                // ReflectanceBSampler�̒���
                {
                    isSamplerValid = res_table.GetReflectanceBSampler().IsValid();
                    if ( isSamplerValid )
                    {
                        res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceLookupTable>(
                            res_table.GetReflectanceBSampler().GetSampler()
                        );
                    }
                    CheckLookupTableReference(
                        isSamplerValid && isRefEnabled,
                        res_ref,
                        model_name,
                        i,
                        matName, "RefB Lut is not assigned"
                    ); 
                }

                // Distribution0Sampler�̒���
                {
                    isSamplerValid = res_table.GetDistribution0Sampler().IsValid();
                    if ( isSamplerValid )
                    {
                        res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceLookupTable>(
                            res_table.GetDistribution0Sampler().GetSampler()
                        );
                    }
                    CheckLookupTableReference(
                        isSamplerValid && isDis0Enbaled,
                        res_ref,
                        model_name,
                        i,
                        matName, "Dis0 Lut is not assigned"
                    );
                }

                // Distribution1Sampler�̒���
                {
                    isSamplerValid = res_table.GetDistribution1Sampler().IsValid();
                    if ( isSamplerValid )
                    {
                        res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceLookupTable>(
                            res_table.GetDistribution1Sampler().GetSampler()
                        );
                    }
                    CheckLookupTableReference(
                        isSamplerValid && isDis1Enbaled,
                        res_ref,
                        model_name,
                        i,
                        matName, "Dis1 Lut is not assigned"
                    );
                }

                // FresnelSampler�̒���
                {
                    isSamplerValid = res_table.GetFresnelSampler().IsValid();
                    if ( isSamplerValid )
                    {
                        res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceLookupTable>(
                            res_table.GetFresnelSampler().GetSampler()
                        );
                    }
                    CheckLookupTableReference(
                        isSamplerValid && isFresnelEnabled,
                        res_ref,
                        model_name,
                        i,
                        matName, "Fresnel Lut is not assigned"
                    );
                }
            }
        }    
    }
}

void ErrorCheck::CheckModelShaderReference( const nw::gfx::res::ResModel res_model )
{
    // ���f�������擾
    const char* model_name = res_model.GetName();

    for ( int i = 0; i < res_model.GetMaterialsCount(); ++i )
    {
        // �}�e���A�����擾
        nw::gfx::res::ResMaterial res_material = res_model.GetMaterials( i );
        if ( !res_material.IsValid() ) continue;

        // �}�e���A�������擾
        const char* matName = res_material.GetName();

        // �V�F�[�_�̊O���Q�Ƃ��`�F�b�N
        {
            // �V�F�[�_�f�[�^���擾
            nw::gfx::res::ResShader res_shader = res_material.GetShader();

            // �V�F�[�_�̊O���Q�Ɛ�𒲍�
            {
                nw::gfx::res::ResReferenceShader res_ref = nw::ut::ResStaticCast<nw::gfx::res::ResReferenceShader>( res_shader );
              
                if( res_ref.IsValid() )
                {
                  // �V�F�[�_���Q�Ƃ��Ă���
                  // �V�F�[�_�����擾
                  const char* shader_name = res_ref.GetPath();  // GetName�ł̓_��
               
                  if( !(res_ref.GetTargetShader().IsValid()) )  // Dereference�̓_��������
                  {
                    // �V�F�[�_���Q�Ƃ��Ă��邪�A���̂��Ȃ�
                    GFL_PRINT("Model=%s, Material[%d]=%s, Shader=%s���������Ă��܂���B\n",
                        model_name, i, matName, shader_name );
                  }
                }
            }
        }    
    }
}

void
ErrorCheck::CheckLookupTableReference( 
    bool isValid,
    nw::gfx::res::ResReferenceLookupTable refLut,
    const char* model_name,
    int material_index,
    const char* matName,
    const char* message
) {

    bool error = false;
    if ( isValid ) {
        if( refLut.IsValid() )
        {
            // ���b�N�A�b�v�e�[�u�����Q�Ƃ��Ă���
            // ���b�N�A�b�v�e�[�u�������擾
            const char* lut_name = refLut.GetPath();  // GetName�ł̓_��

            if( !(refLut.GetTargetLut().IsValid()) )  // Dereference�̓_�����Ȃ�
            {
              // ���b�N�A�b�v�e�[�u�����Q�Ƃ��Ă��邪�A���̂��Ȃ�
              GFL_PRINT("Model=%s, Material[%d]=%s, Lut=%s���������Ă��܂���B%s\n",
                  model_name, material_index, matName, lut_name, message );
            }
        }
    }
}


}   // namespace ec
}   // namespace g3d
}   // namespace grp
}   // namespace gfl

