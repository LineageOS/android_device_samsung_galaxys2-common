#define LOG_TAG "secril-shim"
#define RIL_SHLIB
#define VDBG 1

#include <pthread.h>
#include <termios.h>
#include <alloca.h>
#include <assert.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

#include <telephony/ril_cdma_sms.h>
#include <sys/system_properties.h>
#include <telephony/librilutils.h>
#include <cutils/sockets.h>
#include <cutils/compiler.h>
#include <telephony/ril.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/cdefs.h>
#include <utils/Log.h>
#include <sys/stat.h>
#define REAL_RIL_NAME				"/system/lib/libsec-ril.so"


static RIL_RadioFunctions const *mRealRadioFuncs;
static const struct RIL_Env *mEnv;

const char *
requestToString(int request) {
/*
 cat libs/telephony/ril_commands.h \
 | egrep "^ *{RIL_" \
 | sed -re 's/\{RIL_([^,]+),[^,]+,([^}]+).+/case RIL_\1: return "\1";/'


 cat libs/telephony/ril_unsol_commands.h \
 | egrep "^ *{RIL_" \
 | sed -re 's/\{RIL_([^,]+),([^}]+).+/case RIL_\1: return "\1";/'

*/
    switch(request) {
        case RIL_REQUEST_GET_SIM_STATUS: return "GET_SIM_STATUS";
        case RIL_REQUEST_ENTER_SIM_PIN: return "ENTER_SIM_PIN";
        case RIL_REQUEST_ENTER_SIM_PUK: return "ENTER_SIM_PUK";
        case RIL_REQUEST_ENTER_SIM_PIN2: return "ENTER_SIM_PIN2";
        case RIL_REQUEST_ENTER_SIM_PUK2: return "ENTER_SIM_PUK2";
        case RIL_REQUEST_CHANGE_SIM_PIN: return "CHANGE_SIM_PIN";
        case RIL_REQUEST_CHANGE_SIM_PIN2: return "CHANGE_SIM_PIN2";
        case RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION: return "ENTER_NETWORK_DEPERSONALIZATION";
        case RIL_REQUEST_GET_CURRENT_CALLS: return "GET_CURRENT_CALLS";
        case RIL_REQUEST_DIAL: return "DIAL";
        case RIL_REQUEST_GET_IMSI: return "GET_IMSI";
        case RIL_REQUEST_HANGUP: return "HANGUP";
        case RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND: return "HANGUP_WAITING_OR_BACKGROUND";
        case RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND: return "HANGUP_FOREGROUND_RESUME_BACKGROUND";
        case RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE: return "SWITCH_WAITING_OR_HOLDING_AND_ACTIVE";
        case RIL_REQUEST_CONFERENCE: return "CONFERENCE";
        case RIL_REQUEST_UDUB: return "UDUB";
        case RIL_REQUEST_LAST_CALL_FAIL_CAUSE: return "LAST_CALL_FAIL_CAUSE";
        case RIL_REQUEST_SIGNAL_STRENGTH: return "SIGNAL_STRENGTH";
        case RIL_REQUEST_VOICE_REGISTRATION_STATE: return "VOICE_REGISTRATION_STATE";
        case RIL_REQUEST_DATA_REGISTRATION_STATE: return "DATA_REGISTRATION_STATE";
        case RIL_REQUEST_OPERATOR: return "OPERATOR";
        case RIL_REQUEST_RADIO_POWER: return "RADIO_POWER";
        case RIL_REQUEST_DTMF: return "DTMF";
        case RIL_REQUEST_SEND_SMS: return "SEND_SMS";
        case RIL_REQUEST_SEND_SMS_EXPECT_MORE: return "SEND_SMS_EXPECT_MORE";
        case RIL_REQUEST_SETUP_DATA_CALL: return "SETUP_DATA_CALL";
        case RIL_REQUEST_SIM_IO: return "SIM_IO";
        case RIL_REQUEST_SEND_USSD: return "SEND_USSD";
        case RIL_REQUEST_CANCEL_USSD: return "CANCEL_USSD";
        case RIL_REQUEST_GET_CLIR: return "GET_CLIR";
        case RIL_REQUEST_SET_CLIR: return "SET_CLIR";
        case RIL_REQUEST_QUERY_CALL_FORWARD_STATUS: return "QUERY_CALL_FORWARD_STATUS";
        case RIL_REQUEST_SET_CALL_FORWARD: return "SET_CALL_FORWARD";
        case RIL_REQUEST_QUERY_CALL_WAITING: return "QUERY_CALL_WAITING";
        case RIL_REQUEST_SET_CALL_WAITING: return "SET_CALL_WAITING";
        case RIL_REQUEST_SMS_ACKNOWLEDGE: return "SMS_ACKNOWLEDGE";
        case RIL_REQUEST_GET_IMEI: return "GET_IMEI";
        case RIL_REQUEST_GET_IMEISV: return "GET_IMEISV";
        case RIL_REQUEST_ANSWER: return "ANSWER";
        case RIL_REQUEST_DEACTIVATE_DATA_CALL: return "DEACTIVATE_DATA_CALL";
        case RIL_REQUEST_QUERY_FACILITY_LOCK: return "QUERY_FACILITY_LOCK";
        case RIL_REQUEST_SET_FACILITY_LOCK: return "SET_FACILITY_LOCK";
        case RIL_REQUEST_CHANGE_BARRING_PASSWORD: return "CHANGE_BARRING_PASSWORD";
        case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE: return "QUERY_NETWORK_SELECTION_MODE";
        case RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC: return "SET_NETWORK_SELECTION_AUTOMATIC";
        case RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL: return "SET_NETWORK_SELECTION_MANUAL";
        case RIL_REQUEST_QUERY_AVAILABLE_NETWORKS: return "QUERY_AVAILABLE_NETWORKS";
        case RIL_REQUEST_DTMF_START: return "DTMF_START";
        case RIL_REQUEST_DTMF_STOP: return "DTMF_STOP";
        case RIL_REQUEST_BASEBAND_VERSION: return "BASEBAND_VERSION";
        case RIL_REQUEST_SEPARATE_CONNECTION: return "SEPARATE_CONNECTION";
        case RIL_REQUEST_SET_MUTE: return "SET_MUTE";
        case RIL_REQUEST_GET_MUTE: return "GET_MUTE";
        case RIL_REQUEST_QUERY_CLIP: return "QUERY_CLIP";
        case RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE: return "LAST_DATA_CALL_FAIL_CAUSE";
        case RIL_REQUEST_DATA_CALL_LIST: return "DATA_CALL_LIST";
        case RIL_REQUEST_RESET_RADIO: return "RESET_RADIO";
        case RIL_REQUEST_OEM_HOOK_RAW: return "OEM_HOOK_RAW";
        case RIL_REQUEST_OEM_HOOK_STRINGS: return "OEM_HOOK_STRINGS";
        case RIL_REQUEST_SCREEN_STATE: return "SCREEN_STATE";
        case RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION: return "SET_SUPP_SVC_NOTIFICATION";
        case RIL_REQUEST_WRITE_SMS_TO_SIM: return "WRITE_SMS_TO_SIM";
        case RIL_REQUEST_DELETE_SMS_ON_SIM: return "DELETE_SMS_ON_SIM";
        case RIL_REQUEST_SET_BAND_MODE: return "SET_BAND_MODE";
        case RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE: return "QUERY_AVAILABLE_BAND_MODE";
        case RIL_REQUEST_STK_GET_PROFILE: return "STK_GET_PROFILE";
        case RIL_REQUEST_STK_SET_PROFILE: return "STK_SET_PROFILE";
        case RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND: return "STK_SEND_ENVELOPE_COMMAND";
        case RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE: return "STK_SEND_TERMINAL_RESPONSE";
        case RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM: return "STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM";
        case RIL_REQUEST_EXPLICIT_CALL_TRANSFER: return "EXPLICIT_CALL_TRANSFER";
        case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE: return "SET_PREFERRED_NETWORK_TYPE";
        case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE: return "GET_PREFERRED_NETWORK_TYPE";
        case RIL_REQUEST_GET_NEIGHBORING_CELL_IDS: return "GET_NEIGHBORING_CELL_IDS";
        case RIL_REQUEST_SET_LOCATION_UPDATES: return "SET_LOCATION_UPDATES";
        case RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE: return "CDMA_SET_SUBSCRIPTION_SOURCE";
        case RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE: return "CDMA_SET_ROAMING_PREFERENCE";
        case RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE: return "CDMA_QUERY_ROAMING_PREFERENCE";
        case RIL_REQUEST_SET_TTY_MODE: return "SET_TTY_MODE";
        case RIL_REQUEST_QUERY_TTY_MODE: return "QUERY_TTY_MODE";
        case RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE: return "CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE";
        case RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE: return "CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE";
        case RIL_REQUEST_CDMA_FLASH: return "CDMA_FLASH";
        case RIL_REQUEST_CDMA_BURST_DTMF: return "CDMA_BURST_DTMF";
        case RIL_REQUEST_CDMA_VALIDATE_AND_WRITE_AKEY: return "CDMA_VALIDATE_AND_WRITE_AKEY";
        case RIL_REQUEST_CDMA_SEND_SMS: return "CDMA_SEND_SMS";
        case RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE: return "CDMA_SMS_ACKNOWLEDGE";
        case RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG: return "GSM_GET_BROADCAST_SMS_CONFIG";
        case RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG: return "GSM_SET_BROADCAST_SMS_CONFIG";
        case RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION: return "GSM_SMS_BROADCAST_ACTIVATION";
        case RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG: return "CDMA_GET_BROADCAST_SMS_CONFIG";
        case RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG: return "CDMA_SET_BROADCAST_SMS_CONFIG";
        case RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION: return "CDMA_SMS_BROADCAST_ACTIVATION";
        case RIL_REQUEST_CDMA_SUBSCRIPTION: return "CDMA_SUBSCRIPTION";
        case RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM: return "CDMA_WRITE_SMS_TO_RUIM";
        case RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM: return "CDMA_DELETE_SMS_ON_RUIM";
        case RIL_REQUEST_DEVICE_IDENTITY: return "DEVICE_IDENTITY";
        case RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE: return "EXIT_EMERGENCY_CALLBACK_MODE";
        case RIL_REQUEST_GET_SMSC_ADDRESS: return "GET_SMSC_ADDRESS";
        case RIL_REQUEST_SET_SMSC_ADDRESS: return "SET_SMSC_ADDRESS";
        case RIL_REQUEST_REPORT_SMS_MEMORY_STATUS: return "REPORT_SMS_MEMORY_STATUS";
        case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING: return "REPORT_STK_SERVICE_IS_RUNNING";
        case RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE: return "CDMA_GET_SUBSCRIPTION_SOURCE";
        case RIL_REQUEST_ISIM_AUTHENTICATION: return "ISIM_AUTHENTICATION";
        case RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU: return "ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU";
        case RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS: return "STK_SEND_ENVELOPE_WITH_STATUS";
        case RIL_REQUEST_VOICE_RADIO_TECH: return "VOICE_RADIO_TECH";
        case RIL_REQUEST_GET_CELL_INFO_LIST: return "GET_CELL_INFO_LIST";
        case RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE: return "SET_UNSOL_CELL_INFO_LIST_RATE";
        case RIL_REQUEST_SET_INITIAL_ATTACH_APN: return "SET_INITIAL_ATTACH_APN";
        case RIL_REQUEST_IMS_REGISTRATION_STATE: return "IMS_REGISTRATION_STATE";
        case RIL_REQUEST_IMS_SEND_SMS: return "IMS_SEND_SMS";
        case RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC: return "SIM_TRANSMIT_APDU_BASIC";
        case RIL_REQUEST_SIM_OPEN_CHANNEL: return "SIM_OPEN_CHANNEL";
        case RIL_REQUEST_SIM_CLOSE_CHANNEL: return "SIM_CLOSE_CHANNEL";
        case RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL: return "SIM_TRANSMIT_APDU_CHANNEL";
        case RIL_REQUEST_NV_READ_ITEM: return "NV_READ_ITEM";
        case RIL_REQUEST_NV_WRITE_ITEM: return "NV_WRITE_ITEM";
        case RIL_REQUEST_NV_WRITE_CDMA_PRL: return "NV_WRITE_CDMA_PRL";
        case RIL_REQUEST_NV_RESET_CONFIG: return "NV_RESET_CONFIG";
        case RIL_REQUEST_SET_UICC_SUBSCRIPTION: return "SET_UICC_SUBSCRIPTION";
        case RIL_REQUEST_ALLOW_DATA: return "ALLOW_DATA";
        case RIL_REQUEST_GET_HARDWARE_CONFIG: return "GET_HARDWARE_CONFIG";
        case RIL_REQUEST_SIM_AUTHENTICATION: return "SIM_AUTHENTICATION";
        case RIL_REQUEST_GET_DC_RT_INFO: return "GET_DC_RT_INFO";
        case RIL_REQUEST_SET_DC_RT_INFO_RATE: return "SET_DC_RT_INFO_RATE";
        case RIL_REQUEST_SET_DATA_PROFILE: return "SET_DATA_PROFILE";
        case RIL_REQUEST_SHUTDOWN: return "SHUTDOWN";
        case RIL_REQUEST_GET_RADIO_CAPABILITY: return "GET_RADIO_CAPABILITY";
        case RIL_REQUEST_SET_RADIO_CAPABILITY: return "SET_RADIO_CAPABILITY";
        case RIL_REQUEST_START_LCE: return "START_LCE";
        case RIL_REQUEST_STOP_LCE: return "STOP_LCE";
        case RIL_REQUEST_PULL_LCEDATA: return "PULL_LCEDATA";
        case RIL_REQUEST_GET_ACTIVITY_INFO: return "GET_ACTIVITY_INFO";
        case RIL_REQUEST_SET_CARRIER_RESTRICTIONS: return "SET_CARRIER_RESTRICTIONS";
        case RIL_REQUEST_GET_CARRIER_RESTRICTIONS: return "GET_CARRIER_RESTRICTIONS";
        case RIL_RESPONSE_ACKNOWLEDGEMENT: return "RESPONSE_ACKNOWLEDGEMENT";
        case RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED: return "UNSOL_RESPONSE_RADIO_STATE_CHANGED";
        case RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED: return "UNSOL_RESPONSE_CALL_STATE_CHANGED";
        case RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED: return "UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED";
        case RIL_UNSOL_RESPONSE_NEW_SMS: return "UNSOL_RESPONSE_NEW_SMS";
        case RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT: return "UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT";
        case RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM: return "UNSOL_RESPONSE_NEW_SMS_ON_SIM";
        case RIL_UNSOL_ON_USSD: return "UNSOL_ON_USSD";
        case RIL_UNSOL_ON_USSD_REQUEST: return "UNSOL_ON_USSD_REQUEST";
        case RIL_UNSOL_NITZ_TIME_RECEIVED: return "UNSOL_NITZ_TIME_RECEIVED";
        case RIL_UNSOL_SIGNAL_STRENGTH: return "UNSOL_SIGNAL_STRENGTH";
        case RIL_UNSOL_DATA_CALL_LIST_CHANGED: return "UNSOL_DATA_CALL_LIST_CHANGED";
        case RIL_UNSOL_SUPP_SVC_NOTIFICATION: return "UNSOL_SUPP_SVC_NOTIFICATION";
        case RIL_UNSOL_STK_SESSION_END: return "UNSOL_STK_SESSION_END";
        case RIL_UNSOL_STK_PROACTIVE_COMMAND: return "UNSOL_STK_PROACTIVE_COMMAND";
        case RIL_UNSOL_STK_EVENT_NOTIFY: return "UNSOL_STK_EVENT_NOTIFY";
        case RIL_UNSOL_STK_CALL_SETUP: return "UNSOL_STK_CALL_SETUP";
        case RIL_UNSOL_SIM_SMS_STORAGE_FULL: return "UNSOL_SIM_SMS_STORAGE_FULL";
        case RIL_UNSOL_SIM_REFRESH: return "UNSOL_SIM_REFRESH";
        case RIL_UNSOL_CALL_RING: return "UNSOL_CALL_RING";
        case RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED: return "UNSOL_RESPONSE_SIM_STATUS_CHANGED";
        case RIL_UNSOL_RESPONSE_CDMA_NEW_SMS: return "UNSOL_RESPONSE_CDMA_NEW_SMS";
        case RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS: return "UNSOL_RESPONSE_NEW_BROADCAST_SMS";
        case RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL: return "UNSOL_CDMA_RUIM_SMS_STORAGE_FULL";
        case RIL_UNSOL_RESTRICTED_STATE_CHANGED: return "UNSOL_RESTRICTED_STATE_CHANGED";
        case RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE: return "UNSOL_ENTER_EMERGENCY_CALLBACK_MODE";
        case RIL_UNSOL_CDMA_CALL_WAITING: return "UNSOL_CDMA_CALL_WAITING";
        case RIL_UNSOL_CDMA_OTA_PROVISION_STATUS: return "UNSOL_CDMA_OTA_PROVISION_STATUS";
        case RIL_UNSOL_CDMA_INFO_REC: return "UNSOL_CDMA_INFO_REC";
        case RIL_UNSOL_OEM_HOOK_RAW: return "UNSOL_OEM_HOOK_RAW";
        case RIL_UNSOL_RINGBACK_TONE: return "UNSOL_RINGBACK_TONE";
        case RIL_UNSOL_RESEND_INCALL_MUTE: return "UNSOL_RESEND_INCALL_MUTE";
        case RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED: return "UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED";
        case RIL_UNSOL_CDMA_PRL_CHANGED: return "UNSOL_CDMA_PRL_CHANGED";
        case RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE: return "UNSOL_EXIT_EMERGENCY_CALLBACK_MODE";
        case RIL_UNSOL_RIL_CONNECTED: return "UNSOL_RIL_CONNECTED";
        case RIL_UNSOL_VOICE_RADIO_TECH_CHANGED: return "UNSOL_VOICE_RADIO_TECH_CHANGED";
        case RIL_UNSOL_CELL_INFO_LIST: return "UNSOL_CELL_INFO_LIST";
        case RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED: return "UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED";
        case RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED: return "UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED";
        case RIL_UNSOL_SRVCC_STATE_NOTIFY: return "UNSOL_SRVCC_STATE_NOTIFY";
        case RIL_UNSOL_HARDWARE_CONFIG_CHANGED: return "UNSOL_HARDWARE_CONFIG_CHANGED";
        case RIL_UNSOL_DC_RT_INFO_CHANGED: return "UNSOL_DC_RT_INFO_CHANGED";
        case RIL_UNSOL_RADIO_CAPABILITY: return "UNSOL_RADIO_CAPABILITY";
        case RIL_UNSOL_MODEM_RESTART: return "UNSOL_MODEM_RESTART";
        case RIL_UNSOL_ON_SS: return "UNSOL_ON_SS";
        case RIL_UNSOL_STK_CC_ALPHA_NOTIFY: return "UNSOL_STK_CC_ALPHA_NOTIFY";
        case RIL_UNSOL_LCEDATA_RECV: return "UNSOL_LCEDATA_RECV";
        case RIL_UNSOL_PCO_DATA: return "UNSOL_PCO_DATA";
        default: return "<unknown request>";
    }
}

