#ifndef __BSP_CH423_H__
#define __BSP_CH423_H__

// CH423 interface definition
#define     CH423_I2C_ADDR1     0x40         // Address of CH423
#define     CH423_I2C_MASK      0x3E         // CH423 high byte command mask

/* Set system parameter command */

#define     CH423_SYS_CMD     0x4800     // Set system parameter command, default mode
#define     BIT_SLEEP         0X40
#define     BIT_INTENS        0X20          //
#define     BIT_OD_EN         0X10          //0: Push-pull output (can output low and high levels) 1: Open-drain output (can only output low level and no output) Default is push-pull output
#define     BIT_X_INT         0x08       // Enable input level change interrupt, 0 to disable input level change interrupt; 1 and DEC_H are allowed to output level change interrupt
#define     BIT_DEC_H         0x04       // Control the high 8-bit chip selection code of open-drain output pin
#define     BIT_DEC_L         0x02       // Control the low 8-bit chip selection code of open-drain output pin
#define     BIT_IO_OE         0x01       // Control the three-state output of bidirectional input and output pins, 1 to allow output

/* Set low 8-bit open-drain output command */
#define     CH423_OC_L_CMD    0x4400     // Set low 8-bit open-drain output command, default mode
#define     BIT_OC0_L_DAT     0x01       // If OC0 is 0, the pin outputs low level, otherwise the pin does not output (high level)
#define     BIT_OC1_L_DAT     0x02       // If OC1 is 0, the pin outputs low level, otherwise the pin does not output (high level)
#define     BIT_OC2_L_DAT     0x04       // If OC2 is 0, the pin outputs low level, otherwise the pin does not output (high level)
#define     BIT_OC3_L_DAT     0x08       // If OC3 is 0, the pin outputs low level, otherwise the pin does not output (high level)
#define     BIT_OC4_L_DAT     0x10       // If OC4 is 0, the pin outputs low level, otherwise the pin does not output (high level)
#define     BIT_OC5_L_DAT     0x20       // If OC5 is 0, the pin outputs low level, otherwise the pin does not output (high level)
#define     BIT_OC6_L_DAT     0x40       // If OC6 is 0, the pin outputs low level, otherwise the pin does not output (high level)
#define     BIT_OC7_L_DAT     0x80       // If OC7 is 0, the pin outputs low level, otherwise the pin does not output (high level)

#define     CH423_OC_H_CMD    0x4600      // Set low 8-bit open-drain output command, default mode
#define     BIT_OC8_L_DAT     0x01        // If OC8 is 0, the pin outputs low level, otherwise the pin does not output (high level)
#define     BIT_OC9_L_DAT     0x02        // If OC9 is 0, the pin outputs low level, otherwise the pin does not output (high level)
#define     BIT_OC10_L_DAT    0x04        // If OC10 is 0, the pin outputs low level, otherwise the pin does not output (high level)
#define     BIT_OC11_L_DAT    0x08        // If OC11 is 0, the pin outputs low level, otherwise the pin does not output (high level)
#define     BIT_OC12_L_DAT    0x10        // If OC12 is 0, the pin outputs low level, otherwise the pin does not output (high level)
#define     BIT_OC13_L_DAT    0x20        // If OC13 is 0, the pin outputs low level, otherwise the pin does not output (high level)
#define     BIT_OC14_L_DAT    0x40        // If OC14 is 0, the pin outputs low level, otherwise the pin does not output (high level)
#define     BIT_OC15_L_DAT    0x80        // If OC15 is 0, the pin outputs low level, otherwise the pin does not output (high level)

/* Set bidirectional input/output command */

#define     CH423_SET_IO_CMD   0x6000    // Set bidirectional input/output command, default mode
#define     BIT_IO0_DAT        0x01      // Write to output register of bidirectional input/output pin, when IO_OE=1, IO0 outputs low level when 0, high level when 1
#define     BIT_IO1_DAT        0x02      // Write to output register of bidirectional input/output pin, When IO_OE=1, IO1 outputs low level when 0, high level when 1
#define     BIT_IO2_DAT        0x04      // Write to output register of bidirectional input/output pin, When IO_OE=1, IO2 outputs low level when 0, high level when 1
#define     BIT_IO3_DAT        0x08      // Write to output register of bidirectional input/output pin, When IO_OE=1, IO3 outputs low level when 0, high level when 1
#define     BIT_IO4_DAT        0x10      // Write to output register of bidirectional input/output pin, When IO_OE=1, IO4 outputs low level when 0, high level when 1
#define     BIT_IO5_DAT        0x20      // Write to output register of bidirectional input/output pin, When IO_OE=1, IO5 outputs low level when 0, high level when 1
#define     BIT_IO6_DAT        0x40      // Write to output register of bidirectional input/output pin, When IO_OE=1, IO6 outputs low level when 0, high level when 1
#define     BIT_IO7_DAT        0x80      // Write to output register of bidirectional input/output pin, When IO_OE=1, IO7 outputs low level when 0, high level when 1

/* Read bidirectional input/output command */

#define CH423_RD_IO_CMD     0x4D    // Input/Output pin current status

void CH423_Init(void);
void CH423_Write(unsigned short cmd);           // Write command
void CH423_WriteByte(unsigned short cmd);       // Write out data
unsigned char CH423_ReadByte();                 // Read data

void SetIOChannel(unsigned char IOChannel);
void ClrIOChannel(unsigned char IOChannel);
void SetOCChannel(unsigned char OCChannel);
void ClrOCChannel(unsigned char OCChannel);

//IO
#define        VDO_PIN      5
#define  MIC_IN_EN_PIN      6
#define MIC_OUT_EN_PIN      7
#define  SPK_IN_EN_PIN      1
#define SPK_OUT_EN_PIN      0

//OC
#define POWER_EN_8_CHAN     1
#define POWER_EN_12_CHAN    2
#define FM_AMP_EN_CHAN      8


#define SET_VDO_PIN            SetIOChannel(VDO_PIN)
#define CLR_VDO_PIN            ClrIOChannel(VDO_PIN)

#define SET_MIC_IN_EN_PIN       SetIOChannel (MIC_IN_EN_PIN)
#define CLR_MIC_IN_EN_PIN       ClrIOChannel (MIC_IN_EN_PIN)
#define SET_MIC_OUT_EN_PIN      SetIOChannel (MIC_OUT_EN_PIN)
#define CLR_MIC_OUT_EN_PIN      ClrIOChannel (MIC_OUT_EN_PIN)

#define SET_SPK_IN_EN_PIN       SetIOChannel (SPK_IN_EN_PIN)
#define CLR_SPK_IN_EN_PIN       ClrIOChannel (SPK_IN_EN_PIN)
#define SET_SPK_OUT_EN_PIN      SetIOChannel (SPK_OUT_EN_PIN)
#define CLR_SPK_OUT_EN_PIN      ClrIOChannel (SPK_OUT_EN_PIN)


#define SET_POWER_EN_8_CHAN     SetOCChannel(POWER_EN_8_CHAN)
#define CLR_POWER_EN_8_CHAN     ClrOCChannel(POWER_EN_8_CHAN)
#define SET_POWER_EN_12_CHAN    SetOCChannel(POWER_EN_12_CHAN)
#define CLR_POWER_EN_12_CHAN    ClrOCChannel(POWER_EN_12_CHAN)

#define SET_FM_AMP_EN_CHAN     SetOCChannel(FM_AMP_EN_CHAN)
#define CLR_FM_AMP_EN_CHAN     ClrOCChannel(FM_AMP_EN_CHAN)


#endif
