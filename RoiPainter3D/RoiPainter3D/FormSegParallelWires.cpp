#include "stdafx.h"
#include "FormSegParallelWires.h"
#include "FormMain.h"
#include "Mode/ModeSegParallelWires.h"

using namespace RoiPainter3D;
using namespace System::Runtime::InteropServices;

FormSegParallelWires::FormSegParallelWires(void)
{
	InitializeComponent();
	m_checkbox_xy->Checked = true ;
	m_checkbox_yz->Checked = false;
	m_checkbox_zx->Checked = false;
}



System::Void FormSegParallelWires::m_checkbox_xy_CheckedChanged(
    System::Object^ sender, 
    System::EventArgs^  e)
{
	m_checkbox_xy->Checked = true ;
	m_checkbox_yz->Checked = false;
	m_checkbox_zx->Checked = false;
  FormMain::getInst()->redrawMainPanel();
}

System::Void FormSegParallelWires::m_checkbox_yz_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
	m_checkbox_xy->Checked = false;
	m_checkbox_yz->Checked = true ;
	m_checkbox_zx->Checked = false;
  FormMain::getInst()->redrawMainPanel();

}

System::Void FormSegParallelWires::m_checkbox_zx_CheckedChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
	m_checkbox_xy->Checked = false;
	m_checkbox_yz->Checked = false;
	m_checkbox_zx->Checked = true ;
  FormMain::getInst()->redrawMainPanel();

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

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return false;

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