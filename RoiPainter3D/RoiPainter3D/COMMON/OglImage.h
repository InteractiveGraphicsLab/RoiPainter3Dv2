#pragma once

#include "OglForCLI.h"
#include "tqueue.h"
#include "timageloader.h"



enum OGL_IMAGE_CH
{
  CH_INTES = 1,
  CH_RGBA  = 4
};


template<class T>
void t_FlipVolumeInZ(const int W, const int H, const int D, T* vol)
{
  const int WH = W*H;

  T *tmp = new T[WH];

  for (int z = 0; z < D / 2; ++z)
  {
    memcpy( tmp               , &vol[z * WH]          , sizeof(T) * WH);
    memcpy( &vol[z * WH]      , &vol[(D - 1 - z) * WH], sizeof(T) * WH);
    memcpy( &vol[(D-1-z) * WH], tmp                   , sizeof(T) * WH);
  }
  delete[] tmp;

}






// class OglImage3D
//
// single channel volume image
// Note : bindのタイミングについて
//  - bindのタイミングで bUpdated == trueなら unbindする
//  - unbindを忘れるとGRAMでleakするので注意

class OglImage3D
{
private:
  GLuint m_name_gpu;
  EVec3i m_resolution;
  bool   m_is_updated;
  GLubyte *m_volume;

public:


  ~OglImage3D()
  {
    if (m_volume) delete[] m_volume;
  }


  OglImage3D()
  {
    m_volume     = 0;
    m_name_gpu   = -1;
    m_is_updated = true;
    m_resolution = EVec3i(0, 0, 0);
  }

  OglImage3D(const OglImage3D &src)
  {
    Set(src);
  }

  OglImage3D &operator=(const OglImage3D &src)
  {
    Set(src);
    return *this;
  }

private:
  void Set(const OglImage3D &src)
  {
    m_resolution = src.m_resolution;
    if (src.m_volume)
    {
      int N = m_resolution[0] * m_resolution[1] * m_resolution[2];
      m_volume = new GLubyte[N];
      memcpy(m_volume, src.m_volume, sizeof(GLubyte) * N);
    }
  }

public:
  void Allocate(const int W, const int H, const int D)
  {
    if ( m_volume ) delete[] m_volume;
    m_resolution = EVec3i(W, H, D);
    m_volume     = new GLubyte[m_resolution[0] * m_resolution[1] * m_resolution[2]];
    m_is_updated = true;
  }

  void Allocate(const EVec3i resolution)
  {
    Allocate(resolution[0], resolution[1], resolution[2]);
  }




  //should be wglMakeCurrent
  void BindOgl(bool bInterpolate = true)
  {
    if (m_is_updated)
    {
      UnbindOgl();
      m_is_updated = false;
    }

    if (m_name_gpu == -1 || !glIsTexture(m_name_gpu))
    {
      //generate textrue
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      glGenTextures(1, &m_name_gpu);

      glBindTexture(GL_TEXTURE_3D, m_name_gpu);
      glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE8, m_resolution[0], m_resolution[1], m_resolution[2], 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_volume);
    }
    else
    {
      //use the previous texture on GPU
      glBindTexture(GL_TEXTURE_3D, m_name_gpu);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    GLint param = bInterpolate ? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, param);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, param);
  }


  //should be wglMakeCurrent
  void UnbindOgl()
  {
    if (m_name_gpu != -1 && glIsTexture(m_name_gpu)) glDeleteTextures(1, &m_name_gpu);
    m_name_gpu = -1;
  }


  // should be allocated & the size of v is samge as m_res
  template<class T>
  void SetValue(const T* v, const T minV, const T maxV)
  {
    if (m_volume == 0) return;

    float rate = 255.0f / (maxV - minV);
    int N = m_resolution[0] * m_resolution[1] * m_resolution[2];

#pragma omp parallel for 
    for (int i = 0; i < N; ++i) m_volume[i] = (byte)max(0.f, min(255.f, (v[i] - minV) * rate));

    m_is_updated = true;
  }

  void SetValue(byte *v)
  {
    if (m_volume == 0) return;
    memcpy(m_volume, v, sizeof(byte) * m_resolution[0] * m_resolution[1] * m_resolution[2]);
    m_is_updated = true;
  }


  void SetValue(float** slices, const float minV, const float maxV)
  {
    if (m_volume == 0) return;

    float rate = 255.0f / (maxV - minV);
    int WH = m_resolution[0] * m_resolution[1];

    for (int z = 0; z < m_resolution[2]; ++z)
    {
      for (int i = 0; i < WH; ++i)
      {
        m_volume[z * WH + i] = (byte)std::max(0.f, std::min(255.f, (slices[z][i] - minV) * rate));
      }
    }

    m_is_updated = true;
  }

  void SetAllZero()
  {
    memset(m_volume, 0, sizeof(GLubyte) * m_resolution[0] * m_resolution[1] * m_resolution[2]);
    m_is_updated = true;
  }

  inline GLubyte& operator[](int i) { return m_volume[i]; }
  inline GLubyte  operator[](int i) const { return m_volume[i]; }

  inline GLubyte GetValue(const int x, const int y, const int z) const
  {
    return m_volume[x + y * m_resolution[0] + z * m_resolution[0] * m_resolution[1]];
  }

  inline GLubyte SetValue(const int x, const int y, const int z, GLubyte v)
  {
    m_volume[x + y * m_resolution[0] + z * m_resolution[0] * m_resolution[1]] = v;
    m_is_updated = true;
  }

  inline void SetUpdated() { 
    m_is_updated = true; 
  }

  int GetW() const { return m_resolution[0]; }
  int GetH() const { return m_resolution[1]; }
  int GetD() const { return m_resolution[2]; }
  GLubyte* GetVolumePtr() { return m_volume; }

  void FlipInZ() {
    t_FlipVolumeInZ<byte>(m_resolution[0], m_resolution[1], m_resolution[2], m_volume);
  }
};









