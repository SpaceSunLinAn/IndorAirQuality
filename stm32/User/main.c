#include "stm32f10x.h" //STM32头文件
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "i2c.h"
#include "main.h"
#include "led.h"

struct _air_data {
  int _iaq;
  float _tvoc;
  float _eco2;
  float _rmox;
}; 
struct _air_data air_data;

int main (void){
    zmod44xx_dev_t dev;
    int8_t ret;
    uint8_t zmod44xx_status;
    float r_mox;  
    uint8_t iaq;
	uint8_t _head;
    float eco2;
    float r_cda;
    float tvoc;
    control_signal_state_t cs_state;

    eco2_params eco2_par = {
            .min_co2 = 400,
            .max_co2 = 5000,
            .tvoc_to_eco2 = 800,
            .hot_wine_rate = 0.3,
            .open_window_rate = -0.05,
    };

    tvoc_params tvoc_par = {
            .A = 680034,
            .alpha = 0.7,
    };

    odor_params odor_par = {
            .alpha = 0.8,
            .stop_delay = 24,
            .threshold = 1.3,
            .tau = 720,
            .stabilization_samples = 15,
    };   
    dev.read = I2C_READ_BUFFER;
    dev.write = I2C_SAND_BUFFER;	 
    dev.delay_ms = delay_ms;
	dev.i2c_addr = ZMOD4410_I2C_ADDRESS;   
	RCC_Configuration(); //系统时钟初始化 
   	LED_Init();
	USART2_Init(115200);
	GPIO_WriteBit(LEDPORT,LED0,(BitAction)(0));
	delay_ms(100);
	GPIO_WriteBit(LEDPORT,LED0,(BitAction)(1));
	GPIO_WriteBit(LEDPORT,LED1,(BitAction)(0));
	I2C_Configuration();
    ret = zmod44xx_read_sensor_info(&dev);
//	printf("read sensor info\n");
	if(ret) {
        printf("Error %d, exiting program!\n", ret);
        goto exit;
    }

    ret = zmod44xx_init_sensor(&dev);
    if(ret) {
        printf("Error %d, exiting program!\n", ret);
        goto exit;
    }
	   
    ret = zmod44xx_init_measurement(&dev);
    if(ret) {
        printf("Error %d, exiting program!\n", ret);
        goto exit;
    }

    ret = zmod44xx_start_measurement(&dev);
    if(ret) {
        printf("Error %d, exiting program!\n", ret);
        goto exit;
    }

    do {
        dev.delay_ms(50);
        ret = zmod44xx_read_status(&dev, &zmod44xx_status);
        if(ret) {
            printf("Error %d, exiting program!\n", ret);
            goto exit;
        }
    } while (FIRST_SEQ_STEP != (zmod44xx_status & 0x07));

//    printf("Evaluate measurements in a loop. Press any key to quit.\n");
    do {	
        while (LAST_SEQ_STEP != (zmod44xx_status & 0x07)) {
            dev.delay_ms(50);
            ret = zmod44xx_read_status(&dev, &zmod44xx_status);
            if(ret) {
                printf("Error %d, exiting program!\n", ret);
                goto exit;
            }
        }

        // evaluate and show measurement results 
        ret = zmod44xx_read_rmox(&dev, &r_mox);
        if(ret) {
            printf("Error %d, exiting program!\n", ret);
            goto exit;
        }

        // To work with the algorithms target specific libraries needs to be
         // downloaded from IDT webpage and included into the project 

        // calculate clean dry air resistor 
        r_cda = r_cda_tracker(r_mox);

        // calculate result to TVOC value 
        tvoc = calc_tvoc(r_mox, r_cda, &tvoc_par);

        // calculate IAQ index 
        iaq = calc_iaq(r_mox, r_cda, &tvoc_par);

        // calculate estimated CO2 
        eco2 = calc_eco2(tvoc, 3, &eco2_par);

        // get odor control signal 
        cs_state = calc_odor(r_mox, &odor_par);
		_head = 99;
		air_data._iaq = iaq;
		air_data._eco2 = eco2;
		air_data._rmox = r_cda/1000;
		printf("A,%5.0f,%d,%.2f,%f,",air_data._rmox, air_data._iaq,air_data._eco2,tvoc);
//		printf("size of air_data = %d\n",sizeof(struct _air_data));
//        printf("Measurement:\n");
//        printf("  Rmox  = %5.0f kOhm\n", (r_mox / 1000.0));
//        printf("  IAQ %d\n", iaq);
//        printf("  TVOC %f mg/m^3\n", tvoc);
//        printf("  eCO2 %f\n", eco2);
//        printf("  odor control state %d\n", cs_state);

        // INSTEAD OF POLLING THE INTERRUPT CAN BE USED FOR OTHER HW 
        // waiting for sensor ready 
        while (FIRST_SEQ_STEP != (zmod44xx_status & 0x07)) {
            dev.delay_ms(50);
            ret = zmod44xx_read_status(&dev, &zmod44xx_status);
            if(ret) {
                printf("Error %d, exiting program!\n", ret);
                goto exit;
            }
        }
    // kbhit() is a windows specific function and needs to be removed for
      // other hardware.
    } while (1);
//    } while (!kbhit());

exit:
while(1) {
		printf("end ------\n");
		delay_ms(100);
	}

    printf("Bye\n");  	 
    return 0;
}

/*********************************************************************************************
 * 杜洋工作室 www.DoYoung.net
 * 洋桃电子 www.DoYoung.net/YT 
*********************************************************************************************/
/*

【变量定义】
u32     a; //定义32位无符号变量a
u16     a; //定义16位无符号变量a
u8     a; //定义8位无符号变量a
vu32     a; //定义易变的32位无符号变量a
vu16     a; //定义易变的 16位无符号变量a
vu8     a; //定义易变的 8位无符号变量a
uc32     a; //定义只读的32位无符号变量a
uc16     a; //定义只读 的16位无符号变量a
uc8     a; //定义只读 的8位无符号变量a

#define ONE  1   //宏定义

delay_us(1); //延时1微秒
delay_ms(1); //延时1毫秒
delay_s(1); //延时1秒

GPIO_WriteBit(LEDPORT,LED1,(BitAction)(1)); //LED控制

*/



