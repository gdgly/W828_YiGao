#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

/*************************************************************************
 * INCLUDES
 *************************************************************************/
#include "hyhwChip.h"
#include "hyTypes.h"
//#include "HycRightConfig.h"
/*************************************************************************
 * DEFINES
 *************************************************************************/

/* Bit-defines for GPIO lines */
#define GPIO_0  (0x00000001)            
#define GPIO_1  (0x00000002)            
#define GPIO_2  (0x00000004)            
#define GPIO_3  (0x00000008)      
#define GPIO_4  (0x00000010)        
#define GPIO_5  (0x00000020)        
#define GPIO_6  (0x00000040)        
#define GPIO_7  (0x00000080)            
#define GPIO_8  (0x00000100)            
#define GPIO_9  (0x00000200)            
#define GPIO_10 (0x00000400)            
#define GPIO_11 (0x00000800)            
#define GPIO_12 (0x00001000)            
#define GPIO_13 (0x00002000)            
#define GPIO_14 (0x00004000)            
#define GPIO_15 (0x00008000)            
#define GPIO_16 (0x00010000)           
#define GPIO_17 (0x00020000)           
#define GPIO_18 (0x00040000)           
#define GPIO_19 (0x00080000)           
#define GPIO_20 (0x00100000)           
#define GPIO_21 (0x00200000)           
#define GPIO_22 (0x00400000)           
#define GPIO_23 (0x00800000)           
#define GPIO_24 (0x01000000)           
#define GPIO_25 (0x02000000)           
#define GPIO_26 (0x04000000)           
#define GPIO_27 (0x08000000)           
#define GPIO_28 (0x10000000)                    
#define GPIO_29 (0x20000000)               
#define GPIO_30 (0x40000000)               
#define GPIO_31 (0x80000000)            
           

/****************************************************
*
*����GPI0�����I/0������
*****************************************************/ 
#define PORT_A						0
#define PORT_B						1
#define PORT_C						2
#define PORT_D						3			
#define PORT_E						4
#define PORT_F						5

#define GPIO_NUM					6



#define GPIO_GROUP_REGISTER_BASEADDR	TCC7901_GPIO_GROUP_REGISTER_BASEADDR
#define PULL_BASEADDR					TCC7901_GPIO_PULL_CONTROL_BASEADDR

/*--------If a certain bit of this Register is set, the correspongding GPIO pin act as Normal GPIO pin.------*/

#define	PORTCFG0	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER0	/* GPIOC[23:0] */
#define	PORTCFG1	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER1	/* GPIOC[31:25],GPIOF[27]*/ 
#define	PORTCFG2	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER2	/* GPIOF[26:22],GPIOF[19:7] */ /*GPIOF[21]*/
#define	PORTCFG3	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER3	/* GPIOF[6:0],,GPIOD[5] */
#define	PORTCFG4	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER4	/* GPIOA[5:2],GPIOD[8:6],GPIOC[24] */
#define	PORTCFG5	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER5	/* GPIOB[6:0],GPIOF[28] */
#define	PORTCFG6	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER6	/* GPIOB[14:7] */
#define	PORTCFG7	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER7	/* GPIOA[8:6],GPIOB[15],GPIOD[12:9] */
#define	PORTCFG8	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER8	/* GPIOA[12:9],GPIOE[27:24] */
#define	PORTCFG9	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER9	/* GPIOE[31:28],GPIOE[3:0] */
#define	PORTCFG10	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER10	/* GPIOE[11:4],*//*HAVE NO GPIOE[3:0] */
#define	PORTCFG11	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER11	/* GPIOF[31:29],GPIOE[20],GPIOD[4:0],GPIOA[1:0] */
#define	PORTCFG12	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER12	/* GPIOE[16:12],GPIOE[23:21] */
#define	PORTCFG13	TCC7901_GPIO_PORT_CONFIGURATION_REGISTER13	/* GPIOE[19:17],GPIOF[20] */

/*-------------�Զ˿�Ϊ��λ������˿ڵľ���λ�ڲ�ͬ���üĴ����еľ�����һλ��--------------*/
/*   PORT_A   */
#define GPIOA_0to1				BIT4|BIT5|BIT6|BIT7       /*GPIOA[1,0]��PORTCFG11�еĿ���λΪ4��5��6��7 */
#define GPIOA_2					BIT16|BIT17|BIT18|BIT19   /*GPIO[5,2]��PORTCFG4�е���Ӧ����λ  */
#define GPIOA_3					BIT12|BIT13|BIT14|BIT15
#define GPIOA_4					BIT8|BIT9|BIT10|BIT11
#define GPIOA_5					BIT4|BIT5|BIT6|BIT7
#define GPIOA_6					BIT8|BIT9|BIT10|BIT11	  /*GPIOA[8,6]��PORTCFG7�е���Ӧ����λ  */
#define GPIOA_7					BIT4|BIT5|BIT6|BIT7
#define GPIOA_8					BIT0|BIT1|BIT2|BIT3
#define GPIOA_9					BIT28|BIT29|BIT30|BIT31   /*GPIOA[12,9]��PORTCFG8�е���Ӧ����λ  */
#define GPIOA_10				BIT24|BIT25|BIT26|BIT27 
#define GPIOA_11				BIT20|BIT21|BIT22|BIT23
#define GPIOA_12				BIT16|BIT17|BIT18|BIT19
/*   PORT_B   */
#define GPIOB_0					BIT24|BIT25|BIT26|BIT27	  /*GPIOB[6,0]��PORTCFG5�е���Ӧ����λ  */
#define GPIOB_1					BIT20|BIT21|BIT22|BIT23
#define GPIOB_2					BIT16|BIT17|BIT18|BIT19
#define GPIOB_3	   				BIT12|BIT13|BIT14|BIT15
#define GPIOB_4					BIT8|BIT9|BIT10|BIT11	
#define GPIOB_5					BIT4|BIT5|BIT6|BIT7
#define GPIOB_6					BIT0|BIT1|BIT2|BIT3
#define GPIOB_7					BIT28|BIT29|BIT30|BIT31   /*GPIOB[14,7]��PORTCFG6�е���Ӧ����λ  */
#define GPIOB_8					BIT24|BIT25|BIT26|BIT27
#define GPIOB_9					BIT20|BIT21|BIT22|BIT23
#define GPIOB_10				BIT16|BIT17|BIT18|BIT19
#define GPIOB_11	    		BIT12|BIT13|BIT14|BIT15
#define GPIOB_12				BIT8|BIT9|BIT10|BIT11	
#define GPIOB_13				BIT4|BIT5|BIT6|BIT7
#define GPIOB_14				BIT0|BIT1|BIT2|BIT3
#define GPIOB_15				BIT28|BIT29|BIT30|BIT31	  /*GPIOB[15]��PORTCFG7�е���Ӧ����λ  */
/*   PORT_C   */
#define GPIOC_0to7      		BIT0|BIT1|BIT2|BIT3       /*GPIOC[23,0]��PORTCFG0�е���Ӧ����λ  */
#define GPIOC_8to15     		BIT4|BIT5|BIT6|BIT7
#define GPIOC_16to17    		BIT8|BIT9|BIT10|BIT11
#define GPIOC_18to19			BIT12|BIT13|BIT14|BIT15
#define GPIOC_20				BIT16|BIT17|BIT18|BIT19
#define GPIOC_21				BIT20|BIT21|BIT22|BIT23
#define GPIOC_22       	 		BIT24|BIT25|BIT26|BIT27	
#define GPIOC_23        		BIT28|BIT29|BIT30|BIT31	
#define GPIOC_24        		BIT0|BIT1|BIT2|BIT3       /*GPIOC[24]��PORTCFG4�е���Ӧ����λ  */
#define GPIOC_25        		BIT28|BIT29|BIT30|BIT31	  /*GPIOC[31,25]��PORTCFG1�е���Ӧ����λ  */
#define GPIOC_26        		BIT24|BIT25|BIT26|BIT27
#define GPIOC_27				BIT20|BIT21|BIT22|BIT23
#define GPIOC_28				BIT16|BIT17|BIT18|BIT19
#define GPIOC_29    			BIT12|BIT13|BIT14|BIT15
#define GPIOC_30        		BIT8|BIT9|BIT10|BIT11
#define GPIOC_31        		BIT4|BIT5|BIT6|BIT7
/*   PORT_D   */
#define GPIOD_0         		BIT24|BIT25|BIT26|BIT27		/*GPIOD[4,0]��PORTCFG11�е���Ӧ����λ  */
#define GPIOD_1		    		BIT20|BIT21|BIT22|BIT23
#define GPIOD_2					BIT16|BIT17|BIT18|BIT19
#define GPIOD_3					BIT12|BIT13|BIT14|BIT15
#define GPIOD_4	        		BIT8|BIT9|BIT10|BIT11
#define GPIOD_5	        		BIT0|BIT1|BIT2|BIT3         /*GPIOD[5]��PORTCFG3�е���Ӧ����λ  */
#define GPIOD_6					BIT28|BIT29|BIT30|BIT31		/*GPIOD[8,6]��PORTCFG4�е���Ӧ����λ  */
#define GPIOD_7					BIT24|BIT25|BIT26|BIT27
#define GPIOD_8         		BIT20|BIT21|BIT22|BIT23
#define GPIOD_9					BIT24|BIT25|BIT26|BIT27		/*GPIOD[12,9]��PORTCFG7�е���Ӧ����λ  */
#define GPIOD_10				BIT20|BIT21|BIT22|BIT23	
#define GPIOD_11				BIT16|BIT17|BIT18|BIT19		 
#define GPIOD_12				BIT12|BIT13|BIT14|BIT15
/*   PORT_E   */
#define GPIOE_0    				BIT12|BIT13|BIT14|BIT15     /*GPIOE[3,0]��PORTCFG9�е���Ӧ����λ  */
#define GPIOE_1					BIT8|BIT9|BIT10|BIT11
#define GPIOE_2		    		BIT4|BIT5|BIT6|BIT7
#define GPIOE_3					BIT0|BIT1|BIT2|BIT3
#define GPIOE_4					BIT28|BIT29|BIT30|BIT31	    /*GPIOE[11,4]��PORTCFG10�е���Ӧ����λ  */
#define GPIOE_5					BIT24|BIT25|BIT26|BIT27
#define GPIOE_6					BIT20|BIT21|BIT22|BIT23
#define GPIOE_7					BIT16|BIT17|BIT18|BIT19
#define GPIOE_8					BIT12|BIT13|BIT14|BIT15
#define GPIOE_9					BIT8|BIT9|BIT10|BIT11
#define GPIOE_10				BIT4|BIT5|BIT6|BIT7
#define GPIOE_11				BIT0|BIT1|BIT2|BIT3
#define GPIOE_12				BIT16|BIT17|BIT18|BIT19      /*GPIOE[16,12]��PORTCFG12�е���Ӧ����λ  */
#define GPIOE_13				BIT12|BIT13|BIT14|BIT15
#define GPIOE_14				BIT8|BIT9|BIT10|BIT11
#define GPIOE_15				BIT4|BIT5|BIT6|BIT7
#define GPIOE_16				BIT0|BIT1|BIT2|BIT3
#define GPIOE_17				BIT28|BIT29|BIT30|BIT31	     /*GPIOE[19,17]��PORTCFG13�е���Ӧ����λ  */
#define GPIOE_18				BIT24|BIT25|BIT26|BIT27
#define GPIOE_19				BIT20|BIT21|BIT22|BIT23
#define GPIOE_20				BIT0|BIT1|BIT2|BIT3			 /*GPIOE[20]��PORTCFG11�е���Ӧ����λ  */
#define GPIOE_21				BIT24|BIT25|BIT26|BIT27		 /*GPIOE[23,21]��PORTCFG12�е���Ӧ����λ  */
#define GPIOE_22				BIT20|BIT21|BIT22|BIT23
#define GPIOE_23				BIT28|BIT29|BIT30|BIT31
#define GPIOE_24				BIT12|BIT13|BIT14|BIT15		 /*GPIOE[27,24]��PORTCFG8�е���Ӧ����λ  */
#define GPIOE_25				BIT8|BIT9|BIT10|BIT11
#define GPIOE_26				BIT4|BIT5|BIT6|BIT7
#define GPIOE_27				BIT0|BIT1|BIT2|BIT3
#define GPIOE_28				BIT28|BIT29|BIT30|BIT31	     /*GPIOE[31,28]��PORTCFG9�е���Ӧ����λ  */
#define GPIOE_29				BIT24|BIT25|BIT26|BIT27
#define GPIOE_30				BIT20|BIT21|BIT22|BIT23
#define GPIOE_31				BIT16|BIT17|BIT18|BIT19

