object Form2: TForm2
  Left = 0
  Top = 0
  Caption = 'Form2'
  ClientHeight = 650
  ClientWidth = 600
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poDesigned
  DesignSize = (
    600
    650)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 160
    Top = 607
    Width = 3
    Height = 13
  end
  object TreeView1: TTreeView
    Left = 0
    Top = 1
    Width = 600
    Height = 600
    Anchors = [akTop]
    AutoExpand = True
    Indent = 19
    TabOrder = 0
  end
  object Button1: TButton
    Left = 8
    Top = 617
    Width = 75
    Height = 25
    Caption = 'Parsing'
    TabOrder = 1
    OnClick = Button1Click
  end
  object MainMenu1: TMainMenu
    Left = 568
    Top = 624
    object File1: TMenuItem
      Caption = 'File'
      object Loadgrammarfile1: TMenuItem
        Caption = 'Load grammar file'
        OnClick = Loadgrammarfile1Click
      end
      object Loadsourcecode1: TMenuItem
        Caption = 'Load scan file'
        OnClick = Loadsourcecode1Click
      end
    end
  end
  object OpenDialog1: TOpenDialog
    Left = 536
    Top = 624
  end
  object OpenDialog2: TOpenDialog
    Left = 504
    Top = 624
  end
end
