#pragma once


//
// class ModeSegVoxelPaint
// support 
//   1. Segmentation Voxel paint  : paint modeである領域を生成する 
//   2. Refinement   Voxel paint  : paint modeである領域を修正する
//
// 二種類のモードをこのクラスがサポートする



#include "ModeInterface.h"
#include "GlslShader.h"
#include <vector>

using namespace::std;



#pragma unmanaged

class ModeSegVoxelPaint : public ModeInterface
{
  GlslShaderVolume m_volumeShader;
  GlslShaderCrsSec m_crssecShader;

  // field for MODE_REF_VOXEL_PAINT
  bool m_bRefinmentMode;
  int  m_refineMaskId  ; 

  //mouse manipuration
	bool      m_bPaintVoxel;
	bool      m_bDrawLasso ;
  CRSSEC_ID m_lassoTrgtId;
	vector<EVec3f>  m_lasso;
	vector<EVec4i>  m_paintVoxels;

  ModeSegVoxelPaint();
public:
  ~ModeSegVoxelPaint();

  static ModeSegVoxelPaint* getInst() { 
    static ModeSegVoxelPaint p; 
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

  void finishSegmentation();
  void cancelSegmentation();
};

#pragma managed

