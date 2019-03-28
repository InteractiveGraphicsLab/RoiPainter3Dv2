
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

#pragma unmanaged


ModeRefStrokeTrim::~ModeRefStrokeTrim()
{
	if( m_volPrev ) delete[] m_volPrev;
}

ModeRefStrokeTrim::ModeRefStrokeTrim() :
	  m_volumeShader("shader/volVtx.glsl"   , "shader/volFlg_Seg.glsl"),
	  m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg_Seg.glsl")
{
  m_bL = m_bR = m_bM = m_bDrawStrok = false;
  m_bModified = false;

  m_volPrev = 0;
}


bool ModeRefStrokeTrim::canEndMode()
{
  if( !m_bModified ) return true;
	
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
void ModeRefStrokeTrim::startMode()
{
	m_bModified = false;

  m_trgtMskId = formMaskIdSelection_showModalDialog();
  if( m_trgtMskId < 0 ){
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    return;
  }

  if( m_trgtMskId == 0  ){
    CLI_MessageBox_OK_Show("Cant modify mask ID = 0", "message");
    ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
    return;
  }

  formRefStrokeTrim_Show();
  CrssecCore::GetInst()->ClearCurvedCrossec();

  const vector<MaskData> &mask = ImageCore::GetInst()->m_mask_data;
  OglImage3D  &vMask = ImageCore::GetInst()->m_vol_mask  ;
	OglImage3D  &vFlg  = ImageCore::GetInst()->m_vol_flag  ;
	const EVec3i r  =  ImageCore::GetInst()->GetResolution();
	const int    N  =  r[0] * r[1] * r[2];

	for (int i = 0; i < N; ++i) vFlg[i] = ( vMask[i] == m_trgtMskId) ? 255 : 0;
	vFlg.SetUpdated();

	if( m_volPrev ) delete[] m_volPrev;
	m_volPrev = new byte[N];
	memcpy(m_volPrev, &(vMask[0]), sizeof(byte)*N);
	
}




void ModeRefStrokeTrim::cancelSegmentation()
{
  if( m_volPrev ) delete[] m_volPrev;
  m_volPrev = 0;
  ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK );
  formMain_redrawMainPanel();
}


void ModeRefStrokeTrim::finishSegmentation()
{
  vector<MaskData> &mask = ImageCore::GetInst()->m_mask_data;
	OglImage3D      &vMask = ImageCore::GetInst()->m_vol_mask;
	OglImage3D      &vFlg  = ImageCore::GetInst()->m_vol_flag;
	const EVec3i     r     = ImageCore::GetInst()->GetResolution();
	const int        N     = r[0] * r[1] * r[2];


	//一旦 トリムされた画素のIDを0に
	for( int i= 0; i < N; ++i)
	{
		if( vMask[i] == m_trgtMskId && vFlg[i] != 255 ) vMask[i] = 0;
	}
	mask[m_trgtMskId].m_surface.clear();
	mask[m_trgtMskId].m_b_drawsurface = false;

	//トリムされた領域を新しい領域として追加
  bool bTrimRegionExist = false;
	for( int i= 0; i < N; ++i) 
  {
    if( vFlg[i] == 1 ) bTrimRegionExist  = true;
    vFlg[i] = ( vFlg[i] == 1 ) ? 255 : 0;
  }
  if(bTrimRegionExist)
	  ImageCore::GetInst()->StoreForegroundAsNewMask();


	vFlg .SetUpdated();
	vMask.SetUpdated();
	m_bModified = false;

  if( m_volPrev ) delete[] m_volPrev;
  m_volPrev = 0;

	ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK );
	formMain_redrawMainPanel();
}







////////////////////////////////////////////////////////////////////////
//Mouse Listener ///////////////////////////////////////////////////////

void ModeRefStrokeTrim::LBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeRefStrokeTrim::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeRefStrokeTrim::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}





void ModeRefStrokeTrim::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;
	m_stroke2D.clear();
	m_stroke3D.clear();

	if (isShiftKeyOn())
	{
		m_bDrawStrok = true;
		EVec3f rayP, rayD;
		ogl->GetCursorRay(p,rayP,rayD);

		m_stroke2D.push_back( p );
		m_stroke3D.push_back( rayP + 0.1f * rayD );
	}
	else 
  {
		ogl->BtnDown_Trans(p);
  }
}


