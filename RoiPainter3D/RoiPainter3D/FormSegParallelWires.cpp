#include "stdafx.h"
#include "FormSegParallelWires.h"
#include "FormMain.h"
#include "Mode/ModeSegParallelWires.h"

using namespace RoiPainter3D;
using namespace System::Runtime::InteropServices;

FormSegParallelWires::FormSegParallelWires(void)
{
	InitializeComponent();
}





System::Void FormSegParallelWires::m_btn_exportwires_Click(
    System::Object^  sender, 
    System::EventArgs^  e)
{

  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = "wire data (*.txt)|*.txt";

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr  = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());


  ModeSegParallelWires::GetInst()->ExportWireInfo(fname);
  FormMain::getInst()->redrawMainPanel();
}



System::Void FormSegParallelWires::m_btn_importwires_Click(
    System::Object^  sender, 
    System::EventArgs^  e)
{

  OpenFileDialog^ dlg = gcnew OpenFileDialog();
  dlg->Filter = "wire data (*.txt)|*.txt";
  dlg->Multiselect = false;

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr = Marshal::StringToHGlobalAnsi(dlg->FileName);
  std::string fname = static_cast<const char*>(mptr.ToPointer());


  ModeSegParallelWires::GetInst()->ImportWireInfo(fname);
  FormMain::getInst()->redrawMainPanel();

}

System::Void FormSegParallelWires::m_btn_cancel_Click(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  ModeSegParallelWires::GetInst()->CancelSegmentation();
  FormMain::getInst()->redrawMainPanel();
}

System::Void FormSegParallelWires::m_btn_finish_Click(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  ModeSegParallelWires::GetInst()->FinishSegmentation();
  FormMain::getInst()->redrawMainPanel();
}


System::Void FormSegParallelWires::m_radiobtn_planexy_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  FormMain::getInst()->redrawMainPanel();
}


System::Void FormSegParallelWires::m_radiobtn_planeyz_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  FormMain::getInst()->redrawMainPanel();
}


System::Void FormSegParallelWires::m_radiobtn_planezx_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  FormMain::getInst()->redrawMainPanel();
}



