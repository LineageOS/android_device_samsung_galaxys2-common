#define LOG_TAG "sec-sap"
#define RIL_SHLIB

#include "sap-functions.h"
#include <pthread.h>
#include "secril-sap.h"

// --------------------------------------------------------------------------
// Connect

typedef struct
{
    RIL_Token t;
    void (*connectRsp)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_CONNECT_RSP rsp);
    void (*statusInd)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_STATUS_IND rsp);
} SapConnectParam;

static void SecSapConnectRsp1(void* param, RIL_Errno e);
static void SecSapConnectRsp2(void* param, RIL_Errno e);

void SapConnect(RIL_Token t, RIL_SIM_SAP_CONNECT_REQ req,
                void (*connectRsp)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_CONNECT_RSP rsp),
                void (*statusInd)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_STATUS_IND rsp))
{
    static SapConnectParam param;
    param.t = t;
    param.connectRsp = connectRsp;
    param.statusInd = statusInd;

    SecSapConnect(&param, SecSapConnectRsp1);
}

static void SecSapConnectRsp1(void* param, RIL_Errno e)
{
    SapConnectParam* connectParam = (SapConnectParam*)param;

    RIL_SIM_SAP_CONNECT_RSP rsp = RIL_SIM_SAP_CONNECT_RSP_init_zero;
    rsp.response = (e == RIL_E_SUCCESS)
        ? RIL_SIM_SAP_CONNECT_RSP_Response_RIL_E_SUCCESS
        : RIL_SIM_SAP_CONNECT_RSP_Response_RIL_E_SAP_CONNECT_FAILURE;
    rsp.has_max_message_size = false;

    connectParam->connectRsp(connectParam->t, e, rsp);

    if (e == RIL_E_SUCCESS)
    {
        SecSapResetSim(param, SecSapConnectRsp2);
    }
}

static void SecSapConnectRsp2(void* param, RIL_Errno e)
{
    SapConnectParam* connectParam = (SapConnectParam*)param;

    RIL_SIM_SAP_STATUS_IND rsp = RIL_SIM_SAP_STATUS_IND_init_zero;
    rsp.statusChange = RIL_SIM_SAP_STATUS_IND_Status_RIL_SIM_STATUS_CARD_RESET;

    connectParam->statusInd(connectParam->t, e, rsp);
}

// --------------------------------------------------------------------------
// Disconnect

typedef struct
{
    RIL_Token t;
    void (*rsp)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_DISCONNECT_RSP rsp);
} SapDisconnectParam;

static void SapDisconnectRsp1(void* param, RIL_Errno e);
static void SapDisconnectRsp2(void* param, RIL_Errno e);

void SapDisconnect(RIL_Token t, RIL_SIM_SAP_DISCONNECT_REQ req,
                   void (*rsp)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_DISCONNECT_RSP rsp))
{
    static SapDisconnectParam param;
    param.t = t;
    param.rsp = rsp;

    SecSapResetSim(&param, SapDisconnectRsp1);
}

static void SapDisconnectRsp1(void* param, RIL_Errno e)
{
    SecSapDisconnect(param, SapDisconnectRsp2);
}

static void SapDisconnectRsp2(void* param, RIL_Errno e)
{
    // response from SecSapDisconnect
    SapDisconnectParam* disconnectParam = (SapDisconnectParam*)param;

    RIL_SIM_SAP_DISCONNECT_RSP rsp = RIL_SIM_SAP_DISCONNECT_RSP_init_zero;

    disconnectParam->rsp(disconnectParam->t, e, rsp);
}

// --------------------------------------------------------------------------
// ATR

typedef struct
{
    RIL_Token t;
    void (*rsp)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_TRANSFER_ATR_RSP rsp);
} SapTransferAtrParam;

static void SapTransferAtrRsp(void* param, RIL_Errno e, int resultCode, void* atr, size_t atrlen);

void SapTransferAtr(RIL_Token t, RIL_SIM_SAP_TRANSFER_ATR_REQ req,
                    void (*rsp)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_TRANSFER_ATR_RSP rsp))
{
    static SapTransferAtrParam param;
    param.t = t;
    param.rsp = rsp;

    SecSapTransferAtr(&param, SapTransferAtrRsp);
}

