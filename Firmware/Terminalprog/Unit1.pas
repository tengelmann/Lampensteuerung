unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, CPort, CPortCtl, ActnList, ImgList, Buttons, ComCtrls;

type
  TForm1 = class(TForm)
    mMessages: TMemo;
    cmprtComPort: TComPort;
    ilImages: TImageList;
    actlst1: TActionList;
    actComConnect: TAction;
    actComDisconnect: TAction;
    grpTop: TGroupBox;
    statStatusBar: TStatusBar;
    cbbBaudRate: TComComboBox;
    cbbPortSelect: TComComboBox;
    btnComConnect: TBitBtn;
    procedure cmprtComPortAfterClose(Sender: TObject);
    procedure cmprtComPortAfterOpen(Sender: TObject);
    procedure cmprtComPortBeforeOpen(Sender: TObject);
    procedure actComConnectExecute(Sender: TObject);
    procedure actComDisconnectExecute(Sender: TObject);
    procedure cmprtComPortRxBuf(Sender: TObject; const Buffer: PAnsiChar;
      Count: Integer);
  private
    { Private-Deklarationen }
  public
    { Public-Deklarationen }
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

procedure TForm1.actComConnectExecute(Sender: TObject);
begin
        if cbbPortSelect.ItemIndex >= 0 then
        begin
                cmprtComPort.Port := cbbPortSelect.Text;
                cmprtComPort.Open;
        end;
end;

procedure TForm1.actComDisconnectExecute(Sender: TObject);
begin
        if cmprtComPort.Connected then
                cmprtComPort.Close;
end;

procedure TForm1.cmprtComPortAfterClose(Sender: TObject);
begin
        btnComConnect.Action := actComConnect;
     mMessages.Lines.Add(Format('Serial Port <%s> closed.', [Trim(cmprtComPort.Port)]));
end;

procedure TForm1.cmprtComPortAfterOpen(Sender: TObject);
begin
     btnComConnect.Action := actComDisconnect;
     mMessages.Lines.Add(Format('Serial Port <%s> opened.', [Trim(cmprtComPort.Port)]));

end;

procedure TForm1.cmprtComPortBeforeOpen(Sender: TObject);
begin
        mMessages.Lines.Add(Format('Opening <%s> ...', [Trim(cmprtComPort.Port)]));
end;

procedure TForm1.cmprtComPortRxBuf(Sender: TObject; const Buffer: PAnsiChar;
  Count: Integer);
begin
     mMessages.Lines.Add(Format('Received: %d Bytes.', [Count]));
end;

end.
