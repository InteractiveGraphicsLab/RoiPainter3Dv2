#include "FormSegRGrow.h"
#include "FormMain.h"
#include "mode/ModeSegRGrow.h"


using namespace RoiPainter3D;

void FormSegRGrow::InitAllItems(short volMin, short volMax)
{
  trackbar_min->SetRange( volMin, volMax);
  trackbar_max->SetRange( volMin, volMax);
  trackbar_min->Value = volMin;
  trackbar_max->Value = volMax;
  textBox_min->Text = trackbar_min->Value.ToString();
  textBox_max->Text = trackbar_max->Value.ToString();

  trackBar_maxiteration->SetRange(1,512*512*512);
  trackBar_maxiteration->Value = 10;
  textBox_maxiteration->Text = trackBar_maxiteration->Value.ToString();
}


System::Void FormSegRGrow::textBox_max_TextChanged(System::Object^  sender, System::EventArgs^ e) {} 
System::Void FormSegRGrow::textBox_min_TextChanged(System::Object^  sender, System::EventArgs^ e) {}

System::Void FormSegRGrow::trackbar_min_Scroll(System::Object^  sender, System::EventArgs^     e) 
{
  textBox_min->Text = trackbar_min->Value.ToString();
  trackbar_min->Refresh();
}

System::Void FormSegRGrow::trackBar_max_Scroll(System::Object^  sender, System::EventArgs^     e) 
{
  textBox_max->Text = trackbar_max->Value.ToString();
  trackbar_max->Refresh();
}

System::Void FormSegRGrow::btn_thresholding_Click(System::Object^  sender, System::EventArgs^  e) 
{ 
  ModeSegRGrow::GetInst()->RunThresholding( (short)trackbar_min->Value, (short)trackbar_max->Value);
  FormMain_RedrawMainPanel();
}

System::Void FormSegRGrow::btn_rgrow6_Click(System::Object^  sender, System::EventArgs^    e) 
{
  ModeSegRGrow::GetInst()->RunRegionGrow6((short)trackbar_min->Value, (short)trackbar_max->Value);
  FormMain_RedrawMainPanel();
}
System::Void FormSegRGrow::btn_rgrow26_Click(System::Object^  sender, System::EventArgs^   e)
{
  ModeSegRGrow::GetInst()->RunRegionGrow26((short)trackbar_min->Value, (short)trackbar_max->Value);
  FormMain_RedrawMainPanel();
}
System::Void FormSegRGrow::btn_erode_Click(System::Object^  sender, System::EventArgs^     e) {
  ModeSegRGrow::GetInst()->RunErosion();
  FormMain_RedrawMainPanel();
}
System::Void FormSegRGrow::btn_dilate_Click(System::Object^  sender, System::EventArgs^    e) {
  ModeSegRGrow::GetInst()->RunDilation();
  FormMain_RedrawMainPanel();
}
System::Void FormSegRGrow::btn_fillhole_Click(System::Object^  sender, System::EventArgs^  e) {
  ModeSegRGrow::GetInst()->RunFillHole();
  FormMain_RedrawMainPanel();
} 
System::Void FormSegRGrow::btn_finish_Click(System::Object^  sender, System::EventArgs^    e) {
  ModeSegRGrow::GetInst()->FinishSegmentation();
  FormMain_RedrawMainPanel();
}

System::Void FormSegRGrow::trackBar_maxiteration_Scroll(System::Object^  sender, System::EventArgs^  e) 
{
  textBox_maxiteration->Text = trackBar_maxiteration->Value.ToString();
}