static void SapTransferAtrRsp(void* param, RIL_Errno e, int resultCode, void* atr, size_t atrlen)
{
    SapTransferAtrParam* atrParam = (SapTransferAtrParam*)param;

    RIL_SIM_SAP_TRANSFER_ATR_RSP rsp = RIL_SIM_SAP_TRANSFER_ATR_RSP_init_zero;
    rsp.response = (RIL_SIM_SAP_TRANSFER_ATR_RSP_Response)resultCode;
    rsp.atr = (pb_bytes_array_t*) malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(atrlen));
    rsp.atr->size = atrlen;
    memcpy(rsp.atr->bytes, atr, atrlen);

    atrParam->rsp(atrParam->t, e, rsp);

    free(rsp.atr);
}

// --------------------------------------------------------------------------
// APDU

typedef struct
{
    RIL_Token t;
    void (*rsp)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_APDU_RSP rsp);
} SapApduParam;

static void SapApduRsp(void* param, RIL_Errno e, int resultCode, void* apdu, size_t apdulen);

void SapApdu(RIL_Token t, RIL_SIM_SAP_APDU_REQ req,
             void (*rsp)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_APDU_RSP rsp))
{
    static SapApduParam param;
    param.t = t;
    param.rsp = rsp;

    SecSapApdu(&param, req.command->bytes, req.command->size, SapApduRsp);
}

static void SapApduRsp(void* param, RIL_Errno e, int resultCode, void* apdu, size_t apdulen)
{
    SapApduParam* apduParam = (SapApduParam*)param;

    RIL_SIM_SAP_APDU_RSP rsp = RIL_SIM_SAP_APDU_RSP_init_zero;
    rsp.type = RIL_SIM_SAP_APDU_RSP_Type_RIL_TYPE_APDU;
    rsp.response = (RIL_SIM_SAP_APDU_RSP_Response)resultCode;
    rsp.apduResponse = (pb_bytes_array_t*) malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(apdulen));
    rsp.apduResponse->size = apdulen;
    memcpy(rsp.apduResponse->bytes, apdu, apdulen);

    apduParam->rsp(apduParam->t, e, rsp);

    free(rsp.apduResponse);
}

// --------------------------------------------------------------------------
// SIM Power

typedef struct
{
    RIL_Token t;
    void (*rsp)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_POWER_RSP rsp);
} SapSimPowerParam;

static void SapSimPowerRsp(void* param, RIL_Errno e);

void SapSimPower(RIL_Token t, RIL_SIM_SAP_POWER_REQ req,
                 void (*rsp)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_POWER_RSP rsp))
{
    static SapSimPowerParam param;
    param.t = t;
    param.rsp = rsp;

    SecSapSimPower(&param, req.state, SapSimPowerRsp);
}

static void SapSimPowerRsp(void* param, RIL_Errno e)
{
    SapSimPowerParam* simPowerParam = (SapSimPowerParam*)param;

    RIL_SIM_SAP_POWER_RSP rsp = RIL_SIM_SAP_POWER_RSP_init_zero;
    rsp.response = RIL_SIM_SAP_POWER_RSP_Response_RIL_E_SUCCESS;

    simPowerParam->rsp(simPowerParam->t, e, rsp);
}

// --------------------------------------------------------------------------
// Reset SIM

typedef struct
{
    RIL_Token t;
    void (*rsp)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_RESET_SIM_RSP rsp);
} SapResetSimParam;

static void SapResetSimRsp(void* param, RIL_Errno e);

void SapResetSim(RIL_Token t, RIL_SIM_SAP_RESET_SIM_REQ req,
                 void (*rsp)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_RESET_SIM_RSP rsp))
{
    static SapResetSimParam param;
    param.t = t;
    param.rsp = rsp;

    SecSapResetSim(&param, SapResetSimRsp);
}

static void SapResetSimRsp(void* param, RIL_Errno e)
{
    SapResetSimParam* resetSimParam = (SapResetSimParam*)param;

    RIL_SIM_SAP_RESET_SIM_RSP rsp = RIL_SIM_SAP_RESET_SIM_RSP_init_zero;
    rsp.response = RIL_SIM_SAP_RESET_SIM_RSP_Response_RIL_E_SUCCESS;

    resetSimParam->rsp(resetSimParam->t, e, rsp);
}
