#define LOG_TAG "sec-sap"
#define RIL_SHLIB

#include "proto/sap-api.pb.h"
#include "proto/pb_encode.h"
#include "proto/pb_decode.h"
#include "secril-sap.h"
#include "sap-functions.h"

#include <telephony/ril.h>
#include <utils/Log.h>

// --------------------------------------------------------------------------

static const struct RIL_Env *g_sapEnv;

static void SapRequestFunc(int request, void *data, size_t datalen, RIL_Token t);

static RIL_RadioFunctions g_radioFunctions =
{
    .version = RIL_VERSION,
    .onRequest = SapRequestFunc,

    // other functions are not used
};

// SAP entry point
const RIL_RadioFunctions *RIL_SAP_Init(const struct RIL_Env *env, int argc, char **argv)
{
    RLOGD("***** RIL_SAP_Init");
    g_sapEnv = env;
    return &g_radioFunctions;
}

// --------------------------------------------------------------------------

typedef enum
{
    Disconnected,
    Connecting,
    Connected,
    Disconnecting
} ConnectionState;

static ConnectionState g_connectionState = Disconnected;

// --------------------------------------------------------------------------

static void SapConnectRsp(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_CONNECT_RSP rsp)
{
    RLOGD("<<<<<    ConnectRsp(%p,%d)", t, e);

    unsigned char response[RIL_SIM_SAP_CONNECT_RSP_size];
    pb_ostream_t stream = pb_ostream_from_buffer(response, sizeof(response));
    pb_encode(&stream, RIL_SIM_SAP_CONNECT_RSP_fields, &rsp);
    g_sapEnv->OnRequestComplete(t, e, response, stream.bytes_written);

    g_connectionState = (e == RIL_E_SUCCESS) ? Connected : Disconnected;
}

static void SapStatusInd(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_STATUS_IND rsp)
{
    RLOGD("<<<<<    StatusInd(%p,%d)", t, e);

    unsigned char response[RIL_SIM_SAP_STATUS_IND_size];
    pb_ostream_t stream = pb_ostream_from_buffer(response, sizeof(response));
    pb_encode(&stream, RIL_SIM_SAP_STATUS_IND_fields, &rsp);
    g_sapEnv->OnUnsolicitedResponse(MsgId_RIL_SIM_SAP_STATUS, response, stream.bytes_written);
}

static void SapDisconnectRsp(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_DISCONNECT_RSP rsp)
{
    RLOGD("<<<<<    DisconnectRsp(%p,%d)", t, e);

    unsigned char response[RIL_SIM_SAP_DISCONNECT_RSP_size];
    pb_ostream_t stream = pb_ostream_from_buffer(response, sizeof(response));
    pb_encode(&stream, RIL_SIM_SAP_DISCONNECT_RSP_fields, &rsp);
    g_sapEnv->OnRequestComplete(t, e, response, stream.bytes_written);

    g_connectionState = Disconnected;
}

static void SapTransferAtrRsp(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_TRANSFER_ATR_RSP rsp)
{
    RLOGD("<<<<<    TransferAtrRsp(%p,%d)", t, e);

    unsigned char response[rsp.atr->size + 20];
    pb_ostream_t stream = pb_ostream_from_buffer(response, sizeof(response));
    pb_encode(&stream, RIL_SIM_SAP_TRANSFER_ATR_RSP_fields, &rsp);
    g_sapEnv->OnRequestComplete(t, e, response, stream.bytes_written);
}

static void SapApduRsp(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_APDU_RSP rsp)
{
    RLOGD("<<<<<    ApduRsp(%p,%d)", t, e);

    unsigned char response[rsp.apduResponse->size + 20];
    pb_ostream_t stream = pb_ostream_from_buffer(response, sizeof(response));
    pb_encode(&stream, RIL_SIM_SAP_APDU_RSP_fields, &rsp);
    g_sapEnv->OnRequestComplete(t, e, response, stream.bytes_written);
}

static void SapSimPowerRsp(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_POWER_RSP rsp)
{
    RLOGD("<<<<<    SimPowerRsp(%p,%d)", t, e);

    unsigned char response[RIL_SIM_SAP_POWER_RSP_size];
    pb_ostream_t stream = pb_ostream_from_buffer(response, sizeof(response));
    pb_encode(&stream, RIL_SIM_SAP_POWER_RSP_fields, &rsp);
    g_sapEnv->OnRequestComplete(t, e, response, stream.bytes_written);
}

static void SapResetSimRsp(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_RESET_SIM_RSP rsp)
{
    RLOGD("<<<<<    ResetSimRsp(%p,%d)", t, e);

    unsigned char response[RIL_SIM_SAP_RESET_SIM_RSP_size];
    pb_ostream_t stream = pb_ostream_from_buffer(response, sizeof(response));
    pb_encode(&stream, RIL_SIM_SAP_RESET_SIM_RSP_fields, &rsp);
    g_sapEnv->OnRequestComplete(t, e, response, stream.bytes_written);
}