//2D image
//
// CH = 1 : grayscale image 
// CH = 4 : rgba image
//
template <OGL_IMAGE_CH CH>
class OglImage2D
{
protected:
  GLubyte *m_image;
  GLuint   m_name_gpu;
  EVec2i   m_resolution;
  bool     m_is_updated;

public:
  ~OglImage2D()
  {
    if (m_image) delete[] m_image;
  }

  OglImage2D()
  {
    m_resolution << 0, 0;
    m_image      = 0;
    m_name_gpu   = -1;
    m_is_updated = true;
  }

  OglImage2D(const OglImage2D &src)
  {
    Set(src);
  }

  OglImage2D &operator=(const OglImage2D &src)
  {
    Set(src);
    return *this;
  }
  inline GLubyte& operator[](int i) { return m_image[i]; }
  inline GLubyte  operator[](int i) const { return m_image[i]; }


  int GetW() const { return m_resolution[0]; }
  int GetH() const { return m_resolution[1]; }

private:
  void Set(const OglImage2D &src)
  {
    m_resolution = src.m_resolution;
    //m_oglName = src.m_oglName; コピーしない
    if (src.m_image)
    {
      m_rgba = new GLubyte[m_resolution[0] * m_resolution[1] * CH];
      memcpy(m_image, src.m_image, sizeof(GLubyte) * m_resolution[0] * m_resolution[1] * CH);
    }
  }


public:
  void Allocate(const int W, const int H)
  {
    if (m_image) delete[] m_image;
    m_resolution << W, H;
    m_image = new GLubyte[m_resolution[0] * m_resolution[1] * CH];
    m_is_updated = true;
  }

  //bmp/png/tif/jpeg (only 24 bit color image) are supported
  //画像左上が原点となるので、OpenGL利用の際はflipする
  bool Allocate(const char *fname);
  bool SaveAs(const char *fname);


  //should be wglMakeCurrent
  void UnbindOgl()
  {
    if (m_name_gpu != -1 && glIsTexture(m_name_gpu)) glDeleteTextures(1, &m_name_gpu);
    m_name_gpu = -1;
  }

 
  //should be wglMakeCurrent
  void BindOgl(bool bInterpolate = true)
  {
    if (m_is_updated)
    {
      UnbindOgl();
      m_is_updated = false;
    }

    if (m_name_gpu == -1 || !glIsTexture(m_name_gpu))
    {
      //generate textrue
      glGenTextures(1, &m_name_gpu);
      glBindTexture(GL_TEXTURE_2D, m_name_gpu);
      SendImageToGPU();
    }
    else
    {
      //use the previous texture on GPU
      glBindTexture(GL_TEXTURE_2D, m_name_gpu);
    }

    //CHに異存、ここにあったらだめ　glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    GLint param = bInterpolate ? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
  }

