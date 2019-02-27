#include "ModeSegRGrow.h"

#include <iostream>

#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegRGrow.h"

#include "climessagebox.h"
#include "./COMMON/tqueue.h"

using namespace RoiPainter3D;


#pragma unmanaged


// usage of "volFlg" 
//  0 : non target voxels (never change )
//  1 : target & back ground voxels 
// 255: target & fore ground voxels


ModeSegRGrow::ModeSegRGrow() :
  m_volumeShader("shader/volVtx.glsl"   , "shader/volFlg_Seg.glsl"),
  m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg_Seg.glsl")
{
	m_bL = m_bR = m_bM = false;
	m_dragCpId  = -1;
  m_CpSize    = 0.1f;
	m_bRegionUpdated = false;

  m_bDrawStr = false;
}



ModeSegRGrow::~ModeSegRGrow()
{
}



bool ModeSegRGrow::canEndMode()
{
	if( !m_bRegionUpdated) return true;

	if ( CLI_MessageBox_YESNO_Show("Current Result is not stored. \nDo you want to leave?" ,"caution") )
	{
		return true;
	}
	return false;
}



void ModeSegRGrow::startMode()
{
  //prepare flg volume (consider the Locked mask Ids)
	vector<MaskData> &mask = ImageCore::getInst()->m_maskData;
	OglImage3D      &vMask = ImageCore::getInst()->m_volMsk  ;
	OglImage3D      &vFlg  = ImageCore::getInst()->m_volFlg  ;
	EVec3i           r     = ImageCore::getInst()->getResolution();
	EVec2i          vMinMax= ImageCore::getInst()->getVolMinMax();

	const int N = r[0] * r[1] * r[2];
	for (int i = 0; i < N; ++i) vFlg[i] = ( mask[vMask[i]].lock ) ? 0 : 1;
	vFlg.SetUpdated();

	m_CPs.clear();
	m_CpSize = ImageCore::getInst()->getPitchW() * 1;
	m_CpSph.initializeIcosaHedron( m_CpSize );

  formSegRGrow_Show();
  formSegRGrow_InitAllItems(vMinMax[0],vMinMax[1]);
}



//////////////////////////////////////////////////////////
// Mouse Listener (Camera manipuration & CP manipuration)


void ModeSegRGrow::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  EVec3f rayP, rayD, cuboid = ImageCore::getInst()->getCuboidF();
	ogl->GetCursorRay(p, rayP, rayD);
  m_bL = true;

  if (isCtrKeyOn())
  {
    m_stroke.clear();
    m_bDrawStr = true;
  }
  else if( (m_dragCpId = pickCPs(rayP, rayD) ) != -1 ) 
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
  if (m_bDrawStr)
  {
    EVec3f cube = ImageCore::getInst()->getCuboidF();
    CrssecCore::getInst()->GenerateCurvedCrssec(cube, ogl->GetCamPos(), m_stroke);
  }

  m_bL = m_bDrawStr = false;
	m_dragCpId = -1;
	ogl->BtnUp();
  formMain_redrawMainPanel();
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
  formMain_redrawMainPanel();
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
  formMain_redrawMainPanel();
}


void ModeSegRGrow::LBtnDclk(const EVec2i &p, OglForCLI *ogl)
{
  EVec3f rayP, rayD, pos;
	ogl->GetCursorRay(p, rayP, rayD);

	int cpid = pickCPs(rayP, rayD);
	if (cpid != -1)
	{
		m_CPs.erase(m_CPs.begin() + cpid);
	}
  else
  {
	  CRSSEC_ID id = pickCrsSec(rayP, rayD, &pos);
	  if (id != CRSSEC_NON) m_CPs.push_back( pos );
  }
  formMain_redrawMainPanel();
}


void ModeSegRGrow::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegRGrow::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}



void ModeSegRGrow::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if ( !m_bL && !m_bR && !m_bM) return;

  EVec3f rayP, rayD, pos;
  ogl->GetCursorRay(p, rayP, rayD);

  if (m_bDrawStr)
  {
    m_stroke.push_back(rayP + 0.1f * rayD);
    formMain_redrawMainPanel();
  }
  else if (m_dragCpId != -1)
	{
		CRSSEC_ID id = pickCrsSec(rayP, rayD, &pos);
		if (id != CRSSEC_NON) m_CPs[m_dragCpId] = pos;
	}
	else 
	{
		ogl->MouseMove(p);
	}

  formMain_redrawMainPanel();
}


