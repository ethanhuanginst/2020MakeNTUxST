/**
  ******************************************************************************
  * @file    img_preprocess.h
  * @author  MCD Application Team
  * @brief   Header for img_preprocess.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IMG_PREPROC_H
#define IMG_PREPROC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32f7xx.h"
   
/*Resizing algorithm*/
#define RESIZING_NEAREST_NEIGHBOR 1
#define RESIZING_BILINEAR         2


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void RGB24_to_Float_Sym(void *, void *, uint32_t, uint32_t);
void RGB24_to_Float_Asym(void *, void *, uint32_t, uint32_t);
void Precompute_8FXP(uint8_t *, uint32_t );
void Precompute_8IntU(uint8_t *, float , int32_t );
void Precompute_8IntS(uint8_t *, float , int32_t );
void ImageResize( uint8_t* , uint32_t , uint32_t ,
                 uint32_t , uint32_t , uint32_t ,
                 uint32_t , uint32_t ,  uint8_t* ,
                 uint32_t , uint32_t);
void ImageResize_NearestNeighbor( uint8_t* , uint32_t , uint32_t ,
                            uint32_t , uint32_t , uint32_t ,
                            uint32_t , uint32_t ,  uint8_t* ,
                            uint32_t , uint32_t);
void ImageResize_Bilinear( uint8_t* , uint32_t , uint32_t ,
                     uint32_t , uint32_t , uint32_t ,
                     uint32_t , uint32_t ,  uint8_t* ,
                     uint32_t , uint32_t);

#ifdef __cplusplus
}
#endif

#endif /*IMG_PREPROC_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
