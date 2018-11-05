#include "ImageCore.h"
#include "FormStackOrder.h"

#include "climessagebox.h"
#include "./COMMON/tmath.h"
#include "./COMMON/tmarchingcubes.h"
#include "./3rdParty/dcmtk/tdcmtk.h"
#include "./3rdparty/vvv/ddsbase.h"


ImageCore::ImageCore()
{
  fprintf(stderr, "CONSTRUCTOR ImageCore ...\n");

	m_volOrig = 0;
	loadVolume("init","init");
	m_imgMskCol.Allocate(256);
  m_maskSelectedId = -1;
  fprintf(stderr, "CONSTRUCTOR ImageCore ...DONE !\n");
}





/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//VOLUME LOADER//////////////////////////////////////

static bool t_LoadDefoultVolume(
	EVec3i &reso,
	EVec3f &pitch,
	short* &vol
	)
{
	const int N = 64, R = N/5;
	reso  << N, N, N;
	pitch << 1, 1, 1;
	vol = new short[reso[0] * reso[1] * reso[2]];

	EVec3f c1(2*R, 2*R, 2*R), c2(3*R, 3 * R, 3 * R);

	for (int z = 0; z < reso[2]; ++z)
	{
		for (int y = 0; y < reso[1]; ++y)
		{
			for (int x = 0; x < reso[0]; ++x)
			{
				EVec3f p((float)x, (float)y, (float)z);
				float d1 = (p - c1).norm();
				float d2 = (p - c2).norm();
				float v = 3000 * exp(-(d1-R)*(d1-R) / 10 ) + 3000 * exp(-(d2-R)*(d2-R) / 10);
				vol[x + y * reso[0] + z * reso[1] * reso[0]] = (short)( v - 1500 );
			}
		}
	}
	return true;
}




static bool t_LoadTRawSS(
	const char* fname,
	EVec3i &reso ,
	EVec3f &pitch,
	short* &vol
	)
{
	FILE *fp = fopen(fname, "rb");
	if (fp == 0) return false;

	fread(&reso [0], sizeof(int), 1, fp);
	fread(&reso [1], sizeof(int), 1, fp);
	fread(&reso [2], sizeof(int), 1, fp);
	
	double px, py, pz;
	fread(&px, sizeof(double), 1, fp);
	fread(&py, sizeof(double), 1, fp);
	fread(&pz, sizeof(double), 1, fp);
	pitch << (float) px, (float) py, (float) pz;

	const int WHD = reso[0] * reso[1] * reso[2];


	if (WHD> 1024 * 1024 * 1024 )
	{
		System::Windows::Forms::MessageBox::Show("volume size is too big (2Gbyte voxels)");
		return false;
	}

	//read signed short array
	vol = new short[WHD];
  size_t num = fread(vol, sizeof(short), WHD, fp);
	fclose(fp);

	if (num != WHD){
		delete[] vol;
    vol = 0;
		return false;
	}

	return true;
}



//気象データ用のレガシーコード
static bool t_LoadTxt(
	const char* fname,
	EVec3i &reso     ,
	EVec3f &pitch    ,
	short* &vol )
{
	reso  << 600, 300, 110;
	pitch << 1,1,1;

	vol = new short[600 * 300 * 110];

	double d;

	FILE *fp = fopen(fname, "r");

	for (int r = 0; r < 600; ++r)
	{
		for (int p = 0; p < 110; ++p)
		{
			for (int t = 0; t < 300; ++t)
			{
				fscanf( fp, "%lf", &d );
				vol[ p * 300 * 600 + t * 600 + r] = (short)d;
				fscanf( fp, ",");
			}
		}
	}

	return true;
}