static void rilOnRequest(int request, void *data, size_t datalen, RIL_Token t)
{
    if (VDBG)
        RLOGD("%s: %s data:(%p) datalen:%d token: %d", __func__, requestToString(request), data, datalen, t);

    switch (request) {
        case RIL_REQUEST_GET_RADIO_CAPABILITY:
            RLOGW("%s: Returning NOT_SUPPORTED on %s", __func__, requestToString(request));
            mEnv->OnRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
            break;
        default:
            mRealRadioFuncs->onRequest(request, data, datalen, t);
    }
}

static void patchMem(void *libHandle) {
	/*
	 * MAX_TIMEOUT is used for a call to pthread_cond_timedwait_relative_np.
	 * The issue is bionic has switched to using absolute timeouts instead of
	 * relative timeouts, and a maximum time value can cause an overflow in
	 * the function converting relative to absolute timespecs if unpatched.
	 *
	 * By patching this to 0x01FFFFFF from 0x7FFFFFFF, the timeout should
	 * expire in about a year rather than 68 years, and the RIL should be good
	 * up until the year 2036 or so.
	 */
	uint32_t *MAX_TIMEOUT;

	MAX_TIMEOUT = (uint32_t *)dlsym(libHandle, "MAX_TIMEOUT");
	if (CC_UNLIKELY(!MAX_TIMEOUT)) {
		RLOGE("%s: MAX_TIMEOUT could not be found!", __FUNCTION__);
		return;
	}
	RLOGD("%s: MAX_TIMEOUT found at %p!", __FUNCTION__, MAX_TIMEOUT);
	RLOGD("%s: MAX_TIMEOUT is currently 0x%" PRIX32, __FUNCTION__, *MAX_TIMEOUT);
	if (CC_LIKELY(*MAX_TIMEOUT == 0x7FFFFFFF)) {
		*MAX_TIMEOUT = 0x01FFFFFF;
		RLOGI("%s: MAX_TIMEOUT was changed to 0x0%" PRIX32, __FUNCTION__, *MAX_TIMEOUT);
	} else {
		RLOGW("%s: MAX_TIMEOUT was not 0x7FFFFFFF; leaving alone", __FUNCTION__);
	}

}

