#include "lls_process_broker.h"

#include "gpk_json_expression.h"
#include "gpk_parse.h"

#include <process.h>

static	void	closeHandle				(HANDLE & handle) {
	if(handle && INVALID_HANDLE_VALUE != handle)
		::CloseHandle(handle);
	handle								= INVALID_HANDLE_VALUE;
}

static	void	closeProcessHandles		(::llswin32::SProcessBrokerJob & job) {
	::closeHandle(job.IOHandles.ChildStd_IN_Read);
	::closeHandle(job.IOHandles.ChildStd_IN_Write);
	::closeHandle(job.IOHandles.ChildStd_OUT_Read);
	::closeHandle(job.IOHandles.ChildStd_OUT_Write);
	::closeHandle(job.IOHandles.ChildStd_ERR_Read);
	::closeHandle(job.IOHandles.ChildStd_ERR_Write);
	::closeHandle(job.Process.ProcessInfo.hProcess);
	::closeHandle(job.Process.ProcessInfo.hThread);
}

static	::gpk::error_t	initHandles		(::llswin32::SProcessHandles & handles) {
	SECURITY_ATTRIBUTES			saAttr					= {sizeof(SECURITY_ATTRIBUTES)};
	saAttr.bInheritHandle							= TRUE;
	saAttr.lpSecurityDescriptor					= nullptr;
	ree_if(false == (bool)::CreatePipe			(&handles.ChildStd_OUT_Read, &handles.ChildStd_OUT_Write, &saAttr, 0), "Stdout CreatePipe: '%s'.", "Failed to create a pipe for the child process's STDOUT.");
	ree_if(false == (bool)::SetHandleInformation	(handles.ChildStd_OUT_Read, HANDLE_FLAG_INHERIT, 0), "Stdout SetHandleInformation: '%s'.", "Failed to keep the child process from inheriting the STDOUT read handle.");
	ree_if(false == (bool)::CreatePipe			(&handles.ChildStd_ERR_Read, &handles.ChildStd_ERR_Write, &saAttr, 0), "Stderr CreatePipe: '%s'.", "Failed to create a pipe for the child process's STDERR.");
	ree_if(false == (bool)::SetHandleInformation	(handles.ChildStd_ERR_Read, HANDLE_FLAG_INHERIT, 0), "Stderr SetHandleInformation: '%s'.", "Failed to keep the child process from inheriting the STDERR read handle.");
	ree_if(false == (bool)::CreatePipe			(&handles.ChildStd_IN_Read, &handles.ChildStd_IN_Write, &saAttr, 0), "Stdin CreatePipe: '%s'.", "Failed to create a pipe for the child process's STDIN.");
	ree_if(false == (bool)::SetHandleInformation	(handles.ChildStd_IN_Write, HANDLE_FLAG_INHERIT, 0), "Stdin SetHandleInformation: '%s'.", "Failed to keep the child process from inheriting the STDIN write handle.");
	return 0;
}

static	::gpk::error_t	createChildProcess
	( ::llswin32::SProcess	& process
	, ::gpk::vu8			environmentBlock
	, ::gpk::vcst_t		appPath
	, ::gpk::vsc_t		commandLine
	) {
	stacxpr	uint32_t		creationFlags				= CREATE_SUSPENDED;
	::closeHandle(process.ProcessInfo.hProcess);
	::closeHandle(process.ProcessInfo.hThread);
	ree_if(false == (bool)::CreateProcessA
		( appPath.begin()
		, commandLine.begin()
		, nullptr
		, nullptr
		, true
		, creationFlags
		, environmentBlock.begin()
		, nullptr
		, &process.StartInfo
		, &process.ProcessInfo
		), "Failed to create process '%s'.", appPath.begin());
	info_printf("Creating process '%s' with command line '%s'.", appPath.begin(), commandLine.begin());
	::ResumeThread(process.ProcessInfo.hThread);
	return 0;
}

static	::gpk::error_t	writeToPipe			(::llswin32::SProcessHandles & handles, ::gpk::vcu0_t bytesToSend) {
	uint32_t					offsetWritten				= 0;
	while(offsetWritten < bytesToSend.size()) {
		DWORD						dwWritten					= 0;
		ree_if(false == (bool)::WriteFile(handles.ChildStd_IN_Write, &bytesToSend[offsetWritten], bytesToSend.size() - offsetWritten, &dwWritten, nullptr), "%s", "Failed to write to child process' standard input.");
		ree_if(0 == dwWritten, "%s", "No bytes were written to child process' standard input.");
		offsetWritten							+= dwWritten;
	}
	::closeHandle(handles.ChildStd_IN_Write);
	return 0;
}

