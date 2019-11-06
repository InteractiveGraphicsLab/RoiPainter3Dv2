#include "stdafx.h"
#include "ModeSegParallelWires.h"
#include "FormSegParallelWires.h"
#include "ImageCore.h"
#include "CrsSecCore.h"
#include "climessagebox.h"
#include "kcurves.h"


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





//---------------------------------------------------------------------------//
//--------SplineWire---------------------------------------------------------//

SplineWire::SplineWire( PLANE_ID plane_id ) : 
  m_plane_id( plane_id )
{
  m_cps.clear();
  m_curve.clear();
}



void SplineWire::AddControlPoint (const EVec3f &p)
{
  //todo insert the closest two points
  m_cps.push_back(p);
  UpdateCurveFromCPs();
}



void SplineWire::MoveControlPoint(const int idx, const EVec3f &p)
{
  if ( idx < 0 || m_cps.size() <= idx ) return;
  m_cps[ idx ] = p;
  UpdateCurveFromCPs();
}



int SplineWire::PickControlPoint(
    const EVec3f &ray_pos, 
    const EVec3f &ray_dir, 
    const float  &radius)
{
  for ( int i = 0; i < (int)m_cps.size(); ++i)
  {
    if( t_distPointToLineSegment( m_cps[i], ray_pos, ray_dir) < radius ) 
      return i;
  }
  return -1;
}



void SplineWire::PickAndRemoveControlPoint(
    const EVec3f &ray_pos, 
    const EVec3f &ray_dir, 
    const float  &radius)
{
  int idx = PickControlPoint(ray_pos, ray_dir, radius);
  
  if ( idx != -1 ) 
  {
    m_cps.erase( m_cps.begin() + idx );  
  }

  UpdateCurveFromCPs();
}




int SplineWire::UpdateCurveFromCPs()
{
  m_curve.clear();
  if( m_cps.size() < 3 ) return;
  
  std::vector<EVec2d> cps_2d ( m_cps.size() ) ;

  if ( m_plane_id == PLANE_ID::PLANE_XY ) 
  {
    for ( int i = 0; i < (int) m_cps.size(); ++i)
      cps_2d[i] << m_cps[i][0], m_cps[i][1];   
  }
  else if ( m_plane_id == PLANE_ID::PLANE_YZ ) 
  {
    for ( int i = 0; i < (int) m_cps.size(); ++i)
      cps_2d[i] << m_cps[i][1], m_cps[i][2];   
  }
  else 
  {
    for ( int i = 0; i < (int) m_cps.size(); ++i)
      cps_2d[i] << m_cps[i][2], m_cps[i][0];
  }

  //  
  EVec3f pitch        = ImageCore::GetInst()->GetPitch();
  float  ave_pitch    = (pitch[0] + pitch[1] + pitch[2]) / 3.0;
  float  total_length = t_verts_Length( m_cps, true );
  int    num_samples  = (int)( total_length / ave_pitch / 10.0);
  if ( num_samples < 15 ) num_samples = 15; 

  
  std::vector<EVec2d> new_cps, curve_2d;
  compute_kCurves( cps_2d, num_samples, new_cps, curve_2d);
  
  //input info


}



