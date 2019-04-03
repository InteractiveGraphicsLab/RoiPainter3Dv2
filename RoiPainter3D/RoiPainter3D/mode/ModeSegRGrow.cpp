#include "ModeSegRGrow.h"

#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegRGrow.h"

#include "climessagebox.h"
#include "./COMMON/tqueue.h"

using namespace RoiPainter3D;
using namespace std;

#pragma unmanaged

// usage of "vol_flg" 
//  0 : non target voxels (never change )
//  1 : target & back ground voxels 
// 255: target & fore ground voxels

ModeSegRGrow::ModeSegRGrow() :
  m_volume_shader("shader/volVtx.glsl"   , "shader/volFlg_Seg.glsl"),
  m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg_Seg.glsl")
{
	m_bL = m_bR = m_bM = false;
	m_drag_cp_id   = -1;
  m_cp_size      = 0.1f;
	m_b_roi_update = false;
  m_b_drawstroke = false;
}


ModeSegRGrow::~ModeSegRGrow()
{
}



bool ModeSegRGrow::CanLeaveMode()
{
	if( !m_b_roi_update) return true;

	if ( CLI_MessageBox_YESNO_Show("Current Result is not stored. \nDo you want to leave?" ,"caution") )
	{
		return true;
	}
	return false;
}



void ModeSegRGrow::StartMode()
{
  //prepare flg volume (consider the Locked mask Ids)
	vector<MaskData> &mask  = ImageCore::GetInst()->m_mask_data;
	OglImage3D   &vol_mask  = ImageCore::GetInst()->m_vol_mask  ;
	OglImage3D   &vol_flg   = ImageCore::GetInst()->m_vol_flag  ;
	EVec3i       reso       = ImageCore::GetInst()->GetResolution();
	EVec2i       vol_minmax = ImageCore::GetInst()->GetVolMinMax();

	const int N = reso[0] * reso[1] * reso[2];
	for (int i = 0; i < N; ++i) vol_flg[i] = ( mask[vol_mask[i]].m_b_locked ) ? 0 : 1;
	vol_flg.SetUpdated();

	m_cp_centers.clear();
	m_cp_size = ImageCore::GetInst()->GetPitchW() * 1;
	m_cp_sphere.initializeIcosaHedron( m_cp_size );

  formSegRGrow_Show();
  formSegRGrow_InitAllItems(vol_minmax[0],vol_minmax[1]);
}



//////////////////////////////////////////////////////////
// Mouse Listener (Camera manipuration & CP manipuration)
void ModeSegRGrow::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  EVec3f ray_pos, ray_dir;
	ogl->GetCursorRay(p, ray_pos, ray_dir);

  m_bL = true;

  if (IsCtrKeyOn())
  {
    m_stroke.clear();
    m_b_drawstroke = true;
  }
  else if( (m_drag_cp_id = PickControlPoints(ray_pos, ray_dir) ) != -1 ) 
  {
    //do nothing 
  }
  else
  {
	  ogl->BtnDown_Trans(p);
  }
}


void ModeSegRGrow::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  if ( m_b_drawstroke )
  {
    EVec3f cube = ImageCore::GetInst()->GetCuboid();
    CrssecCore::GetInst()->GenerateCurvedCrssec(cube, ogl->GetCamPos(), m_stroke);
  }

  m_bL = m_b_drawstroke = false;
	m_drag_cp_id = -1;
	ogl->BtnUp();
  FormMain_RedrawMainPanel();
}

void ModeSegRGrow::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  ogl->BtnDown_Rot(p);
	m_bR = true;
}

void ModeSegRGrow::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = false;
	ogl->BtnUp();
  FormMain_RedrawMainPanel();
}

void ModeSegRGrow::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  ogl->BtnDown_Zoom(p);
	m_bM = true;
}

void ModeSegRGrow::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
	ogl->BtnUp();
  FormMain_RedrawMainPanel();
}


