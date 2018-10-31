#pragma once

#include "OglForCLI.h"
#include "tqueue.h"
#include "timageloader.h"

enum OGL_IMAGE_CH
{
  CH_INTES = 1,
  CH_RGBA = 4
};



template<class T>
void t_flipVolumeInZ(const int W, const int H, const int D, T* vol)
{
  const int WH = W*H;

  T *tmp = new T[WH];

  for (int z = 0; z < D / 2; ++z)
  {
    memcpy(tmp, &vol[z       * WH], sizeof(T) * WH);
    memcpy(&vol[z       * WH], &vol[(D - 1 - z) * WH], sizeof(T) * WH);
    memcpy(&vol[(D - 1 - z) * WH], tmp, sizeof(T) * WH);
  }
  delete[] tmp;

}


//voxel value 0:never change, 1:background, 255:foreground
template <class T>
inline void t_erode3D(
    const int &W,
    const int &H,
    const int &D,
    T *vol)
{
  const int WH = W*H, WHD = W*H*D;

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

  for (int i = 0; i < WHD; ++i) if (vol[i] == 2) vol[i] = 1;

}


//voxel value 0:never cahnge, 1:background, 255:foreground
template <class T>
inline void t_dilate3D(
    const int &W,
    const int &H,
    const int &D,
    T *vol)
{
  const int WH = W*H, WHD = W*H*D;

  for (int z = 0; z < D; ++z)
  {
    for (int y = 0; y < H; ++y)
    {
      for (int x = 0; x < W; ++x)
      {
        int idx = x + y * W + z*WH;
        if (vol[idx] != 1) continue;

        if ((x > 0 && vol[idx - 1] == 255) || (y > 0 && vol[idx - W] == 255) || (z > 0 && vol[idx - WH] == 255) ||
          (x < W - 1 && vol[idx + 1] == 255) || (y < H - 1 && vol[idx + W] == 255) || (z < D - 1 && vol[idx + WH] == 255)) vol[idx] = 2;
      
      }
    }
  }

  for (int i = 0; i < WHD; ++i) if (vol[i] == 2) vol[i] = 255;

}



//TODO takashi double check the places that uses this function
//voxel value 0: background, 255:foreground
template <class T>
inline void t_fillHole3D(
    const int &W,
    const int &H,
    const int &D,
    T *vol)
{
  const int WH = W*H, WHD = W*H*D;


  TQueue<EVec3i> Q(WHD / 10);

  for (int y = 0; y < H; ++y)
    for (int x = 0; x < W; ++x)
    {
      if (vol[x + y * W + 0 * WH] == 0)      Q.push_back(EVec3i(x, y, 0));
      if (vol[x + y * W + (D - 1)*WH] == 0)  Q.push_back(EVec3i(x, y, (D - 1)));
    }

  for (int z = 0; z < D; ++z)
    for (int y = 0; y < H; ++y)
    {
      if (vol[0 + y * W + z*WH] == 0)        Q.push_back(EVec3i(0, y, z));
      if (vol[(W - 1) + y * W + z*WH] == 0)  Q.push_back(EVec3i(W - 1, y, z));
    }

  for (int z = 0; z < D; ++z)
    for (int x = 0; x < W; ++x)
    {
      if (vol[x + 0 * W + z*WH] == 0)       Q.push_back(EVec3i(x, 0, z));
      if (vol[x + (H - 1)* W + z*WH] == 0)  Q.push_back(EVec3i(x, H - 1, z));
    }

  for (int i = 0; i < Q.size(); ++i) vol[Q[i].x() + Q[i].y()*W + Q[i].z()*WH] = 2;



  //region growing for background
  while (!Q.empty())
  {
    const int x = Q.front().x();
    const int y = Q.front().y();
    const int z = Q.front().z();
    const int I = x + y*W + z*WH;
    Q.pop_front();

    if (x != 0 && !vol[I - 1]) { vol[I - 1] = 2;   Q.push_back(EVec3i(x - 1, y, z)); }
    if (x != W - 1 && !vol[I + 1]) { vol[I + 1] = 2;   Q.push_back(EVec3i(x + 1, y, z)); }
    if (y != 0 && !vol[I - W]) { vol[I - W] = 2;   Q.push_back(EVec3i(x, y - 1, z)); }
    if (y != H - 1 && !vol[I + W]) { vol[I + W] = 2;   Q.push_back(EVec3i(x, y + 1, z)); }
    if (z != 0 && !vol[I - WH]) { vol[I - WH] = 2;  Q.push_back(EVec3i(x, y, z - 1)); }
    if (z != D - 1 && !vol[I + WH]) { vol[I + WH] = 2;  Q.push_back(EVec3i(x, y, z + 1)); }
  }

  for (int i = 0; i < WHD; ++i) vol[i] = (vol[i] == 2) ? 0 : 255;

}



