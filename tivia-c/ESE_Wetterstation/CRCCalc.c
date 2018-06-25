/*
 *  ======== CRCCalc.c ========
 *  Author: Eva Gergely
 *
 */
#include <stdbool.h>
#include <stdint.h>
#include <inc/hw_memmap.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* Driverlib headers */
#include <driverlib/gpio.h>

/* Board Header files */
#include <Board.h>
#include <EK_TM4C1294XL.h>

/* Application headers */
#include <local_inc/Poll_Task.h>

/* Schieberegister := 0000… (Startwert)
 * solange Bits im Datenstrom verbleiben:
 * falls  das am weitesten links stehende Bit vom Schieberegister
 * ungleich dem nächsten Bit (z. B. beginnend mit dem MSB, und dann jeweils eins weiter rechts pro Iteration) aus dem Datenstrom ist:
 * Schieberegister := (Schieberegister linksschieben um 1, rechtes Bit 0)
 * or CRC-Polynom (ohne den Term vom Grad n+1)
 * andernfalls:
 * Schieberegister := Schieberegister linksschieben um 1, rechtes Bit 0
 * nächstes Bit im Datenstrom
 * Schieberegister enthält das Ergebnis.
 */