/*   PORT_F  */
#define GPIOF_0					BIT4|BIT5|BIT6|BIT7				/*GPIOF[6,0]��PORTCFG3�е���Ӧ����λ  */
#define GPIOF_1					BIT8|BIT9|BIT10|BIT11
#define GPIOF_2					BIT12|BIT13|BIT14|BIT15
#define GPIOF_3					BIT16|BIT17|BIT18|BIT19			
#define GPIOF_4					BIT20|BIT21|BIT22|BIT23
#define GPIOF_5					BIT24|BIT25|BIT26|BIT27	
#define GPIOF_6					BIT28|BIT29|BIT30|BIT31
#define GPIOF_7					BIT0|BIT1|BIT2|BIT3				/*GPIOF[19,7]��PORTCFG2�е���Ӧ����λ  */
#define GPIOF_8to11				BIT4|BIT5|BIT6|BIT7	
#define GPIOF_12to15			BIT8|BIT9|BIT10|BIT11
#define GPIOF_16to17			BIT12|BIT13|BIT14|BIT15
#define GPIOF_18to19_21to23 	BIT16|BIT17|BIT18|BIT19 /*GPIOF[23,21]��PORTCFG2�е���Ӧ����λ  */
#define GPIOF_20   				BIT0|BIT1|BIT2|BIT3				/*GPIOF[20]��PORTCFG13�е���Ӧ����λ  */
#define GPIOF_24        		BIT20|BIT21|BIT22|BIT23         
#define GPIOF_25				BIT24|BIT25|BIT26|BIT27	
#define GPIOF_26				BIT28|BIT29|BIT30|BIT31
#define GPIOF_27				BIT0|BIT1|BIT2|BIT3				/*GPIOF[27]��PORTCFG1�е���Ӧ����λ  */
#define GPIOF_28				BIT28|BIT29|BIT30|BIT31			/*GPIOF[28]��PORTCFG5�е���Ӧ����λ  */
#define GPIOF_29to31			BIT28|BIT29|BIT30|BIT31			/*GPIOF[29,31]��PORTCFG11�е���Ӧ����λ  */


/*-------------���üĴ����У����ÿ����Ƴ�GPIO�ڵĶ�Ӧֵ------------------------------------*/
/*   PORT_A   */
#define GPIOA_0to1_GPIO      PORTCFG11 & (~(GPIOA_0to1))     	/*����ֵΪ0*/
#define GPIOA_2_GPIO		 PORTCFG4 & (~(GPIOA_2))
#define GPIOA_3_GPIO		 PORTCFG4 & (~(GPIOA_3))
#define GPIOA_4_GPIO		 PORTCFG4 & (~(GPIOA_4))
#define GPIOA_5_GPIO		 PORTCFG4 & (~(GPIOA_5))
#define GPIOA_6_GPIO	   	 PORTCFG7 & (~(GPIOA_6))
#define GPIOA_7_GPIO		 PORTCFG7 & (~(GPIOA_7))
#define GPIOA_8_GPIO	 	 PORTCFG7 & (~(GPIOA_8))
#define GPIOA_9_GPIO		 PORTCFG8 & (~(GPIOA_9))
#define GPIOA_10_GPIO		 PORTCFG8 & (~(GPIOA_10))
#define GPIOA_11_GPIO		 PORTCFG8 & (~(GPIOA_11))
#define GPIOA_12_GPIO		 PORTCFG8 & (~(GPIOA_12))

/*   PORT_B   */
#define GPIOB_0_GPIO			PORTCFG5 & (~(GPIOB_0))         /*����ֵΪ0*/
#define GPIOB_1_GPIO	    	PORTCFG5 & (~(GPIOB_1))
#define GPIOB_2_GPIO			PORTCFG5 & (~(GPIOB_2))
#define GPIOB_3_GPIO	    	PORTCFG5 & (~(GPIOB_3))
#define GPIOB_4_GPIO	    	PORTCFG5 & (~(GPIOB_4))
#define GPIOB_5_GPIO			PORTCFG5 & (~(GPIOB_5))
#define GPIOB_6_GPIO	    	PORTCFG5 & (~(GPIOB_6))
#define GPIOB_7_GPIO	    	PORTCFG6 & (~(GPIOB_7))
#define GPIOB_8_GPIO			PORTCFG6 & (~(GPIOB_8))
#define GPIOB_9_GPIO	    	PORTCFG6 & (~(GPIOB_9))
#define GPIOB_10_GPIO	    	PORTCFG6 & (~(GPIOB_10))
#define GPIOB_11_GPIO			PORTCFG6 & (~(GPIOB_11))
#define GPIOB_12_GPIO	    	PORTCFG6 & (~(GPIOB_12))
#define GPIOB_13_GPIO			PORTCFG6 & (~(GPIOB_13))
#define GPIOB_14_GPIO	    	PORTCFG6 & (~(GPIOB_14))
#define GPIOB_15_GPIO	    	PORTCFG7 & (~(GPIOB_15))

/*   PORT_C   */
#define GPIOC_0to7_GPIO	        (PORTCFG0 & (~(GPIOC_0to7))) | BIT1      /*����ֵΪ2*/
#define GPIOC_8to15_GPIO	    (PORTCFG0 & (~(GPIOC_8to15))) | BIT5
#define GPIOC_16to17_GPIO	    (PORTCFG0 & (~(GPIOC_16to17))) | BIT9
#define GPIOC_18to19_GPIO		(PORTCFG0 & (~(GPIOC_18to19))) | BIT13
#define GPIOC_20_GPIO			(PORTCFG0 & (~(GPIOC_20))) | BIT17
#define GPIOC_21_GPIO			(PORTCFG0 & (~(GPIOC_21))) | BIT21
#define GPIOC_22_GPIO	        (PORTCFG0 & (~(GPIOC_22))) | BIT25
#define GPIOC_23_GPIO	   		(PORTCFG0 & (~(GPIOC_23))) | BIT29
#define GPIOC_24_GPIO			(PORTCFG4 & (~(GPIOC_24))) | BIT0        /*����ֵΪ1*/
#define GPIOC_25_GPIO           (PORTCFG1 & (~(GPIOC_25))) | BIT29       /*����ֵΪ2*/
#define GPIOC_26_GPIO           (PORTCFG1 & (~(GPIOC_26))) | BIT25
#define GPIOC_27_GPIO		    (PORTCFG1 & (~(GPIOC_27))) | BIT21
#define GPIOC_28_GPIO		    (PORTCFG1 & (~(GPIOC_28))) | BIT17
#define GPIOC_29_GPIO    	    (PORTCFG1 & (~(GPIOC_29))) | BIT13
#define GPIOC_30_GPIO           (PORTCFG1 & (~(GPIOC_30))) | BIT9
#define GPIOC_31_GPIO           (PORTCFG1 & (~(GPIOC_31))) | BIT5