   void FlipInY()
  {
    const int W = m_resolution[0];
    const int H = m_resolution[1];
    byte *tmp = new byte[W * H * CH];
    for (int y = 0; y < H; ++y) memcpy(&tmp[CH*(H - 1 - y)*W], &m_image[4 * y* W], sizeof(byte) * CH * W);
    memcpy(m_image, tmp, sizeof(byte) * W * H * CH);
    delete[] tmp;
    m_is_updated = true;
  }

  void SetGrayValue_normalize( float* image );
  void SetIntensity(const int x, const int y, byte c);

  bool IsAllocated() { return m_image ? true : false; }
private:
  void SendImageToGPU();
};




inline void OglImage2D<CH_RGBA>::SendImageToGPU()
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution[0], m_resolution[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image);
}


inline void OglImage2D<CH_INTES>::SendImageToGPU()
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, m_resolution[0], m_resolution[1], 0, GL_LUMINANCE8, GL_UNSIGNED_BYTE, m_image);
}


inline bool OglImage2D<CH_RGBA>::Allocate(const char *fname)
{
  if ( m_image != 0) delete[] m_image;
  m_name_gpu = -1;
  m_image = 0;
  m_is_updated = true;
  return t_loadImage(fname, m_resolution[0], m_resolution[1], m_image);
}


inline bool OglImage2D<CH_RGBA>::SaveAs(const char *fname)
{
  t_saveImage(fname, m_resolution[0], m_resolution[1], m_image);
  return true;
}


inline bool OglImage2D<CH_INTES>::SaveAs(const char *fname)
{
  t_saveImage_gray(fname, m_resolution[0], m_resolution[1], m_image);
  return true;
}

inline void OglImage2D<CH_RGBA>::SetGrayValue_normalize(float* image)
{
  const int W = m_resolution[0];
  const int H = m_resolution[1];
  int N = W * H;
  float minV = +FLT_MAX;
  float maxV = -FLT_MAX;

  for( int i=0; i < N; ++i ){
    minV = std::min( minV, image[i]);
    maxV = std::max( maxV, image[i]);
  }
  for( int i=0; i < N; ++i ){
    float v = (image[i] - minV) / (maxV - minV);
    m_image[4*i+0] = m_image[4*i+1] = m_image[4*i+2] = m_image[4*i+3] = (byte)(255 * v); 
  }
}

inline void OglImage2D<CH_RGBA>::SetIntensity(
    const int x, 
    const int y, 
    byte c)
{
  int i = 4 * ( x + y * m_resolution[0]); 
  m_image[i] = m_image[i+1] = m_image[i+2] = c;
}




// rgba 1D image 
template <OGL_IMAGE_CH CH>
class OglImage1D
{
protected:
  GLubyte *m_image;
  GLuint   m_name_gpu;
  int      m_resolution;
  bool     m_is_updated;

public:
  ~OglImage1D()
  {
    if (m_image) delete[] m_image;
  }

  OglImage1D()
  {
    m_resolution =  0;
    m_image      =  0;
    m_name_gpu   = -1;
    m_is_updated = true;
  }

  OglImage1D(const OglImage1D &src)
  {
    Set(src);
  }

  OglImage1D &operator=(const OglImage1D &src)
  {
    Set(src);
    return *this;
  }

private:
  void Set(const OglImage1D &src)
  {
    m_resolution = src.m_resolution;
    // m_name_gpu = src.m_name_gpu;
    if (src.m_image)
    {
      m_rgba = new GLubyte[m_resolution * CH];
      memcpy(m_image, src.m_image, sizeof(GLubyte) * m_resolution * CH);
    }
  }


public:
  void Allocate(const int N)
  {
    if (m_image) delete[] m_image;
    m_resolution = N;
    m_image = new GLubyte[m_resolution * CH];
  }

  
  void Allocate( const char *fname)
  {
    if (m_image) delete[] m_image;
    m_image = 0;
    m_resolution = 0;

    int W, H;
    GLubyte* rgba;
    if ( t_loadImage(fname, W,H, rgba) )
    {
      Allocate(W);
      for( int i=0; i < W; ++i) memcpy( &m_image[CH*i], &rgba[CH*i], sizeof(byte) * CH );
      delete[] rgba;
    }
  }


