// RoiPainter3D.cpp : メイン プロジェクト ファイルです。

#include "FormMain.h"
#include <stdio.h>
#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

using namespace System;
using namespace RoiPainter3D;


[STAThreadAttribute]
int main()
{
  printf("start main function\n");

  //Initialize non-managed Singletons
  //非managedなシングルトンはここで初期化
  //だたしこのタイミングでは、Formを生成しshowしてはだめ．
    
  ModeCore  ::getInst();
  ImageCore ::getInst();
  CrssecCore::getInst();

  /*
  ModeVizMask::getInst();
  ModeVizNormal::getInst();
  ModeSegRGrow::getInst();
  ModeSegPixPaint::getInst();
  ModeSegRigidICP::GetInstance();
  ModeSegClosestPix::getInst();
  ModeSegLocalRGrow::getInst();
  ModeRefStrokeTrim::getInst();
  */

  printf("FormMain::getInst()->ShowDialog() \n");
  FormMain::getInst()->ShowDialog();
  
  printf("aaaa FormMain::getInst()->ShowDialog() \n");

  return 0;
}



// UPDATE Memo
// mode_vis_norm   OK
//   implement overall workflow    OK
//   support pixel val vis         OK
//   support gradient magnitude    OK
//   support gradient magnitude tf OK
//
// modeVisMask
//   implement overall workflow
//   formVisMask
//   maskManipulations   
//   I/O
//
// modeSegRgrow
//
//
//
// CORE_CLASSES
//   ImageCore     OK 
//   CrssecCore    OK
//   ViewAngleCore OK
//   ModeCore      50%
//
// 
// event handler (MainForm): 
//    Wheeling OK
//    mouse    OK
//    resize   80% --> set dialog position when resizing
//    repaint  OK
//
// 
//
//
// IO
//   initial volume    OK
//   2D slices bmp/tif OK
//   2D slices dcm     OK
//   3D traw3D         OK
//   3D dcm            OK
//   save / load mask  
//   export traw3d_ss  
//   open fav          
//   save fav          
//   show_dlg for stack orientation
//   
// etc...


