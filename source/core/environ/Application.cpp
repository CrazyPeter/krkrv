#if 0
#define INITGUID // XP 有効にするとdxguid.libが使用できないため
#endif
#include "tjsCommHead.h"

#include <algorithm>
#include <string>
#include <vector>
#include <assert.h>

#if 0
#include <eh.h>

#define DIRECTINPUT_VERSION 0x0500
#include <dinput.h>

#include <DbgHelp.h>
#include <Strsafe.h>
#endif
#include "tjsError.h"
#include "tjsDebug.h"

#include "Application.h"
#include "SysInitIntf.h"
#include "SysInitImpl.h"
#include "DebugIntf.h"
#include "MsgIntf.h"
#include "ScriptMgnIntf.h"
#include "tjsError.h"
#include "PluginImpl.h"
#include "SystemIntf.h"

#include "Exception.h"
#if 0
#include "WindowFormUnit.h"
#include "Resource.h"
#endif
#include "SystemControl.h"
#if 0
#include "MouseCursor.h"
#endif
#include "SystemImpl.h"
#include "WaveImpl.h"
#include "GraphicsLoadThread.h"
#include "CharacterSet.h"
#include "Platform.h"
#include "EventIntf.h"
#include "StorageIntf.h"
#include "TVPColor.h"
#include <unistd.h>

#if 0
#include "resource.h"
#endif

#ifdef _MSC_VER
#pragma comment(lib,"dbghelp.lib")
/*
kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;winmm.lib;dsound.lib;version.lib;mpr.lib;shlwapi.lib;vfw32.lib;imm32.lib;zlib_d.lib;jpeg-6bx_d.lib;libpng_d.lib;onig_s_d.lib;freetype250MT_D.lib;tvpgl_ia32.lib;tvpsnd_ia32.lib;%(AdditionalDependencies)
kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;winmm.lib;dsound.lib;version.lib;mpr.lib;shlwapi.lib;vfw32.lib;imm32.lib;zlib.lib;jpeg-6bx.lib;libpng.lib;onig_s.lib;freetype250MT.lib;tvpgl_ia32.lib;tvpsnd_ia32.lib;%(AdditionalDependencies)
*/
#endif

tTVPApplication* Application;
tTJS *TVPAppScriptEngine;

#if 0
#ifdef TJS_64BIT_OS
extern void TVPHandleSEHException( int ErrorCode, EXCEPTION_RECORD *P, unsigned long long osEsp, PCONTEXT ctx);
#else
extern void TVPHandleSEHException( int ErrorCode, EXCEPTION_RECORD *P, unsigned long osEsp, PCONTEXT ctx);
#endif

// アプリケーションの開始時に呼ぶ
inline void CheckMemoryLeaksStart()
{
#ifdef  _DEBUG
   _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif  // _DEBUG
}

inline void DumpMemoryLeaks()
{
#ifdef  _DEBUG
	int is_leak = _CrtDumpMemoryLeaks();
	assert( !is_leak );
#endif  // _DEBUG
}
#endif

tjs_string ExePath() {
	static tjs_string exepath(TJS_W(""));
	if (exepath.empty()) {
		exepath = tjs_string(TJS_W("app0:/eboot.bin"));
	}
	return exepath;
}

bool TVPCheckPrintDataPath();
void TVPOnError();
void TVPLockSoundMixer();
void TVPUnlockSoundMixer();

int _argc;
tjs_char ** _wargv;
#if 0
extern void TVPInitCompatibleNativeFunctions();

AcceleratorKeyTable::AcceleratorKeyTable() {
	// デフォルトを読み込む
	hAccel_ = ::LoadAccelerators( (HINSTANCE)GetModuleHandle(0), MAKEINTRESOURCE(IDC_TVPWIN32));
}
AcceleratorKeyTable::~AcceleratorKeyTable() {
	std::map<HWND,AcceleratorKey*>::iterator i = keys_.begin();
	for( ; i != keys_.end(); i++ ) {
		delete (i->second);
	}
}
void AcceleratorKeyTable::AddKey( HWND hWnd, WORD id, WORD key, BYTE virt ) {
	std::map<HWND,AcceleratorKey*>::iterator i = keys_.find(hWnd);
	if( i != keys_.end() ) {
		i->second->AddKey(id,key,virt);
	} else {
		AcceleratorKey* acc = new AcceleratorKey();
		acc->AddKey( id, key, virt );
		keys_.insert( std::map<HWND, AcceleratorKey*>::value_type( hWnd, acc ) );
	}
}
void AcceleratorKeyTable::DelKey( HWND hWnd, WORD id ) {
	std::map<HWND,AcceleratorKey*>::iterator i = keys_.find(hWnd);
	if( i != keys_.end() ) {
		i->second->DelKey(id);
	}
}

