
#include "ModeSegGCut.h"

#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"
#include "climessagebox.h"

#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegGCut.h"

#include <time.h>
#include <process.h>
#include <iostream>

#include "../3rdparty/riken/TMaxFlow_BK4.h"
#include "../3rdParty\riken\TWatershedEx.h"

#ifndef FOREBACK_MAX 
#define FOREBACK_MAX 1000000000.0
#endif 


using namespace RoiPainter3D;

#pragma unmanaged




static bool t_loadWsdLabel ( const string fname,  const EVec3i &reso, vector<int> &map_vox2wsd );
static void t_constructWsdNodesFromLabel( const int W, const int H, const int D, 
                                          const vector<int> &vLabel, const short *vol, 
                                          vector< WsdNode> &wsdNodes, vector<set<int>> &wsdNodeNei);


ModeSegGCut::~ModeSegGCut()
{

}

ModeSegGCut::ModeSegGCut() :
	m_volumeShader("shader/volVtx.glsl"   , "shader/volFlg_Seg.glsl"   ),
	m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg_Seg.glsl")
{
  std::cout << "ModeSegGCut constructor start\n";

  m_bWsdInitialized = false;
  m_bWsdComputing   = false;


  std::cout << "ModeSegGCut constructor done \n";
}


// flg 0: back
//     1: trgt & fore
//   255: trgt & back

bool ModeSegGCut::canEndMode()
{
	if ( (m_fCPs.size() || m_bCPs.size() ) && !CLI_MessageBox_YESNO_Show("Current Result is not stored. \nDo you want to leave?", "caution") )
	{
		return false;
	}
	return true;
}


void ModeSegGCut::startMode()
{
  //initialize flg volume --------------
	const EVec3i r = ImageCore::getInst()->getResolution();
  vector<MaskData> &mask = ImageCore::getInst()->m_maskData;
	OglImage3D      &vMask = ImageCore::getInst()->m_volMsk  ;
	OglImage3D      &vFlg  = ImageCore::getInst()->m_volFlg  ; 
	const int WHD = r[0] * r[1] * r[2];
	for (int i = 0; i < WHD; ++i) vFlg[i] = ( mask[vMask[i]].lock ) ? 0 : 1;
	vFlg.SetUpdated();

  //initialize control points ----------
	m_fCPs.clear();
	m_bCPs.clear();
	m_CpSize = (float) ImageCore::getInst()->getPitchW() * 2;
	m_CpMesh.initializeIcosaHedron( m_CpSize );

  formSegGCut_Show();

	//compute watershad ------------------
	if( !m_bWsdInitialized )
  {
    //backup fileの読み込みを試す
	  string backUpFilePath = ImageCore::getInst()->getFilePath() + ".RpWsdPre";
	  if( t_loadWsdLabel( backUpFilePath, r, m_map_vox2wsd) )
	  {
		  const short *vol = ImageCore::getInst()->m_volOrig;
		  t_constructWsdNodesFromLabel( r[0], r[1], r[2], m_map_vox2wsd, vol, m_wsdNodes, m_wsdNodeNei);
		  m_bWsdInitialized = true;
		  return;
	  }

	  //threadを起動しwatershedを計算する
	  m_bWsdComputing = true;
    _beginthread( ModeSegGCut::initWsdNodes_thread, 0, (LPVOID)this); 
  }

}


void ModeSegGCut::finishSegm ()
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
		CLI_MessageBox_OK_Show("No foreground pixel exist\n", "caution");
		return;
	}

	//CPを削除（しないと、canEndModeでダイアログが表示される)
	m_fCPs.clear();
	m_bCPs.clear();

	ImageCore::getInst()->mask_storeCurrentForeGround();
	ModeCore ::getInst()->ModeSwitch( MODE_VIS_MASK );
	formMain_redrawMainPanel();	
}


void ModeSegGCut::cancelSegm ()
{
  //CPを削除（しないと、canEndModeでダイアログが表示される)
	m_fCPs.clear();
	m_bCPs.clear();
	ModeCore::getInst()->ModeSwitch( MODE_VIS_MASK );
	formMain_redrawMainPanel();	
}

void ModeSegGCut::clearAllCPs()
{
  m_fCPs.clear();
	m_bCPs.clear();
	formMain_redrawMainPanel();	
}





////////////////////////////////////////////////////////////////////
//MOUSE LISTENER 

