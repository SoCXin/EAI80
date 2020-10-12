/**
  ******************************************************************************
  * @file    stm32f4xx_ll_usb.c
  * @author  MCD Application Team
  * @version V1.7.1
  * @date    14-April-2017
  * @brief   USB Low Layer HAL module driver.
  *    
  *          This file provides firmware functions to manage the following 
  *          functionalities of the USB Peripheral Controller:
  *           + Initialization/de-initialization functions
  *           + I/O operation functions
  *           + Peripheral Control functions 
  *           + Peripheral State functions
  *         
  @verbatim
  ==============================================================================
                    ##### How to use this driver #####
  ==============================================================================
    [..]
      (#) Fill parameters of Init structure in USB_OTG_CfgTypeDef structure.
  
      (#) Call USB_CoreInit() API to initialize the USB Core peripheral.

      (#) The upper HAL HCD/PCD driver will call the right routines for its internal processes.

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
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

/* Includes ------------------------------------------------------------------*/

/** @addtogroup STM32F4xx_LL_USB_DRIVER
  * @{
  */

#if 1
#if 1

#include "gm_ll_usb.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static HAL_StatusTypeDef USB_CoreReset(USB_OTG_GlobalTypeDef *USBx);

/* Exported functions --------------------------------------------------------*/

/** @defgroup LL_USB_Exported_Functions USB Low Layer Exported Functions
  * @{
  */

/** @defgroup LL_USB_Group1 Initialization/de-initialization functions 
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
 ===============================================================================
              ##### Initialization/de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
 
@endverbatim
  * @{
  */



/*Send data out*/
#define MAX_EP_LENGTH 64   /*by wizephen*/
#define MAX_EP0_LENGTH 64   /*by wizephen*/


void USB_SetDynamicSize(USB_OTG_GlobalTypeDef *USBx);
                

/**
  * @brief  Initializes the USB Core
  * @param  USBx: USB Instance
  * @param  cfg : pointer to a USB_OTG_CfgTypeDef structure that contains
  *         the configuration information for the specified USBx peripheral.
  * @retval HAL status
  */
HAL_StatusTypeDef USB_CoreInit(USB_OTG_GlobalTypeDef *USBx, USB_OTG_CfgTypeDef cfg)
{
#if !defined(CHIP_GM6721)
  if (cfg.phy_itface == USB_OTG_ULPI_PHY)
  {
    
    USBx->GCCFG &= ~(USB_OTG_GCCFG_PWRDWN);

    /* Init The ULPI Interface */
    USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_TSDPS | USB_OTG_GUSBCFG_ULPIFSLS | USB_OTG_GUSBCFG_PHYSEL);
   
    /* Select vbus source */
    USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD | USB_OTG_GUSBCFG_ULPIEVBUSI);
    if(cfg.use_external_vbus == 1U)
    {
      USBx->GUSBCFG |= USB_OTG_GUSBCFG_ULPIEVBUSD;
    }
    /* Reset after a PHY select  */
    USB_CoreReset(USBx); 
  }
  else /* FS interface (embedded Phy) */
  {
    /* Select FS Embedded PHY */
    USBx->GUSBCFG |= USB_OTG_GUSBCFG_PHYSEL;
    
    /* Reset after a PHY select and set Host mode */
    USB_CoreReset(USBx);
    
    /* Deactivate the power down*/
    USBx->GCCFG = USB_OTG_GCCFG_PWRDWN;
  }
 
  if(cfg.dma_enable == ENABLE)
  {
    USBx->GAHBCFG |= USB_OTG_GAHBCFG_HBSTLEN_2;
    USBx->GAHBCFG |= USB_OTG_GAHBCFG_DMAEN;
  } 
#else  
  if (cfg.phy_itface == USB_OTG_ULPI_PHY)
  {
     /*Do nothing here!!!*/
  }
  else
  {
    /* Select FS Embedded PHY */    
    
    /* Reset after a PHY select and set Host mode */
    USB_CoreReset(USBx);
    
    /* Deactivate the power down*/    
  }

  if (cfg.dma_enable == ENABLE)
  {
     /*DMA enabled is dedicatedly for Tx or Rx Enpoints, should not enable here !!!*/
  }
#endif

  return HAL_OK;
}

/**
  * @brief  USB_EnableGlobalInt
  *         Enables the controller's Global Int in the AHB Config reg
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef USB_EnableGlobalInt(USB_OTG_GlobalTypeDef *USBx)
{
#if !defined(CHIP_GM6721)
  USBx->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
#else
  /*Interrupts for USB Device or Host, now only implement that for Device*/
  USBx->IntrUSBE |= (USB_OTG_INTRUSBE_SUSPEND | USB_OTG_INTRUSBE_RESUME | USB_OTG_INTRUSBE_RSTBABL |\
                     USB_OTG_INTRUSBE_SOF | USB_OTG_INTRUSBE_CON | USB_OTG_INTRUSBE_DISCON |\
                     USB_OTG_INTRUSBE_SESSION | USB_OTG_INTRUSBE_VBUSERR);
#endif
  return HAL_OK;
}


/**
  * @brief  USB_DisableGlobalInt
  *         Disable the controller's Global Int in the AHB Config reg
  * @param  USBx : Selected device
  * @retval HAL status
*/
HAL_StatusTypeDef USB_DisableGlobalInt(USB_OTG_GlobalTypeDef *USBx)
{
#if !defined(CHIP_GM6721)
  USBx->GAHBCFG &= ~USB_OTG_GAHBCFG_GINT;
#else
  /*Interrupts for USB Device or Host, now only implement that for Device*/
  USBx->IntrUSBE &= ~(USB_OTG_INTRUSBE_SUSPEND | USB_OTG_INTRUSBE_RESUME | USB_OTG_INTRUSBE_RSTBABL |\
                      USB_OTG_INTRUSBE_SOF | USB_OTG_INTRUSBE_CON | USB_OTG_INTRUSBE_DISCON |\
                      USB_OTG_INTRUSBE_SESSION | USB_OTG_INTRUSBE_VBUSERR);
#endif
  return HAL_OK;
}
   
/**
  * @brief  USB_SetCurrentMode : Set functional mode
  * @param  USBx : Selected device
  * @param  mode :  current core mode
  *          This parameter can be one of these values:
  *            @arg USB_OTG_DEVICE_MODE: Peripheral mode
  *            @arg USB_OTG_HOST_MODE: Host mode
  *            @arg USB_OTG_DRD_MODE: Dual Role Device mode  
  * @retval HAL status
  */
HAL_StatusTypeDef USB_SetCurrentMode(USB_OTG_GlobalTypeDef *USBx , USB_OTG_ModeTypeDef mode)
{
#if !defined(CHIP_GM6721)
  USBx->GUSBCFG &= ~(USB_OTG_GUSBCFG_FHMOD | USB_OTG_GUSBCFG_FDMOD); 
  
  if ( mode == USB_OTG_HOST_MODE)
  {
    USBx->GUSBCFG |= USB_OTG_GUSBCFG_FHMOD; 
  }
  else if ( mode == USB_OTG_DEVICE_MODE)
  {
    USBx->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD; 
  }
//  HAL_Delay(50U);
  SysTick_DelayMS(50U);
#else
    if ( mode == USB_OTG_HOST_MODE)
    {
        USBx->DevCtl |= USB_OTG_DEVCTL_HOSTMODE;
    }
    else if (mode == USB_OTG_DEVICE_MODE)
    {
        USBx->DevCtl &= ~(USB_OTG_DEVCTL_HOSTMODE);
    }
    /*Maybe some time-delay here*/    
#endif
  return HAL_OK;
}



/**
  * @brief  USB_DevInit : Initializes the USB_OTG controller registers 
  *         for device mode
  * @param  USBx : Selected device
  * @param  cfg  : pointer to a USB_OTG_CfgTypeDef structure that contains
  *         the configuration information for the specified USBx peripheral.
  * @retval HAL status
  */
