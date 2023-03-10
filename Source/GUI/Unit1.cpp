//---------------------------------------------------------------------------

#include <vcl.h>
#include <System.hpp>
#pragma hdrstop

#include "Unit1.h"
#include <tchar.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TForm1 *Form1;

using namespace boost::interprocess;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------


void __fastcall TForm1::FormCreate(TObject *Sender)
{
	radio_algorithm->ItemIndex = 0;
	Worker* control = new Worker(false, this);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
    TerminateProcess(pi.hProcess, 0);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::button_startClick(TObject *Sender)
{
	shouldStart = true;
	button_start->Enabled = false;
	radio_algorithm->Enabled = false;
	combo_threads->Enabled = false;
	edit_seconds->Enabled = false;
}
//---------------------------------------------------------------------------



void TForm1::drawGraph(FixedQueue<int,40> usages)
{
	((PanelHack*)graph)->Canvas->Pen->Width = 2;
	//((PanelHack*)graph)->Canvas->Pen->Color = clSkyBlue;
	((PanelHack*)graph)->Canvas->FillRect(Rect(0,0,graph->Width,graph->Height));
	((PanelHack*)graph)->Canvas->MoveTo(0, (graph->Height-2) - (*usages.begin())*(graph->Height-2)/100);
	int i = 0;
	for (auto it = usages.begin(); it != usages.end(); ++it, i++)
	{
		((PanelHack*)graph)->Canvas->LineTo(i*graph->Width/40, (graph->Height-2) - (*it)*(graph->Height-2)/100);
	}
}



__fastcall Worker::Worker(bool CreateSuspended, TForm1 *f)
	: TThread(CreateSuspended)
{
	Form1 = f;
}
//---------------------------------------------------------------------------

using namespace boost::interprocess;

void __fastcall Worker::Execute()
{

	try {
		message_queue::remove("GUI_TO_BENCH");
	}
	catch (interprocess_exception& ex) {

	}

	message_queue gui_to_bench(create_only, "GUI_TO_BENCH", 200, sizeof(int));
	int command = 9; //send info please
	gui_to_bench.send(&command, sizeof(command), 0);

	try {
		message_queue::remove("BENCH_TO_GUI");
	}
	catch (interprocess_exception& ex) {

	}

	message_queue bench_to_gui(create_only, "BENCH_TO_GUI", 200, sizeof(int));

	Synchronize(&CreateProcessBench);

	try
	{
		unsigned int priority;
		message_queue::size_type recvd_size;
		int j;
		for(int i=0;i<64;i++){
			bench_to_gui.receive(&brand[i], sizeof(int), recvd_size, priority);
		}
		for(int i=0;i<32;i++){
			bench_to_gui.receive(&vendor[i], sizeof(int), recvd_size, priority);
		}
		for(int i=0;i<256;i++){
			bench_to_gui.receive(&gpuName[i], sizeof(int), recvd_size, priority);
		}
		for(int i=0;i<1024;i++){
			bench_to_gui.receive(&instructions[i], sizeof(int), recvd_size, priority);
		}
		for(int i=0;i<1024;i++){
			bench_to_gui.receive(&drives[i], sizeof(int), recvd_size, priority);
		}
        for(int i=0;i<1024;i++){
			bench_to_gui.receive(&storageDevices[i], sizeof(int), recvd_size, priority);
		}
		bench_to_gui.receive(&processorCoreCount, sizeof(DWORD), recvd_size, priority);
		bench_to_gui.receive(&logicalProcessorCount, sizeof(DWORD), recvd_size, priority);
		bench_to_gui.receive(&processorL1CacheCount, sizeof(DWORD), recvd_size, priority);
		bench_to_gui.receive(&processorL2CacheCount, sizeof(DWORD), recvd_size, priority);
		bench_to_gui.receive(&processorL3CacheCount, sizeof(DWORD), recvd_size, priority);
		bench_to_gui.receive(&totalMemory, sizeof(int), recvd_size, priority);
		bench_to_gui.receive(&availableMemory, sizeof(int), recvd_size, priority);
		bench_to_gui.receive(&memoryLength, sizeof(int), recvd_size, priority);
		bench_to_gui.receive(&pageSize, sizeof(int), recvd_size, priority);
		bench_to_gui.receive(&nominalFrequency, sizeof(int), recvd_size, priority);
		Synchronize(&UpdateStatic);
	}
	catch(interprocess_exception& ex)
	{

	}

	while(true)
	{
		Synchronize(&GetStart);
        Synchronize(&GetStartMemory);
		if(shouldStart)
		{
            Synchronize(&GetAlgorithmIndex);
			Synchronize(&GetThreads);
			Synchronize(&GetSeconds);
			gui_to_bench.send(&algorithmIndex, sizeof(int), 0);
			gui_to_bench.send(&threads, sizeof(int), 0);
			gui_to_bench.send(&seconds, sizeof(int), 0);
			try
			{
				unsigned int priority;
				message_queue::size_type recvd_size;
				while(true)
				{
					bench_to_gui.receive(&usage, sizeof(int), recvd_size, priority);
					bench_to_gui.receive(&frequency, sizeof(int), recvd_size, priority);
					bench_to_gui.receive(&availableMemory, sizeof(int), recvd_size, priority);
					bench_to_gui.receive(&message, sizeof(int), recvd_size, priority);
					if(usage == -2)
					{
						message = -2;
						shouldStart = false;
						Synchronize(&SetStart);
						break;
					}
					score = message;
					Synchronize(&UpdateDynamic);
					Synchronize(&UpdateScore);

					usages.push(usage);
					Synchronize(&Draw);
				}
			}
			catch(interprocess_exception& ex)
			{

			}
			Synchronize(&CreateBenchmarkResult);
		}
		else if(shouldStartMemory)
		{
            int command = 6;
			gui_to_bench.send(&command, sizeof(command), 0);

			Synchronize(&GetAllocationSize);
            allocation_size *= 1024; //send the number of bytes
			gui_to_bench.send(&allocation_size, sizeof(int), 0);
            try
			{
				unsigned int priority;
				message_queue::size_type recvd_size;

				bench_to_gui.receive(&message, sizeof(int), recvd_size, priority);
				Synchronize(&UpdateScoreMemory);
				message = -2;
				shouldStartMemory = false;
				Synchronize(&SetStartMemory);
			}
			catch(interprocess_exception& ex)
			{

			}
		}
        else
		{
            int command = 5;
			gui_to_bench.send(&command, sizeof(command), 0);
			try
			{
				unsigned int priority;
				message_queue::size_type recvd_size;
				bench_to_gui.receive(&usage, sizeof(int), recvd_size, priority);
				bench_to_gui.receive(&frequency, sizeof(int), recvd_size, priority);
				bench_to_gui.receive(&availableMemory, sizeof(int), recvd_size, priority);
				Synchronize(&UpdateDynamic);
				usages.push(usage);
                Synchronize(&Draw);
			}
			catch(interprocess_exception& ex)
			{

			}
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall Worker::UpdateStatic()
{
	Form1->memo_gpu->WordWrap = true;
	Form1->memo_gpu->Lines->BeginUpdate();
	Form1->memo_gpu->Lines->Add(gpuName);
	Form1->memo_gpu->Lines->EndUpdate();
	Form1->edit_brand->Text = brand;
	Form1->edit_vendor->Text = vendor;
	Form1->memo_drives->WordWrap = true;
	Form1->memo_drives->Lines->BeginUpdate();
	Form1->memo_drives->Lines->Add("Drives:");
	Form1->memo_drives->Lines->Add(drives);
	Form1->memo_drives->Lines->Add("Devices:");
	Form1->memo_drives->Lines->Add(storageDevices);
	Form1->memo_drives->Lines->EndUpdate();
	strcpy(Form1->instructions, instructions);
	Form1->label_instructions->Hint = instructions;
	Form1->edit_cores->Text = processorCoreCount;
	Form1->edit_threads->Text = logicalProcessorCount;
	Form1->edit_caches->Text += processorL1CacheCount;
	Form1->edit_caches->Text += "/";
	Form1->edit_caches->Text += processorL2CacheCount;
	Form1->edit_caches->Text += "/";
	Form1->edit_caches->Text += processorL3CacheCount;
	Form1->edit_total_memory->Text = totalMemory;
	Form1->edit_total_memory->Text += " MB";
	Form1->edit_available_memory->Text = availableMemory;
	Form1->edit_available_memory->Text += " MB";
	Form1->edit_available_memory->Text += " (";
	Form1->edit_available_memory->Text += ((totalMemory - availableMemory)*100)/totalMemory;
	Form1->edit_available_memory->Text += "% used)";
	Form1->edit_page_size->Text = pageSize;
	Form1->edit_page_size->Text += " B";
	Form1->edit_length->Text = memoryLength;
	Form1->edit_length->Text += " bit";
	Form1->edit_nominal_frequency->Text = nominalFrequency;
	Form1->edit_nominal_frequency->Text += " Mhz";
	Form1->combo_threads->AddItem(1, NULL);
	for(int i = 2, j = 1; i<=logicalProcessorCount; i+=2, j++)
		Form1->combo_threads->AddItem(i, NULL);
	Form1->combo_threads->ItemIndex = 0;
	Form2->setThreads(logicalProcessorCount);
	Form2->combo_threads->ItemIndex = algorithmIndex;
	Form2->combo_algorithm->ItemIndex = 0;
}

void __fastcall Worker::UpdateDynamic()
{
	Form1->edit_usage->Text = usage;
	Form1->edit_usage->Text += "%";
	Form1->edit_available_memory->Text = availableMemory;
	Form1->edit_available_memory->Text += " MB";
	Form1->edit_memory_usage->Text = ((totalMemory - availableMemory)*100)/totalMemory;
	Form1->edit_memory_usage->Text += "%";
}

void __fastcall Worker::GetAlgorithmIndex()
{
	algorithmIndex = Form1->radio_algorithm->ItemIndex;
}

void __fastcall Worker::GetStart()
{
    shouldStart = Form1->shouldStart;
}

void __fastcall Worker::SetStart()
{
	Form1->button_start->Enabled = !shouldStart;
	Form1->radio_algorithm->Enabled = !shouldStart;
	Form1->combo_threads->Enabled = !shouldStart;
	Form1->edit_seconds->Enabled = !shouldStart;
	Form1->shouldStart = shouldStart;
}

void __fastcall Worker::GetStartMemory()
{
    shouldStartMemory = Form1->shouldStartMemory;
}

void __fastcall Worker::SetStartMemory()
{
	Form1->button_start_memory->Enabled = !shouldStartMemory;
	Form1->shouldStartMemory = shouldStartMemory;
}

void __fastcall Worker::UpdateScore()
{
	Form1->label_score->Caption = message;
}

void __fastcall Worker::UpdateScoreMemory()
{
	Form1->label_score_memory->Caption = message;
}

void __fastcall Worker::GetThreads()
{
	threads = StrToInt(Form1->combo_threads->Text);
}

void __fastcall Worker::GetSeconds()
{
    seconds = StrToInt(Form1->edit_seconds->Text);
}

void __fastcall Worker::GetAllocationSize()
{
    allocation_size = StrToInt(Form1->edit_allocation_size->Text);
}

void __fastcall Worker::Draw()
{
	Form1->drawGraph(usages);
}

void __fastcall Worker::CreateBenchmarkResult()
{
	Form1->lastResult = new BenchmarkResult();
	/*
	//strcpy(Form1->lastResult->brand, brand);
	Form1->lastResult->brand = brand;
	//strcpy(Form1->lastResult->vendor, vendor);
	Form1->lastResult->vendor = vendor;
	//strcpy(Form1->lastResult->gpuName, gpuName);
	Form1->lastResult->gpuName = gpuName;
	//strcpy(Form1->lastResult->instructions, instructions);
	Form1->lastResult->instructions = instructions;
	//strcpy(Form1->lastResult->storageDevices, storageDevices);
	Form1->lastResult->storageDevices = storageDevices;
	Form1->lastResult->cores = processorCoreCount;
	Form1->lastResult->threads = logicalProcessorCount;
	std::stringstream ss;
	ss<<processorL1CacheCount<<"/"<<processorL2CacheCount<<"/"<<processorL3CacheCount;
	Form1->lastResult->cacheCount = ss.str().c_str();
	Form1->lastResult->totalMemory = totalMemory;
	Form1->lastResult->memoryLength = memoryLength;
	Form1->lastResult->pageSize = pageSize;
	Form1->lastResult->nominalFrequency = nominalFrequency;*/
	switch(algorithmIndex){
	case 0:		{
		std::stringstream s1;
		s1<<"Integer";
		strcpy(Form1->lastResult->benchmarkAlgorithm, s1.str().c_str());
		//Form1->lastResult->benchmarkAlgorithm = "Integer";
		break;     		  }
	case 1:     {
		std::stringstream s2;
		s2<<"Floating point";
		strcpy(Form1->lastResult->benchmarkAlgorithm, s2.str().c_str());
		//Form1->lastResult->benchmarkAlgorithm = "Floating point";
		break;            }
	case 2:     {
		std::stringstream s3;
		s3<<"SHA256";
		strcpy(Form1->lastResult->benchmarkAlgorithm, s3.str().c_str());
		//Form1->lastResult->benchmarkAlgorithm = "SHA256";
		break;            }
	case 3:     {
		std::stringstream s4;
		s4<<"-";
		strcpy(Form1->lastResult->benchmarkAlgorithm, s4.str().c_str());
		//Form1->lastResult->benchmarkAlgorithm = "-";
		break;            }
	}
	//Form1->lastResult->run_on_threads = threads;
	//Form1->lastResult->seconds = seconds;
	std::stringstream s;
	s<<"insert into results (brand, vendor, cores, threads, nominal_frequency, cache_number, supported_instructions, storage_devices, gpu_names, total_memory, page_size, benchmark_algorithm, run_on_threads, seconds, score) values (";
	s<<"'"<<brand<<"',"<<"'"<<vendor<<"',"<<"'"<<processorCoreCount<<"',"<<"'"<<logicalProcessorCount<<"',"<<"'"<<nominalFrequency<<"',"<<"'"<<processorL1CacheCount<<"/"<<processorL2CacheCount<<"/"<<processorL3CacheCount<<"',";
	s<<"'"<<instructions<<"','"<<storageDevices<<"','"<<gpuName<<"','"<<totalMemory<<"','"<<pageSize<<"','"<<Form1->lastResult->benchmarkAlgorithm<<"','"<<threads<<"','"<<seconds<<"','"<<score<<"')";
	String insertStatement = s.str().c_str();

	SYSTEM_POWER_STATUS spsPwr;
	int batteryStatus;
	if( GetSystemPowerStatus(&spsPwr) ) {
		batteryStatus = static_cast<int>(spsPwr.ACLineStatus);
	} else batteryStatus = 0;

	if(seconds >= 8 && algorithmIndex != 3 && batteryStatus != 0)
		Form2->FDConnection1->ExecSQL(insertStatement);

	Form2->combo_threads->ItemIndex = Form1->combo_threads->ItemIndex + 1;
	Form2->combo_algorithm->ItemIndex = Form1->radio_algorithm->ItemIndex;


	std::stringstream ss;
	ss<<"select results.brand as Identifier, results.cores as \"Cores\", results.threads as Threads, results.nominal_frequency as \"Nominal frequency\", results.benchmark_algorithm as Benchmark, results.run_on_threads as \"Multithreaded\", avg(results.score) as \"Average score\" from results where ";
	ss<<"run_on_threads = \""<<threads<<"\" and ";
	ss<<"benchmark_algorithm = \"";
	ss<<Form1->lastResult->benchmarkAlgorithm;
	ss<<"\" group by brand, cores, threads, nominal_frequency ";
    ss<<"order by score desc";
	String updateStatement = ss.str().c_str();
	Form2->FDQuery1->SQL->Text = updateStatement;
	Form2->FDQuery1->Open();
	Form2->DBGrid1->Columns->Items[0]->Width = 430;
	Form2->DBGrid1->Columns->Items[1]->Width = 50;
	Form2->DBGrid1->Columns->Items[2]->Width = 65;
	Form2->DBGrid1->Columns->Items[3]->Width = 140;
	Form2->DBGrid1->Columns->Items[4]->Width = 130;
	Form2->DBGrid1->Columns->Items[5]->Width = 110;
	Form2->DBGrid1->Columns->Items[6]->Width = 100;
}

void __fastcall Worker::CreateProcessBench()
{
	ZeroMemory(&Form1->si, sizeof(Form1->si));
	Form1->si.cb = sizeof(Form1->si);
	ZeroMemory(&Form1->pi, sizeof(Form1->pi));

	LPWSTR buffer;
	buffer = (LPWSTR) malloc(MAX_PATH*sizeof(wchar_t));
	GetModuleFileName( NULL, buffer, MAX_PATH );
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");

	buffer[pos+1] = '\0';
	std::wstring bufferString(buffer);
	std::wstring concatted_stdstr = bufferString + L"bench.exe";
	LPCWSTR concatted = concatted_stdstr.c_str();

	LPCWSTR path = concatted;

	LPWSTR argv[10];
	argv[1] = (LPWSTR)malloc(10 * sizeof(wchar_t));

    if(CreateProcess(path,   // the path
		argv[1],        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory
		&Form1->si,            // Pointer to STARTUPINFO structure
		&Form1->pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	)==0)
		;
	else
		;

}

void __fastcall TForm1::button_start_memoryClick(TObject *Sender)
{
    shouldStartMemory = true;
	button_start_memory->Enabled = false;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::trackbar_allocation_sizeChange(TObject *Sender)
{
    edit_allocation_size->Text = trackbar_allocation_size->Position;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::label_instructionsClick(TObject *Sender)
{
	//ShowMessage(instructions);
}
//---------------------------------------------------------------------------







void __fastcall TForm1::button_view_scoresClick(TObject *Sender)
{
	Form2->Show();
}
//---------------------------------------------------------------------------