void ModeRefStrokeTrim::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  if( m_bDrawStrok ) updateVolFlgByStroke(ogl);

	ogl->BtnUp();
	m_bL = false;
	m_bDrawStrok = false;
	m_stroke2D.clear();
	m_stroke3D.clear();
  formMain_redrawMainPanel();
}


void ModeRefStrokeTrim::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  if (m_bL && m_bDrawStrok)
	{
    //cancel current trimming stroke
		m_bDrawStrok = false;
		m_bL = false;
		m_stroke2D.clear();
		m_stroke3D.clear();
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
  formMain_redrawMainPanel();
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
  formMain_redrawMainPanel();
}

void ModeRefStrokeTrim::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if( !m_bL && !m_bR && !m_bM ) return;

	if (m_bDrawStrok)
	{
		EVec3f rayP, rayD;
		ogl->GetCursorRay(p,rayP,rayD);
		m_stroke2D.push_back( p);
		m_stroke3D.push_back( rayP + 0.1f * rayD );
	}
	else
	{
		ogl->MouseMove( p );
	}
  formMain_redrawMainPanel();
}


void ModeRefStrokeTrim::MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl)
{
  EVec3f rayP, rayD, pos;
	ogl->GetCursorRay(p, rayP, rayD);
  
  CRSSEC_ID id = pickCrsSec(rayP, rayD, &pos);
  if( id != CRSSEC_NON ) CrssecCore::GetInst()->MoveCrssec(ImageCore::GetInst()->GetResolution(), 
                                                           ImageCore::GetInst()->GetPitch(), id, zDelta);
  else ogl->ZoomCam(zDelta * 0.1f);
  formMain_redrawMainPanel();
}

void ModeRefStrokeTrim::keyDown(int nChar) 
{
  if (nChar == 'Z')
	{
    // 変更前の状態を一個だけ持っておく実装（今後複数回のundoに対応したい）
    printf( "undo!!");

		OglImage3D &vFlg  = ImageCore::GetInst()->m_vol_flag ;
		EVec3i r = ImageCore::GetInst()->GetResolution();
		memcpy(&vFlg[0], m_volPrev, sizeof( byte ) * r[0] * r[1] * r[2] );
		vFlg.SetUpdated();

    formMain_redrawMainPanel();
	}
}


void ModeRefStrokeTrim::keyUp(int nChar) {}


void ModeRefStrokeTrim::drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF)
{
  const bool   bXY      = formVisParam_bPlaneXY();
  const bool   bYZ      = formVisParam_bPlaneYZ();
  const bool   bZX      = formVisParam_bPlaneZX();
  const bool isOnManip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
  const int  sliceN    = (int)( (isOnManip ? ONMOVE_SLICE_RATE : 1.0 ) * formVisParam_getSliceNum() );
  const float alpha    = formVisParam_getAlpha();
  const EVec3i reso     = ImageCore::GetInst()->GetResolution();

  if (m_bDrawStrok && m_stroke3D.size() > 1)
	{
    m_stroke3D.push_back(m_stroke3D.front());
    t_drawLineStrip( EVec3f(1,1,0), 4, m_stroke3D );
    m_stroke3D.pop_back();
	}
	
	//bind volumes ---------------------------------------
	glActiveTextureARB(GL_TEXTURE0);
	ImageCore::GetInst()->m_vol.BindOgl();
	glActiveTextureARB(GL_TEXTURE1);
	ImageCore::GetInst()->m_vol_gm.BindOgl();
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
  m_crssecShader.bind(0, 1, 2, 3, 6, reso, false, !isSpaceKeyOn());
  CrssecCore::GetInst()->DrawCrssec(bXY, bYZ, bZX, cuboid);
  m_crssecShader.unbind();

	//volume rendering ---------------------------------------
	if ( formVisParam_bRendVol() )
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		m_volumeShader.bind(0, 1, 2, 3, 4, 5, 6, alpha * 0.1f, reso, camP, false, !isSpaceKeyOn());
		t_DrawCuboidSlices( sliceN, camP,camF, cuboid);
		m_volumeShader.unbind();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
}




