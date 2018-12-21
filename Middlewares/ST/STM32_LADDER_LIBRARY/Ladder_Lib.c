/**
  ******************************************************************************
  * @file    Ladder_Lib.c
  * @author  System Lab Motion Control & Automation  Team 
  * @version V1.0.0
  * @date    02-Dicember-2015
  * @brief   
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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
  
#include "Ladder_Lib.h"
#include "stdlib.h"
#include "stdbool.h"
#include "main.h"
#include "X_NUCLEO_PLC01A1.h"
#include "string.h"
#include "math.h"



/** @addtogroup PLC_Demo_Example     PLC_Demo Example 
  * @{
  */ 


/** @addtogroup MIDDLEWARE     MIDDLEWARE
  * @{
  * @brief Middleware layer
  */ 


/** @addtogroup Ladder_Library     Ladder Library
  * @{
  * @brief Ladder Library layer
  */ 


/** @defgroup Ladder_Library_Exported_Variable    Ladder Library Exported Variable
  * @{
  * @brief Exported variable
  */

extern uint8_t output_status;
extern uint8_t Input_CHS;
extern int8_t  ServerData_RX[]; 
extern Decode_Status_Typedef Frame_decoding;
extern PLC_State_TypeDef PLC_State;
extern TIM_HandleTypeDef htim2,htim1,htim,htim4;
/**
  * @}
  */

/** @defgroup Ladder_Library_Private_Variable     Ladder Library Private Variable
  * @{
  * @brief Private variable
  */
int8_t check_status=-1;
uint8_t argument=0;
uint8_t decode=0;
uint16_t dec_index=0;
uint8_t output_buffer=0;
uint8_t decoded_index=0;
uint8_t element_index=0;
uint8_t res_index=0; 
uint8_t rung_pos=0;
//uint8_t index=0;
uint8_t reset_request=0;
uint8_t var_tim=0;
uint16_t cnt1[MAX_COMPONENT_NUMBER];
uint16_t num_obj[MAX_COMPONENT_NUMBER];             /*!< number of object to count*/
uint8_t start_delay[MAX_COMPONENT_NUMBER]={0};      /*!< flag used to start the counter delay of 5 sec*/
uint16_t delay_CNT[MAX_COMPONENT_NUMBER]={0};
int8_t objects1[MAX_COMPONENT_NUMBER];              /*!< flag set when one object has been counted in up-count*/
int8_t objects2[MAX_COMPONENT_NUMBER];               /*!< flag set when one object has been counted in down-count*/
uint16_t component_index=0;
uint16_t comp_param[4];
uint8_t timer_1[MAX_COMPONENT_NUMBER];
uint8_t htim4_idx,htim1_idx=0;
uint8_t out_tim[MAX_COMPONENT_NUMBER];
uint8_t index_t,index_c=0;
uint8_t Component[512];

uint8_t element_buffer[EXPRESSION_MAX_SIZE];
uint8_t cnt1TH=255;                                  /*!< delay timer1 value threshold*/
uint8_t cnt2TH=255;                                  /*!< delay timer2 value threshold*/
 uint8_t timer1_cnt=0;                               /*!< flag set when timer1 count reach  cnt1TH threshold*/
 uint8_t timer2_cnt=0;                               /*!< flag set when timer4 count reach  cnt2TH threshold*/
 int8_t POG=-1;                                      /*!< flag set when prugromming expression is ongoing*/
/**
  * @}
  */

/** @defgroup Ladder_Library_TypedefStruct     Ladder Library TypedefStruct
  * @{
  * @brief Private variable
  */
OutputStructure_Typedef         output[MAX_OUTPUT_NUMBER];
OutputStructure_Typedef         output_temp[MAX_OUTPUT_NUMBER];
TimerStruct_Typedef             tim_setting[MAX_COMPONENT_NUMBER];
CounterStruct_Typedef           counter_up[MAX_COMPONENT_NUMBER];
/**
  * @}
  */


/** @defgroup Ladder_Library_Exported_Function     Ladder Library Exported Function
  * @{
  * @brief Exported Function
  */

/** 
  *@brief: Output buffer initialization
  *@param: None
  */
