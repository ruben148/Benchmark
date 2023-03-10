//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Data.DB.hpp>
#include <FireDAC.Comp.Client.hpp>
#include <FireDAC.Comp.DataSet.hpp>
#include <FireDAC.DApt.hpp>
#include <FireDAC.DApt.Intf.hpp>
#include <FireDAC.DatS.hpp>
#include <FireDAC.Phys.hpp>
#include <FireDAC.Phys.Intf.hpp>
#include <FireDAC.Phys.MySQL.hpp>
#include <FireDAC.Phys.MySQLDef.hpp>
#include <FireDAC.Stan.Async.hpp>
#include <FireDAC.Stan.Def.hpp>
#include <FireDAC.Stan.Error.hpp>
#include <FireDAC.Stan.Intf.hpp>
#include <FireDAC.Stan.Option.hpp>
#include <FireDAC.Stan.Param.hpp>
#include <FireDAC.Stan.Pool.hpp>
#include <FireDAC.UI.Intf.hpp>
#include <FireDAC.VCLUI.Wait.hpp>
#include <Vcl.DBGrids.hpp>
#include <Vcl.Grids.hpp>
#include <string>
#include <sstream>
#include <iostream>
//---------------------------------------------------------------------------
class TForm2 : public TForm
{
__published:	// IDE-managed Components
	TFDConnection *FDConnection1;
	TFDPhysMySQLDriverLink *MySqlPhysDriver;
	TDBGrid *DBGrid1;
	TFDQuery *FDQuery1;
	TDataSource *DataSource1;
	TLabel *Label1;
	TComboBox *combo_algorithm;
	TLabel *Label2;
	TComboBox *combo_threads;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall combo_algorithmChange(TObject *Sender);
	void __fastcall combo_threadsChange(TObject *Sender);
	void __fastcall FDConnection1Error(TObject *ASender, TObject *AInitiator, Exception *&AException);

private:	// User declarations
public:		// User declarations
	__fastcall TForm2(TComponent* Owner);
	void setThreads(int threads){
		combo_threads->AddItem("Any", NULL);
		combo_threads->AddItem(1, NULL);
		for(int i = 2, j = 1; i<=threads; i+=2, j++)
			combo_threads->AddItem(i, NULL);
	}
};
//---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
//---------------------------------------------------------------------------
#endif