HAL_StatusTypeDef USB_DevInit (USB_OTG_GlobalTypeDef *USBx, USB_OTG_CfgTypeDef cfg)
{
#if !defined(CHIP_GM6721)
  uint32_t i = 0U;

  /*Activate VBUS Sensing B */
#if defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || \
    defined(STM32F412Rx) || defined(STM32F412Cx) || defined(STM32F413xx) || defined(STM32F423xx)
  USBx->GCCFG |= USB_OTG_GCCFG_VBDEN;
  
  if (cfg.vbus_sensing_enable == 0U)
  {
    /* Deactivate VBUS Sensing B */
    USBx->GCCFG &= ~USB_OTG_GCCFG_VBDEN;
    
    /* B-peripheral session valid override enable*/ 
    USBx->GOTGCTL |= USB_OTG_GOTGCTL_BVALOEN;
    USBx->GOTGCTL |= USB_OTG_GOTGCTL_BVALOVAL;
  }
#else
  if (cfg.vbus_sensing_enable == 0U)
  {
    USBx->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
  }
  else
  {
    /* Enable VBUS */
    USBx->GCCFG |= USB_OTG_GCCFG_VBUSBSEN;
  }
#endif /* STM32F446xx || STM32F469xx || STM32F479xx || STM32F412Zx || STM32F412Rx || STM32F412Vx || STM32F412Cx || STM32F413xx || STM32F423xx  */
  
  /* Restart the Phy Clock */
  USBx_PCGCCTL = 0U;
  
  /* Device mode configuration */
  USBx_DEVICE->DCFG |= DCFG_FRAME_INTERVAL_80;
  
  if(cfg.phy_itface  == USB_OTG_ULPI_PHY)
  {
    if(cfg.speed == USB_OTG_SPEED_HIGH)
    {      
      /* Set High speed phy */
      USB_SetDevSpeed (USBx , USB_OTG_SPEED_HIGH);
    }
    else 
    {
      /* set High speed phy in Full speed mode */
      USB_SetDevSpeed (USBx , USB_OTG_SPEED_HIGH_IN_FULL);
    }
  }
  else
  {
    /* Set Full speed phy */
    USB_SetDevSpeed (USBx , USB_OTG_SPEED_FULL);
  }

  /* Flush the FIFOs */
  USB_FlushTxFifo(USBx , 0x10U); /* all Tx FIFOs */
  USB_FlushRxFifo(USBx);
  
  /* Clear all pending Device Interrupts */
  USBx_DEVICE->DIEPMSK = 0U;
  USBx_DEVICE->DOEPMSK = 0U;
  USBx_DEVICE->DAINT = 0xFFFFFFFFU;
  USBx_DEVICE->DAINTMSK = 0U;
  
  for (i = 0U; i < cfg.dev_endpoints; i++)
  {
    if ((USBx_INEP(i)->DIEPCTL & USB_OTG_DIEPCTL_EPENA) == USB_OTG_DIEPCTL_EPENA)
    {
      USBx_INEP(i)->DIEPCTL = (USB_OTG_DIEPCTL_EPDIS | USB_OTG_DIEPCTL_SNAK);
    }
    else
    {
      USBx_INEP(i)->DIEPCTL = 0U;
    }
    
    USBx_INEP(i)->DIEPTSIZ = 0U;
    USBx_INEP(i)->DIEPINT  = 0xFFU;
  }
  
  for (i = 0U; i < cfg.dev_endpoints; i++)
  {
    if ((USBx_OUTEP(i)->DOEPCTL & USB_OTG_DOEPCTL_EPENA) == USB_OTG_DOEPCTL_EPENA)
    {
      USBx_OUTEP(i)->DOEPCTL = (USB_OTG_DOEPCTL_EPDIS | USB_OTG_DOEPCTL_SNAK);
    }
    else
    {
      USBx_OUTEP(i)->DOEPCTL = 0U;
    }
    
    USBx_OUTEP(i)->DOEPTSIZ = 0U;
    USBx_OUTEP(i)->DOEPINT  = 0xFFU;
  }
  
  USBx_DEVICE->DIEPMSK &= ~(USB_OTG_DIEPMSK_TXFURM);
  
  if (cfg.dma_enable == 1U)
  {
    /*Set threshold parameters */
    USBx_DEVICE->DTHRCTL = (USB_OTG_DTHRCTL_TXTHRLEN_6 | USB_OTG_DTHRCTL_RXTHRLEN_6);
    USBx_DEVICE->DTHRCTL |= (USB_OTG_DTHRCTL_RXTHREN | USB_OTG_DTHRCTL_ISOTHREN | USB_OTG_DTHRCTL_NONISOTHREN);
    
    i= USBx_DEVICE->DTHRCTL;
  }
  
  /* Disable all interrupts. */
  USBx->GINTMSK = 0U;
  
  /* Clear any pending interrupts */
  USBx->GINTSTS = 0xBFFFFFFFU;

  /* Enable the common interrupts */
  if (cfg.dma_enable == DISABLE)
  {
    USBx->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM; 
  }
  
  /* Enable interrupts matching to the Device mode ONLY */
  USBx->GINTMSK |= (USB_OTG_GINTMSK_USBSUSPM | USB_OTG_GINTMSK_USBRST |\
                    USB_OTG_GINTMSK_ENUMDNEM | USB_OTG_GINTMSK_IEPINT |\
                    USB_OTG_GINTMSK_OEPINT   | USB_OTG_GINTMSK_IISOIXFRM|\
                    USB_OTG_GINTMSK_PXFRM_IISOOXFRM | USB_OTG_GINTMSK_WUIM);
  
  if(cfg.Sof_enable)
  {
    USBx->GINTMSK |= USB_OTG_GINTMSK_SOFM;
  }

  if (cfg.vbus_sensing_enable == ENABLE)
  {
    USBx->GINTMSK |= (USB_OTG_GINTMSK_SRQIM | USB_OTG_GINTMSK_OTGINT); 
  }
#else

  uint8_t tmp = 0;
  uint8_t i = 0;
  if (cfg.vbus_sensing_enable == 0U)
  {
  }
  else
  {   
  }


  /* Restart the Phy Clock */
  //USBx_PCGCCTL = 0U;
  
  /* Device mode configuration */
  //USBx_DEVICE->DCFG |= DCFG_FRAME_INTERVAL_80;
  
  if(cfg.phy_itface  == USB_OTG_ULPI_PHY)
  {
    if(cfg.speed == USB_OTG_SPEED_HIGH)
    {      
      /* Set High speed phy */
      //USB_SetDevSpeed (USBx , USB_OTG_SPEED_HIGH);
    }
    else 
    {
      /* set High speed phy in Full speed mode */
      //USB_SetDevSpeed (USBx , USB_OTG_SPEED_HIGH_IN_FULL);
    }
  }
  else
  {
    /* Set Full speed phy */
    //USB_SetDevSpeed (USBx , USB_OTG_SPEED_FULL);
  }

  /* Flush the FIFOs */
  USB_FlushTxFifo(USBx, 0x10U); /* all Tx FIFOs */
  USB_FlushRxFifo(USBx, 0x10U); /* all Rx FIFOs */  

  /* Clear all pending Device Interrupts */  
  tmp = USBx->IntrTx1; /*read to clear all pending interrupts for Tx endpoints*/
  tmp = USBx->IntrTx2;
  
  tmp = USBx->IntrRx1; /*read to clear all pending interrupts for Rx endpoints*/
  tmp = USBx->IntrRx2;
  tmp = USBx->IntrUSB; /*read to clear all pending interrupts for USB*/

  /*Disable each individual interrupt for Tx endpoints*/    
  USBx->InTrTx1E &= 0x00;
  USBx->InTrTx2E &= 0x00;

  /*Disable each individual interrupt for Rx endpoints*/
  USBx->InTrRx1E &= 0x00;
  USBx->InTrRx2E &= 0x00;  

  /*Set dynamic FIFO*/
  USB_SetDynamicSize(USBx);

  /*And any other pending interrupt ???*/
   
  /*DMA enable only fo each Endpoint, not implement here for the time being*/

#if 0
  /*Enable endpoint interrupt*/
  for (i = 0U; i < cfg.dev_endpoints; i++)
  {
      if (i > 7)
      {
          USBx->InTrTx2E |= 1 << (i - 7);
      }
      else
      {
         USBx->InTrTx1E |= 1 << i;
      }
  }
  
  /*Enable endpoint interrupt*/
  for (i = 0U; i < cfg.dev_endpoints; i++)
  {
      if (i > 7)
      {
          USBx->InTrRx2E |= 1 << (i - 7);
      }
      else
      {
         USBx->InTrRx1E |= 1 << i; /*it matters nothing while you're writing bit0*/
      }            
  }  

  /*Any other enable interrupt ???*/
  USBx->IntrUSBE = 0xFF;  
#endif  
  
  if(cfg.Sof_enable)
  {
 
  }

  if (cfg.vbus_sensing_enable == ENABLE)
  {
 
  }
#endif
  return HAL_OK;
}

#if defined(CHIP_GM6721)
#if 1
void USB_DoubleFlushTxFifo(USB_OTG_GlobalTypeDef *USBx)
{
    /*In case of double fifo is used*/
    USBx_DEVICE->CtrlSts0_Union.TxCSR1 |= USB_OTG_TXCSR1_FLUSHFIFO;
    USBx_DEVICE->CtrlSts0_Union.TxCSR1 |= USB_OTG_TXCSR1_FLUSHFIFO;
	
#if 0    	
    //printf("0x%08x \n", ((uint32_t )USBx + USB_OTG_DEVICE_BASE));
    /*just have a test*/
    USBx_DEVICE->TxMaxP = 0x40;
    USBx_DEVICE->CtrlSts0_Union.TxCSR1 |= 1;	
#endif	
}

void USB_DoubleFlushRxFifo(USB_OTG_GlobalTypeDef *USBx)
{
    /*In case of double fifo is used*/
    USBx_DEVICE->RxCSR1 |= USB_OTG_RXCSR1_FLUSHFIFO;
    USBx_DEVICE->RxCSR1 |= USB_OTG_RXCSR1_FLUSHFIFO;
}
#endif
#endif


/**
  * @brief  USB_OTG_FlushTxFifo : Flush a Tx FIFO
  * @param  USBx : Selected device
  * @param  num : FIFO number
  *         This parameter can be a value from 1 to 15
            16 means Flush all Tx FIFOs
  * @retval HAL status
  */
HAL_StatusTypeDef USB_FlushTxFifo (USB_OTG_GlobalTypeDef *USBx, uint32_t num )
{
#if !defined(CHIP_GM6721)

  uint32_t count = 0;
  
  USBx->GRSTCTL = ( USB_OTG_GRSTCTL_TXFFLSH |(uint32_t)( num << 6)); 
  
  do
  {
    if (++count > 200000)
    {
      return HAL_TIMEOUT;
    }
  }
  while ((USBx->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH) == USB_OTG_GRSTCTL_TXFFLSH);
#else
  uint8_t ep = 1;

  /*Flush the FIFO of Tx Endpoints, where num is the numeric of endpoint*/
  if (num == 16)
  {
      /*Only for Tx Endpoint, not for Endpoint 0*/
      for (ep = 1; ep < num; ep ++)
      {
         USBx->Index = ep;
         USB_DoubleFlushTxFifo(USBx);
      }

      /*Finally flush the endpoint 0 FIFO, but it has no effect while TxPktRdy or RxPktRdy  is not set*/
      USBx->Index = 0;
      USBx_DEVICE->CtrlSts1_Union.CSR02 = USB_OTG_CSR02_FLUSHFIFO;
  }
  else if (num != 0)
  {
      /*Indexed for a dedicated Endpoint*/
      USBx->Index = num;
      USB_DoubleFlushTxFifo(USBx);
  }
  else if (num == 0)
  {
      USBx->Index = num;
      USBx_DEVICE->CtrlSts1_Union.CSR02 = USB_OTG_CSR02_FLUSHFIFO;
  }
#endif
  
  return HAL_OK;
}


/**
  * @brief  USB_FlushRxFifo : Flush Rx FIFO
  * @param  USBx : Selected device
  * @retval HAL status
  */

#if !defined(CHIP_GM6721)  
HAL_StatusTypeDef USB_FlushRxFifo(USB_OTG_GlobalTypeDef *USBx)
{
  uint32_t count = 0;
  
  USBx->GRSTCTL = USB_OTG_GRSTCTL_RXFFLSH;
  
  do
  {
    if (++count > 200000)
    {
      return HAL_TIMEOUT;
    }
  }
  while ((USBx->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH) == USB_OTG_GRSTCTL_RXFFLSH);
  return HAL_OK;
}

#else
/**
  * @brief  USB_OTG_FlushRxFifo : Flush a Rx FIFO
  * @param  USBx : Selected device
  * @param  num : FIFO number
  *         This parameter can be a value from 1 to 15
            16 means Flush all Tx FIFOs
  * @retval HAL status
  */
HAL_StatusTypeDef USB_FlushRxFifo(USB_OTG_GlobalTypeDef *USBx, uint32_t num)
{
  uint8_t ep = 1;

  /*Flush the FIFO of Rx Endpoints, where num is the numeric of endpoint*/
  if (num == 16)
  {
      /*Only for Rx Endpoint, not for Endpoint 0*/
      for (ep = 1; ep < num; ep ++)
      {
         USBx->Index = ep; 
         USB_DoubleFlushRxFifo(USBx);
      }

      /*Finally flush the endpoint 0 FIFO, but it has no effect while TxPktRdy or RxPktRdy  is not set*/
      USBx->Index = 0;
      USBx_DEVICE->CtrlSts1_Union.CSR02 = USB_OTG_CSR02_FLUSHFIFO;
  }
  else if (num != 0)
  {
      /*Indexed for a dedicated Endpoint*/
      USBx->Index = num;
      USB_DoubleFlushRxFifo(USBx);
  }
  else if (num == 0)
  {
      USBx->Index = num;
      USBx_DEVICE->CtrlSts1_Union.CSR02 = USB_OTG_CSR02_FLUSHFIFO;
  }
}

#endif

/**
  * @brief  USB_SetDevSpeed :Initializes the DevSpd field of DCFG register 
  *         depending the PHY type and the enumeration speed of the device.
  * @param  USBx : Selected device
  * @param  speed : device speed
  *          This parameter can be one of these values:
  *            @arg USB_OTG_SPEED_HIGH: High speed mode
  *            @arg USB_OTG_SPEED_HIGH_IN_FULL: High speed core in Full Speed mode
  *            @arg USB_OTG_SPEED_FULL: Full speed mode
  *            @arg USB_OTG_SPEED_LOW: Low speed mode
  * @retval  Hal status
  */
