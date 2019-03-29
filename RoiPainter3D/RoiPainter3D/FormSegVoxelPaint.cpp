#include "FormSegVoxelPaint.h"
#include "FormMain.h"
#include "mode/ModeSegVoxelPaint.h"

using namespace RoiPainter3D;

System::Void FormSegVoxelPaint::rbtn_paint_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
  FormMain_RedrawMainPanel();
}
System::Void FormSegVoxelPaint::rbtn_lasso_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
  FormMain_RedrawMainPanel();
}
System::Void FormSegVoxelPaint::btn_Cancel_Click(System::Object^  sender, System::EventArgs^  e) {
  ModeSegVoxelPaint::GetInst()->CancelSegmentation();
  FormMain_RedrawMainPanel();
}
System::Void FormSegVoxelPaint::btn_finish_Click(System::Object^  sender, System::EventArgs^  e) {
  ModeSegVoxelPaint::GetInst()->FinishSegmentation();
  FormMain_RedrawMainPanel();
}