void AcceleratorKeyTable::DelTable( HWND hWnd ) {
	std::map<HWND,AcceleratorKey*>::iterator i = keys_.find(hWnd);
	if( i != keys_.end() ) {
		delete (i->second);
		keys_.erase(i);
	}
}
AcceleratorKey::AcceleratorKey() : hAccel_(NULL), keys_(NULL), key_count_(0) {
}
AcceleratorKey::~AcceleratorKey() {
	if( hAccel_ != NULL ) ::DestroyAcceleratorTable( hAccel_ );
	delete[] keys_;
}
void AcceleratorKey::AddKey( WORD id, WORD key, BYTE virt ) {
	// まずは存在するかチェックする
	bool found = false;
	int index = 0;
	for( int i = 0; i < key_count_; i++ ) {
		if( keys_[i].cmd == id ) {
			index = i;
			found = true;
			break;
		}
	}
	if( found ) {
		// 既に登録されているコマンドなのでキー情報の更新を行う
		if( keys_[index].key == key && keys_[index].fVirt == virt ) {
			// 変更されていない
			return;
		}
		keys_[index].key = key;
		keys_[index].fVirt = virt;
		HACCEL hAccel = ::CreateAcceleratorTable( keys_, key_count_ );
		if( hAccel_ != NULL ) ::DestroyAcceleratorTable( hAccel_ );
		hAccel_ = hAccel;
	} else {
		ACCEL* table = new ACCEL[key_count_+1];
		for( int i = 0; i < key_count_; i++ ) {
			table[i] = keys_[i];
		}
		table[key_count_].cmd = id;
		table[key_count_].key = key;
		table[key_count_].fVirt = virt;
		key_count_++;
		HACCEL hAccel = ::CreateAcceleratorTable( table, key_count_ );
		if( hAccel_ != NULL ) ::DestroyAcceleratorTable( hAccel_ );
		hAccel_ = hAccel;
		delete[] keys_;
		keys_ = table;
	}

}
void AcceleratorKey::DelKey( WORD id ) {
	// まずは存在するかチェックする
	bool found = false;
	for( int i = 0; i < key_count_; i++ ) {
		if( keys_[i].cmd == id ) {
			found = true;
			break;
		}
	}
	if( found == false ) return;

	// 存在した場合作り直し
	ACCEL* table = new ACCEL[key_count_-1];
	int dest = 0;
	for( int i = 0; i < key_count_; i++ ) {
		if( keys_[i].cmd != id ) {
			table[dest] = keys_[i];
			dest++;
		}
	}
	key_count_--;
	HACCEL hAccel = ::CreateAcceleratorTable( table, key_count_ );
	if( hAccel_ != NULL ) ::DestroyAcceleratorTable( hAccel_ );
	hAccel_ = hAccel;
	delete[] keys_;
	keys_ = table;
}

int APIENTRY wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow ) {
	try {
		CheckMemoryLeaksStart();
		// ウォッチで _crtBreakAlloc にセットする

		// XP より後で使えるAPIを動的に読み込んで互換性を取る
		TVPInitCompatibleNativeFunctions();

		_argc = __argc;
		_wargv = __wargv;

		MouseCursor::Initialize();
		Application = new tTVPApplication();
		Application->StartApplication( __argc, __argv );
	
		// delete application and exit forcely
		// this prevents ugly exception message on exit
		// アプリケーションを削除し強制終了させる。
		// これは終了時の醜い例外メッセージを抑止する
		delete Application;

#ifndef _DEBUG
//		::ExitProcess(TVPTerminateCode); // ここで終了させるとメモリリーク表示が行われない
#endif
	} catch (...) {
		return 2;
	}
	return TVPTerminateCode;
}
#endif
tTVPApplication::tTVPApplication() : is_attach_console_(false), tarminate_(false), application_activating_(true)
	 , image_load_thread_(NULL), has_map_report_process_(false)
{
}
tTVPApplication::~tTVPApplication() {
#if 0
	while( windows_list_.size() ) {
		std::vector<TTVPWindowForm*>::iterator i = windows_list_.begin();
		delete (*i);
		// TTVPWindowForm のデストラクタ内でリストから削除されるはず
	}
	windows_list_.clear();
#endif
	delete image_load_thread_;
}
#if 0
struct SEHException {
	unsigned int Code;
	_EXCEPTION_POINTERS* ExceptionPointers;
	SEHException( unsigned int code, _EXCEPTION_POINTERS* ep )
		: Code(code), ExceptionPointers(ep)
	{}
};

