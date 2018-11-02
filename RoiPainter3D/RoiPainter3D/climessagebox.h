#pragma once



namespace RoiPainter3D {

  using namespace System;
  using namespace System::ComponentModel;
  using namespace System::Collections;
  using namespace System::Windows::Forms;
  using namespace System::Data;
  using namespace System::Drawing;


  inline bool CLI_MessageBox_OK_Show(const char* text, const char* caption)
  {
    String ^textStr = gcnew String(text);
    String ^capStr = gcnew String(caption);
    MessageBox::Show(textStr, capStr, MessageBoxButtons::OK, MessageBoxIcon::Asterisk);
    return true;
  }

  inline bool CLI_MessageBox_YESNO_Show(const char* text, const char* caption)
  {
    String ^textStr = gcnew String(text);
    String ^capStr = gcnew String(caption);
    return DialogResult::Yes == MessageBox::Show(textStr, capStr, MessageBoxButtons::YesNo, MessageBoxIcon::Question);
  }

}