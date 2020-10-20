//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSortingMaterialIdGenerator.cpp
 *	@brief  ソートを行った結果からマテリアルIDの生成を行うクラス
 *	@author	Koji Kawada
 *	@date		2011.08.23
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

/*
NintendoWare/CTR/sources/libraries/gfx/
gfx_SortingMaterialIdGenerator.cpp
Revision: 31290
を元に作成 
 */


#define GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE (1)  // これが1のとき、テクスチャソートが有効。0のとき無効。


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
        TEXTURE_NUM_MAX  = 4  // 3でいいと思うがプロシージャルテクスチャのことも考えて4にしておく
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
      // MatTexInfoのvalueは比較しない
      if( a.mapperCount != b.mapperCount ) return false;
      if( a.textureCount != b.textureCount ) return false;
      for(s32 i=0; i<a.textureCount; ++i)
      {
        if( a.texturePtr[i] != b.texturePtr[i] ) return false;  // 前から詰めてある
      }
      return true;
    }

    static void SortingMaterialIdGenerator_GetMatTexInfo(internal::MaterialKeyValue* p, internal::MatTexInfo* info);
    void SortingMaterialIdGenerator_GetMatTexInfo(internal::MaterialKeyValue* p, internal::MatTexInfo* info)
    {
        *info  =  internal::MatTexInfo();

        info->value = p;
        nw::gfx::res::ResMaterial  resMaterial  =  p->material->GetTextureMapperResMaterial();
        info->mapperCount  =  resMaterial.GetTextureMappersCount();  // テクスチャを1枚しか使っていなくても3が返ってくる
        for( s32 mapperIndex=0; mapperIndex<info->mapperCount; ++mapperIndex )
        {
            nw::gfx::res::ResPixelBasedTextureMapper  resTextureMapper  =  resMaterial.GetTextureMappers(mapperIndex);
            if( resTextureMapper.IsValid() )  // テクスチャを1枚しか使っていないときは、2枚目3枚目は無効になっている
            {
                nw::gfx::res::ResTexture  resTexture  =  resTextureMapper.GetTexture().Dereference();
                if( resTexture.IsValid() )  // テクスチャが貼られていないときは無効になっている
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
                    if( !exist )  // 同じものは省く
                    {
                        info->texturePtr[info->textureCount]  =  ptr;  // 前から詰める
                        ++(info->textureCount);
                    }
                }
            }
        }
    }

    //　テクスチャでソート
    // beginValueからendValueはkeyが同じものを渡すこと。この関数はkeyが同じことを前提に書かれており、subKeyしかチェックしていない。
    static void SortingMaterialIdGenerator_SortForTexture(internal::MaterialKeyValue* beginValue, internal::MaterialKeyValue* endValue);
    void SortingMaterialIdGenerator_SortForTexture(internal::MaterialKeyValue* beginValue, internal::MaterialKeyValue* endValue)
    {
        // ここの関数に来た時点で、beginValue～endValueは並んでおり
        // keyにはシェーダバイナリのポインタ
        // subKeyには参照テーブルのハッシュ値
        // が入っている。
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
        // こんなふうに並んでいる。
        // これをテクスチャでソートすると
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
        // こんなふうになる。

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
                // 基本となるマテリアルを探す
            case STEP_FIND_BASE:
                {
                    // 初期化
                    MatTexInfo  maxInfo;  // textureCountが最大になるものを探す
                    /*key    = currValue->key;*/
                    subKey = currValue->subKey;

                    // テクスチャの枚数が多いものを探す
                    for(internal::MaterialKeyValue* p = currValue; p!=endValue; ++p)
                    {
                        if( /*key == p->key &&*/ subKey == p->subKey )  // この関数はkeyが同じことを前提に書かれており、subKeyしかチェックしていない。
                        {
                            // p
                            MatTexInfo  info;
                            SortingMaterialIdGenerator_GetMatTexInfo(p, &info);

                            // 2個目以降
                            if( maxInfo.value )
                            {
                                if( maxInfo.textureCount < info.textureCount )
                                {
                                    maxInfo = info;
                                }
                            }
                            // 最初の1個
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

                    // 入れ替え
                    internal::MaterialKeyValue  temp  =  *currValue; 
                    GFL_ASSERT_STOP_MSG(maxInfo.value, "maxInfo.valueがNULLです。\n");
                    *currValue = *(maxInfo.value);
                    *(maxInfo.value) = temp;

                    prevInfo = maxInfo;
                    prevInfo.value = currValue;
                    
                    ++currValue;
                    step = STEP_FIND_SAME;
                }
                break;

                // 基本と同じテクスチャを持つマテリアルを探す
            case STEP_FIND_SAME:
                {
                    // 初期化
                    s32         maxMatchCount = 0;  // textureの一致枚数
                    MatTexInfo  maxInfo;  // 1 texture一致枚数が最大のものを探す  2 textureCountが最大になるものを探す
                    /*key    = prevInfo.value->key;*/
                    subKey = prevInfo.value->subKey;

                    // 1 texture一致枚数が最大のものを探す  2 textureCountが最大になるものを探す
                    for(internal::MaterialKeyValue* p = currValue; p!=endValue; ++p)
                    {
                        if( /*key == p->key &&*/ subKey == p->subKey )  // この関数はkeyが同じことを前提に書かれており、subKeyしかチェックしていない。
                        {
                            // p
                            MatTexInfo  info;
                            SortingMaterialIdGenerator_GetMatTexInfo(p, &info);

                            // texture一致枚数が最大のものを探す
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

                    // 入れ替え
                    if( maxMatchCount > 0 )
                    {
                        internal::MaterialKeyValue  temp  =  *currValue; 
                        GFL_ASSERT_STOP_MSG(maxInfo.value, "maxInfo.valueがNULLです。\n");
                        *currValue = *(maxInfo.value);
                        *(maxInfo.value) = temp;

                        prevInfo = maxInfo;
                        prevInfo.value = currValue;
                    
                        ++currValue;
                    }
                    else
                    {
                        // 基本と同じテクスチャを持つマテリアルが見付からなかったので、基本から探し直し
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
//! @brief        ソート比較用関数オブジェクトです。
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

    bool isPushed = m_Materials.push_back(materialKeyValue);  // PushBackFastに置き換えてもいいかもしれない。このコメントだけ追加。  2014/05/15_kawada_koji
    GFL_ASSERT_MSG(isPushed, "マテリアル数%dを越えました。\n", m_Materials.capacity());  //@check マテリアルを追加できないだけ。
    NW_ASSERT(isPushed);
}

//----------------------------------------
#if 1
// なるべくシェーダの変更がおきないようにソートする
void
SortingMaterialIdGenerator::Generate()
{
    // ResMaterialData を基にソートします。
    ::std::sort( m_Materials.begin(), m_Materials.end(), 
        gfl::grp::g3d::MaterialKeyValueCompare() );

    u32 lastPtrKey = 0x0;
    u32 uniquePtrId = 0;
    // ResMaterialData が同一のノードを統合しつつ ResBinaryShaderData を基にキーを作成します。
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
            bool isPushed = m_MaterialsWorkSpace.push_back(materialKeyValue);  // PushBackFastに置き換えてもいいかもしれない。このコメントと次の行のASSERTを追加。  2014/05/15_kawada_koji
            GFL_ASSERT_MSG(isPushed, "マテリアル数%dを越えました。\n", m_MaterialsWorkSpace.capacity());
            lastPtrKey = (*iter).key;
        }
        (*iter).uniqueId = uniquePtrId;
    }

    // ResBinaryShaderData を基にソートします。
    ::std::sort( m_MaterialsWorkSpace.begin(), m_MaterialsWorkSpace.end(), 
        gfl::grp::g3d::MaterialKeyValueCompare() );

    // ResBinaryShaderData が同一であるマテリアル列の内部を ResFragmentLightingTable の Hash 値を基にソートします。
    s32 endIndex = m_MaterialsWorkSpace.size() - 1;
    s32 sortStartIndex = 0;
    s32 sortEndIndex   = 0;
    u32 lastShaderKey = 0;
    if( endIndex >= 0 ) lastShaderKey = m_MaterialsWorkSpace[0].key;  // このif文が真なら1個は存在する。
    for (int index = 0; index <= endIndex; ++index)
    {
        if (lastShaderKey != m_MaterialsWorkSpace[index].key ||
            index == endIndex)
        {
#if 1 
            if( index == endIndex )
            {
                int diff = 1+ index - sortStartIndex;
                // 同一キーが３ノード以上連続した場合にソートを行います。
                if (diff >= 3)
                {
                    ::std::sort(m_MaterialsWorkSpace.begin() + sortStartIndex,
                        m_MaterialsWorkSpace.end(), 
                        gfl::grp::g3d::MaterialSubKeyValueCompare() );
        #if GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE
                    //　テクスチャでソート
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
            // 同一キーが３ノード以上連続した場合にソートを行います。
            if (diff >= 3)
            {
                ::std::sort(m_MaterialsWorkSpace.begin() + sortStartIndex,
                    m_MaterialsWorkSpace.begin() + sortEndIndex, 
                    gfl::grp::g3d::MaterialSubKeyValueCompare() );
        #if GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE
                //　テクスチャでソート
                internal::SortingMaterialIdGenerator_SortForTexture(m_MaterialsWorkSpace.begin() + sortStartIndex,
                    m_MaterialsWorkSpace.begin() + sortEndIndex);
        #endif // GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE
            }
            sortStartIndex = index;
            
            }
#else
//これだと (index == endIndex) のときがうまくいかない
            sortEndIndex = index;
            int diff = (sortEndIndex - sortStartIndex);
            // 同一キーが３ノード以上連続した場合にソートを行います。
            if (diff >= 3)
            {
                ::std::sort(m_MaterialsWorkSpace.begin() + sortStartIndex,
                    m_MaterialsWorkSpace.begin() + sortEndIndex, 
                    gfl::grp::g3d::MaterialSubKeyValueCompare() );
        #if GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE
                //　テクスチャでソート
                internal::SortingMaterialIdGenerator_SortForTexture(m_MaterialsWorkSpace.begin() + sortStartIndex,
                    m_MaterialsWorkSpace.begin() + sortEndIndex);
        #endif  // GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE
            }
            sortStartIndex = index;
#endif
        }

        lastShaderKey = m_MaterialsWorkSpace[index].key;
    }

    // 統合したノードを展開しながら MaterialId を 1 から順に付けます。
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
            
            static const u32      MaterialBitMask = (1 << (GFX_MATERIAL_ID_BIT_SIZE-1)) -1;//１９ビット分のマスク=２０ビット目は０              
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

    // 生成後はマテリアルのリストを解放します。
    m_Materials.clear();
    m_MaterialsWorkSpace.clear();
}
#else
// なるべくテクスチャの変更がおきないようにソートする
void
SortingMaterialIdGenerator::Generate()
{
    // ResMaterialData を基にソートします。
    ::std::sort( m_Materials.begin(), m_Materials.end(), 
        gfl::grp::g3d::MaterialKeyValueCompare() );

    u32 lastPtrKey = 0x0;
    u32 uniquePtrId = 0;
    // ResMaterialData が同一のノードを統合する。
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
            // テクスチャでソートできるよう、ここではキーを設定せず、全てデフォルト値のままにしておく。
            bool isPushed = m_MaterialsWorkSpace.push_back(materialKeyValue);  // PushBackFastに置き換えてもいいかもしれない。このコメントと次の行のASSERTを追加。  2014/05/15_kawada_koji
            GFL_ASSERT_MSG(isPushed, "マテリアル数%dを越えました。\n", m_MaterialsWorkSpace.capacity());
            lastPtrKey = (*iter).key;
        }
        (*iter).uniqueId = uniquePtrId;
    }

#if GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE
    {
      //　テクスチャでソート
      internal::SortingMaterialIdGenerator_SortForTexture(m_MaterialsWorkSpace.begin(), m_MaterialsWorkSpace.end());
    }
#endif  // GFL_G3D_SORTING_MATERIAL_ID_GENERATOR_TEXTURE_SORT_ENABLE

    // シェーダでソート→参照テーブルでソート
    {
        s32 endIndex = m_MaterialsWorkSpace.size() - 1;
        
        // 初期値をつくっておく
        s32 targetStartIndex = 0;
        s32 targetEndIndex   = 0;  // targetStartIndex<= <=targetEndIndex
        internal::MatTexInfo targetStartInfo;
        if( endIndex >= 0 )  // このif文が真なら1個は存在する。
        {
          internal::SortingMaterialIdGenerator_GetMatTexInfo(&(m_MaterialsWorkSpace[0]), &targetStartInfo);
        }
        
        for (int index = 0; index <= endIndex; ++index)
        {
          // シェーダと参照テーブルをキーにする
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
            // シェーダでソート
            {
              if( index == endIndex )
              {
                targetEndIndex = index;
              }

              ::std::sort( m_MaterialsWorkSpace.begin()+targetStartIndex, m_MaterialsWorkSpace.begin()+targetEndIndex, 
                  gfl::grp::g3d::MaterialKeyValueCompare() );

              // 参照テーブルでソート
              u32 sortStartIndex = targetStartIndex;
              u32 sortEndIndex   = targetStartIndex;
              u32 lastShaderKey  = m_MaterialsWorkSpace[sortStartIndex].key;  // ここに来たということは必ず1個は存在する
              for(s32 subIndex=targetStartIndex; subIndex<=targetEndIndex; ++subIndex)
              {
                if(    lastShaderKey != m_MaterialsWorkSpace[subIndex].key
                    || subIndex == targetEndIndex )
                {
                  if( subIndex == targetEndIndex )
                  {
                    sortEndIndex = subIndex;
                  }

                  // 同一キーが３ノード以上連続した場合しか呼び出さないようにしたほうがいいか？
                  ::std::sort(m_MaterialsWorkSpace.begin()+sortStartIndex, m_MaterialsWorkSpace.begin()+sortEndIndex,
                      gfl::grp::g3d::MaterialSubKeyValueCompare() );
                 
                  // 次へ
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
 
            // 次へ
            targetStartIndex = index;
            targetEndIndex   = index;
            targetStartInfo  = targetCurrInfo;
          }
        }
    }

    // 統合したノードを展開しながら MaterialId を 1 から順に付けます。
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

    // 生成後はマテリアルのリストを解放します。
    m_Materials.clear();
    m_MaterialsWorkSpace.clear();
}
#endif

#if GFL_DEBUG
// 確認出力
void SortingMaterialIdGenerator::PrintMaterialKeyValueArray(MaterialKeyValueArray& materialKeyValueArray)
{
    {
        // テクスチャ確認
        int no = 0;
        MaterialKeyValueArray::iterator materialEnd = materialKeyValueArray.end();
        for(MaterialKeyValueArray::iterator materialIter = materialKeyValueArray.begin();
            materialIter != materialEnd;
            ++materialIter)
        { 
            internal::MaterialKeyValue* p = materialIter;

            //nw::gfx::res::ResMaterial  resMaterial  =  p->material->GetTextureMapperResMaterial();  これだとMaterialIdが取得できず0が返ってくることがある
            nw::gfx::res::ResMaterial  resMaterial  =  p->material->GetOriginal();
            s32  count  =  resMaterial.GetTextureMappersCount();  // テクスチャを1枚しか使っていなくても3が返ってくる
  
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
                if( resTextureMapper.IsValid() )  // テクスチャを1枚しか使っていないときは、2枚目3枚目は無効になっている
                {
                    nw::gfx::res::ResTexture  resTexture  =  resTextureMapper.GetTexture().Dereference();
                    if( resTexture.IsValid() )  // テクスチャが貼られていないときは無効になっている
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
// 以下テストコード。何かの役に立つかもしれないので残しておく。

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
    static void SortingMaterialIdGenerator_SortForTexture(internal::MaterialKeyValue* beginValue, internal::MaterialKeyValue* endValue);
    void SortingMaterialIdGenerator_SortForTexture(internal::MaterialKeyValue* beginValue, internal::MaterialKeyValue* endValue)
    {
#if 0
        // データ出力テスト
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
  
        // データ入れ替えテスト
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
        // テクスチャ確認テスト
        int no = 0;
        for(internal::MaterialKeyValue* p = beginValue; p!=endValue; ++p)
        {
            nw::gfx::res::ResMaterial  resMaterial  =  p->material->GetTextureMapperResMaterial();
            s32  count  =  resMaterial.GetTextureMappersCount();  // テクスチャを1枚しか使っていなくても3が返ってくる
  
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
                if( resTextureMapper.IsValid() )  // テクスチャを1枚しか使っていないときは、2枚目3枚目は無効になっている
                {
                    nw::gfx::res::ResTexture  resTexture  =  resTextureMapper.GetTexture().Dereference();
                    if( resTexture.IsValid() )  // テクスチャが貼られていないときは無効になっている
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
        // テクスチャ確認テスト
        int no = 0;
        MaterialKeyValueArray::iterator materialEnd = m_Materials.end();
        for(MaterialKeyValueArray::iterator materialIter = m_Materials.begin();
            materialIter != materialEnd;
            ++materialIter)
        { 
            internal::MaterialKeyValue* p = materialIter;

            nw::gfx::res::ResMaterial  resMaterial  =  p->material->GetTextureMapperResMaterial();
            s32  count  =  resMaterial.GetTextureMappersCount();  // テクスチャを1枚しか使っていなくても3が返ってくる
  
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
                if( resTextureMapper.IsValid() )  // テクスチャを1枚しか使っていないときは、2枚目3枚目は無効になっている
                {
                    nw::gfx::res::ResTexture  resTexture  =  resTextureMapper.GetTexture().Dereference();
                    if( resTexture.IsValid() )  // テクスチャが貼られていないときは無効になっている
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

