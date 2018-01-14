#define LOG_TAG "sec-sap"
#define RIL_SHLIB

#include "secril-sap.h"

#include <string.h>
#include <utils/Log.h>

// --------------------------------------------------------------------------
// Constants

static const int CMD_CONNECT = 1;
static const int CMD_SIM_POWER = 4;
static const int CMD_ATR = 5;

static const int MSGID_CONNECT = 0;
static const int MSGID_DISCONNECT = 2;

static const int MSGID_SIM_OFF = 0x09;
static const int MSGID_SIM_ON = 0x0B;
static const int MSGID_SIM_RESET = 0x0D;

static const int MSGID_ATR = 0x07;

// --------------------------------------------------------------------------
// Local types

typedef struct
{
    int cmd;
    int msgId;
    RIL_Token t;
    void* bytes;
    size_t byteslen;
} RequestParam;

typedef struct _ResponseData
{
    int counter;
    void* param;
    void (*callback)(void* param, RIL_Errno e);
    void (*callbackData)(void* param, RIL_Errno e, int resultCode, void* data, size_t datalen);
} ResponseData;

// --------------------------------------------------------------------------
// Local data

static  RIL_RadioFunctions const *s_radioFunctions;
static const struct RIL_Env *s_env;
static ResponseData g_responseData = { .counter = 0x10000 };

// --------------------------------------------------------------------------
// Declarations

static void OnRequestComplete(RIL_Token t, RIL_Errno e, void *response, size_t responselen);

static void SendSapRequest(int cmd, int msgId, RIL_Token t);
static void SendSapApduRequest(void* data, size_t datalen, RIL_Token t);
static RIL_Token GetToken(void (*callback)(void* param, RIL_Errno e), void* param);
static RIL_Token GetTokenData(void (*callbackData)(void* param, RIL_Errno e, int resultCode,
                              void* data, size_t datalen), void* param);

// --------------------------------------------------------------------------
// Public functions

struct RIL_Env* GetEnv(const struct RIL_Env *env)
{
	static struct RIL_Env rilEnv;

    s_env = env;
    memcpy(&rilEnv, env, sizeof(struct RIL_Env));
    rilEnv.OnRequestComplete = OnRequestComplete;
    return &rilEnv;
}

void SetRadioFunctions(const RIL_RadioFunctions* radioFunctions)
{
    s_radioFunctions = radioFunctions;
}

void SecSapConnect(void* param, void (*callback)(void* param, RIL_Errno e))
{
    SendSapRequest(CMD_CONNECT, MSGID_CONNECT, GetToken(callback, param));
}

void SecSapDisconnect(void* param, void (*callback)(void* param, RIL_Errno e))
{
    SendSapRequest(CMD_CONNECT, MSGID_DISCONNECT, GetToken(callback, param));
}

void SecSapResetSim(void* param, void (*callback)(void* param, RIL_Errno e))
{
    SendSapRequest(CMD_SIM_POWER, MSGID_SIM_RESET, GetToken(callback, param));
}

void SecSapApdu(void* param, void* apdu, size_t apdusize, void (*callback)(void* param, RIL_Errno e,
                int resultCode, void* apdu, size_t apdulen))
{
    SendSapApduRequest(apdu, apdusize, GetTokenData(callback, param));
}

void SecSapTransferAtr(void* param, void (*callback)(void* param, RIL_Errno e, int resultCode,
                       void* apdu, size_t apdulen))
{
    SendSapRequest(CMD_ATR, MSGID_ATR, GetTokenData(callback, param));
}

void SecSapSimPower(void* param, bool on, void (*callback)(void* param, RIL_Errno e))
{
    SendSapRequest(CMD_SIM_POWER, on ? MSGID_SIM_ON : MSGID_SIM_OFF, GetToken(callback, param));
}

// --------------------------------------------------------------------------
// Handle requests

static RIL_Token GetToken(void (*callback)(void* param, RIL_Errno e), void* param)
{
    g_responseData.counter++;
    g_responseData.param = param;
    g_responseData.callback = callback;
    g_responseData.callbackData = NULL;
    return &g_responseData;
}

