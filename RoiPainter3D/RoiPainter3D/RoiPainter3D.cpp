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
// mode_vis_norm   
//   implement overall workflow
//   support pixel val vis
//   support gradient magnitude
//   support gradient magnitude tf
//
// ImageCore     OK 
// CrssecCore    OK
// ViewAngleCore OK
// ModeCore      50%
//
// event handler (MainForm): 
//    Wheeling OK
//    mouse    OK
//    resize   80% --> set dialog position when resizing
//    repaint  OK
//
//
//TODO  
// event handler
// mode_vis_mask
// mode_seg_rgrow
//
// IO --> 2D slices bmp
// IO --> 2D slices tif
// IO --> 2D slices dcm
// IO --> 3D traw3D
// IO --> 3D dcm
// IO --> 3D fav
// etc...