int TVPWriteHWEDumpFile( EXCEPTION_POINTERS* pExceptionPointers ) {
	BOOL bMiniDumpSuccessful;
	tjs_char szPath[MAX_PATH]; 
	tjs_char szFileName[MAX_PATH]; 
	const tjs_char* szAppName = TVPKirikiri;
	const tjs_char* szVersion = TVPGetVersionString().c_str();

	TVPEnsureDataPathDirectory();
	TJS_strcpy(szPath, TVPNativeDataPath.c_str());

	SYSTEMTIME stLocalTime;
	::GetLocalTime( &stLocalTime );
	StringCchPrintf( szFileName, MAX_PATH, TJS_W("%s%s%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp"),
				szPath, szAppName, szVersion,
				stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
				stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
				GetCurrentProcessId(), GetCurrentThreadId());
	HANDLE hDumpFile = ::CreateFile(szFileName, GENERIC_READ|GENERIC_WRITE,
				FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

	MINIDUMP_EXCEPTION_INFORMATION ExpParam;
	ExpParam.ThreadId = ::GetCurrentThreadId();
	ExpParam.ExceptionPointers = pExceptionPointers;
	ExpParam.ClientPointers = TRUE;
	bMiniDumpSuccessful = MiniDumpWriteDump( ::GetCurrentProcess(), ::GetCurrentProcessId(), hDumpFile, MiniDumpWithDataSegs, &ExpParam, NULL, NULL);
	return EXCEPTION_EXECUTE_HANDLER;
}
static bool TVPIsHandledHWException = false;
void se_translator_function(unsigned int code, struct _EXCEPTION_POINTERS* ep) {
	if( !TVPIsHandledHWException ) {
		//ShowStackTrace( ep->ContextRecord );
		TVPWriteHWEDumpFile( ep );
#ifdef TJS_64BIT_OS
		TVPHandleSEHException( code, ep->ExceptionRecord, ep->ContextRecord->Rsp, ep->ContextRecord );
#else
		TVPHandleSEHException( code, ep->ExceptionRecord, ep->ContextRecord->Esp, ep->ContextRecord );
#endif
		TVPIsHandledHWException = true;
	}
	throw SEHException(code,ep);
}
const tjs_char* SECodeToMessage( unsigned int code ) {
	switch(code){
	case EXCEPTION_ACCESS_VIOLATION: return TVPExceptionAccessViolation;
	case EXCEPTION_BREAKPOINT: return TVPExceptionBreakpoint;
	case EXCEPTION_DATATYPE_MISALIGNMENT: return TVPExceptionDatatypeMisalignment;
	case EXCEPTION_SINGLE_STEP: return TVPExceptionSingleStep;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return TVPExceptionArrayBoundsExceeded;
	case EXCEPTION_FLT_DENORMAL_OPERAND: return TVPExceptionFltDenormalOperand;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO: return TVPExceptionFltDivideByZero;
	case EXCEPTION_FLT_INEXACT_RESULT: return TVPExceptionFltInexactResult;
	case EXCEPTION_FLT_INVALID_OPERATION: return TVPExceptionFltInvalidOperation;
	case EXCEPTION_FLT_OVERFLOW: return TVPExceptionFltOverflow;
	case EXCEPTION_FLT_STACK_CHECK: return TVPExceptionFltStackCheck;
	case EXCEPTION_FLT_UNDERFLOW: return TVPExceptionFltUnderflow;
	case EXCEPTION_INT_DIVIDE_BY_ZERO: return TVPExceptionIntDivideByZero;
	case EXCEPTION_INT_OVERFLOW: return TVPExceptionIntOverflow;
	case EXCEPTION_PRIV_INSTRUCTION: return TVPExceptionPrivInstruction;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: return TVPExceptionNoncontinuableException;
	case EXCEPTION_GUARD_PAGE: return TVPExceptionGuardPage;
	case EXCEPTION_ILLEGAL_INSTRUCTION: return TVPExceptionIllegalInstruction;
	case EXCEPTION_IN_PAGE_ERROR: return TVPExceptionInPageError;
	case EXCEPTION_INVALID_DISPOSITION: return TVPExceptionInvalidDisposition;
	case EXCEPTION_INVALID_HANDLE: return TVPExceptionInvalidHandle;
	case EXCEPTION_STACK_OVERFLOW: return TVPExceptionStackOverflow;
	case STATUS_UNWIND_CONSOLIDATE: return TVPExceptionUnwindCconsolidate;
	}
	return TJS_W("Unknown");
}

#endif
extern void TVPLoadPluigins(void);
bool tTVPApplication::StartApplication( int argc, tjs_char* argv[] ) {
	//	_set_se_translator(se_translator_function);

	ArgC = argc;
	ArgV = argv;
#if 0
	for( int i = 0; i < argc; i++ ) {
		if(!TJS_strcmp(argv[i], TJS_W("-@processohmlog"))) {
			has_map_report_process_ = true;
		}
	}
#endif
	TVPTerminateCode = 0;
	m_msgQueueLock = SDL_CreateMutex();

	CheckConsole();

	// try starting the program!
	bool engine_init = false;
	try {
#if 0
		if(TVPCheckProcessLog()) return true; // sub-process for processing object hash map log
#endif

		TVPInitScriptEngine();
		engine_init = true;

		// banner
		TVPAddImportantLog( TVPFormatMessage(TVPProgramStartedOn, TVPGetOSName(), TVPGetPlatformName()) );

		// TVPInitializeBaseSystems
		TVPInitializeBaseSystems();

		Initialize();

		if(TVPCheckPrintDataPath()) return true;
		
		image_load_thread_ = new tTVPAsyncImageLoader();

		TVPSystemInit();

		SetTitle( tjs_string(TVPKirikiri) );
		TVPSystemControl = new tTVPSystemControl();
#ifndef TVP_IGNORE_LOAD_TPM_PLUGIN
		TVPLoadPluigins(); // load plugin module *.tpm
#endif
		// Check digitizer
		CheckDigitizer();

		// start image load thread
		image_load_thread_->StartTread();

		if(TVPProjectDirSelected) TVPInitializeStartupScript();

#if 0
		Run();
#endif

#if 0
		try {
			// image_load_thread_->ExitRequest();
			delete image_load_thread_;
			image_load_thread_ = NULL;
		} catch(...) {
			// ignore errors
		}
		try {
			TVPSystemUninit();
		} catch(...) {
			// ignore errors
		}
#endif
	} catch( const EAbort & ) {
		// nothing to do
#if !(defined(_MSC_VER) && defined(_DEBUG))
	} catch( const Exception &exception ) {
		TVPOnError();
		if(!TVPSystemUninitCalled)
			ShowException(exception.what());
	} catch( const TJS::eTJSScriptError &e ) {
		TVPOnError();
		if(!TVPSystemUninitCalled)
			ShowException( e.GetMessage().c_str() );
	} catch( const TJS::eTJS &e) {
		TVPOnError();
		if(!TVPSystemUninitCalled)
			ShowException( e.GetMessage().c_str() );
	} catch( const std::exception &e ) {
		ShowException( ttstr(e.what()).c_str() );
	} catch( const char* e ) {
		ShowException( ttstr(e).c_str() );
	} catch( const tjs_char* e ) {
		ShowException( e );
#if 0
	} catch( const SEHException& e ) {
		PEXCEPTION_RECORD rec = e.ExceptionPointers->ExceptionRecord;
		tjs_string text(SECodeToMessage(e.Code));
		ttstr result = TJSGetStackTraceString( 10 );
		PrintConsole( result.c_str(), result.length(), true );

		TVPDumpHWException();
		ShowException( text.c_str() );
#endif
	} catch(...) {
		ShowException( (const tjs_char*)TVPUnknownError );
#endif
	}

#if 0
	if(engine_init) TVPUninitScriptEngine();

	if(TVPSystemControl) delete TVPSystemControl;
	TVPSystemControl = NULL;

	CloseConsole();
#endif

	return false;
}
/**
 * コンソールからの起動か確認し、コンソールからの起動の場合は、標準出力を割り当てる
 */
void tTVPApplication::CheckConsole() {
#if 0
#ifdef TVP_LOG_TO_COMMANDLINE_CONSOLE
	if( has_map_report_process_ ) return; // 書き出し用子プロセスして起動されていた時はコンソール接続しない
	HANDLE hin  = ::GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hout = ::GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE herr = ::GetStdHandle(STD_ERROR_HANDLE);

	DWORD curProcId = ::GetCurrentProcessId();
	DWORD processList[256];
	DWORD count = ::GetConsoleProcessList( processList, 256 );
	bool thisProcHasConsole = false;
	for( DWORD i = 0; i < count; i++ ) {
		if( processList[i] == curProcId ) {
			thisProcHasConsole = true;
			break;
		}
	}
	bool attachedConsole = true;
	if( thisProcHasConsole == false ) {
		attachedConsole = ::AttachConsole(ATTACH_PARENT_PROCESS) != 0;
	}

	if( (hin==0||hout==0||herr==0) && attachedConsole ) {
		tjs_char console[256];
		::GetConsoleTitle( console, 256 );
		console_title_ = tjs_string( console );
		// 元のハンドルを再割り当て
		if (hin)  ::SetStdHandle(STD_INPUT_HANDLE, hin);
		if (hout) ::SetStdHandle(STD_OUTPUT_HANDLE, hout);
		if (herr) ::SetStdHandle(STD_ERROR_HANDLE, herr);
	}
	is_attach_console_ = attachedConsole;
#endif
#endif
}

void tTVPApplication::CloseConsole() {
#if 0
	tjs_char buf[100];
	DWORD len = TJS_snprintf(buf, 100, TVPExitCode, TVPTerminateCode);
	PrintConsole(buf, len);
	if( is_attach_console_ ) {
		::SetConsoleTitle( console_title_.c_str() );
		::FreeConsole();
		is_attach_console_ = false;
	}
#endif
}
void TVPConsoleLog(const ttstr &mes, bool important);
void tTVPApplication::PrintConsole(const ttstr &mes, bool important) {
	TVPConsoleLog(mes, important);
}
#if 0
HWND tTVPApplication::GetHandle() {
	if( windows_list_.size() > 0 ) {
		return windows_list_[0]->GetHandle();
	} else {
		return INVALID_HANDLE_VALUE;
	}
}
void tTVPApplication::Minimize() {
	size_t size = windows_list_.size();
	for( size_t i = 0; i < size; i++ ) {
		if( windows_list_[i]->GetVisible() ) {
			::ShowWindow( windows_list_[i]->GetHandle(), SW_MINIMIZE );
		}
	}
}
void tTVPApplication::Restore() {
	size_t size = windows_list_.size();
	for( size_t i = 0; i < size; i++ ) {
		if( windows_list_[i]->GetVisible() ) {
			::ShowWindow( windows_list_[i]->GetHandle(), SW_RESTORE );
		}
	}
}

void tTVPApplication::BringToFront() {
	size_t size = windows_list_.size();
	for( size_t i = 0; i < size; i++ ) {
		windows_list_[i]->BringToFront();
	}
}
#endif
void tTVPApplication::ShowException( const tjs_char* e ) {
	TVPAddLog(ttstr(TVPScriptExceptionRaised) + TJS_W("\n") + e);
	TVPExitApplication(0);
}
void tTVPApplication::Run() {
	try {
		if (TVPTerminated) {
			TVPSystemUninit();
			TVPExitApplication(TVPTerminateCode);
		}
		ProcessMessages();
		if (TVPSystemControl) TVPSystemControl->SystemWatchTimerTimer();
	} catch (const EAbort &) {
		// nothing to do
#if !(defined(_MSC_VER) && defined(_DEBUG))
	} catch (const Exception &exception) {
		TVPOnError();
		if(!TVPSystemUninitCalled)
			ShowException(exception.what());
	} catch( const TJS::eTJSScriptError &e ) {
		TVPOnError();
		if(!TVPSystemUninitCalled)
			ShowException( e.GetMessage().c_str() );
	} catch( const TJS::eTJS &e) {
		TVPOnError();
		if(!TVPSystemUninitCalled)
			ShowException( e.GetMessage().c_str() );
	} catch( const std::exception &e ) {
		ShowException( ttstr(e.what()).c_str() );
	} catch( const char* e ) {
		ShowException( ttstr(e).c_str() );
	} catch( const tjs_char* e ) {
		ShowException( e );
	} catch (...) {
		ShowException((const tjs_char*)TVPUnknownError);
#endif
	}
}

void tTVPApplication::ProcessMessages()
{
	std::vector<std::tuple<void*, int, tMsg> > lstUserMsg;
	if (SDL_LockMutex(m_msgQueueLock) == 0) {
		m_lstUserMsg.swap(lstUserMsg);
		SDL_UnlockMutex(m_msgQueueLock);
	}
	for (std::tuple<void*, int, tMsg>& it : lstUserMsg) {
		std::get<2>(it)();
	}
	// TVPTimer::ProgressAllTimer();
}

#if 0
bool tTVPApplication::ProcessMessage( MSG &msg ) {
	bool result = false;
	if( ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE) ) {
		BOOL msgExists = ::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE);
		if( msgExists == 0 ) {
			return result;
		}
		result = true;
		if( msg.message != WM_QUIT ) {
			HACCEL hAccelTable = accel_key_.GetHandle(msg.hwnd);
			if( !TranslateAccelerator(msg.hwnd, hAccelTable, &msg) ) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else {
			TVPTerminateCode = (int)msg.wParam;
			tarminate_ = true;
		}
	}
	return result;
}
void tTVPApplication::ProcessMessages() {
	MSG msg = {0};
	while(ProcessMessage(msg));
}
void tTVPApplication::HandleMessage() {
	MSG msg = {0};
	if( !ProcessMessage(msg) ) {
		HandleIdle(msg);
	}
}
void tTVPApplication::HandleIdle(MSG &) {
	bool done = true;
	if( TVPSystemControl ) {
		done = TVPSystemControl->ApplicationIdle();
	}
	if( done ) ::WaitMessage();
}
#endif
void tTVPApplication::SetTitle( const tjs_string& caption ) {
	title_ = caption;
#if 0
	if( windows_list_.size() > 0 ) {
		windows_list_[0]->SetCaption( caption );
	}
	if( is_attach_console_ ) {
		::SetConsoleTitle( caption.c_str() );
	}
#endif
}

