/**
  ******************************************************************************
  * @file    img_preprocess.c
  * @author  MCD Application Team
  * @brief   Library of functions for image preprocessing before NN inference
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

/* Includes ------------------------------------------------------------------*/
#include "img_preprocess.h"


/** @addtogroup Middlewares
  * @{
  */

/** @addtogroup STM32_Image
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define RESIZING_ALGO RESIZING_NEAREST_NEIGHBOR
/* Private macros ------------------------------------------------------------*/
#define _MIN(x_, y_) \
    ( ((x_)<(y_)) ? (x_) : (y_) )

#define _MAX(x_, y_) \
    ( ((x_)>(y_)) ? (x_) : (y_) )

#define _CLAMP(x_, min_, max_, type_) \
    (type_) (_MIN(_MAX(x_, min_), max_))

#define _ROUND(v_, type_) \
    (type_) ( ((v_)<0) ? ((v_)-0.5f) : ((v_)+0.5f) )

      
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Performs pixel conversion from 8-bits integer to float simple precision with asymmetric normalization, i.e. in the range [0,+1]
  * @param  pSrc     Pointer to source buffer
  * @param  pDst     Pointer to destination buffer
  * @param  pixels   Number of pixels
  * @param  rb_swap  Swap (if set to 1) or not (if set to 0) the r and b components
  * @retval void     None
  */
void RGB24_to_Float_Asym(void *pSrc, void *pDst, uint32_t pixels, uint32_t rb_swap)
{
  struct rgb
  {
    uint8_t r, g, b;
  };
  struct rgbf
  {
    float r, g, b;
  };
  struct rgb *pivot = (struct rgb *) pSrc;
  struct rgbf *dest = (struct rgbf *) pDst;
  
  /*NN input data in the range [0 , +1]*/
  if(rb_swap == 1)
  {
    for (int i = 0; i < pixels; i++)
    {
      dest[i].r = (((float)(pivot[i].b)) / 255.0F);
      dest[i].g = (((float)(pivot[i].g)) / 255.0F);
      dest[i].b = (((float)(pivot[i].r)) / 255.0F);
    }
  }
  else /*if(rb_swap == 0)*/
  {
    
    for (int i = 0; i < pixels; i++)
    {
      dest[i].r = (((float)(pivot[i].r)) / 255.0F);
      dest[i].g = (((float)(pivot[i].g)) / 255.0F);
      dest[i].b = (((float)(pivot[i].b)) / 255.0F);
    }
  }
}

/**
  * @brief  Performs pixel conversion from 8-bits integer to float simple precision with symmetric normalization, i.e. in the range [-1,+1]
  * @param  pSrc     Pointer to source buffer
  * @param  pDst     Pointer to destination buffer
  * @param  pixels   Number of pixels
  * @param  rb_swap  Swap (if set to 1) or not (if set to 0) the r and b components
  * @retval void     None
  */
void RGB24_to_Float_Sym(void *pSrc, void *pDst, uint32_t pixels, uint32_t rb_swap)
{
  struct rgb
  {
    uint8_t r, g, b;
  };
  struct rgbf
  {
    float r, g, b;
  };
  struct rgb *pivot = (struct rgb *) pSrc;
  struct rgbf *dest = (struct rgbf *) pDst;
  
  /*NN input data in the range [-1 , +1]*/
  if(rb_swap == 1)
  {
    for (int i = 0; i < pixels; i++)
    {
      dest[i].r = ((float)pivot[i].b / 127.5F) - 1.0F;
      dest[i].g = ((float)pivot[i].g / 127.5F) - 1.0F;
      dest[i].b = ((float)pivot[i].r / 127.5F) - 1.0F;
    }
  }
  else /*if(rb_swap == 0)*/
  {
    for (int i = 0; i < pixels; i++)
    {
      dest[i].r = ((float)pivot[i].r / 127.5F) - 1.0F;
      dest[i].g = ((float)pivot[i].g / 127.5F) - 1.0F;
      dest[i].b = ((float)pivot[i].b / 127.5F) - 1.0F;
    }
  }
}

 /**
  * @brief Performs pixel conversion from 8-bits integer to 8-bits "Fixed-point Qm,n" quantized format expected by NN input with normalization in the range [0,+1]
  * 
  * @param lut            Look Up Table for storing the precomputed values that will be subsequently used for the pixel conversion at execution time     
  * @param q_input_shift  Shift to be applied to be compatible with the Qm,n format expected by the input layer of the quantized model
  */