static void SapRequestFunc(int request, void *data, size_t datalen, RIL_Token t /*, RIL_SOCKET_ID socket_id*/)
{
    RLOGD(">>>>>    SapRequestFunc(%d,%p,%u,%p)", request, data, datalen, t);

    pb_byte_t* pData = (pb_byte_t*)data;
    switch (request)
    {
        case MsgId_RIL_SIM_SAP_CONNECT:
            {
                if (g_connectionState == Disconnected)
                {
                    RIL_SIM_SAP_CONNECT_REQ req = RIL_SIM_SAP_CONNECT_REQ_init_zero;
                    pb_istream_t stream = pb_istream_from_buffer(pData, datalen);
                    bool status = pb_decode(&stream, RIL_SIM_SAP_CONNECT_REQ_fields, &req);

                    if (status)
                    {
                        g_connectionState = Connecting;
                        SapConnect(t, req, SapConnectRsp, SapStatusInd);
                    }
                    else
                    {
                        RLOGD("+++++ MsgId_RIL_SIM_SAP_CONNECT failed +++++");
                        g_sapEnv->OnRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
                    }
                }
                else
                {
                    RIL_SIM_SAP_CONNECT_RSP rsp = RIL_SIM_SAP_CONNECT_RSP_init_zero;
                    rsp.response = RIL_SIM_SAP_CONNECT_RSP_Response_RIL_E_SUCCESS;
                    rsp.has_max_message_size = false;

                    SapConnectRsp(t, RIL_E_SUCCESS, rsp);
                }
            }
            break;

        case MsgId_RIL_SIM_SAP_DISCONNECT:
            {
                if (g_connectionState == Connected)
                {
                    RIL_SIM_SAP_DISCONNECT_REQ req = RIL_SIM_SAP_DISCONNECT_REQ_init_zero;
                    pb_istream_t stream = pb_istream_from_buffer(pData, datalen);
                    bool status = pb_decode(&stream, RIL_SIM_SAP_DISCONNECT_REQ_fields, &req);

                    if (status)
                    {
                        g_connectionState = Disconnecting;
                        SapDisconnect(t, req, SapDisconnectRsp);
                    }
                    else
                    {
                        RLOGD("+++++ MsgId_RIL_SIM_SAP_DISCONNECT failed +++++");
                        g_sapEnv->OnRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
                    }
                }
                else
                {
                    RIL_SIM_SAP_DISCONNECT_RSP rsp = RIL_SIM_SAP_DISCONNECT_RSP_init_zero;

                    SapDisconnectRsp(t, RIL_E_SUCCESS, rsp);
                }
            }
            break;

        case MsgId_RIL_SIM_SAP_APDU:
            {
                RIL_SIM_SAP_APDU_REQ req = RIL_SIM_SAP_APDU_REQ_init_zero;
                pb_istream_t stream = pb_istream_from_buffer(pData, datalen);
                bool status = pb_decode(&stream, RIL_SIM_SAP_APDU_REQ_fields, &req);

                if (status)
                {
                    SapApdu(t, req, SapApduRsp);
                }
                else
                {
                    RLOGD("+++++ MsgId_RIL_SIM_SAP_APDU failed +++++");
                    g_sapEnv->OnRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
                }
            }
            break;

        case MsgId_RIL_SIM_SAP_TRANSFER_ATR:
            {
                RIL_SIM_SAP_TRANSFER_ATR_REQ req = RIL_SIM_SAP_TRANSFER_ATR_REQ_init_zero;
                pb_istream_t stream = pb_istream_from_buffer(pData, datalen);
                bool status = pb_decode(&stream, RIL_SIM_SAP_TRANSFER_ATR_REQ_fields, &req);

                if (status)
                {
                    SapTransferAtr(t, req, SapTransferAtrRsp);
                }
                else
                {
                    RLOGD("+++++ MsgId_RIL_SIM_SAP_TRANSFER_ATR failed +++++");
                    g_sapEnv->OnRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
                }
            }
            break;

        case MsgId_RIL_SIM_SAP_POWER:
            {
                RIL_SIM_SAP_POWER_REQ req = RIL_SIM_SAP_POWER_REQ_init_zero;
                pb_istream_t stream = pb_istream_from_buffer(pData, datalen);
                bool status = pb_decode(&stream, RIL_SIM_SAP_POWER_REQ_fields, &req);

                if (status)
                {
                    SapSimPower(t, req, SapSimPowerRsp);
                }
                else
                {
                    RLOGD("+++++ MsgId_RIL_SIM_SAP_POWER failed +++++");
                    g_sapEnv->OnRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
                }
            }
            break;

        case MsgId_RIL_SIM_SAP_RESET_SIM:
            {
                RIL_SIM_SAP_RESET_SIM_REQ req = RIL_SIM_SAP_RESET_SIM_REQ_init_zero;
                pb_istream_t stream = pb_istream_from_buffer(pData, datalen);
                bool status = pb_decode(&stream, RIL_SIM_SAP_RESET_SIM_REQ_fields, &req);

                if (status)
                {
                    SapResetSim(t, req, SapResetSimRsp);
                }
                else
                {
                    RLOGD("+++++ MsgId_RIL_SIM_SAP_RESET_SIM failed +++++");
                    g_sapEnv->OnRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
                }
            }
            break;

        case MsgId_RIL_SIM_SAP_TRANSFER_CARD_READER_STATUS:
        case MsgId_RIL_SIM_SAP_ERROR_RESP:
        case MsgId_RIL_SIM_SAP_SET_TRANSFER_PROTOCOL:

        default:
            RLOGD("+++++ not handled +++++");
            g_sapEnv->OnRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
            break;
    }
}