void ModeSegRGrow::MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl)
{
  EVec3f rayP, rayD, pos;
  ogl->GetCursorRay(p, rayP, rayD);
  EVec3i reso  = ImageCore::getInst()->getResolution();
  EVec3f pitch = ImageCore::getInst()->getPitch();


  CRSSEC_ID id = pickCrsSec(rayP, rayD, &pos);
  if( id != CRSSEC_NON ) CrssecCore::getInst()->MoveCrssec(reso, pitch, id, zDelta);
  else ogl->ZoomCam(zDelta * 0.1f);

  formMain_redrawMainPanel();
}



int ModeSegRGrow::pickCPs(const EVec3f &rayP, const EVec3f &rayD)
{
	for (int i = 0; i < (int)m_CPs.size(); ++i)
	{
		if (t_distRayToPoint(rayP, rayD, m_CPs[i]) < m_CpSize) return i;
	}
	return -1;
}







void ModeSegRGrow::keyDown(int nChar) 
{
  formMain_redrawMainPanel();
}
void ModeSegRGrow::keyUp(int nChar) 
{
  formMain_redrawMainPanel();
}


void ModeSegRGrow::drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF)
{
  const bool bXY = formVisParam_bPlaneXY();
  const bool bYZ = formVisParam_bPlaneYZ();
  const bool bZX = formVisParam_bPlaneZX();
  const bool   bDrawVol = formVisParam_bRendVol();
  const bool   bGradMag = formVisParam_bGradMag();
  const bool   bPsuedo  = formVisParam_bDoPsued();
  const float  alpha    = formVisParam_getAlpha();
  const EVec3i reso     = ImageCore::getInst()->getResolution();

  const bool isOnManip  = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
  const int  sliceN     = (int)((isOnManip ? ONMOVE_SLICE_RATE : 1.0) * formVisParam_getSliceNum());

  //bind volumes ---------------------------------------
  glActiveTextureARB(GL_TEXTURE0);
  ImageCore::getInst()->m_vol.BindOgl();
  glActiveTextureARB(GL_TEXTURE1);
  ImageCore::getInst()->m_volGmag.BindOgl();
  glActiveTextureARB(GL_TEXTURE2);
  ImageCore::getInst()->m_volFlg.BindOgl(false);
  glActiveTextureARB(GL_TEXTURE3);
  ImageCore::getInst()->m_volMsk.BindOgl(false);
  glActiveTextureARB(GL_TEXTURE4);
  formVisParam_bindTfImg();
  glActiveTextureARB(GL_TEXTURE5);
  formVisParam_bindPsuImg();
  glActiveTextureARB(GL_TEXTURE6);
  ImageCore::getInst()->m_imgMskCol.BindOgl(false);

  //draw cut stroke 
  if (m_bDrawStr) t_drawLineStrip(EVec3f(1,1,0), 3, m_stroke);

  //draw planes
  glColor3d(1, 1, 1);
  m_crssecShader.bind(0, 1, 2, 3, 6, reso, false, !isSpaceKeyOn());
  CrssecCore::getInst()->DrawCrssec(bXY, bYZ, bZX, cuboid);
  m_crssecShader.unbind();


  if (bDrawVol && !isSpaceKeyOn())
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    m_volumeShader.bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, camP, bPsuedo, !isSpaceKeyOn());
    t_drawSlices(sliceN, camP, camF, cuboid);
    m_volumeShader.unbind();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }

  //draw control points
  const static float diff[4] = { 1   ,0,0,0 };
  const static float ambi[4] = { 0.5f,0,0,0 };
  const static float spec[4] = { 1   ,1,1,0 };
  const static float shin[1] = { 56.0f };
  glEnable(GL_LIGHTING);
  for (const auto& it : m_CPs) 
  {
    glTranslated( it[0], it[1], it[2]);
    m_CpSph.draw(diff, ambi, spec, shin);
    glTranslated(-it[0], -it[1], -it[2]);
  }
  glDisable(GL_LIGHTING);
}




void ModeSegRGrow::runThresholding(short minV, short maxV)
{
	short       *vol    = ImageCore::getInst()->m_volOrig;
	OglImage3D  &volFlg = ImageCore::getInst()->m_volFlg ;
	const EVec3i res    = ImageCore::getInst()->getResolution();

	const int    N = res[0] * res[1] * res[2];

	for (int i = 0; i < N; ++i) if (volFlg[i] != 0)
	{
		volFlg[i] = (minV <= vol[i] && vol[i] <= maxV) ? 255 : 1;
	}

	volFlg.SetUpdated();
	m_bRegionUpdated = true;
  formMain_redrawMainPanel();
}



