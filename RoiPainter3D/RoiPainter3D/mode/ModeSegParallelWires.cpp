#include "stdafx.h"
#include "ModeSegParallelWires.h"
#include "FormSegParallelWires.h"
#include "ImageCore.h"
#include "CrsSecCore.h"
#include "climessagebox.h"
#include "kcurves.h"
#include "FormMain.h"
#include "FormVisParam.h"



// TODO
//
// 0. render volume and planes OK 
// 1. camera rotation          OK
// 2. mouse wheel and key to move target plane OK
// 3. place/move/delete control point 
// 4. insert controlpoint 
// 5. fill inside   
// 6. save/load wires 
// 7. test and manual  
// 


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

  m_wires_xy.resize(D, SplineWire(SplineWire::PLANE_XY) );
  m_wires_yz.resize(W, SplineWire(SplineWire::PLANE_YZ) );
  m_wires_zx.resize(H, SplineWire(SplineWire::PLANE_ZX) );
  
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
  
  CrssecCore::GetInst()->ClearCurvedCrossec();
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
  return FormParallelWires_bPlaneXY() ? CRSSEC_XY : 
         FormParallelWires_bPlaneYZ() ? CRSSEC_YZ : CRSSEC_ZX; 
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


void ModeSegParallelWires::MouseMove (const EVec2i &p, OglForCLI *ogl)
{
  if ( !m_bL && !m_bR && !m_bM)  return;

  if ( m_draging_cpid[0] == -2 )
  {
	  EVec3f ray_pos, ray_dir;
	  ogl->GetCursorRay( p, ray_pos, ray_dir);
    //todo
  }
  else
  {
		ogl->MouseMove(p);
  }

  FormMain_RedrawMainPanel();

}




void ModeSegParallelWires::MouseWheel(
    const EVec2i &p, 
    short zDelta, 
    OglForCLI *ogl)
{
	EVec3f ray_pos, ray_dir, pos;
	ogl->GetCursorRay( p, ray_pos, ray_dir);
  
  const bool b_xy = formVisParam_bPlaneXY() || FormParallelWires_bPlaneXY();
  const bool b_yz = formVisParam_bPlaneYZ() || FormParallelWires_bPlaneYZ();
  const bool b_zx = FormVisParam_bPlaneZX() || FormParallelWires_bPlaneZX();
  CRSSEC_ID trgt_id = PickCrsSec(ray_pos, ray_dir, &pos, b_xy, b_yz, b_zx);

  int W,H,D;
  std::tie(W,H,D) = ImageCore::GetInst()->GetResolution3();
  
  if ( trgt_id == CRSSEC_XY )
  {
    m_planexy_pos += (zDelta>0)?1:-1;
    m_planexy_pos = t_crop<int>(0, D-1,m_planexy_pos);

    float xy_pos = (m_planexy_pos + 0.5f) * (1.0f / D);
    CrssecCore::GetInst()->SetPlaneXyPosition( xy_pos );
  } 
  else if (trgt_id == CRSSEC_YZ) 
  { 
    m_planeyz_pos += (zDelta>0)?1:-1;
    m_planeyz_pos = t_crop<int>(0, W-1,m_planeyz_pos);

    float yz_pos = (m_planeyz_pos + 0.5f) * (1.0f / W);
    CrssecCore::GetInst()->SetPlaneYzPosition( yz_pos );
  } 
  else if (trgt_id == CRSSEC_ZX)
  {
    m_planezx_pos += (zDelta>0)?1:-1;
    m_planezx_pos = t_crop<int>(0, H-1,m_planezx_pos);

    float zx_pos = (m_planezx_pos + 0.5f) * (1.0f / H);
    CrssecCore::GetInst()->SetPlaneZxPosition( zx_pos );
  } 

  FormMain_RedrawMainPanel();
}




// Mouse Listener ---------------------------------------------
//-------------------------------------------------------------  


void ModeSegParallelWires::KeyDown(int nChar)
{
  if ( nChar == 38 /* up */ || nChar == 40 /* down */) 
  {
    int step = (nChar == 38) ? + 1: -1;
 
    int W,H,D;
    std::tie(W,H,D) = ImageCore::GetInst()->GetResolution3();
  
    if ( FormParallelWires_bPlaneXY() )
    {
      m_planexy_pos += step;
      m_planexy_pos = t_crop<int>(0, D-1,m_planexy_pos);

      float xy_pos = (m_planexy_pos + 0.5f) * (1.0f / D);
      CrssecCore::GetInst()->SetPlaneXyPosition( xy_pos );
    } 
    else if ( FormParallelWires_bPlaneYZ() ) 
    { 
      m_planeyz_pos += step;
      m_planeyz_pos = t_crop<int>(0, W-1,m_planeyz_pos);

      float yz_pos = (m_planeyz_pos + 0.5f) * (1.0f / W);
      CrssecCore::GetInst()->SetPlaneYzPosition( yz_pos );
    } 
    else if ( FormParallelWires_bPlaneZX() )
    {
      m_planezx_pos += step;
      m_planezx_pos = t_crop<int>(0, H-1,m_planezx_pos);

      float zx_pos = (m_planezx_pos + 0.5f) * (1.0f / H);
      CrssecCore::GetInst()->SetPlaneZxPosition( zx_pos );
    } 
    FormMain_RedrawMainPanel();
  }

}

