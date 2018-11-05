#pragma once

#include "ModeInterface.h"
#include "GlslShader.h"
#include <vector>

using namespace::std;





#pragma unmanaged

class ModeSegRGrow : public ModeInterface
{
  GlslShaderVolume m_volumeShader;
  GlslShaderCrsSec m_crssecShader;
  
  float m_CpSize;
  bool  m_bRegionUpdated;
  int   m_dragCpId;
  bool  m_bDrawStr;
  vector<EVec3f> m_stroke;


  TMesh m_CpSph;
	vector<EVec3f> m_CPs;

  ModeSegRGrow();
public:
  ~ModeSegRGrow();

  static ModeSegRGrow* getInst() { 
    static ModeSegRGrow p; 
    return &p; 
  }

  // overload functions ---------------------------------------------
  MODE_ID getModeID() { return MODE_VIS_NORMAL; }

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


public:
  void runThresholding(short minV, short maxV);
  void runRegionGrow6 (short minV, short maxV);
  void runRegionGrow26(short minV, short maxV);
  void runErosion();
  void runDilation();
  void runFillHole();

  void finishSegmentationStoreRegion();



private:
  int pickCPs(const EVec3f &rayP, const EVec3f &rayD);

};

#pragma managed