void Init_Output(void)     
{ 

       for(uint8_t i=0;i<MAX_OUTPUT_NUMBER;i++)
      {
        output[i].Programmed=0;
        output[i].output_value=-1;
        memset(output[i].Expression,0,EXPRESSION_MAX_SIZE);    
      }
       memset(Component,0,512);
       memset(comp_param,0,4);
       memset(timer_1,0,MAX_COMPONENT_NUMBER);
       memset(num_obj,0,MAX_COMPONENT_NUMBER);
     //  memset(cnt1,1,MAX_COMPONENT_NUMBER);
       HAL_TIM_Base_Stop_IT(&htim1);
       HAL_TIM_Base_Stop_IT(&htim4);
       Reset_Count();
       htim1.Instance->CNT=0;
       htim4.Instance->CNT=0;
       for(uint8_t i=0;i<MAX_COMPONENT_NUMBER;i++)
       {
         tim_setting[i].TIM_cnt=0;
         tim_setting[i].TIM_number=0;
         tim_setting[i].TIM_output=0;
         tim_setting[i].TIM_period=0;
         counter_up[i].CNT_dir=0;
         counter_up[i].CNT_number=0;
         counter_up[i].CNT_output=0;
         counter_up[i].CNT_val=0;
         num_obj[i]=0;
         cnt1[i]=1;
         
       }
      reset_request=1;
 
}



/** 
  *@brief:  Get Reset Status
  *@param:  None
  *@retval: uint8_t reset_request
  */


uint8_t Get_ResetOUT (void)
{
  return reset_request;
}


/** 
  *@brief: Clear Reset Status
  *@param: None
  */


void ClearFlag_ResetOUT (void)
{
  reset_request=0;
}


/** 
    @brief: Routine used to temporany backup
            of programmed expression for each output
    @param: None
*/

void Save (void)
{
    memcpy(output_temp,output,sizeof(output));
}


/** 
    brief: Routine used to recall backup
       of programmed expression for each output
    param: None
*/

void Restore (void)
{
  memcpy(output,output_temp,sizeof(output));
}


/** 
    @brief: Routine used to decode the WiFi incoming frame
    @param: uint8_t* frame: WiFi incoming frame
    @retval: decoding result
*/