void tTVPApplication::Terminate()
{
	//::PostQuitMessage(0);
	tarminate_ = true;
	TVPTerminated = true;
}
#if 0
HWND tTVPApplication::GetMainWindowHandle() const {
	if( windows_list_.size() > 0 ) {
		return windows_list_[0]->GetHandle();
	}
	return INVALID_HANDLE_VALUE;
}

void tTVPApplication::RemoveWindow( TTVPWindowForm* win ) {
	std::vector<class TTVPWindowForm*>::iterator it = std::remove( windows_list_.begin(), windows_list_.end(), win );
	if( it != windows_list_.end() ) {
		windows_list_.erase( it, windows_list_.end() );
	}
}

void tTVPApplication::PostMessageToMainWindow(UINT message, WPARAM wParam, LPARAM lParam) {
	if( windows_list_.size() > 0 ) {
		::PostMessage( windows_list_[0]->GetHandle(), message, wParam, lParam );
	}
}
void tTVPApplication::GetDisableWindowList( std::vector<class TTVPWindowForm*>& win ) {
	size_t count = windows_list_.size();
	for( size_t i = 0; i < count; i++ ) {
		if( windows_list_[i]->GetEnable() == false ) {
			win.push_back( windows_list_[i] );
		}
	}
}
void tTVPApplication::GetEnableWindowList( std::vector<class TTVPWindowForm*>& win, class TTVPWindowForm* activeWindow ) {
	size_t count = windows_list_.size();
	for( size_t i = 0; i < count; i++ ) {
		if( activeWindow != windows_list_[i] && windows_list_[i]->GetEnable() ) {
			win.push_back( windows_list_[i] );
		}
	}
}