HAL_StatusTypeDef USB_SetDevSpeed(USB_OTG_GlobalTypeDef *USBx , uint8_t speed)
{
#if !defined(CHIP_GM6721)
  USBx_DEVICE->DCFG |= speed;
#else
  /*Nothing to set about USB speed*/
#endif
  return HAL_OK;
}

/**
  * @brief  USB_GetDevSpeed :Return the  Dev Speed 
  * @param  USBx : Selected device
  * @retval speed : device speed
  *          This parameter can be one of these values:
  *            @arg USB_OTG_SPEED_HIGH: High speed mode
  *            @arg USB_OTG_SPEED_FULL: Full speed mode
  *            @arg USB_OTG_SPEED_LOW: Low speed mode
  */
uint8_t USB_GetDevSpeed(USB_OTG_GlobalTypeDef *USBx)
{
  uint8_t speed = 0U;
  
 #if !defined(CHIP_GM6721) 
  if((USBx_DEVICE->DSTS & USB_OTG_DSTS_ENUMSPD) == DSTS_ENUMSPD_HS_PHY_30MHZ_OR_60MHZ)
  {
    speed = USB_OTG_SPEED_HIGH;
  }
  else if (((USBx_DEVICE->DSTS & USB_OTG_DSTS_ENUMSPD) == DSTS_ENUMSPD_FS_PHY_30MHZ_OR_60MHZ)||
           ((USBx_DEVICE->DSTS & USB_OTG_DSTS_ENUMSPD) == DSTS_ENUMSPD_FS_PHY_48MHZ))
  {
    speed = USB_OTG_SPEED_FULL;
  }
  else if((USBx_DEVICE->DSTS & USB_OTG_DSTS_ENUMSPD) == DSTS_ENUMSPD_LS_PHY_6MHZ)
  {
    speed = USB_OTG_SPEED_LOW;
  }
#else

  /*Assume the USB is FULL speed feature*/
  speed = USB_OTG_SPEED_FULL;
#endif
  return speed;
}

/**
  * @brief  Activate and configure an endpoint
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @retval HAL status
  */
HAL_StatusTypeDef USB_ActivateEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep)
{
#if !defined(CHIP_GM6721)

  if (ep->is_in == 1U)
  {
   USBx_DEVICE->DAINTMSK |= USB_OTG_DAINTMSK_IEPM & ((1U << (ep->num)));
   
    if (((USBx_INEP(ep->num)->DIEPCTL) & USB_OTG_DIEPCTL_USBAEP) == 0U)
    {
      USBx_INEP(ep->num)->DIEPCTL |= ((ep->maxpacket & USB_OTG_DIEPCTL_MPSIZ ) | (ep->type << 18U) |\
        ((ep->num) << 22U) | (USB_OTG_DIEPCTL_SD0PID_SEVNFRM) | (USB_OTG_DIEPCTL_USBAEP)); 
    } 
  }
  else
  {
     USBx_DEVICE->DAINTMSK |= USB_OTG_DAINTMSK_OEPM & ((1U << (ep->num)) << 16U);
     
    if (((USBx_OUTEP(ep->num)->DOEPCTL) & USB_OTG_DOEPCTL_USBAEP) == 0U)
    {
      USBx_OUTEP(ep->num)->DOEPCTL |= ((ep->maxpacket & USB_OTG_DOEPCTL_MPSIZ ) | (ep->type << 18U) |\
       (USB_OTG_DIEPCTL_SD0PID_SEVNFRM)| (USB_OTG_DOEPCTL_USBAEP));
    } 
  }
#else
    /*First configure index register to select the endpoint*/
    USBx->Index = ep->num;

    if (ep->is_in == 1U)
    {
        /*Configure the endpoint as In endpoint*/
        USBx_DEVICE->CtrlSts1_Union.TxCSR2 |= USB_OTG_TXCSR2_MODE;

        /*Enable the endpoint's interrupt of Tx endpoint*/
        if (ep->num > 7)
        {
            USBx->InTrTx2E |= 1 << (ep->num - 7);
        }
        else
        {
           USBx->InTrTx1E |= 1 << (ep->num);
        }        
    }
    else
    {   
        /*Configure the endpoint as Out endpoint*/
        USBx_DEVICE->CtrlSts1_Union.TxCSR2 &= ~(USB_OTG_TXCSR2_MODE);

        /*Enable the endpoint's interrupt of Rx endpoint*/
        if (ep->num > 7)
        {
            USBx->InTrRx2E |= 1 << (ep->num - 7);
        }
        else
        {
           USBx->InTrRx1E |= 1 << (ep->num);
        }        
    }
#endif
  return HAL_OK;
}
/**
  * @brief  Activate and configure a dedicated endpoint
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @retval HAL status
  */
HAL_StatusTypeDef USB_ActivateDedicatedEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep)
{
#if !defined(CHIP_GM6721)
  static __IO uint32_t debug = 0U;
  
  /* Read DEPCTLn register */
  if (ep->is_in == 1U)
  {
    if (((USBx_INEP(ep->num)->DIEPCTL) & USB_OTG_DIEPCTL_USBAEP) == 0U)
    {
      USBx_INEP(ep->num)->DIEPCTL |= ((ep->maxpacket & USB_OTG_DIEPCTL_MPSIZ ) | (ep->type << 18U) |\
        ((ep->num) << 22U) | (USB_OTG_DIEPCTL_SD0PID_SEVNFRM) | (USB_OTG_DIEPCTL_USBAEP)); 
    } 
    
    
    debug  |= ((ep->maxpacket & USB_OTG_DIEPCTL_MPSIZ ) | (ep->type << 18U) |\
        ((ep->num) << 22U) | (USB_OTG_DIEPCTL_SD0PID_SEVNFRM) | (USB_OTG_DIEPCTL_USBAEP)); 
    
   USBx_DEVICE->DEACHMSK |= USB_OTG_DAINTMSK_IEPM & ((1U << (ep->num)));
  }
  else
  {
    if (((USBx_OUTEP(ep->num)->DOEPCTL) & USB_OTG_DOEPCTL_USBAEP) == 0U)
    {
      USBx_OUTEP(ep->num)->DOEPCTL |= ((ep->maxpacket & USB_OTG_DOEPCTL_MPSIZ ) | (ep->type << 18U) |\
        ((ep->num) << 22U) | (USB_OTG_DOEPCTL_USBAEP));
      
      debug = (uint32_t)(((uint32_t )USBx) + USB_OTG_OUT_ENDPOINT_BASE + (0U)*USB_OTG_EP_REG_SIZE);
      debug = (uint32_t )&USBx_OUTEP(ep->num)->DOEPCTL;
      debug |= ((ep->maxpacket & USB_OTG_DOEPCTL_MPSIZ ) | (ep->type << 18U) |\
        ((ep->num) << 22U) | (USB_OTG_DOEPCTL_USBAEP)); 
    } 
    
     USBx_DEVICE->DEACHMSK |= USB_OTG_DAINTMSK_OEPM & ((1U << (ep->num)) << 16U);
  }
#else
  /*This API is not invoked in whole code, it will not be implemented, here*/
#endif
  return HAL_OK;
}
/**
  * @brief  De-activate and de-initialize an endpoint
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @retval HAL status
  */
HAL_StatusTypeDef USB_DeactivateEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep)
{
#if !defined(CHIP_GM6721)

  uint32_t count = 0U;
  
  /* Disable the IN endpoint */
  if (ep->is_in == 1U)
  {
    USBx_INEP(ep->num)->DIEPCTL &= ~USB_OTG_DIEPCTL_USBAEP;  
    
    /* sets the NAK bit for the IN endpoint */
    USBx_INEP(ep->num)->DIEPCTL = USB_OTG_DIEPCTL_SNAK;
    
    /* Disable IN endpoint */
    USBx_INEP(ep->num)->DIEPCTL = USB_OTG_DIEPCTL_EPDIS;
    
    do
    {
      if (++count > 200000U)
      {
        return HAL_TIMEOUT;
      }
    }
    
    /*Wait for  EPDISD endpoint disabled interrupt*/ 
    while ((USBx_INEP(ep->num)->DIEPINT & USB_OTG_DIEPCTL_EPDIS) == USB_OTG_DIEPCTL_EPDIS);
    
    
    /* Flush any data remaining in the TxFIFO */
    USB_FlushTxFifo(USBx , 0x10U);
    
    /* Disable endpoint interrupts */
    USBx_DEVICE->DAINTMSK &= ~(USB_OTG_DAINTMSK_IEPM & ((1U << (ep->num))));   
  
  }
  else /* Disable the OUT endpoint */
  {
    
    USBx_OUTEP(ep->num)->DOEPCTL &= ~USB_OTG_DOEPCTL_USBAEP;  
    
    /* sets the NAK bit for the OUT endpoint */
    USBx_OUTEP(ep->num)->DOEPCTL = USB_OTG_DOEPCTL_SNAK;
    
    /* Disable OUT endpoint */
    USBx_OUTEP(ep->num)->DOEPCTL = USB_OTG_DOEPCTL_EPDIS;
    
    do
    {
      if (++count > 200000U)
      {
        return HAL_TIMEOUT;
      }
    }
    
    /*Wait for  EPDISD endpoint disabled interrupt*/ 
    while ((USBx_OUTEP(ep->num)->DOEPINT & USB_OTG_DOEPINT_OTEPDIS) == USB_OTG_DOEPINT_OTEPDIS);
    
    /* Set the "Clear the Global OUT NAK bit" to disable global OUT NAK mode */
    USBx_DEVICE->DCTL |= USB_OTG_DCTL_CGONAK;
    
    /* Disable endpoint interrupts */
    USBx_DEVICE->DAINTMSK &= ~(USB_OTG_DAINTMSK_OEPM & ((1U << (ep->num)) << 16U));         
  }
#else
    /*First configure index register to select the endpoint*/
    USBx->Index = ep->num;

    if (ep->is_in == 1U)
    {
        /*Configure the endpoint as In endpoint*/
        USBx_DEVICE->CtrlSts1_Union.TxCSR2 |= USB_OTG_TXCSR2_MODE;

        /*Disable the endpoint's interrupt of Tx endpoint*/
        if (ep->num > 7)
        {
            USBx->InTrTx2E &= ~(1 << (ep->num - 7));
        }
        else
        {
           USBx->InTrTx1E &= ~(1 << (ep->num));
        }        
    }
    else
    {   
        /*Configure the endpoint as Out endpoint*/
        USBx_DEVICE->CtrlSts1_Union.TxCSR2 &= ~(USB_OTG_TXCSR2_MODE);

        /*Disable the endpoint's interrupt of Rx endpoint*/
        if (ep->num > 7)
        {
            USBx->InTrRx2E &= ~(1 << (ep->num - 7));
        }
        else
        {
           USBx->InTrRx1E &= ~(1 << (ep->num));
        }
    }
#endif
  return HAL_OK;
}

/**
  * @brief  De-activate and de-initialize a dedicated endpoint
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @retval HAL status
  */
