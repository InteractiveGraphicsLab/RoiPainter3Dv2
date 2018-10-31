#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormVisNorm �̊T�v
	/// </summary>
	public ref class FormVisNorm : public System::Windows::Forms::Form
	{

    static FormVisNorm^ m_singleton;
   
    FormVisNorm(void)
		{
			InitializeComponent();
			//
			//TODO: �����ɃR���X�g���N�^�[ �R�[�h��ǉ����܂�
			//
		}
   
  public:
    static FormVisNorm^ getInst()
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormVisNorm();
      return m_singleton;
    }

    void setVolumeValVis(short v)
    {
      String ^s = gcnew String("");
      s->Format("v:%d", v);
      textBox->Text = s;
    }


	protected:
		/// <summary>
		/// �g�p���̃��\�[�X�����ׂăN���[���A�b�v���܂��B
		/// </summary>
		~FormVisNorm()
		{
			if (components)
			{
				delete components;
			}
		}
  private: System::Windows::Forms::Label^  label;
  private: System::Windows::Forms::TextBox^  textBox;
  protected:

	private:
		/// <summary>
		/// �K�v�ȃf�U�C�i�[�ϐ��ł��B
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// �f�U�C�i�[ �T�|�[�g�ɕK�v�ȃ��\�b�h�ł��B���̃��\�b�h�̓��e��
		/// �R�[�h �G�f�B�^�[�ŕύX���Ȃ��ł��������B
		/// </summary>
		void InitializeComponent(void)
		{
      this->label = (gcnew System::Windows::Forms::Label());
      this->textBox = (gcnew System::Windows::Forms::TextBox());
      this->SuspendLayout();
      // 
      // label
      // 
      this->label->AutoSize = true;
      this->label->Location = System::Drawing::Point(12, 30);
      this->label->Name = L"label";
      this->label->Size = System::Drawing::Size(90, 15);
      this->label->TabIndex = 0;
      this->label->Text = L"volume value";
      // 
      // textBox
      // 
      this->textBox->Location = System::Drawing::Point(109, 30);
      this->textBox->Name = L"textBox";
      this->textBox->Size = System::Drawing::Size(100, 22);
      this->textBox->TabIndex = 1;
      // 
      // FormVisNorm
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(8, 15);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(295, 136);
      this->Controls->Add(this->textBox);
      this->Controls->Add(this->label);
      this->Name = L"FormVisNorm";
      this->Text = L"FormVisNorm";
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
	};

  inline void formVisNorm_Show(){
    FormVisNorm::getInst()->Show();
  }



}
