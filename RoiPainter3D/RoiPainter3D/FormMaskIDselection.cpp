#include "FormMaskIDselection.h"



using namespace RoiPainter3D;
using namespace std;


System::Void FormMaskIDselection::initList(
  const vector<MaskData> &mask_set, 
  const int default_mask_id) 
{
  m_bListInit = true;

 
  //初期化
  maskIdList->Rows->Clear();
  for (int i = 0; i < mask_set.size(); ++i) maskIdList->Rows->Add();

  //このタイミングでhighlightをoffに
  maskIdList->CurrentCell = nullptr;

  //cell colors
  for (int i = 0; i < mask_set.size(); ++i)
  {
    string regionName = to_string(i);
    regionName.append(" : " + mask_set[i].m_name);

    maskIdList[0, i]->Value = gcnew String(regionName.c_str());
    maskIdList[0, i]->Style->BackColor = Color::FromArgb(255, 255, 255);
    maskIdList[1, i]->Style->BackColor = Color::FromArgb(mask_set[i].m_color[0], mask_set[i].m_color[1], mask_set[i].m_color[2]);
  }


  this->Update();
  this->Invalidate();

  if (0 <= default_mask_id) {
    m_activeId = default_mask_id;
    maskIdList->CurrentCell = maskIdList[0, default_mask_id];
  }

  this->Update();
  this->Invalidate();

  m_bListInit = false;
}

System::Void FormMaskIDselection::maskIdList_SelectionChanged(System::Object^  sender, System::EventArgs^  e){
  //FormVisMask::updateList の maskList->Rows->Clear(); のタイミングで呼ばれてしまうので、その際は何もしない
  if (m_bListInit) return;
  std::cout << "selection changed " << maskIdList->CurrentCell->RowIndex << " " << maskIdList->CurrentCell->ColumnIndex << "\n";
  m_activeId = maskIdList->CurrentCell->RowIndex;
}


System::Void FormMaskIDselection::btnOk_Click(System::Object^  sender, System::EventArgs^  e) {
}

System::Void FormMaskIDselection::btnCancel_Click(System::Object^  sender, System::EventArgs^  e){
}


System::Void FormMaskIDselection::maskIdList_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e){
}
