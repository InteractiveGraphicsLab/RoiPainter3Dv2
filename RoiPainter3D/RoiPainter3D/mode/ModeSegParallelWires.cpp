#include "stdafx.h"
#include "ModeSegParallelWires.h"
#include "FormSegParallelWires.h"
#include "ImageCore.h"
#include "CrsSecCore.h"
#include "climessagebox.h"


using namespace RoiPainter3D;


ModeSegParallelWires::ModeSegParallelWires() : 
  m_volume_shader("shader/volVtx.glsl"   , "shader/volFlg.glsl"),
  m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg.glsl")
{
  std::cout << "ModeSegParallelWires constructure...\n";

  m_bL = m_bR = m_bM = false;
  m_draging_cpid << -1, -1;

  std::cout << "ModeSegParallelWires done\n";
}



ModeSegParallelWires::~ModeSegParallelWires()
{
}



// このモードでは plane をちょうど画素中心を通る位置に配置したい 
//
// D = 5なら
// plane_xyは，0.1, 0.3, 0.5, 0.7, 0.9 に配置したい
// position = 

void ModeSegParallelWires::StartMode ()
{
  std::cout << "ModeSegParallelWires...startMode----------\n";

  m_bL = m_bR = m_bM = false;
  m_draging_cpid << -1, -1;
  FormSegParallelWires_Show();

  //initialize wires
  int W, H, D;
  std::tie(W,H,D) =ImageCore::GetInst()->GetResolution3();
  
  m_wires_xy.clear();
  m_wires_yz.clear();
  m_wires_zx.clear();

  m_wires_xy.resize(D);
  m_wires_yz.resize(W);
  m_wires_zx.resize(H);
  
  //x
  m_planexy_pos = D / 2;
  m_planeyz_pos = W / 2;
  m_planezx_pos = H / 2;
  
  float xy_pos = (m_planexy_pos + 0.5f) * (1.0f / D);
  float yz_pos = (m_planeyz_pos + 0.5f) * (1.0f / W);
  float zx_pos = (m_planezx_pos + 0.5f) * (1.0f / H);
  CrssecCore::GetInst()->SetPlaneXyPosition( xy_pos );
  CrssecCore::GetInst()->SetPlaneYzPosition( yz_pos );
  CrssecCore::GetInst()->SetPlaneZxPosition( zx_pos );
  
  std::cout << "ModeSegParallelWires...startMode DONE-----\n";
}



bool ModeSegParallelWires::CanLeaveMode()
{
  bool is_wire_exist = false;

  for ( const auto wire : m_wires_xy )
  {
    if ( wire.GetNumCPs() != 0 ) is_wire_exist = true;
    if ( is_wire_exist ) break;
  }
  for ( const auto wire : m_wires_yz )
  {
    if ( wire.GetNumCPs() != 0 ) is_wire_exist = true;
    if ( is_wire_exist ) break;
  }
  for ( const auto wire : m_wires_zx )
  {
    if ( wire.GetNumCPs() != 0 ) is_wire_exist = true;
    if ( is_wire_exist ) break;
  }  
  
  if ( !is_wire_exist ) return true;	

  if (CLI_MessageBox_YESNO_Show("Current Wires will be trushed. Do you want to leave?", "message") )
	{
		return true;
	}

	return false;
}




CRSSEC_ID ModeSegParallelWires::GetCurrentTrgtPlane()
{
  if ( )

}











//-------------------------------------------------------------  
// Mouse Listener ---------------------------------------------
void ModeSegParallelWires::LBtnDclk  (const EVec2i &p, OglForCLI *ogl){}
void ModeSegParallelWires::RBtnDclk  (const EVec2i &p, OglForCLI *ogl){}
void ModeSegParallelWires::MBtnDclk  (const EVec2i &p, OglForCLI *ogl){}


void ModeSegParallelWires::LBtnDown  (const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;
    
  if ( IsShiftKeyOn() )
  {
    //add control point
  }
  else if ( IsCtrKeyOn() )
  {
    
  }
  else
  {
    ogl->BtnDown_Trans(p);
  }
}


void ModeSegParallelWires::RBtnDown  (const EVec2i &p, OglForCLI *ogl)
{
  m_bR = true;
    
  if ( IsShiftKeyOn() )
  {
    //pick to remove controlpoint
    
  }
  else
  {
    ogl->BtnDown_Rot(p);
  }
}



void ModeSegParallelWires::MBtnDown  (const EVec2i &p, OglForCLI *ogl)
{
  m_bM = true;
    
  if ( IsShiftKeyOn() )
  {
    //pick to remove controlpoint
    
  }
  else
  {
    ogl->BtnDown_Zoom(p);
  }
}



void ModeSegParallelWires::LBtnUp    (const EVec2i &p, OglForCLI *ogl)
{
  m_bL = false;
  ogl->BtnUp();
}


void ModeSegParallelWires::RBtnUp    (const EVec2i &p, OglForCLI *ogl)
{
  m_bR = false;
  ogl->BtnUp();
}

void ModeSegParallelWires::MBtnUp    (const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
  ogl->BtnUp();
}


void ModeSegParallelWires::MouseMove (const EVec2i &p, OglForCLI *ogl){}
void ModeSegParallelWires::MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl){}

// Mouse Listener ---------------------------------------------
//-------------------------------------------------------------  


  void ModeSegParallelWires::KeyDown(int nChar){}
  void ModeSegParallelWires::KeyUp  (int nChar){}

  void ModeSegParallelWires::DrawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF){}

  void ModeSegParallelWires::FinishSegmentation(){}
  void ModeSegParallelWires::CancelSegmentation(){}
  //------------------------------------------------------------------------------

  void ModeSegParallelWires::ImportWireInfo(std::string fname){}
  void ModeSegParallelWires::ExportWireInfo(std::string fname){}