#include "bsp_storage.h"
#include "lcd.h"
#include "bsp_timer.h"

#include "nvs.h"
#include "nvs_flash.h"

size_t MEM_LENGTH = 64; //40    //



const char* TAG_FLAG[]= {
    "RESETADDR",
    "FLAG_CF",
    "FLAG_VU"
};


CHAN_ARV chan_arv[ARV_MEM_COUNT] = {1, 0, 0, 1, 0, 0, 435.025, 435.025, "       "};

nvs_handle PRC152handle;                    //储存空间打开后的操作句柄
esp_err_t  err;                             //操作结果返回值
const char* PRC152MARK2 = "PRC152MARK2";    //储存空间名称
void Init_Storage(void) {
    err = nvs_flash_init();
    if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = nvs_open(PRC152MARK2, NVS_READWRITE, &PRC152handle);
    if(err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        printf("Failed to initialise NVS\n");
        printf("Restarting...\n");
        delay(1000);
        ESP.restart();
    }
    D_printf("Non-volatile File Initial Successfully!\n");
    DATA_Init();
}

//修改标志位
void    set_Flag(int flag_number, uint8_t Flag) {
    err = nvs_set_u8(PRC152handle, TAG_FLAG[flag_number], Flag);
    if(err) {
        log_v("set_Flag fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
}
uint8_t get_Flag(int flag_number) {
    uint8_t Flag = 0;
    err = nvs_get_u8(PRC152handle, TAG_FLAG[flag_number], &Flag);
    if(err) {
        log_v("get_Flag fail: %s", nvs_error(err));
    }
    return Flag;
}

//扫描添加
void    set_Scan(uint8_t channel, uint8_t scan) {
}
uint8_t get_Scan(uint8_t channel) {
    return 0;
}

void    save_ChanA(uint8_t chan) {
    err = nvs_set_u8(PRC152handle, "CHANA", chan);
    if(err) {
        log_v("save_ChanA fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
}
uint8_t load_ChanA(void) {
    uint8_t chan = 1;
    err = nvs_get_u8(PRC152handle, "CHANA", &chan);
    if(err) {
        log_v("load_ChanA fail: %s", nvs_error(err));
    }
    return chan;
}
//
void    save_ChanB(uint8_t chan) {
    err = nvs_set_u8(PRC152handle, "CHANB", chan);
    if(err) {
        log_v("save_ChanB fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
}
uint8_t load_ChanB(void) {
    uint8_t chan = 2;
    err = nvs_get_u8(PRC152handle, "CHANB", &chan);
    if(err) {
        log_v("load_ChanB fail: %s", nvs_error(err));
    }
    return chan;
}

//信道号
void    save_CurrentChannel(uint8_t channel) {
    err = nvs_set_u8(PRC152handle, "CURRENTCHAN", channel);
    if(err) {
        log_v("save_CurrentChannel fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
}
uint8_t load_CurrentChannel(void) {
    uint8_t chan = 0;
    err = nvs_get_u8(PRC152handle, "CURRENTCHAN", &chan);
    if(err) {
        log_v("load_CurrentChannel fail: %s", nvs_error(err));
    }
    return chan;
}
//

//步进
void    save_Step(uint8_t step) {
    step = step<3 ? step : 0;
    err = nvs_set_u8(PRC152handle, "STEP", step);

    if(err) {
        log_v("save_Step fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
}
uint8_t load_Step(void) {
    uint8_t step = 0;
    err = nvs_get_u8(PRC152handle, "STEP", &step);
    if(err) {
        log_v("load_Step fail: %s", nvs_error(err));
    }
    return step;
}
//

//静噪
void    save_Sql(uint8_t sql) {
    sql = sql<9 ? sql : 1;
    err = nvs_set_u8(PRC152handle, "SQL", sql);

    if(err) {
        log_v("save_Sql fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
}
uint8_t load_Sql(void) {
    uint8_t sql = 1;
    err = nvs_get_u8(PRC152handle, "SQL", &sql);
    if(err) {
        log_v("load_Sql fail: %s", nvs_error(err));
    }
    return sql;
}
//

//咪灵敏度
void    save_MicLevel(uint8_t mic) {
    mic = mic<=7 ? mic : 0;
    err = nvs_set_u8(PRC152handle, "MIC_LEVEL", mic);

    if(err) {
        log_v("save_MicLevel fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
}
uint8_t load_MicLevel(void) {
    uint8_t mic = 0;
    err = nvs_get_u8(PRC152handle, "MIC_LEVEL", &mic);
    if(err) {
        log_v("load_MicLevel fail: %s", nvs_error(err));
    }
    return mic;
}
//

//音频输出
void    save_AudioSelect(uint8_t audio) {
    audio = audio<=2 ? audio : 0;
    err = nvs_set_u8(PRC152handle, "AUDIO", audio);

    if(err) {
        log_v("save_AudioSelect fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
}
uint8_t load_AudioSelect(void) {
    uint8_t audio = 0;
    err = nvs_get_u8(PRC152handle, "AUDIO", &audio);
    if(err) {
        log_v("load_AudioSelect fail: %s", nvs_error(err));
    }
    return audio;
}
//

//加密
void    save_ScramLevel(uint8_t scram) {
    scram = scram<9 ? scram : 0;
    err = nvs_set_u8(PRC152handle, "SCRAM", scram);

    if(err) {
        log_v("save_ScramLevel fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
}
uint8_t load_ScramLevel(void) {
    uint8_t scram = 0;
    err = nvs_get_u8(PRC152handle, "SCRAM", &scram);
    if(err) {
        log_v("load_ScramLevel fail: %s", nvs_error(err));
    }
    return scram;
}
//

//发射延时
void    save_Tot(uint8_t tot) {
    tot = tot<10 ? tot : 0;
    err = nvs_set_u8(PRC152handle, "TOT", tot);

    if(err) {
        log_v("save_Tot fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
}
uint8_t load_Tot(void) {
    uint8_t tot = 0;
    err = nvs_get_u8(PRC152handle, "TOT", &tot);
    if(err) {
        log_v("load_Tot fail: %s", nvs_error(err));
    }
    return tot;
}
//

//背光强度
void    save_Backlightness(uint8_t backlightness) {
    backlightness = backlightness<=100 ? backlightness : 50;
    err = nvs_set_u8(PRC152handle, "BACKLIGHTNESS", backlightness);

    if(err) {
        log_v("save_Backlightness fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
}
uint8_t load_Backlightness(void) {
    uint8_t backlightness = 0;
    err = nvs_get_u8(PRC152handle, "BACKLIGHTNESS", &backlightness);
    if(err) {
        log_v("load_Backlightness fail: %s", nvs_error(err));
    }
    return backlightness;
}
//

//背光时间  1:10s  0:常亮
void    save_LampTime(uint8_t lamptime) {
    err = nvs_set_u8(PRC152handle, "LAMPTIME", lamptime);
    if(err) {
        log_v("save_LampTime fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
}
uint8_t load_LampTime(void) {
    uint8_t lamptime = 0;
    err = nvs_get_u8(PRC152handle, "LAMPTIME", &lamptime);
    if(err) {
        log_v("load_LampTime fail: %s", nvs_error(err));
    }
    return lamptime;
}
//

//对比度
void    save_ScreenContrast(uint8_t screencontrast) {
    screencontrast = screencontrast<7 ? screencontrast : 3;
    err = nvs_set_u8(PRC152handle, "SCREENCONTRAST", screencontrast);

    if(err) {
        log_v("save_ScreenContrast fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
}
uint8_t load_ScreenContrast(void) {
    uint8_t screencontrast = 3;
    err = nvs_get_u8(PRC152handle, "SCREENCONTRAST", &screencontrast);
    if(err) {
        log_v("load_ScreenContrast fail: %s", nvs_error(err));
    }
    return screencontrast;
}
//

//六针头输出
void    save_VDO(uint8_t vdo) {
    err = nvs_set_u8(PRC152handle, "VDO", vdo);
    if(err) {
        log_v("save_VDO fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
}
uint8_t load_VDO(void) {
    uint8_t vdo = 0;
    err = nvs_get_u8(PRC152handle, "VDO", &vdo);
    if(err) {
        log_v("load_VDO fail: %s", nvs_error(err));
    }
    return vdo;
}
//

//全局音量
void    save_OverVolume(uint8_t volume) {
    volume = volume<=7 ? volume : 2;
    err = nvs_set_u8(PRC152handle, "VOLUME", volume);

    if(err) {
        log_v("save_OverVolume fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
}
uint8_t load_OverVolume(void) {
    uint8_t volume = 2;
    err = nvs_get_u8(PRC152handle, "VOLUME", &volume);
    if(err) {
        log_v("load_OverVolume fail: %s", nvs_error(err));
    }
    return volume;
}
//

//PTT前置提示音
void    save_PreTone(uint8_t pretone) {
    pretone = pretone<2 ? pretone : 1;
    err = nvs_set_u8(PRC152handle, "PRETONE", pretone);

    if(err) {
        log_v("save_PreTone fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
}
uint8_t load_PreTone(void) {
    uint8_t pretone = 1;
    err = nvs_get_u8(PRC152handle, "PRETONE", &pretone);
    if(err) {
        log_v("load_PreTone fail: %s", nvs_error(err));
    }
    return pretone;
}
//

//PTT结束提示音
void    save_EndTone(uint8_t endtone) {
//    Serial.printf("change endtone:%d", endtone);
    endtone = endtone<2 ? endtone : 0;
    err = nvs_set_u8(PRC152handle, "ENDTONE", endtone);

    if(err) {
        log_v("save_EndTone fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
}
uint8_t load_EndTone(void) {
    uint8_t endtone = 0;
    err = nvs_get_u8(PRC152handle, "ENDTONE", &endtone);
    if(err) {
        log_v("load_EndTone fail: %s", nvs_error(err));
    }
    return endtone;
}
//

//收音机频率//870~1080
void save_FMFreq(int32_t fm_freq) {
    fm_freq = (fm_freq < 870 || fm_freq > 1080) ? 885 : fm_freq;
    err = nvs_set_i32(PRC152handle, "FM_FREQ", fm_freq);

    if(err) {
        log_v("save_FMFreq fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
}
int  load_FMFreq(void) {
    int32_t fm_freq = 0;
    err = nvs_get_i32(PRC152handle, "FM_FREQ", &fm_freq);
    if(err) {
        log_v("load_FMFreq fail: %s", nvs_error(err));
    }
    return fm_freq;
}
//


//信道数据保存
void save_ChannelParameter(uint8_t chan, CHAN_ARV S) {

    D_printf("Save:  CHAN:%3d, TS:%3d, RS:%3d, POWER:%d, GBW:%d, TX:%f,  RX:%f,  NN:%s**\n",
             chan,   S.TS,  S.RS,  S.POWER, S.GBW,  S.TX_FREQ, S.RX_FREQ, S.NN);

    char chan_save[MEM_LENGTH] = {0};
    char tag_chan[8] = "CHAN001";

    S.CHAN = chan;
    if(chan > 100) {
        chan = 1;
    }
    if(S.RS > 121) {
        S.RS = 0;
    }
    if(S.TS > 121) {
        S.TS = 0;
    }
    if(S.POWER > 1) {
        S.POWER = 1;
    }
    if(S.GBW > 1) {
        S.GBW = 1;
    }
    if(S.SCAN > 1) {
        S.SCAN = 0;
    }

    if(S.RX_FREQ < 136.0) {
        S.RX_FREQ = 136.0000;
    } else if((S.RX_FREQ > 174.0 && S.RX_FREQ < 400.0) || S.RX_FREQ > 480.0) {
        S.RX_FREQ = 435.0250;
    }

    if(S.TX_FREQ < 136.0 && S.RX_FREQ < 174.0) {
        S.TX_FREQ = 136.0000;
    } else if((S.TX_FREQ > 174.0 && S.TX_FREQ < 400.0) || S.TX_FREQ > 480.0) {
        S.TX_FREQ = 435.0250;
    }

    sprintf(chan_save, "%03d%3.4f%3.4f%03d%03d%d%d", chan, S.RX_FREQ, S.TX_FREQ, S.RS, S.TS, S.POWER, S.GBW);
    for(uint8_t i = 0; i < 8; i++) {
        chan_save[NN_RANK + i] = S.NN[i];
    }
    chan_save[SCAN_RANK] = S.SCAN;


    sprintf(tag_chan, "CHAN%03d", chan);
    // StorageData.putString(tag_chan, chan_save);
    err = nvs_set_str(PRC152handle, tag_chan, chan_save);
    if(err) {
        log_v("save_ChannelParameter fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));
    D_printf("save:%s\n", tag_chan);
}
void load_ChannelParameter(uint8_t chan, CHAN_ARV_P L) {
    char chan_load[MEM_LENGTH] = {0};
    char tag_chan[8] = "CHAN001";
    memset(L, 0, sizeof(CHAN_ARV));
    sprintf(tag_chan, "CHAN%03d", chan);

    //StorageData.getBytes(tag_chan, chan_load, MEM_LENGTH);
    //StorageData.getString(tag_chan, chan_load, MEM_LENGTH);
    err = nvs_get_str(PRC152handle, tag_chan, chan_load, &MEM_LENGTH);
    if(err) {
        log_e("load_ChannelParameter fail:%s", nvs_error(err));
        return ;
    }

    L->CHAN = (chan_load[CHAN_RANK + 0] - '0') * 100 + (chan_load[CHAN_RANK + 1] - '0') * 10 + (chan_load[CHAN_RANK + 2] - '0');
    if(chan != L->CHAN) {
        return;
    }

    L->RS = (chan_load[RS_RANK + 0] - '0') * 100 + (chan_load[RS_RANK + 1] - '0') * 10 + (chan_load[RS_RANK + 2] - '0');
    L->TS = (chan_load[TS_RANK + 0] - '0') * 100 + (chan_load[TS_RANK + 1] - '0') * 10 + (chan_load[TS_RANK + 2] - '0');
    L->POWER = chan_load[POWER_RANK] - '0';
    L->GBW = chan_load[BW_RANK] - '0';
    L->SCAN = 0;
    L->RX_FREQ = (chan_load[RX_RANK + 0] - '0') * 100 + (chan_load[RX_RANK + 1] - '0') * 10 + (chan_load[RX_RANK + 2] - '0');
    L->RX_FREQ += (chan_load[RX_RANK + 4] - '0') * 0.1 + (chan_load[RX_RANK + 5] - '0') * 0.01 + (chan_load[RX_RANK + 6] - '0') * 0.001 + (chan_load[RX_RANK + 7] - '0') * 0.0001;
    L->TX_FREQ = (chan_load[TX_RANK + 0] - '0') * 100 + (chan_load[TX_RANK + 1] - '0') * 10 + (chan_load[TX_RANK + 2] - '0');
    L->TX_FREQ += (chan_load[TX_RANK + 4] - '0') * 0.1 + (chan_load[TX_RANK + 5] - '0') * 0.01 + (chan_load[TX_RANK + 6] - '0') * 0.001 + (chan_load[TX_RANK + 7] - '0') * 0.0001;

    for(int i = 0; i < 7; i++) {
        L->NN[i] = chan_load[NN_RANK + i];
    }
    //  sprintf((char *)L->NN, "%7s", chan_load+NN_RANK);

    if(L->RS > 121) {
        L->RS = 0;
    }
    if(L->TS > 121) {
        L->TS = 0;
    }
    if(L->POWER > 1) {
        L->POWER = 1;
    }
    if(L->GBW > 1) {
        L->GBW = 1;
    }
    if(L->SCAN > 1) {
        L->SCAN = 0;
    }
    if(L->RX_FREQ < 136.0 || (L->RX_FREQ > 174.0 && L->RX_FREQ < 400.0) || L->RX_FREQ > 480.0) {
        L->RX_FREQ = 435.025;
    }
    if(L->TX_FREQ < 136.0 || (L->TX_FREQ > 174.0 && L->TX_FREQ < 400.0) || L->TX_FREQ > 480.0) {
        L->TX_FREQ = 435.025;
    }

    //  memcpy(L->NN, chan_load+NN_RANK, 7);

    D_printf("Load:  CHAN:%3d, TS:%d, RS:%d, POWER:%d, GBW:%d, SCAN:%d, TX:%f, RX:%f,  NN:%s**\n",
             L->CHAN, L->TS, L->RS, L->POWER, L->GBW, L->SCAN, L->TX_FREQ, L->RX_FREQ, L->NN);
    return;
}
//
void save_ChannelParameterStr(uint8_t chan, char* S) {
    char tag_chan[8] = "CHAN001";
    sprintf(tag_chan, "CHAN%03d", chan);
    err = nvs_set_str(PRC152handle, tag_chan, S);
    if(err) {
        log_v("save_ChannelParameter fail: %s", nvs_error(err));
    }
    ESP_ERROR_CHECK(nvs_commit(PRC152handle));

}
void load_ChannelParameterStr(uint8_t chan, char* L) {
    char tag_chan[8] = "CHAN001";
    sprintf(tag_chan, "CHAN%03d", chan);
    err = nvs_get_str(PRC152handle, tag_chan, L, &MEM_LENGTH);
    if(err) {
        log_e("load_ChannelParameter fail:%s", nvs_error(err));
        return ;
    }
}
//数据初始化
void DATA_Init(void) {
    int i = 0;
    int val = 0xAA;
    val = get_Flag(RESETADDR);
//    Serial.printf("resetflag:%0#x\n", val);

    if(val != RESET_VAL) {
        D_printf("RESETING\n");
        LCD_Clear(GLOBAL32);
        LCD_ShowString1016(24, 0, "RESETING", 1, 128);
        LCD_ShowProcessBar(0, 3, 0);
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
        ESP_ERROR_CHECK(nvs_open(PRC152MARK2, NVS_READWRITE, &PRC152handle));

        save_Step(0);
        save_Sql(1);
        save_AudioSelect(0);
        save_MicLevel(1);
        save_ScramLevel(0);
        save_Tot(0);
        save_Backlightness(40);
        save_ScreenContrast(3);
        save_LampTime(1);
        save_VDO(0);
        save_OverVolume(3);
        save_PreTone(1);
        save_EndTone(1);
        //      Storage_WriteOneByte(FLAG_WFMMOD_ADDR,          0);
        save_FMFreq(880);
        save_CurrentChannel(1);
        set_Flag(FLAG_VU_SWITCH_ADDR, 1);
        set_Flag(FLAG_CF_SWITCH_ADDR, 0);

        FeedDog(); //喂狗

        save_ChanA(1);
        save_ChanB(2);

        LCD_ShowProcessBar(0, 3, 5);

        chan_arv[TMP].RS = 0;
        chan_arv[TMP].TS = 0;
        chan_arv[TMP].GBW = 1;
        chan_arv[TMP].SCAN = 0;
        memset((char*)chan_arv[TMP].NN, 0, 8);
        /////////////////////////////////////////////////////////////////////////////////////////////////
        chan_arv[TMP].POWER = 1;

        chan_arv[TMP].RX_FREQ = 145.5500;
        chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
        sprintf((char*)chan_arv[TMP].NN, "%s", "VHF    ");
        save_ChannelParameter(0, chan_arv[TMP]);

        chan_arv[TMP].RX_FREQ = 435.0250;
        chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
        sprintf((char*)chan_arv[TMP].NN, "%s", "UHF    ");
        save_ChannelParameter(100, chan_arv[TMP]);

        LCD_ShowProcessBar(0, 3, 10); //以上数据占据总数据进度条的10%
////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
        FeedDog(); //喂狗
        chan_arv[TMP].RX_FREQ = 136.0250;
        chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
        memcpy((char*)chan_arv[TMP].NN, "LOW 136", 7);
        chan_arv[TMP].POWER = 1;
        save_ChannelParameter(1, chan_arv[TMP]);
        memcpy((char*)chan_arv[TMP].NN, "HIGH136", 7);
        chan_arv[TMP].POWER = 0;
        save_ChannelParameter(2, chan_arv[TMP]);

        chan_arv[TMP].RX_FREQ = 145.0250;
        chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
        memcpy((char*)chan_arv[TMP].NN, "LOW 145", 7);
        chan_arv[TMP].POWER = 1;
        save_ChannelParameter(3, chan_arv[TMP]);
        memcpy((char*)chan_arv[TMP].NN, "HIGH145", 7);
        chan_arv[TMP].POWER = 0;
        save_ChannelParameter(4, chan_arv[TMP]);

        chan_arv[TMP].RX_FREQ = 148.0250;
        chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
        memcpy((char*)chan_arv[TMP].NN, "LOW 148", 7);
        chan_arv[TMP].POWER = 1;
        save_ChannelParameter(5, chan_arv[TMP]);
        memcpy((char*)chan_arv[TMP].NN, "HIGH148", 7);
        chan_arv[TMP].POWER = 0;
        save_ChannelParameter(6, chan_arv[TMP]);
        FeedDog(); //喂狗
        LCD_ShowProcessBar(0, 3, 10);
        chan_arv[TMP].RX_FREQ = 155.0250;
        chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
        memcpy((char*)chan_arv[TMP].NN, "LOW 155", 7);
        chan_arv[TMP].POWER = 1;
        save_ChannelParameter(7, chan_arv[TMP]);
        memcpy((char*)chan_arv[TMP].NN, "HIGH155", 7);
        chan_arv[TMP].POWER = 0;
        save_ChannelParameter(8, chan_arv[TMP]);

        chan_arv[TMP].RX_FREQ = 165.0250;
        chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
        memcpy((char*)chan_arv[TMP].NN, "LOW 165", 7);
        chan_arv[TMP].POWER = 1;
        save_ChannelParameter(9, chan_arv[TMP]);
        memcpy((char*)chan_arv[TMP].NN, "HIGH165", 7);
        chan_arv[TMP].POWER = 0;
        save_ChannelParameter(10, chan_arv[TMP]);

        chan_arv[TMP].RX_FREQ = 169.0250;
        chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
        memcpy((char*)chan_arv[TMP].NN, "LOW 169", 7);
        chan_arv[TMP].POWER = 1;
        save_ChannelParameter(11, chan_arv[TMP]);
        memcpy((char*)chan_arv[TMP].NN, "HIGH169", 7);
        chan_arv[TMP].POWER = 0;
        save_ChannelParameter(12, chan_arv[TMP]);

        FeedDog(); //喂狗
        LCD_ShowProcessBar(0, 3, 14);

        chan_arv[TMP].RX_FREQ = 400.0250;
        chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
        memcpy((char*)chan_arv[TMP].NN, "LOW 400", 7);
        chan_arv[TMP].POWER = 1;
        save_ChannelParameter(13, chan_arv[TMP]);
        memcpy((char*)chan_arv[TMP].NN, "HIGH400", 7);
        chan_arv[TMP].POWER = 0;
        save_ChannelParameter(14, chan_arv[TMP]);

        chan_arv[TMP].RX_FREQ = 415.0250;
        chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
        memcpy((char*)chan_arv[TMP].NN, "LOW 415", 7);
        chan_arv[TMP].POWER = 1;
        save_ChannelParameter(15, chan_arv[TMP]);
        memcpy((char*)chan_arv[TMP].NN, "HIGH415", 7);
        chan_arv[TMP].POWER = 0;
        save_ChannelParameter(16, chan_arv[TMP]);

        chan_arv[TMP].RX_FREQ = 425.0250;
        chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
        memcpy((char*)chan_arv[TMP].NN, "LOW 425", 7);
        chan_arv[TMP].POWER = 1;
        save_ChannelParameter(17, chan_arv[TMP]);
        memcpy((char*)chan_arv[TMP].NN, "HIGH425", 7);
        chan_arv[TMP].POWER = 0;
        save_ChannelParameter(18, chan_arv[TMP]);
        FeedDog(); //喂狗
        LCD_ShowProcessBar(0, 3, 18);
        chan_arv[TMP].RX_FREQ = 435.0250;
        chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
        memcpy((char*)chan_arv[TMP].NN, "LOW 435", 7);
        chan_arv[TMP].POWER = 1;
        save_ChannelParameter(19, chan_arv[TMP]);
        memcpy((char*)chan_arv[TMP].NN, "HIGH435", 7);
        chan_arv[TMP].POWER = 0;
        save_ChannelParameter(20, chan_arv[TMP]);

        chan_arv[TMP].RX_FREQ = 445.0250;
        chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
        memcpy((char*)chan_arv[TMP].NN, "LOW 445", 7);
        chan_arv[TMP].POWER = 1;
        save_ChannelParameter(21, chan_arv[TMP]);
        memcpy((char*)chan_arv[TMP].NN, "HIGH445", 7);
        chan_arv[TMP].POWER = 0;
        save_ChannelParameter(22, chan_arv[TMP]);

        chan_arv[TMP].RX_FREQ = 455.0250;
        chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
        memcpy((char*)chan_arv[TMP].NN, "LOW 455", 7);
        chan_arv[TMP].POWER = 1;
        save_ChannelParameter(23, chan_arv[TMP]);
        memcpy((char*)chan_arv[TMP].NN, "HIGH455", 7);
        chan_arv[TMP].POWER = 0;
        save_ChannelParameter(24, chan_arv[TMP]);
        FeedDog(); //喂狗
        LCD_ShowProcessBar(0, 3, 24);
        chan_arv[TMP].RX_FREQ = 465.0250;
        chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
        memcpy((char*)chan_arv[TMP].NN, "LOW 465", 7);
        chan_arv[TMP].POWER = 1;
        save_ChannelParameter(25, chan_arv[TMP]);
        memcpy((char*)chan_arv[TMP].NN, "HIGH465", 7);
        chan_arv[TMP].POWER = 0;
        save_ChannelParameter(26, chan_arv[TMP]);

        chan_arv[TMP].RX_FREQ = 469.0250;
        chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
        memcpy((char*)chan_arv[TMP].NN, "LOW 469", 7);
        chan_arv[TMP].POWER = 1;
        save_ChannelParameter(27, chan_arv[TMP]);
        memcpy((char*)chan_arv[TMP].NN, "HIGH469", 7);
        chan_arv[TMP].POWER = 0;
        save_ChannelParameter(28, chan_arv[TMP]);

        LCD_ShowProcessBar(0, 3, 28);

        chan_arv[TMP].POWER = 1;
        chan_arv[TMP].RX_FREQ = 145.5500;
        chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
        memset((char*)chan_arv[TMP].NN, 0, 8);

        for(i = 29; i < 100; i++) {
            FeedDog(); //喂狗
            sprintf((char*)chan_arv[TMP].NN, "CH-%02d  ", i);
            save_ChannelParameter(i, chan_arv[TMP]);
            LCD_ShowProcessBar(0, 3, i);
        }
#endif
#ifndef DEBUG

        chan_arv[TMP].POWER = 0;
        memcpy((char*)chan_arv[TMP].NN, "AMA7500", 7);
        for(int i = 1; i < 21; i++) {
            FeedDog(); //喂狗
            chan_arv[TMP].RX_FREQ = 409.7500 + (0.0125) * (i - 1);
            chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
            //          sprintf((void *)(chan_arv[TMP].NN+3), "%d", (int)((chan_arv[TMP].RX_FREQ*10000-4090000)));
            sprintf((char*)chan_arv[TMP].NN, "CH-%02d  ", i);
            save_ChannelParameter(i, chan_arv[TMP]);
            if(i == 10) {
                LCD_ShowProcessBar(0, 3, 15);
            }
        }
        LCD_ShowProcessBar(0, 3, 20);

        for(int i = 0; i < 7; i++) {
            FeedDog(); //喂狗
            chan_arv[TMP].RX_FREQ = 462.5625 + (0.0250) * i;
            chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
            //          sprintf((void *)chan_arv[TMP].NN, "%d", (int)(chan_arv[TMP].RX_FREQ*10000));
            sprintf((char*)chan_arv[TMP].NN, "FRS-%02d ", i + 1);
            save_ChannelParameter(21 + i, chan_arv[TMP]);
            LCD_ShowProcessBar(0, 3, 21 + i);
        }
        //

        for(int i = 0; i < 7; i++) {
            FeedDog(); //喂狗
            chan_arv[TMP].RX_FREQ = 467.5625 + (0.0250) * i;
            chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
            //          sprintf((void *)chan_arv[TMP].NN, "%d", (int)(chan_arv[TMP].RX_FREQ*10000));
            sprintf((char*)chan_arv[TMP].NN, "FRS-%02d ", i + 8);
            save_ChannelParameter(28 + i, chan_arv[TMP]);
            LCD_ShowProcessBar(0, 3, 28 + i);
        }
        //

        for(int i = 0; i < 8; i++) {
            FeedDog(); //喂狗
            chan_arv[TMP].RX_FREQ = 462.5500 + (0.0250) * i;
            chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
            //          sprintf((void *)chan_arv[TMP].NN, "%d", (int)(chan_arv[TMP].RX_FREQ*10000));
            sprintf((char*)chan_arv[TMP].NN, "FRS-%02d ", i + 15);
            save_ChannelParameter(35 + i, chan_arv[TMP]);
            LCD_ShowProcessBar(0, 3, 34 + i);
        }
        //          sprintf((void *)(chan_arv[TMP].NN+3), "%d", (int)((chan_arv[TMP].RX_FREQ*10000-4620000)));

        chan_arv[TMP].POWER = 1;
        chan_arv[TMP].RX_FREQ = 145.5500;
        chan_arv[TMP].TX_FREQ = chan_arv[TMP].RX_FREQ;
        memset((char*)chan_arv[TMP].NN, 0, 8);

        for(i = 43; i < 100; i++) {
            FeedDog(); //喂狗
            sprintf((char*)chan_arv[TMP].NN, "CH-%02d  ", i);
            save_ChannelParameter(i, chan_arv[TMP]);
            LCD_ShowProcessBar(0, 3, i);
        }
#endif
        set_Flag(RESETADDR, RESET_VAL);
        LCD_ShowProcessBar(0, 3, 100);
        delay_ms(1500);
        LCD_ShowPICALL(pic_HARRIS);
    }
}
//