/*   PORT_D   */
#define GPIOD_0_GPIO 				 (PORTCFG11 & (~(GPIOD_0))) | BIT24		/*����ֵΪ1*/
#define GPIOD_1_GPIO 				 (PORTCFG11 & (~(GPIOD_1))) | BIT20
#define GPIOD_2_GPIO 				 (PORTCFG11 & (~(GPIOD_2))) | BIT16
#define GPIOD_3_GPIO 				 (PORTCFG11 & (~(GPIOD_3))) | BIT12
#define GPIOD_4_GPIO 				 (PORTCFG11 & (~(GPIOD_4))) | BIT8
#define GPIOD_5_GPIO 				 (PORTCFG3 & (~(GPIOD_5))) | BIT0
#define GPIOD_6_GPIO 				 (PORTCFG4 & (~(GPIOD_6))) | BIT28
#define GPIOD_7_GPIO 				 (PORTCFG4 & (~(GPIOD_7))) | BIT24
#define GPIOD_8_GPIO 				 (PORTCFG4 & (~(GPIOD_8))) | BIT20
#define GPIOD_9_GPIO 				 (PORTCFG7 & (~(GPIOD_9))) | BIT24
#define GPIOD_10_GPIO				 (PORTCFG7 & (~(GPIOD_10))) | BIT20
#define GPIOD_11_GPIO 			 	 (PORTCFG7 & (~(GPIOD_11))) | BIT16
#define GPIOD_12_GPIO 			 	 (PORTCFG7 & (~(GPIOD_12))) | BIT12
/*   PORT_E   */
#define GPIOE_0_GPIO  				(PORTCFG9 & (~(GPIOE_0))) | BIT12       /*����ֵΪ1*/
#define GPIOE_1_GPIO				(PORTCFG9 & (~(GPIOE_1))) | BIT8
#define GPIOE_2_GPIO				(PORTCFG9 & (~(GPIOE_2))) | BIT4
#define GPIOE_3_GPIO				(PORTCFG9 & (~(GPIOE_3))) | BIT0
#define GPIOE_4_GPIO				(PORTCFG10 & (~(GPIOE_4))) | BIT28		/*����ֵΪ1*/
#define GPIOE_5_GPIO				(PORTCFG10 & (~(GPIOE_5))) | BIT24
#define GPIOE_6_GPIO				(PORTCFG10 & (~(GPIOE_6))) | BIT20
#define GPIOE_7_GPIO				(PORTCFG10 & (~(GPIOE_7))) | BIT16
#define GPIOE_8_GPIO				(PORTCFG10 & (~(GPIOE_8))) | BIT12
#define GPIOE_9_GPIO				(PORTCFG10 & (~(GPIOE_9))) | BIT8
#define GPIOE_10_GPIO				(PORTCFG10 & (~(GPIOE_10))) | BIT4
#define GPIOE_11_GPIO				(PORTCFG10 & (~(GPIOE_11))) | BIT0
#define GPIOE_12_GPIO				(PORTCFG12 & (~(GPIOE_12))) | BIT16     /*����ֵΪ1*/
#define GPIOE_13_GPIO				(PORTCFG12 & (~(GPIOE_13))) | BIT12
#define GPIOE_14_GPIO				(PORTCFG12 & (~(GPIOE_14))) | BIT8
#define GPIOE_15_GPIO				(PORTCFG12 & (~(GPIOE_15))) | BIT4
#define GPIOE_16_GPIO				(PORTCFG12 & (~(GPIOE_16))) | BIT0
#define GPIOE_17_GPIO				(PORTCFG13 & (~(GPIOE_17))) | BIT28		/*����ֵΪ1*/
#define GPIOE_18_GPIO				(PORTCFG13 & (~(GPIOE_18))) | BIT24
#define GPIOE_19_GPIO				(PORTCFG13 & (~(GPIOE_19))) | BIT20
#define GPIOE_20_GPIO				(PORTCFG11 & (~(GPIOE_20))) | BIT0      /*����ֵΪ1*/
#define GPIOE_21_GPIO				(PORTCFG12 & (~(GPIOE_21))) | BIT24     /*����ֵΪ1*/
#define GPIOE_22_GPIO				(PORTCFG12 & (~(GPIOE_22))) | BIT20
#define GPIOE_23_GPIO				(PORTCFG12 & (~(GPIOE_23))) | BIT28
#define GPIOE_24_GPIO				(PORTCFG8 & (~(GPIOE_24))) | BIT12	    /*����ֵΪ1*/
#define GPIOE_25_GPIO				(PORTCFG8 & (~(GPIOE_25))) | BIT8
#define GPIOE_26_GPIO				(PORTCFG8 & (~(GPIOE_26))) | BIT4
#define GPIOE_27_GPIO				(PORTCFG8 & (~(GPIOE_27))) | BIT0
#define GPIOE_28_GPIO				(PORTCFG9 & (~(GPIOE_28))) | BIT28		/*����ֵΪ1*/
#define GPIOE_29_GPIO				(PORTCFG9 & (~(GPIOE_29))) | BIT24
#define GPIOE_30_GPIO				(PORTCFG9 & (~(GPIOE_30))) | BIT20
#define GPIOE_31_GPIO				(PORTCFG9 & (~(GPIOE_31))) | BIT16
/*   PORT_F  */
#define GPIOF_0_GPIO  			    (PORTCFG3 & (~(GPIOF_0))) | BIT4        /*����ֵΪ1*/
#define GPIOF_1_GPIO 				(PORTCFG3 & (~(GPIOF_1))) | BIT8
#define GPIOF_2_GPIO 				(PORTCFG3 & (~(GPIOF_2))) | BIT12
#define GPIOF_3_GPIO 				(PORTCFG3 & (~(GPIOF_3))) | BIT16
#define GPIOF_4_GPIO 				(PORTCFG3 & (~(GPIOF_4))) | BIT20
#define GPIOF_5_GPIO 				(PORTCFG3 & (~(GPIOF_5))) | BIT24
#define GPIOF_6_GPIO 				(PORTCFG3 & (~(GPIOF_6))) | BIT28
#define GPIOF_7_GPIO				(PORTCFG2 & (~(GPIOF_7))) | BIT0         /*����ֵΪ1*/
#define GPIOF_8to11_GPIO		    (PORTCFG2 & (~(GPIOF_8to11))) | BIT4
#define GPIOF_12to15_GPIO	        (PORTCFG2 & (~(GPIOF_12to15))) | BIT8
#define GPIOF_16to17_GPIO	        (PORTCFG2 & (~(GPIOF_16to17))) | BIT12
#define GPIOF_18to19_21to23_GPIO 	(PORTCFG2 & (~(GPIOF_18to19_21to23))) | BIT16  
#define GPIOF_20_GPIO   			(PORTCFG13 & (~(GPIOF_20))) | BIT0			/*����ֵΪ1*/
#define GPIOF_24_GPIO      			(PORTCFG2 & (~(GPIOF_24))) | BIT20			/*����ֵΪ1*/
#define GPIOF_25_GPIO				(PORTCFG2 & (~(GPIOF_25))) | BIT24
#define GPIOF_26_GPIO				(PORTCFG2 & (~(GPIOF_26))) | BIT28
#define GPIOF_27_GPIO				(PORTCFG1 & (~(GPIOF_27))) | BIT0			/*����ֵΪ1*/
#define GPIOF_28_GPIO				(PORTCFG5 & (~(GPIOF_28))) | BIT28			/*����ֵΪ1*/
#define GPIOF_29to31_GPIO			(PORTCFG11 & (~(GPIOF_29to31))) | BIT28		/*����ֵΪ1*/


/*-------------------------------group register--------------------------------------------*/
#define	GPIO_PORTA_DATA_REGISTER					TCC7901_GPIO_PORTA_DATA_REGISTER
#define	GPIO_PORTA_OUTPUT_ENABLE_REGISTER			TCC7901_GPIO_PORTA_OUTPUT_ENABLE_REGISTER
#define	GPIO_PORTA_OR_FUNCTION_REGISTER				TCC7901_GPIO_PORTA_OR_FUNCTION_REGISTER
#define	GPIO_PORTA_BIC_FUNCTION_REGISTER			TCC7901_GPIO_PORTA_BIC_FUNCTION_REGISTER
#define	GPIO_PORTA_XOR_FUNCTION_REGISTER			TCC7901_GPIO_PORTA_XOR_FUNCTION_REGISTER

#define	GPIO_PORTB_DATA_REGISTER					TCC7901_GPIO_PORTB_DATA_REGISTER
#define	GPIO_PORTB_OUTPUT_ENABLE_REGISTER			TCC7901_GPIO_PORTB_OUTPUT_ENABLE_REGISTER
#define	GPIO_PORTB_OR_FUNCTION_REGISTER				TCC7901_GPIO_PORTB_OR_FUNCTION_REGISTER
#define	GPIO_PORTB_BIC_FUNCTION_REGISTER			TCC7901_GPIO_PORTB_BIC_FUNCTION_REGISTER
#define	GPIO_PORTB_XOR_FUNCTION_REGISTER			TCC7901_GPIO_PORTB_XOR_FUNCTION_REGISTER

#define	GPIO_PORTC_DATA_REGISTER					TCC7901_GPIO_PORTC_DATA_REGISTER
#define	GPIO_PORTC_OUTPUT_ENABLE_REGISTER			TCC7901_GPIO_PORTC_OUTPUT_ENABLE_REGISTER
#define	GPIO_PORTC_OR_FUNCTION_REGISTER				TCC7901_GPIO_PORTC_OR_FUNCTION_REGISTER
#define	GPIO_PORTC_BIC_FUNCTION_REGISTER			TCC7901_GPIO_PORTC_BIC_FUNCTION_REGISTER
#define	GPIO_PORTC_XOR_FUNCTION_REGISTER			TCC7901_GPIO_PORTC_XOR_FUNCTION_REGISTER

#define	GPIO_PORTD_DATA_REGISTER					TCC7901_GPIO_PORTD_DATA_REGISTER
#define	GPIO_PORTD_OUTPUT_ENABLE_REGISTER			TCC7901_GPIO_PORTD_OUTPUT_ENABLE_REGISTER
#define	GPIO_PORTD_OR_FUNCTION_REGISTER				TCC7901_GPIO_PORTD_OR_FUNCTION_REGISTER
#define	GPIO_PORTD_BIC_FUNCTION_REGISTER			TCC7901_GPIO_PORTD_BIC_FUNCTION_REGISTER
#define	GPIO_PORTD_XOR_FUNCTION_REGISTER			TCC7901_GPIO_PORTD_XOR_FUNCTION_REGISTER

#define	GPIO_PORTE_DATA_REGISTER					TCC7901_GPIO_PORTE_DATA_REGISTER
#define	GPIO_PORTE_OUTPUT_ENABLE_REGISTER			TCC7901_GPIO_PORTE_OUTPUT_ENABLE_REGISTER
#define	GPIO_PORTE_OR_FUNCTION_REGISTER				TCC7901_GPIO_PORTE_OR_FUNCTION_REGISTER
#define	GPIO_PORTE_BIC_FUNCTION_REGISTER			TCC7901_GPIO_PORTE_BIC_FUNCTION_REGISTER
#define	GPIO_PORTE_XOR_FUNCTION_REGISTER			TCC7901_GPIO_PORTE_XOR_FUNCTION_REGISTER

#define	GPIO_PORTF_DATA_REGISTER					TCC7901_GPIO_PORTF_DATA_REGISTER
#define	GPIO_PORTF_OUTPUT_ENABLE_REGISTER			TCC7901_GPIO_PORTF_OUTPUT_ENABLE_REGISTER
#define	GPIO_PORTF_OR_FUNCTION_REGISTER				TCC7901_GPIO_PORTF_OR_FUNCTION_REGISTER
#define	GPIO_PORTF_BIC_FUNCTION_REGISTER			TCC7901_GPIO_PORTF_BIC_FUNCTION_REGISTER
#define	GPIO_PORTF_XOR_FUNCTION_REGISTER			TCC7901_GPIO_PORTF_XOR_FUNCTION_REGISTER


/*-----------------------Pull up/down control registers---------------------*/
#define	CPUD0		TCC7901_GPIO_PULL_CONTROL0_REGISTER						/* Pull-Up/Down Control Register 0 */		
#define	CPUD1		TCC7901_GPIO_PULL_CONTROL1_REGISTER						/* Pull-Up/Down Control Register 1 */
#define	CPUD2		TCC7901_GPIO_PULL_CONTROL2_REGISTER						/* Pull-Up/Down Control Register 2 */
#define	CPUD3		TCC7901_GPIO_PULL_CONTROL3_REGISTER						/* Pull-Up/Down Control Register 3  GPIOA[2] */
#define	CPUD4		TCC7901_GPIO_PULL_CONTROL4_REGISTER						/* Pull-Up/Down Control Register 4  GPIOB[10:0]*/
#define	CPUD5		TCC7901_GPIO_PULL_CONTROL5_REGISTER						/* Pull-Up/Down Control Register 5 	GPIOB[15:11] GPIOA[12:6]*/
#define	CPUD6		TCC7901_GPIO_PULL_CONTROL6_REGISTER						/* Pull-Up/Down Control Register 6 */
#define	CPUD7		TCC7901_GPIO_PULL_CONTROL7_REGISTER						/* Pull-Up/Down Control Register 7 */
#define	CPUD8		TCC7901_GPIO_PULL_CONTROL8_REGISTER						/* Pull-Up/Down Control Register 8 */
#define	CPUD9		TCC7901_GPIO_PULL_CONTROL9_REGISTER						/* Pull-Up/Down Control Register 9 */
#define	CDRV0		TCC7901_GPIO_DRIVER_STRENGTH_CONTROL0_REGISTER			/* Driver strength Control Register 0 */
#define	CDRV1		TCC7901_GPIO_DRIVER_STRENGTH_CONTROL1_REGISTER			/* Driver strength Control Register 1 */
#define	AINCFG		TCC7901_GPIO_ANALOG_INPUT_PAD_CONTROL_REGISTER			/* Analog Input Pad Control Register */
#define	EINTSEL0	TCC7901_GPIO_EXTERNAL_INTERRUPT_SELECT0_REGISTER		/* External Interrupt Select Register 0 */
#define	EINTSEL1	TCC7901_GPIO_EXTERNAL_INTERRUPT_SELECT1_REGISTER		/* External Interrupt Select Register 1 */
#define	IRQSEL		TCC7901_GPIO_INTERRUPT_SELECT_REGISTER					/* Interrupt Select Register */

/*---------------GPIO�������������ƼĴ����еľ���λ-------------------------*/
/*  PORT_A  */
#define	GPIOA_CPUD_0    	BIT6|BIT7			/*  CPUD7   */
#define	GPIOA_CPUD_1		BIT4|BIT5			/*  CPUD7   */
#define	GPIOA_CPUD_2		BIT0|BIT1			/*  CPUD3   */
#define	GPIOA_CPUD_3		BIT30|BIT31			/*  CPUD4   */
#define	GPIOA_CPUD_4		BIT28|BIT29			/*  CPUD4   */
#define	GPIOA_CPUD_5		BIT26|BIT27			/*  CPUD4   */
#define	GPIOA_CPUD_6		BIT12|BIT13			/*  CPUD5   */
#define	GPIOA_CPUD_7		BIT10|BIT11			/*  CPUD5   */
#define	GPIOA_CPUD_8		BIT8|BIT9			/*  CPUD5   */
#define	GPIOA_CPUD_9		BIT6|BIT7			/*  CPUD5   */
#define	GPIOA_CPUD_10		BIT4|BIT5			/*  CPUD5   */
#define	GPIOA_CPUD_11		BIT2|BIT3			/*  CPUD5   */
#define	GPIOA_CPUD_12		BIT0|BIT1			/*  CPUD5   */

