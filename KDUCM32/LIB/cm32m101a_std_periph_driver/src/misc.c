/*
  ******************************************************************************
  *
  * COPYRIGHT(c) 2020, China Mobile IOT
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of China Mobile IOT nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/**
 * @file misc.c
 * @author CMIOT
 * @version v1.0.0
 *
 * @COPYRIGHT(c) 2020, China Mobile IOT. All rights reserved.
 */
#include "misc.h"

/** @addtogroup cm32m101a_StdPeriph_Driver
 * @{
 */

/** @addtogroup MISC
 * @brief MISC driver modules
 * @{
 */

/** @addtogroup MISC_Private_TypesDefinitions
 * @{
 */

/**
 * @}
 */

/** @addtogroup MISC_Private_Defines
 * @{
 */

#define AIRCR_VECTKEY_MASK ((uint32_t)0x05FA0000)
/**
 * @}
 */

/** @addtogroup MISC_Private_Macros
 * @{
 */

/**
 * @}
 */

/** @addtogroup MISC_Private_Variables
 * @{
 */

/**
 * @}
 */

/** @addtogroup MISC_Private_FunctionPrototypes
 * @{
 */

/**
 * @}
 */

/** @addtogroup MISC_Private_Functions
 * @{
 */

/**
 * @brief  Configures the priority grouping: pre-emption priority and subpriority.
 * @param NVIC_PriorityGroup specifies the priority grouping bits length.
 *   This parameter can be one of the following values:
 *     @arg NVIC_PriorityGroup_0 0 bits for pre-emption priority
 *                                4 bits for subpriority
 *     @arg NVIC_PriorityGroup_1 1 bits for pre-emption priority
 *                                3 bits for subpriority
 *     @arg NVIC_PriorityGroup_2 2 bits for pre-emption priority
 *                                2 bits for subpriority
 *     @arg NVIC_PriorityGroup_3 3 bits for pre-emption priority
 *                                1 bits for subpriority
 *     @arg NVIC_PriorityGroup_4 4 bits for pre-emption priority
 *                                0 bits for subpriority
 */
void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup)
{
    /* Check the parameters */
    assert_param(IS_NVIC_PRIORITY_GROUP(NVIC_PriorityGroup));

    /* Set the PRIGROUP[10:8] bits according to NVIC_PriorityGroup value */
    SCB->AIRCR = AIRCR_VECTKEY_MASK | NVIC_PriorityGroup;
}

/**
 * @brief  Initializes the NVIC peripheral according to the specified
 *         parameters in the NVIC_InitStruct.
 * @param NVIC_InitStruct pointer to a NVIC_InitType structure that contains
 *         the configuration information for the specified NVIC peripheral.
 */
void NVIC_Init(NVIC_InitType* NVIC_InitStruct)
{
    uint32_t tmppriority = 0x00, tmppre = 0x00, tmpsub = 0x0F;

    /* Check the parameters */
    assert_param(IS_FUNCTIONAL_STATE(NVIC_InitStruct->NVIC_IRQChannelCmd));
    assert_param(IS_NVIC_PREEMPTION_PRIORITY(NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority));
    assert_param(IS_NVIC_SUB_PRIORITY(NVIC_InitStruct->NVIC_IRQChannelSubPriority));

    if (NVIC_InitStruct->NVIC_IRQChannelCmd != DISABLE)
    {
        /* Compute the Corresponding IRQ Priority --------------------------------*/
        tmppriority = (0x700 - ((SCB->AIRCR) & (uint32_t)0x700)) >> 0x08;
        tmppre      = (0x4 - tmppriority);
        tmpsub      = tmpsub >> tmppriority;

        tmppriority = (uint32_t)NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority << tmppre;
        tmppriority |= NVIC_InitStruct->NVIC_IRQChannelSubPriority & tmpsub;
        tmppriority = tmppriority << 0x04;

        NVIC->IP[NVIC_InitStruct->NVIC_IRQChannel] = tmppriority;

        /* Enable the Selected IRQ Channels --------------------------------------*/
        NVIC->ISER[NVIC_InitStruct->NVIC_IRQChannel >> 0x05] = (uint32_t)0x01
                                                               << (NVIC_InitStruct->NVIC_IRQChannel & (uint8_t)0x1F);
    }
    else
    {
        /* Disable the Selected IRQ Channels -------------------------------------*/
        NVIC->ICER[NVIC_InitStruct->NVIC_IRQChannel >> 0x05] = (uint32_t)0x01
                                                               << (NVIC_InitStruct->NVIC_IRQChannel & (uint8_t)0x1F);
    }
}

/**
 * @brief  Sets the vector table location and Offset.
 * @param NVIC_VectTab specifies if the vector table is in RAM or FLASH memory.
 *   This parameter can be one of the following values:
 *     @arg NVIC_VectTab_RAM
 *     @arg NVIC_VectTab_FLASH
 * @param Offset Vector Table base offset field. This value must be a multiple
 *         of 0x200.
 */
void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset)
{
    /* Check the parameters */
    assert_param(IS_NVIC_VECTTAB(NVIC_VectTab));
    assert_param(IS_NVIC_OFFSET(Offset));

    SCB->VTOR = NVIC_VectTab | (Offset & (uint32_t)0x1FFFFF80);
}

/**
 * @brief  Selects the condition for the system to enter low power mode.
 * @param LowPowerMode Specifies the new mode for the system to enter low power mode.
 *   This parameter can be one of the following values:
 *     @arg NVIC_LP_SEVONPEND
 *     @arg NVIC_LP_SLEEPDEEP
 *     @arg NVIC_LP_SLEEPONEXIT
 * @param Cmd new state of LP condition. This parameter can be: ENABLE or DISABLE.
 */
