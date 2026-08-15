//
// Created by lxcy on 2026/7/29.
//
#include "USB_JustFloat.h"

#include "usbd_cdc_if.h"
#include "PMSM_Control_Core/Clarke_Park.h"
#include "PMSM_Control_Core/FluxObserver_PLL.h"
#include "PMSM_Control_Core/PI_Controller.h"
/*
 *使用以下示例可以测得USB发送数据中的float转换消耗:
    #define singleDataLength 9

    USB_data[USB_DataRecordIndex++]=TIM1->CNT;
    USB_data[USB_DataRecordIndex++]=Q15_I_GetValue(Iq_PIstate.Measure);
    USB_data[USB_DataRecordIndex++]=TIM1->CNT;
    USB_data[USB_DataRecordIndex++]=Iq_PIstate.Measure.child_value;
    USB_data[USB_DataRecordIndex++]=TIM1->CNT;
    USB_data[USB_DataRecordIndex++]=Id_PIstate.Set.child_value;
    USB_data[USB_DataRecordIndex++]=Id_PIstate.Measure.child_value;
    USB_data[USB_DataRecordIndex++]=TIM1->CNT;
    USB_data[USB_DataRecordIndex++]=*((float*)&tail[0]);
    上位机看到:
    1220  0.012524  1338  342  1375  0  -92  1466
    可知，直接将int16_t转成float需要消耗37个TIM1计数值(其中测试时，TIM1配置为15khz中心对称PWM,向上向下计数0->2400->0)
    调用Q15_xx_GetValue函数需要消耗118个TIM1计数值,建议尽量使用int16_t原始值,9个int16_t -> float 需要 37*9=333个计数值，占比不大(333/4800=7%中断时间)
    而使用浮点值发送则是118*9=1062，占比比较大(1062/4800=22%中断时间)，建议在计算资源比较充裕的时候使用，不然会导致中断风暴
    值得注意的是，上述测试只是比较粗略的测试，实际可能需要反复多次测试得到平均值才更准确，但结论大体不变
*/

#define singleDataLength 9 //单次记录需要的数据长度,注意这里要算上帧尾(以float为单位)
#define USB_HalfDataCount 100//填充数据的一半数量
#define USB_HalfDataLength (singleDataLength*USB_HalfDataCount)//填充数据的一半长度
#define USB_MaxDataRecordLength (USB_HalfDataLength*2)//当数据每被填满一半，就发送一次数据
static uint32_t USB_DataRecordIndex=0;//表示已经记录数据的下标
static float USB_data[USB_MaxDataRecordLength+1000];//长度为USB_MaxDataRecordLength+1000，留有一定裕量，防止超过数组下标(理论上不会超过)
unsigned char tail[4]={0x00,0x00,0x80,0x7f};
//记录电机运行和系统状态，用于发送给上位机
void recordRunningData() {
    USB_data[USB_DataRecordIndex++]=Id_PIstate.Set.child_value;
    USB_data[USB_DataRecordIndex++]=Id_PIstate.Measure.child_value;
    USB_data[USB_DataRecordIndex++]=Iq_PIstate.Set.child_value;
    USB_data[USB_DataRecordIndex++]=Iq_PIstate.Measure.child_value;
    USB_data[USB_DataRecordIndex++]=Speed_PIstate.Set.child_value;
    USB_data[USB_DataRecordIndex++]=Speed_PIstate.Measure.child_value;
    USB_data[USB_DataRecordIndex++]=fluxObserver_pll_est.Flux_alpha_O.child_value;
    USB_data[USB_DataRecordIndex++]=fluxObserver_pll_est.Etheta_O.child_value;
    USB_data[USB_DataRecordIndex++]=*((float*)&tail[0]);
    if (USB_DataRecordIndex==USB_HalfDataLength) {
        CDC_Transmit_FS((uint8_t*)USB_data,USB_HalfDataLength*4);
    }
    else if (USB_DataRecordIndex==USB_MaxDataRecordLength) {
        CDC_Transmit_FS((uint8_t*)(USB_data+USB_HalfDataLength),USB_HalfDataLength*4);
        USB_DataRecordIndex=0;
    }
}