void ModeSegRGrow::LBtnDclk(const EVec2i &p, OglForCLI *ogl)
{
  EVec3f ray_pos, ray_dir;
	ogl->GetCursorRay( p, ray_pos, ray_dir );

	int cpid = PickControlPoints( ray_pos, ray_dir );
	if (cpid != -1)
	{
		m_cp_centers.erase( m_cp_centers.begin() + cpid );
	}
  else
  {
    EVec3f pos;
	  CRSSEC_ID id = PickCrssec(ray_pos, ray_dir, &pos);
	  if (id != CRSSEC_NON) m_cp_centers.push_back( pos );
  }
  FormMain_RedrawMainPanel();
}


void ModeSegRGrow::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegRGrow::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}


void ModeSegRGrow::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if ( !m_bL && !m_bR && !m_bM ) return;

  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);

  if ( m_b_drawstroke )
  {
    m_stroke.push_back(ray_pos + 0.1f * ray_dir);
    FormMain_RedrawMainPanel();
  }
  else if ( m_drag_cp_id != -1)
	{
		CRSSEC_ID id = PickCrssec(ray_pos, ray_dir, &pos);
		if (id != CRSSEC_NON) m_cp_centers[m_drag_cp_id] = pos;
	}
	else 
	{
		ogl->MouseMove(p);
	}

  FormMain_RedrawMainPanel();
}


void ModeSegRGrow::MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl)
{
  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay(p, ray_pos, ray_dir);
  EVec3i reso  = ImageCore::GetInst()->GetResolution();
  EVec3f pitch = ImageCore::GetInst()->GetPitch();


  CRSSEC_ID id = PickCrssec(ray_pos, ray_dir, &pos);
  if( id != CRSSEC_NON ) CrssecCore::GetInst()->MoveCrssec(reso, pitch, id, z_delta);
  else ogl->ZoomCam(z_delta * 0.1f);

  FormMain_RedrawMainPanel();
}



int ModeSegRGrow::PickControlPoints(const EVec3f &ray_pos, const EVec3f &ray_dir)
{
	for (int i = 0; i < (int)m_cp_centers.size(); ++i)
	{
		if (t_distRayToPoint(ray_pos, ray_dir, m_cp_centers[i]) < m_cp_size) return i;
	}
	return -1;
}



void ModeSegRGrow::KeyDown(int nChar) 
{
  FormMain_RedrawMainPanel();
}


void ModeSegRGrow::KeyUp(int nChar) 
{
  FormMain_RedrawMainPanel();
}


void ModeSegRGrow::DrawScene(const EVec3f &cuboid, const EVec3f &cam_pos, const EVec3f &cam_center)
{
  const bool image_interpolation = formVisParam_doInterpolation();
  
  //bind volumes ---------------------------------------
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

  //draw cut stroke 
  if ( m_b_drawstroke ) t_DrawPolyLine( EVec3f(1,1,0), 3, m_stroke );

  //draw planes
  const bool   b_xy = formVisParam_bPlaneXY();
  const bool   b_yz = formVisParam_bPlaneYZ();
  const bool   b_zx = formVisParam_bPlaneZX();
  const EVec3i reso = ImageCore::GetInst()->GetResolution();
  glColor3d(1, 1, 1);
  m_crssec_shader.Bind(0, 1, 2, 3, 6, reso, false, !IsSpaceKeyOn());
  CrssecCore::GetInst()->DrawCrssec( b_xy, b_yz, b_zx, cuboid );
  m_crssec_shader.Unbind();

  //volume rendering
  const bool   b_draw_vol= formVisParam_bRendVol();
  const bool   b_psuedo  = formVisParam_bDoPsued();
  const float  alpha     = formVisParam_getAlpha();
  const bool b_on_manip  = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
  const int  num_slice   = (int)((b_on_manip ? ONMOVE_SLICE_RATE : 1.0) * formVisParam_getSliceNum());

  if (b_draw_vol && !IsSpaceKeyOn())
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    m_volume_shader.Bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, cam_pos, b_psuedo, !IsSpaceKeyOn());
    t_DrawCuboidSlices(num_slice, cam_pos, cam_center, cuboid);
    m_volume_shader.Unbind();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }


  //draw control points
  const static float diff[4] = { 1   ,0,0,0 };
  const static float ambi[4] = { 0.5f,0,0,0 };
  const static float spec[4] = { 1   ,1,1,0 };
  const static float shin[1] = { 56.0f };
  glEnable(GL_LIGHTING);
  for ( const auto& it : m_cp_centers ) 
  {
    glTranslated( it[0], it[1], it[2]);
    m_cp_sphere.draw(diff, ambi, spec, shin);
    glTranslated(-it[0], -it[1], -it[2]);
  }
  glDisable(GL_LIGHTING);
}