void tTVPApplication::DisableWindows() {
	size_t count = windows_list_.size();
	for( size_t i = 0; i < count; i++ ) {
		windows_list_[i]->SetEnable( false );
	}
}
void tTVPApplication::EnableWindows( const std::vector<TTVPWindowForm*>& win ) {
	size_t count = win.size();
	for( size_t i = 0; i < count; i++ ) {
		win[i]->SetEnable( true );
	}
	/*
	size_t count = windows_list_.size();
	for( size_t i = 0; i < count; i++ ) {
		TTVPWindowForm* win = windows_list_[i];
		std::vector<TTVPWindowForm*>::const_iterator f = std::find( ignores.begin(), ignores.end(), win );
		if( f == ignores.end() ) {
			windows_list_[i]->SetEnable( true );
		}
	}
	*/
}
void tTVPApplication::FreeDirectInputDeviceForWindows() {
	size_t count = windows_list_.size();
	for( size_t i = 0; i < count; i++ ) {
		windows_list_[i]->FreeDirectInputDevice();
	}
}


void tTVPApplication::RegisterAcceleratorKey(HWND hWnd, char virt, short key, short cmd) {
	accel_key_.AddKey( hWnd, cmd, key, virt );
}
void tTVPApplication::UnregisterAcceleratorKey(HWND hWnd, short cmd) {
	accel_key_.DelKey( hWnd, cmd );
}
void tTVPApplication::DeleteAcceleratorKeyTable( HWND hWnd ) {
	accel_key_.DelTable( hWnd );
}
#endif
void tTVPApplication::CheckDigitizer() {
	// Windows 7 以降でのみ有効
#if 0
	int value = ::GetSystemMetrics(SM_DIGITIZER);
	if( value == 0 ) return;

	TVPAddLog( (const tjs_char*)TVPEnableDigitizer );
	if( value & NID_INTEGRATED_TOUCH ) {
		TVPAddLog( (const tjs_char*)TVPTouchIntegratedTouch );
	}
	if( value & NID_EXTERNAL_TOUCH ) {
		TVPAddLog( (const tjs_char*)TVPTouchExternalTouch );
	}
	if( value & NID_INTEGRATED_PEN ) {
		TVPAddLog( (const tjs_char*)TVPTouchIntegratedPen );
	}
	if( value & NID_EXTERNAL_PEN ) {
		TVPAddLog( (const tjs_char*)TVPTouchExternalPen );
	}
	if( value & NID_MULTI_INPUT ) {
		TVPAddLog( (const tjs_char*)TVPTouchMultiInput );
	}
	if( value & NID_READY ) {
		TVPAddLog( (const tjs_char*)TVPTouchReady );
	}
#endif
}

