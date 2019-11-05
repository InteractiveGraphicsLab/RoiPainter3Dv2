#pragma once

// class ModeSegVoxelPaint
// 
// Segmetation by placing multiple wires (spline curve)
//

#include "ModeInterface.h"
#include "GlslShader.h"
#include "COMMON\kcurves.h"
#include <vector>


class SplineWire
{
public:
  SplineWire(){}

  //control point manipulation
  void AddControlPoint(){}
  void InsertControlPoint(){}
  int  PickControlPoint(){return -1;}
  void MoveControlPoint(){}
  void RemoveControlPoint(){}
  
  //rendering
  void DrawControlPoints(){}
  void DrawWire(){}
};




class ModeSegParallelWires : public ModeInterface
{
private:
  GlslShaderVolume m_volume_shader;
  GlslShaderCrsSec m_crssec_shader;
  
  CRSSEC_ID m_trgtplane;

  //wires
  std::vector<SplineWire> m_wires_xy;
  std::vector<SplineWire> m_wires_yz;
  std::vector<SplineWire> m_wires_zx;
  
  //dragging wire id , cp id
	EVec2i m_draging_cpid;

private:
  ModeSegParallelWires();

public:
  ~ModeSegParallelWires();


  static ModeSegParallelWires* GetInst() { 
    static ModeSegParallelWires p; 
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

  void KeyDown(int nChar);
  void KeyUp  (int nChar);

  bool CanLeaveMode();
  void StartMode ();
  void DrawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF);

  void FinishSegmentation();
  void CancelSegmentation();

};


#pragma managed