  void AllocateHeuImg(const int N)
  {
    Allocate(N);
    const float S = N / 6.0f;
    Allocate(N);
    for (int i = 0; i < N; ++i)
    {
      EVec3f C;
      if      (i < 1 * S) C << 1, (i - 0 * S) / S, 0;
      else if (i < 2 * S) C << 1 - (i - 1 * S) / S, 1, 0;
      else if (i < 3 * S) C << 0, 1, (i - S * 2) / S;
      else if (i < 4 * S) C << 0, 1 - (i - S * 3) / S, 1;
      else if (i < 5 * S) C << (i - 4 * S) / S, 0, 1;
      else if (i < 6 * S) C << 1, 0, 1 - (i - S * 3) / S;
      m_image[4 * i] = (byte)(C[0] * 255);
      m_image[4 * i + 1] = (byte)(C[1] * 255);
      m_image[4 * i + 2] = (byte)(C[2] * 255);
    }
  }


  //should be wglMakeCurrent
  void UnbindOgl()
  {
    if (m_name_gpu != -1 && glIsTexture(m_name_gpu)) glDeleteTextures(1, &m_name_gpu);
    m_name_gpu = -1;
  }


  //should be wglMakeCurrent
  void BindOgl(bool bInterpolate = true)
  {
    if (m_is_updated)
    {
      UnbindOgl();
      m_is_updated = false;
    }

    if (m_name_gpu == -1 || !glIsTexture(m_name_gpu))
    {
      //generate textrue
      glGenTextures(1, &m_name_gpu);
      glBindTexture(GL_TEXTURE_1D, m_name_gpu);
      SendImageToGPU();
    }
    else
    {
      //use the previous texture on GPU
      glBindTexture(GL_TEXTURE_1D, m_name_gpu);
    }

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    GLint param = bInterpolate ? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, param);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, param);
  }

  inline GLubyte& operator[](int i) { return m_image[i]; }
  inline GLubyte  operator[](int i) const { return m_image[i]; }

  inline void SetZero();
  void SetUpdated() { m_is_updated = true; }
  int GetResolution() { return m_resolution; }
private:
  void SendImageToGPU();
};

inline void OglImage1D<CH_RGBA>::SendImageToGPU()
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, m_resolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image );
}
inline void OglImage1D<CH_INTES>::SendImageToGPU()
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_LUMINANCE8, m_resolution, 0, GL_LUMINANCE8, GL_UNSIGNED_BYTE, m_image);
}


inline void OglImage1D<CH_INTES>::SetZero()
{
  m_is_updated = true;
  memset(m_image, 0, sizeof(byte) * m_resolution);
}

inline void OglImage1D<CH_RGBA>::SetZero()
{
  m_is_updated = true;
  memset(m_image, 0, sizeof(byte) * m_resolution * 4);
}

typedef OglImage1D<CH_RGBA > OGLImage1D4;
typedef OglImage1D<CH_INTES> OGLImage1D1;
typedef OglImage2D<CH_RGBA > OGLImage2D4;
typedef OglImage2D<CH_INTES> OGLImage2D1;











//voxel value 0:never change, 1:background, 255:foreground
template <class T>
inline void t_Erode3D(
    const int &W,
    const int &H,
    const int &D,
    T *vol)
{
  const int WH = W*H, WHD = W*H*D;

#pragma omp parallel for 
  for (int z = 0; z < D; ++z)
  {
    for (int y = 0; y < H; ++y)
    {
      for (int x = 0; x < W; ++x)
      {
        int idx = x + y * W + z*WH;
        if (vol[idx] != 255) continue;

        if (x == 0 || y == 0 || z == 0 || x == W - 1 || y == H - 1 || z == D - 1 ||
            vol[idx - 1] <= 1 || vol[idx - W] <= 1 || vol[idx - WH] <= 1 ||
            vol[idx + 1] <= 1 || vol[idx + W] <= 1 || vol[idx + WH] <= 1)
        {
          vol[idx] = 2;
        }
      }
    }
  }

#pragma omp parallel for 
  for (int i = 0; i < WHD; ++i) if (vol[i] == 2) vol[i] = 1;

}