static RIL_Token GetTokenData(void (*callbackData)(void* param, RIL_Errno e, int resultCode,
                              void* data, size_t datalen), void* param)
{
    g_responseData.counter++;
    g_responseData.param = param;
    g_responseData.callback = NULL;
    g_responseData.callbackData = callbackData;
    return &g_responseData;
}

static void SapRequestTimedCallback(void *param)
{
    RequestParam* data = (RequestParam*)param;

    RLOGD("..... SapRequestTimedCallback() cmd = %d, msgId = %d, t = %p",
          data->cmd, data->msgId, data->t);

    static unsigned char request[5] = { 0x14, 0xFF, 0x00, 0x05, 0xFF };
    request[1] = data->cmd;
    request[4] = data->msgId;
    s_radioFunctions->onRequest(RIL_REQUEST_OEM_HOOK_RAW, request, sizeof(request), data->t);
}

static void SendSapRequest(int cmd, int msgId, RIL_Token t)
{
    RLOGD("..... sendSapRequest(cmd = %d, msgId = %d, t = %p)", cmd, msgId, t);

    static RequestParam data;
    data.cmd = cmd;
    data.msgId = msgId;
    data.t = t;
    data.bytes = NULL;
    data.byteslen = 0;

    s_env->RequestTimedCallback(SapRequestTimedCallback, &data, NULL);
}

static void SapApduRequestTimedCallback(void *param)
{
    RequestParam* data = (RequestParam*)param;

    RLOGD("..... SapApduRequestTimedCallback() bytes = %p, byteslen = %d, t = %p",
          data->bytes, data->byteslen, data->t);

    unsigned char* request = (unsigned char*)malloc(data->byteslen+6);
    request[0] = 0x14;
    request[1] = 0x06;
    request[2] = (data->byteslen+6) >> 8;
    request[3] = (data->byteslen+6) & 0xFF;
    request[4] = data->byteslen & 0xFF;
    request[5] = data->byteslen >> 8;
    memcpy(request+6, data->bytes, data->byteslen);
    s_radioFunctions->onRequest(RIL_REQUEST_OEM_HOOK_RAW, request, data->byteslen+6, data->t);

    //free(request);
    free(data->bytes);
}

static void SendSapApduRequest(void* bytes, size_t byteslen, RIL_Token t)
{
    RLOGD("..... sendSapApduRequest(bytes = %p, byteslen = %d, t = %p)", bytes, byteslen, t);

    static RequestParam data;
    data.cmd = 6;
    data.msgId = 0;
    data.t = t;
    data.bytes = malloc(byteslen);
    memcpy(data.bytes, bytes, byteslen);
    data.byteslen = byteslen;

    s_env->RequestTimedCallback(SapApduRequestTimedCallback, &data, NULL);
}

// --------------------------------------------------------------------------
// Handle response

static void OnRequestComplete(RIL_Token t, RIL_Errno e, void *response, size_t responselen)
{
    if ((t == &g_responseData) && (*((int*)t) == g_responseData.counter))
    {
        RLOGD("-----    OnRequestComplete(%p,%d,%p,%d)", t, e, response, responselen);

        ResponseData* req = (ResponseData*)t;

        if (req->callback != NULL)
        {
            RLOGD("        Callback(%p,%d)", req, e);
            req->callback(req->param, e);
        }
        else
        {
            void* bytes = NULL;
            short len = 0;
            int resultCode = 6; // Error, data not available
            if (responselen >= 7)
            {
                resultCode = ((unsigned char*)response)[4];
                len = *(const short*)((char*)response + 5);
                bytes = (char*)response + 7;
            }

            RLOGD("        Callback(%p,%d,%d,%p,%d)", req, e, resultCode, bytes, len);
            req->callbackData(req->param, e, resultCode, bytes, len);
        }
    }
    else
    {
        s_env->OnRequestComplete(t, e, response, responselen);
    }
}
