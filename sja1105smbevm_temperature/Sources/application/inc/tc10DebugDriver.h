/*
 * tc10DebugDriver.h
 *
 *  Created on: 28.08.2018
 *      Author: nxf33016
 */

#ifndef APPLICATION_INC_TC10DEBUGDRIVER_H_
#define APPLICATION_INC_TC10DEBUGDRIVER_H_

#include <stdint.h>

typedef enum {
  dde_sleep_nolink,
  dde_sleep_link,
  dde_sleep,
  dde_wakeup_normal,
  dde_wakeup_sleep,
  dde_isr_wake,
  dde_stb2norm_normal,
  dde_stb2norm_not_normal,
  dde_enable_link,
  dde_disable_link,
  dde_linkup,
  dde_wakeup_from_sleep,
  dde_wakeup_from_other,
} DDE;

typedef struct {
  DDE type;
  uint8_t phy;
  uint16_t extCtrlReg;
  uint16_t commStatReg;
  uint16_t genStatReg;
  uint16_t commConfReg;
} DDS;

extern int readDds(DDS *entry);


#endif /* APPLICATION_INC_TC10DEBUGDRIVER_H_ */