/*
//voxel value 0: none change, 1: background, 255:foreground
template <class T>
inline void t_fillHole3D(
  const int &W,
  const int &H,
  const int &D,
  T *vol
)
{
  const int WH = W*H, WHD = W*H*D;


  TQueue<EVec3i> Q(WHD / 10);

  for (int y = 0; y < H; ++y)
    for (int x = 0; x < W; ++x)
    {
      if (vol[x + y * W + 0 * WH] == 1)      Q.push_back(EVec3i(x, y, 0));
      if (vol[x + y * W + (D - 1)*WH] == 1)  Q.push_back(EVec3i(x, y, (D - 1)));
    }

  for (int z = 0; z < D; ++z)
    for (int y = 0; y < H; ++y)
    {
      if (vol[0 + y * W + z*WH] == 1)        Q.push_back(EVec3i(0, y, z));
      if (vol[(W - 1) + y * W + z*WH] == 1)  Q.push_back(EVec3i(W - 1, y, z));
    }

  for (int z = 0; z < D; ++z)
    for (int x = 0; x < W; ++x)
    {
      if (vol[x + 0 * W + z*WH] == 1)       Q.push_back(EVec3i(x, 0, z));
      if (vol[x + (H - 1)* W + z*WH] == 1)  Q.push_back(EVec3i(x, H - 1, z));
    }

  for (int i = 0; i < Q.size(); ++i) vol[Q[i].x() + Q[i].y()*W + Q[i].z()*WH] = 2;



  //region growing for background
  while (!Q.empty())
  {
    const int x = Q.front().x();
    const int y = Q.front().y();
    const int z = Q.front().z();
    const int I = x + y*W + z*WH;
    Q.pop_front();

    if (x != 0     && vol[I - 1] == 1) { vol[I - 1] = 2;   Q.push_back(EVec3i(x - 1, y, z)); }
    if (x != W - 1 && vol[I + 1] == 1) { vol[I + 1] = 2;   Q.push_back(EVec3i(x + 1, y, z)); }
    if (y != 0     && vol[I - W] == 1) { vol[I - W] = 2;   Q.push_back(EVec3i(x, y - 1, z)); }
    if (y != H - 1 && vol[I + W] == 1) { vol[I + W] = 2;   Q.push_back(EVec3i(x, y + 1, z)); }
    if (z != 0     && vol[I - WH]== 1) { vol[I - WH] = 2;  Q.push_back(EVec3i(x, y, z - 1)); }
    if (z != D - 1 && vol[I + WH]== 1) { vol[I + WH] = 2;  Q.push_back(EVec3i(x, y, z + 1)); }
  }

  for (int i = 0; i < WHD; ++i) {
    if (vol[i] == 1)     vol[i] = 255;
    else if(vol[i] == 2) vol[i] = 1;
  }
}
*/












// single channel volume image
// Note oglBindについて
//   - bindのタイミングで bUpdated == trueなら unbindする
//   - unbindを忘れるとGRAMでleakするので注意


class OglImage3D
{
private:
  GLuint name_gpu_;
  EVec3i resolution_;
  bool   is_updated_;
  GLubyte *volume_;

public:


  ~OglImage3D()
  {
    if (volume_) delete[] volume_;
  }


  OglImage3D()
  {
    volume_     = 0;
    name_gpu_   = -1;
    is_updated_ = true;
    resolution_ = EVec3i(0, 0, 0);
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
    resolution_ = src.resolution_;
    if (src.volume_)
    {
      int N = resolution_[0] * resolution_[1] * resolution_[2];
      volume_ = new GLubyte[N];
      memcpy(volume_, src.volume_, sizeof(GLubyte) * N);
    }
  }

public:
  void Allocate(const int W, const int H, const int D)
  {
    if (volume_) delete[] volume_;
    resolution_ = EVec3i(W, H, D);
    volume_     = new GLubyte[resolution_[0] * resolution_[1] * resolution_[2]];
    is_updated_ = true;
  }

