//
// TwoNotes.cpp
//
// by Michal Vanka (github.com/eMVe)
//
// Use ultimatepp (https://ultimatepp.org/) pixelmode branch to compile.
//
// Note: This is unfinished work in progress.
// Note: Portions taken from examples/UWord/UWord.cpp
//


#include <CtrlLib/CtrlLib.h>
#include <RichEdit/RichEdit.h>
#include <PdfDraw/PdfDraw.h>
#include "helpers.h"

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

	bool SaveEditorContent();
	bool SelectFileNameForSave();
	void GenerateHtml();
	void GeneratePdf();
	bool SaveIt();

	TwoFileSel fileSel;
	String m_fileName;
	bool m_bAutoGenerateHtml;
	bool m_bAutoGeneratePdf;

	typedef TwoNotes CLASSNAME;


public:
	StatusBar statusBar;

	virtual void DragAndDrop(Point, PasteClip& d);
	void ClearEditor();
	String GetFileName();
	void LoadEditorContent();
	TwoNotes();
};

TwoNotes::TwoNotes()
	:m_bAutoGenerateHtml(true)
	,m_bAutoGeneratePdf(true)
{
	CtrlLayout(*this, "Two Notes V 0.0.1");

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

	if(CommandLine().size()) {
		m_fileName = CommandLine()[0];
	}
}

void TwoNotes::Destroy()
{
	if(editor.IsModified()) {
		switch(PromptYesNoCancel("Do you want to save the changes to the document?")) {
		case 1:
			if(SaveIt() == false) return;
			else break;
		case -1:
			return;
		}
	}
	Close();
}

String TwoNotes::GetFileName()
{
	return m_fileName;
}

void TwoNotes::Open()
{
	char cwd[MAX_PATH];
	getcwd(cwd, MAX_PATH);
	fileSel.Type("QTF Files","*.qtf").Type("All Files","*.*").DefaultExt("qtf");
	if(!fileSel.InitialDir(cwd).ExecuteOpen()) return;
	m_fileName = fileSel.Get();
	LoadEditorContent();
}

void TwoNotes::LoadEditorContent()
{
	if(FileExists(m_fileName)) {
		std::ifstream t(m_fileName);
		std::string str((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

		editor.SetQTF(str.c_str());
		editor.ClearModify();
		statusBar = m_fileName;
	}
}

void TwoNotes::New()
{
	if(editor.IsModified()) {
		switch(PromptYesNoCancel("Do you want to save the changes to the document first?")) {
		case 1:
			if(SaveIt() == false) return;
			else break;
		case -1:
			return;
		}
	}
	ClearEditor();
    m_fileName = "";
    statusBar = "";
}

void TwoNotes::Save()
{
	SaveIt();
}

bool TwoNotes::SaveIt()
{
	if(IsEmpty(m_fileName)) {
		if(!SelectFileNameForSave()) {
			ErrorOK("Not saved");
			return false;
		}
	}
	return SaveEditorContent();
}

bool TwoNotes::SelectFileNameForSave()
{
	char cwd[MAX_PATH];
	getcwd(cwd, MAX_PATH);
	fileSel.Type("QTF Files","*.qtf").Type("All Files","*.*").DefaultExt("qtf");
	if(!fileSel.InitialDir(cwd).ExecuteSaveAs()) return false;
	m_fileName = fileSel.Get();
	return true;
}

void TwoNotes::GenerateHtml()
{
	String qtf = editor.GetQTF();
    RichText txt = ParseQTF(qtf);

   	String outdir = ::GetFileFolder(m_fileName);

   	if(outdir.IsEmpty()) outdir = "html";
   	else outdir += "/html";

	RealizeDirectory(outdir);

	String pathNoExt = m_fileName.Left(m_fileName.ReverseFind('.'));	//assume that m_fileName has at least one dot in it
	String fileNameNoExt = ::GetFileName(pathNoExt);

	RealizeDirectory(outdir + "/" + fileNameNoExt + ".img/");				//make sure that dir for images exists
	String imageDir = fileNameNoExt + ".img/";
	String body = EncodeHtmlSimple(txt, outdir, imageDir);	//also save images
	String fileName = AppendFileName(outdir, fileNameNoExt + ".html");
	SaveFile(fileName,
	         "<html><body><pre>" + body + "</pre></body></html>");
}

void TwoNotes::GeneratePdf()
{

	String outdir = GetExeDirFile("pdf");
	RealizeDirectory(outdir);

	String pathNoExt = m_fileName.Left(m_fileName.ReverseFind('.'));	//assume that m_fileName has at least one dot in it
	String fileNameNoExt = ::GetFileName(pathNoExt);
	String fileName = AppendFileName(outdir, fileNameNoExt + ".pdf");

	Size page = Size(3968, 6074);
	PdfDraw pdf;
	UPP::Print(pdf, editor.Get(), page);
	SaveFile(fileName, pdf.Finish());
}

bool TwoNotes::SaveEditorContent()
{
	String qtf = editor.GetQTF();
	std::string input(qtf);
    std::cin >> input;
    std::ofstream out(m_fileName);
    out << input;
    out.close();
    editor.ClearModify();
	if(m_bAutoGenerateHtml) {	//keep html in sync with qtf
		GenerateHtml();
	}
	if(m_bAutoGeneratePdf) {
		//GeneratePdf();		//doesn't scale to our needs yet
	}
	statusBar = m_fileName;
	statusBar.Temporary("Saved...");
	return true;
}

void TwoNotes::SaveAs()
{
	if(SelectFileNameForSave()) {
		SaveEditorContent();
	}
}

void TwoNotes::ClearEditor()
{
	editor.Clear();
	//Set initial font to fixed size one by adding empty text object to the editor.
	//Maybe there is a better way to do it...?
	RichText txt;
	{
		RichPara para;
		RichPara::Format fmt;
		//(Font&)fmt = Monospace(120).Bold();
		(Font&)fmt = Monospace(130);	//TODO font size configurable
		para.Cat("", fmt);
		txt.Cat(para);
	}
	editor.SetQTF(AsQTF(txt));
    editor.ClearModify();
}

void TwoNotes::Quit()
{
	Destroy();
}

void TwoNotes::DragAndDrop(Point, PasteClip& d)
{
	if(IsAvailableFiles(d)) {
		Vector<String> fn = GetFiles(d);
		for(int open = 0; open < 2; open++) {
			for(int i = 0; i < fn.GetCount(); i++) {
				String ext = GetFileExt(fn[i]);
				if(FileExists(fn[i]) && (ext == ".qtf")) {
					if(open) {
						m_fileName = fn[i];
						LoadEditorContent();
					}
					else {
						if(d.Accept())
							break;
						return;
					}
				}
			}
			if(!d.IsAccepted())
				return;
		}
	}
}

GUI_APP_MAIN
{
	RichEdit re;						//TODO not used, REMOVE
	TwoNotes twoNotes;
	twoNotes.editor.PixelMode();
	twoNotes.editor.ShowCodes(Null);	//hide marks
	twoNotes.ClearEditor();

	if(!twoNotes.GetFileName().IsEmpty()) {
		twoNotes.LoadEditorContent();
	}

	twoNotes.Sizeable();
	twoNotes << twoNotes.editor.HSizePos().VSizePos(20, 0);
	twoNotes.AddFrame(twoNotes.statusBar);

	twoNotes.Run();
}