HAL_StatusTypeDef USB_DeactivateDedicatedEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep)
{
#if !defined(CHIP_GM6721)

  uint32_t count = 0U;
  
  /* Disable the IN endpoint */
  if (ep->is_in == 1U)
  {
    USBx_INEP(ep->num)->DIEPCTL &= ~USB_OTG_DIEPCTL_USBAEP;  
    
    /* sets the NAK bit for the IN endpoint */
    USBx_INEP(ep->num)->DIEPCTL = USB_OTG_DIEPCTL_SNAK;
    
    /* Disable IN endpoint */
    USBx_INEP(ep->num)->DIEPCTL = USB_OTG_DIEPCTL_EPDIS;
    
    do
    {
      if (++count > 200000U)
      {
        return HAL_TIMEOUT;
      }
    }
    
    /*Wait for  EPDISD endpoint disabled interrupt*/ 
    while ((USBx_INEP(ep->num)->DIEPINT & USB_OTG_DIEPCTL_EPDIS) == USB_OTG_DIEPCTL_EPDIS);
    
    
    /* Flush any data remaining in the TxFIFO */
    USB_FlushTxFifo(USBx , 0x10U);
    
    /* Disable endpoint interrupts */
    USBx_DEVICE->DAINTMSK &= ~(USB_OTG_DAINTMSK_IEPM & ((1U << (ep->num))));   
  
  }
  else /* Disable the OUT endpoint */
  {
    
    USBx_OUTEP(ep->num)->DOEPCTL &= ~USB_OTG_DOEPCTL_USBAEP;  
    
    /* sets the NAK bit for the OUT endpoint */
    USBx_OUTEP(ep->num)->DOEPCTL = USB_OTG_DOEPCTL_SNAK;
    
    /* Disable OUT endpoint */
    USBx_OUTEP(ep->num)->DOEPCTL = USB_OTG_DOEPCTL_EPDIS;
    
    do
    {
      if (++count > 200000U)
      {
        return HAL_TIMEOUT;
      }
    }
    
    /*Wait for  EPDISD endpoint disabled interrupt*/ 
    while ((USBx_OUTEP(ep->num)->DOEPINT & USB_OTG_DOEPINT_OTEPDIS) == USB_OTG_DOEPINT_OTEPDIS);
    
    /* Set the "Clear the Global OUT NAK bit" to disable global OUT NAK mode */
    USBx_DEVICE->DCTL |= USB_OTG_DCTL_CGONAK;
    
    /* Disable endpoint interrupts */
    USBx_DEVICE->DAINTMSK &= ~(USB_OTG_DAINTMSK_OEPM & ((1U << (ep->num)) << 16U));         
  }
#else
  /*This API is not invoked in whole code, it will not be implemented, here*/
#endif
  return HAL_OK;

}

/**
  * @brief  USB_EPStartXfer : setup and starts a transfer over an EP
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @param  dma: USB dma enabled or disabled 
  *          This parameter can be one of these values:
  *           0 : DMA feature not used 
  *           1 : DMA feature used  
  * @retval HAL status
  */
HAL_StatusTypeDef USB_EPStartXfer(USB_OTG_GlobalTypeDef *USBx , USB_OTG_EPTypeDef *ep, uint8_t dma)
{
	 
	uint32_t nbytes = 0;
	uint8_t  rxstus = 0;
	/* IN endpoint */
	if (ep->is_in == 1U) /*Send out data to Host*/
	{		 
		/* Zero Length Packet? */
		if (ep->xfer_len == 0U)
		{
		   
		}
		else
		{
			if (dma == 1)
			{
				
			}
			else
			{	
				if(ep->xfer_len > 0)					
				{					 
				//	if (ep->xfer_len > MAX_EP_LENGTH)
	                if(ep->xfer_len > ep->maxpacket)
				    {
						//nbytes = MAX_EP_LENGTH;
						ep->xfer_len = ep->maxpacket;
					}
					else
					{
						nbytes = ep->xfer_len;
					}
					
					/*Write index register*/
					USBx->Index = ep->num;
					/*write data to fifo, dma not enabled*/
					USB_WritePacket(USBx, ep->xfer_buff, ep->num, nbytes, 0);
					/*trigger to send data, now it will trigger next interrupt*/
					USBx_DEVICE->CtrlSts0_Union.TxCSR1 |= USB_OTG_TXCSR1_TXPKTRDY;
					
					if(ep->type == EP_TYPE_ISOC)
					{
					    USBx_DEVICE->CtrlSts1_Union.TxCSR2 |= USB_OTG_TXCSR2_ISO;
					}

					ep->xfer_len -= nbytes;
					ep->xfer_buff += nbytes;
					ep->xfer_count += nbytes;
				}	
			}			 
		}
	}
	else /* OUT endpoint */  /*Receive data from Host*/
	{
		if (dma == 1)
		{
			
		}

		/*Write index register*/
		USBx->Index = ep->num;

	    
		/*read RxCSR1*/
		rxstus = USBx_DEVICE->RxCSR1;
		/*there's data in FIFO to read*/
		while (rxstus & USB_OTG_RXCSR1_RXPKTRDY)
		{
			nbytes = (USBx_DEVICE->RxCount2) << 8 | USBx_DEVICE->Count_Union.RxCount1;

			if (nbytes > ep->xfer_len)
			{
				USBx_DEVICE->RxCSR1 &= ~(USB_OTG_RXCSR1_RXPKTRDY);
				/*Set Stall flag*/
				USBx_DEVICE->RxCSR1 |= USB_OTG_RXCSR1_SENDSTLL;
				return ;
			}
			
			/*read nbytes of data to buffer*/
			USB_ReadPacket(USBx, ep->xfer_buff, ep->num, nbytes, 0);

			ep->xfer_len -= nbytes;
			ep->xfer_buff += nbytes;
			
			//if (nbytes < MAX_EP_LENGTH)
	        if( nbytes < ep->maxpacket)
			{
				/*it's the end of transfer*/
				USBx_DEVICE->RxCSR1 &= ~(USB_OTG_RXCSR1_RXPKTRDY);			  
				return;
			}

			/*Clear the flag*/
			USBx_DEVICE->RxCSR1 &= ~(USB_OTG_RXCSR1_RXPKTRDY);			  

			/*read again*/
			rxstus = USBx_DEVICE->RxCSR1;
		}			
	}

  return HAL_OK;
}

/**
  * @brief  USB_EP0StartXfer : setup and starts a transfer over the EP  0
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure
  * @param  dma: USB dma enabled or disabled 
  *          This parameter can be one of these values:
  *           0 : DMA feature not used 
  *           1 : DMA feature used  
  * @retval HAL status
  */
HAL_StatusTypeDef USB_EP0StartXfer(USB_OTG_GlobalTypeDef *USBx , USB_OTG_EPTypeDef *ep, uint8_t dma)
{

   uint32_t nbytes = 0;
   uint8_t val = 0;
   uint8_t rxstus = 0;

   /* IN endpoint */
   if (ep->is_in == 1U) /*Send data out*/
   {        
       /* Zero Length Packet? */
       if (ep->xfer_len == 0U)
       {
          /*Write index register*/
          USBx->Index = ep->num;
          /*it's the end of transfer*/
          val = USB_OTG_CSR0_TXPKTRDY | USB_OTG_CSR0_DATAEND;
          /*trigger to send data*/
          USBx_DEVICE->CtrlSts0_Union.CSR0 |= val;
       }
       else
       {
           if (dma == 1)
           {
               
           }
           else
           {
               /*Send all data out*/
          //      while(ep->xfer_len > 0)
              
               if(ep->xfer_len > 0)
			   {                    
                    if (ep->xfer_len > MAX_EP0_LENGTH)
                    {
						nbytes = MAX_EP_LENGTH;      
                    }
                    else
                    {
                        nbytes = ep->xfer_len; 
                    }
                    
                    /*Write index register*/
                    USBx->Index = ep->num;

                    /*write data to fifo, dma not enabled*/
                    USB_WritePacket(USBx, ep->xfer_buff, ep->num, nbytes, 0);

                    if (nbytes < MAX_EP0_LENGTH)
                    {   
                        /*it's the end of transfer*/
                        val = USB_OTG_CSR0_TXPKTRDY | USB_OTG_CSR0_DATAEND;
                    }
                    else
                    {
                        val = USB_OTG_CSR0_TXPKTRDY;
                    }
                    /*trigger to send data*/
                    USBx_DEVICE->CtrlSts0_Union.CSR0 |= val;                    
                                    
                    ep->xfer_len -= nbytes;
                    ep->xfer_buff += nbytes;
                    ep->xfer_count += nbytes;
                }
           }
       }
   }
   else /* OUT endpoint */
   {
       if (dma == 1)
       {
           
       }

       /*Read data from FIFO*/
        /*Write index register*/
        USBx->Index = ep->num;

        /*read CSR0*/
        rxstus = USBx_DEVICE->CtrlSts0_Union.CSR0;
        /*there's data in FIFO to read*/
        while (rxstus & USB_OTG_CSR0_RXPKTRDY)
        {
            nbytes = USBx_DEVICE->Count_Union.Count0;

            if (nbytes > ep->xfer_len)
            {
                USBx_DEVICE->CtrlSts0_Union.CSR0 |= USB_OTG_CSR0_SERVRXPKTRDY;
                /*Set Stall flag*/
                USBx_DEVICE->CtrlSts0_Union.CSR0 |= USB_OTG_RXCSR1_SENDSTLL;
                return ;
            }
            
            /*read nbytes of data to buffer*/
            USB_ReadPacket(USBx, ep->xfer_buff, ep->num, nbytes, 0);

            ep->xfer_len -= nbytes;
            ep->xfer_buff += nbytes;
            
            if (nbytes < MAX_EP_LENGTH)
            {
                /*it's the end of transfer*/
                USBx_DEVICE->CtrlSts0_Union.CSR0 |= USB_OTG_CSR0_SERVRXPKTRDY | USB_OTG_CSR0_DATAEND;
                return;
            }

            /*Clear the flag*/
            USBx_DEVICE->CtrlSts0_Union.CSR0 |= USB_OTG_CSR0_SERVRXPKTRDY;

            /*read again*/
            rxstus = USBx_DEVICE->CtrlSts0_Union.CSR0;
        }     
       
   }

  return HAL_OK;
}

/**
  * @brief  USB_WritePacket : Writes a packet into the Tx FIFO associated 
  *         with the EP/channel
  * @param  USBx : Selected device           
  * @param  src :  pointer to source buffer
  * @param  ch_ep_num : endpoint or host channel number
  * @param  len : Number of bytes to write
  * @param  dma: USB dma enabled or disabled 
  *          This parameter can be one of these values:
  *           0 : DMA feature not used 
  *           1 : DMA feature used  
  * @retval HAL status
  */
HAL_StatusTypeDef USB_WritePacket(USB_OTG_GlobalTypeDef *USBx, uint8_t *src, uint8_t ch_ep_num, uint16_t len, uint8_t dma)
{
#if !defined(CHIP_GM6721)
  uint32_t count32b = 0U , i = 0U;
  
  if (dma == 0U)
  {
    count32b =  (len + 3U) / 4U;
    for (i = 0U; i < count32b; i++, src += 4U)
    {
      USBx_DFIFO(ch_ep_num) = *((__packed uint32_t *)src);
    }
  }
#else
  uint32_t count32b = 0U , i = 0U;
  
  if (dma == 0U)
  {  
    for (i = 0U; i < len; i++, src += 1U)
    {
      USBx_DFIFO(ch_ep_num) = *((__packed uint8_t *)src);
    }    
  }
#endif
  return HAL_OK;
}

