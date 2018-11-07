#include "FormMaskIDselection.h"

#include "ImageCore.h"




using namespace RoiPainter3D;



System::Void FormMaskIDselection::initList() {
  m_bListInit = true;

  const vector<MaskData> &maskData = ImageCore::getInst()->m_maskData;
  const int              &maskSelectedId = ImageCore::getInst()->m_maskSelectedId;

  //初期化
  maskIdList->Rows->Clear();
  for (int i = 0; i < maskData.size(); ++i) maskIdList->Rows->Add();

  //このタイミングでhighlightをoffに
  maskIdList->CurrentCell = nullptr;

  //cell colors
  for (int i = 0; i < maskData.size(); ++i)
  {
    string regionName = to_string(i);
    regionName.append(" : " + maskData[i].name);

    maskIdList[0, i]->Value = gcnew String(regionName.c_str());
    maskIdList[0, i]->Style->BackColor = Color::FromArgb(255, 255, 255);
    maskIdList[1, i]->Style->BackColor = Color::FromArgb(maskData[i].color[0], maskData[i].color[1], maskData[i].color[2]);
  }


  this->Update();
  this->Invalidate();

  if (0 <= maskSelectedId) {
    m_activeId = maskSelectedId;
    maskIdList->CurrentCell = maskIdList[0, maskSelectedId];
  }

  this->Update();
  this->Invalidate();

  m_bListInit = false;


}

System::Void FormMaskIDselection::maskIdList_SelectionChanged(System::Object^  sender, System::EventArgs^  e){
  //FormVisMask::updateList の maskList->Rows->Clear(); のタイミングで呼ばれてしまうので、その際は何もしない
  if (m_bListInit) return;
  printf("selection changed %d %d\n", maskIdList->CurrentCell->RowIndex, maskIdList->CurrentCell->ColumnIndex);
  m_activeId = maskIdList->CurrentCell->RowIndex;
}


System::Void FormMaskIDselection::btnOk_Click(System::Object^  sender, System::EventArgs^  e) {
}

System::Void FormMaskIDselection::btnCancel_Click(System::Object^  sender, System::EventArgs^  e){
}


System::Void FormMaskIDselection::maskIdList_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e){
}