void ModeSegRGrow::runRegionGrow6(short minV, short maxV)
{
	std::cout << "runRegionGrow6...";
	const short  *vO   = ImageCore::getInst()->m_volOrig;
	OglImage3D   &vF   = ImageCore::getInst()->m_volFlg ; 
	const EVec3i  reso = ImageCore::getInst()->getResolution();
  const EVec3f  pitch= ImageCore::getInst()->getPitch();

	const int W = reso[0];
	const int H = reso[1];
	const int D = reso[2], WH = W*H, WHD = W*H*D;

	for (int i = 0; i < WHD; ++i) vF[i] = ( vF[i] == 0) ? 0 : 1;

	//CP --> pixel id
	//volFlg : 0:never change, 1:back, 255:fore
	TQueue<EVec4i> Q;
	for ( const auto cp : m_CPs)
	{
		const int x = min(W-1, (int)( cp[0] / pitch[0] ) ) ;
		const int y = min(H-1, (int)( cp[1] / pitch[1] ) ) ;
		const int z = min(D-1, (int)( cp[2] / pitch[2] ) ) ;
		const int I = x + y*W + z*WH;
		if ( vF[I] != 0 && minV <= vO[I] && vO[I] <= maxV)
		{
			Q.push_back(EVec4i(x, y, z, I));
			vF[I] = 255;
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
		K = I- 1; if (x > 0  && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x-1, y, z, K)); }
		K = I- W; if (y > 0  && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x, y-1, z, K)); }
		K = I-WH; if (z > 0  && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x, y, z-1, K)); }
		K = I+ 1; if (x <W-1 && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x+1, y, z, K)); }
		K = I+ W; if (y <H-1 && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x, y+1, z, K)); }
		K = I+WH; if (z <D-1 && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x, y, z+1, K)); }
	}
	
  vF.SetUpdated();
	m_bRegionUpdated = true;
  formMain_redrawMainPanel();

	std::cout << "runRegionGrow6...DONE\n\n";

}


void ModeSegRGrow::runRegionGrow26(short minV, short maxV)
{
  std::cout << "runRegionGrow26...";
	const short  *vO   = ImageCore::getInst()->m_volOrig;
	OglImage3D   &vF   = ImageCore::getInst()->m_volFlg ; 
	const EVec3i  reso = ImageCore::getInst()->getResolution();
  const EVec3f  pitch= ImageCore::getInst()->getPitch();

	const int W = reso[0];
	const int H = reso[1];
	const int D = reso[2], WH = W*H, WHD = W*H*D;

	for (int i = 0; i < WHD; ++i) vF[i] = ( vF[i] == 0) ? 0 : 1;

	//CP --> pixel id
	//volFlg : 0:never change, 1:back, 255:fore
	TQueue<EVec4i> Q;
	for ( const auto cp : m_CPs)
	{
		const int x = min(W-1, (int)( cp[0] / pitch[0] ) ) ;
		const int y = min(H-1, (int)( cp[1] / pitch[1] ) ) ;
		const int z = min(D-1, (int)( cp[2] / pitch[2] ) ) ;
		const int I = x + y*W + z*WH;
		if ( vF[I] != 0 && minV <= vO[I] && vO[I] <= maxV)
		{
			Q.push_back(EVec4i(x, y, z, I));
			vF[I] = 255;
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
    K = I-1-W-WH; if (x> 0 &&y> 0 &&z> 0 && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x-1, y-1, z-1, K)); }
    K = I  -W-WH; if (       y> 0 &&z> 0 && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x  , y-1, z-1, K)); }
    K = I+1-W-WH; if (x<W-1&&y> 0 &&z> 0 && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x+1, y-1, z-1, K)); }
    K = I-1  -WH; if (x> 0        &&z> 0 && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x-1, y  , z-1, K)); }
    K = I    -WH; if (              z> 0 && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x  , y  , z-1, K)); }
    K = I+1  -WH; if (x<W-1       &&z> 0 && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x+1, y  , z-1, K)); }
    K = I-1+W-WH; if (x> 0 &&y<H-1&&z> 0 && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x-1, y+1, z-1, K)); }
    K = I  +W-WH; if (       y<H-1&&z> 0 && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x  , y+1, z-1, K)); }
    K = I+1+W-WH; if (x<W-1&&y<H-1&&z> 0 && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x+1, y+1, z-1, K)); }

    K = I-1-W   ; if (x> 0 &&y> 0        && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x-1, y-1, z  , K)); }
    K = I  -W   ; if (       y> 0        && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x  , y-1, z  , K)); }
    K = I+1-W   ; if (x<W-1&&y> 0        && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x+1, y-1, z  , K)); }
    K = I-1     ; if (x> 0               && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x-1, y  , z  , K)); }
  //K = I       ; if (                   && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x  , y  , z  , K)); }
    K = I+1     ; if (x<W-1              && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x+1, y  , z  , K)); }
    K = I-1+W   ; if (x> 0 &&y<H-1       && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x-1, y+1, z  , K)); }
    K = I  +W   ; if (       y<H-1       && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x  , y+1, z  , K)); }
    K = I+1+W   ; if (x<W-1&&y<H-1       && vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x+1, y+1, z  , K)); }

    K = I-1-W+WH; if (x> 0 &&y> 0 &&z<D-1&& vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x-1, y-1, z+1, K)); }
    K = I  -W+WH; if (       y> 0 &&z<D-1&& vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x  , y-1, z+1, K)); }
    K = I+1-W+WH; if (x<W-1&&y> 0 &&z<D-1&& vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x+1, y-1, z+1, K)); }
    K = I-1  +WH; if (x> 0        &&z<D-1&& vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x-1, y  , z+1, K)); }
    K = I    +WH; if (              z<D-1&& vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x  , y  , z+1, K)); }
    K = I+1  +WH; if (x<W-1       &&z<D-1&& vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x+1, y  , z+1, K)); }
    K = I-1+W+WH; if (x> 0 &&y<H-1&&z<D-1&& vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x-1, y+1, z+1, K)); }
    K = I  +W+WH; if (       y<H-1&&z<D-1&& vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x  , y+1, z+1, K)); }
    K = I+1+W+WH; if (x<W-1&&y<H-1&&z<D-1&& vF[K] == 1 && minV <= vO[K] && vO[K] <= maxV) { vF[K] = 255; Q.push_back(EVec4i(x+1, y+1, z+1, K)); }
	}
	
  vF.SetUpdated();
	m_bRegionUpdated = true;
  formMain_redrawMainPanel();

	std::cout << "runRegionGrow26...DONE\n\n";
}