#if !defined(CHIP_GM6721)
/**
  * @brief  USB_ReadPacket : read a packet from the Tx FIFO associated 
  *         with the EP/channel
  * @param  USBx : Selected device  
  * @param  src : source pointer
  * @param  ch_ep_num : endpoint or host channel number
  * @param  len : Number of bytes to read
  * @param  dma: USB dma enabled or disabled 
  *          This parameter can be one of these values:
  *           0 : DMA feature not used 
  *           1 : DMA feature used  
  * @retval pointer to destination buffer
  */
void *USB_ReadPacket(USB_OTG_GlobalTypeDef *USBx, uint8_t *dest, uint16_t len)
{
#if !defined(CHIP_GM6721)
  uint32_t i=0U;
  uint32_t count32b = (len + 3U) / 4U;
  
  for ( i = 0U; i < count32b; i++, dest += 4U )
  {
    *(__packed uint32_t *)dest = USBx_DFIFO(0U);
    
  }
  return ((void *)dest);
 #else
 
 uint32_t i=0U;
 uint32_t count32b = (len + 3U) / 4U;
 
 for ( i = 0U; i < count32b; i++, dest += 4U )
 {
   *(__packed uint8_t *)dest = USBx_DFIFO(0U);
 }    
 return ((void *)dest);
 #endif
}
#else

void USB_ReadPacket(USB_OTG_GlobalTypeDef *USBx, uint8_t *dest, uint8_t ch_ep_num,uint16_t len, uint8_t dma)
{
 uint32_t i=0U; 
 for ( i = 0U; i < len; i++, dest += 1U )
 {
   *(__packed uint8_t *)dest = USBx_DFIFO(ch_ep_num);
 }    
}

#endif


/*
 * Endpoint 0 Tx & Rx: Fixed 64bytes
 * Endpoint 1 Tx & Rx: Fixed 128bytes
 * Endpoint 2 Tx & Rx: Fixed 128bytes
 * Endpoint 3 Tx & Rx: Fixed 128bytes
 * Endpoint 0 Occupied 64 * 2 bytes by USB IP pre-designed
 * Endpoint 1 ~ 3 Occupied 128 * 2 bytes dynamic allocation
 */

#define DYNAMIC_FIFOSIZEOF_TXENDPOINT_X  128
#define DYNAMIC_FIFOSIZEOF_RXENDPOINT_X  128
#define DYNAMIC_TXRXENDPOINT_OFFSET      128
#define DYNAMIC_MAXPKT_SIZE              3

void USB_SetDynamicSize(USB_OTG_GlobalTypeDef *USBx)
{
	uint8_t ep;
	uint16_t addr = 0;
	addr += DYNAMIC_TXRXENDPOINT_OFFSET;
	for (ep = 1; ep < 4; ep ++)
	{		    
		//printf("addr 0: 0x%08x \n", addr);
		/*Set index register*/
		USBx->Index = ep;
		/*Set Tx FIFO size for endpoint ep*/
		USBx_DEVICE->DyTxFIFO1 = (addr / 8) & 0xFF;
		/*Double Packet Disabled*/
		USBx_DEVICE->DyTxFIFO2 = (addr / 8) >> 8 & 0x0F | DYNAMIC_MAXPKT_SIZE << 5;
		
		addr += DYNAMIC_FIFOSIZEOF_TXENDPOINT_X;
		//printf("addr 1: 0x%08x \n", addr);
		/*Set Rx FIFO size for endpoint ep*/
		USBx_DEVICE->DyRxFIFO1 = (addr / 8) & 0xFF;
		/*Double Packet Disabled*/
		USBx_DEVICE->FIFO_Union.DyRxFIFO2 = (addr / 8) >> 8 & 0x0F | DYNAMIC_MAXPKT_SIZE << 5;
		addr += DYNAMIC_FIFOSIZEOF_RXENDPOINT_X;
	    //printf("addr 2: 0x%08x \n", addr);
	}
}

/**
  * @brief  USB_EPSetStall : set a stall condition over an EP
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure   
  * @retval HAL status
  */
HAL_StatusTypeDef USB_EPSetStall(USB_OTG_GlobalTypeDef *USBx , USB_OTG_EPTypeDef *ep)
{
#if !defined(CHIP_GM6721)
  if (ep->is_in == 1U)
  {
    if (((USBx_INEP(ep->num)->DIEPCTL) & USB_OTG_DIEPCTL_EPENA) == 0U)
    {
      USBx_INEP(ep->num)->DIEPCTL &= ~(USB_OTG_DIEPCTL_EPDIS); 
    } 
    USBx_INEP(ep->num)->DIEPCTL |= USB_OTG_DIEPCTL_STALL;
  }
  else
  {
    if (((USBx_OUTEP(ep->num)->DOEPCTL) & USB_OTG_DOEPCTL_EPENA) == 0U)
    {
      USBx_OUTEP(ep->num)->DOEPCTL &= ~(USB_OTG_DOEPCTL_EPDIS); 
    } 
    USBx_OUTEP(ep->num)->DOEPCTL |= USB_OTG_DOEPCTL_STALL;
  }
#else

    USBx->Index = ep->num;

    /*implemented for Endpoint 0*/
    if (ep->num == 0)
    {   
        /*Set only one-time*/
        if (!(USBx_DEVICE->CtrlSts0_Union.CSR0 & USB_OTG_CSR0_SENDSTALL))
        {
        	USBx_DEVICE->CtrlSts0_Union.CSR0 = USB_OTG_CSR0_SENDSTALL | USB_OTG_CSR0_SERVRXPKTRDY; // | USB_OTG_CSR0_SERVRXPKTRDY
        }
    }
    else
    {
        /*implemented for endpoints other than 0*/
        if (ep->is_in == 1U)
        {
            /*Send STALL condition when as a Tx endpoint*/
            USBx_DEVICE->CtrlSts0_Union.TxCSR1 |= USB_OTG_TXCSR1_SENDSTLL;
        }
        else
        {
            /*Send STALL condition when as a RX endpoint*/
            USBx_DEVICE->RxCSR1 |= USB_OTG_RXCSR1_SENDSTLL;
        }
    }

#endif
  return HAL_OK;
}


/**
  * @brief  USB_EPClearStall : Clear a stall condition over an EP
  * @param  USBx : Selected device
  * @param  ep: pointer to endpoint structure   
  * @retval HAL status
  */
HAL_StatusTypeDef USB_EPClearStall(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep)
{
#if !defined(CHIP_GM6721)
  if (ep->is_in == 1U)
  {
    USBx_INEP(ep->num)->DIEPCTL &= ~USB_OTG_DIEPCTL_STALL;
    if (ep->type == EP_TYPE_INTR || ep->type == EP_TYPE_BULK)
    {
       USBx_INEP(ep->num)->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM; /* DATA0 */
    }    
  }
  else
  {
    USBx_OUTEP(ep->num)->DOEPCTL &= ~USB_OTG_DOEPCTL_STALL;
    if (ep->type == EP_TYPE_INTR || ep->type == EP_TYPE_BULK)
    {
      USBx_OUTEP(ep->num)->DOEPCTL |= USB_OTG_DOEPCTL_SD0PID_SEVNFRM; /* DATA0 */
    }    
  }
#else
  USBx->Index = ep->num;

  if (ep->is_in == 1U)
  {
      /*Send STALL condition when as a Tx endpoint*/
      USBx_DEVICE->CtrlSts0_Union.TxCSR1 &= ~(USB_OTG_TXCSR1_SENTSTLL);
      if (ep->type == EP_TYPE_INTR || ep->type == EP_TYPE_BULK)
      {
          USBx_DEVICE->CtrlSts0_Union.TxCSR1 |= USB_OTG_TXCSR1_CLRDATATOG; /* DATA0 */
      }
  }
  else
  {
      /*Send STALL condition when as a RX endpoint*/
      USBx_DEVICE->RxCSR1 &= ~(USB_OTG_RXCSR1_SENTSTLL);
      if (ep->type == EP_TYPE_INTR || ep->type == EP_TYPE_BULK)
      {
          USBx_DEVICE->RxCSR1 |= USB_OTG_RXCSR1_CLRDATATOG; /* DATA0 */
      }
  } 
#endif
  return HAL_OK;
}

/**
  * @brief  USB_StopDevice : Stop the usb device mode
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef USB_StopDevice(USB_OTG_GlobalTypeDef *USBx)
{
#if !defined(CHIP_GM6721)
  uint32_t i;
  
  /* Clear Pending interrupt */
  for (i = 0U; i < 15U ; i++)
  {
    USBx_INEP(i)->DIEPINT  = 0xFFU;
    USBx_OUTEP(i)->DOEPINT  = 0xFFU;
  }
  USBx_DEVICE->DAINT = 0xFFFFFFFFU;
  
  /* Clear interrupt masks */
  USBx_DEVICE->DIEPMSK  = 0U;
  USBx_DEVICE->DOEPMSK  = 0U;
  USBx_DEVICE->DAINTMSK = 0U;
  
  /* Flush the FIFO */
  USB_FlushRxFifo(USBx);
  USB_FlushTxFifo(USBx ,  0x10U);  
#else
  
  uint8_t tmp = 0;

  /* Flush the FIFOs */
  USB_FlushTxFifo(USBx, 0x10U); /* all Tx FIFOs */
  USB_FlushRxFifo(USBx, 0x10U); /* all Rx FIFOs */  
    
  tmp = USBx->IntrTx1; /*read to clear all pending interrupts for Tx endpoints*/
  tmp = USBx->IntrTx2;
  
  tmp = USBx->IntrRx1; /*read to clear all pending interrupts for Rx endpoints*/
  tmp = USBx->IntrRx2;
  tmp = USBx->IntrUSB; /*read to clear all pending interrupts for USB*/

  /*Disable each individual interrupt for Tx endpoints*/    
  USBx->InTrTx1E &= 0x00;
  USBx->InTrTx2E &= 0x00;

  /*Disable each individual interrupt for Rx endpoints*/
  USBx->InTrRx1E &= 0x00;
  USBx->InTrRx2E &= 0x00;  

  /*And any other pending interrupt ???*/
  USBx->IntrUSBE &= 0x00;

#endif
  return HAL_OK;
}

/**
  * @brief  USB_SetDevAddress : Stop the usb device mode
  * @param  USBx : Selected device
  * @param  address : new device address to be assigned
  *          This parameter can be a value from 0 to 255
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_SetDevAddress (USB_OTG_GlobalTypeDef *USBx, uint8_t address)
{
#if !defined(CHIP_GM6721)
  USBx_DEVICE->DCFG &= ~ (USB_OTG_DCFG_DAD);
  USBx_DEVICE->DCFG |= (address << 4U) & USB_OTG_DCFG_DAD ;
#else
  USBx->FAddr = (address & 0x7F);
#endif
  return HAL_OK;
}

/**
  * @brief  USB_DevConnect : Connect the USB device by enabling the pull-up/pull-down
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_DevConnect (USB_OTG_GlobalTypeDef *USBx)
{
#if !defined(CHIP_GM6721)
  USBx_DEVICE->DCTL &= ~USB_OTG_DCTL_SDIS ;
//  HAL_Delay(3U);
  SysTick_DelayMS(3U);
#else

  /*How to implement ???*/
  //SysTick_DelayMS(3U);

  /*Enable endpoint interrupt*/
  USBx->InTrTx1E  |= 0xF;
  USBx->InTrRx1E  |= 0xF; /*it matters nothing while you're writing bit0*/  
  USBx->IntrUSBE = 0xFF;  

  k_busy_wait(3000);