void ModeSegGCut::LBtnDown(const EVec2i &p, OglForCLI *ogl) {
  m_bL = true;
	m_stroke.clear();
	if (     isShiftKeyOn()) m_bPaintCP = true;
	else if (isCtrKeyOn()  ) m_bDrawCutStr = true;
	else                     ogl->BtnDown_Trans(p);
}

void ModeSegGCut::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{  
  if (m_bDrawCutStr) CrssecCore::getInst()->GenerateCurvedCrssec( ImageCore::getInst()->getCuboidF(), ogl->GetCamPos(), m_stroke );
	m_bPaintCP = m_bDrawCutStr = m_bL = false;
	ogl->BtnUp();
  formMain_redrawMainPanel();
}

void ModeSegGCut::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  if( isShiftKeyOn() ) m_bPaintCP = true;
	else                 ogl->BtnDown_Rot(p);
	m_bR = true;
}

void ModeSegGCut::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = m_bPaintCP = false;
  ogl->BtnUp();
  formMain_redrawMainPanel();
}

void ModeSegGCut::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  if( isShiftKeyOn() ) m_bPaintCP = true;
	else                 ogl->BtnDown_Zoom(p);
	m_bM = true;
}

void ModeSegGCut::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = m_bPaintCP = false;
	ogl->BtnUp();
  formMain_redrawMainPanel();
}


void ModeSegGCut::LBtnDclk(const EVec2i &p, OglForCLI *ogl){}
void ModeSegGCut::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegGCut::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}


void ModeSegGCut::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if ( !m_bL && !m_bR && !m_bM)  return;

	EVec3f rayP, rayD, pos;
	ogl->GetCursorRay( p, rayP, rayD);

	if (m_bDrawCutStr)
	{
		m_stroke.push_back( rayP + 0.1f * rayD );
	}
	else if (m_bPaintCP && m_bM)
	{
		for (int i = 0; i < (int)m_fCPs.size(); ++i)
		{
			if (t_distRayToPoint(rayP, rayD, m_fCPs[i].m_pos) < m_CpSize) m_fCPs.erase( m_fCPs.begin() + i );
		}
		for (int i = 0; i < (int)m_bCPs.size(); ++i)
		{
			if (t_distRayToPoint(rayP, rayD, m_bCPs[i].m_pos) < m_CpSize) m_bCPs.erase( m_bCPs.begin() + i );
		}
	}
	else if( m_bPaintCP && (m_bL || m_bR) )
	{
		if (pickCrsSec( rayP, rayD, &pos) != CRSSEC_NON )
		{
      vector<GCutCp> &cps =  m_bL ? m_fCPs : m_bCPs;

			if( cps.empty() || ( pos - cps.back().m_pos).norm() > m_CpSize * 3 )
			{
				EVec4i vi = ImageCore::getInst()->getVoxelIndex4i(pos);
				cps.push_back( GCutCp( pos, vi) );	
			}
		}
	}
	else
	{
		ogl->MouseMove(p);
	}
  formMain_redrawMainPanel();
}


void ModeSegGCut::MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl)
{
  EVec3f rayP, rayD, pos;
	ogl->GetCursorRay(p, rayP, rayD);

  
  CRSSEC_ID id = pickCrsSec(rayP, rayD, &pos);
  if( id != CRSSEC_NON ) CrssecCore::getInst()->MoveCrssec(ImageCore::getInst()->getResolution(), 
                                                           ImageCore::getInst()->getPitch(), id, zDelta);
  else ogl->ZoomCam(zDelta * 0.1f);

  formMain_redrawMainPanel();
}



void ModeSegGCut::keyDown(int nChar) {
  formMain_redrawMainPanel();
}
void ModeSegGCut::keyUp(int nChar) {
  formMain_redrawMainPanel();
}


