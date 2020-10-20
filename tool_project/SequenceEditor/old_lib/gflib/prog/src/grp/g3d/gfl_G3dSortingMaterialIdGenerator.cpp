//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSortingMaterialIdGenerator.cpp
 *	@brief  �\�[�g���s�������ʂ���}�e���A��ID�̐������s���N���X
 *	@author	Koji Kawada
 *	@date		2011.08.23
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

/*
NintendoWare/CTR/sources/libraries/gfx/
gfx_SortingMaterialIdGenerator.cpp
Revision: 31290
�����ɍ쐬 
 */


#define GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE (1)  // ���ꂪ1�̂Ƃ��A�e�N�X�`���\�[�g���L���B0�̂Ƃ������B


#include <nw/gfx.h>
#include <grp/g3d/gfl_G3dSortingMaterialIdGenerator.h>

namespace gfl
{
namespace grp
{
namespace g3d
{

//----------------------------------------
#if GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE
namespace internal
{
    enum
    {
        TEXTURE_NUM_MAX  = 4  // 3�ł����Ǝv�����v���V�[�W�����e�N�X�`���̂��Ƃ��l����4�ɂ��Ă���
    };
    struct MatTexInfo
    {
        internal::MaterialKeyValue*  value;
        s32                          mapperCount;
        s32                          textureCount;
        u32                          texturePtr[TEXTURE_NUM_MAX];
        MatTexInfo(void)
        {
            value         = NULL;
            mapperCount   = 0;
            textureCount  = 0;
            for(s32 i=0; i<TEXTURE_NUM_MAX; ++i)
            {
                texturePtr[i] = 0;
            }
        }
    };

    static b32 SortingMaterialIdGenerator_IsSameTex(const internal::MatTexInfo& a, const internal::MatTexInfo& b);
    b32 SortingMaterialIdGenerator_IsSameTex(const internal::MatTexInfo& a, const internal::MatTexInfo& b)
    {
      // MatTexInfo��value�͔�r���Ȃ�
      if( a.mapperCount != b.mapperCount ) return false;
      if( a.textureCount != b.textureCount ) return false;
      for(s32 i=0; i<a.textureCount; ++i)
      {
        if( a.texturePtr[i] != b.texturePtr[i] ) return false;  // �O����l�߂Ă���
      }
      return true;
    }

    static void SortingMaterialIdGenerator_GetMatTexInfo(internal::MaterialKeyValue* p, internal::MatTexInfo* info);
    void SortingMaterialIdGenerator_GetMatTexInfo(internal::MaterialKeyValue* p, internal::MatTexInfo* info)
    {
        *info  =  internal::MatTexInfo();

        info->value = p;
        nw::gfx::res::ResMaterial  resMaterial  =  p->material->GetTextureMapperResMaterial();
        info->mapperCount  =  resMaterial.GetTextureMappersCount();  // �e�N�X�`����1�������g���Ă��Ȃ��Ă�3���Ԃ��Ă���
        for( s32 mapperIndex=0; mapperIndex<info->mapperCount; ++mapperIndex )
        {
            nw::gfx::res::ResPixelBasedTextureMapper  resTextureMapper  =  resMaterial.GetTextureMappers(mapperIndex);
            if( resTextureMapper.IsValid() )  // �e�N�X�`����1�������g���Ă��Ȃ��Ƃ��́A2����3���ڂ͖����ɂȂ��Ă���
            {
                nw::gfx::res::ResTexture  resTexture  =  resTextureMapper.GetTexture().Dereference();
                if( resTexture.IsValid() )  // �e�N�X�`�����\���Ă��Ȃ��Ƃ��͖����ɂȂ��Ă���
                {
                    u32  ptr = reinterpret_cast<u32>(resTexture.ptr());
                    bool exist = false;
                    for( s32 i=0; i<info->textureCount; ++i )
                    {
                        if( info->texturePtr[i] == ptr )
                        {
                            exist = true;
                            break;
                        }
                    }
                    if( !exist )  // �������̂͏Ȃ�
                    {
                        info->texturePtr[info->textureCount]  =  ptr;  // �O����l�߂�
                        ++(info->textureCount);
                    }
                }
            }
        }
    }