#endif
  return HAL_OK;  
}

/**
  * @brief  USB_DevDisconnect : Disconnect the USB device by disabling the pull-up/pull-down
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_DevDisconnect (USB_OTG_GlobalTypeDef *USBx)
{
#if !defined(CHIP_GM6721)
  USBx_DEVICE->DCTL |= USB_OTG_DCTL_SDIS ;
//  HAL_Delay(3U);
  SysTick_DelayMS(3);
#else

   /*How to implement ???*/
  USB_FlushRxFifo(USBx, 0x10);
  USB_FlushTxFifo(USBx, 0x10);
  USBx->InTrTx1E &= 0x0;
  USBx->InTrRx1E  &= 0x0; /*it matters nothing while you're writing bit0*/  
  USBx->IntrUSBE = 0x00;  

   k_busy_wait(3000);
#endif
  return HAL_OK;  
}

/**
  * @brief  USB_ReadInterrupts: return the global USB interrupt status
  * @param  USBx : Selected device
  * @retval HAL status
  */
uint32_t  USB_ReadInterrupts (USB_OTG_GlobalTypeDef *USBx)
{
#if !defined(CHIP_GM6721)
  uint32_t v = 0U;
  
  v = USBx->GINTSTS;
  v &= USBx->GINTMSK;
#else

  uint8_t v = USBx->IntrUSB; 
#endif

  return v;
}

/**
  * @brief  USB_ReadDevAllOutEpInterrupt: return the USB device OUT endpoints interrupt status
  * @param  USBx : Selected device
  * @retval HAL status
  */
uint32_t USB_ReadDevAllOutEpInterrupt (USB_OTG_GlobalTypeDef *USBx)
{
#if !defined(CHIP_GM6721)
  uint32_t v;
  v  = USBx_DEVICE->DAINT;
  v &= USBx_DEVICE->DAINTMSK;
  return ((v & 0xffff0000U) >> 16U);
#else

  uint16_t v;
  v = USBx->IntrRx1 | (USBx->IntrRx2 << 8);
  return v;
#endif
}

/**
  * @brief  USB_ReadDevAllInEpInterrupt: return the USB device IN endpoints interrupt status
  * @param  USBx : Selected device
  * @retval HAL status
  */
uint32_t USB_ReadDevAllInEpInterrupt (USB_OTG_GlobalTypeDef *USBx)
{
#if !defined(CHIP_GM6721)
  uint32_t v;
  v  = USBx_DEVICE->DAINT;
  v &= USBx_DEVICE->DAINTMSK;
  return ((v & 0xFFFFU));
#else

  uint16_t v;
  v = USBx->IntrTx1 | (USBx->IntrTx2 << 8);
  return v;
#endif
}

/**
  * @brief  Returns Device OUT EP Interrupt register
  * @param  USBx : Selected device
  * @param  epnum : endpoint number
  *          This parameter can be a value from 0 to 15
  * @retval Device OUT EP Interrupt register
  */
uint32_t USB_ReadDevOutEPInterrupt (USB_OTG_GlobalTypeDef *USBx , uint8_t epnum)
{
#if !defined(CHIP_GM6721)
  uint32_t v;
  v  = USBx_OUTEP(epnum)->DOEPINT;
  v &= USBx_DEVICE->DOEPMSK;
  return v;
#else

  uint16_t v;

  USBx->Index = epnum;
  if (epnum == 0)
  {
      /*v = USBx_DEVICE->CtrlSts0_Union.CSR0;*/
	  /*do nothing*/
	  return 0;
  }
  else
  {
      v = (USBx_DEVICE->RxCSR2) << 8 | USBx_DEVICE->RxCSR1;
  }
  
  return v;
#endif
}

/**
  * @brief  Returns Device IN EP Interrupt register
  * @param  USBx : Selected device
  * @param  epnum : endpoint number
  *          This parameter can be a value from 0 to 15
  * @retval Device IN EP Interrupt register
  */
uint32_t USB_ReadDevInEPInterrupt (USB_OTG_GlobalTypeDef *USBx , uint8_t epnum)
{
#if !defined(CHIP_GM6721)
  uint32_t v, msk, emp;
  
  msk = USBx_DEVICE->DIEPMSK;
  emp = USBx_DEVICE->DIEPEMPMSK;
  msk |= ((emp >> epnum) & 0x1U) << 7U;
  v = USBx_INEP(epnum)->DIEPINT & msk;
  return v;
#else  
    uint16_t v;

    USBx->Index = epnum;
    if (epnum == 0)
    {
        v = USBx_DEVICE->CtrlSts0_Union.CSR0;
    }
    else
    {
        v = (USBx_DEVICE->CtrlSts1_Union.TxCSR2) << 8 | USBx_DEVICE->CtrlSts0_Union.TxCSR1;
    }
    return v;
#endif
}

/**
  * @brief  USB_ClearInterrupts: clear a USB interrupt
  * @param  USBx : Selected device
  * @param  interrupt : interrupt flag
  * @retval None
  */
void  USB_ClearInterrupts (USB_OTG_GlobalTypeDef *USBx, uint32_t interrupt)
{
#if !defined(CHIP_GM6721)
  USBx->GINTSTS |= interrupt; 
#else
  /*Not implemented for the time being*/

#endif
}

/**
  * @brief  Returns USB core mode
  * @param  USBx : Selected device
  * @retval return core mode : Host or Device
  *          This parameter can be one of these values:
  *           0 : Host 
  *           1 : Device
  */
uint32_t USB_GetMode(USB_OTG_GlobalTypeDef *USBx)
{
#if !defined(CHIP_GM6721)
  return ((USBx->GINTSTS ) & 0x1U);
#else
  /*Be careful that it may be different from that of STM32*/
  return ((USBx->DevCtl) & USB_OTG_DEVCTL_HOSTMODE_Msk);
#endif
}


/**
  * @brief  Activate EP0 for Setup transactions
  * @param  USBx : Selected device
  * @retval HAL status
  */
HAL_StatusTypeDef  USB_ActivateSetup (USB_OTG_GlobalTypeDef *USBx)
{
#if !defined(CHIP_GM6721)
  /* Set the MPS of the IN EP based on the enumeration speed */
  USBx_INEP(0U)->DIEPCTL &= ~USB_OTG_DIEPCTL_MPSIZ;
  
  if((USBx_DEVICE->DSTS & USB_OTG_DSTS_ENUMSPD) == DSTS_ENUMSPD_LS_PHY_6MHZ)
  {
    USBx_INEP(0U)->DIEPCTL |= 3U;
  }
  USBx_DEVICE->DCTL |= USB_OTG_DCTL_CGINAK;
#else
  
#endif
  return HAL_OK;
}


/**
  * @brief  Prepare the EP0 to start the first control setup
  * @param  USBx : Selected device
  * @param  dma: USB dma enabled or disabled 
  *          This parameter can be one of these values:
  *           0 : DMA feature not used 
  *           1 : DMA feature used  
  * @param  psetup : pointer to setup packet
  * @retval HAL status
  */
HAL_StatusTypeDef USB_EP0_OutStart(USB_OTG_GlobalTypeDef *USBx, uint8_t dma, uint8_t *psetup)
{
#if !defined(CHIP_GM6721)
  USBx_OUTEP(0U)->DOEPTSIZ = 0U;
  USBx_OUTEP(0U)->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_PKTCNT & (1U << 19U)) ;
  USBx_OUTEP(0U)->DOEPTSIZ |= (3U * 8U);
  USBx_OUTEP(0U)->DOEPTSIZ |=  USB_OTG_DOEPTSIZ_STUPCNT;  
  
  if (dma == 1U)
  {
    USBx_OUTEP(0U)->DOEPDMA = (uint32_t)psetup;
    /* EP enable */
    USBx_OUTEP(0U)->DOEPCTL = 0x80008000U;
  }
#else
   
#endif
  return HAL_OK;  
}


/**
  * @brief  Reset the USB Core (needed after USB clock settings change)
  * @param  USBx : Selected device
  * @retval HAL status
  */
static HAL_StatusTypeDef USB_CoreReset(USB_OTG_GlobalTypeDef *USBx)
{
#if !defined(CHIP_GM6721)

  uint32_t count = 0U;

  /* Wait for AHB master IDLE state. */
  do
  {
    if (++count > 200000U)
    {
      return HAL_TIMEOUT;
    }
  }
  while ((USBx->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL) == 0U);
  
  /* Core Soft Reset */
  count = 0U;
  USBx->GRSTCTL |= USB_OTG_GRSTCTL_CSRST;

  do
  {
    if (++count > 200000U)
    {
      return HAL_TIMEOUT;
    }
  }
  while ((USBx->GRSTCTL & USB_OTG_GRSTCTL_CSRST) == USB_OTG_GRSTCTL_CSRST);
#else

  /* USB Core Soft Reset, controlled by system register */
  
#endif
  return HAL_OK;
}


/**
  * @brief  USB_HostInit : Initializes the USB OTG controller registers 
  *         for Host mode 
  * @param  USBx : Selected device
  * @param  cfg  : pointer to a USB_OTG_CfgTypeDef structure that contains
  *         the configuration information for the specified USBx peripheral.
  * @retval HAL status
  */
