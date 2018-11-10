#pragma once

#include "ModeInterface.h"
#include "GlslShader.h"
#include <vector>

using namespace::std;





#pragma unmanaged

class ModeRefStrokeTrim : public ModeInterface
{
  GlslShaderVolume m_volumeShader;
  GlslShaderCrsSec m_crssecShader;


  ModeRefStrokeTrim();
public:
  ~ModeRefStrokeTrim();

  static ModeRefStrokeTrim* getInst() { 
    static ModeRefStrokeTrim p; 
    return &p; 
  }

  // overload functions ---------------------------------------------

  void LBtnUp    (const EVec2i &p, OglForCLI *ogl);
  void RBtnUp    (const EVec2i &p, OglForCLI *ogl);
  void MBtnUp    (const EVec2i &p, OglForCLI *ogl);
  void LBtnDown  (const EVec2i &p, OglForCLI *ogl);
  void RBtnDown  (const EVec2i &p, OglForCLI *ogl);
  void MBtnDown  (const EVec2i &p, OglForCLI *ogl);
  void LBtnDclk  (const EVec2i &p, OglForCLI *ogl);
  void RBtnDclk  (const EVec2i &p, OglForCLI *ogl);
  void MBtnDclk  (const EVec2i &p, OglForCLI *ogl);
  void MouseMove (const EVec2i &p, OglForCLI *ogl);
  void MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl);

  void keyDown(int nChar);
  void keyUp  (int nChar);

  bool canEndMode();
  void startMode ();
  void drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF);

  void cancelSegmentation();
  void finishSegmentation();


};

#pragma managed

