object Form3: TForm3
  Left = 0
  Top = 0
  Caption = 'Form3'
  ClientHeight = 682
  ClientWidth = 517
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Button1: TButton
    Left = 434
    Top = 649
    Width = 75
    Height = 25
    Caption = 'parsing'
    TabOrder = 0
    OnClick = Button1Click
  end
  object TreeView1: TTreeView
    Left = 4
    Top = 8
    Width = 505
    Height = 635
    Indent = 19
    TabOrder = 1
  end
  object MainMenu1: TMainMenu
    Top = 648
    object file1: TMenuItem
      Caption = 'file'
      object sourcecode1: TMenuItem
        Caption = 'source code'
        OnClick = sourcecode1Click
      end
      object grammarfile1: TMenuItem
        Caption = 'grammar file'
        OnClick = grammarfile1Click
      end
    end
  end
  object OpenDialog1: TOpenDialog
    Left = 32
    Top = 648
  end
  object OpenDialog2: TOpenDialog
    Left = 64
    Top = 648
  end
end
