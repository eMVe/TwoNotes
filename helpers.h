#ifndef _HELPERS_H_
#define _HELPERS_H_

#include <RichEdit/RichEdit.h>

namespace Upp {

String EncodeHtmlSimple(const RichText& text,
                  const VectorMap<String, String>& labels,
                  const String& path, const String& base = Null, Zoom z = Zoom(8, 40),
                  const VectorMap<String, String>& escape = VectorMap<String, String>(),
                  int imtolerance = 0);

String EncodeHtmlSimple(const RichText& text, const String& path, const String& base = Null);


class SimpleHtmlObjectSaver : public HtmlObjectSaver
{
public:
	SimpleHtmlObjectSaver(const String& outdir_, const String& namebase_, int imtolerance_)
	: outdir(outdir_), namebase(namebase_), imtolerance(imtolerance_), im(0) {}

	virtual String GetHtml(const RichObject& object, const String& link);

private:
	String outdir;
	String namebase;
	int imtolerance;
	int im;
};


class TwoFileSel : public FileSel
{
public:
	TwoFileSel & InitialDir(const char *dir) { SetDir(dir); return *this; }
	TwoFileSel();
};


}

#endif
