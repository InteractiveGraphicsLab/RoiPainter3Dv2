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

    
    static FormSegLocalRGrow^ m_singleton;
   
  public:
    static FormSegLocalRGrow^ getInst()
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormSegLocalRGrow();
      return m_singleton;
    }

	private:
		FormSegLocalRGrow(void)
		{
			InitializeComponent();
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
  private: System::Windows::Forms::DataGridViewTextBoxColumn^  regionIDColumn;
  private: System::Windows::Forms::DataGridViewTextBoxColumn^  colorColumn;
  private: System::Windows::Forms::Button^  btn_cancel;
  private: System::Windows::Forms::Button^  btn_finish;
  private: System::Windows::Forms::Label^  label1;
  private: System::Windows::Forms::Label^  label2;
  private: System::Windows::Forms::Label^  label3;
  private: System::Windows::Forms::Label^  label4;
  private: System::Windows::Forms::Label^  label5;
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
      System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle2 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
      this->maskList = (gcnew System::Windows::Forms::DataGridView());
      this->regionIDColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->colorColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->btn_cancel = (gcnew System::Windows::Forms::Button());
      this->btn_finish = (gcnew System::Windows::Forms::Button());
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->label2 = (gcnew System::Windows::Forms::Label());
      this->label3 = (gcnew System::Windows::Forms::Label());
      this->label4 = (gcnew System::Windows::Forms::Label());
      this->label5 = (gcnew System::Windows::Forms::Label());
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
        this->regionIDColumn,
          this->colorColumn
      });
      this->maskList->Location = System::Drawing::Point(13, 13);
      this->maskList->Margin = System::Windows::Forms::Padding(4);
      this->maskList->MultiSelect = false;
      this->maskList->Name = L"maskList";
      this->maskList->ReadOnly = true;
      this->maskList->RowHeadersVisible = false;
      this->maskList->RowTemplate->Height = 21;
      this->maskList->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
      this->maskList->Size = System::Drawing::Size(148, 403);
      this->maskList->TabIndex = 26;
      // 
      // regionIDColumn
      // 
      this->regionIDColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
      this->regionIDColumn->HeaderText = L"region ID";
      this->regionIDColumn->Name = L"regionIDColumn";
      this->regionIDColumn->ReadOnly = true;
      this->regionIDColumn->Resizable = System::Windows::Forms::DataGridViewTriState::False;
      this->regionIDColumn->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
      // 
      // colorColumn
      // 
      this->colorColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
      dataGridViewCellStyle2->BackColor = System::Drawing::Color::White;
      dataGridViewCellStyle2->ForeColor = System::Drawing::Color::Black;
      dataGridViewCellStyle2->SelectionBackColor = System::Drawing::Color::Transparent;
      dataGridViewCellStyle2->SelectionForeColor = System::Drawing::Color::Black;
      this->colorColumn->DefaultCellStyle = dataGridViewCellStyle2;
      this->colorColumn->FillWeight = 80;
      this->colorColumn->HeaderText = L"color";
      this->colorColumn->Name = L"colorColumn";
      this->colorColumn->ReadOnly = true;
      this->colorColumn->Resizable = System::Windows::Forms::DataGridViewTriState::False;
      this->colorColumn->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
      // 
      // btn_cancel
      // 
      this->btn_cancel->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_cancel->Location = System::Drawing::Point(16, 423);
      this->btn_cancel->Name = L"btn_cancel";
      this->btn_cancel->Size = System::Drawing::Size(97, 48);
      this->btn_cancel->TabIndex = 27;
      this->btn_cancel->Text = L"CANCEL";
      this->btn_cancel->UseVisualStyleBackColor = true;
      // 
      // btn_finish
      // 
      this->btn_finish->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_finish->Location = System::Drawing::Point(130, 423);
      this->btn_finish->Name = L"btn_finish";
      this->btn_finish->Size = System::Drawing::Size(247, 48);
      this->btn_finish->TabIndex = 27;
      this->btn_finish->Text = L"FINISH and store region";
      this->btn_finish->UseVisualStyleBackColor = true;
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label1->Location = System::Drawing::Point(169, 13);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(201, 23);
      this->label1->TabIndex = 28;
      this->label1->Text = L"L-dblclk : Place Fore Seed";
      // 
      // label2
      // 
      this->label2->AutoSize = true;
      this->label2->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label2->Location = System::Drawing::Point(165, 51);
      this->label2->Name = L"label2";
      this->label2->Size = System::Drawing::Size(205, 23);
      this->label2->TabIndex = 29;
      this->label2->Text = L"M-dblclk : Erase Seed / CP";
      // 
      // label3
      // 
      this->label3->AutoSize = true;
      this->label3->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label3->Location = System::Drawing::Point(167, 32);
      this->label3->Name = L"label3";
      this->label3->Size = System::Drawing::Size(210, 23);
      this->label3->TabIndex = 30;
      this->label3->Text = L"R-dblclk : Place Back Seed ";
      // 
      // label4
      // 
      this->label4->AutoSize = true;
      this->label4->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label4->Location = System::Drawing::Point(166, 78);
      this->label4->Name = L"label4";
      this->label4->Size = System::Drawing::Size(133, 23);
      this->label4->TabIndex = 31;
      this->label4->Text = L"Shift + L-dblclk :";
      // 
      // label5
      // 
      this->label5->AutoSize = true;
      this->label5->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label5->Location = System::Drawing::Point(202, 97);
      this->label5->Name = L"label5";
      this->label5->Size = System::Drawing::Size(169, 23);
      this->label5->TabIndex = 31;
      this->label5->Text = L"add CP to active seed";
      // 
      // btn_runLocalRGrow
      // 
      this->btn_runLocalRGrow->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->btn_runLocalRGrow->Location = System::Drawing::Point(175, 364);
      this->btn_runLocalRGrow->Name = L"btn_runLocalRGrow";
      this->btn_runLocalRGrow->Size = System::Drawing::Size(202, 37);
      this->btn_runLocalRGrow->TabIndex = 32;
      this->btn_runLocalRGrow->Text = L"Run Local Region Grow";
      this->btn_runLocalRGrow->UseVisualStyleBackColor = true;
      // 
      // label6
      // 
      this->label6->AutoSize = true;
      this->label6->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label6->Location = System::Drawing::Point(171, 131);
      this->label6->Name = L"label6";
      this->label6->Size = System::Drawing::Size(37, 23);
      this->label6->TabIndex = 33;
      this->label6->Text = L"min";
      // 
      // label7
      // 
      this->label7->AutoSize = true;
      this->label7->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label7->Location = System::Drawing::Point(169, 194);
      this->label7->Name = L"label7";
      this->label7->Size = System::Drawing::Size(41, 23);
      this->label7->TabIndex = 34;
      this->label7->Text = L"max";
      // 
      // label8
      // 
      this->label8->AutoSize = true;
      this->label8->Font = (gcnew System::Drawing::Font(L"メイリオ", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
        static_cast<System::Byte>(128)));
      this->label8->Location = System::Drawing::Point(165, 252);
      this->label8->Name = L"label8";
      this->label8->Size = System::Drawing::Size(54, 23);
      this->label8->TabIndex = 35;
      this->label8->Text = L"radius";
      // 
      // textBox_minV
      // 
      this->textBox_minV->Location = System::Drawing::Point(214, 131);
      this->textBox_minV->Name = L"textBox_minV";
      this->textBox_minV->ReadOnly = true;
      this->textBox_minV->Size = System::Drawing::Size(100, 22);
      this->textBox_minV->TabIndex = 36;
      // 
      // textBox_maxV
      // 
      this->textBox_maxV->Location = System::Drawing::Point(214, 194);
      this->textBox_maxV->Name = L"textBox_maxV";
      this->textBox_maxV->ReadOnly = true;
      this->textBox_maxV->Size = System::Drawing::Size(100, 22);
      this->textBox_maxV->TabIndex = 36;
      // 
      // textBox_radius
      // 
      this->textBox_radius->Location = System::Drawing::Point(214, 252);
      this->textBox_radius->Name = L"textBox_radius";
      this->textBox_radius->ReadOnly = true;
      this->textBox_radius->Size = System::Drawing::Size(100, 22);
      this->textBox_radius->TabIndex = 36;
      // 
      // trackBar_minV
      // 
      this->trackBar_minV->Location = System::Drawing::Point(173, 156);
      this->trackBar_minV->Name = L"trackBar_minV";
      this->trackBar_minV->Size = System::Drawing::Size(211, 56);
      this->trackBar_minV->TabIndex = 37;
      this->trackBar_minV->TickStyle = System::Windows::Forms::TickStyle::None;
      // 
      // trackBar_maxV
      // 
      this->trackBar_maxV->Location = System::Drawing::Point(173, 218);
      this->trackBar_maxV->Name = L"trackBar_maxV";
      this->trackBar_maxV->Size = System::Drawing::Size(211, 56);
      this->trackBar_maxV->TabIndex = 38;
      this->trackBar_maxV->TickStyle = System::Windows::Forms::TickStyle::None;
      // 
      // trackBar_radius
      // 
      this->trackBar_radius->Location = System::Drawing::Point(175, 276);
      this->trackBar_radius->Name = L"trackBar_radius";
      this->trackBar_radius->Size = System::Drawing::Size(211, 56);
      this->trackBar_radius->TabIndex = 39;
      this->trackBar_radius->TickStyle = System::Windows::Forms::TickStyle::None;
      // 
      // FormSegLocalRGrow
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(8, 15);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(385, 482);
      this->Controls->Add(this->trackBar_radius);
      this->Controls->Add(this->textBox_radius);
      this->Controls->Add(this->textBox_maxV);
      this->Controls->Add(this->textBox_minV);
      this->Controls->Add(this->label8);
      this->Controls->Add(this->label7);
      this->Controls->Add(this->label6);
      this->Controls->Add(this->btn_runLocalRGrow);
      this->Controls->Add(this->label5);
      this->Controls->Add(this->label4);
      this->Controls->Add(this->label3);
      this->Controls->Add(this->label2);
      this->Controls->Add(this->label1);
      this->Controls->Add(this->btn_finish);
      this->Controls->Add(this->btn_cancel);
      this->Controls->Add(this->maskList);
      this->Controls->Add(this->trackBar_minV);
      this->Controls->Add(this->trackBar_maxV);
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
	};

  inline void formSegLocalRGrow_Show(){ FormSegLocalRGrow::getInst()->Show();}
  inline void formSegLocalRGrow_Hide(){ FormSegLocalRGrow::getInst()->Hide();}
  inline void formSegLocalRGrow_updateList(){ }
}
