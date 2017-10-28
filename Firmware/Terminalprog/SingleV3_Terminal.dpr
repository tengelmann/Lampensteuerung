program SingleV3_Terminal;

uses
  Forms,
  fmMainForm in 'fmMainForm.pas' {MainForm};

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TMainForm, MainForm);
  Application.Run;
end.