void ModeSegGCut::drawScene (const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF )
{
  
  const bool   bXY      = formVisParam_bPlaneXY();
  const bool   bYZ      = formVisParam_bPlaneYZ();
  const bool   bZX      = formVisParam_bPlaneZX();
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

	//render cross sections ----------------------------------
  glColor3d(1, 1, 1);
  m_crssecShader.bind(0, 1, 2, 3, 6, reso, false, !isSpaceKeyOn());
  CrssecCore::getInst()->DrawCrssec(bXY, bYZ, bZX, cuboid);
  m_crssecShader.unbind();


	//volume rendering ---------------------------------------
	if ( bDrawVol && !isSpaceKeyOn())
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		m_volumeShader.bind(0, 1, 2, 3, 4, 5, 6, alpha * 0.1f, reso, camP, bPsuedo, !isSpaceKeyOn() );
		t_drawSlices(sliceN, camP, camF, cuboid);
		m_volumeShader.unbind();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}



	const static float diffR[4] = {1   ,0,0,0}, diffB[4] = {0.3f,0.3f,1,1};
	const static float ambiR[4] = {0.5f,0,0,0}, ambiB[4] = {0.3f,0.3f,0.8f,1};
	const static float  spec[4] = {1   ,1,1,0};
	const static float  shin[1] = {56.0f};
	glEnable(GL_LIGHTING);
	for (const auto& it: m_fCPs )
	{
		glTranslated( it.m_pos[0], it.m_pos[1], it.m_pos[2]);
		m_CpMesh.draw(diffR, ambiR, spec, shin);
		glTranslated(-it.m_pos[0],-it.m_pos[1],-it.m_pos[2]);
	}
	for (const auto& it: m_bCPs )
	{
		glTranslated( it.m_pos[0], it.m_pos[1], it.m_pos[2]);
		m_CpMesh.draw(diffB, ambiB, spec, shin);
		glTranslated(-it.m_pos[0],-it.m_pos[1],-it.m_pos[2]);
	}
	glDisable(GL_LIGHTING);


    //draw cut stroke 
  if (m_bDrawCutStr) t_drawLineStrip(EVec3f(1,1,0), 3, m_stroke);

}






//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//Watershed segmentation//////////////////////////////////////////////////////////////

static bool t_loadWsdLabel ( 
  const string fname,  
  const EVec3i &reso, 
  vector<int> &map_vox2wsd )
{
	if( fname.length() <= 9 ) return false;
	const int WHD = reso[0] * reso[1] * reso[2];

	FILE *fp = fopen( fname.c_str(), "rb" );
	if( fp == 0 ) return false;

	//check volume size	
	int volumeN; 
	fread( &volumeN, sizeof(int),1, fp );
	if( volumeN != WHD ) 
	{ 
		fclose( fp); 
		return false;
	}

	//check user's intension
	std::cout << "fname: " << fname.c_str() << "\n";

	if(!CLI_MessageBox_YESNO_Show( "watershedの前計算ファイルが利用できます。利用しますか？", "Use precompute file?") )
	{
		fclose( fp); 
		return false;
	}

	//label 
	map_vox2wsd.resize( volumeN );
	for( int i=0; i<WHD; ++i) fread( &map_vox2wsd[i], sizeof(int), 1, fp );
	fclose( fp );

	return true;
}


static bool t_saveWsdLabel( const string fname, const EVec3i &reso, const vector<int> &map_vox2wsd )
{
	const int WHD = reso[0] * reso[1] * reso[2];
	
	FILE *fp = fopen( fname.c_str(), "wb" );
	if( fp == 0 ) return false;

	int arraySize = WHD ; 
	fwrite( &arraySize, sizeof(int),1, fp );
	for( int i=0; i<WHD; ++i) fwrite( &map_vox2wsd[i], sizeof(int), 1, fp );
	fclose( fp );
	return true;
}


void t_constructWsdNodesFromLabel
(
	const int W,
	const int H, 
	const int D,
	const vector<int> &vLabel   ,
	const short       *vol      ,

	vector< WsdNode> &wsdNodes  ,
	vector<set<int>> &wsdNodeNei
)
{
	std::cout << "t_constructWsdNodesFromLabel start......";

	time_t t0 = clock();

	const int WH = W*H;
	const int WHD =W*H*D;

	int maxLabel = 0;
	for( int i= 0; i < WHD; ++i ) maxLabel = max( maxLabel, vLabel[i] );


	wsdNodes  .clear(); 
	wsdNodeNei.clear(); 
	wsdNodes  .resize( maxLabel + 1 );
	wsdNodeNei.resize( maxLabel + 1 );
	

	for( int z = 0; z < D ; ++z)
	for( int y = 0; y < H ; ++y)
	for( int x = 0; x < W ; ++x)
	{
		const int I = x + y*W + z*WH;
		wsdNodes[ vLabel[I] ].addVoxel( I, (float)vol[I] );

		if( x < W-1 && vLabel[I] != vLabel[ I+1 ]) 
		{
			if( vLabel[I] < vLabel[I+1] ) wsdNodeNei[ vLabel[ I   ] ].insert( vLabel[ I+1 ] );
			else                          wsdNodeNei[ vLabel[ I+1 ] ].insert( vLabel[ I   ] );
		}
		if( y < H-1 && vLabel[I] != vLabel[ I+W ] )
		{
			if( vLabel[I] < vLabel[I+W] ) wsdNodeNei[ vLabel[ I   ] ].insert( vLabel[ I+W ] );
			else                          wsdNodeNei[ vLabel[ I+W ] ].insert( vLabel[ I   ] );
		}
		if( z < D-1 && vLabel[I] != vLabel[ I+WH ] )
		{
			if( vLabel[I] < vLabel[I+WH]) wsdNodeNei[ vLabel[ I   ] ].insert( vLabel[ I+WH ] );
			else                          wsdNodeNei[ vLabel[ I+WH] ].insert( vLabel[ I    ] );
		}
	}
	time_t t1 = clock();
	std::cout <<  "t_constructWsdNodesFromLabel : "
    << W << " " << H << " " << D << " time:" <<(t1-t0)/(double)CLOCKS_PER_SEC << "\n";

}