HAL_StatusTypeDef USB_HostInit (USB_OTG_GlobalTypeDef *USBx, USB_OTG_CfgTypeDef cfg)
{
#if !defined(CHIP_GM6721)

  uint32_t i;
  
  /* Restart the Phy Clock */
  USBx_PCGCCTL = 0U;
  
  /* Activate VBUS Sensing B */
#if defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || \
    defined(STM32F412Rx) || defined(STM32F412Cx) || defined(STM32F413xx) || defined(STM32F423xx)
  USBx->GCCFG |= USB_OTG_GCCFG_VBDEN;
#else
  USBx->GCCFG &=~ (USB_OTG_GCCFG_VBUSASEN);
  USBx->GCCFG &=~ (USB_OTG_GCCFG_VBUSBSEN);
  USBx->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
#endif /* STM32F446xx || STM32F469xx || STM32F479xx || STM32F412Zx || STM32F412Rx || STM32F412Vx || STM32F412Cx || STM32F413xx || STM32F423xx  */

  /* Disable the FS/LS support mode only */
  if((cfg.speed == USB_OTG_SPEED_FULL)&&
     (USBx != USB_OTG_FS))
  {
    USBx_HOST->HCFG |= USB_OTG_HCFG_FSLSS; 
  }
  else
  {
    USBx_HOST->HCFG &= ~(USB_OTG_HCFG_FSLSS);  
  }

  /* Make sure the FIFOs are flushed. */
  USB_FlushTxFifo(USBx, 0x10U); /* all Tx FIFOs */
  USB_FlushRxFifo(USBx);

  /* Clear all pending HC Interrupts */
  for (i = 0U; i < cfg.Host_channels; i++)
  {
    USBx_HC(i)->HCINT = 0xFFFFFFFFU;
    USBx_HC(i)->HCINTMSK = 0U;
  }
  
  /* Enable VBUS driving */
  USB_DriveVbus(USBx, 1U);
  
//  HAL_Delay(200U);
  SysTick_DelayMS(200U);
  /* Disable all interrupts. */
  USBx->GINTMSK = 0U;
  
  /* Clear any pending interrupts */
  USBx->GINTSTS = 0xFFFFFFFFU;
  
  if(USBx == USB_OTG_FS)
  {
    /* set Rx FIFO size */
    USBx->GRXFSIZ  = 0x80U; 
    USBx->DIEPTXF0_HNPTXFSIZ = (uint32_t )(((0x60U << 16U)& USB_OTG_NPTXFD) | 0x80U);
    USBx->HPTXFSIZ = (uint32_t )(((0x40U << 16U)& USB_OTG_HPTXFSIZ_PTXFD) | 0xE0U);
  }
  else
  {
    /* set Rx FIFO size */
    USBx->GRXFSIZ  = 0x200U; 
    USBx->DIEPTXF0_HNPTXFSIZ = (uint32_t )(((0x100U << 16U)& USB_OTG_NPTXFD) | 0x200U);
    USBx->HPTXFSIZ = (uint32_t )(((0xE0U << 16U)& USB_OTG_HPTXFSIZ_PTXFD) | 0x300U);
  }
  
  /* Enable the common interrupts */
  if (cfg.dma_enable == DISABLE)
  {
    USBx->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM; 
  }
  
  /* Enable interrupts matching to the Host mode ONLY */
  USBx->GINTMSK |= (USB_OTG_GINTMSK_PRTIM            | USB_OTG_GINTMSK_HCIM |\
                    USB_OTG_GINTMSK_SOFM             |USB_OTG_GINTSTS_DISCINT|\
                    USB_OTG_GINTMSK_PXFRM_IISOOXFRM  | USB_OTG_GINTMSK_WUIM);
#else

  /*NOT IMPLEMENTED HERE FOR THE TIME BEING!!!*/

#endif
  return HAL_OK;
}

/**
  * @brief  USB_InitFSLSPClkSel : Initializes the FSLSPClkSel field of the 
  *         HCFG register on the PHY type and set the right frame interval
  * @param  USBx : Selected device
  * @param  freq : clock frequency
  *          This parameter can be one of these values:
  *           HCFG_48_MHZ : Full Speed 48 MHz Clock 
  *           HCFG_6_MHZ : Low Speed 6 MHz Clock 
  * @retval HAL status
  */
HAL_StatusTypeDef USB_InitFSLSPClkSel(USB_OTG_GlobalTypeDef *USBx , uint8_t freq)
{
#if !defined(CHIP_GM6721)
  USBx_HOST->HCFG &= ~(USB_OTG_HCFG_FSLSPCS);
  USBx_HOST->HCFG |= (freq & USB_OTG_HCFG_FSLSPCS);
  
  if (freq ==  HCFG_48_MHZ)
  {
    USBx_HOST->HFIR = 48000U;
  }
  else if (freq ==  HCFG_6_MHZ)
  {
    USBx_HOST->HFIR = 6000U;
  } 
#else  
#endif  
  return HAL_OK;  
}

/**
* @brief  USB_OTG_ResetPort : Reset Host Port
  * @param  USBx : Selected device
  * @retval HAL status
  * @note   (1)The application must wait at least 10 ms
  *   before clearing the reset bit.
  */
HAL_StatusTypeDef USB_ResetPort(USB_OTG_GlobalTypeDef *USBx)
{
#if !defined(CHIP_GM6721)
  __IO uint32_t hprt0;
  
  hprt0 = USBx_HPRT0;
  
  hprt0 &= ~(USB_OTG_HPRT_PENA    | USB_OTG_HPRT_PCDET |\
    USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG );
  
  USBx_HPRT0 = (USB_OTG_HPRT_PRST | hprt0);  
//  HAL_Delay (10U);                                /* See Note #1 */
  SysTick_DelayMS(10U);
	USBx_HPRT0 = ((~USB_OTG_HPRT_PRST) & hprt0); 
#else
#endif
  return HAL_OK;
}

/**
  * @brief  USB_DriveVbus : activate or de-activate vbus
  * @param  state : VBUS state
  *          This parameter can be one of these values:
  *           0 : VBUS Active 
  *           1 : VBUS Inactive
  * @retval HAL status
*/
HAL_StatusTypeDef USB_DriveVbus (USB_OTG_GlobalTypeDef *USBx, uint8_t state)
{
#if !defined(CHIP_GM6721)
  __IO uint32_t hprt0;

  hprt0 = USBx_HPRT0;
  hprt0 &= ~(USB_OTG_HPRT_PENA    | USB_OTG_HPRT_PCDET |\
                         USB_OTG_HPRT_PENCHNG | USB_OTG_HPRT_POCCHNG );
  
  if (((hprt0 & USB_OTG_HPRT_PPWR) == 0U) && (state == 1U))
  {
    USBx_HPRT0 = (USB_OTG_HPRT_PPWR | hprt0); 
  }
  if (((hprt0 & USB_OTG_HPRT_PPWR) == USB_OTG_HPRT_PPWR) && (state == 0U))
  {
    USBx_HPRT0 = ((~USB_OTG_HPRT_PPWR) & hprt0); 
  }
#else
#endif
  return HAL_OK; 
}

/**
  * @brief  Return Host Core speed
  * @param  USBx : Selected device
  * @retval speed : Host speed
  *          This parameter can be one of these values:
  *            @arg USB_OTG_SPEED_HIGH: High speed mode
  *            @arg USB_OTG_SPEED_FULL: Full speed mode
  *            @arg USB_OTG_SPEED_LOW: Low speed mode
  */
uint32_t USB_GetHostSpeed (USB_OTG_GlobalTypeDef *USBx)
{
#if !defined(CHIP_GM6721)
  __IO uint32_t hprt0;
  
  hprt0 = USBx_HPRT0;
  return ((hprt0 & USB_OTG_HPRT_PSPD) >> 17U);
#else
  return 0;
#endif
}

/**
  * @brief  Return Host Current Frame number
  * @param  USBx : Selected device
  * @retval current frame number
*/
uint32_t USB_GetCurrentFrame (USB_OTG_GlobalTypeDef *USBx)
{
#if !defined(CHIP_GM6721)
  return (USBx_HOST->HFNUM & USB_OTG_HFNUM_FRNUM);
#else
  return (USBx->Frame1 | (USBx->Frame2 << 8));
#endif
}

/**
  * @brief  Initialize a host channel
  * @param  USBx : Selected device
  * @param  ch_num : Channel number
  *         This parameter can be a value from 1 to 15
  * @param  epnum : Endpoint number
  *          This parameter can be a value from 1 to 15
  * @param  dev_address : Current device address
  *          This parameter can be a value from 0 to 255
  * @param  speed : Current device speed
  *          This parameter can be one of these values:
  *            @arg USB_OTG_SPEED_HIGH: High speed mode
  *            @arg USB_OTG_SPEED_FULL: Full speed mode
  *            @arg USB_OTG_SPEED_LOW: Low speed mode
  * @param  ep_type : Endpoint Type
  *          This parameter can be one of these values:
  *            @arg EP_TYPE_CTRL: Control type
  *            @arg EP_TYPE_ISOC: Isochronous type
  *            @arg EP_TYPE_BULK: Bulk type
  *            @arg EP_TYPE_INTR: Interrupt type
  * @param  mps : Max Packet Size
  *          This parameter can be a value from 0 to32K
  * @retval HAL state
  */
HAL_StatusTypeDef USB_HC_Init(USB_OTG_GlobalTypeDef *USBx,  
                              uint8_t ch_num,
                              uint8_t epnum,
                              uint8_t dev_address,
                              uint8_t speed,
                              uint8_t ep_type,
                              uint16_t mps)
{
 #if !defined(CHIP_GM6721)   
  /* Clear old interrupt conditions for this host channel. */
  USBx_HC(ch_num)->HCINT = 0xFFFFFFFFU;
  
  /* Enable channel interrupts required for this transfer. */
  switch (ep_type) 
  {
  case EP_TYPE_CTRL:
  case EP_TYPE_BULK:
    
    USBx_HC(ch_num)->HCINTMSK = USB_OTG_HCINTMSK_XFRCM  |\
                                USB_OTG_HCINTMSK_STALLM |\
                                USB_OTG_HCINTMSK_TXERRM |\
                                USB_OTG_HCINTMSK_DTERRM |\
                                USB_OTG_HCINTMSK_AHBERR |\
                                USB_OTG_HCINTMSK_NAKM ;
 
    if (epnum & 0x80U) 
    {
      USBx_HC(ch_num)->HCINTMSK |= USB_OTG_HCINTMSK_BBERRM;
    } 
    else 
    {
      if(USBx != USB_OTG_FS)
      {
        USBx_HC(ch_num)->HCINTMSK |= (USB_OTG_HCINTMSK_NYET | USB_OTG_HCINTMSK_ACKM);
      }
    }
    break;
    
  case EP_TYPE_INTR:
    
    USBx_HC(ch_num)->HCINTMSK = USB_OTG_HCINTMSK_XFRCM  |\
                                USB_OTG_HCINTMSK_STALLM |\
                                USB_OTG_HCINTMSK_TXERRM |\
                                USB_OTG_HCINTMSK_DTERRM |\
                                USB_OTG_HCINTMSK_NAKM   |\
                                USB_OTG_HCINTMSK_AHBERR |\
                                USB_OTG_HCINTMSK_FRMORM ;    
    
    if (epnum & 0x80U) 
    {
      USBx_HC(ch_num)->HCINTMSK |= USB_OTG_HCINTMSK_BBERRM;
    }
    
    break;
  case EP_TYPE_ISOC:
    
    USBx_HC(ch_num)->HCINTMSK = USB_OTG_HCINTMSK_XFRCM  |\
                                USB_OTG_HCINTMSK_ACKM   |\
                                USB_OTG_HCINTMSK_AHBERR |\
                                USB_OTG_HCINTMSK_FRMORM ;   
    
    if (epnum & 0x80U) 
    {
      USBx_HC(ch_num)->HCINTMSK |= (USB_OTG_HCINTMSK_TXERRM | USB_OTG_HCINTMSK_BBERRM);      
    }
    break;
  }
  
  /* Enable the top level host channel interrupt. */
  USBx_HOST->HAINTMSK |= (1 << ch_num);
  
  /* Make sure host channel interrupts are enabled. */
  USBx->GINTMSK |= USB_OTG_GINTMSK_HCIM;
  
  /* Program the HCCHAR register */
  USBx_HC(ch_num)->HCCHAR = (((dev_address << 22U) & USB_OTG_HCCHAR_DAD)  |\
                             (((epnum & 0x7FU)<< 11U) & USB_OTG_HCCHAR_EPNUM)|\
                             ((((epnum & 0x80U) == 0x80U)<< 15U) & USB_OTG_HCCHAR_EPDIR)|\
                             (((speed == USB_OTG_SPEED_LOW)<< 17U) & USB_OTG_HCCHAR_LSDEV)|\
                             ((ep_type << 18U) & USB_OTG_HCCHAR_EPTYP)|\
                             (mps & USB_OTG_HCCHAR_MPSIZ));
    
  if (ep_type == EP_TYPE_INTR)
  {
    USBx_HC(ch_num)->HCCHAR |= USB_OTG_HCCHAR_ODDFRM ;
  }
#else

#endif
  return HAL_OK; 
}