  void Allocate(const EVec3i reso)
  {
    Allocate(reso[0], reso[1], reso[2]);
  }




  //should be wglMakeCurrent
  void BindOgl(bool bInterpolate = true)
  {
    if (is_updated_)
    {
      UnbindOgl();
      is_updated_ = false;
    }

    if (name_gpu_ == -1 || !glIsTexture(name_gpu_))
    {
      //generate textrue
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      glGenTextures(1, &name_gpu_);

      glBindTexture(GL_TEXTURE_3D, name_gpu_);
      glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE8, resolution_[0], resolution_[1], resolution_[2], 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, volume_);
    }
    else
    {
      //use the previous texture on GPU
      glBindTexture(GL_TEXTURE_3D, name_gpu_);
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
    if (name_gpu_ != -1 && glIsTexture(name_gpu_)) glDeleteTextures(1, &name_gpu_);
    name_gpu_ = -1;
  }


  // should be allocated & the size of v is samge as m_res
  template<class T>
  void SetValue(const T* v, const T minV, const T maxV)
  {
    if (volume_ == 0) return;

    float rate = 255.0f / (maxV - minV);
    int N = resolution_[0] * resolution_[1] * resolution_[2];

#pragma omp parallel for 
    for (int i = 0; i < N; ++i) volume_[i] = (byte)max(0, min(255, (v[i] - minV) * rate));

    is_updated_ = true;
  }

  void SetValue(byte *v)
  {
    if (volume_ == 0) return;
    memcpy(volume_, v, sizeof(byte) * resolution_[0] * resolution_[1] * resolution_[2]);
    is_updated_ = true;
  }


  void SetValue(float** slices, const float minV, const float maxV)
  {
    if (volume_ == 0) return;

    float rate = 255.0f / (maxV - minV);
    int WH = resolution_[0] * resolution_[1];

    for (int z = 0; z < resolution_[2]; ++z)
    {
      for (int i = 0; i < WH; ++i)
      {
        volume_[z * WH + i] = (byte)max(0, min(255, (slices[z][i] - minV) * rate));
      }
    }

    is_updated_ = true;
  }

  void SetAllZero()
  {
    memset(volume_, 0, sizeof(GLubyte) * resolution_[0] * resolution_[1] * resolution_[2]);
    is_updated_ = true;
  }

  inline GLubyte& operator[](int i) { return volume_[i]; }
  inline GLubyte  operator[](int i) const { return volume_[i]; }

  inline GLubyte GetV(const int x, const int y, const int z) const
  {
    return volume_[x + y * resolution_[0] + z * resolution_[0] * resolution_[1]];
  }

  inline GLubyte SetV(const int x, const int y, const int z, GLubyte v)
  {
    volume_[x + y * resolution_[0] + z * resolution_[0] * resolution_[1]] = v;
    is_updated_ = true;
  }

  inline void SetUpdated() { is_updated_ = true; }

  const int GetW() { return resolution_[0]; }
  const int GetH() { return resolution_[1]; }
  const int GetD() { return resolution_[2]; }
  GLubyte* GetVolumePtr() { return volume_; }

  void FlipInZ() {
    t_flipVolumeInZ<byte>(resolution_[0], resolution_[1], resolution_[2], volume_);
  }
};



//voxel value 0:never change, 1:background, 255:foreground
inline void t_morpho3D_erode(OglImage3D &v)
{
  const int W = v.GetW();
  const int H = v.GetH();
  const int D = v.GetD();

  t_erode3D(W, H, D, v.GetVolumePtr());

  v.SetUpdated();
}



//voxel value 0:never cahnge, 1:background, 255:foreground
inline void t_morpho3D_dilate(OglImage3D &v)
{
  const int W = v.GetW();
  const int H = v.GetH();
  const int D = v.GetD();

  t_dilate3D(W, H, D, v.GetVolumePtr());

  v.SetUpdated();
}




//voxel value 0: background, 255:foreground
inline void t_morpho3D_FillHole(OglImage3D &v)
{
  const int W = v.GetW();
  const int H = v.GetH();
  const int D = v.GetD();

  t_fillHole3D(W, H, D, v.GetVolumePtr());

  v.SetUpdated();
}