/*  PORT_B  */
#define	GPIOB_CPUD_0		BIT20|BIT21			/*  CPUD4   */
#define	GPIOB_CPUD_1		BIT18|BIT19			/*  CPUD4   */
#define	GPIOB_CPUD_2		BIT16|BIT17			/*  CPUD4   */
#define	GPIOB_CPUD_3		BIT14|BIT15			/*  CPUD4   */
#define	GPIOB_CPUD_4		BIT12|BIT13			/*  CPUD4   */
#define	GPIOB_CPUD_5		BIT10|BIT11			/*  CPUD4   */
#define	GPIOB_CPUD_6		BIT8|BIT9			/*  CPUD4   */
#define	GPIOB_CPUD_7		BIT6|BIT7			/*  CPUD4   */
#define	GPIOB_CPUD_8		BIT4|BIT5			/*  CPUD4   */
#define	GPIOB_CPUD_9		BIT2|BIT3			/*  CPUD4   */
#define	GPIOB_CPUD_10		BIT0|BIT1			/*  CPUD4   */
#define	GPIOB_CPUD_11		BIT30|BIT31			/*  CPUD5   */
#define	GPIOB_CPUD_12		BIT28|BIT29			/*  CPUD5   */
#define	GPIOB_CPUD_13		BIT26|BIT27			/*  CPUD5   */
#define	GPIOB_CPUD_14		BIT24|BIT25			/*  CPUD5   */
#define	GPIOB_CPUD_15		BIT22|BIT23			/*  CPUD5   */

/*  PORT_C  */
#define	GPIOC_CPUD_0		BIT14|BIT15 		/*  CPUD1   */
#define	GPIOC_CPUD_1		BIT12|BIT13			/*  CPUD1   */
#define	GPIOC_CPUD_2		BIT10|BIT11			/*  CPUD1   */
#define	GPIOC_CPUD_3		BIT8|BIT9			/*  CPUD1   */
#define	GPIOC_CPUD_4		BIT6|BIT7			/*  CPUD1   */
#define	GPIOC_CPUD_5		BIT4|BIT5			/*  CPUD1   */
#define	GPIOC_CPUD_6		BIT2|BIT3			/*  CPUD1   */
#define	GPIOC_CPUD_7		BIT0|BIT1			/*  CPUD1   */
#define	GPIOC_CPUD_8		BIT0|BIT1			/*  CPUD0   */
#define	GPIOC_CPUD_9		BIT2|BIT3			/*  CPUD0   */
#define	GPIOC_CPUD_10		BIT4|BIT5			/*  CPUD0   */
#define	GPIOC_CPUD_11		BIT6|BIT7			/*  CPUD0   */
#define	GPIOC_CPUD_12		BIT8|BIT9			/*  CPUD0   */
#define	GPIOC_CPUD_13		BIT10|BIT11			/*  CPUD0   */
#define	GPIOC_CPUD_14		BIT12|BIT13			/*  CPUD0   */
#define	GPIOC_CPUD_15		BIT14|BIT15			/*  CPUD0   */
#define	GPIOC_CPUD_16		BIT16|BIT17			/*  CPUD0   */
#define	GPIOC_CPUD_17		BIT18|BIT19			/*  CPUD0   */
#define	GPIOC_CPUD_18		BIT20|BIT21			/*  CPUD0   */
#define	GPIOC_CPUD_19		BIT22|BIT23			/*  CPUD0   */
#define	GPIOC_CPUD_20		BIT24|BIT25			/*  CPUD0   */
#define	GPIOC_CPUD_21		BIT26|BIT27			/*  CPUD0   */
#define	GPIOC_CPUD_22		BIT28|BIT29			/*  CPUD0   */
#define	GPIOC_CPUD_23		BIT30|BIT31			/*  CPUD0   */
#define	GPIOC_CPUD_24		BIT24|BIT25			/*  CPUD4   */
#define	GPIOC_CPUD_25		BIT16|BIT17			/*  CPUD1   */
#define	GPIOC_CPUD_26		BIT18|BIT19			/*  CPUD1   */
#define	GPIOC_CPUD_27		BIT20|BIT21			/*  CPUD1   */
#define	GPIOC_CPUD_28	  	BIT22|BIT23			/*  CPUD1   */
#define	GPIOC_CPUD_29	  	BIT24|BIT25			/*  CPUD1   */
#define	GPIOC_CPUD_30		BIT26|BIT27			/*  CPUD1   */
#define	GPIOC_CPUD_31	 	BIT28|BIT29			/*  CPUD1   */

/*  PORT_D  */
#define	GPIOD_CPUD_0		BIT16|BIT17 		/*  CPUD7   */
#define	GPIOD_CPUD_1	 	BIT14|BIT15			/*  CPUD7   */
#define	GPIOD_CPUD_2		BIT12|BIT13			/*  CPUD7   */
#define	GPIOD_CPUD_3		BIT10|BIT11			/*  CPUD7   */
#define	GPIOD_CPUD_4		BIT8|BIT9			/*  CPUD7   */
#define	GPIOD_CPUD_5		BIT8|BIT9			/*  CPUD3   */
#define	GPIOD_CPUD_6		BIT6|BIT7			/*  CPUD3   */
#define	GPIOD_CPUD_7		BIT4|BIT5			/*  CPUD3   */
#define	GPIOD_CPUD_8		BIT2|BIT3			/*  CPUD3   */
#define	GPIOD_CPUD_9		BIT20|BIT21			/*  CPUD5   */
#define	GPIOD_CPUD_10		BIT18|BIT19			/*  CPUD5   */
#define	GPIOD_CPUD_11		BIT16|BIT17			/*  CPUD5   */
#define	GPIOD_CPUD_12	  	BIT14|BIT15			/*  CPUD5   */

/*  PORT_E  */
#define	GPIOE_CPUD_0		BIT14|BIT15 		/*  CPUD6   */
#define	GPIOE_CPUD_1		BIT12|BIT13 		/*  CPUD6   */
#define	GPIOE_CPUD_2		BIT10|BIT11 		/*  CPUD6   */
#define	GPIOE_CPUD_3		BIT8|BIT9 			/*  CPUD6   */
#define	GPIOE_CPUD_4		BIT6|BIT7 			/*  CPUD6   */
#define	GPIOE_CPUD_5		BIT4|BIT5 			/*  CPUD6   */
#define	GPIOE_CPUD_6		BIT2|BIT3 			/*  CPUD6   */
#define	GPIOE_CPUD_7		BIT0|BIT1 			/*  CPUD6   */
#define	GPIOE_CPUD_8		BIT30|BIT31 		/*  CPUD7   */
#define	GPIOE_CPUD_9		BIT28|BIT29 		/*  CPUD7   */
#define	GPIOE_CPUD_10		BIT26|BIT27 		/*  CPUD7   */
#define	GPIOE_CPUD_11		BIT24|BIT25 		/*  CPUD7   */
#define	GPIOE_CPUD_12		BIT26|BIT27 		/*  CPUD8   */
#define	GPIOE_CPUD_13		BIT24|BIT25 		/*  CPUD8   */
#define	GPIOE_CPUD_14		BIT22|BIT23 		/*  CPUD8   */
#define	GPIOE_CPUD_15		BIT20|BIT21 		/*  CPUD8   */
#define	GPIOE_CPUD_16		BIT18|BIT19 		/*  CPUD8   */
#define	GPIOE_CPUD_17		BIT16|BIT17 		/*  CPUD8   */
#define	GPIOE_CPUD_18		BIT14|BIT15 		/*  CPUD8   */
#define	GPIOE_CPUD_19		BIT12|BIT13 		/*  CPUD8   */
#define	GPIOE_CPUD_20		BIT2|BIT3 			/*  CPUD7   */
#define	GPIOE_CPUD_21		BIT30|BIT31 		/*  CPUD8   */
#define	GPIOE_CPUD_22		BIT28|BIT29 		/*  CPUD8   */
#define	GPIOE_CPUD_23		BIT0|BIT1			/*  CPUD7   */ 
#define	GPIOE_CPUD_24		BIT30|BIT31			/*  CPUD6   */ 
#define	GPIOE_CPUD_25		BIT28|BIT29 		/*  CPUD6   */
#define	GPIOE_CPUD_26		BIT26|BIT27 		/*  CPUD6   */
#define	GPIOE_CPUD_27		BIT24|BIT25 		/*  CPUD6   */
#define	GPIOE_CPUD_28		BIT22|BIT23 		/*  CPUD6   */
#define	GPIOE_CPUD_29		BIT20|BIT21 		/*  CPUD6   */
#define	GPIOE_CPUD_30		BIT18|BIT19 		/*  CPUD6   */
#define	GPIOE_CPUD_31		BIT16|BIT17 		/*  CPUD6   */

/*  PORT_F  */
#define	GPIOF_CPUD_28		BIT22|BIT23			/*  CPUD4   */
 
/*----------Pull up/down set as disable (control bit set as 00)----------*/

/*  PORT_A  */
#define	GPIOA_CPUD_0_DISABLE    	~(GPIOA_CPUD_0) 			/*  CPUD7   */
#define	GPIOA_CPUD_1_DISABLE		~(GPIOA_CPUD_1)				/*  CPUD7   */
#define	GPIOA_CPUD_2_DISABLE		~(GPIOA_CPUD_2)				/*  CPUD3   */
#define	GPIOA_CPUD_3_DISABLE		~(GPIOA_CPUD_3)				/*  CPUD4   */
#define	GPIOA_CPUD_4_DISABLE		~(GPIOA_CPUD_4)				/*  CPUD4   */
#define	GPIOA_CPUD_5_DISABLE		~(GPIOA_CPUD_5)				/*  CPUD4   */
#define	GPIOA_CPUD_6_DISABLE		~(GPIOA_CPUD_6)				/*  CPUD5   */
#define	GPIOA_CPUD_7_DISABLE		~(GPIOA_CPUD_7)				/*  CPUD5   */
#define	GPIOA_CPUD_8_DISABLE		~(GPIOA_CPUD_8)				/*  CPUD5   */
#define	GPIOA_CPUD_9_DISABLE		~(GPIOA_CPUD_9)				/*  CPUD5   */
#define	GPIOA_CPUD_10_DISABLE		~(GPIOA_CPUD_10)			/*  CPUD5   */
#define	GPIOA_CPUD_11_DISABLE		~(GPIOA_CPUD_11)			/*  CPUD5   */
#define	GPIOA_CPUD_12_DISABLE		~(GPIOA_CPUD_12)			/*  CPUD5   */