int16_t WiFi_Decode (uint8_t* frame)   /*decode the single line in terms of logic operator and channel number*/
{
   int16_t ret_decode=0;
   int8_t decoding_event=-1;
   
   if(Frame_decoding!=Started)
   {
      Frame_decoding=Started; 
      memset(ServerData_RX, 0x00, 512);
      memset(Component,0,512);
      dec_index=0;
      decoded_index = 0;
      decode=0;
   
    do{
       
          decode=frame[dec_index++];            
          switch (decode)
            {
            case '0':
            case '1':
            case '2':
            case '3':  
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':              
              if(((Component[0]=='T')||(Component[0]=='C'))&&Component[component_index-1]!=')')
              {
                if(Component[component_index-2]!=')')            
                Component[component_index++]=decode;
                else
                  ServerData_RX[decoded_index++]= (decode - 0x30); 
              }
              else
      
                  ServerData_RX[decoded_index++]= (decode - 0x30);  
              
               break;
            case '*':
            case '+':
            case '-':
            case 'X':
            case 'Y':
            case '.':
              ServerData_RX[decoded_index++]=decode;
              break;        
            case 'O':
              if ((decoded_index > 0) &&
                  (ServerData_RX[decoded_index-1] != '-') && 
                  (ServerData_RX[decoded_index-1] != '*') && 
                  (ServerData_RX[decoded_index-1] != '+'))
                  {
                     if ((decoded_index > 1) &&
                        (ServerData_RX[decoded_index-2] != '-') && 
                        (ServerData_RX[decoded_index-2] != '*') && 
                        (ServerData_RX[decoded_index-2] != '+'))
                     {
                       if(ServerData_RX[decoded_index-2] != '.'){
                       ServerData_RX[decoded_index-2]=ServerData_RX[decoded_index-2]*10+ServerData_RX[decoded_index-1];
                       ServerData_RX[decoded_index-2]|= MASK_MSB_OUT;
                       decoded_index=decoded_index-1;}
                       else
                       {
                         ServerData_RX[decoded_index-2]=ServerData_RX[decoded_index-1]| MASK_MSB_OUT;
                         decoded_index=decoded_index-1;}
                     }
                     else
                     ServerData_RX[decoded_index-1]|= MASK_MSB_OUT;
                  }
                  else
                  {
                    decoding_event=0;
                  }
              break;
              
            case 'T':  
                Component[component_index++]=decode;  
            
              break;
            case 'F':
              check_status=1;
              break;
            case 'C':  
              Component[component_index++]=decode;           
              break;
              
            case '(':
              Component[component_index++]=decode;
           
              break;
              
            case ')':
              Component[component_index++]=decode;
             
      
              break;  
              
            case ',':
              Component[component_index++]=decode;
             
              break;
            case '=':
                if ((decoded_index > 0 )&&(ServerData_RX[decoded_index-1]<=9)&&                         
                        (ServerData_RX[decoded_index-1] >=0))
                       { 
                           decoded_index--; 
                           if((ServerData_RX[decoded_index-1]>0)&&(ServerData_RX[decoded_index-1] <=9))
                           {
                                
                               if(Component[component_index-1]==')')
                                  {
                                    if(decoded_index>=2)
                                    {
                                      uint8_t channel=ServerData_RX[decoded_index-1];
                                      rung_pos=channel*10+ServerData_RX[decoded_index];
                                      Component[component_index++]=rung_pos;
                                      decoded_index--;
                                    }
                                    else
                                    {
                                      decoding_event=0;
                                    }

                                  }
                                   else
                                   {
                                     if(decoded_index>=2)
                                        {
                                          uint8_t channel=ServerData_RX[decoded_index-1];
                                          rung_pos=channel*10+ServerData_RX[decoded_index];                               
                                          decoded_index--;
                                        }
                                        else
                                        {
                                          if(decoded_index==1)
                                            {
                                              rung_pos=ServerData_RX[decoded_index]; 
                                            }
                                            else
                                          decoding_event=0;
                                        }   
                                   }
                           }
                           else 
                           {
                             rung_pos=ServerData_RX[decoded_index];
                           }
                                          
                             memset(&output[rung_pos-1],0,EXPRESSION_MAX_SIZE);
                             output[rung_pos-1].Programmed=0; 
                             output[rung_pos-1].output_value=-1;
                           for(uint8_t a=0; a< decoded_index ; a++) 
                           {
                              output[rung_pos-1].Expression[a]=ServerData_RX[a];
                           }
                           if(decoded_index>0)
                              output[rung_pos-1].Programmed=1;                    
                              decoded_index=0;
                              memset(ServerData_RX, 0x00, 512);                     
                        }                          
                        else
                        {                              
                           decoding_event=0;
                        }
                        
              break;
            case 'S':
              ret_decode=-1;
              if(dec_index==1)//reset output
              {                
                Init_Output();
              }
            default:
              decoding_event=0;
              break;
              
            }
        
    }while((decoding_event)&&(dec_index<MAX_SERVERDATA_RX));

    if(component_index!=0)
    { 
      Component[component_index]='#';
      index_c=0;
      index_t=0;
      Component_parser();
    }        
      Frame_decoding=Completed; 
  }
  return ret_decode;
}

/** 
    @brief: Component parser to get param value related to Timer and Counter
    @retval: parser status
*/
int8_t Component_parser(void)
{

  int8_t ret=-1;
  uint8_t a=0;
  index_t=0;
  index_c=0; 
  component_index=0;
  
  memset(comp_param,0,5);
  
    do 
    {
       if(( Component[component_index]!='T')&&( Component[component_index]!='C'))
        {          
             if((Component[component_index]!=',')&&(Component[component_index]!='(')&&(Component[component_index]!=')'))
               { 
                  if((Component[component_index-1]=='(')||(Component[component_index-1]==','))
                  {
                      comp_param[a++] = Component[component_index]-0x30;
                  }
                  else
                  {   
                      comp_param[a-1]=(comp_param[a-1]*(uint8_t)pow(10,1))+(Component[component_index]-0x30);          
                  }
               } 
             else if(Component[component_index]==')')
                {
                  if(comp_param[0]=='T')
                     {
                       index_t=comp_param[1]-1;
                        tim_setting[index_t].TIM_number=comp_param[1];
                        tim_setting[index_t].TIM_period=comp_param[2];
                        tim_setting[index_t].TIM_cnt=comp_param[3];
                        tim_setting[index_t].TIM_output=Component[component_index+1];
                        PLC_TimComp_Init( tim_setting[index_t].TIM_number,tim_setting[index_t].TIM_period);
                        
                        if(index_t==0)
                        {
                        cnt1TH=tim_setting[index_t].TIM_cnt;
                        }
                          else
                          {
                            cnt2TH=tim_setting[index_t].TIM_cnt;
                          }
//                        index_t++;
                     }
                     else
                     {
                        if(comp_param[0]=='C')
                         {
                           index_c=comp_param[1]-1;
                           counter_up[index_c].CNT_number=comp_param[1];
                           counter_up[index_c].CNT_val=comp_param[2];
                           counter_up[index_c].CNT_dir=comp_param[3];
                           counter_up[index_c].CNT_output=Component[component_index+1];
                           
                           if(counter_up[index_c].CNT_dir==0)
                             num_obj[index_c]=counter_up[index_c].CNT_val;
                         }
                     }
                  
                   memset(comp_param,0,4);     
                   a=0;
                }                  
         }
       else 
         comp_param[a++]=Component[component_index];
       
         component_index++;
            
    }while (Component[component_index]!='#');
    
  memset(Component,0,512);
component_index=0;
    ret=1;  
    return ret;
}


