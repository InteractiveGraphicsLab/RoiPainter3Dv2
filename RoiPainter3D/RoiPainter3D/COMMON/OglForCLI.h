
// Copyright (c) 2017, Takashi Ijiri, Tomofumi Narita, Chika Tomiyama
// All rights reserved.
//
//
// This file is released under GNU GPLv3.
//
//
// This class uses Eigen 3.2.8 released under MPL2 without any modification.
// see http://eigen.tuxfamily.org/index.php?title=Main_Page#License
// 
// This class uses Glew released under the Modified BSD
// see http://glew.sourceforge.net/credits.html
// see http://glew.sourceforge.net/glew.txt
//
// Start development at 2017/7/7




#pragma once
#pragma unmanaged


#include <windows.h> 

#include "gl/glew.h"
#include <gl/gl.h> 
#include <gl/glu.h> 
#include "tmath.h"


class OglForCLI
{
private:
  HDC   hdc_;
  HGLRC hglrc_;

  // Camera position/center/Up direction 
  EVec3f cam_position_, camcenter_, cam_updir_;

  // View Size
  bool   is_rendering_;
  EVec2i mouse_position_;
  EVec4f background_color_;


  enum {
    BTN_NON, BTN_TRANS, BTN_ZOOM, BTN_ROT
  } mousebtn_state_;


public:

  ~OglForCLI() {}

