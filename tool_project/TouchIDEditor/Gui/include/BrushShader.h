#ifndef _GF_MAYA_PLUGINS_SHADER_BrushSHADER_H_
#define _GF_MAYA_PLUGINS_SHADER_BrushSHADER_H_

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
  * @class BrushVertexShader
  * @brief シェーダークラス
  */
  class BrushVertexShader : public GLInterface::IShader
  {
  public:
    BrushVertexShader();

    ~BrushVertexShader(){}
    virtual GLInterface::ShaderType::Enum vGetShaderType() const { return GLInterface::ShaderType::Vertex; }
    virtual void vSetLinkProgram( GLInterface::ShaderObjectHandle h_object ){ this->m_hParentShaderObject = h_object; }
    virtual GLInterface::ShaderProgramHandle vGetProgramHandle() const { return m_hProgram; }
    virtual void vSetUpShader() const{}

    GLInterface::ShaderProgramHandle m_hProgram;
    GLInterface::ShaderObjectHandle m_hParentShaderObject;
  };

  //-----------------------------------------------------------------------------
  /**
  * @class BrushPixelShader
  * @brief シェーダークラス
  */
  class BrushPixelShader : public GLInterface::IShader
  {
  public:
    BrushPixelShader();
    ~BrushPixelShader(){}
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
