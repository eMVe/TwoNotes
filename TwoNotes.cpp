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
	
	void SaveEditorContent();
	void LoadEditorContent();
	bool SelectFileNameForSave();
	
	FileSel fileSel;
	String m_fileName;
	
			
	typedef TwoNotes CLASSNAME;
	
	
	
public:
	virtual void DragAndDrop(Point, PasteClip& d);
	void ClearEditor();
	TwoNotes();	
};

TwoNotes::TwoNotes()
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
	m_fileName = fileSel.Get();
	LoadEditorContent();
}
	
void TwoNotes::LoadEditorContent()
{
	std::ifstream t(m_fileName);
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
	if(IsEmpty(m_fileName)) {
		if(!SelectFileNameForSave()) {
			ErrorOK("Not saved");
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
	m_fileName = fileSel.Get();	
	return true;
}

void TwoNotes::SaveEditorContent()
{
	String qtf = edit.GetQTF();
	std::string input(qtf);
    std::cin >> input;
    std::ofstream out(m_fileName);
    out << input;
    out.close();
    edit.ClearModify();
    
#if 0
    RichText txt = ParseQTF(qtf);
    
	Index<String> css;
	VectorMap<String, String> links;
	String outdir = GetExeDirFile("html");
	RealizeDirectory(outdir);
	String body = EncodeHtml(txt, css, links, outdir);
		
	String html = AppendFileName(outdir, "test.html");

	SaveFile(html,
	         "<html><STYLE TYPE=\"text/css\"><!--\r\n" + AsCss(css) + "\r\n-->\r\n</STYLE>\r\n"
		            "<body>" + body + "</body></html>");	
#else
    
    RichText txt = ParseQTF(qtf);
        
	String outdir = GetExeDirFile("html");
	RealizeDirectory(outdir);
	
	String pathNoExt = m_fileName.Left(m_fileName.ReverseFind('.'));	//assume that m_fileName has at least one dot in it
	String fileNameNoExt = GetFileName(pathNoExt);
	
	RealizeDirectory(outdir + "/" + fileNameNoExt + ".img/");				//make sure that dir for images exists
	String imageDir = fileNameNoExt + ".img/";
	String body = EncodeHtmlSimple(txt, outdir, imageDir);	//also save images
	String fileName = AppendFileName(outdir, fileNameNoExt + ".html");
	SaveFile(fileName,
	         "<html><body><pre>" + body + "</pre></body></html>");	

#endif
    
    
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
    edit.ClearModify();
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

	RichEdit re;
	TwoNotes twoNotes;
	twoNotes.edit.PixelMode();
	twoNotes.edit.ShowCodes(Null);	//hide marks
	twoNotes.ClearEditor();
	
	twoNotes.Sizeable();
	twoNotes << twoNotes.edit.HSizePos().VSizePos(20, 0);
	twoNotes.Run();
}
