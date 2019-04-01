#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormSegLocalRGrow の概要
	/// </summary>
	public ref class FormSegLocalRGrow : public System::Windows::Forms::Form
	{
    bool m_bListUpdating;
  private: System::Windows::Forms::Button^  button_addforeseed;

  private: System::Windows::Forms::Button^  button_addbackseed;

  private: System::Windows::Forms::Label^  label9;
  private: System::Windows::Forms::Label^  label4;
           static FormSegLocalRGrow^ m_singleton;
   
  public:
    static FormSegLocalRGrow^ getInst()
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormSegLocalRGrow();
      return m_singleton;
    }

     void updateAllItems();
     void updateList();
     void updateSliders();
     void setSliderRange(float maxRadius, short minV, short maxV);

  private:
		FormSegLocalRGrow(void)
		{
			InitializeComponent();
      m_bListUpdating = false;
			//
			//TODO: ここにコンストラクター コードを追加します
			//
		}

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormSegLocalRGrow()
		{
			if (components)
			{
				delete components;
			}
		}
  private: System::Windows::Forms::DataGridView^  maskList;
  protected:


  private: System::Windows::Forms::Button^  btn_cancel;
  private: System::Windows::Forms::Button^  btn_finish;
  private: System::Windows::Forms::Label^  label1;
  private: System::Windows::Forms::Label^  label2;
  private: System::Windows::Forms::Label^  label3;


  private: System::Windows::Forms::Button^  btn_runLocalRGrow;
  private: System::Windows::Forms::Label^  label6;
  private: System::Windows::Forms::Label^  label7;
  private: System::Windows::Forms::Label^  label8;
  private: System::Windows::Forms::TextBox^  textBox_minV;
  private: System::Windows::Forms::TextBox^  textBox_maxV;
  private: System::Windows::Forms::TextBox^  textBox_radius;
  private: System::Windows::Forms::TrackBar^  trackBar_minV;
  private: System::Windows::Forms::TrackBar^  trackBar_maxV;
  private: System::Windows::Forms::TrackBar^  trackBar_radius;
  private: System::Windows::Forms::DataGridViewTextBoxColumn^  seedColumn;
  private: System::Windows::Forms::DataGridViewTextBoxColumn^  fbColumn;

	private:
		/// <summary>
		/// 必要なデザイナー変数です。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディターで変更しないでください。
		/// </summary>
		void InitializeComponent(void)
		{
      System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle1 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
      this->maskList = (gcnew System::Windows::Forms::DataGridView());
      this->seedColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->fbColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->btn_cancel = (gcnew System::Windows::Forms::Button());
      this->btn_finish = (gcnew System::Windows::Forms::Button());
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->label2 = (gcnew System::Windows::Forms::Label());
      this->label3 = (gcnew System::Windows::Forms::Label());
      this->btn_runLocalRGrow = (gcnew System::Windows::Forms::Button());
      this->label6 = (gcnew System::Windows::Forms::Label());
      this->label7 = (gcnew System::Windows::Forms::Label());
      this->label8 = (gcnew System::Windows::Forms::Label());
      this->textBox_minV = (gcnew System::Windows::Forms::TextBox());
      this->textBox_maxV = (gcnew System::Windows::Forms::TextBox());
      this->textBox_radius = (gcnew System::Windows::Forms::TextBox());
      this->trackBar_minV = (gcnew System::Windows::Forms::TrackBar());
      this->trackBar_maxV = (gcnew System::Windows::Forms::TrackBar());
      this->trackBar_radius = (gcnew System::Windows::Forms::TrackBar());
      this->button_addforeseed = (gcnew System::Windows::Forms::Button());
      this->button_addbackseed = (gcnew System::Windows::Forms::Button());
      this->label9 = (gcnew System::Windows::Forms::Label());
      this->label4 = (gcnew System::Windows::Forms::Label());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maskList))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_minV))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_maxV))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_radius))->BeginInit();
      this->SuspendLayout();
      // 
      // maskList
      // 
      this->maskList->AllowUserToAddRows = false;
      this->maskList->AllowUserToDeleteRows = false;
      this->maskList->AllowUserToResizeColumns = false;
      this->maskList->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::AllCells;
      this->maskList->AutoSizeRowsMode = System::Windows::Forms::DataGridViewAutoSizeRowsMode::AllCells;
      this->maskList->BackgroundColor = System::Drawing::SystemColors::ButtonShadow;
      this->maskList->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
      this->maskList->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) {
        this->seedColumn,
          this->fbColumn
      });
      this->maskList->Location = System::Drawing::Point(10, 10);
      this->maskList->MultiSelect = false;
      this->maskList->Name = L"maskList";
      this->maskList->ReadOnly = true;
      this->maskList->RowHeadersVisible = false;
      this->maskList->RowTemplate->Height = 21;
      this->maskList->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
      this->maskList->Size = System::Drawing::Size(109, 322);
      this->maskList->TabIndex = 26;
      this->maskList->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &FormSegLocalRGrow::maskList_CellContentClick);
      this->maskList->SelectionChanged += gcnew System::EventHandler(this, &FormSegLocalRGrow::maskList_SelectionChanged);
      // 
      // seedColumn
      // 
      this->seedColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
      this->seedColumn->HeaderText = L"seed";
      this->seedColumn->Name = L"seedColumn";
      this->seedColumn->ReadOnly = true;
      this->seedColumn->Resizable = System::Windows::Forms::DataGridViewTriState::False;
      this->seedColumn->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
      // 
      // fbColumn
      // 
      this->fbColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
      dataGridViewCellStyle1->BackColor = System::Drawing::Color::White;
      dataGridViewCellStyle1->ForeColor = System::Drawing::Color::Black;
      dataGridViewCellStyle1->SelectionBackColor = System::Drawing::Color::Transparent;
      dataGridViewCellStyle1->SelectionForeColor = System::Drawing::Color::Black;
      this->fbColumn->DefaultCellStyle = dataGridViewCellStyle1;
      this->fbColumn->FillWeight = 80;
      this->fbColumn->HeaderText = L"F/B";
      this->fbColumn->Name = L"fbColumn";
      this->fbColumn->ReadOnly = true;
      this->fbColumn->Resizable = System::Windows::Forms::DataGridViewTriState::False;
      this->fbColumn->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
      // 
      // btn_cancel
      // 
      this->btn_cancel->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_cancel->Location = System::Drawing::Point(12, 338);
      this->btn_cancel->Margin = System::Windows::Forms::Padding(2);
      this->btn_cancel->Name = L"btn_cancel";
      this->btn_cancel->Size = System::Drawing::Size(94, 38);
      this->btn_cancel->TabIndex = 27;
      this->btn_cancel->Text = L"CANCEL";
      this->btn_cancel->UseVisualStyleBackColor = true;
      this->btn_cancel->Click += gcnew System::EventHandler(this, &FormSegLocalRGrow::btn_cancel_Click);
      // 
      // btn_finish
      // 
      this->btn_finish->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_finish->Location = System::Drawing::Point(110, 338);
      this->btn_finish->Margin = System::Windows::Forms::Padding(2);
      this->btn_finish->Name = L"btn_finish";
      this->btn_finish->Size = System::Drawing::Size(202, 38);
      this->btn_finish->TabIndex = 27;
      this->btn_finish->Text = L"FINISH and store region";
      this->btn_finish->UseVisualStyleBackColor = true;
      this->btn_finish->Click += gcnew System::EventHandler(this, &FormSegLocalRGrow::btn_finish_Click);
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label1->Location = System::Drawing::Point(124, 4);
      this->label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(214, 18);
      this->label1->TabIndex = 28;
      this->label1->Text = L"shift + L-click : Add new cp to seed";
      // 
      // label2
      // 
      this->label2->AutoSize = true;
      this->label2->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label2->Location = System::Drawing::Point(124, 40);
      this->label2->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label2->Name = L"label2";
      this->label2->Size = System::Drawing::Size(198, 18);
      this->label2->TabIndex = 29;
      this->label2->Text = L"shift + R-click : Erase cp or seed";
      // 
      // label3
      // 
      this->label3->AutoSize = true;
      this->label3->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label3->Location = System::Drawing::Point(124, 22);
      this->label3->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label3->Name = L"label3";
      this->label3->Size = System::Drawing::Size(197, 18);
      this->label3->TabIndex = 30;
      this->label3->Text = L"shift + L-drag : move cp of seed";
      // 
      // btn_runLocalRGrow
      // 
      this->btn_runLocalRGrow->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_runLocalRGrow->Location = System::Drawing::Point(143, 301);
      this->btn_runLocalRGrow->Margin = System::Windows::Forms::Padding(2);
      this->btn_runLocalRGrow->Name = L"btn_runLocalRGrow";
      this->btn_runLocalRGrow->Size = System::Drawing::Size(169, 30);
      this->btn_runLocalRGrow->TabIndex = 32;
      this->btn_runLocalRGrow->Text = L"Run Local Region Grow";
      this->btn_runLocalRGrow->UseVisualStyleBackColor = true;
      this->btn_runLocalRGrow->Click += gcnew System::EventHandler(this, &FormSegLocalRGrow::btn_runLocalRGrow_Click);
      // 
      // label6
      // 
      this->label6->AutoSize = true;
      this->label6->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label6->Location = System::Drawing::Point(128, 156);
      this->label6->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label6->Name = L"label6";
      this->label6->Size = System::Drawing::Size(30, 18);
      this->label6->TabIndex = 33;
      this->label6->Text = L"min";
      // 
      // label7
      // 
      this->label7->AutoSize = true;
      this->label7->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label7->Location = System::Drawing::Point(127, 206);
      this->label7->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label7->Name = L"label7";
      this->label7->Size = System::Drawing::Size(34, 18);
      this->label7->TabIndex = 34;
      this->label7->Text = L"max";
      // 
      // label8
      // 
      this->label8->AutoSize = true;
      this->label8->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label8->Location = System::Drawing::Point(124, 253);
      this->label8->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label8->Name = L"label8";
      this->label8->Size = System::Drawing::Size(43, 18);
      this->label8->TabIndex = 35;
      this->label8->Text = L"radius";
      // 
      // textBox_minV
      // 
      this->textBox_minV->Location = System::Drawing::Point(160, 156);
      this->textBox_minV->Margin = System::Windows::Forms::Padding(2);
      this->textBox_minV->Name = L"textBox_minV";
      this->textBox_minV->ReadOnly = true;
      this->textBox_minV->Size = System::Drawing::Size(76, 19);
      this->textBox_minV->TabIndex = 36;
      // 
      // textBox_maxV
      // 
      this->textBox_maxV->Location = System::Drawing::Point(160, 206);
      this->textBox_maxV->Margin = System::Windows::Forms::Padding(2);
      this->textBox_maxV->Name = L"textBox_maxV";
      this->textBox_maxV->ReadOnly = true;
      this->textBox_maxV->Size = System::Drawing::Size(76, 19);
      this->textBox_maxV->TabIndex = 36;
      // 
      // textBox_radius
      // 
      this->textBox_radius->Location = System::Drawing::Point(167, 253);
      this->textBox_radius->Margin = System::Windows::Forms::Padding(2);
      this->textBox_radius->Name = L"textBox_radius";
      this->textBox_radius->ReadOnly = true;
      this->textBox_radius->Size = System::Drawing::Size(76, 19);
      this->textBox_radius->TabIndex = 36;
      // 
      // trackBar_minV
      // 
      this->trackBar_minV->Location = System::Drawing::Point(130, 176);
      this->trackBar_minV->Margin = System::Windows::Forms::Padding(2);
      this->trackBar_minV->Name = L"trackBar_minV";
      this->trackBar_minV->Size = System::Drawing::Size(191, 45);
      this->trackBar_minV->TabIndex = 37;
      this->trackBar_minV->TickStyle = System::Windows::Forms::TickStyle::None;
      this->trackBar_minV->Scroll += gcnew System::EventHandler(this, &FormSegLocalRGrow::trackBar_minV_Scroll);
      // 
      // trackBar_maxV
      // 
      this->trackBar_maxV->Location = System::Drawing::Point(130, 225);
      this->trackBar_maxV->Margin = System::Windows::Forms::Padding(2);
      this->trackBar_maxV->Name = L"trackBar_maxV";
      this->trackBar_maxV->Size = System::Drawing::Size(191, 45);
      this->trackBar_maxV->TabIndex = 38;
      this->trackBar_maxV->TickStyle = System::Windows::Forms::TickStyle::None;
      this->trackBar_maxV->Scroll += gcnew System::EventHandler(this, &FormSegLocalRGrow::trackBar_maxV_Scroll);
      // 
      // trackBar_radius
      // 
      this->trackBar_radius->Location = System::Drawing::Point(131, 272);
      this->trackBar_radius->Margin = System::Windows::Forms::Padding(2);
      this->trackBar_radius->Name = L"trackBar_radius";
      this->trackBar_radius->Size = System::Drawing::Size(190, 45);
      this->trackBar_radius->TabIndex = 39;
      this->trackBar_radius->TickStyle = System::Windows::Forms::TickStyle::None;
      this->trackBar_radius->Scroll += gcnew System::EventHandler(this, &FormSegLocalRGrow::trackBar_radius_Scroll);
      // 
      // button_addforeseed
      // 
      this->button_addforeseed->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->button_addforeseed->Location = System::Drawing::Point(134, 105);
      this->button_addforeseed->Margin = System::Windows::Forms::Padding(2);
      this->button_addforeseed->Name = L"button_addforeseed";
      this->button_addforeseed->Size = System::Drawing::Size(87, 30);
      this->button_addforeseed->TabIndex = 40;
      this->button_addforeseed->Text = L"Fore Seed";
      this->button_addforeseed->UseVisualStyleBackColor = true;
      this->button_addforeseed->Click += gcnew System::EventHandler(this, &FormSegLocalRGrow::button_addforeseed_Click);
      // 
      // button_addbackseed
      // 
      this->button_addbackseed->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->button_addbackseed->Location = System::Drawing::Point(225, 105);
      this->button_addbackseed->Margin = System::Windows::Forms::Padding(2);
      this->button_addbackseed->Name = L"button_addbackseed";
      this->button_addbackseed->Size = System::Drawing::Size(87, 30);
      this->button_addbackseed->TabIndex = 41;
      this->button_addbackseed->Text = L"Back Seed";
      this->button_addbackseed->UseVisualStyleBackColor = true;
      this->button_addbackseed->Click += gcnew System::EventHandler(this, &FormSegLocalRGrow::button_addbackseed_Click);
      // 
      // label9
      // 
      this->label9->AutoSize = true;
      this->label9->Font = (gcnew System::Drawing::Font(L"メイリオ", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label9->Location = System::Drawing::Point(169, 89);
      this->label9->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label9->Name = L"label9";
      this->label9->Size = System::Drawing::Size(110, 20);
      this->label9->TabIndex = 31;
      this->label9->Text = L"Add New Seed";
      // 
      // label4
      // 
      this->label4->AutoSize = true;
      this->label4->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label4->Location = System::Drawing::Point(124, 60);
      this->label4->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
      this->label4->Name = L"label4";
      this->label4->Size = System::Drawing::Size(208, 18);
      this->label4->TabIndex = 42;
      this->label4->Text = L"shift + wheel : modify seed radius";
      // 
      // FormSegLocalRGrow
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(337, 386);
      this->Controls->Add(this->label4);
      this->Controls->Add(this->btn_runLocalRGrow);
      this->Controls->Add(this->button_addbackseed);
      this->Controls->Add(this->button_addforeseed);
      this->Controls->Add(this->label9);
      this->Controls->Add(this->label2);
      this->Controls->Add(this->label3);
      this->Controls->Add(this->trackBar_radius);
      this->Controls->Add(this->textBox_radius);
      this->Controls->Add(this->textBox_maxV);
      this->Controls->Add(this->textBox_minV);
      this->Controls->Add(this->label8);
      this->Controls->Add(this->label7);
      this->Controls->Add(this->label6);
      this->Controls->Add(this->label1);
      this->Controls->Add(this->btn_finish);
      this->Controls->Add(this->btn_cancel);
      this->Controls->Add(this->maskList);
      this->Controls->Add(this->trackBar_minV);
      this->Controls->Add(this->trackBar_maxV);
      this->Margin = System::Windows::Forms::Padding(2);
      this->Name = L"FormSegLocalRGrow";
      this->Text = L"FormSegLocalRGrow";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maskList))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_minV))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_maxV))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_radius))->EndInit();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
  private:
    System::Void maskList_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) ;
    System::Void maskList_SelectionChanged(System::Object^  sender, System::EventArgs^  e) ;
    System::Void trackBar_minV_Scroll(System::Object^  sender, System::EventArgs^  e) ;
    System::Void trackBar_maxV_Scroll(System::Object^  sender, System::EventArgs^  e) ;
    System::Void trackBar_radius_Scroll(System::Object^  sender, System::EventArgs^  e);
    System::Void btn_runLocalRGrow_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void btn_cancel_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void btn_finish_Click(System::Object^  sender, System::EventArgs^  e) ;
    System::Void button_addforeseed_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void button_addbackseed_Click(System::Object^  sender, System::EventArgs^  e);
};

  inline void formSegLocalRGrow_Show(){ FormSegLocalRGrow::getInst()->Show();}
  inline void formSegLocalRGrow_Hide(){ FormSegLocalRGrow::getInst()->Hide();}
 
  inline void formSegLocalRGrow_updateAllItems  (){
    FormSegLocalRGrow::getInst()->updateAllItems();
  } 
  inline void formSegLocalRGrow_setSliderRange(float maxRadius, short minV, short maxV){
    FormSegLocalRGrow::getInst()->setSliderRange(maxRadius, minV, maxV);
  }
  inline void formSegLocalRGrow_updateSliders(){
    FormSegLocalRGrow::getInst()->updateSliders();
  }
}