void Precompute_8FXP(uint8_t *lut, uint32_t q_input_shift)
{
  uint32_t index;
  
 for(index=0;index<256;index++)
 {
   *(lut+index)=__USAT((index + (1 << q_input_shift)) >> (1 + q_input_shift), 8);
 }
}

 /**
  * @brief  Performs pixel conversion from 8-bits integer to 8-bits "Integer Arithmetic Unsigned" quantized format expected by NN input with normalization in the range [0,+1] 
  * 
  * @param lut   Look Up Table for storing the precomputed values that will be subsequently used for the pixel conversion at execution time
  * @param scale Scale factor value to be used for the conversion to "Integer Arithmetic Unsigned" quantized format
  * @param zp    Zero-Point value to be used for the conversion to "Integer Arithmetic Unsigned" quantized format
  */
void Precompute_8IntU(uint8_t *lut, float scale, int32_t zp)
{
  uint32_t index;
  
  scale=1.0f/(scale*255.0f);
  
  for(index=0;index<256;index++)
  {
    *(lut+index)=_CLAMP(zp + _ROUND(index * scale, int32_t), 0, 255, uint8_t);
  }
}

 /**
  * @brief  Performs pixel conversion from 8-bits integer to 8-bits "Integer Arithmetic Signed" quantized format expected by NN input with normalization in the range [0,+1] 
  * 
  * @param lut   Look Up Table for storing the precomputed values that will be subsequently used for the pixel conversion at execution time
  * @param scale Scale factor value to be used for the conversion to "Integer Arithmetic Signed" quantized format
  * @param zp    Zero-Point value to be used for the conversion to "Integer Arithmetic Signed" quantized format
  */
void Precompute_8IntS(uint8_t *lut, float scale, int32_t zp)
{
  uint32_t index;
  
  scale=1.0f/(scale*255.0f);
  
  for(index=0;index<256;index++)
  {
    *(lut+index)=_CLAMP(zp + _ROUND(index * scale, int32_t),  -128, 127, int8_t);
  }
}

/**
* @brief  Performs image (or selected Region Of Interest) resizing using Nearest Neighbor interpolation algorithm
* @param  srcImage     Pointer to source image buffer
* @param  srcW         Source image width
* @param  srcH         Source image height
* @param  pixelSize    Number of bytes per pixel
* @param  roiX         Region Of Interest x starting location 
* @param  roiY         Region Of Interest y starting location
* @param  roiW         Region Of Interest width
* @param  roiH         Region Of Interest height
* @param  dstImage     Pointer to destination image buffer
* @param  dstW         Destination image width
* @param  dstH         Destination image height
* @retval void         None
*/
void ImageResize_NearestNeighbor(uint8_t *srcImage, uint32_t srcW, uint32_t srcH,
                                 uint32_t pixelSize, uint32_t roiX, uint32_t roiY,
                                 uint32_t roiW, uint32_t roiH,  uint8_t *dstImage,
                                 uint32_t dstW, uint32_t dstH)
{
  int x_ratio = (int)(((roiW ? roiW : srcW)<<16)/dstW)+1;
  int y_ratio = (int)(((roiH ? roiH : srcH)<<16)/dstH)+1;
  
  for (int y=0, i=0; y<dstH; y++)
  {
    int sy = (y*y_ratio)>>16;
    for (int x=0; x<dstW; x++, i+=pixelSize)
    {
      int sx = (x*x_ratio)>>16;
      
      for(int j=0; j<pixelSize; j++)
      {
        dstImage[i+j] = (uint8_t) srcImage[(((sy+roiY)*srcW) + (sx+roiX))*pixelSize + j];
      }
    }
  }
}