/*
////////////////////////////////////////////////////////////////////////////////////
static bool t_LoadFAV(
	const  string fname,
	EVec3i &reso     ,
	EVec3f &pitch    ,
	short* &vol)
{

	printf("t_LoadFAV");
	
	FavLibrary::Fav fav;
	fav.read( fname.c_str() );

	if (fav.getNumObjects() == 0) return false;
	
	int id = fav.getObjects().begin()->first;
	FavLibrary::Object     &obj       = fav.getObjects().begin()->second;
	FavLibrary::Structure  &structure = obj.structure;		;
	FavLibrary::Grid       &grid      = obj.grid     ;
	FavLibrary::BitPerVoxel bitPerVox = structure.getBitPerVoxel();
	printf("%d!!  %d , %d\n", (int)fav.getObjects().size(), id, bitPerVox ) ;

	FavLibrary::ColorMode cMode = structure.getColorMode();
	printf("mode %d\n", structure.getColorMode());

	const int W = (int)structure.getDimensionX();
	const int H = (int)structure.getDimensionY();
	const int D = (int)structure.getDimensionZ();
	
	
	reso  << W, H, D;
	pitch << 1, 1, 1;
	vol = new short[W*H*D];

	fprintf(stderr, "dimension %d %d %d\n", W, H, D);

	fprintf( stderr, "%d\n", (int) structure.get_voxel_map()      ->size());
	fprintf( stderr, "%d\n", (int) structure.get_voxel_map_16bit()->size());
	fprintf( stderr, "%d\n", (int) structure.get_alpha_map()      ->size());
	fprintf( stderr, "%d\n", (int) structure.get_color_map()      ->size());
	fprintf( stderr, "%d\n", (int) structure.get_color_map_16bit()->size());

  
	for( int z = 0; z < D; ++z)
  {
	  for( int y = 0; y < H; ++y)
    {
	    for( int x = 0; x < W; ++x)
	    {
		    //int r = structure.getColorRed  (x, y, z);
		    //int g = structure.getColorGreen(x, y, z);
		    //int b = structure.getColorBlue (x, y, z);
		    //fprintf(stderr, "%d %d %d -- ", r,g,b);
		    //vol[x + y * W + z * W*H] = r+g+b;
	    }
    }
  }


    return true;
}
*/


static bool t_LoadBMP_TIFs
(
	const vector<string> &fnames,
	EVec3i &reso     ,
	EVec3f &pitch    ,
	short* &vol 
)
{
	OglImage2D<CH_RGBA> img;
	img.Allocate( fnames.front().c_str() );

	const int W = reso[0] = img.GetW();
	const int H = reso[1] = img.GetH();
	const int D = reso[2] = (int)fnames.size();
	const int WH = W*H;

	if( W == 0 || H == 0 || D == 0) return false;

	vol = new short[W * H* D ];

	for(int z=0; z < (int)fnames.size(); ++z)
	{
		OglImage2D<CH_RGBA> slice;
		slice.Allocate( fnames[z].c_str());

		if( slice.GetW() != W || slice.GetH() != H )
		{
			delete[] vol;
			vol = 0;
			return false;
		}

		//copy
		for(int y=0; y < H; ++y)
		{
			for(int x=0; x < W; ++x)
			{
				const int I = 4 * (x + y * W);
				vol[ x + y * W + z * WH ] = (short) ( (slice[ I ] + slice[ I+1 ] + slice[ I+2 ]) / 3) ;
			}
		}
		if( z%50 ==0 ) printf( "%d/%d done\n", z, (int)fnames.size());
	}

	pitch << 1,1,1;	
	return true;
}



static bool t_LoadDCMs
(
	const vector<string> &fnames,
	EVec3i &reso     ,
	EVec3f &pitch    ,
	short* &vol 
)
{

	//read initial frame to get size info
	Tdcmtk tdcm(fnames.front().c_str());
	int fNum;
	tdcm.getSize(reso[0], reso[1], fNum);

	const int W = reso[0];
	const int H = reso[1];
	const int D = reso[2] = (int)fnames.size();

	pitch[0] = (float) tdcm.getPitchX();
	pitch[1] = (float) tdcm.getPitchY();
	double zPos0 = tdcm.getZPos();

	vol = new short[W*H*D];

	//load all dicom file
	for (int z = 0; z < D; ++z)
	{
		Tdcmtk tdcm( fnames[z].c_str() );
		fprintf( stderr, "%f\n", tdcm.getZPos());

		int tW, tH, tfNum;
		tdcm.getSize(tW, tH, tfNum);

		if (tfNum > 1 || W != tW || H != tH )
		{
			System::Windows::Forms::MessageBox::Show("error strange input");
      delete[] vol;
      vol = 0;
      return false;
		}

		if (z == 1) pitch[2] = (float)( tdcm.getZPos() - zPos0 );

		tdcm.getPixelsAs<short>( &vol[z*W*H] );

		printf( "(%d/%d)", z, D);
	}


	if( pitch[2] < 0)
	{
		printf( "flip in z\n");
		pitch[2] *= -1.0;
		t_flipVolumeInZ(W,H,D, vol);
	}

	return true;
}