void tTVPApplication::PostUserMessage(const std::function<void()> &func, void* host, int msg)
{
	if (SDL_LockMutex(m_msgQueueLock) == 0) {
		m_lstUserMsg.emplace_back(host, msg, func);
		SDL_UnlockMutex(m_msgQueueLock);
	}
}

void tTVPApplication::FilterUserMessage(const std::function<void(std::vector<std::tuple<void*, int, tMsg> > &)> &func)
{
	if (SDL_LockMutex(m_msgQueueLock) == 0) {
		func(m_lstUserMsg);
		SDL_UnlockMutex(m_msgQueueLock);
	}
}

void tTVPApplication::OnActivate()
{
	application_activating_ = true;

#if 0
	TVPRestoreFullScreenWindowAtActivation();
#endif
	TVPResetVolumeToAllSoundBuffer();
	TVPUnlockSoundMixer();

	// trigger System.onActivate event
	TVPPostApplicationActivateEvent();
	for (auto & it : m_activeEvents) {
		it.second(it.first, eTVPActiveEvent::onActive);
	}
}
void tTVPApplication::OnDeactivate(  )
{
	application_activating_ = false;

#if 0
	TVPMinimizeFullScreenWindowAtInactivation();
#endif
	
	// fire compact event
	TVPDeliverCompactEvent(TVP_COMPACT_LEVEL_DEACTIVATE);

	// set sound volume
	TVPResetVolumeToAllSoundBuffer();
	TVPLockSoundMixer();

	// trigger System.onDeactivate event
	TVPPostApplicationDeactivateEvent();
	for (auto & it : m_activeEvents) {
		it.second(it.first, eTVPActiveEvent::onDeactive);
	}
}

