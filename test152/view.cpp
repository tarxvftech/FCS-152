#include "main.h"
#include "view.h"
#include <assert.h>
int _menustackstorage[maxmenudepth] = {0};
int_q_t menustack;

void draw_clear(){
    LCD_Clear(GLOBAL32);
}
extern u8 VOLUME;
bool must_clear_screen = false;

void menu_in(int entry){
    int_q_rpush(&menustack, entry);
    must_clear_screen = true;
}
void menu_out(){
    int_q_rpop(&menustack);
    must_clear_screen = true;
}
void submenu_by_name(menu * m, char * name){
    m->current = child_idx_by_name(m, name);
    assert(m->current >= 0);
    menu_in( m->current);
}
void draw_VFO(){
    LCD_ShowString0408(0, 0, "R BAT", 1);
    LCD_ShowBattery(Get_Battery_Vol());
    LCD_ShowString0408(48, 0, "VULOS MOI", 1);
    LCD_ShowString0408(88, 0, "------- PT", 1);
    LCD_ShowVolume(VOLUME);

    LCD_ShowString0608(0, 1, "1                     ", 1, 128);
    LCD_ShowString0608(0, 2, "2                     ", 1, 128);
    LCD_ShowString0608(0, 3, "3                     ", 1, 128);
}
void base_draw(menu * parent, menu * m){
    if( int_q_empty(&menustack) ){
        m->current = 0;
    }
    draw_VFO();
}
void base_input_handler(menu * parent,menu * m){
    //variadic argument function to jump to specific menus for things like zeroize without having to using ints directly, but finding by string name
    if( ! inp_q_empty(&input_q) ){
        input_et in = inp_q_get(&input_q);
        if( in.key == KEY_ENT ){
            submenu_by_name(m, "MainMenu");
        }
        if( in.key == KEY_PLUS ){
            //vfoA += .0125;
        }
        if( in.key == KEY_MINUS ){
            //vfoA -= .0125;
        }
        if( in.key == KEY_8 ){
            submenu_by_name(m, "PGM");
        }
        if( in.key == KEY_2 ){
            submenu_by_name(m, "LT");
        }
        if( in.key == KEY_5 ){
            submenu_by_name(m, "Zeroize");
        }
        if( in.key == KEY_3 ){
            submenu_by_name(m, "FM");
        }
        if( in.key == KEY_7 ){
            submenu_by_name(m, "OPT");
        }
        if( in.key == KEY_CLR && m->current != 0 ){ 
            assert( int_q_empty(&menustack) );
            m->current = 0;
        }
    }
}
void menu_simple_draw(menu * parent, menu * m){
    D_printf("simplemenu draw\n");
    if( m->numchildren > 0 && m->children != NULL ){
        for( int i = 0; i < m->numchildren ; i++ ){ 
            //TODO scrolling
            if( i == m->current ){
                LCD_ShowString0608(0, i, ">", 1, 128);
            } else {
                LCD_ShowString0608(0, i, " ", 1, 128);
            }
            LCD_ShowString0608(6, i, m->children[i]->name, 1, 128);
        }
    }
}
void menu_viewonly_input(menu * parent,menu * m){
    if( ! inp_q_empty(&input_q) ){
        input_et in = inp_q_get(&input_q);
        if( in.key == KEY_CLR ){
            menu_out();
        }
    }
}
void menu_simple_input(menu * parent,menu * m){
    if( ! inp_q_empty(&input_q) ){
	input_et in = inp_q_get(&input_q);
	if( in.key == KEY_PLUS ){
	    m->current = m->current > 0 ? (m->current - 1) %m->numchildren: 0;
	}
	if( in.key == KEY_MINUS ){
	    m->current = m->current < m->numchildren -1 ? (m->current + 1) %m->numchildren: m->numchildren -1;
	}
	if( in.key == KEY_ENT ){
	    menu_in( m->current);
	}
	if( in.key == KEY_CLR ){
            menu_out();
	}
	if( in.key >= KEY_0 && in.key <= KEY_9 ){
	    int idx= in.key - KEY_0;
	    if( idx >= 0 && idx < m->numchildren ){
		m->current = idx;
		menu_in( m->current);
	    }
	}
    }
}
void menu_unimplemented_draw(menu * parent, menu * m){
    D_printf("unimplemented draw\n");
    LCD_ShowString0408(0, 0, "unimplemented", 1);
}
char * view_enckeydetail_fieldlist[] = { 
    "keylength",
    "name",
    "key",
};
char * view_channeldetail_fieldlist[] = { //order of fields displayed in draw_channeldetail and edited in menu_detailedit_input
    "name",
    "power",
    "txf",
    "rxf",
    "encode",
    "decode",
};
menu channeldetail = {
    .name = "ChannelDetail",
    .type = VIEW_CUSTOM,
    //.draw = draw_channeldetail,
    //.handle_input = menu_detailedit_input,
    //.numfields = 6,
    //.fields = view_channeldetail_fieldlist,
};
menu * channeldetailentry[] = {
    &channeldetail
};
menu channelsmenu = {
    .name = "Channels",
    .type = VIEW_LISTING,
    //.current = 0,
    //.object = &channellist,
    //.draw = draw_channellist,
    //.handle_input = channellist_input,
    //needs a 'detail' view somehow, .children[0].submenu points to it
    //.children = channeldetailentry,
    //.numchildren = 1,
    //.numchildren = 1
};

