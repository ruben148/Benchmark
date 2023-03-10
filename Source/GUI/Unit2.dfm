object Form2: TForm2
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Scores'
  ClientHeight = 706
  ClientWidth = 1081
  Color = clBlack
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 144
    Top = 8
    Width = 159
    Height = 18
    Caption = 'Benchmark algorithm'
    Color = clWhite
    Font.Charset = ANSI_CHARSET
    Font.Color = clWhite
    Font.Height = -15
    Font.Name = 'Verdana'
    Font.Style = []
    ParentColor = False
    ParentFont = False
  end
  object Label2: TLabel
    Left = 560
    Top = 8
    Width = 59
    Height = 18
    Caption = 'Threads'
    Color = clWhite
    Font.Charset = ANSI_CHARSET
    Font.Color = clWhite
    Font.Height = -15
    Font.Name = 'Verdana'
    Font.Style = []
    ParentColor = False
    ParentFont = False
  end
  object DBGrid1: TDBGrid
    Left = 8
    Top = 37
    Width = 1067
    Height = 660
    Color = clBlack
    DataSource = DataSource1
    FixedColor = clBlack
    GradientEndColor = clBlack
    GradientStartColor = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    TitleFont.Charset = ANSI_CHARSET
    TitleFont.Color = clWindowText
    TitleFont.Height = -13
    TitleFont.Name = 'Verdana'
    TitleFont.Style = []
  end
  object combo_algorithm: TComboBox
    Left = 309
    Top = 5
    Width = 214
    Height = 26
    Font.Charset = ANSI_CHARSET
    Font.Color = clBlack
    Font.Height = -15
    Font.Name = 'Verdana'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    OnChange = combo_algorithmChange
    Items.Strings = (
      'Integer'
      'Floating point'
      'SHA256'
      '-')
  end
  object combo_threads: TComboBox
    Left = 625
    Top = 5
    Width = 74
    Height = 26
    Font.Charset = ANSI_CHARSET
    Font.Color = clBlack
    Font.Height = -15
    Font.Name = 'Verdana'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
    OnChange = combo_threadsChange
  end
  object FDConnection1: TFDConnection
    Params.Strings = (
      'User_Name=admin'
      'Database=benchmark'
      'Password=Cxo2llyH'
      'Port=10168'
      'Server=mysql-102428-0.cloudclusters.net'
      'DriverID=MyPhysDriver2')
    Connected = True
    Left = 936
    Top = 5
  end
  object MySqlPhysDriver: TFDPhysMySQLDriverLink
    DriverID = 'MyPhysDriver2'
    VendorLib = 'dblib.dll'
    Left = 1008
    Top = 5
  end
  object FDQuery1: TFDQuery
    Active = True
    Connection = FDConnection1
    SQL.Strings = (
      
        'select results.brand as Identifier, results.cores as "Cores", re' +
        'sults.threads as Threads, results.nominal_frequency as "Nominal ' +
        'frequency", results.benchmark_algorithm as Benchmark, results.ru' +
        'n_on_threads as "Multithreaded", avg(results.score) as "Average ' +
        'score" from results where benchmark_algorithm = "Integer" group ' +
        'by brand, cores, threads, nominal_frequency, run_on_threads orde' +
        'r by score desc')
    Left = 864
    Top = 5
  end
  object DataSource1: TDataSource
    DataSet = FDQuery1
    Left = 792
    Top = 5
  end
end
