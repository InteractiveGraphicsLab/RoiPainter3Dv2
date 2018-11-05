#include "FormSegRGrow.h"


using namespace RoiPainter3D;

void FormSegRGrow::InitAllItems(short volMin, short volMax)
{
  trackbar_min->SetRange( volMin, volMax);
  trackbar_max->SetRange( volMin, volMax);
  trackbar_min->Value = volMin;
  trackbar_max->Value = volMax;
  textBox_min->Text = trackbar_min->Value.ToString();
  textBox_max->Text = trackbar_max->Value.ToString();
}



System::Void FormSegRGrow::textBox_min_TextChanged(System::Object^  sender, System::EventArgs^  e) {}
System::Void FormSegRGrow::trackbar_min_Scroll(System::Object^  sender, System::EventArgs^  e) {}
System::Void FormSegRGrow::textBox_max_TextChanged(System::Object^  sender, System::EventArgs^  e){} 
System::Void FormSegRGrow::trackBar_max_Scroll(System::Object^  sender, System::EventArgs^  e) {}
System::Void FormSegRGrow::btn_thresholding_Click(System::Object^  sender, System::EventArgs^  e) {}
System::Void FormSegRGrow::btn_rgrow8_Click(System::Object^  sender, System::EventArgs^  e) {}
System::Void FormSegRGrow::btn_rgrow26_Click(System::Object^  sender, System::EventArgs^  e) {}
System::Void FormSegRGrow::btn_erode_Click(System::Object^  sender, System::EventArgs^  e) {}
System::Void FormSegRGrow::btn_dilate_Click(System::Object^  sender, System::EventArgs^  e) {}
System::Void FormSegRGrow::btn_fillhole_Click(System::Object^  sender, System::EventArgs^  e){} 
System::Void FormSegRGrow::btn_finish_Click(System::Object^  sender, System::EventArgs^  e) {}