/** 
    @brief: Routine used to evaluate the expression
    @param: uint8_t output_index: output to be evaluate
    @retval: ret

*/

uint8_t Evalute_Expression(uint8_t output_index)
{
 
  int8_t ret=-1;
  uint8_t index=0;
  res_index=0;
  index_t=0;
  index_c=0;
  
  
  memset(element_buffer,0,EXPRESSION_MAX_SIZE);

  if(output[output_index].output_value==-1)
  {
    while((argument=output[output_index].Expression[index++])!=0)
    {               
      if((argument=='*') && (res_index > 1))
      {
        Get_AND ( element_buffer, res_index);
        res_index--;
      } 
      else if((argument=='+') && (res_index > 1))
      { 
        Get_OR ( element_buffer, res_index);
        res_index--;
      } 
      else if((argument=='-') && (res_index > 0))
      { 
        Get_NOT ( element_buffer, res_index);
      } 
      else if(argument=='X')//to activate timer without input
      { 
        res_index=1;
        output[output_index].output_value=1;
        element_buffer[0]=output[output_index].output_value;
      }
      else if(argument=='Y')//to activate timer without input
      { 
        res_index=1;
        output[output_index].output_value=0;
        element_buffer[0]=output[output_index].output_value;
      }
      
      else                
      {
        if(argument & MASK_MSB_OUT)// used to evaluate if an output is used as input
        {
          uint8_t output_pos=(argument&0x1F)-1;
          if(output[output_pos].output_value!=-1) // check if an output has been evaluated or not
          {             
            if(output_pos<16)            
              element_buffer[res_index++]=output[output_pos].output_value;
            else
                {
                  index_t=0;
                  while(((tim_setting[index_t].TIM_output)!=(output_pos+1))&&(index_t<(MAX_COMPONENT_NUMBER-1)))
                  {
                    index_t++;
                  }
                  
                  if(tim_setting[index_t].TIM_number!=0)
                  {
                    if(output[output_pos].output_value==1)
                    {
                      if(tim_setting[index_t].TIM_cnt!=0)
                      {
                        if(timer_1[index_t]==0)
                        { 
                          timer_1[index_t]=1;
                          
                          
                          if(tim_setting[index_t].TIM_number==1)
                          {
                            HAL_TIM_Base_Start_IT(&htim1);
                            htim1_idx=index_t;
                          }
                          else if(tim_setting[index_t].TIM_number>1)
                          {
                            htim4_idx=index_t;
                            HAL_TIM_Base_Start_IT(&htim4);                                      
                          }
                        }
                        if(tim_setting[index_t].TIM_number==1)
                        {
                          if(timer1_cnt==0)
                          {
                            element_buffer[res_index++]=out_tim[index_t];
                          }
                          else
                          { 
                            if(timer1_cnt==1)
                            {
                              timer1_cnt=0;
                            }                        
                            
                            cnt1[index_t]=1;
                            out_tim[index_t]^=0x01;
                            element_buffer[res_index++]=out_tim[index_t]; 
                            
                          }
                        }
                        else if(tim_setting[index_t].TIM_number>1)
                        {
                          if(timer2_cnt==0)
                          {
                            element_buffer[res_index++]=out_tim[index_t];
                          }
                          else
                          { 
                            if(timer2_cnt==1)
                            {
                              timer2_cnt=0;
                            }                        
                            
                            cnt1[index_t]=1;
                            out_tim[index_t]^=0x01;
                            element_buffer[res_index++]=out_tim[index_t]; 
                            
                          }
                        }
                      }
                      else
                      {
                        out_tim[index_t]=0x01;
                        element_buffer[res_index++]=out_tim[index_t];
                      }
                    }
                    else
                    {
                      if(timer_1[index_t]==1)
                      {
                        timer_1[index_t]=0;
                        cnt1[index_t]=0;                           
                        out_tim[index_t]=0;
                        if(tim_setting[index_t].TIM_number==1)
                          HAL_TIM_Base_Stop_IT(&htim1);
                        else
                          HAL_TIM_Base_Stop_IT(&htim4); 
                      }
                      element_buffer[res_index++]=output[output_pos].output_value;
                    }
                                        
                }
                else
                { 
                  index_c=0;       
                  while((counter_up[index_c].CNT_output!=output_pos+1)&&index_c<(MAX_COMPONENT_NUMBER-1))
                  {
                    index_c++;
                  }
                  if(counter_up[index_c].CNT_number!=0)
                  {        
                    if(counter_up[index_c].CNT_dir==1)
                    {
                      if(num_obj[index_c]==counter_up[index_c].CNT_val)
                      {
                        start_delay[index_c]=1;
                        element_buffer[res_index++]=1;
                        if(delay_CNT[index_c]>=5000)
                        {
                          element_buffer[res_index++]=0;
                          ToAnalizeCounterIN();
                          objects1[index_c]=-1;
                        }
                      }
                      else
                      {
                         start_delay[index_c]=0;
                        delay_CNT[index_c]=0;                   
                        if(output[output_pos].output_value==1) 
                        {
                          if(objects1[index_c]==-1)
                          {
                            objects1[index_c]=1;
                            num_obj[index_c]++;
                          } 
                        }
                        else
                          
                        {
                          objects1[index_c]=-1;
                          
                        }
                        
                        element_buffer[res_index++]=0;
                      }
                      index_c=0;
                    } 
                    else
                    {
                      if(num_obj[index_c]==0)
                      {
                        start_delay[index_c]=1;
                        element_buffer[res_index++]=1;
                        
                        if(delay_CNT[index_c]>=5000)
                        { 
                          
                          element_buffer[res_index++]=0;
                          num_obj[index_c]=counter_up[index_c].CNT_val;
                          objects2[index_c]=-1;
                        }
                      }
                      else
                      {
                        start_delay[index_c]=0;
                        delay_CNT[index_c]=0;                    
                        if(output[output_pos].output_value==1) 
                        {
                          if(objects2[index_c]==-1)
                          {
                            objects2[index_c]=1;
                            num_obj[index_c]--;
                          } 
                        }
                        else
                        {
                          objects2[index_c]=-1;
                          
                        }
                        
                        element_buffer[res_index++]=0; 
                        
                      }
                      index_c=0;
                    }
                  }
                }
                }
          }
          else
          {
            ret=output_pos;// if output is not evaluated
            break;
          }
          
        }
        else
        {
          element_buffer[res_index++] = Get_Input(Input_CHS,(argument&0x0F));
        }          
      }
    }
    
    if((ret==-1)&&(res_index==1))//exspression evaluation correct, only correct stack
    {

      output[output_index].output_value=element_buffer[0];
      ret=Get_Next(output_index);
    }
    
  }
  else
  {
    
    ret=Get_Next(output_index);
  }
  return ret;
}


