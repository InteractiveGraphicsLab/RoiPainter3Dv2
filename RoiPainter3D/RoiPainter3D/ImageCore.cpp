#include "ImageCore.h"

#include "./COMMON/tmath.h"
#include "./3rdParty/dcmtk/tdcmtk.h"
#include "./3rdparty/vvv/ddsbase.h"


ImageCore::ImageCore()
{
  fprintf(stderr, "CONSTRUCTOR ImageCore ...\n");

  
	m_volOrig = 0;

	loadVolume("init","init");

	m_imgMskCol.Allocate(256);

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

		fprintf( stderr, "(%d/%d)", z, D);
	}


	if( pitch[2] < 0)
	{
		fprintf( stderr, "flip in z\n");
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

	fprintf( stderr, "resolution %d %d %d\n", reso[0], reso[1], reso[2]);

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

	fprintf( stderr, "load pvm %d %d %d %d\n", W,H,D,components);

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
      System::Windows::Forms::MessageBox::Show("TODO SHOW STACK DIALOG"); 
			//DlgSetStackOrder dlg;
			//if (IDOK != dlg.DoModal()) t_flipVolumeInZ<float>( m_Reso[0], m_Reso[1], m_Reso[2], m_volOrig);
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

  System::Windows::Forms::MessageBox::Show("TODO UPDATE_VIS_PARAM_DLG"); 
	//m_dlg.NewVolumeLoaded(m_Reso, m_Pitch, EVec2d(minV, maxV));
	//m_dlg.HistogramUpdated(HIST_BIN_SIZE, m_histVol, m_histGmag);

  System::Windows::Forms::MessageBox::Show("TODO INIT_CAMERA_POS / UPDATE GRAPHCUT"); 
	//CRoiPaintView::getInst()->initCameraPos( getCuboidF() );
	//ModeSegGCut::getInst()->newVolLoaded();

  return true;
}



/*

void ImageCore::updateHistogram()
{
	const int N = m_Reso[0] * m_Reso[1] * m_Reso[2];

	for (int i = 0; i < HIST_BIN_SIZE; ++i) m_histGmag[i] = 0;
	for (int i = 0; i < HIST_BIN_SIZE; ++i) m_histVol [i] = 0;
	
	for (int i = 0; i < N; ++i) 
	{
		m_histGmag[ m_volGmag[i] ] += 1;
		m_histVol [ m_vol    [i] ] += 1;
	}

	for (int i = 0; i < HIST_BIN_SIZE  ; ++i) 
	{
		m_histGmag[i] = (float) log( 1.0 + m_histGmag[i] );
	}

	float maxV = 0, maxG = 0;
	for (int i = 5; i < HIST_BIN_SIZE-5; ++i) maxV = max(maxV , m_histVol [i]);
	for (int i = 5; i < HIST_BIN_SIZE-5; ++i) maxG = max(maxG , m_histGmag[i]);
	for (int i = 0; i < HIST_BIN_SIZE  ; ++i) m_histVol [i] /= maxV;
	for (int i = 0; i < HIST_BIN_SIZE  ; ++i) m_histGmag[i] /= maxG;

}
*/

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





