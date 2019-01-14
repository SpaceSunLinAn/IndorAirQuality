/*******************************************************************************
 * Copyright (c) 2018 Integrated Device Technology, Inc.
 * All Rights Reserved.
 *
 * This code is proprietary to IDT, and is license pursuant to the terms and
 * conditions that may be accessed at:
 * https://www.idt.com/document/msc/idt-software-license-terms-gas-sensor-software
 *
 ******************************************************************************/

/**
 * @file    main.h
 * @brief   This is an example for the ZMOD4410 gas sensor module.
 * @version 1.0.2
 * @date    2017-05-15
 * @author  Franziska Naepelt <franziska.naepelt@idt.com>
 */

//#include <conio.h> /* This include is just for Windows PCs. */
#include <stdio.h>

/* Algorithm library header files, download the target specific library
 * from the IDT webpage. */
#include "eco2.h"
#include "iaq.h"
#include "odor.h"
#include "tvoc.h"
#include "r_cda.h"

/* Files needed for hardware access, needs to be adjusted to target. */

/* files to control the sensor */
#include "zmod44xx.h"

// start sequencer defines

#define FIRST_SEQ_STEP      0
#define LAST_SEQ_STEP       1

#define STATUS_LAST_SEQ_STEP_MASK   0x0F