/*  PORT_B  */
#define	GPIOB_CPUD_0_DISABLE		~(GPIOB_CPUD_0)			/*  CPUD4   */
#define	GPIOB_CPUD_1_DISABLE		~(GPIOB_CPUD_1)			/*  CPUD4   */
#define	GPIOB_CPUD_2_DISABLE		~(GPIOB_CPUD_2)			/*  CPUD4   */
#define	GPIOB_CPUD_3_DISABLE		~(GPIOB_CPUD_3)			/*  CPUD4   */
#define	GPIOB_CPUD_4_DISABLE		~(GPIOB_CPUD_4)			/*  CPUD4   */
#define	GPIOB_CPUD_5_DISABLE		~(GPIOB_CPUD_5)			/*  CPUD4   */
#define	GPIOB_CPUD_6_DISABLE		~(GPIOB_CPUD_6)			/*  CPUD4   */
#define	GPIOB_CPUD_7_DISABLE		~(GPIOB_CPUD_7)			/*  CPUD4   */
#define	GPIOB_CPUD_8_DISABLE		~(GPIOB_CPUD_8)			/*  CPUD4   */
#define	GPIOB_CPUD_9_DISABLE		~(GPIOB_CPUD_9)			/*  CPUD4   */
#define	GPIOB_CPUD_10_DISABLE		~(GPIOB_CPUD_10)		/*  CPUD4   */
#define	GPIOB_CPUD_11_DISABLE		~(GPIOB_CPUD_11)		/*  CPUD5   */
#define	GPIOB_CPUD_12_DISABLE		~(GPIOB_CPUD_12)		/*  CPUD5   */
#define	GPIOB_CPUD_13_DISABLE		~(GPIOB_CPUD_13)		/*  CPUD5   */
#define	GPIOB_CPUD_14_DISABLE		~(GPIOB_CPUD_14)		/*  CPUD5   */
#define	GPIOB_CPUD_15_DISABLE		~(GPIOB_CPUD_15)		/*  CPUD5   */

/*  PORT_C  */
#define	GPIOC_CPUD_0_DISABLE		~(GPIOC_CPUD_0)		    /*  CPUD1   */
#define	GPIOC_CPUD_1_DISABLE		~(GPIOC_CPUD_1)			/*  CPUD1   */
#define	GPIOC_CPUD_2_DISABLE		~(GPIOC_CPUD_2)			/*  CPUD1   */
#define	GPIOC_CPUD_3_DISABLE		~(GPIOC_CPUD_3)			/*  CPUD1   */
#define	GPIOC_CPUD_4_DISABLE		~(GPIOC_CPUD_4)			/*  CPUD1   */
#define	GPIOC_CPUD_5_DISABLE		~(GPIOC_CPUD_5)			/*  CPUD1   */
#define	GPIOC_CPUD_6_DISABLE		~(GPIOC_CPUD_6)			/*  CPUD1   */
#define	GPIOC_CPUD_7_DISABLE		~(GPIOC_CPUD_7)			/*  CPUD1   */
#define	GPIOC_CPUD_8_DISABLE		~(GPIOC_CPUD_8)			/*  CPUD0   */
#define	GPIOC_CPUD_9_DISABLE		~(GPIOC_CPUD_9)			/*  CPUD0   */
#define	GPIOC_CPUD_10_DISABLE		~(GPIOC_CPUD_10)		/*  CPUD0   */
#define	GPIOC_CPUD_11_DISABLE		~(GPIOC_CPUD_11)		/*  CPUD0   */
#define	GPIOC_CPUD_12_DISABLE		~(GPIOC_CPUD_12)		/*  CPUD0   */
#define	GPIOC_CPUD_13_DISABLE		~(GPIOC_CPUD_13)		/*  CPUD0   */
#define	GPIOC_CPUD_14_DISABLE		~(GPIOC_CPUD_14)		/*  CPUD0   */
#define	GPIOC_CPUD_15_DISABLE		~(GPIOC_CPUD_15)		/*  CPUD0   */
#define	GPIOC_CPUD_16_DISABLE		~(GPIOC_CPUD_16)		/*  CPUD0   */
#define	GPIOC_CPUD_17_DISABLE		~(GPIOC_CPUD_17)		/*  CPUD0   */
#define	GPIOC_CPUD_18_DISABLE		~(GPIOC_CPUD_18)		/*  CPUD0   */
#define	GPIOC_CPUD_19_DISABLE		~(GPIOC_CPUD_19)		/*  CPUD0   */
#define	GPIOC_CPUD_20_DISABLE		~(GPIOC_CPUD_20)		/*  CPUD0   */
#define	GPIOC_CPUD_21_DISABLE		~(GPIOC_CPUD_21)		/*  CPUD0   */
#define	GPIOC_CPUD_22_DISABLE		~(GPIOC_CPUD_22)		/*  CPUD0   */
#define	GPIOC_CPUD_23_DISABLE		~(GPIOC_CPUD_23)		/*  CPUD0   */
#define	GPIOC_CPUD_24_DISABLE		~(GPIOC_CPUD_24)		/*  CPUD4   */
#define	GPIOC_CPUD_25_DISABLE		~(GPIOC_CPUD_25)		/*  CPUD1   */
#define	GPIOC_CPUD_26_DISABLE		~(GPIOC_CPUD_26)		/*  CPUD1   */
#define	GPIOC_CPUD_27_DISABLE		~(GPIOC_CPUD_27)		/*  CPUD1   */
#define	GPIOC_CPUD_28_DISABLE	  	~(GPIOC_CPUD_28) 		/*  CPUD1   */
#define	GPIOC_CPUD_29_DISABLE	  	~(GPIOC_CPUD_29) 		/*  CPUD1   */
#define	GPIOC_CPUD_30_DISABLE		~(GPIOC_CPUD_30)		/*  CPUD1   */
#define	GPIOC_CPUD_31_DISABLE	  	~(GPIOC_CPUD_31) 		/*  CPUD1   */

/*  PORT_D  */
#define	GPIOD_CPUD_0_DISABLE		~(GPIOD_CPUD_0) 		/*  CPUD7   */
#define	GPIOD_CPUD_1_DISABLE	 	~(GPIOD_CPUD_1)			/*  CPUD7   */
#define	GPIOD_CPUD_2_DISABLE		~(GPIOD_CPUD_2)			/*  CPUD7   */
#define	GPIOD_CPUD_3_DISABLE		~(GPIOD_CPUD_3)			/*  CPUD7   */
#define	GPIOD_CPUD_4_DISABLE		~(GPIOD_CPUD_4)			/*  CPUD7   */
#define	GPIOD_CPUD_5_DISABLE		~(GPIOD_CPUD_5)			/*  CPUD3   */
#define	GPIOD_CPUD_6_DISABLE		~(GPIOD_CPUD_6)			/*  CPUD3   */
#define	GPIOD_CPUD_7_DISABLE		~(GPIOD_CPUD_7)			/*  CPUD3   */
#define	GPIOD_CPUD_8_DISABLE		~(GPIOD_CPUD_8)			/*  CPUD3   */
#define	GPIOD_CPUD_9_DISABLE		~(GPIOD_CPUD_9)			/*  CPUD5   */
#define	GPIOD_CPUD_10_DISABLE		~(GPIOD_CPUD_10)		/*  CPUD5   */
#define	GPIOD_CPUD_11_DISABLE		~(GPIOD_CPUD_11)		/*  CPUD5   */
#define	GPIOD_CPUD_12_DISABLE	  	~(GPIOD_CPUD_12)		/*  CPUD5   */

/*  PORT_E  */
#define	GPIOE_CPUD_0_DISABLE		~(GPIOE_CPUD_0)			/*  CPUD6   */
#define	GPIOE_CPUD_1_DISABLE		~(GPIOE_CPUD_1) 		/*  CPUD6   */
#define	GPIOE_CPUD_2_DISABLE		~(GPIOE_CPUD_2) 		/*  CPUD6   */
#define	GPIOE_CPUD_3_DISABLE		~(GPIOE_CPUD_3) 		/*  CPUD6   */
#define	GPIOE_CPUD_4_DISABLE		~(GPIOE_CPUD_4) 		/*  CPUD6   */
#define	GPIOE_CPUD_5_DISABLE		~(GPIOE_CPUD_5)			/*  CPUD6   */
#define	GPIOE_CPUD_6_DISABLE		~(GPIOE_CPUD_6)			/*  CPUD6   */
#define	GPIOE_CPUD_7_DISABLE		~(GPIOE_CPUD_7)			/*  CPUD6   */
#define	GPIOE_CPUD_8_DISABLE		~(GPIOE_CPUD_8) 		/*  CPUD7   */
#define	GPIOE_CPUD_9_DISABLE		~(GPIOE_CPUD_9)			/*  CPUD7   */
#define	GPIOE_CPUD_10_DISABLE		~(GPIOE_CPUD_10)		/*  CPUD7   */
#define	GPIOE_CPUD_11_DISABLE		~(GPIOE_CPUD_11) 		/*  CPUD7   */
#define	GPIOE_CPUD_12_DISABLE		~(GPIOE_CPUD_12) 		/*  CPUD8   */
#define	GPIOE_CPUD_13_DISABLE		~(GPIOE_CPUD_13)		/*  CPUD8   */
#define	GPIOE_CPUD_14_DISABLE		~(GPIOE_CPUD_14) 		/*  CPUD8   */
#define	GPIOE_CPUD_15_DISABLE		~(GPIOE_CPUD_15)		/*  CPUD8   */
#define	GPIOE_CPUD_16_DISABLE		~(GPIOE_CPUD_16) 		/*  CPUD8   */
#define	GPIOE_CPUD_17_DISABLE		~(GPIOE_CPUD_17)		/*  CPUD8   */
#define	GPIOE_CPUD_18_DISABLE		~(GPIOE_CPUD_18)		/*  CPUD8   */
#define	GPIOE_CPUD_19_DISABLE		~(GPIOE_CPUD_19) 		/*  CPUD8   */
#define	GPIOE_CPUD_20_DISABLE		~(GPIOE_CPUD_20)		/*  CPUD7   */
#define	GPIOE_CPUD_21_DISABLE		~(GPIOE_CPUD_21)		/*  CPUD8   */
#define	GPIOE_CPUD_22_DISABLE		~(GPIOE_CPUD_22) 		/*  CPUD8   */
#define	GPIOE_CPUD_23_DISABLE		~(GPIOE_CPUD_23)		/*  CPUD7   */ 
#define	GPIOE_CPUD_24_DISABLE		~(GPIOE_CPUD_24)		/*  CPUD6   */ 
#define	GPIOE_CPUD_25_DISABLE		~(GPIOE_CPUD_25)		/*  CPUD6   */
#define	GPIOE_CPUD_26_DISABLE		~(GPIOE_CPUD_26) 		/*  CPUD6   */
#define	GPIOE_CPUD_27_DISABLE		~(GPIOE_CPUD_27) 		/*  CPUD6   */
#define	GPIOE_CPUD_28_DISABLE		~(GPIOE_CPUD_28)		/*  CPUD6   */
#define	GPIOE_CPUD_29_DISABLE		~(GPIOE_CPUD_29) 		/*  CPUD6   */
#define	GPIOE_CPUD_30_DISABLE	  	~(GPIOE_CPUD_30) 		/*  CPUD6   */
#define	GPIOE_CPUD_31_DISABLE		~(GPIOE_CPUD_31)		/*  CPUD6   */

/*  PORT_F  */
#define	GPIOF_CPUD_28_DISABLE		~(GPIOF_CPUD_28)		/*  CPUD4   */

/*------------------function port config--------------------------------*/
/***----------------------------------------------------------------------
---��Ϊ���еĹ��ܶ˿�����Ϊ������GPIO�ڸ��õģ����½�ֱ���ڶ�ӦGPIO�ڵ�����
---λ�ϣ��������óɶ�Ӧ�Ĺ��ܶ˿ڡ�------------------------------------***/

/*------------------------------  PORTCFG0  ---------------------------*/

/*   LCD CPU   */