//voxel value 0:never change, 1:background, 255:foreground
template <class T>
inline void t_Erode3D(
    const EVec3i &resolution,
    T *vol)
{
  t_Erode3D<T>(resolution[0], resolution[1], resolution[2], vol);
}





//voxel value 0:never cahnge, 1:background, 255:foreground
template <class T>
inline void t_Dilate3D(
    const int &W,
    const int &H,
    const int &D,
    T *vol)
{
  const int WH = W*H, WHD = W*H*D;

#pragma omp parallel for 
  for (int z = 0; z < D; ++z)
  {
    for (int y = 0; y < H; ++y)
    {
      for (int x = 0; x < W; ++x)
      {
        int idx = x + y * W + z*WH;
        if (vol[idx] != 1) continue;

        if ((x >  0  && vol[idx-1] == 255) || (x < W-1 && vol[idx+1] == 255) || 
            (y >  0  && vol[idx-W] == 255) || (y < H-1 && vol[idx+W] == 255) || 
            (z >  0  && vol[idx-WH]== 255) || (z < D-1 && vol[idx+WH]== 255))
        {
          vol[idx] = 2;
        }
      }
    }
  }

#pragma omp parallel for 
  for (int i = 0; i < WHD; ++i) if (vol[i] == 2) vol[i] = 255;

}


//voxel value 0:never cahnge, 1:background, 255:foreground
template <class T>
inline void t_Dilate3D(
    const EVec3i &resolution,
    T *vol)
{
  t_Dilate3D<T>(resolution[0], resolution[1], resolution[2], vol);
}





template <class T>
inline bool __FH_Growbk6( 
  const int &x, const int &y, const int &z, 
  const int &W, const int &WH, T* vol)
{
  const int i = x + y*W + z*WH; 

  if ( (vol[i] == 0) && 
       (vol[i-1]==2 || vol[i-W]==2 || vol[i-WH]==2 || 
        vol[i+1]==2 || vol[i+W]==2 || vol[i+WH]==2 ) )
  {
    vol[i] = 2;
    return true;
  }
  return false;
}




