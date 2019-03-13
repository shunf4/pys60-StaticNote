#ifndef OPENURL_H_
#define OPENURL_H_
#include <e32base.h>
class COpenUrl
	{
public:
	void Open(const TDesC& aurl);
	COpenUrl();
	~COpenUrl();
private:
	void CallSystembrowser(const TDesC& aUrl);
	TInt StartUCWEB( TUid& aUCWEBUid, const TDesC& aRequestUrl );
	};
#endif
