#include "FormSegLocalRGrow.h"
#include "FormMain.h"

#include "./mode/ModeSegLocalRGrow.h"


using namespace RoiPainter3D;
using namespace std;

static float SPIN_VALUE_RATE = 10.0f;




void FormSegLocalRGrow::updateSliders()
{
  const int &activeSeedId      = ModeSegLocalRGrow::GetInst()->m_activeseed_idx;
  const vector<LRGSeed> &seeds = ModeSegLocalRGrow::GetInst()->m_seeds;


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
    const LRGSeed& s = seeds[activeSeedId];

    trackBar_minV  ->Show();
    trackBar_maxV  ->Show();
    trackBar_radius->Show();
    textBox_minV   ->Show();
    textBox_maxV   ->Show();
    textBox_radius ->Show();

    trackBar_minV->Value =  s.m_min_v;
    textBox_minV ->Text  = (s.m_min_v).ToString();
    trackBar_maxV->Value =  s.m_max_v;
    textBox_maxV ->Text  = (s.m_max_v).ToString();
    trackBar_radius->Value = (int) (s.m_radius * SPIN_VALUE_RATE);
    textBox_radius->Text = (s.m_radius).ToString();
  }
}




void FormSegLocalRGrow::updateList()
{
  m_bListUpdating = true;

  const int &activeSeedId      = ModeSegLocalRGrow::GetInst()->m_activeseed_idx;
  const vector<LRGSeed> &seeds = ModeSegLocalRGrow::GetInst()->m_seeds;

  //������
  maskList->Rows->Clear();
  for (int i = 0; i < seeds.size(); ++i) maskList->Rows->Add();

  //���̃^�C�~���O��highlight��off��
  maskList->CurrentCell = nullptr;

  // set all cells
  for (int i = 0; i < seeds.size(); ++i)
  {
    string seedName= string("sd_") + to_string(i);
    maskList[0, i]->Value = gcnew String(seedName.c_str());
    maskList[0, i]->Style->BackColor = Color::FromArgb(255, 255, 255);

    if( seeds[i].m_flg_fore ){
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
  //maskList->Rows->Clear(); �̃^�C�~���O�ŌĂ΂�Ă��܂��̂ŁA���̍ۂ͉������Ȃ�
  if (m_bListUpdating) return;

  std::cout << "selection changed " 
            << maskList->CurrentCell->RowIndex << " " 
            << maskList->CurrentCell->ColumnIndex << "\n";
  ModeSegLocalRGrow::GetInst()->m_activeseed_idx = maskList->CurrentCell->RowIndex;

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



System::Void FormSegLocalRGrow::btn_runLocalRGrow_Click(System::Object^  sender, System::EventArgs^  e) 
{
  ModeSegLocalRGrow::GetInst()->RunLocalRegionGrow();
  RedrawScene();
}



System::Void FormSegLocalRGrow::btn_cancel_Click(System::Object^  sender, System::EventArgs^  e) 
{
  ModeSegLocalRGrow::GetInst()->CancelSegmentation();
  RedrawScene();
}


System::Void FormSegLocalRGrow::btn_finish_Click(System::Object^  sender, System::EventArgs^  e) 
{
  ModeSegLocalRGrow::GetInst()->FinishSegmentation();
  RedrawScene();
}

System::Void FormSegLocalRGrow::button_addforeseed_Click(System::Object^  sender, System::EventArgs^  e)
{
  ModeSegLocalRGrow::GetInst()->AddNewSeed(true);
  formSegLocalRGrow_updateAllItems();
  RedrawScene();
}

System::Void FormSegLocalRGrow::button_addbackseed_Click(System::Object^  sender, System::EventArgs^  e)
{
  ModeSegLocalRGrow::GetInst()->AddNewSeed(false);
  formSegLocalRGrow_updateAllItems();
  RedrawScene();
}




System::Void FormSegLocalRGrow::trackBar_minV_Scroll(System::Object^  sender, System::EventArgs^  e) 
{
  m_trackbarscrolling = true;

  int val = trackBar_minV->Value;
  textBox_minV->Text = val.ToString();

  int  activeSeedId      = ModeSegLocalRGrow::GetInst()->m_activeseed_idx;
  vector<LRGSeed> &seeds = ModeSegLocalRGrow::GetInst()->m_seeds;
  
  if( 0 <= activeSeedId && activeSeedId < seeds.size())
    seeds[activeSeedId].m_min_v = val;

  m_trackbarscrolling = false;
}



System::Void FormSegLocalRGrow::trackBar_maxV_Scroll(System::Object^  sender, System::EventArgs^  e) 
{
  m_trackbarscrolling = true;
  int val = trackBar_maxV->Value;
  textBox_maxV->Text = val.ToString();

  int &activeSeedId      = ModeSegLocalRGrow::GetInst()->m_activeseed_idx;
  vector<LRGSeed> &seeds = ModeSegLocalRGrow::GetInst()->m_seeds;
  
  if( 0 <= activeSeedId && activeSeedId < seeds.size())
    seeds[activeSeedId].m_max_v = val;
  m_trackbarscrolling = false;
}



System::Void FormSegLocalRGrow::trackBar_radius_Scroll(System::Object^  sender, System::EventArgs^  e)
{
  m_trackbarscrolling = true;
  float val = trackBar_radius->Value / SPIN_VALUE_RATE;
  textBox_radius->Text = val.ToString();

  int &activeSeedId      = ModeSegLocalRGrow::GetInst()->m_activeseed_idx;
  vector<LRGSeed> &seeds = ModeSegLocalRGrow::GetInst()->m_seeds;
  
  if( 0 <= activeSeedId && activeSeedId < seeds.size())
    seeds[activeSeedId].m_radius = val;

  RedrawScene();
  m_trackbarscrolling = false;
}



System::Void FormSegLocalRGrow::textBox_minV_TextChanged(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  if (m_trackbarscrolling) return;

  int x;
  if (Int32::TryParse(textBox_minV->Text, x)) 
  {
    t_crop( trackBar_minV->Minimum, trackBar_minV->Maximum, x);
    textBox_minV->Text = (x).ToString();
    trackBar_minV->Value = x;

    int  active_id         = ModeSegLocalRGrow::GetInst()->m_activeseed_idx;
    vector<LRGSeed> &seeds = ModeSegLocalRGrow::GetInst()->m_seeds;
    if( 0 <= active_id && active_id < seeds.size()) seeds[active_id].m_min_v = x;
  }
  else 
  {
    textBox_minV->Text = "0";
    trackBar_minV->Value = 0;
  }
}


System::Void FormSegLocalRGrow::textBox_maxV_TextChanged(
    System::Object^  sender, 
    System::EventArgs^  e) 
{
  if (m_trackbarscrolling) return;
  int x;
  if (Int32::TryParse(textBox_maxV->Text, x)) 
  {
    t_crop( trackBar_maxV->Minimum, trackBar_maxV->Maximum, x);
    textBox_maxV->Text = (x).ToString();
    trackBar_maxV->Value = x;
    
    int  active_id         = ModeSegLocalRGrow::GetInst()->m_activeseed_idx;
    vector<LRGSeed> &seeds = ModeSegLocalRGrow::GetInst()->m_seeds;
    if( 0 <= active_id && active_id < seeds.size()) seeds[active_id].m_max_v = x;
  }
  else 
  {
    textBox_maxV->Text = "0";
    trackBar_maxV->Value = 0;
  }
}


System::Void FormSegLocalRGrow::textBox_radius_TextChanged(
    System::Object^  sender, 
    System::EventArgs^  e)
{
  if (m_trackbarscrolling) return;

  float x_float;
  if (float::TryParse(textBox_radius->Text, x_float)) 
  {
    int x = (int)( x_float * SPIN_VALUE_RATE);

    t_crop( trackBar_radius->Minimum, trackBar_radius->Maximum, x);
    textBox_radius->Text = (x_float).ToString();
    trackBar_radius->Value = x;

    int  active_id         = ModeSegLocalRGrow::GetInst()->m_activeseed_idx;
    vector<LRGSeed> &seeds = ModeSegLocalRGrow::GetInst()->m_seeds;
    if( 0 <= active_id && active_id < seeds.size()) seeds[active_id].m_radius = x_float;
    RedrawScene( true );
  }
  else 
  {
    textBox_radius->Text = "1";
    trackBar_radius->Value = 1;
  }
}