const RIL_RadioFunctions* RIL_Init(const struct RIL_Env *env, int argc, char **argv)
{
	RIL_RadioFunctions const* (*fRealRilInit)(const struct RIL_Env *env, int argc, char **argv);
	static RIL_RadioFunctions rilInfo;
	void *realRilLibHandle;
	int i;


	//save the env;
	mEnv = env;

	//get the real RIL
	realRilLibHandle = dlopen(REAL_RIL_NAME, RTLD_LOCAL);
	if (!realRilLibHandle) {
		RLOGE("Failed to load the real RIL '" REAL_RIL_NAME  "': %s\n", dlerror());
		return NULL;
	}

	//remove "-c" command line as Samsung's RIL does not understand it - it just barfs instead
	for (i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "-c") && i != argc -1) {	//found it
			memcpy(argv + i, argv + i + 2, sizeof(char*[argc - i - 2]));
			argc -= 2;
		}
	}

	//load the real RIL
	fRealRilInit = dlsym(realRilLibHandle, "RIL_Init");
	if (!fRealRilInit) {
		RLOGE("Failed to find the real RIL's entry point\n");
		goto out_fail;
	}

	RLOGD("Calling the real RIL's entry point with %u args\n", argc);
	for (i = 0; i < argc; i++)
		RLOGD("  argv[%2d] = '%s'\n", i, argv[i]);

	// Fix RIL issues by patching memory
	patchMem(realRilLibHandle);

	//try to init the real ril
	mRealRadioFuncs = fRealRilInit(env, argc, argv);
	if (!mRealRadioFuncs) {
		RLOGE("The real RIL's entry point failed\n");
		goto out_fail;
	}

	//copy the real RIL's info struct, then replace the onRequest pointer with our own
	rilInfo = *mRealRadioFuncs;
	rilInfo.onRequest = rilOnRequest;

	RLOGD("Wrapped RIL version is '%s'\n", mRealRadioFuncs->getVersion());

	//we're all good - return to caller
	return &rilInfo;

out_fail:
	dlclose(realRilLibHandle);
	return NULL;
}
