// --------------------------------------------------------------------------
// Call Samsung SAP implementation

#ifndef __SECRIL_SAP_H
#define __SECRIL_SAP_H

#include <telephony/ril.h>
#include <stdbool.h>

struct RIL_Env* GetEnv(const struct RIL_Env *env);
void SetRadioFunctions(const RIL_RadioFunctions* radioFunctions);

void SecSapConnect(void* param, void (*callback)(void* param, RIL_Errno e));
void SecSapDisconnect(void* param, void (*callback)(void* param, RIL_Errno e));
void SecSapResetSim(void* param, void (*callback)(void* param, RIL_Errno e));
void SecSapApdu(void* param, void* apdu, size_t apdusize,
                void (*callback)(void* param, RIL_Errno e, int resultCode, void* apdu, size_t apdulen));
void SecSapTransferAtr(void* param,
                       void (*callback)(void* param, RIL_Errno e, int resultCode, void* apdu, size_t apdulen));
void SecSapSimPower(void* param, bool on, void (*callback)(void* param, RIL_Errno e));

#endif // __SECRIL_SAP_H