//threadとして起動される
void ModeSegGCut::initWsdNodes_thread(void *pParam)
{
	std::cout << "initWsdNodes_thread start!!\n";

	ModeSegGCut *P  = (ModeSegGCut*)pParam;

	const EVec3i  reso  = ImageCore::getInst()->getResolution();
	const short  *vol   = ImageCore::getInst()->m_volOrig   ;
	const float  *volGM = ImageCore::getInst()->m_volOrigGM ;
	
  const int W = reso[0];
	const int H = reso[1];
	const int D = reso[2], WHD = W*H*D, WH = W*H;

	//calc WsdLabel 0:境界画素, >0:ラベル値
	vector<int> &vLabel = P->m_map_vox2wsd;

	t_wsd_CalcLabelFromGMag   ( W,H,D, volGM, 1000, vLabel); 
	t_wsd_RemoveOneVoxWsdLabel( vLabel ); 
	t_wsd_CollapseWsdPixels3D ( W,H,D, vol  ,       vLabel ); 

	for( int i= 0; i < WHD; ++i ) vLabel[i] -= 1; 

	//calc Node & neighbors
	t_constructWsdNodesFromLabel(W,H,D, vLabel, vol, P->m_wsdNodes, P->m_wsdNodeNei );

	//save backup file
	string backUpFilePath = ImageCore::getInst()->getFilePath() + ".RpWsdPre";
	t_saveWsdLabel( backUpFilePath, reso, vLabel);

	P->m_bWsdInitialized = true;
	P->m_bWsdComputing   = false;
	

	std::cout << "initWsdNodes_thread DONE!!\n";
}




inline static EVec4i t_voxIdx(const int &W, const int &WH, const int &vI)
{
	EVec4i v;
	v[3] = vI;
	v[2] =  vI/WH;
	v[1] = (vI - v[2]*WH) / W;
	v[0] = (vI - v[2]*WH - v[1]*W);
	return v;
}