static	::gpk::error_t	readFromPipe			(const ::llswin32::SProcessHandles & handles, ::gpk::au0_t & readBytes) {
	stacxpr	uint32_t		bufferSize					= 1024 * 1024;
	::gpk::au0_t				buffer;
	gpk_necs(buffer.resize(bufferSize));
	for(;;) {
		DWORD						dwRead						= 0;
		if(false == (bool)::ReadFile(handles.ChildStd_OUT_Read, buffer.begin(), buffer.size(), &dwRead, nullptr)) {
			const DWORD				lastError					= ::GetLastError();
			if(ERROR_BROKEN_PIPE == lastError)
				break;
			ree_if(true, "Failed to read child process' standard output. Windows error: %lu.", lastError);
		}
		if(0 == dwRead)
			break;
		gpk_necs(readBytes.append(buffer.begin(), dwRead));
	}
	return 0;
}

static	::gpk::error_t	processRequest			(::llswin32::SProcessBrokerJob & job) {
	const ::gpk::error_t		result						= [&job]() -> ::gpk::error_t {
		gpk_necs(::initHandles(job.IOHandles));
		job.Process.StartInfo.hStdError				= job.IOHandles.ChildStd_ERR_Write;
		job.Process.StartInfo.hStdOutput				= job.IOHandles.ChildStd_OUT_Write;
		job.Process.StartInfo.hStdInput				= job.IOHandles.ChildStd_IN_Read;
		job.Process.StartInfo.dwFlags				|= STARTF_USESTDHANDLES;

		::gpk::vu8				environmentBlock				= job.Request;
		const ::gpk::vcu0_t		packet						= job.Request;
		const ::gpk::error_t	contentOffset				= ::gpk::find_sequence_pod(::gpk::vcu0_t{(const uint8_t*)"\0", 2}, packet);
		ree_if(0 > contentOffset, "%s", "Failed to find the CGI environment block terminator.");

		gpk_necs(::createChildProcess(job.Process, environmentBlock, job.CommandLineApp, job.CommandLine));
		::closeHandle(job.IOHandles.ChildStd_OUT_Write);
		::closeHandle(job.IOHandles.ChildStd_ERR_Write);
		::closeHandle(job.IOHandles.ChildStd_IN_Read);

		const uint32_t			contentBegin					= (uint32_t)contentOffset + 2;
		if(packet.size() > contentBegin)
			gpk_necs(::writeToPipe(job.IOHandles, {&packet[contentBegin], packet.size() - contentBegin}));
		else
			::closeHandle(job.IOHandles.ChildStd_IN_Write);

		gpk_necs(::readFromPipe(job.IOHandles, job.Response));
		return 0;
	}();
	::closeProcessHandles(job);
	return result;
}

static	void	threadProcessRequest		(void * jobAddress) {
	::llswin32::SProcessBrokerJob	& job						= *(::llswin32::SProcessBrokerJob*)jobAddress;
	job.Result									= ::processRequest(job);
	if(::gpk::failed(job.Result) && 0 == job.Response.size()) {
		stacxpr	const char		failureResponse[]				= "LLS process broker failed to execute the configured process.";
		job.Response.append((const uint8_t*)failureResponse, ::gpk::size(failureResponse) - 1);
	}
	gpk_sync_increment(job.Done);
}

