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
  CrssecCore::GetInst();

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
  
  printf("aaaa \ FormMain::getInst()->ShowDialog() \n");

  return 0;
}

// todo 2018/10/31
// important Core classes
// ImageCore
// ModeCore
// 
// event handler
// 
// 
// mode_vis_mask
// mode_vis_norm
// mode_seg_rgrow
//
// IO --> 2D slices bmp
// IO --> 2D slices tif
// IO --> 2D slices dcm
// IO --> 3D slices traw3D
// IO --> 3D slices dcm
// etc...