//
//slipt one wsd region that contains multiple fore/back seeds
//
void t_wsd_DivideOneLabel( 
	const int W, 
	const int H,
	const int D, 
	const int maxLabel,
	const int fI, 
	const int bI,
	vector<int > &vLabel)
{
	std::cout << "divide conflict wsd area " << fI << " " << bI 
            << "Label_pair:" << vLabel[ fI ] << " and " << vLabel[ bI ] << "\n";

	if( vLabel[ fI ] != vLabel[ bI ] ) return;

	const int WH      = W*H;
	const int TRGT    = vLabel[ fI ];
	const int F_LABEL = maxLabel + 2;
	const int B_LABEL = maxLabel + 1;

	TQueue<EVec4i> fQ, bQ; 
	fQ.push_back( t_voxIdx(W,WH,fI) ); 
	bQ.push_back( t_voxIdx(W,WH,bI) ); 

	//growth
	while( !fQ.empty() || !bQ.empty() )
	{
		if( !bQ.empty() )
		{
			const int x = bQ.front()[0]; 
			const int y = bQ.front()[1]; 
			const int z = bQ.front()[2]; 
			const int I = bQ.front()[3]; 
			bQ.pop_front( );
			if( x>0  && vLabel[I-1 ]==TRGT ){ bQ.push_back( EVec4i(x-1,y  ,z  ,I- 1) ); vLabel[I-1 ] = B_LABEL; }
			if( x<W-1&& vLabel[I+1 ]==TRGT ){ bQ.push_back( EVec4i(x+1,y  ,z  ,I+ 1) ); vLabel[I+1 ] = B_LABEL; }
			if( y>0  && vLabel[I-W ]==TRGT ){ bQ.push_back( EVec4i(x  ,y-1,z  ,I- W) ); vLabel[I-W ] = B_LABEL; }
			if( y<H-1&& vLabel[I+W ]==TRGT ){ bQ.push_back( EVec4i(x  ,y+1,z  ,I+ W) ); vLabel[I+W ] = B_LABEL; }
			if( z>0  && vLabel[I-WH]==TRGT ){ bQ.push_back( EVec4i(x  ,y  ,z-1,I-WH) ); vLabel[I-WH] = B_LABEL; }
			if( z<D-1&& vLabel[I+WH]==TRGT ){ bQ.push_back( EVec4i(x  ,y  ,z+1,I+WH) ); vLabel[I+WH] = B_LABEL; }
		}
		if( !fQ.empty() )
		{
			const int x = fQ.front()[0]; 
			const int y = fQ.front()[1]; 
			const int z = fQ.front()[2]; 
			const int I = fQ.front()[3]; 
			fQ.pop_front( );
			if( x>0  && vLabel[I-1 ]==TRGT ){ fQ.push_back( EVec4i(x-1,y,z,I- 1) ); vLabel[I-1 ] = F_LABEL; }
			if( x<W-1&& vLabel[I+1 ]==TRGT ){ fQ.push_back( EVec4i(x+1,y,z,I+ 1) ); vLabel[I+1 ] = F_LABEL; }
			if( y>0  && vLabel[I-W ]==TRGT ){ fQ.push_back( EVec4i(x,y-1,z,I- W) ); vLabel[I-W ] = F_LABEL; }
			if( y<H-1&& vLabel[I+W ]==TRGT ){ fQ.push_back( EVec4i(x,y+1,z,I+ W) ); vLabel[I+W ] = F_LABEL; }
			if( z>0  && vLabel[I-WH]==TRGT ){ fQ.push_back( EVec4i(x,y,z-1,I-WH) ); vLabel[I-WH] = F_LABEL; }
			if( z<D-1&& vLabel[I+WH]==TRGT ){ fQ.push_back( EVec4i(x,y,z+1,I+WH) ); vLabel[I+WH] = F_LABEL; }
		}
	}
	//fore label の値を元に戻す
	for( auto &it: vLabel) if( it == F_LABEL ) it = TRGT;

	std::cout << "done!!\n";

}



//divide the wsd labeled region 
//when the bothe pos and neg CPs exist in the region
bool t_wsd_CheckAndSolveConflictCP( 
	const EVec3i         &reso,
	const vector<GCutCp> &fCPs ,
	const vector<GCutCp> &bCPs ,

	vector<int> &vLabel)
{

	set<int> fIdx,bIdx; 
	for(const auto& cp : fCPs ) fIdx.insert( cp.m_vIdx[3] );
	for(const auto& cp : bCPs ) bIdx.insert( cp.m_vIdx[3] );

	int maxL = 0;
	for( const auto& i: vLabel) maxL = max( maxL, i);

	bool modified = false;

	for ( const auto &f : fIdx)
	{
		for ( const auto &b : bIdx)
		{
			if (vLabel[f] == vLabel[b])
			{
				t_wsd_DivideOneLabel( reso[0], reso[1], reso[2], maxL, f, b, vLabel);

				modified = true;
				maxL++;
			}
		}
	}

	return modified;

}





























////////////////////////////////////////////////////////////////////////////////////////
//Graph cut segmentation////////////////////////////////////////////////////////////////

static inline float t_distSq(const float &f1, const float &f2)
{
	return (f1-f2)*(f1-f2);
}

inline static void t_calcE1(
	const vector<WsdNode> &nodes,
	const vector<int    > &fNodeIDs,
	const vector<int    > &bNodeIDs,
	const int             &pivNodeID,
	EVec2f &e
)
{
	float df = FLT_MAX;
	float db = FLT_MAX;
	float pivV = nodes[pivNodeID].m_aveI;
	for( const auto& it: fNodeIDs)  df = min(df, t_distSq(  nodes[it].m_aveI, pivV) );
	for( const auto& it: bNodeIDs)  db = min(db, t_distSq(  nodes[it].m_aveI, pivV) );

	df = sqrt( df );
	db = sqrt( db );

	if( df == 0 && db == 0 ) df = db =  0.5f;

	//see Lazy snapping papaer for detail
	e << db / ( df + db ), df / ( df + db ); 
}

inline static float t_calcE2( const float &v1, const float &v2, const float &lambda)
{	    
	return  lambda / ( 1.0f + (v1-v2)*(v1-v2) ); 
}