static bool t_LoadDCM3D(
	const  string fname,
	EVec3i &reso     ,
	EVec3f &pitch    ,
	short* &vol)
{
	Tdcmtk tdcm( fname.c_str() );

	tdcm.getSize(reso[0], reso[1], reso[2]);
	if( reso[2] <= 1 ) return false;
	
	pitch[0] = 1;//(float) tdcm.getPitchX();
	pitch[1] = 1;//(float) tdcm.getPitchY();
	pitch[2] = 1;//pitch[0];
	System::Windows::Forms::MessageBox::Show("pitch情報は読み込んでいません。正しい値をダイアログより指定してしてください");

	printf( "resolution %d %d %d\n", reso[0], reso[1], reso[2]);

	vol = new short[ reso[0] * reso[1] * reso[2] ];
	tdcm.getPixels3DAs<short>( vol );

	return true;
}




static bool t_LoadPVM3D(
	const  string fname,
	EVec3i &reso     ,
	EVec3f &pitch    ,
	short* &vol)
{
	unsigned int W,H,D,components;
	float  px,py,pz;

	byte *buf = readPVMvolume(fname.c_str(), &W, &H, &D,&components, &px,&py,&pz );

	printf( "load pvm %d %d %d %d\n", W,H,D,components);

	reso  << W, H, D;
	pitch << px, py, pz;
	vol = new short[W*H*D];

	for( int i=0,s = W*H*D; i < s; ++i)
	{
		if( components == 1) vol[i] = buf[i];
		else if( components == 2) {
			vol[i] = (short) ( buf[2*i] * 256 + buf[2*i+1] );
		}
	}
	return true;
}






bool ImageCore::loadVolume(string fname, string fext)
{
	vector<string> fnames;
	fnames.push_back( fname );
	return loadVolume( fnames, fext );
}




bool ImageCore::loadVolume(vector<string> fnames, string fext)
{
	if (fnames.size() == 0) return false;
	
	const int L = (int)fext.length();

	int trgtId = 
    (fnames.size() == 1 && ( fext == "traw3D_ss"                     ) ) ? 0 : 
		(fnames.size() == 1 && ( fext == "mat" || fext == "MAT"          ) ) ? 1 :
		(fnames.size() == 1 && ( fext == "txt" || fext == "TXT"          ) ) ? 2 :
		(fnames.size() == 1 && ( fext == "dcm" || fext == "DCM" || L == 0) ) ? 3 :
		(fnames.size() == 1 && ( fext == "pvm" || fext == "PVM"          ) ) ? 4 :
		(fnames.size() == 1 && ( fext == "fav" || fext == "FAV"          ) ) ? 5 :
		(fnames.size() >  1 && ( fext == "dcm" || fext == "DCM" || L == 0) ) ? 10 :	
		(fnames.size() >  1 && ( fext == "bmp" || fext == "BMP"	         ) ) ? 11 :
		(fnames.size() >  1 && ( fext == "tif" || fext == "TIF"	         ) ) ? 11 : -1;
	
	if ( m_volOrig  ) delete[] m_volOrig  ;
	if ( m_volOrigGM) delete[] m_volOrigGM;
	m_volOrig   = 0;
	m_volOrigGM = 0;

	bool success = false;
	//3D
  if ( trgtId == 0 && t_LoadTRawSS   (fnames[0].c_str(), m_Reso, m_Pitch, m_volOrig) ) success = true;
	if ( trgtId == 2 && t_LoadTxt      (fnames[0].c_str(), m_Reso, m_Pitch, m_volOrig) ) success = true;
	if ( trgtId == 3 && t_LoadDCM3D    (fnames[0].c_str(), m_Reso, m_Pitch, m_volOrig) ) success = true;
	if ( trgtId == 4 && t_LoadPVM3D    (fnames[0].c_str(), m_Reso, m_Pitch, m_volOrig) ) success = true;
	//if ( trgtId == 5 && t_LoadFAV      (fnames[0].c_str(), m_Reso, m_Pitch, m_volOrig) ) success = true;
  //2D slices
	if ( trgtId ==10 && t_LoadDCMs     (fnames           , m_Reso, m_Pitch, m_volOrig) ) success = true;
	if ( trgtId ==11 && t_LoadBMP_TIFs (fnames           , m_Reso, m_Pitch, m_volOrig) ) success = true;


	bool strangePitch = m_Pitch[0] <= 0 || m_Pitch[1] <= 0 || m_Pitch[2] <= 0;

	if( success && strangePitch ){
		m_Pitch[0] = m_Pitch[1] = m_Pitch[2] = 1;
		System::Windows::Forms::MessageBox::Show("Pitch情報を読み込めませんでした。\n正しい値をダイアログより指定してください.");
	}

	if (success)
	{
		m_filePath = fnames[0];

		if( trgtId == 10 && !strangePitch)
		{
			System::Windows::Forms::MessageBox::Show("Dicom Sliceを読み込みました。\n dcm属性値より画像のスタック方向を自動決定しました。\n（念のため左右反転の有無を確認してください。）");
		}
		else
		{
      int flg = RoiPainter3D::formStackOrder_showModalDialog();
			if (flg == 1) t_flipVolumeInZ<short>( m_Reso[0], m_Reso[1], m_Reso[2], m_volOrig);
		}
	}
	else 
	{
		m_filePath = "";
		t_LoadDefoultVolume(m_Reso, m_Pitch, m_volOrig);
	}


	// post loading 
	short minV, maxV;
	t_getMaxMinOfArray<short>( m_Reso[0]* m_Reso[1] * m_Reso[2], m_volOrig, minV, maxV);

	m_volOrigGM = new float[m_Reso[0]* m_Reso[1] * m_Reso[2]];
	m_vol    .Allocate( m_Reso );
	m_volFlg .Allocate( m_Reso );
	m_volMsk .Allocate( m_Reso );
	m_volGmag.Allocate( m_Reso );
	
	m_vol   .SetValue(m_volOrig, minV, maxV);
	m_volFlg.SetAllZero();
	m_volMsk.SetAllZero();

	m_maskData.clear();
	m_maskData.push_back( MaskData("bckGrnd", EVec3i(0,0,0), 0, 0 ) );

  m_volMinMax  << minV, maxV;

	updateGradVolume();

  System::Windows::Forms::MessageBox::Show("TODO UPDATE GRAPHCUT"); 
	//ModeSegGCut::getInst()->newVolLoaded();

  return true;
}


