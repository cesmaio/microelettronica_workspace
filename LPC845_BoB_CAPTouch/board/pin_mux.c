/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v5.0
processor: LPC845
package_id: LPC845M301JBD48
mcu_data: ksdk2_0
processor_version: 0.0.14
pin_labels:
- {pin_num: '11', pin_signal: PIO1_0/CAPT_X1, label: GREEN, identifier: GREEN}
- {pin_num: '14', pin_signal: PIO1_1/CAPT_X2, label: BLUE, identifier: BLUE}
- {pin_num: '16', pin_signal: PIO1_2/CAPT_X3, label: RED, identifier: RED}
- {pin_num: '6', pin_signal: PIO0_4/ADC_11, label: USER, identifier: USER}
- {pin_num: '4', pin_signal: PIO0_12, label: USER, identifier: USERB}
- {pin_num: '33', pin_signal: PIO0_7/ADC_0, label: ADC, identifier: ADC}
- {pin_num: '9', pin_signal: PIO0_31/CAPT_X0, label: CAPX, identifier: CAPX}
- {pin_num: '42', pin_signal: PIO0_30/ACMP_I5, label: CAPY, identifier: CAPY}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

#include "fsl_common.h"
#include "fsl_swm.h"
#include "pin_mux.h"

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 *
 * END ****************************************************************************************************************/