void ModeSegRGrow::RunThresholding(short min_v, short max_v)
{
	const EVec3i reso    = ImageCore::GetInst()->GetResolution();
	const short  *vol    = ImageCore::GetInst()->m_vol_orig;
	OglImage3D  &vol_flg = ImageCore::GetInst()->m_vol_flag ;

	const int N = reso[0] * reso[1] * reso[2];

	for (int i = 0; i < N; ++i) if ( vol_flg[i] != 0 )
	{
		vol_flg[i] = (min_v <= vol[i] && vol[i] <= max_v) ? 255 : 1;
	}

	vol_flg.SetUpdated();
	m_b_roi_update = true;
  FormMain_RedrawMainPanel();
}



void ModeSegRGrow::RunRegionGrow6(short minv, short maxv)
{
  std::cout << "runRegionGrow6...";

  //サイズ関連を準備  
	const EVec3i reso = ImageCore::GetInst()->GetResolution();
  const EVec3f pitch= ImageCore::GetInst()->GetPitch();
	const int W = reso[0], H = reso[1], D = reso[2];
  const int WH = W*H;
  const int WHD = W*H*D;

	const short  *vol   = ImageCore::GetInst()->m_vol_orig;
	OglImage3D   &vflg   = ImageCore::GetInst()->m_vol_flag ; 

	for (int i = 0; i < WHD; ++i) vflg[i] = ( vflg[i] == 0) ? 0 : 1;

	//CP --> pixel id
	//volFlg : 0:never change, 1:back, 255:fore
	TQueue<EVec4i> Q;
	for ( const auto cp : m_cp_centers)
	{
		const int x = min(W-1, (int)( cp[0] / pitch[0] ) ) ;
		const int y = min(H-1, (int)( cp[1] / pitch[1] ) ) ;
		const int z = min(D-1, (int)( cp[2] / pitch[2] ) ) ;
		const int I = x + y*W + z*WH;

		if ( vflg[I] != 0 && minv <= vol[I] && vol[I] <= maxv)
		{
			Q.push_back( EVec4i(x, y, z, I) );
			vflg[I] = 255;
		}
	}

	while (!Q.empty())
	{
		const int x = Q.front()[0];
		const int y = Q.front()[1];
		const int z = Q.front()[2];
		const int I = Q.front()[3];
		Q.pop_front();

		//grow to 6  neighbors
		int K;
		K = I- 1; if ( x> 0  && vflg[K]==1 && minv<=vol[K] && vol[K]<=maxv) { vflg[K]=255; Q.push_back(EVec4i(x-1, y , z , K)); }
		K = I- W; if ( y> 0  && vflg[K]==1 && minv<=vol[K] && vol[K]<=maxv) { vflg[K]=255; Q.push_back(EVec4i( x ,y-1, z , K)); }
		K = I-WH; if ( z> 0  && vflg[K]==1 && minv<=vol[K] && vol[K]<=maxv) { vflg[K]=255; Q.push_back(EVec4i( x , y ,z-1, K)); }
		K = I+ 1; if ( x<W-1 && vflg[K]==1 && minv<=vol[K] && vol[K]<=maxv) { vflg[K]=255; Q.push_back(EVec4i(x+1, y , z , K)); }
		K = I+ W; if ( y<H-1 && vflg[K]==1 && minv<=vol[K] && vol[K]<=maxv) { vflg[K]=255; Q.push_back(EVec4i( x ,y+1, z , K)); }
		K = I+WH; if ( z<D-1 && vflg[K]==1 && minv<=vol[K] && vol[K]<=maxv) { vflg[K]=255; Q.push_back(EVec4i( x , y ,z+1, K)); }
	}
	
  vflg.SetUpdated();
	m_b_roi_update = true;
  FormMain_RedrawMainPanel();

  std::cout << "runRegionGrow6...DONE\n\n";

}


