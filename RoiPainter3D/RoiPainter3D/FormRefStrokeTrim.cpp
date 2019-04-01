#include "FormRefStrokeTrim.h"
#include "ModeCore.h"
#include "FormMain.h"
#include "mode/ModeRefStrokeTrim.h"

using namespace RoiPainter3D;
System::Void FormRefStrokeTrim::btn_cancel_Click(System::Object^  sender, System::EventArgs^  e){
  ModeRefStrokeTrim::GetInst()->cancelSegmentation();
  FormMain_RedrawMainPanel();
}
System::Void FormRefStrokeTrim::btn_finish_Click(System::Object^  sender, System::EventArgs^  e){
  ModeRefStrokeTrim::GetInst()->finishSegmentation();
  FormMain_RedrawMainPanel();
}
