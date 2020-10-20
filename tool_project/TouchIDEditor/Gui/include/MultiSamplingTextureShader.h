#ifndef _GF_MAYA_PLUGINS_SHADER_MultiSamplingTextureSHADER_H_
#define _GF_MAYA_PLUGINS_SHADER_MultiSamplingTextureSHADER_H_

#include <qt/Shader/ShaderObject.h>
#include <qt/Shader/CombinerAttribute.h>

#include <map>
#include <string>
#include <algorithm>

#define DECLARE_ENUM_BEGIN( CLASSNAME )\
class CLASSNAME {\
public:\
  enum Enum {
#define DECLARE_ENUM_END( CLASSNAME )\
        };\
};

namespace gfl { namespace maya { namespace plugins { namespace Shader {

  //-----------------------------------------------------------------------------
  /**
  * @class MultiSamplingTextureShader
  * @brief シェーダークラス
  */
  class MultiSamplingTextureVertexShader : public GLInterface::IShader
  {
  public:
    MultiSamplingTextureVertexShader();

    ~MultiSamplingTextureVertexShader(){}
    virtual GLInterface::ShaderType::Enum vGetShaderType() const	{ return GLInterface::ShaderType::Vertex; }
    virtual void vSetLinkProgram( GLInterface::ShaderObjectHandle h_object ){ this->m_hParentShaderObject = h_object; }
    virtual GLInterface::ShaderProgramHandle vGetProgramHandle() const { return m_hProgram; }
    virtual void vSetUpShader() const{}

    GLInterface::ShaderProgramHandle m_hProgram;
    GLInterface::ShaderObjectHandle m_hParentShaderObject;
  };

  //-----------------------------------------------------------------------------
  /**
  * @class EdgeFillterPixelShader
  * @brief シェーダークラス
  */
  class MultiSamplingTexturePixelShader : public GLInterface::IShader
  {
  public:
    MultiSamplingTexturePixelShader();
    ~MultiSamplingTexturePixelShader(){}
    virtual GLInterface::ShaderType::Enum vGetShaderType() const { return GLInterface::ShaderType::Pixel; }
    virtual void vSetLinkProgram( GLInterface::ShaderObjectHandle h_object ){ this->m_hParentShaderObject = h_object; }
    virtual GLInterface::ShaderProgramHandle vGetProgramHandle() const { return m_hProgram; }
    virtual void vSetUpShader() const;

    GLInterface::ShaderProgramHandle m_hProgram;
    GLInterface::ShaderObjectHandle m_hParentShaderObject;
  };

}}}}

#undef DECLARE_ENUM_BEGIN
#undef DECLARE_ENUM_END

#endif