menu enckeydetail = {
    .name = "EncKeyDetail",
    .type = VIEW_CUSTOM,
    //.draw = draw_enckeydetail,
    //.handle_input = menu_detailedit_input,
    //.numfields = 3,
    //.fields = view_enckeydetail_fieldlist,
    /*.children = channelsettings,*/
    /*.current = 0,*/
    /*.numchildren = 3*/
};
menu * enckeydetailentry[] = {
    &enckeydetail
};
menu encryptionsettingspage = {
    .name = "Encryption Keys",
    .type = VIEW_LISTING,
    //.current = 0,
    //.object = &ezkeyslist,
    //.draw = draw_encryptionkeyslist,
    //.handle_input = encryptionkeyslist_input,
    //.children = enckeydetailentry,
    //.numchildrenslots = 1,
    //.numchildren = 1
};
menu * topsecuritysettingskids = {
    &encryptionsettingspage
};
menu topsecuritysettings = {
    .name = "Security",
    .type = VIEW_SIMPLEMENU,
    .children = &topsecuritysettingskids,
    .numchildren = 1,
    .numchildrenslots = 1,
};
menu topwaveformsettings = {
    .name = "Waveform",
    .type = VIEW_SIMPLEMENU,
    .numchildren = 0
};
menu topchannelsettings = {
    .name = "Channel",
    .type = VIEW_SIMPLEMENU,
    .numchildren = 0
};
menu toprfnetworksettings = {
    .name = "RFNetwork",
    .type = VIEW_SIMPLEMENU,
    .numchildren = 0
};

char * wifinetworkfields[] = {
    "enabled",
    "connectssid",
    "connectpsk"
    "sharessid",
    "sharepsk"
};

menu topwifinetworksettings = {
    .name = "802.11Network",
    .type = VIEW_CUSTOM,
    //.numchildren = 0,
    //.draw = draw_80211detail,
    //.handle_input = menu_detailedit_input,
    //.numfields = 4,
    //.fields = view_enckeydetail_fieldlist,
};
menu topadminsettings = {
    .name = "Admin",
    .type = VIEW_SIMPLEMENU,
    .numchildren = 0
};
menu toptransfersettings = {
    .name = "Transfer",
    .type = VIEW_SIMPLEMENU,
    .numchildren = 0
};
menu topemergencysettings = {
    .name = "Emergency",
    .type = VIEW_SIMPLEMENU,
    .numchildren = 0
};
menu * settings_top_entries[] = {
    &topsecuritysettings ,
    &topwifinetworksettings ,
    //&topwaveformsettings ,
    //&topchannelsettings ,
    //&toprfnetworksettings ,
    //&topadminsettings ,
    //&toptransfersettings ,
    //&topemergencysettings,
};

menu settings = {
    .name = "Settings",
    .type = VIEW_SIMPLEMENU,
    .children = settings_top_entries,
    .numchildren = 2,
    .numchildrenslots = 2,
};
menu * main_entries[] = {
    &channelsmenu,
    &settings,
};

menu mainmenu = {
    .name = "MainMenu",
    .type = VIEW_SIMPLEMENU,
    .children = main_entries,
    .numchildren = 2,
    .numchildrenslots = 2,
};
menu mainPGM = {
    .name = "PGM",
    .type = VIEW_SIMPLEMENU,
};
menu mainOPT = {
    .name = "OPT",
    .type = VIEW_SIMPLEMENU,
};
menu mainLT = {
    .name = "LT",
    .type = VIEW_SIMPLEMENU,
};
menu mainFM = {
    .name = "FM",
    .type = VIEW_SIMPLEMENU,
};
menu mainZERO = {
    .name = "Zeroize",
    .type = VIEW_SIMPLEMENU,
};
menu * root_entries[] = {
    &mainmenu,
    &mainPGM,
    &mainOPT,
    &mainLT,
    &mainFM,
    &mainZERO
};

menu root = {   
	.name = "Root UI",     
	.type = VIEW_CUSTOM,     
	.draw = base_draw,     
	.handle_input = base_input_handler,     
	.children = root_entries,     
	.numchildren = 6,
	.numchildrenslots = 6,     
	.current = 0,     
};
void ui_draw(menu * m){
    menu * parent = NULL;

    //figure out which menu structure to render
    //convert this into a get_menu_by_idx or something that takes the stack and an index and converts it into a menu *
    //so you can get the parent menu and the current menu with two function calls
    //(and that way more deeply nested things can be fixed in one function
    int i = 0;
    for( ; i < int_q_size(&menustack); i++ ){
        /*int selection = int_q_idx_l(&menustack, i);*/
        if( m->type != VIEW_LISTING && m->children == NULL ){
            break;
        }
        menu * s;
        if( m->type == VIEW_LISTING && m->numchildren > 0 ){
            s = m->children[ 0 ];
            //m->current points to the entry
            //so the next view is hardcoded to 0 for now?
            //gross.
        } else {
            s = m->children[ m->current ];
        }
        if( s == NULL ){
            break;
        } 
        parent = m;
        m = s;
    }
    if( i != int_q_size(&menustack) ){
        D_printf("ERR:Did not consume all of menustack\n");
    }
    D_printf("Menu: %d, %s\n", i, m->name);
    if( must_clear_screen ){
        draw_clear();
        must_clear_screen = false;
    }

    //render the selected menu structure
    assert(m!=NULL);
    if( m->type == VIEW_CUSTOM ){
        m->handle_input(parent,m);
        m->draw(parent,m);
    } else if ( m->type == VIEW_LISTING){
        m->handle_input(parent,m);
        m->draw(parent,m);
    } else if (m->type == VIEW_SIMPLEMENU ){
        menu_simple_input(parent,m);
        menu_simple_draw(parent,m);
    } else {
        assert(parent!=NULL);
        menu_unimplemented_draw(parent,m);
    }
}

void view_init(){
    int_q_init(&menustack, _menustackstorage, 15); 
}
