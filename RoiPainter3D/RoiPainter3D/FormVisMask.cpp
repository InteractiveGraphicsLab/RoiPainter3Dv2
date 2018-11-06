#include "FormVisMask.h"
#include "FormMain.h"
#include "ImageCore.h"



using namespace System::Runtime::InteropServices;

using namespace RoiPainter3D;




void FormVisMask::updateList()
{
  m_bListUpdating = true;

  const vector<MaskData> &maskData       = ImageCore::getInst()->m_maskData;
  const int              &maskSelectedId = ImageCore::getInst()->m_maskSelectedId;


  //初期化
  maskList->Rows->Clear();
  for (int i = 0; i < maskData.size(); ++i) maskList->Rows->Add();

  //このタイミングでhighlightをoffに
  maskList->CurrentCell = nullptr;

  //cell colors
  for (int i = 0; i < maskData.size(); ++i)
  {
    string regionName = to_string(i);
    regionName.append(" : " + maskData[i].name);

    maskList[0, i]->Value = gcnew String(regionName.c_str());
    maskList[0, i]->Style->BackColor = Color::FromArgb(255, 255, 255);
    maskList[1, i]->Style->BackColor = Color::FromArgb(maskData[i].color[0], maskData[i].color[1], maskData[i].color[2]);

    if (i == maskSelectedId) checkbox_lock->CheckState = maskData[i].lock ? CheckState::Checked : CheckState::Unchecked;
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

  printf("selection changed %d %d\n", maskList->CurrentCell->RowIndex, maskList->CurrentCell->ColumnIndex);
  ImageCore::getInst()->m_maskSelectedId = maskList->CurrentCell->RowIndex;

  //modify values
  const int              &tgtMaskId = ImageCore::getInst()->m_maskSelectedId;
  const vector<MaskData> &maskData  = ImageCore::getInst()->m_maskData;

  checkbox_lock->CheckState = maskData[tgtMaskId].lock ? CheckState::Checked : CheckState::Unchecked;
  trackbar_alpha->Value = (int)(100 * maskData[tgtMaskId].alpha);

}



System::Void FormVisMask::maskList_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e)
{

}


System::Void FormVisMask::btnColorPallet_Click  (System::Object^  sender, System::EventArgs^  e) 
{
  const int         &tgtMaskId = ImageCore::getInst()->m_maskSelectedId;
  vector<MaskData>  &maskData  = ImageCore::getInst()->m_maskData;

  if (tgtMaskId < 0 || maskList->RowCount <= tgtMaskId) return;

  System::Windows::Forms::ColorDialog ^colorDialog = gcnew System::Windows::Forms::ColorDialog();  
  if (colorDialog->ShowDialog() != System::Windows::Forms::DialogResult::OK) return;

  maskData[tgtMaskId].color = EVec3i(colorDialog->Color.R, colorDialog->Color.G, colorDialog->Color.B);


  updateList();
  formMain_redrawMainPanel();
}



System::Void FormVisMask::checkbox_lock_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
  vector<MaskData> &maskData  = ImageCore::getInst()->m_maskData;
  const int        &tgtMaskId = ImageCore::getInst()->m_maskSelectedId;
  if (tgtMaskId < 0 || maskList->RowCount <= tgtMaskId) return;

  maskData[tgtMaskId].lock = checkbox_lock->CheckState == CheckState::Checked ? true : false;
  printf("lock value = %d\n", maskData[tgtMaskId].lock);

  formMain_redrawMainPanel();

}


System::Void FormVisMask::trackbar_alpha_Scroll (System::Object^  sender, System::EventArgs^  e) 
{
  const int         &tgtMaskId = ImageCore::getInst()->m_maskSelectedId;
  vector<MaskData>  &maskData  = ImageCore::getInst()->m_maskData;
  if (tgtMaskId < 0 || maskList->RowCount <= tgtMaskId) return;

  maskData[tgtMaskId].alpha = trackbar_alpha->Value / 100.0;
  formMain_redrawMainPanel();
}


static void updateImageCoreVisVolumes()
{
  const EVec2i minmax = ImageCore::getInst()->getVolMinMax();
  ImageCore::getInst()->updateVisVolume(minmax[0], minmax[1]);
}


System::Void FormVisMask::btnDelete_Click       (System::Object^  sender, System::EventArgs^  e) 
{
  ImageCore::getInst()->selectedMsk_delete();
  updateList();

  updateImageCoreVisVolumes();
  formMain_redrawMainPanel();
}

System::Void FormVisMask::btnMargeTo_Click( System::Object^  sender, System::EventArgs^  e) 
{
  todo 
  /*
    + marge to 作る
    + marge to用のselection dialog作る
    + stack orientation dialog作る
    */
  /*

  FormSelectMskId ^modal = gcnew FormSelectMskId();
  if (modal->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  int trgtId = modal->getTrgtID();
  modal->Close();

  ImageCore::getInst()->selectedMsk_marge(trgtId);
  updateList();

  updateImageCoreVisVolumes();
  formMain_redrawMainPanel();
  */
}

System::Void FormVisMask::btnErode_Click(System::Object^  sender, System::EventArgs^  e)
{
  ImageCore::getInst()->selectedMsk_erode();
  updateImageCoreVisVolumes();
  formMain_redrawMainPanel();
}


System::Void FormVisMask::btnDilate_Click(System::Object^  sender, System::EventArgs^  e)
{
  ImageCore::getInst()->selectedMsk_dilate();
  updateImageCoreVisVolumes();
  formMain_redrawMainPanel();
}

System::Void FormVisMask::btnFillHole_Click(System::Object^  sender, System::EventArgs^  e)
{
  ImageCore::getInst()->selectedMsk_fillHole();
  updateImageCoreVisVolumes();
  formMain_redrawMainPanel();
}

System::Void FormVisMask::btnExpObj_Click(System::Object^  sender, System::EventArgs^  e) 
{
  SaveFileDialog^ dlg = gcnew SaveFileDialog();
  dlg->Filter = "surface data (*.obj)|*.obj";

  if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return;

  IntPtr mptr  = Marshal::StringToHGlobalAnsi(dlg->FileName);
  string fname = static_cast<const char*>(mptr.ToPointer());

  ImageCore::getInst()->selectedMsk_expObj(fname);
}

