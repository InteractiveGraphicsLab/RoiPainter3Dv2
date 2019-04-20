
#include "ModeRefStrokeTrim.h"

#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#include "FormMain.h"
#include "FormVisParam.h"
#include "FormRefStrokeTrim.h"
#include "FormMaskIDselection.h"
#include "climessagebox.h"

using namespace RoiPainter3D;
using namespace std;

#pragma unmanaged


ModeRefStrokeTrim::~ModeRefStrokeTrim()
{
	if( m_vol_prev ) delete[] m_vol_prev;
}


ModeRefStrokeTrim::ModeRefStrokeTrim() :
	  m_volume_shader("shader/volVtx.glsl"   , "shader/volFlg_Seg.glsl"),
	  m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg_Seg.glsl")
{
  m_bL = m_bR = m_bM = m_b_drawingstroke = false;
  m_b_modified = false;
  m_vol_prev = 0;
}


bool ModeRefStrokeTrim::CanLeaveMode()
{
  if( !m_b_modified ) return true;
	
  if (CLI_MessageBox_YESNO_Show("Current Result is not stored. Do you want to leave?", "message") )
	{
		return true;
	}
	return false;
}


//volFlg
// 0  : locked and never changed, 
// 1  : back  
//255 : fore 
void ModeRefStrokeTrim::StartMode()
{
	m_b_modified = false;

  
  m_trgt_maskid = formMaskIdSelection_showModalDialog( 
    ImageCore::GetInst()->GetMaskData(), 
    ImageCore::GetInst()->GetActiveMaskID());

  if( m_trgt_maskid < 0 ){
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    return;
  }

  if( m_trgt_maskid == 0  ){
    CLI_MessageBox_OK_Show("Cant modify mask ID = 0", "message");
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    return;
  }

  formRefStrokeTrim_Show();
  CrssecCore::GetInst()->ClearCurvedCrossec();

	const int  nun_voxels = ImageCore::GetInst()->GetNumVoxels();
  const byte *msk3d     = ImageCore::GetInst()->m_vol_mask.GetVolumePtr()  ;
	byte       *flg3d     = ImageCore::GetInst()->m_vol_flag.GetVolumePtr()  ;

	for (int i = 0; i < nun_voxels; ++i) 
    flg3d[i] = ( msk3d[i] == m_trgt_maskid) ? 255 : 0;
	
  ImageCore::GetInst()->m_vol_flag.SetUpdated();

	if( m_vol_prev ) delete[] m_vol_prev;
	m_vol_prev = new byte[nun_voxels];
	memcpy(m_vol_prev, &(msk3d[0]), sizeof(byte)*nun_voxels);
}




void ModeRefStrokeTrim::cancelSegmentation()
{
  if( m_vol_prev ) delete[] m_vol_prev;
  m_vol_prev = 0;
  ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK );
  FormMain_RedrawMainPanel();
}


void ModeRefStrokeTrim::finishSegmentation()
{
	const int num_voxels = ImageCore::GetInst()->GetNumVoxels();
	byte  *msk3d = ImageCore::GetInst()->m_vol_mask.GetVolumePtr();
	byte  *flg3d = ImageCore::GetInst()->m_vol_flag.GetVolumePtr();


	//一旦 トリムされた画素のIDを0に
	for( int i= 0; i < num_voxels; ++i)
	{
		if( msk3d[i] == m_trgt_maskid && flg3d[i] != 255 ) msk3d[i] = 0;
	}
  ImageCore::GetInst()->ClearMaskSurface(m_trgt_maskid);

	//トリムされた領域を新しい領域として追加
  bool bTrimRegionExist = false;
	for( int i= 0; i < num_voxels; ++i) 
  {
    if( flg3d[i] == 1 ) bTrimRegionExist  = true;
    flg3d[i] = ( flg3d[i] == 1 ) ? 255 : 0;
  }
  if(bTrimRegionExist)
	  ImageCore::GetInst()->StoreForegroundAsNewMask();

	ImageCore::GetInst()->m_vol_mask.SetUpdated();
	ImageCore::GetInst()->m_vol_flag.SetUpdated();

	m_b_modified = false;

  if( m_vol_prev ) delete[] m_vol_prev;
  m_vol_prev = 0;

	ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK );
	FormMain_RedrawMainPanel();
}



////////////////////////////////////////////////////////////////////////
//Mouse Listener ///////////////////////////////////////////////////////

void ModeRefStrokeTrim::LBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeRefStrokeTrim::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeRefStrokeTrim::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}