//2D image 
template <OGL_IMAGE_CH CH>
class OglImage2D
{
protected:
  GLubyte *image_;
  GLuint   name_gpu_;
  EVec2i   resolution_;
  bool     is_updated_;

public:
  ~OglImage2D()
  {
    if (image_) delete[] image_;
  }

  OglImage2D()
  {
    resolution_ << 0, 0;
    image_      = 0;
    name_gpu_   = -1;
    is_updated_ = true;
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
  inline GLubyte& operator[](int i) { return image_[i]; }
  inline GLubyte  operator[](int i) const { return image_[i]; }


  int GetW() const { return resolution_[0]; }
  int GetH() const { return resolution_[1]; }

private:
  void Set(const OglImage2D &src)
  {
    resolution_ = src.resolution_;
    //m_oglName = src.m_oglName; コピーしない
    if (src.image_)
    {
      m_rgba = new GLubyte[resolution_[0] * resolution_[1] * CH];
      memcpy(image_, src.image_, sizeof(GLubyte) * resolution_[0] * resolution_[1] * CH);
    }
  }
public:

  void Allocate(const int W, const int H)
  {
    if (image_) delete[] image_;
    resolution_ << W, H;
    image_ = new GLubyte[resolution_[0] * resolution_[1] * CH];
    is_updated_ = true;
  }

  //bmp/png/tif/jpeg (only 24 bit color image) are supported
  //画像左上が原点となるので、OpenGL利用の際はflipする
  bool Allocate(const char *fname);
  bool SaveAs(const char *fname, int flg_BmpJpgPngTiff);


  //should be wglMakeCurrent
  void UnbindOgl()
  {
    if (name_gpu_ != -1 && glIsTexture(name_gpu_)) glDeleteTextures(1, &name_gpu_);
    name_gpu_ = -1;
  }

  void FlipInY()
  {
    const int W = resolution_[0];
    const int H = resolution_[1];
    byte *tmp = new byte[W * H * CH];
    for (int y = 0; y < H; ++y) memcpy(&tmp[CH*(H - 1 - y)*W], &image_[4 * y* W], sizeof(byte) * CH * W);
    memcpy(image_, tmp, sizeof(byte) * W * H * CH);
    delete[] tmp;
    is_updated_ = true;
  }


  //should be wglMakeCurrent
  void BindOgl(bool bInterpolate = true)
  {
    if (is_updated_)
    {
      UnbindOgl();
      is_updated_ = false;
    }

    if (name_gpu_ == -1 || !glIsTexture(name_gpu_))
    {
      //generate textrue
      glGenTextures(1, &name_gpu_);
      glBindTexture(GL_TEXTURE_2D, name_gpu_);
      SendImageToGPU();
    }
    else
    {
      //use the previous texture on GPU
      glBindTexture(GL_TEXTURE_2D, name_gpu_);
    }

    //CHに異存、ここにあったらだめ　glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    GLint param = bInterpolate ? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
  }

  bool IsAllocated() { return image_ ? true : false; }
private:
  void SendImageToGPU();
};




inline void OglImage2D<CH_RGBA>::SendImageToGPU()
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resolution_[0], resolution_[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, image_);
}

inline void OglImage2D<CH_INTES>::SendImageToGPU()
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE8, resolution_[0], resolution_[1], 0, GL_LUMINANCE8, GL_UNSIGNED_BYTE, image_);
}


inline bool OglImage2D<CH_RGBA>::Allocate(const char *fname)
{
  if ( image_ != 0) delete[] image_;
  name_gpu_ = -1;
  image_ = 0;
  is_updated_ = true;
  return t_loadImage(fname, resolution_[0], resolution_[1], image_);
}



inline bool OglImage2D<CH_RGBA>::SaveAs(const char *fname, int flg_BmpJpgPngTiff)
{
  t_saveImage(fname, resolution_[0], resolution_[1], image_);
  return true;
}







// rgba 1D image 
template <OGL_IMAGE_CH CH>
class OglImage1D
{
protected:
  GLubyte *image_;
  GLuint   name_gpu_;
  int      resolution_;
  bool     is_updated_;

public:
  ~OglImage1D()
  {
    if (image_) delete[] image_;
  }

  OglImage1D()
  {
    resolution_ = 0;
    image_      = 0;
    name_gpu_   = -1;
    is_updated_ = true;
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
    resolution_ = src.resolution_;
    // m_oglName = src.m_oglName;
    if (src.image_)
    {
      m_rgba = new GLubyte[resolution_ * CH];
      memcpy(image_, src.image_, sizeof(GLubyte) * resolution_ * CH);
    }
  }


public:
  void Allocate(const int N)
  {
    if (image_) delete[] image_;
    resolution_ = N;
    image_ = new GLubyte[resolution_ * CH];
  }