/**
  * @brief  Start a transfer over a host channel
  * @param  USBx : Selected device
  * @param  hc : pointer to host channel structure
  * @param  dma: USB dma enabled or disabled 
  *          This parameter can be one of these values:
  *           0 : DMA feature not used 
  *           1 : DMA feature used  
  * @retval HAL state
  */
#if defined   (__CC_ARM) /*!< ARM Compiler */
#pragma O0
#elif defined (__GNUC__) /*!< GNU Compiler */
#pragma GCC optimize ("O0")
#endif /* __CC_ARM */
HAL_StatusTypeDef USB_HC_StartXfer(USB_OTG_GlobalTypeDef *USBx, USB_OTG_HCTypeDef *hc, uint8_t dma)
{
#if !defined(CHIP_GM6721)
  uint8_t  is_oddframe = 0; 
  uint16_t len_words = 0;   
  uint16_t num_packets = 0;
  uint16_t max_hc_pkt_count = 256;
  uint32_t tmpreg = 0U;
    
  if((USBx != USB_OTG_FS) && (hc->speed == USB_OTG_SPEED_HIGH))
  {
    if((dma == 0) && (hc->do_ping == 1U))
    {
      USB_DoPing(USBx, hc->ch_num);
      return HAL_OK;
    }
    else if(dma == 1)
    {
      USBx_HC(hc->ch_num)->HCINTMSK &= ~(USB_OTG_HCINTMSK_NYET | USB_OTG_HCINTMSK_ACKM);
      hc->do_ping = 0U;
    }
  }
  
  /* Compute the expected number of packets associated to the transfer */
  if (hc->xfer_len > 0U)
  {
    num_packets = (hc->xfer_len + hc->max_packet - 1U) / hc->max_packet;
    
    if (num_packets > max_hc_pkt_count)
    {
      num_packets = max_hc_pkt_count;
      hc->xfer_len = num_packets * hc->max_packet;
    }
  }
  else
  {
    num_packets = 1;
  }
  if (hc->ep_is_in)
  {
    hc->xfer_len = num_packets * hc->max_packet;
  }
  
  /* Initialize the HCTSIZn register */
  USBx_HC(hc->ch_num)->HCTSIZ = (((hc->xfer_len) & USB_OTG_HCTSIZ_XFRSIZ)) |\
    ((num_packets << 19U) & USB_OTG_HCTSIZ_PKTCNT) |\
      (((hc->data_pid) << 29U) & USB_OTG_HCTSIZ_DPID);
  
  if (dma)
  {
    /* xfer_buff MUST be 32-bits aligned */
    USBx_HC(hc->ch_num)->HCDMA = (uint32_t)hc->xfer_buff;
  }
  
  is_oddframe = (USBx_HOST->HFNUM & 0x01) ? 0 : 1;
  USBx_HC(hc->ch_num)->HCCHAR &= ~USB_OTG_HCCHAR_ODDFRM;
  USBx_HC(hc->ch_num)->HCCHAR |= (is_oddframe << 29);
  
  /* Set host channel enable */
  tmpreg = USBx_HC(hc->ch_num)->HCCHAR;
  tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
  tmpreg |= USB_OTG_HCCHAR_CHENA;
  USBx_HC(hc->ch_num)->HCCHAR = tmpreg;
  
  if (dma == 0) /* Slave mode */
  {  
    if((hc->ep_is_in == 0U) && (hc->xfer_len > 0U))
    {
      switch(hc->ep_type) 
      {
        /* Non periodic transfer */
      case EP_TYPE_CTRL:
      case EP_TYPE_BULK:
        
        len_words = (hc->xfer_len + 3) / 4;
        
        /* check if there is enough space in FIFO space */
        if(len_words > (USBx->HNPTXSTS & 0xFFFF))
        {
          /* need to process data in nptxfempty interrupt */
          USBx->GINTMSK |= USB_OTG_GINTMSK_NPTXFEM;
        }
        break;
        /* Periodic transfer */
      case EP_TYPE_INTR:
      case EP_TYPE_ISOC:
        len_words = (hc->xfer_len + 3) / 4;
        /* check if there is enough space in FIFO space */
        if(len_words > (USBx_HOST->HPTXSTS & 0xFFFF)) /* split the transfer */
        {
          /* need to process data in ptxfempty interrupt */
          USBx->GINTMSK |= USB_OTG_GINTMSK_PTXFEM;          
        }
        break;
        
      default:
        break;
      }
      
      /* Write packet into the Tx FIFO. */
      USB_WritePacket(USBx, hc->xfer_buff, hc->ch_num, hc->xfer_len, 0);
    }
  }
#else

#endif
  return HAL_OK;
}

/**
  * @brief Read all host channel interrupts status
  * @param  USBx : Selected device
  * @retval HAL state
  */
uint32_t USB_HC_ReadInterrupt (USB_OTG_GlobalTypeDef *USBx)
{
#if !defined(CHIP_GM6721)
  return ((USBx_HOST->HAINT) & 0xFFFFU);
#else
  return 0;
#endif
}

/**
  * @brief  Halt a host channel
  * @param  USBx : Selected device
  * @param  hc_num : Host Channel number
  *         This parameter can be a value from 1 to 15
  * @retval HAL state
  */
HAL_StatusTypeDef USB_HC_Halt(USB_OTG_GlobalTypeDef *USBx , uint8_t hc_num)
{

#if !defined(CHIP_GM6721)
  uint32_t count = 0U;
  
  /* Check for space in the request queue to issue the halt. */
  if (((((USBx_HC(hc_num)->HCCHAR) & USB_OTG_HCCHAR_EPTYP) >> 18) == HCCHAR_CTRL) || (((((USBx_HC(hc_num)->HCCHAR) & 
  USB_OTG_HCCHAR_EPTYP) >> 18) == HCCHAR_BULK)))
  {
    USBx_HC(hc_num)->HCCHAR |= USB_OTG_HCCHAR_CHDIS;
    
    if ((USBx->HNPTXSTS & 0xFF0000U) == 0U)
    {
      USBx_HC(hc_num)->HCCHAR &= ~USB_OTG_HCCHAR_CHENA;
      USBx_HC(hc_num)->HCCHAR |= USB_OTG_HCCHAR_CHENA;  
      do 
      {
        if (++count > 1000U) 
        {
          break;
        }
      } 
      while ((USBx_HC(hc_num)->HCCHAR & USB_OTG_HCCHAR_CHENA) == USB_OTG_HCCHAR_CHENA);     
    }
    else
    {
      USBx_HC(hc_num)->HCCHAR |= USB_OTG_HCCHAR_CHENA; 
    }
  }
  else
  {
    USBx_HC(hc_num)->HCCHAR |= USB_OTG_HCCHAR_CHDIS;
    
    if ((USBx_HOST->HPTXSTS & 0xFFFFU) == 0U)
    {
      USBx_HC(hc_num)->HCCHAR &= ~USB_OTG_HCCHAR_CHENA;
      USBx_HC(hc_num)->HCCHAR |= USB_OTG_HCCHAR_CHENA;  
      do 
      {
        if (++count > 1000U) 
        {
          break;
        }
      } 
      while ((USBx_HC(hc_num)->HCCHAR & USB_OTG_HCCHAR_CHENA) == USB_OTG_HCCHAR_CHENA);     
    }
    else
    {
       USBx_HC(hc_num)->HCCHAR |= USB_OTG_HCCHAR_CHENA; 
    }
  }
#else
#endif
  return HAL_OK;
}

/**
  * @brief  Initiate Do Ping protocol
  * @param  USBx : Selected device
  * @param  hc_num : Host Channel number
  *         This parameter can be a value from 1 to 15
  * @retval HAL state
  */
HAL_StatusTypeDef USB_DoPing(USB_OTG_GlobalTypeDef *USBx , uint8_t ch_num)
{
#if !defined(CHIP_GM6721)
  uint8_t  num_packets = 1U;
  uint32_t tmpreg = 0U;

  USBx_HC(ch_num)->HCTSIZ = ((num_packets << 19U) & USB_OTG_HCTSIZ_PKTCNT) |\
                                USB_OTG_HCTSIZ_DOPING;
  
  /* Set host channel enable */
  tmpreg = USBx_HC(ch_num)->HCCHAR;
  tmpreg &= ~USB_OTG_HCCHAR_CHDIS;
  tmpreg |= USB_OTG_HCCHAR_CHENA;
  USBx_HC(ch_num)->HCCHAR = tmpreg;
  
  return HAL_OK;  
}

/**
  * @brief  Stop Host Core
  * @param  USBx : Selected device
  * @retval HAL state
  */
HAL_StatusTypeDef USB_StopHost(USB_OTG_GlobalTypeDef *USBx)
{
  uint8_t i;
  uint32_t count = 0U;
  uint32_t value;
  
  USB_DisableGlobalInt(USBx);
  
    /* Flush FIFO */
  USB_FlushTxFifo(USBx, 0x10U);
  USB_FlushRxFifo(USBx);
  
  /* Flush out any leftover queued requests. */
  for (i = 0; i <= 15; i++)
  {   

    value = USBx_HC(i)->HCCHAR ;
    value |=  USB_OTG_HCCHAR_CHDIS;
    value &= ~USB_OTG_HCCHAR_CHENA;  
    value &= ~USB_OTG_HCCHAR_EPDIR;
    USBx_HC(i)->HCCHAR = value;
  }
  
  /* Halt all channels to put them into a known state. */  
  for (i = 0; i <= 15; i++)
  {
    value = USBx_HC(i)->HCCHAR ;
    
    value |= USB_OTG_HCCHAR_CHDIS;
    value |= USB_OTG_HCCHAR_CHENA;  
    value &= ~USB_OTG_HCCHAR_EPDIR;
    
    USBx_HC(i)->HCCHAR = value;
    do 
    {
      if (++count > 1000U) 
      {
        break;
      }
    } 
    while ((USBx_HC(i)->HCCHAR & USB_OTG_HCCHAR_CHENA) == USB_OTG_HCCHAR_CHENA);
  }

  /* Clear any pending Host interrupts */  
  USBx_HOST->HAINT = 0xFFFFFFFFU;
  USBx->GINTSTS = 0xFFFFFFFFU;
  USB_EnableGlobalInt(USBx);
#else

#endif
  return HAL_OK;  
}
/**
  * @}
  */
#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F427xx || STM32F437xx || STM32F429xx || STM32F439xx ||
          STM32F401xC || STM32F401xE || STM32F411xE || STM32F446xx || STM32F469xx || STM32F479xx || STM32F412Zx || STM32F412Rx ||
          STM32F412Vx || STM32F412Cx || STM32F413xx || STM32F423xx  */
#endif /* defined(HAL_PCD_MODULE_ENABLED) || defined(HAL_HCD_MODULE_ENABLED) */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
