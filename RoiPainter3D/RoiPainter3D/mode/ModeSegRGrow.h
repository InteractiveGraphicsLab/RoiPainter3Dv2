#pragma once

//--------------------------------------------------------------
// class ModeSegRGrow 
// Region growing / thresholding Ç…ÇÊÇÈsegmentationÇçsÇ»Ç§ÉÇÅ[Éh
// 
//--------------------------------------------------------------




#include "ModeInterface.h"
#include "GlslShader.h"
#include <vector>

using namespace::std;





#pragma unmanaged

class ModeSegRGrow : public ModeInterface
{
  GlslShaderVolume m_volumeShader;
  GlslShaderCrsSec m_crssecShader;
  
  float m_CpSize;
  bool  m_bRegionUpdated;
  int   m_dragCpId;
  bool  m_bDrawStr;
  vector<EVec3f> m_stroke;


  TMesh m_CpSph;
	vector<EVec3f> m_CPs;

  ModeSegRGrow();
public:
  ~ModeSegRGrow();

  static ModeSegRGrow* getInst() { 
    static ModeSegRGrow p; 
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


public:
  void runThresholding(short minV, short maxV);
  void runRegionGrow6 (short minV, short maxV);
  void runRegionGrow26(short minV, short maxV);
  void runErosion();
  void runDilation();
  void runFillHole();

  void finishSegmentationStoreRegion();



private:
  int pickCPs(const EVec3f &rayP, const EVec3f &rayD);

};

#pragma managed




/*#pragma once



#include "ModeInterface.h"


#include "tmesh.h"
#include "GlslShader.h"
#include "DlgSegGCut.h"

#include <set>




class GCutCp
{
public:
	EVec3f m_pos ;
	EVec4i m_vIdx;

	GCutCp(EVec3f p, EVec4i vidx)
	{
		m_pos = p   ;
		m_vIdx= vidx;
	}	
};




//Watershed super pixel node (WsdLevel)
class WsdNode
{
public:
	
	//pixel indices
	vector<int> m_pixelIDs ;	
	
	//total/average intensity
	float  m_sumI, m_aveI;
	
	//target for segmentation?
	bool   m_bEnable ;
	
	~WsdNode(){}
	WsdNode (){
		m_sumI  = m_aveI = 0;
		m_bEnable =  false;
	}
	WsdNode(const WsdNode &n){
		m_sumI     = n.m_sumI;   
		m_aveI     = n.m_aveI;  
		m_bEnable  = n.m_bEnable;
		m_pixelIDs = n.m_pixelIDs;
	}
	inline void addVoxel(int voxelIdx, float intensity){
		m_pixelIDs.push_back( voxelIdx );
		m_sumI += intensity;
		m_aveI  = m_sumI / (float) m_pixelIDs.size();
	}
};



//vocel node (vocel Level)
class GCVoxNode
{
public:
	int   m_voxID;
	float m_I    ;

	void set( const int &voxIdx, const float &intensity){
		m_voxID = voxIdx;
		m_I = intensity;
	}
	GCVoxNode(){m_voxID = 0; m_I = 0;}
	GCVoxNode(const int &voxIdx, const float &intensity){
		set( voxIdx,intensity );
	}
};




class ModeSegGCut :
	public ModeInterface
{
	GlslShaderVolume m_volumeShader;
	GlslShaderCrsSec m_crssecShader;

	//mouse manipuration
	bool m_bPaintCP;
	bool m_bDrawCutStr;
	vector<EVec3f> m_stroke;

	//control points
	vector<GCutCp> m_fCPs, m_bCPs;
	TMesh          m_CpMesh;
	float          m_CpSize;

	//watershad super pixel 
	bool   m_bWsdInitialized;
	bool   m_bWsdComputing  ;
	vector< int      > m_map_vox2wsd; // map voxel idx --> wsdNode idx --> 
	vector< WsdNode  > m_wsdNodes   ; // wsdNodes 
	vector< set<int> > m_wsdNodeNei ; // neighbors of wsdNode[i] (small idx --> local idx)


	ModeSegGCut();
public:
	~ModeSegGCut();


	DlgSegGCut m_dlg;

	static ModeSegGCut *getInst(){ static ModeSegGCut p; return &p; }


	// overload functions ---------------------------------------------
	MODE_ID getModeID() { return MODE_SEG_GCUT; }
	void LBtnUp    ( const CPoint &p, OglForMFC &ogl);
	void RBtnUp    ( const CPoint &p, OglForMFC &ogl);
	void MBtnUp    ( const CPoint &p, OglForMFC &ogl);
	void LBtnDown  ( const CPoint &p, OglForMFC &ogl);
	void RBtnDown  ( const CPoint &p, OglForMFC &ogl);
	void MBtnDown  ( const CPoint &p, OglForMFC &ogl);
	void LBtnDclk  ( const CPoint &p, OglForMFC &ogl);
	void RBtnDclk  ( const CPoint &p, OglForMFC &ogl);
	void MBtnDclk  ( const CPoint &p, OglForMFC &ogl);
	void MouseMove ( const CPoint &p, OglForMFC &ogl);
	void MouseWheel( const CPoint &p, int nFlags, short zDelta, OglForMFC &ogl);

	void keyDown(int nChar, int nRepCnt, int nFlags);
	void keyUp  (int nChar, int nRepCnt, int nFlags);

	bool canEndMode();
	void startMode ();
	void drawScene (const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF );
	//-----------------------------------------------------------------


	void newVolLoaded();

	void runGraphCutWsdLv(float lambda);
	void runGraphCutVoxLv(float lambda, int bandWidth, bool genBundOnlyBack );

	void finishSegm ();
	void cancelSegm ();
	void clearAllCPs();

private:
	static UINT initWsdNodes_thread( LPVOID pParam );

};

*/