//voxel value 0: background, 255:foreground
template <class T>
inline void t_FillHole3D_6(
    const int &W,
    const int &H,
    const int &D,
    T *vol)
{
  const int WH = W*H, WHD = W*H*D;


  //calc bounding box
  EVec3i min_idx(W,H,D), max_idx(0,0,0);

  for (int z = 0; z < D; ++z)
  {
    for (int y = 0; y < H; ++y)
    {
      for (int x = 0; x < W; ++x)
      {
        if ( vol[x+y*W+z*WH] == 255 )
        {
          min_idx[0] = std::min(min_idx[0], x);
          min_idx[1] = std::min(min_idx[1], y);
          min_idx[2] = std::min(min_idx[2], z);
          max_idx[0] = std::max(max_idx[0], x);
          max_idx[1] = std::max(max_idx[1], y);
          max_idx[2] = std::max(max_idx[2], z);
        }
      }
    }
  }
  
  TQueue<int> Q ( WHD / 3);

  for (int z = 0; z < D; ++z)
  {
    for (int y = 0; y < H; ++y)
    {
      for (int x = 0; x < W; ++x)
      {
        //paint outside
        if ( x < min_idx[0] || max_idx[0] < x ||
             y < min_idx[1] || max_idx[1] < y ||
             z < min_idx[2] || max_idx[2] < z )  
        {
          vol[x+y*W+z*WH] = 2;
        }
        else if ( 
             x == min_idx[0] || max_idx[0] == x ||
             y == min_idx[1] || max_idx[1] == y ||
             z == min_idx[2] || max_idx[2] == z )  
        {
          vol[x+y*W+z*WH] = 2;
          Q.push_back(x+y*W+z*WH);
        }
      }
    }
  }
  Trace(min_idx);
  Trace(max_idx);

  //region growing for background
  while (!Q.empty())
  {
    const int I = Q.front();
    Q.pop_front();
    const int z = (I) / WH;
    const int y = (I - z * WH) / W;
    const int x = (I - z * WH - y * W);

    if (x != 0   && !vol[I - 1 ]) { vol[I-1 ] = 2; Q.push_back(I-1 ); }
    if (x != W-1 && !vol[I + 1 ]) { vol[I+1 ] = 2; Q.push_back(I+1 ); }
    if (y != 0   && !vol[I - W ]) { vol[I-W ] = 2; Q.push_back(I-W ); }
    if (y != H-1 && !vol[I + W ]) { vol[I+W ] = 2; Q.push_back(I+W ); }
    if (z != 0   && !vol[I - WH]) { vol[I-WH] = 2; Q.push_back(I-WH); }
    if (z != D-1 && !vol[I + WH]) { vol[I+WH] = 2; Q.push_back(I+WH); }
  }

  for (int i = 0; i < WHD; ++i) vol[i] = (vol[i] == 2) ? 0 : 255;
}


 /*
  const int WH = W*H, WHD = W*H*D;

  //周囲を塗る
  for (int y = 0; y < H; ++y)
    for (int x = 0; x < W; ++x)
    {
      if (vol[x + y * W +   0  * WH] == 0) vol[x + y * W +   0  * WH] = 2;
      if (vol[x + y * W + (D-1)* WH] == 0) vol[x + y * W + (D-1)* WH] = 2;
    }

  for (int z = 0; z < D; ++z)
    for (int y = 0; y < H; ++y)
    {
      if (vol[  0   + y * W + z*WH] == 0) vol[  0   + y * W + z*WH] = 2;
      if (vol[(W-1) + y * W + z*WH] == 0) vol[(W-1) + y * W + z*WH] = 2;
    }

  for (int z = 0; z < D; ++z)
    for (int x = 0; x < W; ++x)
    {
      if (vol[x +   0  * W + z*WH] == 0) vol[x +   0  * W + z*WH] = 2;
      if (vol[x + (H-1)* W + z*WH] == 0) vol[x + (H-1)* W + z*WH] = 2;
    }

  //ラスタ順のgrowth (5回だけ)
  for ( int i=0; i < 5; ++i)
  {
    bool tf = false;

    for ( int z = 1; z < D-1; ++z ) 
    {
      for ( int y = 1; y < H-1; ++y ) 
      {
        for ( int x = 1; x < W-1; ++x ) tf |= __FH_Growbk6(x,y,z,W,WH,vol);
        for ( int x = W-2; x > 0; --x ) tf |= __FH_Growbk6(x,y,z,W,WH,vol);
      }
      for ( int y = H-2; y > 0; --y ) 
      {
        for ( int x = 1; x < W-1; ++x ) tf |= __FH_Growbk6(x,y,z,W,WH,vol);
        for ( int x = W-2; x > 0; --x ) tf |= __FH_Growbk6(x,y,z,W,WH,vol);
      }
    }

    for ( int z = D-2; z > 0; --z ) 
    {
      for ( int y = 1; y < H-1; ++y ) 
      {
        for ( int x = 1; x < W-1; ++x ) tf |= __FH_Growbk6(x,y,z,W,WH,vol);
        for ( int x = W-2; x > 0; --x ) tf |= __FH_Growbk6(x,y,z,W,WH,vol);
      }

      for ( int y = H-2; y > 0; --y ) 
      {
        for ( int x = 1; x < W-1; ++x ) tf |= __FH_Growbk6(x,y,z,W,WH,vol);
        for ( int x = W-2; x > 0; --x ) tf |= __FH_Growbk6(x,y,z,W,WH,vol);
      }
    }

    if ( !tf ) break;
  }

  //region growing方式で背景を成長させる
  TQueue<EVec3i> Q ( WHD / 10);

  for (int z = 1; z < D-1; ++z)
    for (int y = 1; y < H-1; ++y)
      for (int x = 1; x < W-1; ++x)
      {
        int i = x + y*W + z*WH;
        if ( vol[i] != 0 ) continue;
        if ( vol[i-1] == 2 || vol[i-W] == 2 || vol[i-WH] == 2 || 
             vol[i+1] == 2 || vol[i+W] == 2 || vol[i+WH] == 2 )
        {
          vol[i] = 2;
          Q.push_back(EVec3i(x,y,z));
        }
      }

  //region growing for background
  while (!Q.empty())
  {
    const int x = Q.front().x();
    const int y = Q.front().y();
    const int z = Q.front().z();
    const int I = x + y*W + z*WH;
    Q.pop_front();

    if (x != 0   && !vol[I - 1 ]) { vol[I - 1 ] = 2;  Q.push_back(EVec3i(x - 1, y, z)); }
    if (x != W-1 && !vol[I + 1 ]) { vol[I + 1 ] = 2;  Q.push_back(EVec3i(x + 1, y, z)); }
    if (y != 0   && !vol[I - W ]) { vol[I - W ] = 2;  Q.push_back(EVec3i(x, y - 1, z)); }
    if (y != H-1 && !vol[I + W ]) { vol[I + W ] = 2;  Q.push_back(EVec3i(x, y + 1, z)); }
    if (z != 0   && !vol[I - WH]) { vol[I - WH] = 2;  Q.push_back(EVec3i(x, y, z - 1)); }
    if (z != D-1 && !vol[I + WH]) { vol[I + WH] = 2;  Q.push_back(EVec3i(x, y, z + 1)); }
  }

  for (int i = 0; i < WHD; ++i) vol[i] = (vol[i] == 2) ? 0 : 255;
  */






