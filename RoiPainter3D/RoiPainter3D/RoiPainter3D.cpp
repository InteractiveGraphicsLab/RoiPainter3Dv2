// RoiPainter3D.cpp : メイン プロジェクト ファイルです。

#include "FormMain.h"
#include <stdio.h>
#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#include "mode/ModeVizMask.h"
#include "mode/ModeVizNormal.h"
#include "mode/ModeSegRGrow.h"
#include "mode/ModeSegGCut.h"


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

  ModeVizMask  ::getInst();
  ModeVizNormal::getInst();
  ModeSegRGrow ::getInst();
  ModeSegGCut  ::getInst();

  /*
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
// modeVisMask OK
//   implement overall workflow OK
//   formVisMask                OK
//   maskManipulations          OK
//   I/O                        OK
//
// modeSegRgrow
//   formSegRGrow    OK
//   Thesholing      OK
//   RegionGrowing6  OK
//   RegionGrowing26 OK
//
// modeSegRgrow
//   formSegGCut
//   watershed (bk-thread) 
//   graphcut Wsd level   
//   graphcut voxel level 
//   erode/dilate/fillhole 
//
//
//
// CORE_CLASSES
//   ImageCore     OK 
//   CrssecCore    OK
//   ViewAngleCore OK
//   ModeCore      80%
//
// 
// event handler (MainForm): 
//    Wheeling OK
//    mouse    OK
//    resize   OK
//    repaint  OK
//
//
// IO
//   initial volume    OK
//   2D slices bmp/tif OK
//   2D slices dcm     OK
//   3D traw3D         OK
//   3D dcm            OK
//   save / load mask  OK 
//   export traw3d_ss  TODO YET TODO YET 
//   open fav          TODO YET TODO YET 
//   save fav          TODO YET TODO YET 
//   show_dlg for stack orientation TODO YET TODO YET 
//   
// etc...

/*

todo
1. modeVisMask作る OK 
2. maskIOのテスト / VisMaskのテスト 
3. mode switchのテスト 
4. volume_load時のflipするかどうかを聞くdialog作る
5. visMaskをonにした瞬間にmarching cubesする機構を作る
--> segmentationへ

*/