void NVIC_SystemLPConfig(uint8_t LowPowerMode, FunctionalState Cmd)
{
    /* Check the parameters */
    assert_param(IS_NVIC_LP(LowPowerMode));
    assert_param(IS_FUNCTIONAL_STATE(Cmd));

    if (Cmd != DISABLE)
    {
        SCB->SCR |= LowPowerMode;
    }
    else
    {
        SCB->SCR &= (uint32_t)(~(uint32_t)LowPowerMode);
    }
}

/**
 * @brief  Configures the SysTick clock source.
 * @param SysTick_CLKSource specifies the SysTick clock source.
 *   This parameter can be one of the following values:
 *     @arg SysTick_CLKSource_HCLK_Div8 AHB clock divided by 8 selected as SysTick clock source.
 *     @arg SysTick_CLKSource_HCLK AHB clock selected as SysTick clock source.
 */
void SysTick_CLKSourceConfig(uint32_t SysTick_CLKSource)
{
    /* Check the parameters */
    assert_param(IS_SYSTICK_CLK_SOURCE(SysTick_CLKSource));
    if (SysTick_CLKSource == SysTick_CLKSource_HCLK)
    {
        SysTick->CTRL |= SysTick_CLKSource_HCLK;
    }
    else
    {
        SysTick->CTRL &= SysTick_CLKSource_HCLK_Div8;
    }
}

#if (__MPU_PRESENT == 1)
/**
  * @brief  Enable the MPU.
  * @param  MPU_Control: Specifies the control mode of the MPU during hard fault, 
  *          NMI, FAULTMASK and privileged accessto the default memory 
  *          This parameter can be one of the following values:
  *            @arg MPU_HFNMI_PRIVDEF_NONE
  *            @arg MPU_HARDFAULT_NMI
  *            @arg MPU_PRIVILEGED_DEFAULT
  *            @arg MPU_HFNMI_PRIVDEF
  * @retval None
  */
void MPU_Enable(uint32_t MPU_Control)
{
/* Ensure MPU setting take effects */
  __DSB();
  __ISB();
  /* Enable the MPU */
  MPU->CTRL = (MPU_Control | MPU_CTRL_ENABLE_Msk);

#ifdef SCB_SHCSR_MEMFAULTENA_Msk
  SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
#endif

}


/**
  * @brief  Disable the MPU.
  * @retval None
  */
void MPU_Disable(void)
{
	__DSB();
  __ISB();
#ifdef SCB_SHCSR_MEMFAULTENA_Msk
  SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk;
#endif
  MPU->CTRL  &= ~MPU_CTRL_ENABLE_Msk;
}


/**
  * @brief  Initialize and configure the Region and the memory to be protected.
  * @param  MPU_Init: Pointer to a MPU_Region_InitTypeDef structure that contains
  *                the initialization and configuration information.
  * @retval None
  */
void MPU_ConfigRegion(MPU_Region_InitTypeDef *MPU_Init)
{
  /* Check the parameters */
  assert_param(IS_MPU_REGION_NUMBER(MPU_Init->Number));
  assert_param(IS_MPU_REGION_ENABLE(MPU_Init->Enable));

  /* Set the Region number */
  MPU->RNR = MPU_Init->Number;

  if ((MPU_Init->Enable) != RESET)
  {
    /* Check the parameters */
    assert_param(IS_MPU_INSTRUCTION_ACCESS(MPU_Init->DisableExec));
    assert_param(IS_MPU_REGION_PERMISSION_ATTRIBUTE(MPU_Init->AccessPermission));
    assert_param(IS_MPU_TEX_LEVEL(MPU_Init->TypeExtField));
    assert_param(IS_MPU_ACCESS_SHAREABLE(MPU_Init->IsShareable));
    assert_param(IS_MPU_ACCESS_CACHEABLE(MPU_Init->IsCacheable));
    assert_param(IS_MPU_ACCESS_BUFFERABLE(MPU_Init->IsBufferable));
    assert_param(IS_MPU_SUB_REGION_DISABLE(MPU_Init->SubRegionDisable));
    assert_param(IS_MPU_REGION_SIZE(MPU_Init->Size));

    MPU->RBAR = MPU_Init->BaseAddress;
    MPU->RASR = ((uint32_t)MPU_Init->DisableExec        << MPU_RASR_XN_Pos)   |
                ((uint32_t)MPU_Init->AccessPermission   << MPU_RASR_AP_Pos)   |
                ((uint32_t)MPU_Init->TypeExtField       << MPU_RASR_TEX_Pos)  |
                ((uint32_t)MPU_Init->IsShareable        << MPU_RASR_S_Pos)    |
                ((uint32_t)MPU_Init->IsCacheable        << MPU_RASR_C_Pos)    |
                ((uint32_t)MPU_Init->IsBufferable       << MPU_RASR_B_Pos)    |
                ((uint32_t)MPU_Init->SubRegionDisable   << MPU_RASR_SRD_Pos)  |
                ((uint32_t)MPU_Init->Size               << MPU_RASR_SIZE_Pos) |
                ((uint32_t)MPU_Init->Enable             << MPU_RASR_ENABLE_Pos);
  }
  else
  {
    MPU->RBAR = 0x00;
    MPU->RASR = 0x00;
  }
}
#endif /* __MPU_PRESENT */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