void ImageCore::updateGradVolume()
{
	const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];

	memset( m_volOrigGM, 0, sizeof(float) * N);
	t_sobel3D<short>( m_Reso[0], m_Reso[1], m_Reso[2], m_volOrig, m_volOrigGM);

	float minV, maxV;
	t_getMaxMinOfArray<float>( N, m_volOrigGM, minV, maxV);

	const float c = 255.0f;
	for (int i = 0; i < N; ++i)
	{
		m_volOrigGM[i] = (m_volOrigGM[i] - minV) / (maxV - minV);
		m_volGmag[i] = (byte)( 255 * m_volOrigGM[i] ) ;
	}

	m_volGmag.SetUpdated();
}


void ImageCore::updateVisVolume(short winLvMin,  short winLvMax)
{
  m_vol.SetValue( m_volOrig, (short)winLvMin, (short)winLvMax);
}







/////////////////////////////////////////////////////////////
//MASK IO////////////////////////////////////////////////////


void ImageCore::loadMask(const char *fname)
{
	FILE* fp = fopen(fname, "rb");
	
	//save mask image
	int version, W,H,D;
	fread(&version, sizeof(int), 1, fp);
	fread(&W      , sizeof(int), 1, fp);
	fread(&H      , sizeof(int), 1, fp);
	fread(&D      , sizeof(int), 1, fp);

	if(W != m_Reso[0] || H != m_Reso[1] || D != m_Reso[2] )
	{
		RoiPainter3D::CLI_MessageBox_OK_Show( "strange volume size\n", "caution");
		fclose( fp );
		return;
	}

  //read mask voxels
	fread( &m_volMsk[0], sizeof(byte), W * H * D, fp);


  int flg = RoiPainter3D::formStackOrder_showModalDialog();
  if (flg == 1) m_volMsk.FlipInZ();

	m_volMsk.SetUpdated();

  //read mask status
	int maskN;
	fread(&maskN, sizeof(int), 1, fp);

	m_maskData.clear();

	for (int i=0; i < maskN; ++i)
	{
		int lock, nLen;
		int col[3];
		double alpha;
		fread(&alpha, sizeof(double), 1, fp);
		fread(col   , sizeof(int   ), 3, fp);
		fread(&lock , sizeof(int   ), 1, fp);

		fread(&nLen , sizeof(int   ), 1, fp);
		char *name = new char[nLen + 1];
		fread(name, sizeof(char), nLen + 1, fp);

		printf("%d %s\n", nLen, name);

		m_maskData.push_back( MaskData(string(name), EVec3i(col[0],col[1],col[2]), alpha, 0, lock?true:false) );

		delete[] name; 
	}
	fclose(fp);
}