    //�@�e�N�X�`���Ń\�[�g
    // beginValue����endValue��key���������̂�n�����ƁB���̊֐���key���������Ƃ�O��ɏ�����Ă���AsubKey�����`�F�b�N���Ă��Ȃ��B
    static void SortingMaterialIdGenerator_SortForTexture(internal::MaterialKeyValue* beginValue, internal::MaterialKeyValue* endValue);
    void SortingMaterialIdGenerator_SortForTexture(internal::MaterialKeyValue* beginValue, internal::MaterialKeyValue* endValue)
    {
        // �����̊֐��ɗ������_�ŁAbeginValue�`endValue�͕���ł���
        // key�ɂ̓V�F�[�_�o�C�i���̃|�C���^
        // subKey�ɂ͎Q�ƃe�[�u���̃n�b�V���l
        // �������Ă���B
        // [ 0] key=0, subKey=0, tex0=aaa, tex1=bbb
        // [ 1] key=0, subKey=0, tex0=aaa, tex1=bbb, tex2=ccc
        // [ 2] key=0, subKey=0, tex0=ddd
        // [ 3] key=0, subKey=1, tex0=aaa
        // [ 4] key=0, subKey=1, tex0=ddd
        // [ 5] key=0, subKey=2
        // [ 6] key=1, subKey=0
        // [ 7] key=1, subKey=0
        // [ 8] key=1, subKey=1
        // [ 9] key=2, subKey=0
        // [10] key=2, subKey=1
        // ����Ȃӂ��ɕ���ł���B
        // ������e�N�X�`���Ń\�[�g�����
        // [ 1] key=0, subKey=0, tex0=aaa, tex1=bbb, tex2=ccc
        // [ 0] key=0, subKey=0, tex0=aaa, tex1=bbb
        // [ 2] key=0, subKey=0, tex0=ddd
        // [ 3] key=0, subKey=1, tex0=aaa
        // [ 4] key=0, subKey=1, tex0=ddd
        // [ 5] key=0, subKey=2
        // [ 6] key=1, subKey=0
        // [ 7] key=1, subKey=0
        // [ 8] key=1, subKey=1
        // [ 9] key=2, subKey=0
        // [10] key=2, subKey=1
        // ����Ȃӂ��ɂȂ�B

        enum Step
        {
            STEP_FIND_BASE,
            STEP_FIND_SAME
        };

        Step                         step              =  STEP_FIND_BASE;
        internal::MaterialKeyValue*  currValue         =  beginValue;
        MatTexInfo                   prevInfo;
        /*u32                          key               =  0;*/
        u32                          subKey            =  0;

        while( currValue != endValue )
        {
            switch( step )
            {
                // ��{�ƂȂ�}�e���A����T��
            case STEP_FIND_BASE:
                {
                    // ������
                    MatTexInfo  maxInfo;  // textureCount���ő�ɂȂ���̂�T��
                    /*key    = currValue->key;*/
                    subKey = currValue->subKey;

                    // �e�N�X�`���̖������������̂�T��
                    for(internal::MaterialKeyValue* p = currValue; p!=endValue; ++p)
                    {
                        if( /*key == p->key &&*/ subKey == p->subKey )  // ���̊֐���key���������Ƃ�O��ɏ�����Ă���AsubKey�����`�F�b�N���Ă��Ȃ��B
                        {
                            // p
                            MatTexInfo  info;
                            SortingMaterialIdGenerator_GetMatTexInfo(p, &info);

                            // 2�ڈȍ~
                            if( maxInfo.value )
                            {
                                if( maxInfo.textureCount < info.textureCount )
                                {
                                    maxInfo = info;
                                }
                            }
                            // �ŏ���1��
                            else
                            {
                                maxInfo = info;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }

                    // ����ւ�
                    internal::MaterialKeyValue  temp  =  *currValue; 
                    GFL_ASSERT_STOP_MSG(maxInfo.value, "maxInfo.value��NULL�ł��B\n");
                    *currValue = *(maxInfo.value);
                    *(maxInfo.value) = temp;

                    prevInfo = maxInfo;
                    prevInfo.value = currValue;
                    
                    ++currValue;
                    step = STEP_FIND_SAME;
                }
                break;

                // ��{�Ɠ����e�N�X�`�������}�e���A����T��
            case STEP_FIND_SAME:
                {
                    // ������
                    s32         maxMatchCount = 0;  // texture�̈�v����
                    MatTexInfo  maxInfo;  // 1 texture��v�������ő�̂��̂�T��  2 textureCount���ő�ɂȂ���̂�T��
                    /*key    = prevInfo.value->key;*/
                    subKey = prevInfo.value->subKey;

                    // 1 texture��v�������ő�̂��̂�T��  2 textureCount���ő�ɂȂ���̂�T��
                    for(internal::MaterialKeyValue* p = currValue; p!=endValue; ++p)
                    {
                        if( /*key == p->key &&*/ subKey == p->subKey )  // ���̊֐���key���������Ƃ�O��ɏ�����Ă���AsubKey�����`�F�b�N���Ă��Ȃ��B
                        {
                            // p
                            MatTexInfo  info;
                            SortingMaterialIdGenerator_GetMatTexInfo(p, &info);

                            // texture��v�������ő�̂��̂�T��
                            s32 matchCount = 0;
                            for(s32 i=0; i<prevInfo.textureCount; ++i)
                            {
                                for(s32 j=0; j<info.textureCount; ++j)
                                {
                                    if(prevInfo.texturePtr[i] == info.texturePtr[j])
                                    {
                                       ++matchCount;
                                       break;
                                    }
                                }
                            }
                            if(
                                   ( matchCount > maxMatchCount )
                                || ( matchCount == maxMatchCount && info.textureCount > maxInfo.textureCount )
                            )
                            {
                                maxMatchCount = matchCount;
                                maxInfo = info;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }

                    // ����ւ�
                    if( maxMatchCount > 0 )
                    {
                        internal::MaterialKeyValue  temp  =  *currValue; 
                        GFL_ASSERT_STOP_MSG(maxInfo.value, "maxInfo.value��NULL�ł��B\n");
                        *currValue = *(maxInfo.value);
                        *(maxInfo.value) = temp;

                        prevInfo = maxInfo;
                        prevInfo.value = currValue;
                    
                        ++currValue;
                    }
                    else
                    {
                        // ��{�Ɠ����e�N�X�`�������}�e���A�������t����Ȃ������̂ŁA��{����T������
                        step = STEP_FIND_BASE;
                    }
                }
                break;
            }
        }
    }
}  // namespace internal
#endif  // GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE

//----------------------------------------
NW_UT_RUNTIME_TYPEINFO_ROOT_DEFINITION(SortingMaterialIdGenerator);
    // NintendoWare/CTR/include/nw/ut/ut_RuntimeTypeInfo.h

//----------------------------------------
nw::gfx::IMaterialIdGenerator*
SortingMaterialIdGenerator::Builder::Create(
    nw::os::IAllocator* allocator
)
{
    NW_NULL_ASSERT(allocator);
    
    void* memory = allocator->Alloc(sizeof(SortingMaterialIdGenerator));
    NW_NULL_ASSERT(memory);
    
    MaterialKeyValueArray materials;
    MaterialKeyValueArray materialsWorkSpace;
    if (m_Description.isFixedSizeMemory)
    {
        materials = MaterialKeyValueArray(m_Description.maxMaterials, allocator);
        materialsWorkSpace = MaterialKeyValueArray(m_Description.maxMaterials, allocator);
    }
    else
    {
        materials = MaterialKeyValueArray(allocator);
        materialsWorkSpace = MaterialKeyValueArray(allocator);
    }

    SortingMaterialIdGenerator* generator =
        GFL_NEW_PLACEMENT(memory) SortingMaterialIdGenerator(
        allocator,
        materials,
        materialsWorkSpace);
    
    return generator;
}


//---------------------------------------------------------------------------
//! @brief        �\�[�g��r�p�֐��I�u�W�F�N�g�ł��B
//---------------------------------------------------------------------------

struct MaterialKeyValueCompare
    : public ::std::binary_function<internal::MaterialKeyValue, internal::MaterialKeyValue, bool>
{
    bool operator() (const internal::MaterialKeyValue& lhs,
        const internal::MaterialKeyValue& rhs)
    {
        return lhs.key < rhs.key;
    }
};

struct MaterialSubKeyValueCompare
    : public ::std::binary_function<internal::MaterialKeyValue, internal::MaterialKeyValue, bool>
{
    bool operator() (const internal::MaterialKeyValue& lhs,
        const internal::MaterialKeyValue& rhs)
    {
        return lhs.subKey < rhs.subKey;
    }
};

struct MaterialUniqueIdCompare
    : public ::std::unary_function<internal::MaterialKeyValue, bool>
{
    bool operator() (const internal::MaterialKeyValue& keyValue, const u32 uniqueId)
    {
        return keyValue.uniqueId < uniqueId;
    }

    bool operator() (const u32 uniqueId, const internal::MaterialKeyValue& keyValue)
    {
        return keyValue.uniqueId > uniqueId;
    }
};

//----------------------------------------
void
SortingMaterialIdGenerator::Accept(nw::gfx::Material* material)
{
    internal::MaterialKeyValue materialKeyValue;
    materialKeyValue.key = reinterpret_cast<u32>(material->GetOriginal().ptr());
    materialKeyValue.material = material;

    bool isPushed = m_Materials.push_back(materialKeyValue);  // PushBackFast�ɒu�������Ă�������������Ȃ��B���̃R�����g�����ǉ��B  2014/05/15_kawada_koji
    GFL_ASSERT_MSG(isPushed, "�}�e���A����%d���z���܂����B\n", m_Materials.capacity());  //@check �}�e���A����ǉ��ł��Ȃ������B
    NW_ASSERT(isPushed);
}

//----------------------------------------
#if 1
// �Ȃ�ׂ��V�F�[�_�̕ύX�������Ȃ��悤�Ƀ\�[�g����
void
SortingMaterialIdGenerator::Generate()
{
    // ResMaterialData ����Ƀ\�[�g���܂��B
    ::std::sort( m_Materials.begin(), m_Materials.end(), 
        gfl::grp::g3d::MaterialKeyValueCompare() );

    u32 lastPtrKey = 0x0;
    u32 uniquePtrId = 0;
    // ResMaterialData ������̃m�[�h�𓝍����� ResBinaryShaderData ����ɃL�[���쐬���܂��B
    MaterialKeyValueArray::iterator materialPtrEnd = m_Materials.end();
    for (MaterialKeyValueArray::iterator iter = m_Materials.begin();
        iter != materialPtrEnd;
        ++iter)
    {
        if (lastPtrKey != (*iter).key)
        {
            ++uniquePtrId;

            nw::gfx::res::ResMaterial resMaterial = (*iter).material->GetOriginal();
            NW_ASSERT(resMaterial.IsValid());
            nw::gfx::res::ResShaderProgramDescription resDescription = (*iter).material->GetDescription();
            NW_ASSERT(resDescription.IsValid());
            u32 key = reinterpret_cast<u32>(resDescription.GetOwnerShaderData());

            internal::MaterialKeyValue materialKeyValue;
            materialKeyValue.uniqueId = uniquePtrId;
            materialKeyValue.key = key;
            materialKeyValue.subKey = resMaterial.GetFragmentLightingTableHash();
            materialKeyValue.material = (*iter).material;
            bool isPushed = m_MaterialsWorkSpace.push_back(materialKeyValue);  // PushBackFast�ɒu�������Ă�������������Ȃ��B���̃R�����g�Ǝ��̍s��ASSERT��ǉ��B  2014/05/15_kawada_koji
            GFL_ASSERT_MSG(isPushed, "�}�e���A����%d���z���܂����B\n", m_MaterialsWorkSpace.capacity());
            lastPtrKey = (*iter).key;
        }
        (*iter).uniqueId = uniquePtrId;
    }

    // ResBinaryShaderData ����Ƀ\�[�g���܂��B
    ::std::sort( m_MaterialsWorkSpace.begin(), m_MaterialsWorkSpace.end(), 
        gfl::grp::g3d::MaterialKeyValueCompare() );

    // ResBinaryShaderData ������ł���}�e���A����̓����� ResFragmentLightingTable �� Hash �l����Ƀ\�[�g���܂��B
    s32 endIndex = m_MaterialsWorkSpace.size() - 1;
    s32 sortStartIndex = 0;
    s32 sortEndIndex   = 0;
    u32 lastShaderKey = 0;
    if( endIndex >= 0 ) lastShaderKey = m_MaterialsWorkSpace[0].key;  // ����if�����^�Ȃ�1�͑��݂���B
    for (int index = 0; index <= endIndex; ++index)
    {
        if (lastShaderKey != m_MaterialsWorkSpace[index].key ||
            index == endIndex)
        {
#if 1 
            if( index == endIndex )
            {
                int diff = 1+ index - sortStartIndex;
                // ����L�[���R�m�[�h�ȏ�A�������ꍇ�Ƀ\�[�g���s���܂��B
                if (diff >= 3)
                {
                    ::std::sort(m_MaterialsWorkSpace.begin() + sortStartIndex,
                        m_MaterialsWorkSpace.end(), 
                        gfl::grp::g3d::MaterialSubKeyValueCompare() );
        #if GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE
                    //�@�e�N�X�`���Ń\�[�g
                    internal::SortingMaterialIdGenerator_SortForTexture(m_MaterialsWorkSpace.begin() + sortStartIndex,
                        m_MaterialsWorkSpace.end());
        #endif  // GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE
                }
                break; 
            }
            else
            {

            sortEndIndex = index;
            int diff = (sortEndIndex - sortStartIndex);
            // ����L�[���R�m�[�h�ȏ�A�������ꍇ�Ƀ\�[�g���s���܂��B
            if (diff >= 3)
            {
                ::std::sort(m_MaterialsWorkSpace.begin() + sortStartIndex,
                    m_MaterialsWorkSpace.begin() + sortEndIndex, 
                    gfl::grp::g3d::MaterialSubKeyValueCompare() );
        #if GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE
                //�@�e�N�X�`���Ń\�[�g
                internal::SortingMaterialIdGenerator_SortForTexture(m_MaterialsWorkSpace.begin() + sortStartIndex,
                    m_MaterialsWorkSpace.begin() + sortEndIndex);
        #endif // GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE
            }
            sortStartIndex = index;
            
            }
#else
//���ꂾ�� (index == endIndex) �̂Ƃ������܂������Ȃ�
            sortEndIndex = index;
            int diff = (sortEndIndex - sortStartIndex);
            // ����L�[���R�m�[�h�ȏ�A�������ꍇ�Ƀ\�[�g���s���܂��B
            if (diff >= 3)
            {
                ::std::sort(m_MaterialsWorkSpace.begin() + sortStartIndex,
                    m_MaterialsWorkSpace.begin() + sortEndIndex, 
                    gfl::grp::g3d::MaterialSubKeyValueCompare() );
        #if GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE
                //�@�e�N�X�`���Ń\�[�g
                internal::SortingMaterialIdGenerator_SortForTexture(m_MaterialsWorkSpace.begin() + sortStartIndex,
                    m_MaterialsWorkSpace.begin() + sortEndIndex);
        #endif  // GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE
            }
            sortStartIndex = index;
#endif
        }

        lastShaderKey = m_MaterialsWorkSpace[index].key;
    }

    // ���������m�[�h��W�J���Ȃ��� MaterialId �� 1 ���珇�ɕt���܂��B
    u32 materialId = MATERIAL_ID_START_NO;//1;
    MaterialKeyValueArray::iterator shaderEnd = m_MaterialsWorkSpace.end();
    for (MaterialKeyValueArray::iterator shaderIter = m_MaterialsWorkSpace.begin();
        shaderIter != shaderEnd;
        ++shaderIter)
    {
        ::std::pair<MaterialKeyValueArray::iterator, MaterialKeyValueArray::iterator> range =
            equal_range(m_Materials.begin(), m_Materials.end(), (*shaderIter).uniqueId,
            MaterialUniqueIdCompare());

        for (MaterialKeyValueArray::iterator ptrIter = range.first;
            ptrIter != range.second;
            ++ptrIter)
        {
            nw::gfx::res::ResMaterial resMaterial = (*ptrIter).material->GetOriginal();
            
            static const u32      MaterialBitMask = (1 << (GFX_MATERIAL_ID_BIT_SIZE-1)) -1;//�P�X�r�b�g���̃}�X�N=�Q�O�r�b�g�ڂ͂O              
            resMaterial.SetMaterialId( materialId & MaterialBitMask );
            ++materialId;
        }
    }

#if GFL_DEBUG
#if 0
    {
        GFL_PRINT("[m_MaterialsWorkSpace]\n");
        PrintMaterialKeyValueArray(m_MaterialsWorkSpace);
        GFL_PRINT("[m_Materials]\n");
        PrintMaterialKeyValueArray(m_Materials);
    }
#endif
#endif  // GFL_DEBUG

    // ������̓}�e���A���̃��X�g��������܂��B
    m_Materials.clear();
    m_MaterialsWorkSpace.clear();
}
#else
// �Ȃ�ׂ��e�N�X�`���̕ύX�������Ȃ��悤�Ƀ\�[�g����
void
SortingMaterialIdGenerator::Generate()
{
    // ResMaterialData ����Ƀ\�[�g���܂��B
    ::std::sort( m_Materials.begin(), m_Materials.end(), 
        gfl::grp::g3d::MaterialKeyValueCompare() );

    u32 lastPtrKey = 0x0;
    u32 uniquePtrId = 0;
    // ResMaterialData ������̃m�[�h�𓝍�����B
    MaterialKeyValueArray::iterator materialPtrEnd = m_Materials.end();
    for (MaterialKeyValueArray::iterator iter = m_Materials.begin();
        iter != materialPtrEnd;
        ++iter)
    {
        if (lastPtrKey != (*iter).key)
        {
            ++uniquePtrId;

            internal::MaterialKeyValue materialKeyValue;
            materialKeyValue.uniqueId = uniquePtrId;
            materialKeyValue.material = (*iter).material;
            // �e�N�X�`���Ń\�[�g�ł���悤�A�����ł̓L�[��ݒ肹���A�S�ăf�t�H���g�l�̂܂܂ɂ��Ă����B
            bool isPushed = m_MaterialsWorkSpace.push_back(materialKeyValue);  // PushBackFast�ɒu�������Ă�������������Ȃ��B���̃R�����g�Ǝ��̍s��ASSERT��ǉ��B  2014/05/15_kawada_koji
            GFL_ASSERT_MSG(isPushed, "�}�e���A����%d���z���܂����B\n", m_MaterialsWorkSpace.capacity());
            lastPtrKey = (*iter).key;
        }
        (*iter).uniqueId = uniquePtrId;
    }

#if GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE
    {
      //�@�e�N�X�`���Ń\�[�g
      internal::SortingMaterialIdGenerator_SortForTexture(m_MaterialsWorkSpace.begin(), m_MaterialsWorkSpace.end());
    }
#endif  // GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE

    // �V�F�[�_�Ń\�[�g���Q�ƃe�[�u���Ń\�[�g
    {
        s32 endIndex = m_MaterialsWorkSpace.size() - 1;
        
        // �����l�������Ă���
        s32 targetStartIndex = 0;
        s32 targetEndIndex   = 0;  // targetStartIndex<= <=targetEndIndex
        internal::MatTexInfo targetStartInfo;
        if( endIndex >= 0 )  // ����if�����^�Ȃ�1�͑��݂���B
        {
          internal::SortingMaterialIdGenerator_GetMatTexInfo(&(m_MaterialsWorkSpace[0]), &targetStartInfo);
        }
        
        for (int index = 0; index <= endIndex; ++index)
        {
          // �V�F�[�_�ƎQ�ƃe�[�u�����L�[�ɂ���
          {
            nw::gfx::res::ResMaterial resMaterial = m_MaterialsWorkSpace[index].material->GetOriginal();
            NW_ASSERT(resMaterial.IsValid());
            nw::gfx::res::ResShaderProgramDescription resDescription = m_MaterialsWorkSpace[index].material->GetDescription();
            NW_ASSERT(resDescription.IsValid());
            u32 key = reinterpret_cast<u32>(resDescription.GetOwnerShaderData());

            m_MaterialsWorkSpace[index].key = key;
            m_MaterialsWorkSpace[index].subKey = resMaterial.GetFragmentLightingTableHash();
          }

          internal::MatTexInfo targetCurrInfo;
          internal::SortingMaterialIdGenerator_GetMatTexInfo(&(m_MaterialsWorkSpace[index]), &targetCurrInfo);
          if(    internal::SortingMaterialIdGenerator_IsSameTex(targetStartInfo, targetCurrInfo)
              && index != endIndex )
          {
            targetEndIndex = index;
          }
          else
          {
            // �V�F�[�_�Ń\�[�g
            {
              if( index == endIndex )
              {
                targetEndIndex = index;
              }

              ::std::sort( m_MaterialsWorkSpace.begin()+targetStartIndex, m_MaterialsWorkSpace.begin()+targetEndIndex, 
                  gfl::grp::g3d::MaterialKeyValueCompare() );

              // �Q�ƃe�[�u���Ń\�[�g
              u32 sortStartIndex = targetStartIndex;
              u32 sortEndIndex   = targetStartIndex;
              u32 lastShaderKey  = m_MaterialsWorkSpace[sortStartIndex].key;  // �����ɗ����Ƃ������Ƃ͕K��1�͑��݂���
              for(s32 subIndex=targetStartIndex; subIndex<=targetEndIndex; ++subIndex)
              {
                if(    lastShaderKey != m_MaterialsWorkSpace[subIndex].key
                    || subIndex == targetEndIndex )
                {
                  if( subIndex == targetEndIndex )
                  {
                    sortEndIndex = subIndex;
                  }

                  // ����L�[���R�m�[�h�ȏ�A�������ꍇ�����Ăяo���Ȃ��悤�ɂ����ق����������H
                  ::std::sort(m_MaterialsWorkSpace.begin()+sortStartIndex, m_MaterialsWorkSpace.begin()+sortEndIndex,
                      gfl::grp::g3d::MaterialSubKeyValueCompare() );
                 
                  // ����
                  sortStartIndex = subIndex;
                  sortEndIndex   = subIndex;
                  lastShaderKey  = m_MaterialsWorkSpace[sortStartIndex].key;
                }
                else
                {
                  sortEndIndex = subIndex;
                }
              }
            }
 
            // ����
            targetStartIndex = index;
            targetEndIndex   = index;
            targetStartInfo  = targetCurrInfo;
          }
        }
    }

    // ���������m�[�h��W�J���Ȃ��� MaterialId �� 1 ���珇�ɕt���܂��B
    u32 materialId = MATERIAL_ID_START_NO;//1;
    MaterialKeyValueArray::iterator shaderEnd = m_MaterialsWorkSpace.end();
    for (MaterialKeyValueArray::iterator shaderIter = m_MaterialsWorkSpace.begin();
        shaderIter != shaderEnd;
        ++shaderIter)
    {
        ::std::pair<MaterialKeyValueArray::iterator, MaterialKeyValueArray::iterator> range =
            equal_range(m_Materials.begin(), m_Materials.end(), (*shaderIter).uniqueId,
            MaterialUniqueIdCompare());

        for (MaterialKeyValueArray::iterator ptrIter = range.first;
            ptrIter != range.second;
            ++ptrIter)
        {
            nw::gfx::res::ResMaterial resMaterial = (*ptrIter).material->GetOriginal();
            resMaterial.SetMaterialId( materialId );
            ++materialId;
        }
    }

#if GFL_DEBUG
#if 0
    {
        GFL_PRINT("[m_MaterialsWorkSpace]\n");
        PrintMaterialKeyValueArray(m_MaterialsWorkSpace);
        GFL_PRINT("[m_Materials]\n");
        PrintMaterialKeyValueArray(m_Materials);
    }
#endif
#endif  // GFL_DEBUG

    // ������̓}�e���A���̃��X�g��������܂��B
    m_Materials.clear();
    m_MaterialsWorkSpace.clear();
}
#endif

#if GFL_DEBUG
// �m�F�o��
void SortingMaterialIdGenerator::PrintMaterialKeyValueArray(MaterialKeyValueArray& materialKeyValueArray)
{
    {
        // �e�N�X�`���m�F
        int no = 0;
        MaterialKeyValueArray::iterator materialEnd = materialKeyValueArray.end();
        for(MaterialKeyValueArray::iterator materialIter = materialKeyValueArray.begin();
            materialIter != materialEnd;
            ++materialIter)
        { 
            internal::MaterialKeyValue* p = materialIter;

            //nw::gfx::res::ResMaterial  resMaterial  =  p->material->GetTextureMapperResMaterial();  ���ꂾ��MaterialId���擾�ł���0���Ԃ��Ă��邱�Ƃ�����
            nw::gfx::res::ResMaterial  resMaterial  =  p->material->GetOriginal();
            s32  count  =  resMaterial.GetTextureMappersCount();  // �e�N�X�`����1�������g���Ă��Ȃ��Ă�3���Ԃ��Ă���
  
            {
                const char* name = resMaterial.GetName();
                if( name )
                {
                    GFL_PRINT("test[%d] Name=%s, MaterialId=%d, uniqueId=%d, key=0x%x, subKey=%d, count=%d\n", no, name, resMaterial.GetMaterialId(),
                        p->uniqueId, p->key, p->subKey, count);
                }
                else
                {
                    GFL_PRINT("test[%d] MaterialId=%d, uniqueId=%d, key=0x%x, subKey=%d, count=%d\n", no, resMaterial.GetMaterialId(),
                        p->uniqueId, p->key, p->subKey, count);
                }
            } 
  
            for( s32 c=0; c<count; ++c )
            {
                nw::gfx::res::ResPixelBasedTextureMapper  resTextureMapper  =  resMaterial.GetTextureMappers(c);
                if( resTextureMapper.IsValid() )  // �e�N�X�`����1�������g���Ă��Ȃ��Ƃ��́A2����3���ڂ͖����ɂȂ��Ă���
                {
                    nw::gfx::res::ResTexture  resTexture  =  resTextureMapper.GetTexture().Dereference();
                    if( resTexture.IsValid() )  // �e�N�X�`�����\���Ă��Ȃ��Ƃ��͖����ɂȂ��Ă���
                    {
                        u32  texturePtr  =  reinterpret_cast<u32>(resTexture.ptr());
 
                        {
                            const char* name = resTexture.GetName();
                            if( name )
                            {
                                GFL_PRINT("    tex[%d] Name=%s, ptr=0x%x, \n", c, name, texturePtr );
                            }
                            else
                            {
                                GFL_PRINT("    tex[%d] ptr=0x%x, \n", c, texturePtr );
                            }
                        }
                    }
                }
            }
            ++no;
        }
    }
}
#endif  // GFL_DEBUG

} // namespace g3d
} // namespace grp
} // namespace gfl




#if 0
// �ȉ��e�X�g�R�[�h�B�����̖��ɗ���������Ȃ��̂Ŏc���Ă����B

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
    static void SortingMaterialIdGenerator_SortForTexture(internal::MaterialKeyValue* beginValue, internal::MaterialKeyValue* endValue);
    void SortingMaterialIdGenerator_SortForTexture(internal::MaterialKeyValue* beginValue, internal::MaterialKeyValue* endValue)
    {
#if 0
        // �f�[�^�o�̓e�X�g
        int no = 0;
        for(internal::MaterialKeyValue* p = beginValue; p!=endValue; ++p)
        {
          nw::gfx::res::ResMaterial resMaterial = p->material->GetOriginal();
          const char* name = resMaterial.GetName();
          if( name )
          {
              GFL_PRINT("test[%d] Name=%s, MaterialId=%d, uniqueId=%d, key=0x%x, subKey=%d\n", no, name, resMaterial.GetMaterialId(),
                  p->uniqueId, p->key, p->subKey);
          }
          else
          {
              GFL_PRINT("test[%d] MaterialId=%d, uniqueId=%d, key=0x%x, subKey=%d\n", no, resMaterial.GetMaterialId(),
                  p->uniqueId, p->key, p->subKey);
          }
          ++no;
        }
  
        // �f�[�^����ւ��e�X�g
        for(int i=0; i<no/2; ++i)
        {
          internal::MaterialKeyValue* p_front = &beginValue[i];
          internal::MaterialKeyValue* p_back  = &beginValue[no-i-1];
          internal::MaterialKeyValue temp = *p_front;
          *p_front = *p_back;
          *p_back = temp;
        }
#endif

#if 0
        // �e�N�X�`���m�F�e�X�g
        int no = 0;
        for(internal::MaterialKeyValue* p = beginValue; p!=endValue; ++p)
        {
            nw::gfx::res::ResMaterial  resMaterial  =  p->material->GetTextureMapperResMaterial();
            s32  count  =  resMaterial.GetTextureMappersCount();  // �e�N�X�`����1�������g���Ă��Ȃ��Ă�3���Ԃ��Ă���
  
            {
                const char* name = resMaterial.GetName();
                if( name )
                {
                    GFL_PRINT("test[%d] Name=%s, MaterialId=%d, uniqueId=%d, key=0x%x, subKey=%d, count=%d\n", no, name, resMaterial.GetMaterialId(),
                        p->uniqueId, p->key, p->subKey, count);
                }
                else
                {
                    GFL_PRINT("test[%d] MaterialId=%d, uniqueId=%d, key=0x%x, subKey=%d, count=%d\n", no, resMaterial.GetMaterialId(),
                        p->uniqueId, p->key, p->subKey, count);
                }
            } 
  
            for( s32 c=0; c<count; ++c )
            {
                nw::gfx::res::ResPixelBasedTextureMapper  resTextureMapper  =  resMaterial.GetTextureMappers(c);
                if( resTextureMapper.IsValid() )  // �e�N�X�`����1�������g���Ă��Ȃ��Ƃ��́A2����3���ڂ͖����ɂȂ��Ă���
                {
                    nw::gfx::res::ResTexture  resTexture  =  resTextureMapper.GetTexture().Dereference();
                    if( resTexture.IsValid() )  // �e�N�X�`�����\���Ă��Ȃ��Ƃ��͖����ɂȂ��Ă���
                    {
                        u32  texturePtr  =  reinterpret_cast<u32>(resTexture.ptr());
 
                        {
                            const char* name = resTexture.GetName();
                            if( name )
                            {
                                GFL_PRINT("    tex[%d] Name=%s, ptr=0x%x, \n", c, name, texturePtr );
                            }
                            else
                            {
                                GFL_PRINT("    tex[%d] ptr=0x%x, \n", c, texturePtr );
                            }
                        }
                    }
                }
            }
            ++no;
        }
#endif
    }

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
#if 0
    {
        int no = 0;
        MaterialKeyValueArray::iterator materialEnd = m_Materials.end();
        for(MaterialKeyValueArray::iterator materialIter = m_Materials.begin();
            materialIter != materialEnd;
            ++materialIter)
        {
            nw::gfx::res::ResMaterial resMaterial = (*materialIter).material->GetOriginal();
            const char* name = resMaterial.GetName();
            if( name )
            {
                GFL_PRINT("[%d] Name=%s, MaterialId=%d\n", no, name, resMaterial.GetMaterialId());
            }
            else
            {
                GFL_PRINT("[%d] MaterialId=%d\n", no, resMaterial.GetMaterialId());
            }
            ++no;
        }
    }
#elif 1
    {
        // �e�N�X�`���m�F�e�X�g
        int no = 0;
        MaterialKeyValueArray::iterator materialEnd = m_Materials.end();
        for(MaterialKeyValueArray::iterator materialIter = m_Materials.begin();
            materialIter != materialEnd;
            ++materialIter)
        { 
            internal::MaterialKeyValue* p = materialIter;

            nw::gfx::res::ResMaterial  resMaterial  =  p->material->GetTextureMapperResMaterial();
            s32  count  =  resMaterial.GetTextureMappersCount();  // �e�N�X�`����1�������g���Ă��Ȃ��Ă�3���Ԃ��Ă���
  
            {
                const char* name = resMaterial.GetName();
                if( name )
                {
                    GFL_PRINT("test[%d] Name=%s, MaterialId=%d, uniqueId=%d, key=0x%x, subKey=%d, count=%d\n", no, name, resMaterial.GetMaterialId(),
                        p->uniqueId, p->key, p->subKey, count);
                }
                else
                {
                    GFL_PRINT("test[%d] MaterialId=%d, uniqueId=%d, key=0x%x, subKey=%d, count=%d\n", no, resMaterial.GetMaterialId(),
                        p->uniqueId, p->key, p->subKey, count);
                }
            } 
  
            for( s32 c=0; c<count; ++c )
            {
                nw::gfx::res::ResPixelBasedTextureMapper  resTextureMapper  =  resMaterial.GetTextureMappers(c);
                if( resTextureMapper.IsValid() )  // �e�N�X�`����1�������g���Ă��Ȃ��Ƃ��́A2����3���ڂ͖����ɂȂ��Ă���
                {
                    nw::gfx::res::ResTexture  resTexture  =  resTextureMapper.GetTexture().Dereference();
                    if( resTexture.IsValid() )  // �e�N�X�`�����\���Ă��Ȃ��Ƃ��͖����ɂȂ��Ă���
                    {
                        u32  texturePtr  =  reinterpret_cast<u32>(resTexture.ptr());
 
                        {
                            const char* name = resTexture.GetName();
                            if( name )
                            {
                                GFL_PRINT("    tex[%d] Name=%s, ptr=0x%x, \n", c, name, texturePtr );
                            }
                            else
                            {
                                GFL_PRINT("    tex[%d] ptr=0x%x, \n", c, texturePtr );
                            }
                        }
                    }
                }
            }
            ++no;
        }
    }
#endif

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
#endif

