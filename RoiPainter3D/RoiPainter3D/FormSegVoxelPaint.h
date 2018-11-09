#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormSegVoxelPaint �̊T�v
	/// </summary>
	public ref class FormSegVoxelPaint : public System::Windows::Forms::Form
	{

    
    static FormSegVoxelPaint^ m_singleton;
   
  public:
    static FormSegVoxelPaint^ getInst()
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormSegVoxelPaint();
      return m_singleton;
    }




	private:
		FormSegVoxelPaint(void)
		{
			InitializeComponent();
			//
			//TODO: �����ɃR���X�g���N�^�[ �R�[�h��ǉ����܂�
			//
		}

	protected:
		/// <summary>
		/// �g�p���̃��\�[�X�����ׂăN���[���A�b�v���܂��B
		/// </summary>
		~FormSegVoxelPaint()
		{
			if (components)
			{
				delete components;
			}
		}

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
			this->components = gcnew System::ComponentModel::Container();
			this->Size = System::Drawing::Size(300,300);
			this->Text = L"FormSegVoxelPaint";
			this->Padding = System::Windows::Forms::Padding(0);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		}
#pragma endregion
	};

  inline void formSegVoxelPaint_Hide(){
    FormSegVoxelPaint::getInst()->Hide();
  }
  inline void formSegVoxelPaint_Show(){
    FormSegVoxelPaint::getInst()->Show();
  }
}