/**
* @brief  Performs image (or selected Region Of Interest) resizing using Bilinear interpolation algorithm
* @param  srcImage     Pointer to source image buffer
* @param  srcW         Source image width
* @param  srcH         Source image height
* @param  pixelSize    Number of bytes per pixel
* @param  roiX         Region Of Interest x starting location 
* @param  roiY         Region Of Interest y starting location
* @param  roiW         Region Of Interest width
* @param  roiH         Region Of Interest height
* @param  dstImage     Pointer to destination image buffer
* @param  dstW         Destination image width
* @param  dstH         Destination image height
* @retval void         None
*/
void ImageResize_Bilinear(uint8_t *srcImage, uint32_t srcW, uint32_t srcH,
                          uint32_t pixelSize, uint32_t roiX, uint32_t roiY,
                          uint32_t roiW, uint32_t roiH,  uint8_t *dstImage,
                          uint32_t dstW, uint32_t dstH)
{
  int32_t srcStride;
  float widthRatio;
  float heightRatio;
  
  int32_t maxWidth;
  int32_t maxHeight;
  
  float srcX, srcY, dX1, dY1, dX2, dY2;
  int32_t dstX1, srcY1, dstX2, srcY2;
  
  uint8_t *tmp1, *tmp2;
  uint8_t *p1, *p2, *p3, *p4;
  
  int32_t offset1;
  int32_t offset2;
  
  srcStride = pixelSize * srcW;
  
  widthRatio = ((roiW ? roiW : srcW) / (float) dstW);
  heightRatio = ((roiH ? roiH : srcH) / (float) dstH);
  
  /* Get horizontal and vertical limits. */
  maxWidth = (roiW ? roiW : srcW) - 1;
  maxHeight = (roiH ? roiH : srcH) - 1;
  
  for (int32_t y = 0; y < dstH; y++)
  {
    /* Get Y from source. */
    srcY = ((float) y * heightRatio) + roiY;
    srcY1 = (int32_t) srcY;
    srcY2 = (srcY1 == maxHeight) ? srcY1 : srcY1 + 1;
    dY1 = srcY - (float) srcY1;
    dY2 = 1.0f - dY1;
    
    /* Calculates the pointers to the two needed lines of the source. */
    tmp1 = srcImage + srcY1 * srcStride;
    tmp2 = srcImage + srcY2 * srcStride;
    
    for (int32_t x = 0; x < dstW; x++)
    {
      /* Get X from source. */
      srcX = x * widthRatio + roiX;
      dstX1 = (int32_t) srcX;
      dstX2 = (dstX1 == maxWidth) ? dstX1 : dstX1 + 1;
      dX1 = srcX - /*(float32)*/dstX1;
      dX2 = 1.0f - dX1;
      
      /* Calculates the four points (p1,p2, p3, p4) of the source. */
      offset1 = dstX1 * pixelSize;
      offset2 = dstX2 * pixelSize;
      p1 = tmp1 + offset1;
      p2 = tmp1 + offset2;
      p3 = tmp2 + offset1;
      p4 = tmp2 + offset2;
      /* For each channel, interpolate the four points. */
      for (int32_t ch = 0; ch < pixelSize; ch++, dstImage++, p1++, p2++, p3++, p4++)
      {
        *dstImage = (uint8_t)(dY2 * (dX2 * (*p1) + dX1 * (*p2)) + dY1 * (dX2 * (*p3) + dX1 * (*p4)));
      }
    }
  }
}

/**
* @brief  Performs image (or selected Region Of Interest) resizing
* @param  srcImage     Pointer to source image buffer
* @param  srcW         Source image width
* @param  srcH         Source image height
* @param  pixelSize    Number of bytes per pixel
* @param  roiX         Region Of Interest x starting location 
* @param  roiY         Region Of Interest y starting location
* @param  roiW         Region Of Interest width
* @param  roiH         Region Of Interest height
* @param  dstImage     Pointer to destination image buffer
* @param  dstW         Destination image width
* @param  dstH         Destination image height
* @retval void         None
*/
void ImageResize(uint8_t *srcImage, uint32_t srcW, uint32_t srcH,
                 uint32_t pixelSize, uint32_t roiX, uint32_t roiY,
                 uint32_t roiW, uint32_t roiH,  uint8_t *dstImage,
                 uint32_t dstW, uint32_t dstH)
{
#if RESIZING_ALGO == RESIZING_NEAREST_NEIGHBOR
  ImageResize_NearestNeighbor(srcImage, srcW, srcH, pixelSize, roiX, roiY,
                              roiW, roiH, dstImage, dstW, dstH);
#elif RESIZING_ALGO == RESIZING_BILINEAR
  ImageResize_Bilinear(srcImage, srcW, srcH, pixelSize, roiX, roiY,
                       roiW, roiH, dstImage, dstW, dstH);
#else
 #error Please check definition of RESIZING_ALGO define
#endif
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