  OglForCLI(HDC dc)
  {
    if (dc == 0) return;

    cam_position_     = EVec3f(0, 0, 10);
    camcenter_        = EVec3f(0, 0, 0 );
    cam_updir_        = EVec3f(0, 1, 0 );
    background_color_ = EVec4f(0, 0, 0, 0.5);

    is_rendering_ = false;

    SetDefaultProperties();


    hdc_ = dc;
    static PIXELFORMATDESCRIPTOR pfd =
    {
      sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
      1,                              // version number
      PFD_DRAW_TO_WINDOW |            // support window
      PFD_SUPPORT_OPENGL |            // support OpenGL
      PFD_DOUBLEBUFFER,               // double buffered
      PFD_TYPE_RGBA,                  // RGBA type
      32,                             // 24-bit color depth
      0, 0, 0, 0, 0, 0,               // color bits ignored
      0,                              // no alpha buffer
      0,                              // shift bit ignored
      0,                              // no accumulation buffer
      0, 0, 0, 0,                     // accum bits ignored
      32,                             // 32-bit z-buffer (or 16, 24 bit)
      0,                              // stencil buffer (no)
      0,                              // auxiliary buffer (no)
      PFD_MAIN_PLANE,                 // main layer
      0,                              // reserved
      0, 0, 0                         // layer masks ignored
    };


    int pfmt = ChoosePixelFormat(hdc_, &pfd);
    if (pfmt == 0) return;
    if (!SetPixelFormat(hdc_, pfmt, &pfd)) return;

    // pure Managed だとランタイムでエラーに 
    if ( (hglrc_ = wglCreateContext(hdc_) ) == 0) return;
    if ( (wglMakeCurrent(hdc_, hglrc_)    ) == 0) return;



    GLenum err = glewInit();
    if (err != GLEW_OK) 
    {
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

    wglMakeCurrent(0, 0);

    return;
  }

  void oglMakeCurrent() const
  {
    wglMakeCurrent(hdc_, hglrc_);
  }


  void OnDrawBegin(
    int viewW, 
    int viewH, 
    double fovY = 45.0, 
    double view_near = 0.02, 
    double view_far = 700.0)
  {
    if (is_rendering_) return;

    is_rendering_ = true;
    oglMakeCurrent();

    glViewport(0, 0, viewW, viewH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovY, viewW / (double)viewH, view_near, view_far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(cam_position_[0], cam_position_[1], cam_position_[2],
      camcenter_[0], camcenter_[1], camcenter_[2],
      cam_updir_[0], cam_updir_[1], cam_updir_[2]);
    glClearColor((float)background_color_[0], (float)background_color_[1], (float)background_color_[2], (float)background_color_[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
  }

  void OnDrawEnd()
  {
    glFinish();
    SwapBuffers(hdc_);
    wglMakeCurrent(NULL, NULL);
    is_rendering_ = false;
  }








  inline bool   isDrawing() const { return is_rendering_; }
  inline EVec3f GetCamPos() const { return cam_position_; }
  inline EVec3f GetCamCnt() const { return camcenter_; }
  inline EVec3f GetCamUp()  const { return cam_updir_; }
  inline void   SetCam(const EVec3f &pos, const EVec3f &cnt, const EVec3f &up) { cam_position_ = pos; camcenter_ = cnt; cam_updir_ = up; }
  inline void   SetBgColor(EVec4f bg) { background_color_ = bg; }
  inline void   SetBgColor(float r, float g, float b, float a) { background_color_ << r, g, b, a; }


  //Mouse Listener for Camera manipuration
  void BtnDown_Trans(const EVec2i &p)
  {
    mouse_position_ = p;
    mousebtn_state_ = BTN_TRANS;
  }
  void BtnDown_Zoom(const EVec2i &p)
  {
    mouse_position_ = p;
    mousebtn_state_ = BTN_ZOOM;
  }
  void BtnDown_Rot(const EVec2i &p)
  {
    mouse_position_ = p;
    mousebtn_state_ = BTN_ROT;
  }

  void BtnUp()
  {
    mousebtn_state_ = BTN_NON;
    ReleaseCapture();
  }



  void MouseMove(const EVec2i &p)
  {
    if (mousebtn_state_ == BTN_NON) return;

    float dX = (float)(p[0] - mouse_position_[0]);
    float dY = (float)(p[1] - mouse_position_[1]);

    if (mousebtn_state_ == BTN_ROT)
    {
      float theta = -dX / 200.0f;
      float phi = -dY / 200.0f;

      EVec3f axis = ((camcenter_ - cam_position_).cross(cam_updir_)).normalized();
      Eigen::AngleAxisf rotTheta = Eigen::AngleAxisf(theta, cam_updir_);
      Eigen::AngleAxisf rotPhi = Eigen::AngleAxisf(phi, axis);
      cam_updir_ = rotPhi * rotTheta * cam_updir_;
      cam_position_ = rotPhi * rotTheta * (cam_position_ - camcenter_) + camcenter_;
    }
    else if (mousebtn_state_ == BTN_ZOOM)
    {
      EVec3f newEyeP = cam_position_ + dY / 80.0f * (camcenter_ - cam_position_);
      if ((newEyeP - camcenter_).norm() > 0.02f) cam_position_ = newEyeP;
    }
    else if (mousebtn_state_ == BTN_TRANS)
    {
      float c = (cam_position_ - camcenter_).norm() / 900.0f;
      EVec3f Xdir = ((cam_position_ - camcenter_).cross(cam_updir_)).normalized();
      EVec3f t = c * dX * Xdir + c * dY * cam_updir_;
      cam_position_ += t;
      camcenter_ += t;
    }
    mouse_position_ = p;
  }

  /*

  void OnDestroy()
  {
    oglMakeCurrent();
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(m_hRC);
    if (m_pDC) delete m_pDC;
  }
*/
  void ZoomCam(float distance)
  {
    EVec3f rayD = (camcenter_ - cam_position_);
    float  len = rayD.norm();

    if (distance > len) return;
    rayD /= len;
    cam_position_ = cam_position_ + distance * rayD;
  }


  void unProject_correctY(
    const double cx, 
    const double cy, 
    const double depth, 
    double &x, 
    double &y, 
    double &z) const
  {
    if (!is_rendering_) oglMakeCurrent();
    double modelMat[16], projMat[16];
    int vp[4];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMat);
    glGetDoublev(GL_PROJECTION_MATRIX, projMat);
    glGetIntegerv(GL_VIEWPORT, vp);

    gluUnProject(cx, vp[3] - cy, depth, modelMat, projMat, vp, &x, &y, &z);

    if (!is_rendering_) wglMakeCurrent(NULL, NULL);
  }



  void Project(
    const double   inX, 
    const double   inY, 
    const double   inZ,
    double &outX, 
    double &outY,
    double &outZ) const
  {
    if (!is_rendering_) oglMakeCurrent();
    double model[16], proj[16];
    int vp[4];
    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    glGetIntegerv(GL_VIEWPORT, vp);

    gluProject(inX, inY, inZ, model, proj, vp, &outX, &outY, &outZ);
    if (!is_rendering_) wglMakeCurrent(NULL, NULL);
  }



  inline void GetCursorRay(
    int cx, 
    int cy, 
    EVec3f &rayPos, 
    EVec3f &rayDir) const
  {
    double x1, y1, z1, x2, y2, z2;
    unProject_correctY(cx, cy, 0.01, x1, y1, z1);
    unProject_correctY(cx, cy, 0.2, x2, y2, z2);

    rayPos << (float)x1, (float)y1, (float)z1;
    rayDir << (float)(x2 - x1), (float)(y2 - y1), (float)(z2 - z1);
    rayDir.normalize();
  }

  inline void GetCursorRay(const EVec2i &pt, EVec3f &rayPos, EVec3f &rayDir) const
  {
    GetCursorRay(pt[0], pt[1], rayPos, rayDir);
  }

private:

  void SetDefaultProperties()
  {
    glClearDepth(1.0f);

    //Material 
    float   shin[1] = { 64 };
    EVec4f  spec(1, 1, 1, 0.5), diff(0.5f, 0.5f, 0.5f, 0.5f), ambi(0.5f, 0.5f, 0.5f, 0.5f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec.data());
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff.data());
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambi.data());
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shin);

    //lights
    glEnable(GL_LIGHTING);
    EVec4f lPosi[3] = { EVec4f(1000,1000,-1000,1), EVec4f(-1000,1000,-1000,1), EVec4f(1000,-1000,-1000,1) };
    EVec4f lambi[3] = { EVec4f(1.0f,1.0f,1.0f,1), EVec4f(0,0,0,0)            , EVec4f(0,0,0,0) };
    EVec4f ldiff[3] = { EVec4f(1.0f,1.0f,1.0f,1), EVec4f(0.5f,0.5f,0.5f,1)   , EVec4f(0.5f,0.5f,0.5f,1) };
    EVec4f lspec[3] = { EVec4f(0.3f,0.3f,0.3f,1), EVec4f(0.3f,0.3f,0.3f,1)   , EVec4f(0.3f,0.3f,0.3f,1) };

    for (int i = 0; i < 3; ++i)
    {
      GLenum light = (i == 0) ? GL_LIGHT0 : (i == 1) ? GL_LIGHT1 : GL_LIGHT2;
      glLightfv(light, GL_POSITION, lPosi[i].data());
      glLightfv(light, GL_AMBIENT, lambi[i].data());
      glLightfv(light, GL_DIFFUSE, ldiff[i].data());
      glLightfv(light, GL_SPECULAR, lspec[i].data());
      glEnable(light);
    }

    // other general states
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glShadeModel(GL_SMOOTH);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glPolygonMode(GL_FRONT, GL_FILL);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
  }

};

#pragma managed
