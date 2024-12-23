// 
// TwoNotes.cpp
// 
// Use ultimatepp (https://ultimatepp.org/) pixelmode branch to compile.
//
// Note that this is unfinished work in progress.
//


#include <CtrlLib/CtrlLib.h>
#include <RichEdit/RichEdit.h>

using namespace Upp;

#define LAYOUTFILE <TwoNotes/TwoNotes.lay>
#include <CtrlCore/lay.h>

class TwoNotes : public WithTwoNotesLayout<TopWindow>
{
	void Open();
	void Save();
	void SaveAs();
	void Quit();
		
	typedef TwoNotes CLASSNAME;
public:
	TwoNotes();	
};

TwoNotes::TwoNotes()
{
	CtrlLayout(*this, "Two Notes");
	
	menu.Set([=](Bar& bar) {
		bar.Sub("File", [=](Bar& bar) {
			bar.Add("Open..", CtrlImg::open(), THISFN(Open));
			bar.Add("Save", CtrlImg::save(), THISFN(Save));
			bar.Add("Save as..", CtrlImg::save_as(), THISFN(SaveAs));
			bar.Separator();
			bar.Add("Quit", THISFN(Quit));
		});
	});	
	
}

void TwoNotes::Open()
{
}

void TwoNotes::Save()
{
}

void TwoNotes::SaveAs()
{
}

void TwoNotes::Quit()
{
}

GUI_APP_MAIN
{

	RichEdit re;
	TwoNotes twoNotes;
	twoNotes.edit.PixelMode();
	twoNotes.edit.ShowCodes(Null);	//hide marks
	
	twoNotes.edit.Clear();
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
	twoNotes.edit.SetQTF(AsQTF(txt));	
	
	twoNotes.Sizeable();
	twoNotes << twoNotes.edit.HSizePos().VSizePos(20, 0);
	twoNotes.Run();
}
