//===================================================================
/**
 * @file    CTRApplication.hpp
 * @brief   CTR�A�v���P�[�V���� ���C���t���[�����[�N
 * @author  GAME FREAK Inc.
 * @date    15.01.17
*/
//===================================================================

#if defined(GF_PLATFORM_CTR)

class CTRApplication: public Main::Framework::IApplication
{
public:
  CTRApplication();

  virtual~CTRApplication();

  virtual bool OnInit(void);
  virtual bool OnUpdate(void);
  virtual bool OnDraw(void);
  virtual bool OnTerm(void);
};


#endif //defined(GF_PLATFORM_CTR)