void ModeSegRGrow::runDilation()
{
	t_morpho3D_dilate(ImageCore::getInst()->m_volFlg);
	ImageCore::getInst()->m_volFlg.SetUpdated();
	m_bRegionUpdated = true;
  formMain_redrawMainPanel();
}

void ModeSegRGrow::runErosion()
{
	t_morpho3D_erode(ImageCore::getInst()->m_volFlg);
	ImageCore::getInst()->m_volFlg.SetUpdated();
	m_bRegionUpdated = true;
  formMain_redrawMainPanel();
}


void ModeSegRGrow::runFillHole()
{
  //compute fill hole
	OglImage3D flg(ImageCore::getInst()->m_volFlg);
	const int N = flg.GetW() * flg.GetH() * flg.GetD();
	for (int i = 0; i < N; ++i) flg[i] = (flg[i] == 255) ? 255 : 0;
	t_morpho3D_FillHole(flg);

  // update flg volume (never change voxel with vflg[i]==0)
	OglImage3D &trgt = ImageCore::getInst()->m_volFlg;

	for (int i = 0; i < N; ++i) if (trgt[i] == 1)
	{
		trgt[i] = (flg[i] == 255) ? 255 : 1;
	}

	trgt.SetUpdated();
	m_bRegionUpdated = true;
  formMain_redrawMainPanel();
}



void ModeSegRGrow::finishSegmentationStoreRegion()
{
	const EVec3i res = ImageCore::getInst()->getResolution();
	const int    N   = res[0] * res[1] * res[2];

	bool bForeExist = false;
	for (int i = 0; i < N; ++i)
	{
		if ( ImageCore::getInst()->m_volFlg[i] == 255)
		{
			bForeExist = true;
			break;
		}
	}

	if (!bForeExist) 
	{
		CLI_MessageBox_OK_Show("No foreground voxel exist\n", "caution");
		return;
	}

	ImageCore::getInst()->mask_storeCurrentForeGround();
	m_bRegionUpdated = false;
	ModeCore::getInst()->ModeSwitch( MODE_VIS_MASK );
}


/*
todo 
test thresholding
implement 26
test mask IO
test mask vis mode
implement graphcut
*/












#pragma managed