void ModeSegRGrow::RunRegionGrow26(short minV, short maxV)
{
  std::cout << "runRegionGrow26...";

  //サイズ関連を準備  
	const EVec3i reso = ImageCore::GetInst()->GetResolution();
  const EVec3f pitch= ImageCore::GetInst()->GetPitch();
	const int W = reso[0], H = reso[1], D = reso[2];
  const int WH = W*H;
  const int WHD = W*H*D;

	const short  *vol  = ImageCore::GetInst()->m_vol_orig;
	OglImage3D   &vflg = ImageCore::GetInst()->m_vol_flag ; 

	for (int i = 0; i < WHD; ++i) vflg[i] = ( vflg[i] == 0) ? 0 : 1;

	//CP --> pixel id
	//vflg : 0:never change, 1:back, 255:fore
	TQueue<EVec4i> Q;
	for ( const auto cp : m_cp_centers)
	{
		const int x = min(W-1, (int)( cp[0] / pitch[0] ) ) ;
		const int y = min(H-1, (int)( cp[1] / pitch[1] ) ) ;
		const int z = min(D-1, (int)( cp[2] / pitch[2] ) ) ;
		const int I = x + y*W + z*WH;

		if ( vflg[I] != 0 && minV <= vol[I] && vol[I] <= maxV)
		{
			Q.push_back( EVec4i(x, y, z, I) );
			vflg[I] = 255;
		}
	}

	while (!Q.empty())
	{
		const int x = Q.front()[0];
		const int y = Q.front()[1];
		const int z = Q.front()[2];
		const int I = Q.front()[3];
		Q.pop_front();

		//grow to 26  neighbors
		int K;
    K = I-1-W-WH; if (x> 0 &&y> 0 &&z> 0 && vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x-1, y-1, z-1, K)); }
    K = I  -W-WH; if (       y> 0 &&z> 0 && vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x  , y-1, z-1, K)); }
    K = I+1-W-WH; if (x<W-1&&y> 0 &&z> 0 && vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x+1, y-1, z-1, K)); }
    K = I-1  -WH; if (x> 0        &&z> 0 && vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x-1, y  , z-1, K)); }
    K = I    -WH; if (              z> 0 && vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x  , y  , z-1, K)); }
    K = I+1  -WH; if (x<W-1       &&z> 0 && vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x+1, y  , z-1, K)); }
    K = I-1+W-WH; if (x> 0 &&y<H-1&&z> 0 && vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x-1, y+1, z-1, K)); }
    K = I  +W-WH; if (       y<H-1&&z> 0 && vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x  , y+1, z-1, K)); }
    K = I+1+W-WH; if (x<W-1&&y<H-1&&z> 0 && vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x+1, y+1, z-1, K)); }

    K = I-1-W   ; if (x> 0 &&y> 0        && vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x-1, y-1, z  , K)); }
    K = I  -W   ; if (       y> 0        && vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x  , y-1, z  , K)); }
    K = I+1-W   ; if (x<W-1&&y> 0        && vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x+1, y-1, z  , K)); }
    K = I-1     ; if (x> 0               && vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x-1, y  , z  , K)); }
  //K = I       ; if (                   && vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x  , y  , z  , K)); }
    K = I+1     ; if (x<W-1              && vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x+1, y  , z  , K)); }
    K = I-1+W   ; if (x> 0 &&y<H-1       && vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x-1, y+1, z  , K)); }
    K = I  +W   ; if (       y<H-1       && vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x  , y+1, z  , K)); }
    K = I+1+W   ; if (x<W-1&&y<H-1       && vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x+1, y+1, z  , K)); }

    K = I-1-W+WH; if (x> 0 &&y> 0 &&z<D-1&& vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x-1, y-1, z+1, K)); }
    K = I  -W+WH; if (       y> 0 &&z<D-1&& vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x  , y-1, z+1, K)); }
    K = I+1-W+WH; if (x<W-1&&y> 0 &&z<D-1&& vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x+1, y-1, z+1, K)); }
    K = I-1  +WH; if (x> 0        &&z<D-1&& vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x-1, y  , z+1, K)); }
    K = I    +WH; if (              z<D-1&& vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x  , y  , z+1, K)); }
    K = I+1  +WH; if (x<W-1       &&z<D-1&& vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x+1, y  , z+1, K)); }
    K = I-1+W+WH; if (x> 0 &&y<H-1&&z<D-1&& vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x-1, y+1, z+1, K)); }
    K = I  +W+WH; if (       y<H-1&&z<D-1&& vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x  , y+1, z+1, K)); }
    K = I+1+W+WH; if (x<W-1&&y<H-1&&z<D-1&& vflg[K] == 1 && minV<=vol[K] && vol[K]<=maxV) { vflg[K] = 255; Q.push_back(EVec4i(x+1, y+1, z+1, K)); }
	}
	
  vflg.SetUpdated();
	m_b_roi_update = true;
  FormMain_RedrawMainPanel();
	std::cout << "runRegionGrow26...DONE\n\n";
}


