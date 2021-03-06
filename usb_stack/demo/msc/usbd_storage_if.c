/**
  ******************************************************************************
  * @file           : usbd_storage_if.c
  * @version        : v2.0_Cube
  * @brief          : Memory management layer.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbd_msc.h"
#include "teeny_usb.h"
#include "string.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @defgroup USBD_STORAGE
  * @brief Usb mass storage device module
  * @{
  */

/** @defgroup USBD_STORAGE_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Defines
  * @brief Private defines.
  * @{
  */


#if    defined(STM32F10X_HD)
#define  FLASH_SIZE   (512ul*1024ul)
#define  PAGE_SIZE    FLASH_PAGE_SIZE
#define  APP_SIZE        (16*1024ul)
#define  FLASH_SIM_MSC

#elif  defined(STM32F072xB)
#define  FLASH_SIZE   (64ul*1024ul)
#define  PAGE_SIZE    FLASH_PAGE_SIZE
#define  APP_SIZE        (16*1024ul)
#define  FLASH_SIM_MSC

#elif  defined(STM32F723xx)
#define  RAM_SIZE   (176ul*1024ul)
#define  FLASH_PAGE_SIZE   2048
#define  PAGE_SIZE    FLASH_PAGE_SIZE
#define  APP_SIZE        (16*1024ul)

#define  RAM_START_ADDR   (0x20000000ul + APP_SIZE)
#define  FLASH_SIZE   RAM_SIZE
#define  RAM_SIM_MSC

#elif defined(STM32F767xx)
#define  RAM_SIZE   (512ul*1024ul)
#define  FLASH_PAGE_SIZE   2048
#define  PAGE_SIZE    FLASH_PAGE_SIZE
#define  APP_SIZE        (16*1024ul)

#define  RAM_START_ADDR   (0x20000000ul + APP_SIZE)
#define  FLASH_SIZE   RAM_SIZE
#define  RAM_SIM_MSC


#elif defined(STM32F407xx)
#define  RAM_SIZE   (128ul*1024ul)
#define  FLASH_PAGE_SIZE   2048
#define  PAGE_SIZE    FLASH_PAGE_SIZE
#define  APP_SIZE        (16*1024ul)

#define  RAM_START_ADDR   (0x20000000ul + APP_SIZE)
#define  FLASH_SIZE   RAM_SIZE
#define  RAM_SIM_MSC

#endif

#define  MSC_START_ADDR  (0x08000000ul + APP_SIZE)
#define  MSC_SIZE        (FLASH_SIZE-APP_SIZE)

#if MSC_MEDIA_PACKET != FLASH_PAGE_SIZE
#error Flash page size and Media packet size not equal
#endif


#define STORAGE_LUN_NBR                  1

#define STORAGE_BLK_SIZ                  PAGE_SIZE
//#define STORAGE_BLK_NBR                  0x10000
//#define STORAGE_BLK_SIZ                  (0x200)
#define STORAGE_BLK_NBR                  (MSC_SIZE / STORAGE_BLK_SIZ)


