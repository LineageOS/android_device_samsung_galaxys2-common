// --------------------------------------------------------------------------
// Implementation of SAP functions based on proto buf types

#ifndef __SAP_FUNCTIONS_H
#define __SAP_FUNCTIONS_H

#include <telephony/ril.h>
#include "proto/sap-api.pb.h"

void SapConnect(RIL_Token t, RIL_SIM_SAP_CONNECT_REQ req,
                void (*connectRsp)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_CONNECT_RSP rsp),
                void (*statusInd)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_STATUS_IND rsp));

void SapDisconnect(RIL_Token t, RIL_SIM_SAP_DISCONNECT_REQ req,
                   void (*rsp)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_DISCONNECT_RSP rsp));

void SapTransferAtr(RIL_Token t, RIL_SIM_SAP_TRANSFER_ATR_REQ req,
                    void (*rsp)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_TRANSFER_ATR_RSP rsp));

void SapApdu(RIL_Token t, RIL_SIM_SAP_APDU_REQ req,
             void (*rsp)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_APDU_RSP rsp));

void SapSimPower(RIL_Token t, RIL_SIM_SAP_POWER_REQ req,
                 void (*rsp)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_POWER_RSP rsp));

void SapResetSim(RIL_Token t, RIL_SIM_SAP_RESET_SIM_REQ req,
                 void (*rsp)(RIL_Token t, RIL_Errno e, RIL_SIM_SAP_RESET_SIM_RSP rsp));

#endif // __SAP_FUNCTIONS_H
