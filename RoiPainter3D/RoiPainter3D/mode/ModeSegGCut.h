#pragma once

//--------------------------------------------------------------
// class ModeSegGCut  
// GraphCutÇ…ÇÊÇÈsegmentationÇçsÇ»Ç§ÉÇÅ[Éh
// 
//--------------------------------------------------------------




#include "ModeInterface.h"
#include "GlslShader.h"
#include <vector>

using namespace::std;


#pragma unmanaged



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








class ModeSegGCut : public ModeInterface
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
	bool             m_bWsdInitialized;
	bool             m_bWsdComputing  ;
	vector<int     > m_map_vox2wsd; // map voxel idx --> wsdNode idx --> 
	vector<WsdNode > m_wsdNodes   ; // wsdNodes 
	vector<set<int>> m_wsdNodeNei ; // neighbors of wsdNode[i] (small idx --> local idx)


  ModeSegGCut();
public:
  ~ModeSegGCut();

  static ModeSegGCut* getInst() { 
    static ModeSegGCut p; 
    return &p; 
  }

  // overload functions ---------------------------------------------
  MODE_ID getModeID() { return MODE_VIS_NORMAL; }

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

  void keyDown(int nChar);
  void keyUp  (int nChar);

  bool canEndMode();
  void startMode ();
  void drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF);

public:
	void runGraphCutWsdLv(float lambda);
	void runGraphCutVoxLv(float lambda, int bandWidth, bool genBundOnlyBack );
	void finishSegm ();
	void cancelSegm ();
	void clearAllCPs();
  void newVolLoaded(){ m_bWsdInitialized = false; }

private:
	static UINT initWsdNodes_thread( LPVOID pParam );
};

#pragma managed