void ImageCore::saveMask( const char* fname)
{
  FILE* fp = fopen(fname, "wb");

  //save mask image
  int version = 0;
  fwrite(&version  , sizeof(int), 1, fp);
  fwrite(&m_Reso[0], sizeof(int), 1, fp);
  fwrite(&m_Reso[1], sizeof(int), 1, fp);
  fwrite(&m_Reso[2], sizeof(int), 1, fp);
  fwrite(&m_volMsk[0], sizeof(byte), m_Reso[0] * m_Reso[1] * m_Reso[2], fp);

	int maskN = (int)m_maskData.size();
	fwrite(&maskN, sizeof(int), 1, fp);

	for( const auto &it : m_maskData )
	{
		int iLock = it.lock;
		fwrite(&it.alpha      , sizeof(double), 1, fp);
		fwrite(it.color.data(), sizeof(int)   , 3, fp);
		fwrite(&iLock, sizeof(int), 1, fp);

		int nLen = (int)it.name.length();
		fwrite(&nLen          , sizeof(int ),  1    , fp);
		fwrite(it.name.c_str(), sizeof(char), nLen+1, fp);

		printf("%d %s\n", nLen, it.name.c_str());
	}
	fclose(fp);
}





//////////////////////////////////////////////////////////
//Mask Manipulation///////////////////////////////////////

using namespace RoiPainter3D;


void ImageCore::selectedMsk_delete  ()
{
  if (m_maskSelectedId < 0 || m_maskData.size() <= m_maskSelectedId) return;

  if (m_maskSelectedId == 0)
  {
    CLI_MessageBox_OK_Show("0th region (background) cannot be removed", "caution");
    return;
  }

  if (CLI_MessageBox_YESNO_Show("Do you want to DELETE the selected mask?", "caution") == false) return;


  const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];

  for (int i = 0; i < N; ++i)
  {
    if (     m_volMsk[i] >  m_maskSelectedId) m_volMsk[i]--;
    else if (m_volMsk[i] == m_maskSelectedId) m_volMsk[i] = 0;
  }

  m_maskData.erase( m_maskData.begin() + m_maskSelectedId );
  m_maskSelectedId = 0;
}


void ImageCore::selectedMsk_marge   (const int &trgtMaskID)
{
  if (m_maskSelectedId < 0 || m_maskData.size() <= m_maskSelectedId) return;

  if (m_maskSelectedId == 0 || trgtMaskID == 0)
  {
    CLI_MessageBox_OK_Show("0th region (background) cannot be marged", "caution");
    return;
  }

  if (trgtMaskID == m_maskSelectedId)
  {
    CLI_MessageBox_OK_Show("Cannot marge to it self", "caution");
    return;
  }


  fprintf(stderr, "active %d, trgt, %d\n", m_maskSelectedId, trgtMaskID);
  const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];

  for (int i = 0; i < N; ++i) if ( m_volMsk[i] == trgtMaskID) m_volMsk[i] = m_maskSelectedId;
  for (int i = 0; i < N; ++i) if ( m_volMsk[i] >  trgtMaskID) --m_volMsk[i];

  if (m_maskSelectedId > trgtMaskID) --m_maskSelectedId;

  m_maskData.erase(m_maskData.begin() + trgtMaskID);

  m_maskData[ m_maskSelectedId ].surf.clear();
  m_maskData[ m_maskSelectedId ].bRendSurf = false;
}


void ImageCore::selectedMsk_erode()
{
  if ( m_maskSelectedId <= 0 || m_maskData.size() <= m_maskSelectedId) return;

  printf( "mask erode...\n");
  
  const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];

  byte* flgs = new byte[N];

  for (int i = 0; i < N; ++i) flgs[i] = (m_volMsk[i] == m_maskSelectedId) ? 255 : 1;
  t_erode3D( m_Reso[0], m_Reso[1], m_Reso[2], flgs);
  for (int i = 0; i < N; ++i) if (m_volMsk[i] == m_maskSelectedId && flgs[i] == 1) m_volMsk[i] = 0;

  delete[] flgs;

  printf( "mask erode...DONE\n");
}



void ImageCore::selectedMsk_dilate  ()
{
  if ( m_maskSelectedId <= 0 || m_maskData.size() <= m_maskSelectedId) return;

  printf( "mask dilate...\n");
  const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];

  byte* flgs = new byte[N];
  for (int i = 0; i < N; ++i) {
    flgs[i] = (m_volMsk[i] == m_maskSelectedId) ? 255 :
              (m_maskData[m_volMsk[i]].lock   ) ? 0 : 1;
  }

  t_dilate3D( m_Reso[0], m_Reso[1], m_Reso[2], flgs);

  for (int i = 0; i < N; ++i) {
    if (flgs[i] == 255) m_volMsk[i] = m_maskSelectedId;
  }

  delete[] flgs;
  printf( "mask dilate...DONE\n");
}