void tTVPApplication::OnExit()
{
	TVPUninitScriptEngine();

	if (TVPSystemControl) delete TVPSystemControl;
	TVPSystemControl = NULL;

	CloseConsole();
}

bool tTVPApplication::GetNotMinimizing() const
{
	return !application_activating_;
#if 0
	HWND hWnd = GetMainWindowHandle();
	if( hWnd != INVALID_HANDLE_VALUE && hWnd != NULL ) {
		return ::IsIconic( hWnd ) == 0;
	}
	return true; // メインがない時は最小化されているとみなす
#endif
}

#if 0
void tTVPApplication::OnActiveAnyWindow() {
	if( modal_window_stack_.empty() != true ) {
		tTVPWindow* win = modal_window_stack_.top();
		if( win->GetVisible() && win->GetEnable() ) {
			win->BringToFront();
		}
	}
}
void tTVPApplication::ModalFinished() {
	modal_window_stack_.pop();
	if( modal_window_stack_.empty() != true ) {
		tTVPWindow* win = modal_window_stack_.top();
		if( win->GetVisible() && win->GetEnable() ) {
			win->BringToFront();
		}
	}
}
#endif
void tTVPApplication::LoadImageRequest( class iTJSDispatch2 *owner, class tTJSNI_Bitmap* bmp, const ttstr &name ) {
	if( image_load_thread_ ) {
		image_load_thread_->LoadRequest( owner, bmp, name );
	}
}

