#include "main.h"
#include "view.h"
#include <assert.h>
#include <AutoConnect.h>
#include <WiFi.h>
#include "q_input.h"
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

extern AutoConnect portal;
int _menustackstorage[maxmenudepth] = {0};
int_q_t menustack;

void draw_clear(){
    LCD_Clear(GLOBAL32);
}
extern u8 VOLUME;
bool must_clear_screen = true;

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
    //assert(m->current >= 0); //assert just crashes on the radio.
    if( m->current >= 0 ){
        menu_in( m->current);
    } else {
        D_printf("Could not find %s in %s children\n", name, m->name);
    }
}
void draw_VFO(){
    LCD_ShowString0408(0, 0, "R BAT", 1);
    
    //I want a voltage and estimated battery life at some point
    LCD_ShowBattery(Get_Battery_Vol());
    //LCD_ShowString0408(48, 0, "VULOS MOI", 1);
    if( portal.isPortalAvailable() ){
        int numclients = WiFi.softAPgetStationNum();
        if( numclients ){
            //i don't know if this part works properly yet. Haven't tested.
            if( numclients > 9 ){
                LCD_ShowString0408(48, 0, ">", 1);
            } else {
                char t[2] = {numclients+'0', 0};
                LCD_ShowString0408(48, 0, t, 1);
            }
        } else {
            LCD_ShowString0408(48, 0, "P", 1);
        }
    } else {
        LCD_ShowString0408(48, 0, "-", 1);
    }
    if( WiFi.status() == WL_CONNECTED ){
        LCD_ShowString0408(52, 0, "W", 1);
    } else {
        LCD_ShowString0408(52, 0, "D", 1);
    }
    //LCD_ShowString0408(48, 0, "VULOS MOI", 1); //don't know what MOI means. Or VULOS for that matter, really.
    LCD_ShowString0408(88, 0, "------- PT", 1); //it's always PT=plaintext because we don't have encryption (sadface)
    LCD_ShowVolume(VOLUME);

    if (chan_arv[NOW].CHAN == 100) {
        LCD_ShowString0608(66, 1, "UHF ", 1, 90);
    } else if (chan_arv[NOW].CHAN == 0) {
        LCD_ShowString0608(66, 1, "VHF ", 1, 90);
    } else {
        LCD_ShowString0608(66, 1, "CHAN", 1, 90);
    }

    //Channel number display
    LCD_ShowChan(83, 2, chan_arv[NOW].CHAN, 1);

    LCD_ShowFreq(0, 1, chan_arv[NOW].RX_FREQ, 1);       //Display reception frequency

    //Send and receive different frequency signs
    if (chan_arv[NOW].RX_FREQ != chan_arv[NOW].TX_FREQ) {
        LCD_ShowPIC0808(92, 1, 0);
    } else {
        LCD_ShowString0408(92, 1, "  ", 1);
    }
    //LCD_ShowString0608(0, 1, "1                     ", 1, 128);
    //LCD_ShowString0608(0, 2, "2                     ", 1, 128);
    //LCD_ShowString0608(0, 3, "3                     ", 1, 128);
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
        D_printf("base_input_handler in.key = %d\n", in.key);
        if( in.key == KEY_ENT ){
            submenu_by_name(m, "MainMenu");
        }
        if( in.key == KEY_PLUS ){
            int channel = (chan_arv[NOW].CHAN + 1)%100;
            load_ChannelParameter(channel, &chan_arv[NOW]);
        }
        if( in.key == KEY_MINUS ){
            int channel = (chan_arv[NOW].CHAN - 1)%100;
            load_ChannelParameter(channel, &chan_arv[NOW]);
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
        //scrolling support:
        //i is the output line
        //i+offset is the index of the thing we're printing
        //
        //m->current is the currently 'hovered' j, about to be selected, 
        //and is what controls our scrolling
        //we have to know where to start j, based on m->current and m->numchildren
        int offset = MAX(0,MIN(MAX(0, m->current-2),m->numchildren-4));
        int i = 0;
        int numlines = 4;
        while( i < numlines && i < m->numchildren ){
            if( i+offset == m->current ){ //selector indicator
                LCD_ShowString0608(0, i, ">", 1, 128);
            } else {
                LCD_ShowString0608(0, i, " ", 1, 128);
            }
            LCD_ShowString0608(6, i, "               ", 1, 128); //Clear the line so as we scroll it doesn't get dirtied up
            LCD_ShowString0608(6, i, m->children[i+offset]->name, 1, 128);
            i++;
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
        D_printf("menu_simple_input in.key = %d\n", in.key);
	if( in.key == KEY_PLUS ){
	    m->current = m->current > 0 ? (m->current - 1) %m->numchildren: 0;
	}
	if( in.key == KEY_MINUS ){
	    m->current = m->current < m->numchildren -1 ? (m->current + 1) %m->numchildren: m->numchildren -1;
	}
	if( in.key == KEY_ENT ){
	    menu_in( m->current );
	}
	if( in.key == KEY_CLR ){
            menu_out();
	}
	if( in.key >= KEY_0 && in.key <= KEY_9 ){
	    int idx= in.key - KEY_0 -1; //1 should select first entry in menu
            if( idx == -1 ){
                idx = 10; //and zero should select tenth
            }
            //no multidigit entries - anything bigger should use a filtering or tagging view of some kind, like for a channel list
	    if( idx >= 0 && idx < m->numchildren ){
		m->current = idx; 
		menu_in( m->current);
	    }
	}
    }
}
void menu_unimplemented_draw(menu * parent, menu * m){
    D_printf("unimplemented draw\n");
    LCD_ShowString0608(0, 1, "unimplemented", 1, 128);
}
bool safe2set( void * ptr ){
    safe2set_t * p = (safe2set_t*) ptr;
    return ptr != NULL && p->sanity == SAFE2SET && p->type != EDIT_NO_TYPE_SELECTED; 
    // && p->type [is one of the known values], or should that be a separate test?
}
update_res updatefield( safe2set_t * ptr, int recordindex, char * fieldname, char * newvalstr );

int record_get_fieldidx_by_name(const struct_desc * desc, const char * name){
    int numfields = desc->count;
    for( int i = 0; i < numfields; i++ ){
        if( strcmp(name, desc->fields[i].name) == 0 ){
            return i;
        }
    }
    return -1;
}


#define QWERTY 1 
//the t9 integration is a very rough draft and doesn't show you the typed input in real time, but it does work!
update_res updatefieldvalue( void * ptr, editabletype type, char * newvalstr ){
    assert(ptr != NULL);
    if( type == EDIT_CHAR8 ){
        memset(ptr, 0, 8);
        if( QWERTY ){
            strncpy( (char *) ptr, newvalstr, 8);
        } else {
            convert_t9simple(newvalstr, (char *) ptr, 8);
        }
        ((char *)ptr)[7] = 0;
        return UPDATE_NO_ERROR;
    }
    if( type == EDIT_CHAR16 ){
        memset(ptr, 0, 16);
        if( QWERTY ){
            strncpy( (char *) ptr, newvalstr, 16);
        } else {
            convert_t9simple(newvalstr, (char *)ptr, 16);
        }
        ((char *)ptr)[15] = 0;
        return UPDATE_NO_ERROR;
    }
    if( type == EDIT_CHAR32 ){
        memset(ptr, 0, 32);
        if( QWERTY ){
            strncpy( (char *) ptr, newvalstr, 32);
        } else {
            convert_t9simple(newvalstr, (char *)ptr, 32);
        }
        ((char *)ptr)[31] = 0;
        return UPDATE_NO_ERROR;
    }
    if( type == EDIT_FLOAT ){
        float * f = (float *) ptr;
        char * endof= 0;
        *f = strtof( newvalstr, &endof);
        if( endof!= newvalstr ){ //means we read _something_
            return UPDATE_NO_ERROR;
        } else {
            return UPDATE_FAILURE;
        }
    }
    if( type == EDIT_DOUBLE ){
        double * f = (double *) ptr;
        char * endof= 0;
        *f = strtod( newvalstr, &endof);
        if( endof!= newvalstr ){ //means we read _something_
            return UPDATE_NO_ERROR;
        } else {
            return UPDATE_FAILURE;
        }
    }
    if( type == EDIT_INT ){
        int * f = (int *) ptr;
        char * endof= 0;
        *f = strtol( newvalstr, &endof, 10); //10 means decimal base
        if( endof!= newvalstr ){ //means we read _something_
            return UPDATE_NO_ERROR;
        } else {
            return UPDATE_FAILURE;
        }
    }
    return UPDATE_FAILBADTYPE;
}
update_res updatefield( safe2set_t * ptr, int recordindex, char * fieldname, char * newvalstr ){
    if( ! safe2set(ptr) ){
        return UPDATE_FAILBADPTR;
    }
    if( ptr->type == EDIT_CHANNELLIST_T ){
        channellist_t * cl = (channellist_t *) ptr;
        channel_t * c = &(cl->channels[recordindex]);
        assert( recordindex < cl->count );
        int fieldindex = record_get_fieldidx_by_name(&channel_t_desc, fieldname);
        assert(fieldindex != -1 );
        struct_field_desc desc = channel_t_desc.fields[fieldindex];
        updatefieldvalue( ((void *)c) + desc.offset, desc.type, newvalstr);
        //note: need the void cast on c to make sure the offset calculation is in bytes and not sizeof(channel_t).
    }
    if( ptr->type == EDIT_ENCRYPTKEYLIST ){
        //encryptkeylist * l = (encryptkeylist *) ptr;
        //assert( recordindex < l->count );
        //encrypt_key * o = &(l->keys[recordindex]);
        //int fieldindex = record_get_fieldidx_by_name(&encryptkey_desc, fieldname);
        //assert(fieldindex != -1 );
        //struct_field_desc desc = encryptkey_desc.fields[fieldindex];
        //updatefieldvalue( ((void *)o) + desc.offset, desc.type, newvalstr);
        //note: need the void cast on c to make sure the offset calculation is in bytes and not sizeof(channel_t).
    }
    return UPDATE_FAILURE;
}
void menu_detailedit_input(menu * parent, menu * m){
    int numoptions = m->numfields;
    //wprintw(rightwin, " editidx: %d\n", m->editidx);
    if( ! inp_q_empty(&input_q) ){
        input_et in = inp_q_get(&input_q);
        if( m->mode == MODE_NORMAL && in.key == '\n' ){
            m->mode = MODE_EDIT;
        } else if( m->mode == MODE_EDIT && in.key == '\n' ){
            m->editidx = 0;
            memset(m->editbuf, 0, 32);
            m->mode = MODE_EDITFIELD;
        } else if( m->mode == MODE_EDITFIELD ){
            assert(m->editidx >= 0);
            if( in.key == '\n' ){
                //parse and save editbuf here
                if( parent->type == VIEW_LISTING ){
                    assert(parent->object != NULL);
                    assert(m->fields != NULL);
                    updatefield((safe2set_t*)parent->object, parent->current, m->fields[m->current], m->editbuf);
                } else {
                    assert(m->object != NULL);
                    assert(m->fields != NULL);
                    updatefield((safe2set_t*)m->object, parent->current, m->fields[m->current], m->editbuf);
                }
                m->mode = MODE_NORMAL;
            }
            if( in.key == KEY_LEFT ){
                if( m->editidx > 0 ){
                    m->editidx--;
                }
            }
            if( in.key == KEY_RIGHT ){
                if( m->editidx < 31 ){ //preserve the last 0
                    if( m->editbuf[ m->editidx ] == 0 ){
                        m->editbuf[ m->editidx ] = ' ';
                    }
                    m->editidx++;
                }
            }
            if( in.key == KEY_MINUS ){
                if( m->editidx <= 0 ){
                    m->editidx = 0;
                    m->editbuf[ m->editidx ] = 0;
                } else {
                    if( m->editidx < 32 &&  m->editbuf[m->editidx] == 0 ){
                        m->editbuf[ --m->editidx ] = 0;
                    } else {
                        m->editbuf[ --m->editidx ] = ' ';
                    }
                }
            }
            if( m->editidx < 31 && in.key >= ' ' && in.key <= '~' ) {
                m->editbuf[ m->editidx++ ] = in.key;
            }
        }//end EDITFIELD
        if( in.key == KEY_CLR ){ //escape/KEY_CLR
            if( m->mode != MODE_NORMAL ){
                m->mode = MODE_NORMAL;
            } else {
                menu_out();
            }
        }
        if( in.key == KEY_PLUS ){
            m->current = m->current > 0 ? (m->current - 1) % numoptions: 0;
        }
        if( in.key == KEY_MINUS ){
            m->current = m->current < numoptions -1 ? (m->current + 1) %numoptions: numoptions -1;
        }
    }
}
void draw_field(menu * m, editabletype rtype, void * ptr, char * fieldname, char * fmt){
    int fieldindex = -1;
    struct_field_desc desc;
    editabletype type;
    if( rtype == EDIT_CHANNEL_T ){
        fieldindex = record_get_fieldidx_by_name(&channel_t_desc, fieldname);
        assert(fieldindex != -1 );
        desc = channel_t_desc.fields[fieldindex];
        type = desc.type;
    } else if( rtype == EDIT_ENCRYPTKEY ){
        //fieldindex = record_get_fieldidx_by_name(&encryptkey_desc, fieldname);
        //assert(fieldindex != -1 );
        //desc = encryptkey_desc.fields[fieldindex];
        //type = desc.type;
    } else {
        return;
    }
    //updatefieldvalue( ((void *)value) + desc.offset, desc.type, newvalstr);

    void * p = ptr + desc.offset;
    //wprintw(display," %s: ", fieldname);
    if( m->mode == MODE_EDITFIELD && strcmp(m->fields[m->current], desc.name) == 0){
        int namelength = strlen(desc.name);
        //wprintw(rightwin," editidx: %d\n", m->editidx); 
        //wprintw(rightwin," nameoffset: %d\n", namelength+3+m->editidx); //3 for the spaces and ':' character
        int y1 = 0; 
        int x1 = 0;
        //getyx(display,y1,x1);
        //wprintw(display,"%s ", m->editbuf); //the space after %s is _not_ an accident. It shows the highlighted editing cursor.
        int y2 = 0; 
        int x2 = 0;
        //getyx(display,y2,x2);
        //wmove(display,y1,1+3+namelength-1+m->editidx);
        //wchgat(display,1,A_REVERSE,0,NULL);
        //wmove(display,y2,x2);
    } else {
        if( type == EDIT_CHAR8 || type == EDIT_CHAR16 || type == EDIT_CHAR32 ){
            //wprintw(display,"%s", (char*)p);
        } else if( type == EDIT_FLOAT ){
            if( fmt != NULL ){
                //wprintw(display,fmt, *(float*) p);
            } else {
                //wprintw(display,"%03.2f", *(float*) p);
            }
        } else if( type == EDIT_DOUBLE ){
            if( fmt != NULL ){
                //wprintw(display,fmt, *(double*) p);
            } else {
                //wprintw(display,"%03.2f", *(double*) p);
            }
        } else if( type == EDIT_INT ){
            //wprintw(display,"%d", *(int*) p);
        } else {
            //wprintw(display,"(display unsupported: type %d)", type);
        }
    }
    //wprintw(display,"\n");
}
void draw_channeldetail(menu * parent, menu * m){//TODO
    int i = parent->current;
    assert(parent->object != NULL);
    channellist_t * chl = (channellist_t *) parent->object;
    //make sure this matches the order specified in m->fields or there will be problems.
    draw_field(m, EDIT_CHANNEL_T, (void*)&chl->channels[i], "name", NULL);
    draw_field(m, EDIT_CHANNEL_T, (void*)&chl->channels[i], "power", "%03.2f W");
    draw_field(m, EDIT_CHANNEL_T, (void*)&chl->channels[i], "txf", "%03.4f MHz");
    draw_field(m, EDIT_CHANNEL_T, (void*)&chl->channels[i], "rxf", "%03.4f MHz");
    draw_field(m, EDIT_CHANNEL_T, (void*)&chl->channels[i], "encode", NULL);
    draw_field(m, EDIT_CHANNEL_T, (void*)&chl->channels[i], "decode", NULL);
    if( m->mode == MODE_EDIT ){
        //wmove(display,m->current + 1,1);
        //wchgat(display,-1,A_REVERSE,0,NULL);
    }

}
void draw_channellist(menu * parent, menu * m){//TODO
    assert(m->object != NULL);
    channellist_t * chl = (channellist_t *) m->object;
    D_printf("Channellist:\n");
    for( int i = 0; i < chl->count; i++ ){
        D_printf("Channel %d: %s\n", i, chl->channels[i].name);
        if( i == m->current ){
            LCD_ShowString0608(0, i, ">", 1, 128);
        } else {
            LCD_ShowString0608(0, i, " ", 1, 128);
        }
        LCD_ShowString0608(6, i, chl->channels[i].name, 1, 128);
    }
}
void channellist_input(menu * parent,menu * m){//TODO
    //very similar to any menu, but accounts for how numchildren isn't known ahead of time
    assert(m->object != NULL);
    channellist_t * chl = (channellist_t *) m->object;
    m->numchildren = chl->count;
    if( ! inp_q_empty(&input_q) ){
        input_et in = inp_q_get(&input_q);
        if( in.key == KEY_PLUS ){
            m->current = m->current > 0 ? (m->current - 1) %m->numchildren: 0;
        }
        if( in.key == KEY_MINUS ){
            m->current = m->current < m->numchildren -1 ? (m->current + 1) %m->numchildren: m->numchildren -1;
        }
        if( in.key == '\n' ){
            int_q_rpush(&menustack, m->current);
        }
        if( in.key == KEY_CLR ){
            menu_out();
        }
        if( in.key >= '0' && in.key <= '9' ){
            int idx= in.key - '0';
            if( idx >= 0 && idx < m->numchildren ){
                m->current = idx;
                int_q_rpush(&menustack, m->current);
            }
        }
    }
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
    .draw = draw_channeldetail,
    .handle_input = menu_detailedit_input,
    .fields = view_channeldetail_fieldlist,
    .numfields = 6,
};
menu * channeldetailentry[] = {
    &channeldetail
};
channel_t channels[4];
channellist_t channellist = {SAFE2SET, EDIT_CHANNELLIST_T, channels, 4};
menu channelsmenu = {
    .name = "Channel List",
    .type = VIEW_LISTING,
    .draw = draw_channellist,
    .handle_input = channellist_input,
    //needs a 'detail' view somehow, so .children[0].submenu points to it
    .object = &channellist,
    .children = channeldetailentry,
    .numchildren = 1,
    .numchildrenslots = 1,
    .current = 0,
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
menu menu_modeselect = {
    .name = "Mode Select",
    .type = VIEW_SIMPLEMENU,
    .numchildren = 0
};
menu menu_info = {
    .name = "Info",
    .type = VIEW_SIMPLEMENU,
    .numchildren = 0
};
menu menu_debug = {
    .name = "Debug",
    .type = VIEW_SIMPLEMENU,
    .numchildren = 0
};
menu * main_entries[] = {
    &channelsmenu,
    &settings,
    &menu_modeselect,
    &menu_info,
    &menu_debug,
};

menu mainmenu = {
    .name = "MainMenu",
    .type = VIEW_SIMPLEMENU,
    .children = main_entries,
    .numchildren = 5,
    .numchildrenslots = 5,
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
    //&mainPGM,
    //&mainOPT,
    //&mainLT,
    //&mainFM,
    //&mainZERO
};

menu root = {   
	.name = "Root UI",     
	.type = VIEW_CUSTOM,     
	.draw = base_draw,     
	.handle_input = base_input_handler,     
	.children = root_entries,     
	.numchildren = 1,
	.numchildrenslots = 1,      //6
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
        D_printf("m=%s\n", m->name);
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
    if( m->type == VIEW_CUSTOM && m->draw != NULL && m->handle_input != NULL){
        m->handle_input(parent,m);
        m->draw(parent,m);
    } else if ( m->type == VIEW_LISTING && m->draw != NULL && m->handle_input != NULL){
        m->handle_input(parent,m);
        m->draw(parent,m);
    } else if (m->type == VIEW_SIMPLEMENU && m->numchildren > 0){
        menu_simple_input(parent,m);
        menu_simple_draw(parent,m);
    } else {
        D_printf("unhandled menu draw");
        assert(parent!=NULL);
        menu_simple_input(parent,m);
        menu_unimplemented_draw(parent,m);
    }
}

void view_init(){
    int_q_init(&menustack, _menustackstorage, 15); 
}