#define LCD_0to7_LXD	         	BIT0     	 /*����ֵΪ1*/
#define LCD_8to15_LXD	     	 	BIT4
#define LCD_16to17_LXD       	 	BIT8
/*   LCD RGB   */

/*  SD interfase Port 0  */
#define SD0_D3toD2 		 	 	 (BIT13|BIT12)/*����ֵΪ3*/  
#define SD0_D1 		 		 	 (BIT17|BIT16)
#define SD0_D0	         	 	 (BIT21|BIT20)
#define SD0_CLK	         	 	 (BIT25|BIT24)
#define SD0_CMD	   		 	 	 (BIT29|BIT28)

/*  Memory Stick Interface Port 0  */
#define MS0_D3toD2		 		 BIT14      /*����ֵΪ4*/
#define MS0_D1		 		 	 BIT18
#define MS0_D0					 BIT22
#define MS0_CLK_ENABLE	         BIT26
#define MS0_BUS_ENABLE	   		 BIT30

/*  GPSB Port 10  */
#define GPSB10_SDO_ENABLE	 	 (BIT18|BIT16)    /*����ֵΪ5*/
#define GPSB10_SDI_ENABLE		 (BIT22|BIT20)
#define GPSB10_SCLK_ENABLE	     (BIT26|BIT24)
#define GPSB10_SFRM_ENABLE   	 (BIT30|BIT28)

/*------------------------------  PORTCFG1  ---------------------------*/
/*  LCD RGB Interface  */


/*  LCD CPU Interface  */
#define LCDCPU_LWEN_ENABLE              BIT28   /*����ֵΪ1*/
#define LCDCPU_LOEN_ENABLE              BIT24
#define LCDCPU_LXA_ENABLE		   	   	BIT20
#define LCDCPU_LCSN0_ENABLE		    	BIT16
#define LCDCPU_LCSN1_ENABLE  	    	BIT12
/*  NAND  */
#define NFC_NANDRDY3_ENABLE 			(BIT1|BIT0)		/*����ֵΪ3*/

/*  GPSB Port 9  */
#define GPSB9_SFRM_ENABLE				 BIT0			/*����ֵΪ2*/

/*  LCD PIP  */
#define LDE_IN_ENABLE 					(BIT2|BIT0)		/*����ֵΪ5*/
/*------------------------------  PORTCFG2  ---------------------------*/
/*  NAND  */
#define NFC_NANDXD_8to11_ENABLE			    BIT5  		/*����ֵΪ2*/
#define NFC_NANDXD_12to15_ENABLE		    BIT9
#define NFC_NANDCSN2_ENABLE	      		(BIT20|BIT22)	/*����ֵΪ3*/
#define NFC_NANDCSN3_ENABLE				(BIT24|BIT26)
#define NFC_NANDRDY2_ENABLE				(BIT28|BIT30)

/*   LCD PIP   */  
#define LCD_LPDIN_7						(BIT0|BIT2)      /*����ֵΪ5*/
#define LCD_LPDIN_8to11			    	(BIT4|BIT6)
#define LCD_LPDIN_12to15	    		(BIT8|BIT10)
#define LCD_LPDIN_16to17	    		(BIT12|BIT14)
#define LCD_LPDIN_18to19_21to23			(BIT16|BIT18)  		
#define LCDPIP_LHS_IN_ENABLE    		(BIT20|BIT22)			
#define LCDPIP_LVS_IN_ENABLE 		    (BIT24|BIT26)
#define LCDPIP_LCLK_IN_ENABLE 			(BIT28|BIT30)
/*     IDE    */
#define IDE_HDDXD_7						BIT2             /*����ֵΪ4*/
#define IDE_HDDXD_8to11			   	 	BIT6
#define IDE_HDDXD_12to15	    		BIT10
#define IDE_HDDCSN_01_ENABLE	        BIT14
#define IDE_HDD_RW_AD_ENABLE			BIT18  		
#define IDE_HDDAK_ENABLE    			BIT22			
#define IDE_HDDRDY_ENABLE 				BIT30

/*  SD Interface Port1  */
//#define SD1_D3toD2_D0_CMD_CLK_ENABLE	(PORTCFG2 & (~(GPIOF_18to19_21to23))) + BIT17+BIT16  
//	#define SD1_SD1CTRL_ENABLE							SD1_D3toD2_D0_CMD_CLK_ENABLE
#define SD1_SD1CTRL_ENABLE              (BIT17|BIT16)		/*����ֵΪ3*/
/*  SD Interface Port2  */
#define SD2_D0toD3		    			(BIT4|BIT5)	/*����ֵΪ3*/
#define SD2_D4toD7	    				(BIT8|BIT9)
#define SD2_CMD_CLK_ENABLE	    		(BIT12|BIT13)
/*  SD Interface Port3  */	
#define SD3_D7											BIT1  			/*����ֵΪ2*/
#define SD3_CMD_CLK_ENABLE	    		BIT13
/*  EHI  */

/*  GPSB Port 9  */
#define GPSB9_SDO_ENABLE    			BIT21			/*����ֵΪ2*/		
#define GPSB9_SDI_ENABLE 				BIT25
#define GPSB9_SCLK_ENABLE 				BIT29
/*  GPSB Port 8  */

#define GPSB8_SFRM_ENABLE				(BIT0|BIT1)    	/*����ֵΪ3*/
/*  Memory Stick Port 1  */
//#define MS1_D3toD2_D0_BUS_CLK_ENABLE	(PORTCFG2 & (~(GPIOF_18to19_21to23))) + BIT17   /*����ֵΪ2*/
//	#define MS1_MS1CTRL_ENABLE				 MS1_D3toD2_D0_BUS_CLK_ENABLE
#define MS1_MS1CTRL_ENABLE	      		BIT17 

/*------------------------------  PORTCFG3  ---------------------------*/
/*  GPSB Port 6  */
//#define GPSB6_SFRM_ENABLE 				(PORTCFG3 & (~(GPIOD_5))) 					 /*����ֵΪ0*/
/*  GPSB Port 7  */
#define GPSB7_SDO_ENABLE  				(BIT4|BIT5)        /*����ֵΪ3*/
#define GPSB7_SDI_ENABLE 				(BIT8|BIT9)
#define GPSB7_SCLK_ENABLE 				(BIT12|BIT13)
#define GPSB7_SFRM_ENABLE 				(BIT16|BIT17)
/*  GPSB Port 8  */
#define GPSB8_SDO_ENABLE				(BIT20|BIT21)		 /*����ֵΪ3*/
#define GPSB8_SDI_ENABLE 				(BIT24|BIT25)
#define GPSB8_SCLK_ENABLE 				(BIT28|BIT29)
/*  EHI  */

/*  SD Interface Port3  */
#define SD3_D0  				 BIT5        /*����ֵΪ2*/
#define SD3_D1 					 BIT9
#define SD3_D2 					 BIT13
#define SD3_D3 					 BIT17
#define SD3_D4 					 BIT21
#define SD3_D5 					 BIT25
#define SD3_D6 					 BIT29
/*     IDE    */
#define IDE_HDDXD_0  			BIT6        /*����ֵΪ4*/
#define IDE_HDDXD_1				BIT10
#define IDE_HDDXD_2 			BIT14
#define IDE_HDDXD_3 			BIT18
#define IDE_HDDXD_4				BIT22
#define IDE_HDDXD_5				BIT26
#define IDE_HDDXD_6				BIT30
/*   LCD PIP   */ 
#define LCD_LPDIN_0  			(BIT4|BIT6)   /*����ֵΪ5*/
#define LCD_LPDIN_1 			(BIT8|BIT10) 
#define LCD_LPDIN_2 			(BIT12|BIT14)
#define LCD_LPDIN_3 			(BIT16|BIT18)
#define LCD_LPDIN_4 			(BIT20|BIT22)
#define LCD_LPDIN_5 			(BIT24|BIT26)
#define LCD_LPDIN_6 			(BIT28|BIT30)
/*------------------------------  PORTCFG4  ---------------------------*/
/*  GPSB Port 6  */

/*  CKC  */
#define CLK_OUT0		 		 	BIT16	    /*����ֵΪ1*/
#define CLK_OUT1		 		 	BIT12
#define CLK_WDTRSTO_ENABLE		 	BIT8
/*  TIMER  */
#define TIM_TCO3		 			BIT9		/*����ֵΪ2*/
#define TIM_TCO2		 			BIT5
/*   EMC   */
//#define EMC_CSN_CS0			 	(PORTCFG4 & (~(GPIOC_24)))              /*����ֵΪ0*/

/*------------------------------  PORTCFG5  ---------------------------*/
/*   EMC   */
//#define EMC_CSN_NOR				(PORTCFG5 & (~(GPIOF_28)))				/*����ֵΪ0*/	
/*   NAND   */
#define NFC_NANDXD_0_ENABLE				BIT24         /*����ֵΪ1*/
#define NFC_NANDXD_1_ENABLE		    	BIT20
#define NFC_NANDXD_2_ENABLE				BIT16
#define NFC_NANDXD_3_ENABLE		    	BIT12
#define NFC_NANDXD_4_ENABLE		    	BIT8
#define NFC_NANDXD_5_ENABLE				BIT4
#define NFC_NANDXD_6_ENABLE		    	BIT0 

/*  SD Interface Port5  */
#define SD5_D0					BIT25         /*����ֵΪ2*/
#define SD5_D1	   	 			BIT21
#define SD5_D2					BIT17
#define SD5_D3	   				BIT13
#define SD5_D4	    			BIT9
#define SD5_D5					BIT5
#define SD5_D6   				BIT1
/*  GPSB Port 3  */
#define GPSB3_SFRM_ENABLE		(BIT24|BIT25)   /*����ֵΪ3*/
#define GPSB3_SCLK_ENABLE 	    (BIT20|BIT21)
#define GPSB3_SDI_ENABLE		(BIT16|BIT17)
#define GPSB3_SDO_ENABLE	    (BIT12|BIT13)
/*  GPSB Port 2  */
#define GPSB2_SFRM_ENABLE	    (BIT8|BIT9)		/*����ֵΪ3*/
#define GPSB2_SCLK_ENABLE		(BIT4|BIT5)
#define GPSB2_SDI_ENABLE	    (BIT0|BIT1)
/*  SFRAMIF   */
#define SRAMIF_XD0				(BIT24|BIT26)   /*����ֵΪ5*/
#define SRAMIF_XD1	    		(BIT20|BIT22)
#define SRAMIF_XD2				(BIT16|BIT18)
#define SRAMIF_XD3	    		(BIT12|BIT14)
#define SRAMIF_XD4	   	 		(BIT8|BIT10)
#define SRAMIF_XD5				(BIT4|BIT6)
#define SRAMIF_XD6	    		(BIT0|BIT2)

/*------------------------------  PORTCFG6  ---------------------------*/
/*    NAND   */
#define NFC_NANDXD_7_ENABLE	    	BIT28		/*����ֵΪ1*/
#define NFC_NANDOEN_ENABLE			BIT24
#define NFC_NANDWEN_ENABLE	    	BIT20
#define NFC_NANDCSN0_ENABLE	    	BIT16
#define NFC_NANDCSN1_ENABLE			BIT12
#define NFC_NANDCLE_ENABLE	   	 	BIT8
#define NFC_NANDALE_ENABLE			BIT4
#define NFC_NANDRDY0_ENABLE	    	BIT0

/*  SD Interface Port5  */
#define SD5_D7	    				BIT29		/*����ֵΪ2*/
#define SD5_CMD_ENABLE				BIT25
#define SD5_CLK_ENABLE	    		BIT21
/*  SD Interface Port6  */
#define SD6_D0	    				BIT17
#define SD6_D1						BIT13
#define SD6_D2	   					BIT9
#define SD6_D3						BIT5
#define SD6_CMD_ENABLE	   			BIT1
/*   GPSB port 2   */
#define GPSB2_SDO_ENABLE	    	(BIT28|BIT29)	/*����ֵΪ3*/
/*   GPSB port 1   */

