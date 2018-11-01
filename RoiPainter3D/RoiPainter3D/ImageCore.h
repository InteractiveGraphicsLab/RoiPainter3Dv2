
//------------------------------------
//class ImageCore
//
//2018/10/31 
//written by Takashi Ijiri
//
//This class manages following images,
//+ volumetric image
//+ maskId     image
//+ transfer functions (as images)
//-------------------------------------



#pragma once


#include "./COMMON/OglForCLI.h"
#include "./COMMON/OglImage.h"
#include "./COMMON/tmesh.h"

#include <vector>
#include <string>


#pragma unmanaged


class MaskData
{

public:
	string name     ;
	TMesh  surf     ;
	EVec3i color    ;
	double alpha    ;
	bool   bRendSurf;
	bool   lock     ;

	MaskData(string _name, EVec3i _color, double _alpha, bool _bRendSurf, bool _lock = false) 
	{
		name      = _name;
		color     = _color;
		alpha     = _alpha;
		bRendSurf = _bRendSurf;
		lock      = _lock;
	}


	void Set(const MaskData& v)
	{
		name     = v.name;
		surf     = v.surf;
		color    = v.color;
		alpha    = v.alpha;
		bRendSurf= v.bRendSurf;
		lock     = v.lock;
	}


	MaskData( const MaskData& v)
	{
		fprintf( stderr, "for debug, MaskData copy const\n");
		Set(v);
	}
	
	MaskData& operator=(const MaskData& v)
	{
		fprintf( stderr, "for debug, MaskData operator =\n");
		Set(v);
		return *this;
	}

};





class ImageCore
{  
private:
	//volume info 
	EVec3i  m_Reso ;
	EVec3f  m_Pitch;
  EVec2i  m_volMinMax;

	string  m_filePath;

  
public:
	//DlgVolInfo  m_dlg;

	//volume 
	short              *m_volOrig  ; // original image          [W,H,D]
	float              *m_volOrigGM; // original image grad mag [W,H,D]
	OglImage3D          m_vol      ; // original image with tone mapping (byte) 
	OglImage3D          m_volFlg   ; // Flg 
	OglImage3D          m_volMsk   ; // mask 
	OglImage3D          m_volGmag  ; // gradient magnitude 
	OglImage1D<CH_RGBA> m_imgMskCol; // func: maskID    --> color

	int                 m_maskActiveId;
	vector<MaskData>    m_maskData    ;

	


  //singleton
private:
  ImageCore();
public:
	static ImageCore* getInst(){ static ImageCore p; return &p;}


  //functions
	void updateVisVolume(short minV, short maxV);

	//I/O Loaders volume 
	bool loadVolume   (vector<string> fnames, string fext);
	bool loadVolume   (string fname         , string fext) ;
	void loadMask     (const char *fname);
	void saveMask     (const char *fname);
	void saveMaskAsFav(const char *fname);

	//getter & setter for resolution and pitch
	EVec3f getCuboidF() { return EVec3f((float)(m_Reso[0] * m_Pitch[0]), (float)(m_Reso[1] * m_Pitch[1]), (float)(m_Reso[2] * m_Pitch[2])); }
	EVec3i getResolution() { return m_Reso; }


	//getter/setter for pitch 
	EVec3f getPitch()  { return m_Pitch; }
	float getPitchW() { return m_Pitch[0]; }
	float getPitchH() { return m_Pitch[1]; }
	float getPitchD() { return m_Pitch[2]; }

  void  setPitch (const EVec3f pitch){ m_Pitch = pitch;}
  void  setPitchW(const float &pW) { m_Pitch[0] = pW; }
	void  setPitchH(const float &pH) { m_Pitch[1] = pH; }
	void  setPitchD(const float &pD) { m_Pitch[2] = pD; }

	//volume min/mac
	EVec2i getVolMinMax() { return m_volMinMax; }

  /*
	//get Value 
	short getVolVal(const EVec3f &pos);


	//mask manipuration
	void mask_storeCurrentForeGround();

	void ActvMsk_SetLock    (const bool   tf    );
	void ActvMsk_SetRendSurf(const bool   tf    );
	void ActvMsk_SetAlpha   (const double alpha );
	void ActvMsk_SetColor   (const EVec3i &c    );
	void ActvMsk_erode      ();
	void ActvMsk_dilate     ();
	void ActvMsk_fillHole   ();
	void ActvMsk_delete     ();
	void ActvMsk_margeTo    ();
	void ActvMsk_fillPsuedoHoleAsNewRegion();
	void ActvMsk_ExportObj(const char *fname);
	void ActvMsk_ExportStl(const char *fname);
	void ActvMsk_ExportBmp(const char *fname);


	string getVolFilePath(){ return m_filePath; }


*/

private:
	void updateGradVolume();



};

#pragma managed



















