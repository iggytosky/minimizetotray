#ifndef PROC_STUBS_H_
#define PROC_STUBS_H_

#include "DebugLog.h"

static NPError StubDestroy(NPP instance, NPSavedData **save) 
{
	DebugLog(_T("StubDestroy\n"));

	return NPERR_NO_ERROR;
}

static NPError StubNewStream(NPP instance, NPMIMEType type, NPStream *stream, NPBool seekable, WORD *stype) 
{
	DebugLog(_T("StubNewStream\n"));
	
	return NPERR_NO_ERROR;
}

static NPError StubDestroyStream(NPP instance, NPStream *stream, NPReason reason) 
{
	DebugLog(_T("StubDestroyStream\n"));

	return NPERR_NO_ERROR;
}

static void StubStreamAsFile(NPP instance,
							 NPStream* stream,
							 const char *fname) 
{
	DebugLog(_T("StubStreamAsFile\n"));
}

static int32_t StubWriteReady(NPP instance, NPStream* stream)
{
	DebugLog(_T("StubWriteReady\n"));

	return 0;
}

static int32_t StubWrite(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer) 
{
	DebugLog(_T("StubWrite\n"));

	return -1;
}

static void StubPrint(NPP instance, NPPrint *PrintInfo) 
{
	DebugLog(_T("StubPrint\n"));
}

static short StubHandleEvent(NPP instance, void* event) 
{
	DebugLog(_T("StubHandleEvent\n"));

	return false;
}

static void StubURLNotify(NPP instance, const char *url, NPReason reason, void *notifyData) 
{
	DebugLog(_T("StubURLNotify\n"));
}

static NPError StubSetValue(NPP instance, NPNVariable variable, void *ret_alue) 
{
	DebugLog(_T("StubSetValue\n"));

	return NPERR_NO_ERROR;
}

static NPObject *StubAllocate(NPP npp, NPClass *clazz) 
{
	DebugLog(_T("StubAllocate\n"));

	NPObject *obj		= new NPObject();
	obj->_class			= clazz;
	obj->referenceCount = 0;

	return obj;
}

static void StubDeallocate(NPObject *npobj) 
{
	DebugLog(_T("StubDeallocate\n"));
}

static void StubInvalidate(NPObject *npobj) 
{
	DebugLog(_T("StubInvalidate\n"));
}

static bool StubInvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result) 
{
	DebugLog(_T("StubInvokeDefault\n"));

	return false;
}

static bool StubHasProperty(NPObject * npobj, NPIdentifier name) 
{
	DebugLog(_T("StubHasProperty\n"));

	return false;
}

static bool StubGetProperty(NPObject *npobj, NPIdentifier name,	NPVariant *result) 
{
	DebugLog(_T("StubGetValue\n"));

	return false;
}

static bool StubSetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value) 
{
	DebugLog(_T("StubSetProperty\n"));

	return false;
}

static bool StubRemoveProperty(NPObject *npobj, NPIdentifier name) 
{
	DebugLog(_T("StubRemoveProperty\n"));

	return false;
}

static bool StubEnumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count) 
{
	DebugLog(_T("StubEnumerate\n"));

	return false;
}

static bool StubConstruct(NPObject *npobj,
						  const NPVariant *args,
						  uint32_t argCount,
						  NPVariant *result) 
{
	DebugLog(_T("StubConstruct\n"));

	return false;
}

NPError StubNewInstance(NPMIMEType pluginType, NPP instance, WORD mode,	short argc,	char *argn[], char *argv[],	NPSavedData *saved)
{
	DebugLog(_T("StubNewInstance\n"));

	return NPERR_NO_ERROR;
}

NPError StubGetValue(NPP instance, NPPVariable variable, void *value) 
{
	DebugLog(_T("StubGetValue\n"));

	return NPERR_NO_ERROR;
}

NPError StubSetWindow(NPP instance, NPWindow *window) 
{
	DebugLog(_T("SetWindow: %lu"), (DWORD)window->window);

	return NPERR_NO_ERROR;
}

#endif //WEBDRIVER_STUBS_H