//重複を削除 (sort --> unique --> erase)
static void t_vectorUnique( vector<int> &vectInt)
{
	sort( vectInt.begin(), vectInt.end() ); 
	auto fvIt = unique( vectInt.begin(), vectInt.end() ); 
	vectInt.erase( fvIt, vectInt.end() ); 
}


//
//only extract region connect to foreground seeds 
//vFlg = 0:not target, 1:back, 255:fore
//
void t_postGCut_extractJointForeRegion( 
	const int &W, 
	const int &H, 
	const int &D, 
	const vector<GCutCp> &fCPs, 
	OglImage3D  &vFlg)
{	


	const int WH = W*H, WHD = W*H*D;

	//新たな foregroundを254として領域拡張する
	TQueue<EVec4i> Q;
	for (const auto &cp : fCPs)
	{
		Q.push_back( cp.m_vIdx );
		vFlg[cp.m_vIdx[3]] = 254;
	}

	while( !Q.empty() )
	{
		const int x = Q.front()[0];
		const int y = Q.front()[1];
		const int z = Q.front()[2];
		const int I = Q.front()[3];
		Q.pop_front();

		if( x != 0   && vFlg[ I - 1  ] == 255 ) { vFlg[ I - 1 ] = 254; Q.push_back( EVec4i(x-1,y  ,z  ,I-1 ) );}
		if( y != 0   && vFlg[ I - W  ] == 255 ) { vFlg[ I - W ] = 254; Q.push_back( EVec4i(x  ,y-1,z  ,I-W ) );}
		if( z != 0   && vFlg[ I - WH ] == 255 ) { vFlg[ I - WH] = 254; Q.push_back( EVec4i(x  ,y  ,z-1,I-WH) );}
		if( x != W-1 && vFlg[ I + 1  ] == 255 ) { vFlg[ I + 1 ] = 254; Q.push_back( EVec4i(x+1,y  ,z  ,I+1 ) );}
		if( y != H-1 && vFlg[ I + W  ] == 255 ) { vFlg[ I + W ] = 254; Q.push_back( EVec4i(x  ,y+1,z  ,I+W ) );}
		if( z != D-1 && vFlg[ I + WH ] == 255 ) { vFlg[ I + WH] = 254; Q.push_back( EVec4i(x  ,y  ,z+1,I+WH) );}
	}

#pragma omp parallel for
	for( int idx = 0; idx<WHD; ++idx) 
	{
		if(      vFlg[ idx ] == 254 ) vFlg[ idx ] = 255;
		else if( vFlg[ idx ] == 255 ) vFlg[ idx ] = 1  ;	
	}
		
}