void ModeSegRGrow::RunDilation()
{
	t_Dilate3D( ImageCore::GetInst()->m_vol_flag );
	ImageCore::GetInst()->m_vol_flag.SetUpdated();
	m_b_roi_update = true;
  FormMain_RedrawMainPanel();
}


void ModeSegRGrow::RunErosion()
{
	t_Erode3D( ImageCore::GetInst()->m_vol_flag );
	ImageCore::GetInst()->m_vol_flag.SetUpdated();
	m_b_roi_update = true;
  FormMain_RedrawMainPanel();
}


void ModeSegRGrow::RunFillHole()
{
  //compute fill hole
	OglImage3D flg( ImageCore::GetInst()->m_vol_flag );
	const int N = flg.GetW() * flg.GetH() * flg.GetD();
	for (int i = 0; i < N; ++i) flg[i] = (flg[i] == 255) ? 255 : 0;
	t_FillHole3D(flg);

  // update flg volume (never change voxel with vflg[i]==0)
	OglImage3D &trgt = ImageCore::GetInst()->m_vol_flag;

	for (int i = 0; i < N; ++i) if (trgt[i] == 1)
	{
		trgt[i] = (flg[i] == 255) ? 255 : 1;
	}

	trgt.SetUpdated();
	m_b_roi_update = true;
  FormMain_RedrawMainPanel();
}


void ModeSegRGrow::FinishSegmentation()
{
	const EVec3i res = ImageCore::GetInst()->GetResolution();
	const int    N   = res[0] * res[1] * res[2];

	bool b_forevoxel_exist = false;
	for ( int i = 0; i < N; ++i)
	{
		if ( ImageCore::GetInst()->m_vol_flag[i] == 255)
		{
			b_forevoxel_exist = true;
			break;
		}
	}

	if ( !b_forevoxel_exist ) 
	{
		CLI_MessageBox_OK_Show("No foreground voxel exist\n", "caution");
		return;
	}

	ImageCore::GetInst()->StoreForegroundAsNewMask();
	m_b_roi_update = false;
	ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK );
}


#pragma managed