/** 
    @brief: Counter routine
    @param: None
    @retval: None

*/



void ToAnalizeCounterIN (void)
{
  uint8_t i=0;
  uint8_t exp_idx=0;
  
  while(exp_idx<EXPRESSION_MAX_SIZE)
  {
    while( ((output[counter_up[index_c].CNT_output-1].Expression[exp_idx]&0x7F)!=tim_setting[i].TIM_output)&&(i<=MAX_COMPONENT_NUMBER))
    {
      i++   ; 
    }
    exp_idx++;
  }
  if(i<=MAX_COMPONENT_NUMBER)
  {
    cnt1[i]=0;
    num_obj[index_c]=0;    
  }
  else
    num_obj[index_c]=0;  
  
}


/** 
    @brief: Reset Timer Counting
    @param: None
    @retval: None

*/
void Reset_Count (void)
{
  if(htim1.Instance->CR1&=TIM_CR1_CEN)
    cnt1[0]=0;
  if(htim4.Instance->CR1&=TIM_CR1_CEN)
 cnt1[1]=0;
  
}


                         


/** 
    @brief: Routine used to get next expression index 
    @param: uint8_t current: next expression to be evaluate 
    @retval:ret
*/


uint8_t Get_Next(uint8_t current)
{
  uint8_t ret=current;
  for(uint8_t i=0;i<MAX_OUTPUT_NUMBER;i++)
  {
    ret++;
    if(ret==MAX_OUTPUT_NUMBER)
      ret=0;
    if(output[ret].output_value==-1)
      break;
  }
  
  
  return ret; 
}