void ModeSegGCut::runGraphCutWsdLv(float lambda)
{
	if (m_bWsdComputing || !m_bWsdInitialized)
	{
		CLI_MessageBox_OK_Show("現在前計算中です、少々お待ちください", "Message from Graph cut tool");
		return;
	}

	time_t t1 = clock();
	std::cout << "graphCut 1....\n";

	const EVec3i  reso  = ImageCore::getInst()->getResolution();
	const short  *vol   = ImageCore::getInst()->m_volOrig  ;
	const int    W      = reso[0];
	const int    H      = reso[1];
	const int    D      = reso[2];
	const int    WHD    = W*H*D, WH = W*H;
	
  OglImage3D  &vFlg    = ImageCore::getInst()->m_volFlg   ;


	// ひとつのノードに二つ以上の前景・背景制御点が配置されていたら、分割する
	if (t_wsd_CheckAndSolveConflictCP(reso, m_fCPs, m_bCPs, m_map_vox2wsd))
	{
		t_constructWsdNodesFromLabel( W,H,D, m_map_vox2wsd, vol, m_wsdNodes, m_wsdNodeNei );
	}

	// disable wsd nodes that exist in locked mask region
	for( auto& n : m_wsdNodes)
	{
		n.m_bEnable = false;
		for (const auto& pI : n.m_pixelIDs) if (vFlg[pI] != 0)
		{
			n.m_bEnable = true; 
			break; 
		}
	}


	// cps --> node ids
	vector< int > fWsdId, bWsdId;
	for (const auto& c : m_fCPs) fWsdId.push_back( m_map_vox2wsd[ c.m_vIdx[3] ] );
	for (const auto& c : m_bCPs) bWsdId.push_back( m_map_vox2wsd[ c.m_vIdx[3] ] );
	t_vectorUnique( fWsdId );
	t_vectorUnique( bWsdId );


	//flow network
	int edgeNumGuess = 0 ; 
	for( const auto& n: m_wsdNodeNei ) edgeNumGuess += (int) n.size(); 

	const int nodeNum  = (int)m_wsdNodes.size(); 
	const int edgeNum  = 2 * (nodeNum + edgeNumGuess)  ; //2 * tLink + nLinks
	const int SourceID = nodeNum + 0 ;
	const int SinkID   = nodeNum + 1 ;

	TFlowNetwork_BK4 network( nodeNum + 2, edgeNum, SourceID, SinkID ); 


	time_t t2 = clock();
	std::cout << "graphCut " << nodeNum << " " << edgeNum << " "  << fWsdId.size() << " " << bWsdId.size() << "\n";

	//t-link
	vector<EVec2f> tLinkE(nodeNum);

#pragma omp parallel for
	for( int i = 0; i < nodeNum; ++i)
	{
		tLinkE[i] << 0, FOREBACK_MAX; 
		if( m_wsdNodes[i].m_bEnable ) t_calcE1( m_wsdNodes, fWsdId, bWsdId, i, tLinkE[i] ); 
	}

	for( const auto &id : fWsdId ) if( m_wsdNodes[ id ].m_bEnable ) tLinkE[id] << FOREBACK_MAX, 0 ;  
	for( const auto &id : bWsdId ) if( m_wsdNodes[ id ].m_bEnable ) tLinkE[id] << 0, FOREBACK_MAX ;  

	for( int i = 0; i < nodeNum; ++i ) network.add_tLink( SourceID, SinkID, i, tLinkE[i][0], tLinkE[i][1]);


	//n-link 
	for (int i = 0; i < nodeNum; ++i) if( m_wsdNodes[i].m_bEnable ) 
	{
		for( const auto &neiI : m_wsdNodeNei[i] )
		{
			if( i < neiI && m_wsdNodes[ neiI ].m_bEnable )
			{
				network.add_nLink( i, neiI, t_calcE2( m_wsdNodes[i].m_aveI, m_wsdNodes[neiI].m_aveI, lambda) );
			}
		}
	}

	time_t t3 = clock();
	std::cout << "graphCut 2....\n";

	//Graph Cut
	byte *minCut = new byte[ nodeNum + 2 ];
	network.calcMaxFlow( SourceID, SinkID, minCut );


#pragma omp parallel for
	for(int i = 0; i < WHD; ++i ) if( vFlg[i] != 0 ) vFlg[i] = (minCut[ m_map_vox2wsd[i] ]) ? 255 : 1;


	t_postGCut_extractJointForeRegion( W,H,D, m_fCPs, vFlg );

	vFlg.SetUpdated();

	time_t t4 = clock();
  std::cout << "graphCut 3.... time ..." 
			      << (t2-t1)/(double)CLOCKS_PER_SEC
			      << (t3-t2)/(double)CLOCKS_PER_SEC
			      << (t4-t3)/(double)CLOCKS_PER_SEC << "\n";

	delete[] minCut ;

  formMain_redrawMainPanel();
}