::gpk::error_t	llswin32::processBrokerSetup		(::llswin32::SProcessBroker & broker, const ::gpk::SJSONReader & jsonReader) {
	uint64_t					port						= broker.Port;
	uint64_t					adapter					= broker.Adapter;
	::gpk::vcsc_t				jsonNumber					= {};
	const ::gpk::error_t		appNodeIndex				= ::gpk::jsonExpressionResolve(::gpk::vcs{"application.brt"}, jsonReader, 0, broker.ProcessFileName);
	wf_if(::gpk::failed(appNodeIndex), "Failed to find application node '%s'.", "application.brt")
	else {
		wf_if(::gpk::failed(::gpk::jsonExpressionResolve(::gpk::vcs{"process.executable_path"}, jsonReader, appNodeIndex, broker.ProcessFileName)), "Failed to load process executable path. Last contents found: '%s'.", broker.ProcessFileName.begin());
		wf_if(::gpk::failed(::gpk::jsonExpressionResolve(::gpk::vcs{"process.command_line_app_name"}, jsonReader, appNodeIndex, broker.ProcessMockPath)), "Failed to load process command-line application name. Last contents found: '%s'.", broker.ProcessMockPath.begin());
		wf_if(::gpk::failed(::gpk::jsonExpressionResolve(::gpk::vcs{"process.command_line_params"}, jsonReader, appNodeIndex, broker.ProcessParams)), "Failed to load process command-line parameters. Last contents found: '%s'.", broker.ProcessParams.begin());
		if(::gpk::succeeded(::gpk::jsonExpressionResolve(::gpk::vcs{"listen_port"}, jsonReader, appNodeIndex, jsonNumber)))
			::gpk::parseIntegerDecimal(jsonNumber, port);
		jsonNumber								= {};
		if(::gpk::succeeded(::gpk::jsonExpressionResolve(::gpk::vcs{"adapter"}, jsonReader, appNodeIndex, jsonNumber)))
			::gpk::parseIntegerDecimal(jsonNumber, adapter);
	}

	broker.Port								= (uint16_t)port;
	broker.Adapter							= (int16_t)adapter;
	broker.CommandLineApp						= broker.ProcessFileName;
	broker.CommandLine						= broker.ProcessMockPath;
	if(broker.ProcessParams.size()) {
		broker.CommandLine.push_back(' ');
		broker.CommandLine.append(broker.ProcessParams);
	}
	broker.CommandLine.push_back(0);
	broker.CommandLineApp.push_back(0);
	gpk_necall(::gpk::serverStart(broker.Server, broker.Port, broker.Adapter), "Failed to start process broker on port %u.", broker.Port);
	broker.Started								= true;
	return 0;
}

::gpk::error_t	llswin32::processBrokerUpdate		(::llswin32::SProcessBroker & broker) {
	::gpk::aobj<::gpk::TUDPQueue>	receivedPerClient;
	gpk_necs(::gpk::serverPayloadCollect(broker.Server, receivedPerClient));
	for(uint32_t iClient = 0; iClient < receivedPerClient.size(); ++iClient) {
		::gpk::pobj<::gpk::SUDPConnection>	connection;
		{
			::std::lock_guard			lock						(broker.Server.Mutex);
			if(iClient < broker.Server.Clients.size())
				connection							= broker.Server.Clients[iClient];
		}
		if(!connection)
			continue;
		for(uint32_t iMessage = 0; iMessage < receivedPerClient[iClient].size(); ++iMessage) {
			const ::gpk::pobj<::gpk::SUDPMessage>	& message			= receivedPerClient[iClient][iMessage];
			if(!message)
				continue;
			::gpk::pobj<::llswin32::SProcessBrokerJob>	job;
			job.create();
			job->Connection							= connection;
			job->Request								= message->Payload;
			job->CommandLineApp						= broker.CommandLineApp;
			job->CommandLine							= broker.CommandLine;
			gpk_necs(broker.Jobs.push_back(job));
			if((uintptr_t)-1L == ::_beginthread(::threadProcessRequest, 0, job.operator->())) {
				job->Result								= -1;
				stacxpr const char	failureResponse[]				= "LLS process broker failed to start its request worker.";
				job->Response.append((const uint8_t*)failureResponse, ::gpk::size(failureResponse) - 1);
				gpk_sync_increment(job->Done);
			}
		}
	}

	for(uint32_t iJob = 0; iJob < broker.Jobs.size();) {
		::gpk::pobj<::llswin32::SProcessBrokerJob>	job				= broker.Jobs[iJob];
		if(!job || false == gpk_sync_compare_exchange(job->Done, 1, 1)) {
			++iJob;
			continue;
		}
		if(job->Connection && job->Response.size()) {
			::std::lock_guard			lock						(broker.Server.Mutex);
			es_if_failed(::gpk::connectionPushData(*job->Connection, job->Connection->Queue, job->Response, true, true));
		}
		gpk_necs(broker.Jobs.remove_unordered(iJob));
	}
	return 0;
}

::gpk::error_t	llswin32::processBrokerShutdown	(::llswin32::SProcessBroker & broker) {
	if(broker.Started)
		ws_if_failed(::gpk::serverStop(broker.Server));
	broker.Started								= false;
	while(broker.Jobs.size()) {
		for(uint32_t iJob = 0; iJob < broker.Jobs.size();) {
			::gpk::pobj<::llswin32::SProcessBrokerJob>	& job		= broker.Jobs[iJob];
			if(job && false == gpk_sync_compare_exchange(job->Done, 1, 1)) {
				++iJob;
				continue;
			}
			gpk_necs(broker.Jobs.remove_unordered(iJob));
		}
		if(broker.Jobs.size())
			::gpk::sleep(1);
	}
	return 0;
}