#define GPSB1_SFRM_ENABLE	    	BIT18		/*����ֵΪ4*/
#define GPSB1_SCLK_ENABLE			BIT14
#define GPSB1_SDI_ENABLE	    	BIT10
#define GPSB1_SDO_ENABLE			BIT6
/*   Memory Stick Interface port 3   */
#define MS3_D0	   					(BIT16|BIT17)	/*����ֵΪ3*/
#define MS3_D1						(BIT12|BIT13)
#define MS3_D2    					(BIT8|BIT9)
#define MS3_D3						(BIT4|BIT5)
#define MS3_BUS_ENABLE				(BIT0|BIT1)
/*  SFRAMIF   */
#define SRAMIF_XD7    				(BIT30|BIT28)	/*����ֵΪ5*/
#define SRAMIF_OEN					(BIT26|BIT24)
#define SRAMIF_WEN	    			(BIT22|BIT20)
#define SRAMIF_CSN0	    			(BIT18|BIT16)
#define SRAMIF_CSN1					(BIT14|BIT12)
#define SRAMIF_XA0	    			(BIT10|BIT8)
#define SRAMIF_XA1					(BIT6|BIT4)

/*------------------------------  PORTCFG7  ---------------------------*/
/*    NAND   */
#define NFC_NANDRDY1_ENABLE	    		BIT28		/*����ֵΪ1*/
/*   SD Interface port 6   */
#define SD6_CLK_ENABLE	    			BIT29		/*����ֵΪ2*/
/*   Memory Stick Interface port 3  */

#define MS3_CLK_ENABLE	    			(BIT28|BIT29)	/*����ֵΪ3*/
/*   GPSB port 5   */

/*   SPDIFTX   */

#define SPDIFTX_ENABLE	    		BIT8		/*����ֵΪ1*/

/*    I2C1    */
#define I2C_SCL1_ENABLE	 		 	BIT0
/*#define I2C_SCL1_ENABLE	 		 	(PORTCFG7 & (~(GPIOA_8)))+BIT0	*/	/*����ֵΪ1*/
/*   Timer   */
#define TIM_TCO1	    	 		BIT9		/*����ֵΪ2*/
#define TIM_TCO0			 		BIT5		/*����ֵΪ2*/
/*   Timer External Clock Input */
//#define TIM_EXTCLKI			 		(PORTCFG7 & (~(GPIOA_7)))+BIT4
#define TIM_EXTCLKI_ENABLE			 		BIT4		/*����ֵΪ1*/


/*------------------------------  PORTCFG8  ---------------------------*/
/*    I2C1    */
/*#define I2C_SDA1_ENABLE		 	 	(PORTCFG8 & (~(GPIOA_9)))+BIT28	*/
#define I2C_SDA1_ENABLE		 	 	BIT28		/*����ֵΪ1*/

/*   CD interface   */
#define CD_CBCLK_ENABLE		 		(PORTCFG8 & (~(GPIOA_10)))|BIT24	/*����ֵΪ1*/
#define CD_CLRCK_ENABLE		 		(PORTCFG8 & (~(GPIOA_11)))|BIT20
#define CD_CDATA_ENABLE		 		(PORTCFG8 & (~(GPIOA_12)))|BIT12
/*    ADC    */

/*   SD Interface port 7   */
#define SD7_CMD_ENABLE				 BIT5	/*����ֵΪ2*/
#define SD7_CLK_ENABLE				 BIT1
/*   Memory Stick Interface port 4   */
#define MS4_BUS_ENABLE				(BIT4|BIT5)	/*����ֵΪ3*/
#define MS4_CLK_ENABLE				(BIT0|BIT1)
/*------------------------------  PORTCFG9  ---------------------------*/
/*    ADC    */

/*   SD Interface port 7   */
#define SD7_D0						BIT29		/*����ֵΪ2*/
#define SD7_D1						BIT25
#define SD7_D2						BIT21
#define SD7_D3						BIT17
/*   Memory Stick Interface port 4   */
#define MS4_D0						(BIT28|BIT2)	/*����ֵΪ3*/
#define MS4_D1						(BIT24|BIT25)
#define MS4_D2						(BIT20|BIT21)
#define MS4_D3						(BIT16|BIT17)
/*   GPSB port 0    */
#define GPSB0_SFRM_ENABLE 			BIT30		/*����ֵΪ4*/
#define GPSB0_SCLK_ENABLE			BIT26
#define GPSB0_SDI_ENABLE			BIT22
#define GPSB0_SDO_ENABLE			BIT18
/*    UART port 0   */

/*    UART port 4   */
#define UART4_UTXD_ENABLE			BIT5		/*����ֵΪ2*/
#define UART4_URXD_ENABLE			BIT1


/*------------------------------  PORTCFG10  ---------------------------*/
/*   UART port 1   */

/*   UART port 2   */

/*   UART port 3   */

/*   UART port 5   */
#define UART5_UTXD_ENABLE			(PORTCFG10 & (~(GPIOE_6))) | BIT21		/*����ֵΪ2*/
#define UART5_URXD_ENABLE			(PORTCFG10 & (~(GPIOE_7))) | BIT17
/*     GPSB port 4   */
#define GPSB4_SFRM_ENABLE 			BIT13		/*����ֵΪ2*/
#define GPSB4_SCLK_ENABLE			BIT9
#define GPSB4_SDI_ENABLE			BIT5
#define GPSB4_SDO_ENABLE			BIT1
/*   SD Interface port 4   */
#define SD4_CLK_ENABLE				 (BIT20|BIT21)/*����ֵΪ3*/
#define SD4_CMD_ENABLE				 (BIT16|BIT17)
#define SD4_D0						 (BIT12|BIT13)
#define SD4_D1						 (BIT8|BIT9)
#define SD4_D2						 (BIT4|BIT5)
#define SD4_D3						 (BIT0|BIT1)
/*     Memory Stick Interface port 2    */

#define MS2_CLK_ENABLE				 BIT22		/*����ֵΪ4*/
#define MS2_BUS_ENABLE				 BIT18
#define MS2_D0						 BIT14
#define MS2_D1						 BIT10
#define MS2_D2						 BIT6
#define MS2_D3						 BIT2


/*------------------------------  PORTCFG11  ---------------------------*/
/*   Boot Mode Selector   */
//#define BM_SEL_ENABLE				(PORTCFG11 & (~(GPIOF_29to31))) 	/*����ֵΪ0*/
/*    DAI    */

/*    I2C0    */
#define I2C_SCL0_SDA0_ENABLE     	BIT4
/*    CIF   */

/*#define CIF_CCKI_ENABLE				(PORTCFG11 & (~(GPIOE_20)))  */   	/*����ֵΪ0*/

/*------------------------------  PORTCFG12  ---------------------------*/
/*    CIF   */

/*------------------------------  PORTCFG13  ---------------------------*/

/*    CIF   */

/*  EHI  */      /* HPCSN field is write-only register and always read as 0 */

//#define  EHI_HPCSN_ENABLE   		(PORTCFG13 & (~(GPIOF_20)))  		/*����ֵΪ0*/
/*     Memory Stick Interface port 1    */
#define MS1_D1	   					BIT1  			/*����ֵΪ2*/
/*   SD Interface port 1   */
#define SD1_D1   					(BIT1|BIT0)  	/*����ֵΪ3*/
/*   IDE   */
#define IDE_HDDXA0_ENABLE   		BIT2  			/*����ֵΪ4*/
/*   LCD PIP   */
#define LCD_LPDIN_20   				BIT2|BIT0  		/*����ֵΪ5*/
/*-----------------------Interrupt Related Registers--------------------*/

#define	EINTSEL0			TCC7901_GPIO_EXTERNAL_INTERRUPT_SELECT0_REGISTER		/* External Interrupt Select Register 0 */
#define	EINTSEL1			TCC7901_GPIO_EXTERNAL_INTERRUPT_SELECT1_REGISTER		/* External Interrupt Select Register 1 */
#define	IRQSEL				TCC7901_GPIO_INTERRUPT_SELECT_REGISTER					/* Interrupt Select Register */

/*-----------------------32 interrupt sources-----------------------------*/
#define INTERRUPT_URXD3  	0x0
#define INTERRUPT_UTXD2  	0x1
#define INTERRUPT_AIN7  	0x2
#define INTERRUPT_AIN1  	0x3
#define INTERRUPT_AIN0  	0x4
#define INTERRUPT_GPIOA11  	0x5
#define INTERRUPT_GPIOA7  	0x6
#define INTERRUPT_GPIOA6  	0x7
#define INTERRUPT_SDO1  	0x8
#define INTERRUPT_SCMD1  	0x9
#define INTERRUPT_GPIOB15  	0xa
#define INTERRUPT_GPIOB14  	0xb
#define INTERRUPT_GPIOB13  	0xc
#define INTERRUPT_GPIOB4  	0xd
#define INTERRUPT_GPIOB3  	0xe
#define INTERRUPT_GPIOB0  	0xf
#define INTERRUPT_GPIOA5  	0x10
#define INTERRUPT_GPIOA3  	0x11
#define INTERRUPT_GPIOA2  	0x12
#define INTERRUPT_SDO0  	0x13
#define INTERRUPT_SCMD0  	0x14
#define INTERRUPT_HPXD3  	0x15
#define INTERRUPT_HPXD11  	0x16
#define INTERRUPT_HPRDN  	0x17
#define INTERRUPT_HPCSN_L  	0x18
#define INTERRUPT_GPIOF26  	0x19
#define INTERRUPT_GPIOF27  	0x1a
#define INTERRUPT_GPIOC30  	0x1b
#define INTERRUPT_LPD18  	0x1c
#define INTERRUPT_LPD23  	0x1d
#define INTERRUPT_PMKUP  	0x1e
#define INTERRUPT_USB_VBOFF  	0x1f    /* 0,even */

#define INTERRUPT_USB_VBON  	0x1f    /* odd */

/*---------------------8 external interruptsSelect----------------------*/
#define EINT0SEL    0
#define EINT1SEL	1
#define EINT2SEL	2
#define EINT3SEL	3
#define EINT4SEL	4
#define EINT5SEL	5
#define EINT6SEL	6
#define EINT7SEL	7

#define EINT_NUM	8

/*----------------�ж�ͨ�����жϼĴ����еľ���λ------------------------*/

#define EINTSEL0_0SEL     (BIT0|BIT1|BIT2|BIT3|BIT4)
#define EINTSEL0_1SEL     (BIT8|BIT9|BIT10|BIT11|BIT12)
#define EINTSEL0_2SEL     (BIT16|BIT17|BIT18|BIT19|BIT20)
#define EINTSEL0_3SEL     (BIT24|BIT25|BIT26|BIT27|BIT28)

#define EINTSEL1_4SEL     (BIT0|BIT1|BIT2|BIT3|BIT4)
#define EINTSEL1_5SEL     (BIT8|BIT9|BIT10|BIT11|BIT12)
#define EINTSEL1_6SEL     (BIT16|BIT17|BIT18|BIT19|BIT20)
#define EINTSEL1_7SEL     (BIT24|BIT25|BIT26|BIT27|BIT28)