//float lambda,        係数ラムダ
//int bandWidth,　　   計算領域のwidth
//bool genBundOnlyBack 計算領域を背景ボクセルのみから生成する
void ModeSegGCut::runGraphCutVoxLv(float lambda, int bandWidth, bool genBundOnlyBack)
{
	if (m_bWsdComputing || !m_bWsdInitialized)
	{
		CLI_MessageBox_OK_Show("現在前計算中です、少々お待ちください", "Message from Graph cut tool");
		return;
	}

	const int    SEAM_W = bandWidth;
	const EVec3i  reso  = ImageCore::getInst()->getResolution();
	const short  *vol   = ImageCore::getInst()->m_volOrig  ;
	const int    W      = reso[0];
	const int    H      = reso[1];
	const int    D      = reso[2];
	const int    WHD    = W*H*D, WH = W*H;
	OglImage3D &vFlg    = ImageCore::getInst()->m_volFlg   ;



	// 前景-背景 境界帯状領域の生成

	// v_mapNodeId --> -1:fore, -2:back, 0以上: nodeIdx
	int  *v_mapNodeId = new int[ WHD ];  
	
	for( int i=0; i < WHD; ++i) v_mapNodeId[i] = (vFlg[i] == 255) ? -1 : -2;
	
	for( int z = 1; z < D -1; ++z)
	for( int y = 1; y < H -1; ++y)
	for( int x = 1; x < W -1; ++x)
	{
		const int I = x + y*W + z*WH;
		if( vFlg[ I ] != 255 ) continue;

		if( vFlg[I-1] <= 1 || vFlg[I-W] <= 1 || vFlg[I-WH] <= 1 || 
			  vFlg[I+1] <= 1 || vFlg[I+W] <= 1 || vFlg[I+WH] <= 1 )
		{
			for( int zz = -SEAM_W; zz <= SEAM_W; ++zz) if( 0 <= z+zz && z+zz < D )
			for( int yy = -SEAM_W; yy <= SEAM_W; ++yy) if( 0 <= y+yy && y+yy < H )
			for( int xx = -SEAM_W; xx <= SEAM_W; ++xx) if( 0 <= x+xx && x+xx < W )
			{
				const int pivI = I + xx + yy*W + zz * WH;
				if (genBundOnlyBack)
				{
					if( vFlg[ pivI ] != 255 ) v_mapNodeId[ pivI ] = 0;
				}
				else
				{
					v_mapNodeId[ pivI ] = 0;
				}
			}
		}
	}


	//gen flow network
	vector<GCVoxNode> voxNodes;
	voxNodes.reserve(WHD);
	for(int i = 0, k=0; i < WHD; ++i) if( v_mapNodeId[i] == 0 )
	{
		v_mapNodeId[i] = (int) voxNodes.size();
		voxNodes.push_back( GCVoxNode(i,vol[i]) );
	}


	const int nodeN    = (int) voxNodes.size();
	const int edgeN    = 2 * nodeN + 6 * nodeN ;
	const int SourceID = nodeN + 0;
	const int SinkID   = nodeN + 1;
	TFlowNetwork_BK4 network( nodeN + 2, edgeN, SourceID, SinkID );

	for( int ni = 0; ni < nodeN; ++ni)
	{	
		EVec4i vI = t_voxIdx(W, WH, voxNodes[ni].m_voxID);
		const int I = vI[3];

		int state = 0; // -2 : 背景隣接   -1 :前景隣接   >=0:その他
		if( vI[0] != 0  && v_mapNodeId[ I-1 ] < 0 ) state = v_mapNodeId[ I-1  ]; // = -1 or -2
		if( vI[1] != 0  && v_mapNodeId[ I-W ] < 0 ) state = v_mapNodeId[ I-W  ]; 
		if( vI[2] != 0  && v_mapNodeId[ I-WH] < 0 ) state = v_mapNodeId[ I-WH ]; 
		if( vI[0] !=W-1 && v_mapNodeId[ I+1 ] < 0 ) state = v_mapNodeId[ I+1  ]; 
		if( vI[1] !=H-1 && v_mapNodeId[ I+W ] < 0 ) state = v_mapNodeId[ I+W  ]; 
		if( vI[2] !=D-1 && v_mapNodeId[ I+WH] < 0 ) state = v_mapNodeId[ I+WH ]; 

		//tlink
		double e1_f = (state == -1) ? FOREBACK_MAX : (state ==-2 ) ? 0            : 1;
		double e1_b = (state == -1) ?     0        : (state ==-2 ) ? FOREBACK_MAX : 1;
		network.add_tLink( SourceID, SinkID,  ni, e1_f, e1_b );

		//nlink
		if( vI[0] != W-1 && v_mapNodeId[I+1 ] >= 0 ) { network.add_nLink( ni, v_mapNodeId[I+1 ], t_calcE2( voxNodes[ni].m_I, voxNodes[v_mapNodeId[I+1 ]].m_I, lambda) ); }
		if( vI[1] != H-1 && v_mapNodeId[I+W ] >= 0 ) { network.add_nLink( ni, v_mapNodeId[I+W ], t_calcE2( voxNodes[ni].m_I, voxNodes[v_mapNodeId[I+W ]].m_I, lambda) ); }
		if( vI[2] != D-1 && v_mapNodeId[I+WH] >= 0 ) { network.add_nLink( ni, v_mapNodeId[I+WH], t_calcE2( voxNodes[ni].m_I, voxNodes[v_mapNodeId[I+WH]].m_I, lambda) ); }
	}

	byte *minCut = new byte[ nodeN + 2 ];
	network.calcMaxFlow( SourceID, SinkID, minCut );


#pragma omp parallel for
	for( int i = 0; i < WHD; ++i) if( vFlg[i] != 0 )
	{
		vFlg[i] = ( v_mapNodeId[i] == -1    ) ?  255 :
			        ( v_mapNodeId[i] == -2    ) ?  1   : 
				      ( minCut[ v_mapNodeId[i] ]) ?  255 : 1;
	}

	delete[] minCut;
	delete[] v_mapNodeId;
	vFlg.SetUpdated();
  formMain_redrawMainPanel();
}





#pragma managed