  void AllocateHeuImg(const int N)
  {
    Allocate(N);
    const float S = N / 6.0f;
    Allocate(N);
    for (int i = 0; i < N; ++i)
    {
      EVec3f C;
      if (i < 1 * S) C << 1, (i - 0 * S) / S, 0;
      else if (i < 2 * S) C << 1 - (i - 1 * S) / S, 1, 0;
      else if (i < 3 * S) C << 0, 1, (i - S * 2) / S;
      else if (i < 4 * S) C << 0, 1 - (i - S * 3) / S, 1;
      else if (i < 5 * S) C << (i - 4 * S) / S, 0, 1;
      else if (i < 6 * S) C << 1, 0, 1 - (i - S * 3) / S;
      image_[4 * i] = (byte)(C[0] * 255);
      image_[4 * i + 1] = (byte)(C[1] * 255);
      image_[4 * i + 2] = (byte)(C[2] * 255);
    }

  }


  //should be wglMakeCurrent
  void UnbindOgl()
  {
    if (name_gpu_ != -1 && glIsTexture(name_gpu_)) glDeleteTextures(1, &name_gpu_);
    name_gpu_ = -1;
  }


  //should be wglMakeCurrent
  void BindOgl(bool bInterpolate = true)
  {
    if (is_updated_)
    {
      UnbindOgl();
      is_updated_ = false;
    }

    if (name_gpu_ == -1 || !glIsTexture(name_gpu_))
    {
      //generate textrue
      glGenTextures(1, &name_gpu_);
      glBindTexture(GL_TEXTURE_1D, name_gpu_);
      SendImageToGPU();
    }
    else
    {
      //use the previous texture on GPU
      glBindTexture(GL_TEXTURE_1D, name_gpu_);
    }

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    GLint param = bInterpolate ? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, param);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, param);
  }

  inline GLubyte& operator[](int i) { return image_[i]; }
  inline GLubyte  operator[](int i) const { return image_[i]; }

  inline void SetZero();
  void SetUpdated() { is_updated_ = true; }
private:
  void SendImageToGPU();
};

inline void OglImage1D<CH_RGBA>::SendImageToGPU()
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, resolution_, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_);
}
inline void OglImage1D<CH_INTES>::SendImageToGPU()
{
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_LUMINANCE8, resolution_, 0, GL_LUMINANCE8, GL_UNSIGNED_BYTE, image_);
}


inline void OglImage1D<CH_INTES>::SetZero()
{
  is_updated_ = true;
  memset(image_, 0, sizeof(byte) * resolution_);
}

inline void OglImage1D<CH_RGBA>::SetZero()
{
  is_updated_ = true;
  memset(image_, 0, sizeof(byte) * resolution_ * 4);
}

typedef OglImage1D<CH_RGBA > OGLImage1D4;
typedef OglImage1D<CH_INTES> OGLImage1D1;
typedef OglImage2D<CH_RGBA > OGLImage2D4;
typedef OglImage2D<CH_INTES> OGLImage2D1;





template<class T>
void t_sobel3D(const int W, const int H, const int D, const T* vol, T* res)
{
  const T sblX[3][3][3] = { 
    { {-1,  0, +1},
      {-2,  0, +2},
      {-1,  0, +1} }, { {-2,  0,  2},
                        {-4,  0,  4},
                        {-2,  0,  2} }, { {-1,  0,  1},
                                          {-2,  0,  2},
                                          {-1,  0,  1}} };
  static T sblY[3][3][3] = { 
    {{-1, -2, -1},
    { 0,  0,  0},
    { 1,  2,  1}},   {{-2, -4, -2},
                     { 0,  0,  0},
                     { 2,  4,  2}}, {{-1, -2, -1},
                                    { 0,  0,  0},
                                    { 1,  2,  1}} };
  static T sblZ[3][3][3] = { 
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

    T gx = 0, gy = 0, gz = 0;

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
    res[i] = (T)sqrt((double)gx * gx + gy * gy + gz * gz) / 16.0f;

    if (res[i] > 40000) fprintf(stderr, "%d %d %d\n", x, y, z);
  }
}



