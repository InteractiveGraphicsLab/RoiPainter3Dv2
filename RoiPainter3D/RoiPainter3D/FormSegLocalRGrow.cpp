#include "FormSegLocalRGrow.h"
#include "FormMain.h"

#include "./mode/ModeSegLocalRGrow.h"

using namespace RoiPainter3D;

static float SPIN_VALUE_RATE = 10.0f;




void FormSegLocalRGrow::updateSliders()
{
  const int &activeSeedId      = ModeSegLocalRGrow::getInst()->m_ActiveSeedIdx;
  const vector<LRGseed> &seeds = ModeSegLocalRGrow::getInst()->m_seeds;


  //items 
  if( activeSeedId < 0 ||  seeds.size() <= activeSeedId)
  {
    trackBar_minV  ->Hide();
    trackBar_maxV  ->Hide();
    trackBar_radius->Hide();
    textBox_minV   ->Hide();
    textBox_maxV   ->Hide();
    textBox_radius ->Hide();
  }
  else
  {
    const LRGseed& s = seeds[activeSeedId];

    trackBar_minV  ->Show();
    trackBar_maxV  ->Show();
    trackBar_radius->Show();
    textBox_minV   ->Show();
    textBox_maxV   ->Show();
    textBox_radius ->Show();

    trackBar_minV->Value =  s.m_minV;
    textBox_minV ->Text  = (s.m_minV).ToString();
    trackBar_maxV->Value =  s.m_maxV;
    textBox_maxV ->Text  = (s.m_maxV).ToString();
    trackBar_radius->Value = (int) (s.m_rad * SPIN_VALUE_RATE);
    textBox_radius->Text = (s.m_rad).ToString();
  }
}




void FormSegLocalRGrow::updateList()
{
  m_bListUpdating = true;

  const int &activeSeedId      = ModeSegLocalRGrow::getInst()->m_ActiveSeedIdx;
  const vector<LRGseed> &seeds = ModeSegLocalRGrow::getInst()->m_seeds;

  //初期化
  maskList->Rows->Clear();
  for (int i = 0; i < seeds.size(); ++i) maskList->Rows->Add();

  //このタイミングでhighlightをoffに
  maskList->CurrentCell = nullptr;

  // set all cells
  for (int i = 0; i < seeds.size(); ++i)
  {
    string seedName= string("sd_") + to_string(i);
    maskList[0, i]->Value = gcnew String(seedName.c_str());
    maskList[0, i]->Style->BackColor = Color::FromArgb(255, 255, 255);

    if( seeds[i].m_flg ){
      maskList[1, i]->Value = gcnew String("Fore");
      maskList[1, i]->Style->BackColor = Color::FromArgb( 255,0,0 );
    }
    else {
      maskList[1, i]->Value = gcnew String("Back");
      maskList[1, i]->Style->BackColor = Color::FromArgb( 0,0,255 );
    }

    //if (i == activeSeedId) checkbox_lock->CheckState = maskData[i].lock ? CheckState::Checked : CheckState::Unchecked;
  }

  this->Update();
  this->Invalidate();

  //set selection
  if (0 <= activeSeedId && activeSeedId < seeds.size()){
    maskList->CurrentCell = maskList[0, activeSeedId];
  }
  this->Update();
  this->Invalidate();
  m_bListUpdating = false;
}



void FormSegLocalRGrow::updateAllItems()
{
  updateList();
  updateSliders();
}


System::Void FormSegLocalRGrow::maskList_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) 
{
}



System::Void FormSegLocalRGrow::maskList_SelectionChanged(System::Object^  sender, System::EventArgs^  e) 
{
  //maskList->Rows->Clear(); のタイミングで呼ばれてしまうので、その際は何もしない
  if (m_bListUpdating) return;

  printf("selection changed %d %d\n", maskList->CurrentCell->RowIndex, maskList->CurrentCell->ColumnIndex);
  ModeSegLocalRGrow::getInst()->m_ActiveSeedIdx = maskList->CurrentCell->RowIndex;

  updateSliders();
}




void FormSegLocalRGrow::setSliderRange(float maxRadius, short minV, short maxV)
{
  trackBar_radius->SetRange( (int)( 0.01 * SPIN_VALUE_RATE), (int)( maxRadius * SPIN_VALUE_RATE ));
  trackBar_radius->Value   = (int)( 0.1f * maxRadius * SPIN_VALUE_RATE );
  trackBar_minV->SetRange(minV, maxV);
  trackBar_minV->Value = minV;
  trackBar_maxV->SetRange(minV, maxV);
  trackBar_maxV->Value = maxV;

  updateAllItems();
}








System::Void FormSegLocalRGrow::trackBar_minV_Scroll(System::Object^  sender, System::EventArgs^  e) 
{
  int val = trackBar_minV->Value;
  textBox_minV->Text = val.ToString();

  int &activeSeedId      = ModeSegLocalRGrow::getInst()->m_ActiveSeedIdx;
  vector<LRGseed> &seeds = ModeSegLocalRGrow::getInst()->m_seeds;
  
  if( 0 <= activeSeedId && activeSeedId < seeds.size())
    seeds[activeSeedId].m_minV = val;

}



System::Void FormSegLocalRGrow::trackBar_maxV_Scroll(System::Object^  sender, System::EventArgs^  e) 
{
  int val = trackBar_maxV->Value;
  textBox_maxV->Text = val.ToString();

  int &activeSeedId      = ModeSegLocalRGrow::getInst()->m_ActiveSeedIdx;
  vector<LRGseed> &seeds = ModeSegLocalRGrow::getInst()->m_seeds;
  
  if( 0 <= activeSeedId && activeSeedId < seeds.size())
    seeds[activeSeedId].m_maxV = val;
}



System::Void FormSegLocalRGrow::trackBar_radius_Scroll(System::Object^  sender, System::EventArgs^  e)
{
  float val = trackBar_radius->Value / SPIN_VALUE_RATE;
  textBox_maxV->Text = val.ToString();

  int &activeSeedId      = ModeSegLocalRGrow::getInst()->m_ActiveSeedIdx;
  vector<LRGseed> &seeds = ModeSegLocalRGrow::getInst()->m_seeds;
  
  if( 0 <= activeSeedId && activeSeedId < seeds.size())
    seeds[activeSeedId].m_rad = val;

  formMain_redrawMainPanel();
}



System::Void FormSegLocalRGrow::btn_runLocalRGrow_Click(System::Object^  sender, System::EventArgs^  e) 
{
  ModeSegLocalRGrow::getInst()->runLocalRegionGrow();
  formMain_redrawMainPanel();
}



System::Void FormSegLocalRGrow::btn_cancel_Click(System::Object^  sender, System::EventArgs^  e) 
{
  ModeSegLocalRGrow::getInst()->cancelSegmentation();
  formMain_redrawMainPanel();
}


System::Void FormSegLocalRGrow::btn_finish_Click(System::Object^  sender, System::EventArgs^  e) 
{
  ModeSegLocalRGrow::getInst()->finishSegmentation();
  formMain_redrawMainPanel();
}