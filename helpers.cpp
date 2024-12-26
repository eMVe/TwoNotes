//
// helpers.cpp
//
// by Michal Vanka (github.com/eMVe)
//
// Use ultimatepp (https://ultimatepp.org/) pixelmode branch to compile.
//

#include "helpers.h"

#include <CtrlLib/CtrlLib.h>
#include <RichText/RichText.h>

namespace Upp {

String SimpleHtmlObjectSaver::GetHtml(const RichObject& object, const String& link)
{
	StringBuffer html;
	String name;
	name << namebase << "_" << im++ << ".png";
	Size psz = object.GetPixelSize();
	Size sz = object.GetSize();
	PNGEncoder png;
	png.SaveFile(AppendFileName(outdir, name), object.ToImage(psz));
	String el = "</a>";
	if(IsNull(link)) {
		if(psz.cx * psz.cy != 0)
			html << "<a href=\"" << name << "\">";
		else
			el.Clear();
	}
	else
		html << "<a href=\"" << link << "\">";
	html << Format("<img src=\"%s\" border=\"0\" alt=\"\">",
	               name);
	html << el;
	return String(html);
}

String AsHtmlSimple(const RichTxt& text, const RichStyles& styles,
              const VectorMap<String, String>& escape,
              HtmlObjectSaver& object_saver,
              RichPara::Number& n)
{

	String html;
	for(int i = 0; i < text.GetPartCount(); i++) {
		if(text.IsTable(i)) {
			//omit tables
		}
		else
		if(text.IsPara(i)) {
			RichPara p = text.Get(i, styles);
			//if(p.format.ruler)
			//	html << "<hr>";
			bool bultext = false;
			String number;
			if(p.format.GetNumberLevel() > 0) {
				n.Next(p.format);
				number = n.AsText(p.format);
			}

			for(int i = 0; i < p.part.GetCount(); i++) {
				const RichPara::Part& part = p.part[i];
				int q;
				String lnk = part.format.link;

				if(part.object) {
					html << object_saver.GetHtml(part.object, lnk);	//save image here
				}
				else
				if(part.format.indexentry.GetCount() &&
				   (q = escape.Find(part.format.indexentry.ToString())) >= 0)
					html << escape[q];
				else {
					//x String endtag;
					//x if(!lnk.IsEmpty() && lnk[0] != ':') {
					//x 	html << "<a href=\"" << lnk << "\">";
					//x 	endtag = "</a>";
					//x }
					String cs;
					//? cs = HtmlCharStyle(part.format, p.format);
					//x if(!cs.IsEmpty()) {
					//x 	html << "<span" << FormatClass(css, cs) << ">";
					//x 	endtag = "</span>" + endtag;
					//x }
					//x if(part.format.sscript == 1) {
					//x 	html << "<sup>";
					//x 	endtag = "</sup>" + endtag;
					//x }
					//x if(part.format.sscript == 2) {
					//x 	html << "<sub>";
					//x 	endtag = "</sub>" + endtag;
					//x }
					//x if(part.format.IsUnderline()) {
					//x 	html << "<u>";
					//x 	endtag = "</u>" + endtag;
					//x }
					//x if(part.format.IsStrikeout()) {
					//x 	html << "<del>";
					//x 	endtag = "</del>" + endtag;
					//x }
					//x if(part.format.capitals) {
					//x 	html << "<span style=\"font-variant: small-caps;\">";
					//x 	endtag << "</span>";
					//x }
					bool spc = false;
					const wchar *end = part.text.End();
					const wchar *s = part.text.Begin();
					WString h;
					if(number.GetCount()) {
						if(*s == '\t')
							s++;
						h << number.ToWString() << ' ' << WString(s, end);
						s = h;
						end = h.end();
					}
					for(;s != end; s++) {
						if(*s == ' ') {
							html << " ";
						//x	html.Cat(spc ? "&nbsp;" : " ");
						//x	spc = true;
						}
						else {
							//xspc = false;
							//xif(*s == 160)  html.Cat("&nbsp;");
							//xelse
							//xif(*s == '<')  html.Cat("&lt;");
							//xelse
							//xif(*s == '>')  html.Cat("&gt;");
							//xelse
							//xif(*s == '&')  html.Cat("&amp;");
							//xelse
							//xif(*s == '\"') html.Cat("&quot;");
							//xelse
							//xif(*s == 9) {
							//x	if(bultext) {
							//x		if(!cs.IsEmpty() && part.text[0] != 9)
							//x			html << "</span>";
							//x		html << "</p>";
							//x		html << "</td>\r\n<td valign=\"top\" bgcolor=\"#F0F0F0\">\r\n";
							//x		html << par;
							//x		if(s[1]) {
							//x			cs = HtmlCharStyle(part.format, p.format);
							//x			if(!cs.IsEmpty())
							//x				html << "<span" << FormatClass(css, cs) << ">";
							//x		}
							//x	}
							//x	else
							//x		html.Cat("&nbsp;&nbsp;&nbsp;&nbsp;");
							//x}
							//xelse
								html.Cat(ToUtf8(*s));
						}
					}
					//xhtml << endtag;
				}
			}
			//xif(bultext)
			//x	html << "</td></tr></table>";
			html << "\r\n";
		}
	}
	return html;


}

String EncodeHtmlSimple2(const RichText& text,
                  const String& outdir, const String& namebase,
                  const VectorMap<String, String>& escape, int imt)
{
	RichPara::Number n;
	SimpleHtmlObjectSaver default_saver(outdir, namebase, imt);
	return AsHtmlSimple(text, text.GetStyles(), escape, default_saver, n);
}

String EncodeHtmlSimple(const RichText& text, const String& path, const String& base) {
	VectorMap<String, String> escape;
	return EncodeHtmlSimple2(text, path, base, escape, 0);
}


TwoFileSel::TwoFileSel() : FileSel()
{
}

}