void ModeRefStrokeTrim::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;
	m_stroke2d.clear();
	m_stroke3d.clear();

	if (IsShiftKeyOn())
	{
		m_b_drawingstroke = true;
		EVec3f rayP, rayD;
		ogl->GetCursorRay(p,rayP,rayD);

		m_stroke2d.push_back( p );
		m_stroke3d.push_back( rayP + 0.1f * rayD );
	}
	else 
  {
		ogl->BtnDown_Trans(p);
  }
}


void ModeRefStrokeTrim::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  if( m_b_drawingstroke ) UpdateVolFlgByStroke(ogl);

	ogl->BtnUp();
	m_bL = false;
	m_b_drawingstroke = false;
	m_stroke2d.clear();
	m_stroke3d.clear();
  FormMain_RedrawMainPanel();
}


void ModeRefStrokeTrim::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  if (m_bL && m_b_drawingstroke)
	{
    //cancel current trimming stroke
		m_b_drawingstroke = false;
		m_bL = false;
		m_stroke2d.clear();
		m_stroke3d.clear();
		return;
	}
  else
  {
	  ogl->BtnDown_Rot (p); 
	  m_bR = true ;   
  }
}

void ModeRefStrokeTrim::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  ogl->BtnUp();
  m_bR = false;
  FormMain_RedrawMainPanel();
}

void ModeRefStrokeTrim::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  ogl->BtnDown_Zoom(p); 
  m_bM = true ; 
}

void ModeRefStrokeTrim::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  ogl->BtnUp();
  m_bM = false;
  FormMain_RedrawMainPanel();
}

void ModeRefStrokeTrim::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if( !m_bL && !m_bR && !m_bM ) return;

	if (m_b_drawingstroke)
	{
		EVec3f rayP, rayD;
		ogl->GetCursorRay(p,rayP,rayD);
		m_stroke2d.push_back( p);
		m_stroke3d.push_back( rayP + 0.1f * rayD );
	}
	else
	{
		ogl->MouseMove( p );
	}
  FormMain_RedrawMainPanel();
}


void ModeRefStrokeTrim::MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl)
{
  EVec3f rayP, rayD, pos;
	ogl->GetCursorRay(p, rayP, rayD);
  
  CRSSEC_ID id = PickCrssec(rayP, rayD, &pos);
  if( id != CRSSEC_NON ) CrssecCore::GetInst()->MoveCrssec(ImageCore::GetInst()->GetResolution(), 
                                                           ImageCore::GetInst()->GetPitch(), id, zDelta);
  else ogl->ZoomCam(zDelta * 0.1f);
  FormMain_RedrawMainPanel();
}

void ModeRefStrokeTrim::KeyDown(int nChar) 
{
  if (nChar == 'Z')
	{
    // 変更前の状態を一個だけ持っておく実装（今後複数回のundoに対応したい）
    std::cout<< "undo!!\n";

		OglImage3D &vFlg  = ImageCore::GetInst()->m_vol_flag ;
		EVec3i r = ImageCore::GetInst()->GetResolution();
		memcpy(&vFlg[0], m_vol_prev, sizeof( byte ) * r[0] * r[1] * r[2] );
		vFlg.SetUpdated();

    FormMain_RedrawMainPanel();
	}
}


void ModeRefStrokeTrim::KeyUp(int nChar) {}