//voxel value 0:never change, 1:background, 255:foreground
inline void t_Erode3D(OglImage3D &v)
{
  t_Erode3D(v.GetW(), v.GetH(), v.GetD(), v.GetVolumePtr());
  v.SetUpdated();
}

//voxel value 0:never cahnge, 1:background, 255:foreground
inline void t_Dilate3D(OglImage3D &v)
{
  t_Dilate3D(v.GetW(), v.GetH(), v.GetD(), v.GetVolumePtr());
  v.SetUpdated();
}

//voxel value 0: background, 255:foreground
inline void t_FillHole3D_6(OglImage3D &v)
{
  t_FillHole3D_6(v.GetW(), v.GetH(), v.GetD(), v.GetVolumePtr());
  v.SetUpdated();
}






template<class T1, class T2>
void t_Sobel3D(const int W, const int H, const int D, const T1* vol, T2* res)
{
  const T1 sblX[3][3][3] = { 
    { {-1,  0, +1},
      {-2,  0, +2},
      {-1,  0, +1} }, { {-2,  0,  2},
                        {-4,  0,  4},
                        {-2,  0,  2} }, { {-1,  0,  1},
                                          {-2,  0,  2},
                                          {-1,  0,  1}} };
  static T1 sblY[3][3][3] = { 
    {{-1, -2, -1},
    { 0,  0,  0},
    { 1,  2,  1}},   {{-2, -4, -2},
                     { 0,  0,  0},
                     { 2,  4,  2}}, {{-1, -2, -1},
                                    { 0,  0,  0},
                                    { 1,  2,  1}} };
  static T1 sblZ[3][3][3] = { 
    {{-1, -2, -1},
    {-2, -4, -2},
    {-1, -2, -1}},  {{ 0,  0,  0},
                     { 0,  0,  0},
                     { 0,  0,  0}}, {{ 1,  2,  1},
                                     { 2,  4,  2},
                                     { 1,  2,  1}} };

  const int WH = W*H, WHD = WH*D;

#pragma omp parallel for
  for (int i = 0; i < WHD; ++i)
  {
    const int z = (i) / WH;
    const int y = (i - z * WH) / W;
    const int x = (i - z * WH - y * W);

    double gx = 0, gy = 0, gz = 0;

    for (int zz = -1; zz < 2; ++zz) if (0 <= z + zz && z + zz < D)
      for (int yy = -1; yy < 2; ++yy) if (0 <= y + yy && y + yy < H)
        for (int xx = -1; xx < 2; ++xx) if (0 <= x + xx && x + xx < W)
        {
          int I = i + xx + yy*W + zz*WH;
          gx += sblX[zz + 1][yy + 1][xx + 1] * vol[I];
          gy += sblY[zz + 1][yy + 1][xx + 1] * vol[I];
          gz += sblZ[zz + 1][yy + 1][xx + 1] * vol[I];
        }

    //for boundary voxels
    if (x == 0 || x == W - 1) gx = 0;
    if (y == 0 || y == H - 1) gy = 0;
    if (z == 0 || z == D - 1) gz = 0;
    res[i] = (T2)sqrt(gx * gx + gy * gy + gz * gz) / 16.0f;

    if (res[i] > 40000) std::cout << x << " " << y << " " << z << "\n";
  }
}



