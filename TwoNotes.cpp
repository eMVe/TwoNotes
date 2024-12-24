// 
// TwoNotes.cpp
// 
// Use ultimatepp (https://ultimatepp.org/) pixelmode branch to compile.
//
// Note that this is unfinished work in progress.
// Note: in examples/UWord/UWord.cpp exists a complete word like prototype
//


#include <CtrlLib/CtrlLib.h>
#include <RichEdit/RichEdit.h>

using namespace Upp;

#define LAYOUTFILE <TwoNotes/TwoNotes.lay>
#include <CtrlCore/lay.h>

#include <fstream>
#include <string>
#include <iostream>
#include <streambuf>

class TwoNotes : public WithTwoNotesLayout<TopWindow>
{
	void New();
	void Open();
	void Save();
	void SaveAs();
	void Quit();
	void Destroy();
	
	void SaveEditorContent();
	bool SelectFileNameForSave();
	
	FileSel fileSel;
	String fileName;
	
			
	typedef TwoNotes CLASSNAME;
	
	
	
public:
	void ClearEditor();
	TwoNotes();	
};

TwoNotes::TwoNotes()
{
	CtrlLayout(*this, "Two Notes");
	
	menu.Set([=](Bar& bar) {
		bar.Sub("File", [=](Bar& bar) {
			bar.Add("New", CtrlImg::new_doc(),THISFN(New)).Key(K_CTRL_N).Help("New document");			
			bar.Add("Open..", CtrlImg::open(), THISFN(Open)).Key(K_CTRL_O).Help("Open document");;
			bar.Add("Save", CtrlImg::save(), THISFN(Save)).Key(K_CTRL_S).Help("Save current document");
			bar.Add("Save as..", CtrlImg::save_as(), THISFN(SaveAs));
			bar.Separator();
			bar.Add("Quit", THISFN(Quit));
		});
	});	
	
	WhenClose = THISBACK(Destroy);
	
}

void TwoNotes::Destroy()
{
	if(edit.IsModified()) {
		switch(PromptYesNoCancel("Do you want to save the changes to the document?")) {
		case 1:
			Save();
			break;
		case -1:
			return;
		}
	}
	delete this;
}

void TwoNotes::Open()
{
	char cwd[MAX_PATH];
	getcwd(cwd, MAX_PATH);
	fileSel.Type("QTF Files","*.qtf").Type("All Files","*.*").DefaultExt("qtf");	
	if(!fileSel.BaseDir(cwd).ExecuteOpen()) return;	
	fileName = fileSel.Get();
	
	std::ifstream t(fileName);
	std::string str((std::istreambuf_iterator<char>(t)),
	std::istreambuf_iterator<char>());
	
	edit.SetQTF(str.c_str());
	edit.ClearModify();	
}

void TwoNotes::New()
{
	if(edit.IsModified()) {
		switch(PromptYesNoCancel("Do you want to save the changes to the document first?")) {
		case 1:
			Save();
			break;
		case -1:
			return;
		}
	}
	ClearEditor();
}

void TwoNotes::Save()
{
	if(IsEmpty(fileName)) {
		if(!SelectFileNameForSave()) {
			return;
		}
	}
	SaveEditorContent();
}

bool TwoNotes::SelectFileNameForSave()
{
	char cwd[MAX_PATH];
	getcwd(cwd, MAX_PATH);
	fileSel.Type("QTF Files","*.qtf").Type("All Files","*.*").DefaultExt("qtf");	
	if(!fileSel.BaseDir(cwd).ExecuteSaveAs()) return false;	
	fileName = fileSel.Get();	
	return true;
}

void TwoNotes::SaveEditorContent()
{
	String qtf = edit.GetQTF();
	std::string input(qtf);
    std::cin >> input;
    std::ofstream out(fileName);
    out << input;
    out.close();
    edit.ClearModify();
}


void TwoNotes::SaveAs()
{
	if(SelectFileNameForSave()) {
		SaveEditorContent();
	}
}

void TwoNotes::ClearEditor()
{
	edit.Clear();
	//Set initial font to fixed size one by adding empty text object to the editor.
	//Maybe there is a better way to do it...?
	RichText txt;
	{
		RichPara para;
		RichPara::Format fmt;
		//(Font&)fmt = Monospace(120).Bold();
		(Font&)fmt = Monospace(130);
		para.Cat("", fmt);
		txt.Cat(para);
	}
	edit.SetQTF(AsQTF(txt));
}

void TwoNotes::Quit()
{
	Destroy();
}

GUI_APP_MAIN
{

	RichEdit re;
	TwoNotes twoNotes;
	twoNotes.edit.PixelMode();
	twoNotes.edit.ShowCodes(Null);	//hide marks
	twoNotes.ClearEditor();
	
	twoNotes.Sizeable();
	twoNotes << twoNotes.edit.HSizePos().VSizePos(20, 0);
	twoNotes.Run();
}
