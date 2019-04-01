#include "FormVisMask.h"
#include "FormMain.h"
#include "FormMaskIDselection.h"
#include "ImageCore.h"



using namespace System::Runtime::InteropServices;
using namespace RoiPainter3D;
using namespace std;



void FormVisMask::updateList()
{
  m_bListUpdating = true;

  const vector<MaskData> &maskData       = ImageCore::GetInst()->m_mask_data;
  const int              &maskSelectedId = ImageCore::GetInst()->m_active_mask_id;


  //初期化
  maskList->Rows->Clear();
  for (int i = 0; i < maskData.size(); ++i) maskList->Rows->Add();

  //このタイミングでhighlightをoffに
  maskList->CurrentCell = nullptr;

  //cell colors
  for (int i = 0; i < maskData.size(); ++i)
  {
    string regionName = to_string(i);
    regionName.append(" : " + maskData[i].m_name);

    maskList[0, i]->Value = gcnew String(regionName.c_str());
    maskList[0, i]->Style->BackColor = Color::FromArgb(255, 255, 255);
    maskList[1, i]->Style->BackColor = Color::FromArgb(maskData[i].m_color[0], maskData[i].m_color[1], maskData[i].m_color[2]);

    if (i == maskSelectedId) checkbox_lock->CheckState = maskData[i].m_b_locked ? CheckState::Checked : CheckState::Unchecked;
  }


  this->Update();
  this->Invalidate();

  if (0 <= maskSelectedId) maskList->CurrentCell = maskList[0, maskSelectedId];
  this->Update();
  this->Invalidate();

  m_bListUpdating = false;
}





System::Void FormVisMask::maskList_SelectionChanged(System::Object^  sender, System::EventArgs^  e)
{
  //FormVisMask::updateList の maskList->Rows->Clear(); のタイミングで呼ばれてしまうので、その際は何もしない
  if (m_bListUpdating) return;

  std::cout << "selection changed " << maskList->CurrentCell->RowIndex << " " << maskList->CurrentCell->ColumnIndex << "\n";
  ImageCore::GetInst()->m_active_mask_id = maskList->CurrentCell->RowIndex;

  //modify values
  const int              &tgtMaskId = ImageCore::GetInst()->m_active_mask_id;
  const vector<MaskData> &maskData  = ImageCore::GetInst()->m_mask_data;

  checkbox_lock->CheckState = maskData[tgtMaskId].m_b_locked ? CheckState::Checked : CheckState::Unchecked;
  trackbar_alpha->Value = (int)(100 * maskData[tgtMaskId].m_alpha);

}



System::Void FormVisMask::maskList_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e)
{

}


System::Void FormVisMask::btnColorPallet_Click  (System::Object^  sender, System::EventArgs^  e) 
{
  const int         &tgtMaskId = ImageCore::GetInst()->m_active_mask_id;
  vector<MaskData>  &maskData  = ImageCore::GetInst()->m_mask_data;

  if (tgtMaskId < 0 || maskList->RowCount <= tgtMaskId) return;

  System::Windows::Forms::ColorDialog ^colorDialog = gcnew System::Windows::Forms::ColorDialog();  
  if (colorDialog->ShowDialog() != System::Windows::Forms::DialogResult::OK) return;

  maskData[tgtMaskId].m_color = EVec3i(colorDialog->Color.R, colorDialog->Color.G, colorDialog->Color.B);


  updateList();
  FormMain_RedrawMainPanel();
}



System::Void FormVisMask::checkbox_lock_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
  vector<MaskData> &maskData  = ImageCore::GetInst()->m_mask_data;
  const int        &tgtMaskId = ImageCore::GetInst()->m_active_mask_id;
  if (tgtMaskId < 0 || maskList->RowCount <= tgtMaskId) return;

  maskData[tgtMaskId].m_b_locked = checkbox_lock->CheckState == CheckState::Checked ? true : false;
  std::cout << "lock value = " << maskData[tgtMaskId].m_b_locked << "\n";

  FormMain_RedrawMainPanel();
}


System::Void FormVisMask::trackbar_alpha_Scroll (System::Object^  sender, System::EventArgs^  e) 
{
  const int         &tgtMaskId = ImageCore::GetInst()->m_active_mask_id;
  vector<MaskData>  &maskData  = ImageCore::GetInst()->m_mask_data;
  if (tgtMaskId < 0 || maskList->RowCount <= tgtMaskId) return;

  maskData[tgtMaskId].m_alpha = trackbar_alpha->Value / 100.0;
  FormMain_RedrawMainPanel();
}


static void updateImageCoreVisVolumes()
{
  const EVec2i minmax = ImageCore::GetInst()->GetVolMinMax();
  ImageCore::GetInst()->UpdateOGLVolume(minmax[0], minmax[1]);
}


System::Void FormVisMask::btnDelete_Click       (System::Object^  sender, System::EventArgs^  e) 
{
  ImageCore::GetInst()->ActiveMask_Delete();
  updateList();

  updateImageCoreVisVolumes();
  FormMain_RedrawMainPanel();
}

System::Void FormVisMask::btnMargeTo_Click( System::Object^  sender, System::EventArgs^  e) 
{
  int trgtId = formMaskIdSelection_showModalDialog();
  if( trgtId == -1) return;

  ImageCore::GetInst()->ActiveMask_Marge(trgtId);
  updateList();

  updateImageCoreVisVolumes();
  FormMain_RedrawMainPanel();
}

System::Void FormVisMask::btnErode_Click(System::Object^  sender, System::EventArgs^  e)
{
  ImageCore::GetInst()->ActiveMask_Erode();
  updateImageCoreVisVolumes();
  FormMain_RedrawMainPanel();
}


System::Void FormVisMask::btnDilate_Click(System::Object^  sender, System::EventArgs^  e)
{
  ImageCore::GetInst()->ActiveMask_Dilate();
  updateImageCoreVisVolumes();
  FormMain_RedrawMainPanel();
}

System::Void FormVisMask::btnFillHole_Click(System::Object^  sender, System::EventArgs^  e)
{
  ImageCore::GetInst()->ActiveMask_FillHole();
  updateImageCoreVisVolumes();
  FormMain_RedrawMainPanel();
}

System::Void FormVisMask::btnExpObj_Click(System::Object^  sender, System::EventArgs^  e) 
{
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = "surface data (*.obj)|*.obj";

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr  = Marshal::StringToHGlobalAnsi(dlg->FileName);
  string fname = static_cast<const char*>(mptr.ToPointer());

  ImageCore::GetInst()->ActiveMask_ExportObj(fname);
}

