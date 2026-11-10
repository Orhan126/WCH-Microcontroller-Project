#include <ch32v00x.h>
#include <debug.h>

#define ADC_GPIO_PORT   GPIOD
#define ADC_GPIO_PIN    GPIO_Pin_4
#define ADC_CHANNEL     ADC_Channel_7

// Timer 1 PWM Pins for CH32V003
// Red: PD2 (TIM1_CH1), Green: PC3 (TIM1_CH3), Blue: PC4 (TIM1_CH4)
#define PWM_PERIOD      255  // 8-bit resolution (0-255)

void ADC_Function_Init(void);
uint16_t Get_ADC_Value(uint8_t ch);
void TIM1_PWM_Init(uint16_t arr, uint16_t psc);
void Set_RGB_PWM(uint8_t r, uint8_t g, uint8_t b);
void HSV_To_RGB(uint16_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b);

/*********************************************************************
 * @fn      TIM1_PWM_Init
 *
 * @brief   Configures TIM1 Channels 1, 3, and 4 for PWM output.
 */
void TIM1_PWM_Init(uint16_t arr, uint16_t psc)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};

    // 1. Enable clocks for GPIOC, GPIOD, and TIM1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_TIM1, ENABLE);

    // 2. Configure PWM Pins as Alternate Function Push-Pull
    // PD2 -> TIM1_CH1 (Red)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // PC3 -> TIM1_CH3 (Green), PC4 -> TIM1_CH4 (Blue)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 3. Configure Time Base
    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    // 4. Configure PWM Channels
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // High = ON for Common Cathode
    TIM_OCInitStructure.TIM_Pulse = 0;

    TIM_OC1Init(TIM1, &TIM_OCInitStructure); // Channel 1 (Red)
    TIM_OC3Init(TIM1, &TIM_OCInitStructure); // Channel 3 (Green)
    TIM_OC4Init(TIM1, &TIM_OCInitStructure); // Channel 4 (Blue)

    // Preload enable
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);

    // Enable TIM1 main output (Required for Advanced Timer TIM1)
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    // Enable Counter
    TIM_Cmd(TIM1, ENABLE);
}

/*********************************************************************
 * @fn      Set_RGB_PWM
 *
 * @brief   Update PWM duty cycle for each channel (0-255).
 */
void Set_RGB_PWM(uint8_t r, uint8_t g, uint8_t b)
{
    // For Common Anode LEDs, invert values: (PWM_PERIOD - r)
    TIM_SetCompare1(TIM1, r); // Red
    TIM_SetCompare3(TIM1, g); // Green
    TIM_SetCompare4(TIM1, b); // Blue
}

/*********************************************************************
 * @fn      HSV_To_RGB
 *
 * @brief   Converts Hue (0-360) to RGB (0-255) for smooth transitions.
 */
void HSV_To_RGB(uint16_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b)
{
    uint8_t region, remainder, p, q, t;

    if (s == 0) {
        *r = *g = *b = v;
        return;
    }

    region = h / 60;
    remainder = (h % 60) * 255 / 60;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
        case 0:  *r = v; *g = t; *b = p; break;
        case 1:  *r = q; *g = v; *b = p; break;
        case 2:  *r = p; *g = v; *b = t; break;
        case 3:  *r = p; *g = q; *b = v; break;
        case 4:  *r = t; *g = p; *b = v; break;
        default: *r = v; *g = p; *b = q; break;
    }
}

void ADC_Function_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    ADC_InitTypeDef ADC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    GPIO_InitStructure.GPIO_Pin = ADC_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(ADC_GPIO_PORT, &GPIO_InitStructure);

    ADC_DeInit(ADC1);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;

    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));

    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}

uint16_t Get_ADC_Value(uint8_t ch)
{
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_241Cycles);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
}

int main(void)
{
    uint16_t adc_val = 0;
    uint16_t hue = 0;
    uint8_t r = 0, g = 0, b = 0;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();

#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init(115200);
#endif

    printf("CH32V003 PWM Smooth RGB Blend Test\r\n");

    ADC_Function_Init();

    // Initialize TIM1 PWM: 
    // SystemClock = 48MHz -> Prescaler = 47 -> 1MHz Timer Clock
    // Period = 255 -> PWM Frequency ≈ 3.9kHz (flicker-free)
    TIM1_PWM_Init(PWM_PERIOD, 47);

    while (1)
    {
        adc_val = Get_ADC_Value(ADC_CHANNEL);

        // Map 10-bit ADC (0-1023) to Hue angle (0-360 degrees)
        hue = ((uint32_t)adc_val * 360) / 1023;

        // Convert Hue angle to RGB values at 100% saturation and brightness
        HSV_To_RGB(hue, 255, 255, &r, &g, &b);

        // Set PWM output
        Set_RGB_PWM(r, g, b);

        printf("ADC: %d | Hue: %d deg | R: %d, G: %d, B: %d\r\n", adc_val, hue, r, g, b);

        Delay_Ms(20); // Fast update rate for smooth transitions
    }
}