void HAL_NVIC_SystemReset(void){}
uint32_t HAL_GetTick(void)
{
  static uint32_t tick_sim = 0;
  static uint32_t tick_cnt = 0;
  tick_cnt++;
  if(tick_cnt > 200000){
    tick_cnt = 0;
    tick_sim++;
  }
  return tick_sim;
}
/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_Variables
  * @brief Private variables.
  * @{
  */

/* USER CODE BEGIN INQUIRY_DATA_FS */
/** USB Mass storage Standard Inquiry Data. */
const int8_t STORAGE_Inquirydata_FS[] = {/* 36 */
  
  /* LUN 0 */
  0x00,
  0x80,
  0x02,
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,	
  0x00,
  'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ', /* Manufacturer : 8 bytes */
  'P', 'r', 'o', 'd', 'u', 'c', 't', ' ', /* Product      : 16 Bytes */
  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
  '0', '.', '0' ,'1'                      /* Version      : 4 Bytes */
}; 
/* USER CODE END INQUIRY_DATA_FS */

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_STORAGE_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t STORAGE_Init_FS(uint8_t lun);
static int8_t STORAGE_GetCapacity_FS(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
static int8_t STORAGE_IsReady_FS(uint8_t lun);
static int8_t STORAGE_IsWriteProtected_FS(uint8_t lun);
static int8_t STORAGE_Read_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
static int8_t STORAGE_Write_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
static int8_t STORAGE_GetMaxLun_FS(void);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_StorageTypeDef USBD_Storage_Interface_fops_FS =
{
  STORAGE_Init_FS,
  STORAGE_GetCapacity_FS,
  STORAGE_IsReady_FS,
  STORAGE_IsWriteProtected_FS,
  STORAGE_Read_FS,
  STORAGE_Write_FS,
  STORAGE_GetMaxLun_FS,
  (int8_t *)STORAGE_Inquirydata_FS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  .
  * @param  lun: .
  * @param  block_num: .
  * @param  block_size: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_GetCapacity_FS(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
  /* USER CODE BEGIN 3 */
  *block_num  = STORAGE_BLK_NBR;
  *block_size = STORAGE_BLK_SIZ;
  return (USBD_OK);
  /* USER CODE END 3 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_IsReady_FS(uint8_t lun)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_IsWriteProtected_FS(uint8_t lun)
{
  /* USER CODE BEGIN 5 */
  return (USBD_OK);
  /* USER CODE END 5 */
}

#if defined(FLASH_SIM_MSC)

void erase_page(uint32_t addr)
{
  uint32_t PAGEError;
  FLASH_EraseInitTypeDef EraseInitStruct;
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = addr;
  EraseInitStruct.NbPages     = 1;
  if(HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK){
    while(1);
  }
}


void write_buf(uint32_t addr, uint8_t* buf, uint32_t size)
{
  uint32_t endAddr = addr+size;
  uint32_t* pData = (uint32_t*)buf;
  while(addr<endAddr){
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, *pData);
    addr+=4;
    pData++;
  }
}

void write_data(uint32_t addr, uint8_t* buf, uint32_t size)
{
  addr += MSC_START_ADDR;
  while(size >= PAGE_SIZE){
    erase_page(addr);
    write_buf(addr,buf, PAGE_SIZE);
    size-=PAGE_SIZE;
    addr+=PAGE_SIZE;
  };
  return;
}

void read_data(uint32_t addr, uint8_t* buf, uint32_t size)
{
  memcpy(buf, (uint8_t*)(addr+MSC_START_ADDR), size);
}

void init_data(void)
{
  HAL_FLASH_Unlock();
}
#elif defined(RAM_SIM_MSC)
void init_data(void){}
void read_data(uint32_t addr, uint8_t* buf, uint32_t size)
{
  memcpy(buf, (uint8_t*)(addr+RAM_START_ADDR), size);
}
void write_data(uint32_t addr, uint8_t* buf, uint32_t size)
{
  memcpy((uint8_t*)(addr+RAM_START_ADDR), buf, size);
}
#else
#error Unknown MSC media

#endif
/**
  * @brief  Initializes over USB FS IP
  * @param  lun:
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Init_FS(uint8_t lun)
{
  /* USER CODE BEGIN 2 */
  init_data();
  return (USBD_OK);
  /* USER CODE END 2 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Read_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
  /* USER CODE BEGIN 6 */
  uint32_t  addr = (blk_addr*STORAGE_BLK_SIZ);
  read_data(addr, buf, blk_len*STORAGE_BLK_SIZ);
  return (USBD_OK);
  /* USER CODE END 6 */
}

/**
  * @brief  .
  * @param  lun: .
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t STORAGE_Write_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
  /* USER CODE BEGIN 7 */
  write_data((blk_addr * STORAGE_BLK_SIZ),
            buf,
            blk_len*STORAGE_BLK_SIZ);
  return (USBD_OK);
  /* USER CODE END 7 */
}

/**
  * @brief  .
  * @param  None
  * @retval .
  */
int8_t STORAGE_GetMaxLun_FS(void)
{
  /* USER CODE BEGIN 8 */
  return (STORAGE_LUN_NBR - 1);
  /* USER CODE END 8 */
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
