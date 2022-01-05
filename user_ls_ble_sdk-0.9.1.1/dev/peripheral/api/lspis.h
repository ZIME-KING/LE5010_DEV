#ifndef LSPIS_H_
#define LSPIS_H_
#include <stdbool.h>
#include "reg_pis_type.h"
#include "reg_base_addr.h"
#include "HAL_def.h"
#define LSPIS ((reg_pis_t *)LSPIS_BASE_ADDR)

enum pis_sync_mode
{
    PIS_SYNC_DIRECT,
    PIS_SYNC_SRC_LEVEL,
    PIS_SYNC_SRC_PULSE,
};

enum pis_edge_sel
{
    PIS_EDGE_NONE,
    PIS_POS_EDGE,
    PIS_NEG_EDGE,
    PIS_BOTH_EDGES,
};

HAL_StatusTypeDef HAL_PIS_Init(void);

HAL_StatusTypeDef HAL_PIS_DeInit(void);

HAL_StatusTypeDef HAL_PIS_Config(uint8_t channel,enum pis_src src,enum pis_dst dst,enum pis_sync_mode sync,enum pis_edge_sel edge);

HAL_StatusTypeDef HAL_PIS_Output(uint8_t channel,bool enable);

#endif
