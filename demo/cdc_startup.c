/* This file is the part of the Lightweight USB device Stack for STM32 microcontrollers
 *
 * Copyright ©2016 Dmitry Filimonchuk <dmitrystu[at]gmail[dot]com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "stm32.h"

static void cdc_init_rcc (void) {
#if defined(STM32L0)
    _BST(RCC->APB1ENR, RCC_APB1ENR_PWREN);
    _BMD(PWR->CR, PWR_CR_VOS, PWR_CR_VOS_0);
    _WBC(PWR->CSR, PWR_CSR_VOSF);
    /* set FLASH latency to 1 */
    _BST(FLASH->ACR, FLASH_ACR_LATENCY);
    /* set clock at 32MHz PLL 6/3 HSI */
    _BMD(RCC->CFGR, RCC_CFGR_PLLDIV | RCC_CFGR_PLLMUL | RCC_CFGR_PLLSRC, RCC_CFGR_PLLDIV3 | RCC_CFGR_PLLMUL6);
    _BST(RCC->CR, RCC_CR_HSION);
    _WBS(RCC->CR, RCC_CR_HSIRDY);
    _BST(RCC->CR, RCC_CR_PLLON);
    _WBS(RCC->CR, RCC_CR_PLLRDY);
    /* switch clock to PLL */
    _BMD(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    _WVL(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_PLL);

#elif defined(STM32F0)
    _BST(RCC->APB1ENR, RCC_APB1ENR_CRSEN); /* Clock Recovery System interface clock enabled */
    _BCL(RCC->CFGR3, RCC_CFGR3_USBSW); /*HSI48 clock selected as USB clock source */
    _BST(RCC->CR2, RCC_CR2_HSI48ON); /* HSI48 oscillator ON */
    _WBS(RCC->CR2, RCC_CR2_HSI48RDY); /* HSI48 oscillator ready */
    _BST(FLASH->ACR, FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY); /* Prefetch buffer enable, One wait state */
    _BMD(CRS->CFGR, CRS_CFGR_SYNCSRC,CRS_CFGR_SYNCSRC_1); /* USB SOF selected as SYNC signal source */
    _BST(CRS->CR,CRS_CR_AUTOTRIMEN|CRS_CR_CEN); /* Automatic trimming enabled, Frequency error counter enabled */
    _BST(RCC->CFGR, RCC_CFGR_SW);  /* HSI48 selected as system clock */

#elif defined(STM32L1)
    _BST(RCC->APB1ENR, RCC_APB1ENR_PWREN);
    _BMD(PWR->CR, PWR_CR_VOS, PWR_CR_VOS_0);
    _WBC(PWR->CSR, PWR_CSR_VOSF);
    /* set FLASH latency to 1 */
    _BST(FLASH->ACR, FLASH_ACR_ACC64);
    _BST(FLASH->ACR, FLASH_ACR_LATENCY);
    /* set clock at 32 MHz PLL 6/3 HSI */
    _BMD(RCC->CFGR, RCC_CFGR_PLLDIV | RCC_CFGR_PLLMUL | RCC_CFGR_PLLSRC, RCC_CFGR_PLLDIV3 | RCC_CFGR_PLLMUL6);
    _BST(RCC->CR, RCC_CR_HSION);
    _WBS(RCC->CR, RCC_CR_HSIRDY);
    _BST(RCC->CR, RCC_CR_PLLON);
    _WBS(RCC->CR, RCC_CR_PLLRDY);
    /* switch clock to PLL */
    _BMD(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    _WVL(RCC->CFGR, RCC_CFGR_SWS, RCC_CFGR_SWS_PLL);

#elif defined(STM32L4)
    _BST(RCC->APB1ENR1, RCC_APB1ENR1_PWREN);
    /* Set power Range 1 */
    _BMD(PWR->CR1, PWR_CR1_VOS, PWR_CR1_VOS_0);
    _WBC(PWR->SR2, PWR_SR2_VOSF);
    /* Adjust Flash latency */
    _BMD(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2WS);
    /* set clock 48Mhz MSI */
    _BMD(RCC->CR, RCC_CR_MSIRANGE, RCC_CR_MSIRANGE_11 | RCC_CR_MSIRGSEL);
    /* set MSI as 48MHz USB */
    _BMD(RCC->CCIPR, RCC_CCIPR_CLK48SEL, RCC_CCIPR_CLK48SEL_0 | RCC_CCIPR_CLK48SEL_1);
    /* enable GPIOA clock */
    _BST(RCC->AHB2ENR, RCC_AHB2ENR_GPIOAEN);
    /* set GP11 and GP12 as USB data pins AF10 */
    _BST(GPIOA->AFR[1], (0x0A << 12) | (0x0A << 16));
    _BMD(GPIOA->MODER, (0x03 << 22) | (0x03 << 24), (0x02 << 22) | (0x02 << 24));
#else
    #error Not supported
#endif
}

void __libc_init_array(void) {

}

void SystemInit(void) {
    cdc_init_rcc();
}