#pragma managed





static double calcAngle(const EVec2d &d1, const EVec2d &d2)
{
	double l = d1.norm() * d2.norm();
	if( l == 0 ) return 0;

	double cosT = t_crop<double>(-1., 1., ( d1.dot(d2) ) / l);

	if( d1[0] * d2[1] - d1[1] * d2[0] >= 0)
    return  acos(cosT);
	else
    return -acos(cosT);
}


static bool isInClosedStroke(double x, double y, const vector<EVec2i> &stroke)
{
	//stroke.size() > 3 
	EVec2d d1,d2;
	double sum = 0;

	d1 << stroke.back()[0] - x, stroke.back()[1] - y;
	d2 << stroke[0]    [0] - x, stroke[0]    [1]- y;
	sum = calcAngle(d1,d2);
	
	for (int i = 1; i < (int)stroke.size(); ++i)
	{
		d1 << stroke[i-1][0] - x, stroke[i-1][1] - y;
		d2 << stroke[ i ][0] - x, stroke[ i ][1] - y;
		sum += calcAngle(d1,d2);
	}

	return fabs(sum) > M_PI * 1.5;
}



void ModeRefStrokeTrim::updateVolFlgByStroke( OglForCLI *ogl)
{
	if( m_stroke2D.size() < 3 ) return;

	
	OglImage3D  &vFlg  = ImageCore::GetInst()->m_vol_flag ;
	const EVec3i r     = ImageCore::GetInst()->GetResolution();
	const EVec3f pitch = ImageCore::GetInst()->GetPitch();
	const int    WH    = r[0] * r[1];

	memcpy( m_volPrev, &vFlg[0], sizeof( byte ) * r[0] * r[1] * r[2] );

  //get projection information
	if( !ogl->isDrawing() ) ogl->oglMakeCurrent();
	double model[16], proj[16];
	int vp[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, model);
	glGetDoublev(GL_PROJECTION_MATRIX, proj);
	glGetIntegerv(GL_VIEWPORT, vp);
  

  //gen 2D in/out image 
  printf("get 2D foreground image\n");
	byte *imgInOut = new byte[ vp[2] * vp[3] ];
	memset( imgInOut, 0, sizeof(byte) * vp[2] * vp[3] );

	for( auto &p : m_stroke2D) p[1] = vp[3] - p[1];

	EVec2i minBB, maxBB;
	t_calcBoundBox2D(m_stroke2D, minBB, maxBB);
	minBB[0] = max( 0     , minBB[0] - 1);
	minBB[1] = max( 0     , minBB[1] - 1);
	maxBB[0] = min(vp[2]-1, maxBB[0] + 1);
	maxBB[1] = min(vp[3]-1, maxBB[1] + 1);

#pragma omp parallel for
	for (int y = minBB[1]; y < maxBB[1]; ++y)
	{
		for (int x = minBB[0]; x < maxBB[0]; ++x)
		{
			imgInOut[x + y * vp[2] ] = isInClosedStroke( x, y, m_stroke2D ) ? 255 : 0;
		}
	}


  //compute projection of all foreground voxels
  printf("compute projection of all voxels\n");
#pragma omp parallel for
	for (int z = 0; z < r[2]; ++z)
	{
		for( int y = 0 ; y < r[1]; ++y)
		{
			for (int x = 0; x < r[0]; ++x)
			{
				const int I = x + y * r[0] + z * WH;
				if( vFlg[I] != 255 ) continue;

				double px, py, pz;
				gluProject(	(x+0.5)*pitch[0],  (y+0.5)*pitch[1],  (z+0.5)*pitch[2],  model, proj, vp, &px,&py,&pz);

				if( 0<= px && px <= vp[2]-1 && 
					  0<= py && py <= vp[3]-1 && imgInOut[ (int)px + (int)py*vp[2] ] == 255 ) vFlg[I] = 1;
			}	
		}
	}

	delete[] imgInOut;

	printf( "...done\n");

	if( !ogl->isDrawing() ) wglMakeCurrent(NULL,NULL);

	vFlg.SetUpdated();
	m_bModified = true;
}
