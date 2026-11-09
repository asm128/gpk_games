#include "gpk_udp_server.h"
#include "gpk_string.h"
#include "gpk_sync.h"

#include <Windows.h>

#ifndef LLS_PROCESS_BROKER_H_23627
#define LLS_PROCESS_BROKER_H_23627

namespace gpk { struct SJSONReader; }

namespace llswin32
{
	struct SProcessHandles {
		HANDLE				ChildStd_IN_Read		= INVALID_HANDLE_VALUE;
		HANDLE				ChildStd_IN_Write		= INVALID_HANDLE_VALUE;
		HANDLE				ChildStd_OUT_Read		= INVALID_HANDLE_VALUE;
		HANDLE				ChildStd_OUT_Write		= INVALID_HANDLE_VALUE;
		HANDLE				ChildStd_ERR_Read		= INVALID_HANDLE_VALUE;
		HANDLE				ChildStd_ERR_Write		= INVALID_HANDLE_VALUE;
	};

	struct SProcess {
		PROCESS_INFORMATION	ProcessInfo				= {};
		STARTUPINFOA		StartInfo				= {sizeof(STARTUPINFOA)};
	};

	struct SProcessBrokerJob {
		::gpk::pobj<::gpk::SUDPConnection>	Connection;
		::gpk::au0_t					Request;
		::gpk::au0_t					Response;
		::gpk::string					CommandLineApp;
		::gpk::string					CommandLine;
		::llswin32::SProcessHandles		IOHandles;
		::llswin32::SProcess			Process;
		::gpk::refcount_t				Done					= 0;
		::gpk::error_t					Result					= 0;
	};

	struct SProcessBroker {
		::gpk::SUDPServer					Server;
		::gpk::apobj<::llswin32::SProcessBrokerJob>	Jobs;
		::gpk::vcsc_t					ProcessFileName			= "";
		::gpk::vcsc_t					ProcessMockPath			= "";
		::gpk::vcsc_t					ProcessParams				= "";
		::gpk::string					CommandLineApp			= "";
		::gpk::string					CommandLine				= "";
		uint16_t						Port						= 9998;
		int16_t						Adapter					= 0;
		bool							Started					= false;
	};

	::gpk::error_t	processBrokerSetup		(::llswin32::SProcessBroker & broker, const ::gpk::SJSONReader & jsonReader);
	::gpk::error_t	processBrokerUpdate		(::llswin32::SProcessBroker & broker);
	::gpk::error_t	processBrokerShutdown	(::llswin32::SProcessBroker & broker);
}

#endif // LLS_PROCESS_BROKER_H_23627
