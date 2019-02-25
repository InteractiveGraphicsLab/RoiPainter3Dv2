
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
#include "./COMMON/tmath.h"
#include "./COMMON/tmesh.h"

#include <vector>
#include <string>


#pragma unmanaged


class MaskData
{

public:
  std::string name     ;
  TMesh  surf     ;
  EVec3i color    ;
  double alpha    ;
  bool   bRendSurf;
  bool   lock     ;

	MaskData(std::string _name, EVec3i _color, double _alpha, bool _bRendSurf, bool _lock = false) 
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
	std::string  m_filePath;

  
public:
	//volume 
	short              *m_volOrig  ; // original image          [W,H,D]
	float              *m_volOrigGM; // original image grad mag [W,H,D]
	OglImage3D          m_vol      ; // original image with tone mapping (byte) 
	OglImage3D          m_volFlg   ; // Flg 
	OglImage3D          m_volMsk   ; // mask 
	OglImage3D          m_volGmag  ; // gradient magnitude 
	OglImage1D<CH_RGBA> m_imgMskCol; // func: maskID    --> color

	  int               m_maskSelectedId; // -1:none, 0...:maskID
	std::vector<MaskData> m_maskData      ;

	


  //singleton
private:
  ImageCore();
public:
	static ImageCore* getInst(){ static ImageCore p; return &p;}


  //functions
	void updateVisVolume(short minV, short maxV);

	//I/O Loaders volume 
	bool loadVolume   (std::vector<std::string> fnames, std::string fext);
	bool loadVolume   (std::string fname              , std::string fext) ;
	void loadMask     (const char *fname);
	void saveMask     (const char *fname);
	void saveMaskAsFav(const char *fname);
  void saveVolumeAsTraw3dss(const char *fname);

	//getter & setter for resolution and pitch
	EVec3f getCuboidF() { return EVec3f((float)(m_Reso[0] * m_Pitch[0]), (float)(m_Reso[1] * m_Pitch[1]), (float)(m_Reso[2] * m_Pitch[2])); }
	EVec3i getResolution() { return m_Reso; }

  std::string getFilePath(){ return m_filePath;}


	//getter/setter for pitch 
	EVec3f getPitch()  { return m_Pitch; }
	float  getPitchW() { return m_Pitch[0]; }
	float  getPitchH() { return m_Pitch[1]; }
	float  getPitchD() { return m_Pitch[2]; }

  void  setPitch (const EVec3f pitch){ m_Pitch = pitch;}
  void  setPitchW(const float &pW) { m_Pitch[0] = pW; }
	void  setPitchH(const float &pH) { m_Pitch[1] = pH; }
	void  setPitchD(const float &pD) { m_Pitch[2] = pD; }

	//volume min/mac
	EVec2i getVolMinMax() { return m_volMinMax; }

  int getVoxelIndex(const EVec3f& position)
  {
    const int x = std::min(m_Reso[0] - 1, (int)(position[0] / m_Pitch[0]));
    const int y = std::min(m_Reso[1] - 1, (int)(position[1] / m_Pitch[1]));
    const int z = std::min(m_Reso[2] - 1, (int)(position[2] / m_Pitch[2]));
    return x + y * m_Reso[0] + z * m_Reso[0] * m_Reso[1];
  }
  EVec4i getVoxelIndex4i(const EVec3f& position)
  {
    EVec4i v;
    v[0] = std::min(m_Reso[0] - 1, (int)(position[0] / m_Pitch[0]));
    v[1] = std::min(m_Reso[1] - 1, (int)(position[1] / m_Pitch[1]));
    v[2] = std::min(m_Reso[2] - 1, (int)(position[2] / m_Pitch[2]));
    v[3] = v[0] + v[1] * m_Reso[0] + v[2] * m_Reso[0] * m_Reso[1];
    return v;
  }


  short getVoxelValue(const EVec3f& position)
  {
    return m_volOrig[ getVoxelIndex(position)];
  }




	//mask manipuration

  void mask_storeCurrentForeGround();
  void selectedMsk_setLock    (const bool   tf    );
  void selectedMsk_setRendSurf(const bool   tf    );
  void selectedMsk_setAlpha   (const double alpha );
  void selectedMsk_setColor   (const EVec3i &c    );

  void selectedMsk_delete  ();
  void selectedMsk_marge   (const int &trgtMaskID);
  void selectedMsk_erode   ();
  void selectedMsk_dilate  ();
  void selectedMsk_fillHole();
  void selectedMsk_expObj  (const std::string &fname);


	//void ActvMsk_fillPsuedoHoleAsNewRegion();
	//void ActvMsk_ExportObj(const char *fname);
	//void ActvMsk_ExportStl(const char *fname);
	//void ActvMsk_ExportBmp(const char *fname);
	//string getVolFilePath(){ return m_filePath; }

private:
	void updateGradVolume();



};

#pragma managed




