#define BIT_MASK0					0x0000000f	
#define BIT_MASK1					0x000000f0	
#define BIT_MASK2					0x00000f00	
#define BIT_MASK3					0x0000f000	
#define BIT_MASK4					0x000f0000	
#define BIT_MASK5					0x00f00000	
#define BIT_MASK6					0x0f000000	
#define BIT_MASK7					0xf0000000	

/*************************************************************************
 * TYPEDEFS
 *************************************************************************/


/*	Gpio �Ĵ����Ķ��壬ÿ��gpioռ��5���Ĵ���  */
typedef volatile struct	tgpio_register
{
	U32 DATA_REGISTER;		/*GPIO Data Register */
	U32 DIRECTION;			/*GPIO Direction Control Register */
	U32 SET_REGISTER;		/*GPIO Set Register */
	U32 CLEAR_REGISTER;		/*GPIO Clear Register */
	U32 XOR_REGISTER;		/*GPIO XOR Register */
	U32 RESERVED1;
	U32 RESERVED2;
	U32 RESERVED3;
}tGPIO_REGISTER;

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_Read
* ����:	��ȡָ��GPIO��״̬��ֵ��
* ����:	ָ��GPIO�����루ֻ��һ��GPIO��
* ����:	ָ��GPIO��״̬��ֵ��,1:high;0:low
*       λ����
        for example  �����PORT_Aλ0��״̬ unsigned char hyhwGpio_Read(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_Read( U8 port,U32 mask );

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setLow
* ����:	����ָ��GPIOΪlow
* ����:	ָ��GPIO������
* ����:	none
        for example  �����PORT_Aλ0Ϊ0���ú���hyhwGpio_SetLow(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_setLow(U8 port,U32 mask );

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setHigh
* ����:	����ָ��GPIOΪhigh
* ����:	ָ��GPIO������
* ����:	none
		for example  �����PORT_Aλ0Ϊ1���ú���hyhwGpio_SetHigh(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_setHigh(U8 port,U32 mask );

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setXOR
* ����:	����ָ��GPIO�����ֵ
* ����:	ָ��GPIO������
* ����:	HY_OK		-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
		for example  �����PORT_Aλ0Ϊ���ֵ���ú���hyhwGpio_setXOR(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_setXOR(U8 port,U32 mask );

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setOut
* ����:	����ָ��GPIOΪ���IO
* ����:	ָ��GPIO������  direction control: 1 output and 0 input
* ����:	none
		for example �����PORT_Aλ0ΪOUTPUT���ú��� hyhwGpio_SetOut(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_setOut(U8 port,U32 mask );

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setIn
* ����:	����ָ��GPIOΪ����IO
* ����:	ָ��GPIO������  direction control: 1 output and 0 input
* ����:	none
		for example �����PORT_Aλ0ΪINTPUT���ú��� hyhwGpio_SetOut(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_setIn(U8 port,U32 mask );

/*----------------------------------------------------------------------------
* ����:	hyhwGpio_setAsGpio
* ����:	����ָ��GPIOΪIO�ڹ���
* ����:	ָ��GPIO������
* ����:	none
* NOTE:
		1��The Priority of this register is higher than Function Selection Register.
		2��If a certain bit of Function Selection Register is not set, the corresponding GPIO pin act as
		   Normal GPIO pin independent of This Register.
        λ����
        for example �����PORT_Aλ0ΪIO�ڹ����ú��� hyhwGpio_SetasGpio(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_setAsGpio(U8 port, U32 mask );


/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_pullDisable
* ����:	����ָ��GPIO pull disable 
* ����:	ָ��GPIO������  direction control: 1 output and 0 input
* ����:	none
		for example �����PORT_Aλ0ΪPull Disable���ú��� hyhwGpio_Pull_Disable(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_pullDisable(U8 port,U32 mask);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setPullUp
* ����:	����ָ��GPIO pull up model
* ����:	ָ��GPIO������  direction control: 1 output and 0 input
* ����:	none
		for example �����PORT_Aλ0ΪPull Up ���ú��� hyhwGpio_Set_Pull_Up(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_setPullUp(U8 port,U32 mask);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setPullDown
* ����:	����ָ��GPIO PULL DOWN model 
* ����:	ָ��GPIO������  direction control: 1 output and 0 input
* ����:	none
		for example �����PORT_Aλ0ΪPull_Down ���ú��� hyhwGpio_Set_Pull_Down(PORT_A,BIT0)
*----------------------------------------------------------------------------*/
U32 hyhwGpio_setPullDown(U8 port,U32 mask);


/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setNfc8Function
* ����:	����ָ��GPIOΪָ�����ܿ�:NAND FLASH CONTROLLER
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setNfc8Function(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setNfc16Function
* ����:	����ָ��GPIOΪָ�����ܿ�:NAND FLASH CONTROLLER
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setNfc16Function(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setLcdrgbFunction
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none ������Rgb ��
*----------------------------------------------------------------------------*/
void hyhwGpio_setLcdrgbFunction(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setLcdpipFunction
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none ������pip ��
*----------------------------------------------------------------------------*/
void hyhwGpio_setLcdpipFunction(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setLcdcpuFunction
* ����:	����ָ��GPIOΪָ�����ܿ�:LCD cpu
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setLcdcpuFunction(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setAdcFunction
* ����:	����ָ��GPIOΪָ�����ܿ�:ADC
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setAdcFunction(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setI2c0Function
* ����:	����ָ��GPIOΪָ�����ܿ�:I2C0
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setI2c0Function(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setI2c1Function
* ����:	����ָ��GPIOΪָ�����ܿ�:I2C1
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setI2c1Function(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_Set_CIF_Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none 
*----------------------------------------------------------------------------*/
void hyhwGpio_setCifFunction(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_Set_Uart0_Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none 
*----------------------------------------------------------------------------*/
void hyhwGpio_setUart0Function(int isFlow);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_Set_Uart1_Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none 
*----------------------------------------------------------------------------*/
void hyhwGpio_setUart1Function(int isFlow);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_Set_Uart2_Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none 
*----------------------------------------------------------------------------*/
void hyhwGpio_setUart2Function(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_Set_Uart3_Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none 
*----------------------------------------------------------------------------*/
void hyhwGpio_setUart3Function(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_Set_Uart4_Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none 
*----------------------------------------------------------------------------*/
void hyhwGpio_setUart4Function(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_Set_Uart5_Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none 
*----------------------------------------------------------------------------*/
void hyhwGpio_setUart5Function(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setDaiFunction
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setDaiFunction(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setSpdiftxFunction
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSpdiftxFunction(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setTimerFunction
* ����:	����ָ��GPIOΪָ�����ܿڣ�Output of Timer
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setTimerFunction(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setTimerExtclkiFunction
* ����:	����ָ��GPIOΪָ�����ܿڣ�External Clock Input
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setTimerExtclkiFunction(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setEmcFunction
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setEmcFunction(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setCkcFunction
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setCkcFunction(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setIdeFunction
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setIdeFunction(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setSFRamFunction
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSFRamFunction(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setSD0Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSD0Function(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setSD1Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSD1Function(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setSD2Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSD2Function(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setSD3Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSD3Function(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setSD4Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSD4Function(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setSF5amFunction
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSD5Function(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setSD6Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSD6Function(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setSD7Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setSD7Function(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setEhiFunction
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setEhiFunction(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setGpsb0Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb0Function(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setGpsb1Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb1Function(void);
/*-----------------------1------------------------------------------------------
* ����:	hyhwGpio_setGpsb2Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb2Function(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setGpsb3Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb3Function(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setGpsb4Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb4Function(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setGpsb5Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb5Function(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setGpsb6Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb6Function(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setGpsb7Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb7Function(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setGpsb8Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb8Function(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setGpsb9Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb9Function(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setGpsb10Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setGpsb10Function(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setMmstick0Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setMmstick0Function(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setMmstick1Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setMmstick1Function(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setMmstick2Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setMmstick2Function(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setMmstick3Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setMmstick3Function(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setMmstick4Function
* ����:	����ָ��GPIOΪָ�����ܿ�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setMmstick4Function(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setBmselFunction
* ����:	����ָ��GPIOΪָ�����ܿ� boot mode
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwGpio_setBmselFunction(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_setInterruptEnable
* ����:	����ָ�����ж�ԴΪָ���ж�ͨ�����ж�Դ
* ����: Eintsel---ָ�����ж�ͨ��
		intsources---ָ�����ж�Դ
* ����:	none
*----------------------------------------------------------------------------*
U32 hyhwGpio_setInterruptEnable(U8 Eintsel,U32 intsources); 
*****************************************************************************/

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenUrxd3
* ����:	ʹ���ж�ԴURXD3����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenUrxd3(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenUtxd2
* ����:	ʹ���ж�ԴUtxd2����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenUtxd2(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenAin7
* ����:	ʹ���ж�ԴAin7����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenAin7(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenAin1
* ����:	ʹ���ж�ԴAin1����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenAin1(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenAin0
* ����:	ʹ���ж�ԴAin0����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenAin0(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenGpioa11
* ����:	ʹ���ж�ԴGpioa11����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpioa11(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenGpioa7
* ����:	ʹ���ж�ԴGpioa7����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpioa7(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenGpioa6
* ����:	ʹ���ж�ԴGpioa6����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpioa6(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenSdo1
* ����:	ʹ���ж�ԴSdo1����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenSdo1(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenScmd1
* ����:	ʹ���ж�ԴSCMD1����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenScmd1(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenGpiob15
* ����:	ʹ���ж�ԴGPIOB15����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpiob15(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenGpiob14
* ����:	ʹ���ж�ԴGPIOB14����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpiob14(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenGpiob13
* ����:	ʹ���ж�ԴGPIOB13����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpiob13(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenGpiob4
* ����:	ʹ���ж�ԴGPIOB4����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpiob4(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenGpiob3
* ����:	ʹ���ж�ԴGPIOB3����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpiob3(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenGpiob0
* ����:	ʹ���ж�ԴGPIOB0����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpiob0(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenGpioa5
* ����:	ʹ���ж�ԴGPIOA5����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpioa5(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenGpioa3
* ����:	ʹ���ж�ԴGPIOA3����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpioa3(U8 Eintsel);
/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenGpioa2
* ����:	ʹ���ж�GPIOA2Դ����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpioa2(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenSdo0
* ����:	ʹ���ж�ԴSdo0����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenSdo0(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenScmd0
* ����:	ʹ���ж�ԴScmd0����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenScmd0(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenHpxd3
* ����:	ʹ���ж�ԴHpxd3����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenHpxd3(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenHpxd11
* ����:	ʹ���ж�ԴHpxd11����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenHpxd11(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenHprdn
* ����:	ʹ���ж�ԴHprdn����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenHprdn(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenHpcsn_l
* ����:	ʹ���ж�ԴHPCSN_L����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenHpcsn_l(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenGpiof26
* ����:	ʹ���ж�ԴGPIOF26����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpiof26(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenGpiof27
* ����:	ʹ���ж�ԴGPIOF27����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpiof27(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenGpioc30
* ����:	ʹ���ж�ԴGPIOC30����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenGpioc30(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenLpd18
* ����:	ʹ���ж�ԴLPD18����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenLpd18(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenLpd23
* ����:	ʹ���ж�ԴLPD23����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenLpd23(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenPmwkup
* ����:	ʹ���ж�ԴPMWKUP����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenPmwkup(U8 Eintsel);


/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenUsb_vboff
* ����:	ʹ���ж�ԴUSB_VBOFF����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenUsb_vboff(U8 Eintsel);

/*-----------------------------------------------------------------------------
* ����:	hyhwGpio_IntenUsb_vbon
* ����:	ʹ���ж�ԴUSB_VBON����ָ���ж�ͨ��
* ����: Eintsel---ָ�����ж�ͨ��
		
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwGpio_IntenUsb_vbon(U8 Eintsel);

#endif 