void ImageCore::selectedMsk_fillHole()
{
  if ( m_maskSelectedId <= 0 || m_maskData.size() <= m_maskSelectedId) return;

  printf( "mask fillhole...\n");
  const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];
  
  byte* flgs = new byte[ N ]; //0:back, 255:trgt_mask_id

  for (int i = 0; i < N; ++i){
    flgs[i] = (m_volMsk[i] == m_maskSelectedId) ? 255 : 0;
  } 

  t_fillHole3D(m_Reso[0], m_Reso[1], m_Reso[2], flgs);

  for (int i = 0; i < N; ++i)
  {
    if (flgs[i] == 255 && m_volMsk[i] == 0 ) m_volMsk[i] = m_maskSelectedId;
  }

  delete[] flgs;

  fprintf(stderr, "fillhole...DONE\n");
}



void ImageCore::selectedMsk_expObj  (const string &fname)
{
  if ( m_maskSelectedId <= 0 || m_maskData.size() <= m_maskSelectedId) return;

  const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];

  short *v = new short[N];
  for (int i = 0; i < N; ++i) v[i] = (m_volMsk[i] == m_maskSelectedId) ? 255 : 0;

  TMesh mesh;
  marchingcubes::t_MarchingCubes(m_Reso, m_Pitch, v, 128, 0, 0, mesh);
  mesh.smoothing(2);
  mesh.exportObjNoTexCd(fname.c_str());

  delete[] v;
}



static const int ColPalletN = 14;
static const EVec3i ColPallet[ColPalletN] = {
	EVec3i(255,0  ,0) , EVec3i(0  ,0,255), 
	EVec3i(0,255,255 ), EVec3i(255,0,255), EVec3i(255,255,0 ),
	EVec3i(0, 128,128), EVec3i(128,0,128), EVec3i(128,128, 0), 
	EVec3i(255,128,0) , EVec3i(0,255,128), EVec3i(128,0,255 ), 
	EVec3i(128,255,0) , EVec3i(0,128,255), EVec3i(255, 0, 128) , 
	
};



//mask_storeCurrentForeGround
//generate new region by using all voxels with (m_volFlg[i] == 255) 
void ImageCore::mask_storeCurrentForeGround()
{
	const int id = (int) m_maskData.size();
	const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];

	for (int i = 0; i < N; ++i) if( m_volFlg[i] == 255 )  m_volMsk[i] = id;
	m_volMsk.SetUpdated();

	//initial color
	static int c = 1;
	c++;
	//store new region
	m_maskData.push_back( MaskData("region", ColPallet[c%ColPalletN], 0.1, false, true) );
}


void ImageCore::selectedMsk_setLock(const bool tf)
{
	if( m_maskSelectedId < 0 || m_maskData.size() <= m_maskSelectedId ) return;
	m_maskData[m_maskSelectedId].lock = tf;
}

void ImageCore::selectedMsk_setAlpha(const double alpha)
{
	if( m_maskSelectedId < 0 || m_maskData.size() <= m_maskSelectedId ) return;
	m_maskData[m_maskSelectedId].alpha = alpha;
}

void ImageCore::selectedMsk_setColor(const EVec3i &c)
{
	if( m_maskSelectedId < 0 || m_maskData.size() <= m_maskSelectedId ) return;
	m_maskData[m_maskSelectedId].color = c;
}


void ImageCore::selectedMsk_setRendSurf(const bool tf)
{
	if( m_maskSelectedId < 0 || m_maskData.size() <= m_maskSelectedId ) return;
	MaskData &trgtMsk = m_maskData[m_maskSelectedId];

	trgtMsk.bRendSurf = tf;
	
	if( tf == true && trgtMsk.surf.m_vSize == 0)
	{
		const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];

		short *v = new short[N];

		for( int i=0; i < N; ++i ) v[i] = ( m_volMsk[i] == m_maskSelectedId ) ? 255 : 0;

    marchingcubes::t_MarchingCubes(m_Reso, m_Pitch, v, 128, 0, 0, trgtMsk.surf);
		trgtMsk.surf.smoothing(2);

		delete[] v;
	}
}




