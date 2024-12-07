/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
uint8_t uart3_receive;

CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;
CAN_TxHeaderTypeDef CAN1_pHeader; //TX tester
CAN_RxHeaderTypeDef CAN1_pHeaderRx; //RX tester
CAN_FilterTypeDef CAN1_sFilterConfig;
CAN_TxHeaderTypeDef CAN2_pHeader; //TX ECU
CAN_RxHeaderTypeDef CAN2_pHeaderRx; //RX ECU
CAN_FilterTypeDef CAN2_sFilterConfig;
uint32_t CAN1_pTxMailbox; // tester mailbox
uint32_t CAN2_pTxMailbox; // ECU mailbox

uint16_t NumBytesReq = 0;
uint8_t  REQ_BUFFER  [4096];
uint8_t  REQ_1BYTE_DATA;
uint16_t tester_ID=0x712;
uint16_t ECU_ID=0x7A2;
uint16_t ECU_IDtmp;
uint8_t check=0;
uint8_t enb2E=0;

//TX tester
uint8_t CAN1_DATA_TX[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// RX tester
uint8_t CAN1_DATA_RX[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// TX ECU
uint8_t CAN2_DATA_TX[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
// RX ECU
uint8_t CAN2_DATA_RX[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

uint8_t CAN2_STORE_SEED[8];


uint16_t Num_Consecutive_Tester;
uint8_t  Flg_Consecutive = 0;


unsigned int TimeStamp;
int fifo;
// maximum characters send out via UART is 30
char bufsend[30]="XXX: D1 D2 D3 D4 D5 D6 D7 D8  ";

uint8_t MessageCounter = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
static void MX_CAN2_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */
void MX_CAN1_Setup();
void MX_CAN2_Setup();
void USART3_SendString(uint8_t *ch);
void PrintCANLog(uint16_t CANID, uint8_t * CAN_Frame);
void delay(uint16_t delay);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void readFromTester(uint8_t* req_tester, uint16_t len, uint8_t* data_tx);
void checkAndHandle_tester();
void checkAndHandle_ECU();


// void checkAndHandle_service22(); //tester
// void checkAndHandle_service27(); //tester
// void checkAndhandle_service2E();  //tester
//
//Service22
 //Start service22
void ReadDataByIdentifierRequestFrame(uint8_t data[]);
void ReadDataByIdentifierResponseFrame(uint8_t data[]);
void HandleSID22(); //ECU xá»­ lÃ­ SID22
void HandleSID62(); //Tester xá»­ lÃ­ SID62

//Service27

void HandleSID27();
void HandleSID67();
uint8_t generate_seed();
uint8_t* cal_key (uint8_t Tx_data[], uint8_t data[]);
void securityAccessServiceSeedRequestFrame (uint8_t data[]);
void securityAccessServiceSeedResponseFrame (uint8_t data[]);
void securityAccessSendKeyRequestFrame(uint8_t Tx_data[], uint8_t data[]);
void securityAccessSendKeyResponseFrame(uint8_t data[]);
void SecurityAccessNegativeResponseMessage(uint8_t data[], uint8_t flag_NRC, uint8_t SID);
uint8_t checkKey (uint8_t data[], uint8_t CAN2_STORE_SEED[] );

//Service 2E

void HandleSID2E();
void HandleSID6E();
void WriteDataByIdentifierResponseFrame(uint8_t data[]);
void CAN1_Send();
//Ham lay byte

uint8_t get_PCI(uint8_t);
uint8_t get_size(uint8_t);
uint8_t get_SID(uint8_t data[]);
uint16_t get_DID(uint8_t data[]);
uint8_t get_sub_function(uint8_t data[]);
uint8_t get_NRC(uint8_t data[]);


//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  MX_CAN1_Setup();
  MX_CAN2_Setup();
  __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  // Example Function to print can message via uart
while (1)
{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  if(NumBytesReq!=0)
    	{
    		delay(100);
    		readFromTester(REQ_BUFFER, NumBytesReq, CAN1_DATA_TX);
    		CAN1_Send();
    		delay(100);

    		switch(REQ_BUFFER[0])
    		{
    		case 0x27:
    //			SID_27_Practice();
    			//Service27();
    			checkAndHandle_ECU();
    			checkAndHandle_tester();
    			break;
    		case 0x22:
    			//Service22();
    			checkAndHandle_ECU();
    			checkAndHandle_tester();
    			break;
    		case 0x2E:
    //			SID_2E_Practice();
    			//Service2E();
    			checkAndHandle_ECU();
    			checkAndHandle_tester();
    			break;
    		default:
    			USART3_SendString((uint8_t*)"Service not support");
    			break;
    		}
    		PrintCANLog(CAN1_pHeaderRx.StdId,CAN1_DATA_RX);
    		NumBytesReq=0;
    	}
  if(!BtnU) /*IG OFF->ON stimulation*/
  {
    delay(20);
    USART3_SendString((uint8_t *)"IG OFF ");
    ECU_ID=ECU_IDtmp;
    MX_CAN2_Init();
    MX_CAN1_Init();
    MX_CAN2_Setup();
    MX_CAN1_Setup();
    while(!BtnU);
    USART3_SendString((uint8_t *)"-> IG ON \n");
    delay(20);
  }

}
}
  /* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */
void CAN1_Send()
{
	 PrintCANLog(CAN1_pHeader.StdId, CAN1_DATA_TX);
	  if(HAL_CAN_AddTxMessage(&hcan1, &CAN1_pHeader, CAN1_DATA_TX, &CAN1_pTxMailbox) != HAL_OK)
		{
		 Error_Handler();
		}

}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */
  CAN1_pHeader.DLC = 8;
  CAN1_pHeader.IDE = CAN_ID_STD;
  CAN1_pHeader.RTR = CAN_RTR_DATA;
  CAN1_pHeader.StdId = tester_ID;

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 16;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_11TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */
  CAN1_sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
  CAN1_sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  CAN1_sFilterConfig.SlaveStartFilterBank = 13;
  CAN1_sFilterConfig.FilterBank = 8;
  CAN1_sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  CAN1_sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
  CAN1_sFilterConfig.FilterIdHigh = ECU_ID << 5;
  CAN1_sFilterConfig.FilterIdLow = 0;
  CAN1_sFilterConfig.FilterMaskIdHigh = ECU_ID << 5;
  CAN1_sFilterConfig.FilterMaskIdLow = 0;
  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief CAN2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN2_Init(void)
{

  /* USER CODE BEGIN CAN2_Init 0 */
  CAN2_pHeader.DLC = 8;
  CAN2_pHeader.IDE = CAN_ID_STD;
  CAN2_pHeader.RTR = CAN_RTR_DATA;
  CAN2_pHeader.StdId = ECU_ID;

  /* USER CODE END CAN2_Init 0 */

  /* USER CODE BEGIN CAN2_Init 1 */

  /* USER CODE END CAN2_Init 1 */
  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 16;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_11TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = DISABLE;
  hcan2.Init.AutoWakeUp = DISABLE;
  hcan2.Init.AutoRetransmission = DISABLE;
  hcan2.Init.ReceiveFifoLocked = DISABLE;
  hcan2.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN2_Init 2 */
  CAN2_sFilterConfig.FilterActivation = CAN_FILTER_ENABLE; // Báº­t táº¯t filter
  CAN2_sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO1;
  CAN2_sFilterConfig.SlaveStartFilterBank = 13;
  CAN2_sFilterConfig.FilterBank = 19; // Filter Ä‘Æ°á»£c sá»­ dá»¥ng trong tiáº¿n trÃ¬nh
  CAN2_sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  CAN2_sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
  CAN2_sFilterConfig.FilterIdHigh = tester_ID << 5;
  CAN2_sFilterConfig.FilterIdLow = 0;
  CAN2_sFilterConfig.FilterMaskIdHigh = tester_ID << 5;
  CAN2_sFilterConfig.FilterMaskIdLow = 0;
  /* USER CODE END CAN2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : PC13 PC4 PC5 PC6
                           PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void readFromTester(uint8_t* req_tester, uint16_t len, uint8_t* data_tx) {
  data_tx[0] = len;
  uint8_t loopIndx;
  for (loopIndx = 0; loopIndx < len; loopIndx++) {
    data_tx[loopIndx+1] = req_tester[loopIndx];
  }
  while(loopIndx < 7) {
    loopIndx++;
    data_tx[loopIndx] = 0x55;
  }
}

void MX_CAN1_Setup()
{
	HAL_CAN_ConfigFilter(&hcan1, &CAN1_sFilterConfig);
	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}
void MX_CAN2_Setup()
{
	HAL_CAN_ConfigFilter(&hcan2, &CAN2_sFilterConfig);
	HAL_CAN_Start(&hcan2);
	HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING);
}

void USART3_SendString(uint8_t *ch)
{
   while(*ch!=0)
   {
      HAL_UART_Transmit(&huart3, ch, 1,HAL_MAX_DELAY);
      ch++;
   }
}
void PrintCANLog(uint16_t CANID, uint8_t * CAN_Frame)
{
	uint16_t loopIndx = 0;
	char bufID[3] = "   ";
	char bufDat[2] = "  ";
	char bufTime [8]="        ";

	sprintf(bufTime,"%d",TimeStamp);
	USART3_SendString((uint8_t*)bufTime);
	USART3_SendString((uint8_t*)" ");

	sprintf(bufID,"%03X",CANID);
	for(loopIndx = 0; loopIndx < 3; loopIndx ++)
	{
		bufsend[loopIndx]  = bufID[loopIndx];
	}
	bufsend[3] = ':';
	bufsend[4] = ' ';


	for(loopIndx = 0; loopIndx < 8; loopIndx ++ )
	{
		sprintf(bufDat,"%02X",CAN_Frame[loopIndx]);
		bufsend[loopIndx*3 + 5] = bufDat[0];
		bufsend[loopIndx*3 + 6] = bufDat[1];
		bufsend[loopIndx*3 + 7] = ' ';
	}
	bufsend[29] = '\n';
	USART3_SendString((unsigned char*)bufsend);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	REQ_BUFFER[NumBytesReq] = REQ_1BYTE_DATA;
	NumBytesReq++;

	//REQ_BUFFER[7] = NumBytesReq;
}
void delay(uint16_t delay)
{
	HAL_Delay(delay);
}


//    if (GPIO_Pin == USER_Pin)
//    {
//        ECU_ID1=ECU_IDtmp1;
//        ECU_ID2=ECU_IDtmp2;
//
//    }

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){

	HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &CAN1_pHeaderRx, CAN1_DATA_RX);
	char buffer3[9] = "CAN1RX\n";
	USART3_SendString((unsigned char *)buffer3);
	fifo=0;
	PrintCANLog(tester_ID, CAN1_DATA_RX);
}
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan){

	HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO1, &CAN2_pHeaderRx, CAN2_DATA_RX);
	char buffer4[9] = "CAN2RX\n";
	USART3_SendString((unsigned char *)buffer4);
	fifo=1;
	PrintCANLog(ECU_ID, CAN2_DATA_RX);
}

// Ham check

void checkAndHandle_ECU() {
	if (fifo == 1) {
		fifo = 2;
		uint8_t PCI = get_PCI(CAN2_DATA_RX[0]);
		uint8_t LEN = get_size(CAN2_DATA_RX[0]);

		if (PCI == 0x00) {
			uint8_t SID = get_SID(CAN2_DATA_RX);
			switch (SID) {
			case 0x22:
				if (LEN == 0x03) {

					HandleSID22();


				} else {
					SecurityAccessNegativeResponseMessage(CAN2_DATA_TX, 0x13,SID);
					HAL_CAN_AddTxMessage(&hcan2, &CAN2_pHeader, CAN2_DATA_TX,&CAN2_pTxMailbox);
				}
				break;
			case 0x27:
				if (LEN == 0x06 || LEN == 0x02||LEN ==0x07)
					HandleSID27();
			 else {
								SecurityAccessNegativeResponseMessage(CAN2_DATA_TX, 0x13, SID);
								HAL_CAN_AddTxMessage(&hcan2, &CAN2_pHeader, CAN2_DATA_TX,&CAN2_pTxMailbox);
			 	 }
				break;
			case 0x2E:
				if (enb2E==1)
					HandleSID2E();
				else {
					SecurityAccessNegativeResponseMessage(CAN2_DATA_TX, 0x33,SID);
					HAL_CAN_AddTxMessage(&hcan2, &CAN2_pHeader, CAN2_DATA_TX,&CAN2_pTxMailbox);

				}
				break;
			default:
				break;
			}
		}
	}

}
void checkAndHandle_tester() {
	if (fifo == 0) {
		fifo = 2;
		uint8_t PCI = get_PCI(CAN1_DATA_RX[0]);
		if (PCI == 0x00) {
			uint8_t SID = get_SID(CAN1_DATA_RX);
			switch (SID) {
			case 0x62:
				HandleSID62(CAN1_DATA_RX);
				PrintCANLog(0xFF,CAN1_DATA_RX);
				break;
			case 0x67:
				HandleSID67();
				break;
			case 0x6E:
				HandleSID6E();
				break;

			default:
				break;
			}
		}
	}
}


//Ham xu ly
void HandleSID2E() {

	uint8_t LEN = get_size(CAN2_DATA_RX[0]);
	if (LEN == 0X05) {
		uint16_t DID = get_DID(CAN2_DATA_RX);
		if (DID == 0x0123) {
			WriteDataByIdentifierResponseFrame(CAN2_DATA_TX);

			ECU_IDtmp=(CAN2_DATA_RX[4]<<8)|CAN2_DATA_RX[5];


		} else {
			SecurityAccessNegativeResponseMessage(CAN2_DATA_TX, 0x31, 0x2E);
		}
	}else{
			SecurityAccessNegativeResponseMessage(CAN2_DATA_TX,0x13 ,0x2E);
		}
		HAL_CAN_AddTxMessage(&hcan2, &CAN2_pHeader, CAN2_DATA_TX,
				&CAN2_pTxMailbox);
		delay(100);
	}


//Ham read
void ReadDataByIdentifierRequestFrame(uint8_t data[]){
	data[0] = 0x03;	// PCi, size
	data[1] = 0x22;	// SID
	data[2] = 0x01;	// DID
	data[3] = 0x23;	// DID
}
void ReadDataByIdentifierResponseFrame(uint8_t data[]){
	data[0] = 0x03;	// PCI
	data[1] = 0x62;	// PSID
	data[2] = 0x01;	// DID
	data[3] = 0x23; // DID
}
void WriteDataByIdentifierRequestFrame(uint8_t data[]){
	data[0] = 0x05;	// PCi, size
	data[1] = 0x2E;	// SID
	data[2] = 0x01;	// DID
	data[3] = 0x23;	// DID
  data[4] = 0xAA;
  data[5] = 0xBB;
}
void WriteDataByIdentifierResponseFrame(uint8_t data[]){
	data[0] = 0x03;	// PCi, size
	data[1] = 0x6E;	// PSID
	data[2] = 0x01;	// DID
	data[3] = 0x23;	// DID
}
void securityAccessServiceSeedRequestFrame(uint8_t data[]){
	data[0] = 0x02;	// PCI la 0, size la 2
	data[1] = 0x27;	// service 27
	data[2] = 0x01;	// request seed
}
void securityAccessServiceSeedResponseFrame(uint8_t data[]){
	data[0] = 0x06;	// PCI la 0, size la 4
	data[1] = 0x67;	// response $27 nen la $67
	data[2] = 0x01;	// request seed
	data[3] = generate_seed();	// Security Seed
	CAN2_STORE_SEED[3] = data[3];
	data[4] = generate_seed();	// Security Seed
	CAN2_STORE_SEED[4] = data[4];
	data[5] = generate_seed();	// Security Seed
	CAN2_STORE_SEED[5] = data[5];
	data[6] = generate_seed();	// Security Seed
	CAN2_STORE_SEED[6] = data[6];
  data[7] = 0x55;
}
void securityAccessSendKeyRequestFrame(uint8_t Tx_data[], uint8_t data[]){
	Tx_data[0] = 0x07;				// PCI la 0, size la 4
	Tx_data[1] = 0x27;				// response $27 nen la $67
	Tx_data[2] = 0x02;				// response seed = request seed + 1
  cal_key(Tx_data, data);
  Tx_data[7] = 0x55;
}
void securityAccessSendKeyResponseFrame(uint8_t data[]){
	data[0] = 0x02;	// PCI la 0, size la 2
	data[1] = 0x67;	// response $27 nen la $67
	data[2] = 0x02;	// response seed = request seed + 1
  data[3] = 0x55;
  data[4] = 0x55;
  data[5] = 0x55;
  data[6] = 0x55;
  data[7] = 0x55;


}
void SecurityAccessNegativeResponseMessage(uint8_t data[], uint8_t flag_NRC,uint8_t SID){
	data[0] = 0x03;	// PCI la 0, size la 3
	data[1] = 0x7F;	// NRC code
	data [2] = SID;
	data [3]=flag_NRC;
}
void HandleSID22(){
	uint16_t DID = get_DID(CAN2_DATA_RX);
	if (DID == 0x0123){
		ReadDataByIdentifierResponseFrame(CAN2_DATA_TX);

	}
	else{
		SecurityAccessNegativeResponseMessage(CAN2_DATA_TX, 0x31,0x22);
	}
	HAL_CAN_AddTxMessage(&hcan2, &CAN2_pHeader, CAN2_DATA_TX, &CAN2_pTxMailbox);
	delay(100);
}
void HandleSID27() {
	uint8_t SBF = get_sub_function(CAN2_DATA_RX);
		if (SBF == 0x01) {
			securityAccessServiceSeedResponseFrame(CAN2_DATA_TX);
			check = 1;
		} else if (SBF == 0x02&check==1) {
			if (checkKey(CAN2_DATA_RX, CAN2_STORE_SEED) == 1) {
				securityAccessSendKeyResponseFrame(CAN2_DATA_TX);

			} else {
				SecurityAccessNegativeResponseMessage(CAN2_DATA_TX, 0x35,0x27);// invalid key
			}
		}
	HAL_CAN_AddTxMessage(&hcan2, &CAN2_pHeader, CAN2_DATA_TX, &CAN2_pTxMailbox);
	delay(100);
}
//
void HandleSID6E(){
	uint16_t DID = get_DID(CAN2_DATA_RX);
	if (DID == 0x0123){
		WriteDataByIdentifierResponseFrame(CAN2_DATA_TX);
	}
	else{
		SecurityAccessNegativeResponseMessage(CAN2_DATA_TX, 0x31,0x6E);
	}
	HAL_CAN_AddTxMessage(&hcan2, &CAN2_pHeader, CAN2_DATA_TX, &CAN2_pTxMailbox);
	delay(100);
}
void HandleSID67(){
	uint8_t SBF = get_sub_function(CAN1_DATA_RX);
	if (SBF == 0x01){
		securityAccessSendKeyRequestFrame(CAN1_DATA_TX, CAN1_DATA_RX);
		HAL_CAN_AddTxMessage(&hcan1, &CAN1_pHeader, CAN1_DATA_TX, &CAN1_pTxMailbox);
		delay(100);
	}else{
		enb2E=1;
	}
}
void HandleSID62(uint8_t data[]) {
	data[0] = 0x05;
	data[1] = 0x62;
	data[2] = 0x01;
	data[3] = 0x23;
	data[4] = (ECU_ID>>8)&0XFF;
	data[5] = ECU_ID&0XFF;
	data[6] = 0x55;
	data[7] = 0x55;
}

// Ham get
uint8_t get_sub_function (uint8_t data[]) {
  return data[2];
}
uint8_t generate_seed(){
	return (uint8_t)(HAL_GetTick());
}
uint8_t checkKey (uint8_t data[], uint8_t CAN2_STORE_SEED[] ) {
  uint8_t compareData[8];
  cal_key(compareData, CAN2_STORE_SEED);
  for (int loopIndx = 3; loopIndx < 8; loopIndx++) {
    if (data[loopIndx] != CAN2_STORE_SEED[loopIndx]) return 0;
  }
  return 1;
}
uint8_t get_NRC(uint8_t data[]) {
  return data[2];
}
uint8_t* cal_key(uint8_t Tx_data[], uint8_t data[]) {
  Tx_data[3] = data[3] ^ data[4];
  Tx_data[4] = data[4] + data[5];
  Tx_data[5] = data[5] ^ data[6];
  Tx_data[6] = data[6] + data[3];
  return Tx_data;
}
uint16_t get_DID(uint8_t data[]) {
  uint16_t temp = data[2];
  temp <<= 8;
  temp += data[3];
  return temp;
}
uint8_t get_SID(uint8_t data[]) {
  return data[1];
}
uint8_t get_size(uint8_t data) {
  return data & 0x0F;
}
uint8_t get_PCI(uint8_t data) {
  return data >> 4;
}


//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
//}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