void ModeRefStrokeTrim::DrawScene(const EVec3f &cuboid, const EVec3f &cam_pos, const EVec3f &cam_center)
{
  //renderingに必用なパラメータを集めておく
  const EVec3i reso  = ImageCore::GetInst()->GetResolution();
  const bool image_interpolation = formVisParam_doInterpolation();

  if (m_b_drawingstroke && m_stroke3d.size() > 1)
	{
    t_DrawPolyLine( EVec3f(1,1,0), 4, m_stroke3d, true );
	}
	
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
		
 //render cross sections ----------------------------------
  glColor3d(1, 1, 1);
  m_crssec_shader.Bind(0, 1, 2, 3, 6, reso, false, !IsSpaceKeyOn());
  const bool b_xy  = formVisParam_bPlaneXY();
  const bool b_yz  = formVisParam_bPlaneYZ();
  const bool b_zx  = formVisParam_bPlaneZX();
  CrssecCore::GetInst()->DrawCrssec(b_xy, b_yz, b_zx, cuboid);
  m_crssec_shader.Unbind();

	//volume rendering ---------------------------------------
	if ( formVisParam_bRendVol() )
	{
    const float alpha  = formVisParam_getAlpha();
    const bool b_on_manipuration = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    const int  slice_num         = (int)( (b_on_manipuration ? ONMOVE_SLICE_RATE : 1.0 ) * formVisParam_getSliceNum() );

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		m_volume_shader.Bind(0, 1, 2, 3, 4, 5, 6, alpha * 0.1f, reso, cam_pos, false, !IsSpaceKeyOn());
		t_DrawCuboidSlices( slice_num, cam_pos, cam_center, cuboid);
		m_volume_shader.Unbind();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
}






static bool IsInClosedStroke(double x, double y, const vector<EVec2i> &stroke)
{
	//stroke.size() > 3 
	EVec2d d1,d2;
	double sum = 0;

	d1 << stroke.back ()[0] - x, stroke.back() [1] - y;
	d2 << stroke.front()[0] - x, stroke.front()[1] - y;
	sum = t_CalcAngle(d1,d2);
	
	for (int i = 1; i < (int)stroke.size(); ++i)
	{
		d1 << stroke[i-1][0] - x, stroke[i-1][1] - y;
		d2 << stroke[ i ][0] - x, stroke[ i ][1] - y;
		sum += t_CalcAngle(d1,d2);
	}

	return fabs(sum) > M_PI * 1.5;
}



void ModeRefStrokeTrim::UpdateVolFlgByStroke( OglForCLI *ogl)
{
	if( m_stroke2d.size() < 3 ) return;

	OglImage3D  &vol_flg = ImageCore::GetInst()->m_vol_flag ;
	const EVec3i reso    = ImageCore::GetInst()->GetResolution();
	const EVec3f pitch   = ImageCore::GetInst()->GetPitch();
	const int    WH      = reso[0] * reso[1];

	memcpy( m_vol_prev, &vol_flg[0], sizeof( byte ) * reso[0] * reso[1] * reso[2] );

  //get projection information
	if( !ogl->IsDrawing() ) ogl->oglMakeCurrent();

	double model_mat[16], projection_mat[16];
	int viewport[4];
	glGetDoublev(GL_MODELVIEW_MATRIX , model_mat      );
	glGetDoublev(GL_PROJECTION_MATRIX, projection_mat );
	glGetIntegerv(GL_VIEWPORT        , viewport       );
  

  //gen screen_img (binary inside/outside image) 
  std::cout << "get 2D foreground image\n";

	byte *screen_img = new byte[ viewport[2] * viewport[3] ];
	memset( screen_img, 0, sizeof(byte) * viewport[2] * viewport[3] );

	for( auto &p : m_stroke2d) p[1] = viewport[3] - p[1];

	EVec2i bb_min, bb_max;
	t_CalcBoundingBox(m_stroke2d, bb_min, bb_max);
	bb_min[0] = max( 0           , bb_min[0] - 1);
	bb_min[1] = max( 0           , bb_min[1] - 1);
	bb_max[0] = min(viewport[2]-1, bb_max[0] + 1);
	bb_max[1] = min(viewport[3]-1, bb_max[1] + 1);

#pragma omp parallel for
	for ( int y = bb_min[1]; y < bb_max[1]; ++y)
	{
		for ( int x = bb_min[0]; x < bb_max[0]; ++x)
		{
			if ( IsInClosedStroke( x, y, m_stroke2d ) ) 
        screen_img[x + y * viewport[2] ] = 255;
		}
	}


  //compute projection of all foreground voxels
  std::cout << "compute projection of all voxels\n";

#pragma omp parallel for
	for ( int z = 0; z < reso[2]; ++z)
	{
		for( int y = 0 ; y < reso[1]; ++y)
		{
			for ( int x = 0; x < reso[0]; ++x)
			{
				const int I = x + y * reso[0] + z * WH;
				if( vol_flg[I] != 255 ) continue;

        double posx = (x + 0.5) * pitch[0];
        double posy = (y + 0.5) * pitch[1];
        double posz = (z + 0.5) * pitch[2];
				double px, py, pz;
				gluProject(	posx,  posy, posz,  model_mat, projection_mat, viewport, &px, &py, &pz);

				if( 0<= px && px <= viewport[2]-1 && 
					  0<= py && py <= viewport[3]-1 && screen_img[ (int)px + (int)py*viewport[2] ] == 255 ) vol_flg[I] = 1;
			}	
		}
	}


	delete[] screen_img;

	if( !ogl->IsDrawing() ) wglMakeCurrent(NULL,NULL);

	vol_flg.SetUpdated();
  m_b_modified = true;
	
  std::cout << "...done\n";
}




#pragma managed
