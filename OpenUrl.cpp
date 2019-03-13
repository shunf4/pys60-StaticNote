#include "OpenUrl.h"
#include <APGTASK.H>
#include <APGCLI.H>
#include <e32std.h>
#include <utf.h>
#include <eikenv.h>
COpenUrl::COpenUrl()
	{
	}
COpenUrl::~COpenUrl()
	{
	}
void COpenUrl::Open(const TDesC& aurl)
	{
	TInt sErrCode = KErrNone;
	CArrayFixFlat<TUid>* sUCWEBUidArray = new CArrayFixFlat<TUid>(6);
	sUCWEBUidArray->AppendL( TUid::Uid(0x2001f84c) );
	sUCWEBUidArray->AppendL( TUid::Uid(0x2001f84b) );
	sUCWEBUidArray->AppendL( TUid::Uid(0x2001f84a) );
	sUCWEBUidArray->AppendL( TUid::Uid(0x2001f848) );
	sUCWEBUidArray->AppendL( TUid::Uid(0xa000998f) );
	sUCWEBUidArray->AppendL( TUid::Uid(0xa000997f) );
	for ( TInt i = 0; i < sUCWEBUidArray->Count(); i++ ) 
		{
		sErrCode = StartUCWEB( (*sUCWEBUidArray)[i], aurl );
		if ( sErrCode == KErrNone )
			break;
		}
	delete sUCWEBUidArray;
	if ( sErrCode != KErrNone )
		{
		CallSystembrowser(aurl);
		}
	}
TInt COpenUrl::StartUCWEB( TUid& aUCWEBUid, const TDesC& aRequestUrl )
	{
	RApaLsSession sApplicationSession;
	TInt sErrorCode = KErrNone; 
    RWsSession sWindowSession;
    TThreadId sCurrentThreadId;
	sErrorCode = sApplicationSession.Connect();
	if ( sErrorCode != KErrNone )
		{
		return sErrorCode;
		}
	sErrorCode = sWindowSession.Connect();
	if ( sErrorCode != KErrNone )
		{
		sApplicationSession.Close();
		return sErrorCode;
		}
	TApaTaskList apataskList( sWindowSession );    
	TApaTask apatask = apataskList.FindApp( aUCWEBUid );
	if ( apatask.Exists() ) 
		{
        sErrorCode = apatask.SwitchOpenFile( aRequestUrl );
		if ( sErrorCode == KErrNone )
			{
			apatask.BringToForeground();
			}
		else
			{
			HBufC8* sRequestUrl = HBufC8::NewL( aRequestUrl.Length() * 4 );
			TPtr8 sRequestUrlPtr = sRequestUrl->Des();
			sErrorCode = CnvUtfConverter::ConvertFromUnicodeToUtf8( sRequestUrlPtr, aRequestUrl );
			if ( sErrorCode == KErrNone )
				{
				sRequestUrl = sRequestUrl->ReAllocL( sRequestUrl->Length() );
				}
			else
				{
				delete sRequestUrl;
				sRequestUrl = NULL;
				return sErrorCode;
				}
			RFile sFile;
			TFileName sFileName = _L("C:\\UCUrl.txt");
			sErrorCode = sFile.Replace( CEikonEnv::Static()->FsSession(), sFileName, EFileWrite|EFileStreamText );
			if ( sErrorCode != KErrNone )
				{
				return sErrorCode;
				}
			sErrorCode = sFile.Write( *sRequestUrl );
			if ( sErrorCode != KErrNone )
				{
				return sErrorCode;
				}
			sErrorCode = sFile.Flush();
			if ( sErrorCode != KErrNone )
				{
				return sErrorCode;
				}
			sFile.Close();
			apatask.BringToForeground();
			}
		sApplicationSession.Close();
		sWindowSession.Close();
		return sErrorCode;
		}
	sErrorCode = sApplicationSession.StartDocument( aRequestUrl, aUCWEBUid, sCurrentThreadId );
	sApplicationSession.Close();
	sWindowSession.Close();
	return sErrorCode;
	}
void COpenUrl::CallSystembrowser(const TDesC& aUrl)
	{
	const TInt KBrowserUid = 0x10008D39;
	TUid id( TUid::Uid( KBrowserUid ) );
	TApaTaskList taskList( CEikonEnv::Static()->WsSession() ) ;
	TApaTask task = taskList.FindApp( id ) ;
	RApaLsSession appArcSession;
	TThreadId thread_id = 0;
	TBuf<30> param;
	HBufC8 * param8 ; 
	param.Copy( _L( "4 " ) );
	param.Append(aUrl);
	if ( task.Exists() )
		{
		param8 = HBufC8::NewLC(param.Length());
		param8->Des().Append(param);
		task.SendMessage( TUid::Uid( 0 ),*param8 );
		CleanupStack::PopAndDestroy();
		}
	else
		{
		User::LeaveIfError(appArcSession.Connect());
		appArcSession.StartDocument( param, TUid::Uid( KBrowserUid ), thread_id );
		appArcSession.Close();
		}
	}