/** 
      @brief:   Routine used to know if an expression has been evaluated
      @retval:  ret
*/
uint8_t To_Evaluated (void)
{
  uint8_t ret=0;
  for(uint8_t i=0;i<MAX_OUTPUT_NUMBER;i++)
  {
    if(output[i].output_value==-1)
      ret++;
  }
  
  return ret;
}


/** 
      @brief: Routine used to build output expression
      @param: uint16_t* ToOutput: pointer to output buffer to be sent via SPI 
      @reval: ret_spi
*/


uint8_t PLC_GetOutput (uint16_t* ToOutput)    /** get the output value, evaluating input channel value based on operator Ladder programming***/
{


int8_t next=0;
uint8_t ret_spi=0;
uint8_t cycling=0; 
POG=1;
  for(uint8_t b=0;b<MAX_OUTPUT_NUMBER;b++)//reset output flag
  {
   if(output[b].Programmed==0)
     output[b].output_value=0;
  else
    output[b].output_value=-1;
  }

   while((To_Evaluated()>0)&&(cycling<=60) && (next != -1))
    {
      next=Evalute_Expression(next);//call expression evaluation
      cycling++;      
    }
    if(To_Evaluated()==0)
    {
        
        if(ToOutput!=NULL)
        {
          ret_spi=1; // all output has been evaluate correctly
          *ToOutput=0;
          for(uint8_t b=0;b<MAX_OUTPUT_NUMBER;b++)
            {

                 if(output[b].output_value==1)
                   *ToOutput=(*ToOutput|(0x01<<b));               
              
            }
        }
        else
        {         
          for(uint8_t exp=0;exp<MAX_OUTPUT_NUMBER;exp++)
          {
            if(output[exp].Programmed==0)
              output[exp].Expression[exp]=0;
          }
        }

    }
//POG=-1;
 return ret_spi;
}

/** 
      @brief: Routine used to read input epression
      @param: uint8_t* pointer: input buffer; 
              uint8_t value: channel value
      @reval: param
*/

uint8_t Get_Input(uint8_t param, uint8_t value)
{
    value=value-1;
    param=param>>value;
    param&=0x01;
  return param;
}

/** 
      @brief: Perform AND operation between input channel
      @param: uint8_t* pointer: input buffer; 
              uint8_t i: channel index
*/
                                        
                                        
 void Get_AND (uint8_t* pointer, uint8_t i)
{ 
 pointer[i-2]= pointer[i-2] && pointer[i-1];

}

/** 
      @brief: Perform OR operation between input channel
      @param: uint8_t* pointer: input buffer; 
              uint8_t inputs: channel index
*/
void Get_OR (uint8_t* pointer, uint8_t inputs)
{   
pointer[res_index-2]= pointer[res_index-2] || pointer[res_index-1];
}

//
//
//
//void Get_Masked_Operator(uint8_t* pointer, uint8_t i)
//{
//  
//  
//}
/** 
      @brief: Perform NOT operation between input channel
      @param: pointer to input buffer, channel index
      @reval: none
*/
void  Get_NOT(uint8_t* pointer,uint8_t position)
{

  pointer[position-1]=!pointer[position-1];

}


/** 
    @brief: Check received ICs status command flag
    @param: None
    @retval: None

*/                   
 int8_t Get_FlagStatus(void)
{
   return check_status;
   
}

/** 
    @brief: Reset received ICs status command flag
    @param: None
    @retval: None
*/

void Reset_FlagStatus(void)
{
   check_status=-1;
   
}
    

/**
  * @} 
  */

/**
  * @}    
  */

/**
  * @}  
  */

/**
  * @}   
  */