void ModeSegParallelWires::KeyUp  (int nChar){}




void ModeSegParallelWires::DrawScene(
    const EVec3f &cuboid, 
    const EVec3f &camP, 
    const EVec3f &camF)
{
  //bind volumes ---------------------------------------
  const bool image_interpolation = formVisParam_doInterpolation();

  glActiveTextureARB(GL_TEXTURE0);
  ImageCore::GetInst()->m_vol.BindOgl(image_interpolation);
  glActiveTextureARB(GL_TEXTURE1);
  ImageCore::GetInst()->m_vol_gm.BindOgl(image_interpolation);
  glActiveTextureARB(GL_TEXTURE2);
  ImageCore::GetInst()->m_vol_flag.BindOgl(false);
  glActiveTextureARB(GL_TEXTURE3);
  ImageCore::GetInst()->m_vol_mask.BindOgl(false);
  glActiveTextureARB(GL_TEXTURE4);
  formVisParam_bindTfImg();
  glActiveTextureARB(GL_TEXTURE5);
  formVisParam_bindPsuImg();
  glActiveTextureARB(GL_TEXTURE6);
  ImageCore::GetInst()->m_img_maskcolor.BindOgl(false);

  //if (m_bDrawStr) t_DrawPolyLine(EVec3f(1,1,0), 3, m_stroke);

  const EVec3i reso = ImageCore::GetInst()->GetResolution();
  const bool b_gm = formVisParam_bGradMag();
  const bool b_xy = formVisParam_bPlaneXY() || FormParallelWires_bPlaneXY();
  const bool b_yz = formVisParam_bPlaneYZ() || FormParallelWires_bPlaneYZ();
  const bool b_zx = FormVisParam_bPlaneZX() || FormParallelWires_bPlaneZX();

  glColor3d(1, 1, 1);
  m_crssec_shader.Bind(0, 1, 2, 3, 6, reso, b_gm, false);
  CrssecCore::GetInst()->DrawCrssec(b_xy, b_yz, b_zx, cuboid);
  m_crssec_shader.Unbind();


  const bool   b_rend_vol = formVisParam_bRendVol();

  if (b_rend_vol)
  {
    const bool   b_psuedo = formVisParam_bDoPsued();
    const float alpha     = formVisParam_getAlpha();
    const bool  b_onmanip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    const int   num_slice = (int)((b_onmanip ? ONMOVE_SLICE_RATE : 1.0) * formVisParam_getSliceNum());

    glDisable(GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    m_volume_shader.Bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, camP, b_psuedo, false);
    t_DrawCuboidSlices(num_slice, camP, camF, cuboid);
    m_volume_shader.Unbind();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }
}

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




void SplineWire::UpdateCurveFromCPs()
{
  m_curve.clear();
  if( m_cps.size() < 3 ) return;
  
  //convert point 3D --> 2D 
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

  //prepare num of sampling points
  EVec3f pitch        = ImageCore::GetInst()->GetPitch();
  float  ave_pitch    = (pitch[0] + pitch[1] + pitch[2]) / 3.0f;
  float  total_length = t_verts_Length( m_cps, true );
  int    num_samples  = (int)( total_length / ave_pitch / 10.0);
  if ( num_samples < 15 ) num_samples = 15; 

  //compute curve
  std::vector<EVec2d> new_cps, curve_2d;
  compute_kCurves( cps_2d, num_samples, new_cps, curve_2d);
    

  //convert curve 3D --> 2D 
  m_curve.resize( curve_2d.size() );
  if ( m_plane_id == PLANE_ID::PLANE_XY ) 
  {
    float z = m_cps.front()[2];
    for ( int i = 0; i < (int) curve_2d.size(); ++i)
      m_curve[i] << (float)curve_2d[i][0], (float)curve_2d[i][1], z;
  }
  else if ( m_plane_id == PLANE_ID::PLANE_YZ ) 
  {
    float x = m_cps.front()[0];
    for ( int i = 0; i < (int) curve_2d.size(); ++i)
      m_curve[i] << x, (float)curve_2d[i][0], (float)curve_2d[i][1];
  }
  else 
  {
    float y = m_cps.front()[2];
    for ( int i = 0; i < (int) curve_2d.size(); ++i)
      m_curve[i] << (float)curve_2d[i][2], y, (float)curve_2d[i][0];
  }
}