void BOARD_InitBootPins(void)
{
    BOARD_InitPins();
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPins:
- options: {callFromInitBoot: 'true', coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '11', peripheral: GPIO, signal: 'PIO1, 0', pin_signal: PIO1_0/CAPT_X1, direction: OUTPUT}
  - {pin_num: '14', peripheral: GPIO, signal: 'PIO1, 1', pin_signal: PIO1_1/CAPT_X2, direction: OUTPUT}
  - {pin_num: '16', peripheral: GPIO, signal: 'PIO1, 2', pin_signal: PIO1_2/CAPT_X3, direction: OUTPUT}
  - {pin_num: '6', peripheral: PINT, signal: 'PINT, 0', pin_signal: PIO0_4/ADC_11}
  - {pin_num: '4', peripheral: PINT, signal: 'PINT, 1', pin_signal: PIO0_12}
  - {pin_num: '33', peripheral: ADC0, signal: 'CH, 0', pin_signal: PIO0_7/ADC_0}
  - {pin_num: '9', peripheral: CAPT, signal: 'CAPTX, 0', pin_signal: PIO0_31/CAPT_X0, mode: inactive}
  - {pin_num: '42', peripheral: ACMP, signal: ACMP_IN5, pin_signal: PIO0_30/ACMP_I5, mode: inactive}
  - {pin_num: '20', peripheral: USART0, signal: RXD, pin_signal: PIO0_24}
  - {pin_num: '19', peripheral: USART0, signal: TXD, pin_signal: PIO0_25}
  - {pin_num: '1', peripheral: CAPT, signal: CAPTYL, pin_signal: PIO1_8/CAPT_YL, mode: inactive}
  - {pin_num: '3', peripheral: CAPT, signal: CAPTYH, pin_signal: PIO1_9/CAPT_YH, mode: inactive}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
/* Function assigned for the Cortex-M0P */
void BOARD_InitPins(void)
{
    /* Enables clock for IOCON.: enable */
    CLOCK_EnableClock(kCLOCK_Iocon);
    /* Enables clock for switch matrix.: enable */
    CLOCK_EnableClock(kCLOCK_Swm);

    GPIO->DIR[1] = ((GPIO->DIR[1] &
                     /* Mask bits to zero which are setting */
                     (~(GPIO_DIR_DIRP_MASK)))

                    /* Selects pin direction for pin PIOm_n (bit 0 = PIOn_0, bit 1 = PIOn_1, etc.). Supported pins
                     * depends on the specific device and package. 0 = input. 1 = output.: 0x07u */
                    | GPIO_DIR_DIRP(0x07u));

    IOCON->PIO[51] = ((IOCON->PIO[51] &
                       /* Mask bits to zero which are setting */
                       (~(IOCON_PIO_MODE_MASK)))

                      /* Selects function mode (on-chip pull-up/pull-down resistor control).: Inactive. Inactive (no
                       * pull-down/pull-up resistor enabled). */
                      | IOCON_PIO_MODE(PIO0_30_MODE_INACTIVE));

    IOCON->PIO[35] = ((IOCON->PIO[35] &
                       /* Mask bits to zero which are setting */
                       (~(IOCON_PIO_MODE_MASK)))

                      /* Selects function mode (on-chip pull-up/pull-down resistor control).: Inactive. Inactive (no
                       * pull-down/pull-up resistor enabled). */
                      | IOCON_PIO_MODE(PIO0_31_MODE_INACTIVE));

    IOCON->PIO[31] = ((IOCON->PIO[31] &
                       /* Mask bits to zero which are setting */
                       (~(IOCON_PIO_MODE_MASK)))

                      /* Selects function mode (on-chip pull-up/pull-down resistor control).: Inactive. Inactive (no
                       * pull-down/pull-up resistor enabled). */
                      | IOCON_PIO_MODE(PIO1_8_MODE_INACTIVE));

    IOCON->PIO[32] = ((IOCON->PIO[32] &
                       /* Mask bits to zero which are setting */
                       (~(IOCON_PIO_MODE_MASK)))

                      /* Selects function mode (on-chip pull-up/pull-down resistor control).: Inactive. Inactive (no
                       * pull-down/pull-up resistor enabled). */
                      | IOCON_PIO_MODE(PIO1_9_MODE_INACTIVE));

    /* USART0_TXD connect to P0_25 */
    SWM_SetMovablePinSelect(SWM0, kSWM_USART0_TXD, kSWM_PortPin_P0_25);

    /* USART0_RXD connect to P0_24 */
    SWM_SetMovablePinSelect(SWM0, kSWM_USART0_RXD, kSWM_PortPin_P0_24);

    /* ACMP_INPUT5 connect to P0_30 */
    SWM_SetFixedPinSelect(SWM0, kSWM_ACMP_INPUT5, true);

    /* ADC_CHN0 connect to P0_7 */
    SWM_SetFixedPinSelect(SWM0, kSWM_ADC_CHN0, true);

    /* CAPT_X0 connect to P0_31 */
    SWM_SetFixedPinSelect(SWM0, kSWM_CAPT_X0, true);

    /* CAPT_YL connect to P1_8 */
    SWM_SetFixedPinSelect(SWM0, kSWM_CAPT_YL, true);

    /* CAPT_YH connect to P1_9 */
    SWM_SetFixedPinSelect(SWM0, kSWM_CAPT_YH, true);

    SYSCON->PINTSEL[0] = ((SYSCON->PINTSEL[0] &
                           /* Mask bits to zero which are setting */
                           (~(SYSCON_PINTSEL_INTPIN_MASK)))

                          /* Pin number select for pin interrupt or pattern match engine input. (PIO0_0 to
                           * PIO0_31correspond to numbers 0 to 31 and PIO1_0 to PIO1_31 correspond to numbers 32 to
                           * 63).: 0x04u */
                          | SYSCON_PINTSEL_INTPIN(0x04u));

    SYSCON->PINTSEL[1] = ((SYSCON->PINTSEL[1] &
                           /* Mask bits to zero which are setting */
                           (~(SYSCON_PINTSEL_INTPIN_MASK)))

                          /* Pin number select for pin interrupt or pattern match engine input. (PIO0_0 to
                           * PIO0_31correspond to numbers 0 to 31 and PIO1_0 to PIO1_31 correspond to numbers 32 to
                           * 63).: 0x0Cu */
                          | SYSCON_PINTSEL_INTPIN(0x0Cu));

    /* Disable clock for switch matrix. */
    CLOCK_DisableClock(kCLOCK_Swm);
}
/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/