void tTVPApplication::RegisterActiveEvent(void *host, const std::function<void(void*, eTVPActiveEvent)>& func/*empty = unregister*/)
{
	if (func)
		m_activeEvents.emplace(host, func);
	else
		m_activeEvents.erase(host);
}

#if 0
std::vector<std::string>* LoadLinesFromFile( const tjs_string& path ) {
	FILE *fp = NULL;
	_wfopen_s( &fp, path.c_str(), TJS_W("r"));
    if( fp == NULL ) {
		return NULL;
    }
	char buff[1024];
	std::vector<std::string>* ret = new std::vector<std::string>();
    while( fgets(buff, 1024, fp) != NULL ) {
		ret->push_back( std::string(buff) );
    }
    fclose(fp);
	return ret;
}

void TVPRegisterAcceleratorKey(HWND hWnd, char virt, short key, short cmd) {
	if( Application ) Application->RegisterAcceleratorKey( hWnd, virt, key, cmd );
}
void TVPUnregisterAcceleratorKey(HWND hWnd, short cmd) {
	if( Application ) Application->UnregisterAcceleratorKey( hWnd, cmd );
}
void TVPDeleteAcceleratorKeyTable( HWND hWnd ) {
	if( Application ) Application->DeleteAcceleratorKeyTable( hWnd );
}
#endif

void TVPInitWindowOptions() {}

unsigned long ColorToRGB(unsigned int col)
{
	// 0xBBGGRR
	switch (col) {
	case clScrollBar:
		return 0xc8c8c8;
	case clBackground:
		return 0;
	case clActiveCaption:
		return 0xd1b499;
	case clInactiveCaption:
		return 0xdbcdbf;
	case clMenu:
		return 0xf0f0f0;
	case clWindow:
		return 0xffffff;
	case clWindowFrame:
		return 0x646464;
	case clMenuText:
		return 0;
	case clWindowText:
		return 0;
	case clCaptionText:
		return 0;
	case clActiveBorder:
		return 0xb4b4b4;
	case clInactiveBorder:
		return 0xfcf7f4;
	case clAppWorkSpace:
		return 0xababab;
	case clHighlight:
		return 0xff9933;
	case clHighlightText:
		return 0xffffff;
	case clBtnFace:
		return 0xf0f0f0;
	case clBtnShadow:
		return 0xa0a0a0;
	case clGrayText:
		return 0x6d6d6d;
	case clBtnText:
		return 0;
	case clInactiveCaptionText:
		return 0x544e43;
	case clBtnHighlight:
		return 0xffffff;
	case cl3DDkShadow:
		return 0x696969;
	case cl3DLight:
		return 0xe3e3e3;
	case clInfoText:
		return 0;
	case clInfoBk:
		return 0xe1ffff;
	case clUnknown:
		return 0;
	case clHotLight:
		return 0xcc6600;
	case clGradientActiveCaption:
		return 0xead1b9;
	case clGradientInactiveCaption:
		return 0xf2e4d7;
	case clMenuLight:
		return 0xff9933;
	case clMenuBar:
		return 0xf0f0f0;
	default:
		return col & 0xFFFFFF;
	}
}
