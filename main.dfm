object Form1: TForm1
  Left = 379
  Top = 122
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'ELO 2000'
  ClientHeight = 600
  ClientWidth = 900
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnActivate = FormActivate
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object MusicPlayer: TMediaPlayer
    Left = 0
    Top = 0
    Width = 253
    Height = 30
    Visible = False
    TabOrder = 0
  end
  object Button1: TButton
    Left = 80
    Top = 56
    Width = 75
    Height = 25
    Caption = 'Button1'
    TabOrder = 1
    Visible = False
  end
  object MediaPlayer1: TMediaPlayer
    Left = 256
    Top = 0
    Width = 253
    Height = 30
    Visible = False
    TabOrder = 2
  end
  object TrackBar1: TTrackBar
    Left = 168
    Top = 56
    Width = 150
    Height = 45
    Max = 100
    Orientation = trHorizontal
    Frequency = 10
    Position = 50
    SelEnd = 0
    SelStart = 0
    TabOrder = 3
    TickMarks = tmBottomRight
    TickStyle = tsAuto
    Visible = False
    OnChange = TrackBar1Change
  end
  object Button2: TButton
    Left = 80
    Top = 88
    Width = 75
    Height = 25
    Caption = 'Button2'
    TabOrder = 4
    Visible = False
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 10
    OnTimer = Timer1Timer
    Left = 24
    Top = 48
  end
end
