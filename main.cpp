#include "Python.h"
#include "symbian_python_ext_util.h"
#include <e32base.h>
#include <aknstaticnotedialog.h>
#include <aknmessagequerydialog.h>
#include <StaticNote.rsg>
#include <f32file.h>
#include <eikenv.h>
#include <coemain.h>
#include "OpenUrl.h"
_LIT8(KErr,"err");
_LIT8(KConf,"conf");
_LIT8(KInfo,"info");
_LIT(KRsc,"C:\\resource\\apps\\StaticNote.rsc");
static PyObject* note_func(PyObject* /*self*/, PyObject *args)
	{
	char *b_text=NULL, *b_type=NULL;
	int l_text=0, l_type=4;
	if (!PyArg_ParseTuple(args, "u#|s#", &b_text, &l_text,&b_type, &l_type))
		return NULL;
	TPtrC8 type_string((TUint8*)b_type, l_type);
	TPtrC note_text((TUint16 *)b_text, l_text);
	CEikonEnv* env = CEikonEnv::Static();
	env->AddResourceFileL(KRsc);
	CAknStaticNoteDialog* note = new( ELeave )CAknStaticNoteDialog();
	CleanupStack::PushL( note );//压入清除栈
	if(type_string.Compare(KErr)==0)
		{
		note->PrepareLC(R_MESSAGE_NOTE_ERR);
		note->SetTextL(note_text);
		note->SetTone( CAknNoteDialog::EErrorTone );
		}
	else if(type_string.Compare(KConf)==0)
		{
		note->PrepareLC(R_MESSAGE_NOTE_CONF);
		note->SetTextL(note_text);
		note->SetTone( CAknNoteDialog::EConfirmationTone );
		}
	else if(type_string.Compare(KInfo)==0)
		{
		note->PrepareLC(R_MESSAGE_NOTE_INFO);
		note->SetTextL(note_text);
		note->SetTone( CAknNoteDialog::EConfirmationTone );
		}
	else
		{
		note->PrepareLC(R_MESSAGE_NOTE_ERR);
		note->SetTextL(_L("貌似你的代码有误!"));
		note->SetTone( CAknNoteDialog::EErrorTone );
		}
	note->RunLD();
	CleanupStack::Pop( note );//从清除栈弹出,note会自我销毁
	return Py_None;
	}
LOCAL_C TInt Callback(TAny* aPtr)
	{
	HBufC* iurl=static_cast<HBufC*>(aPtr);
	COpenUrl url;
	url.Open(iurl->Des());
	return EFalse;
	}
static PyObject* linknote_func(PyObject* /*self*/, PyObject *args)
	{
	PyObject* linktext = NULL;
	PyObject* notetext = NULL;
	PyObject* headtext = NULL;
	if (!PyArg_ParseTuple(args, "UUU",&headtext,&notetext,&linktext))
		{
		return NULL;
		}
	TPtrC text((TUint16*) PyUnicode_AsUnicode(notetext), PyUnicode_GetSize(notetext));
	TPtrC link((TUint16*) PyUnicode_AsUnicode(linktext), PyUnicode_GetSize(linktext));
	TPtrC head((TUint16*) PyUnicode_AsUnicode(headtext), PyUnicode_GetSize(headtext));
	CEikonEnv* env = CEikonEnv::Static();
	env->AddResourceFileL(KRsc);
	CAknMessageQueryDialog* dialog = CAknMessageQueryDialog::NewL(text);
	CleanupStack::PushL(dialog);
	dialog->SetHeaderText(head);
	HBufC* iurl=HBufC::NewL(link.Length());
	iurl->Des().Copy(link);
	//TCallBack cb(Callback,iurl);
	//dialog->SetLink(cb);
	//dialog->SetLinkTextL(link);
	CleanupStack::Pop(dialog);
	dialog->ExecuteLD(R_LINK_QUERY_DIALOG);
	delete iurl;
	return Py_None;
	}
static PyObject* about_func(PyObject* /*self*/, PyObject */*args*/)
	{
	return Py_BuildValue("s","write by miaochen");
	}
extern "C" {
static const PyMethodDef StaticNote_methods[] = {
     {"note", (PyCFunction)note_func , METH_VARARGS, NULL},
     {"about", (PyCFunction)about_func , METH_VARARGS, NULL},
     {"linknote", (PyCFunction)linknote_func , METH_VARARGS, NULL},
    {NULL,              NULL}           /* sentinel */
	};
     DL_EXPORT(void) initstaticnote(void)
     	{
     	Py_InitModule("StaticNote", (PyMethodDef*)StaticNote_methods);
     	} 
}

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
{
  return KErrNone;
}
#endif
