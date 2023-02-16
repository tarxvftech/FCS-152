#include "view.h"
#include <assert.h>
int _menustackstorage[maxmenudepth] = {0};
int_q_t menustack;

void VFO_Refresh();
void base_draw(menu * parent, menu * m){
    if( int_q_empty(&menustack) ){
        m->current = 0;
    }
    VFO_Refresh();
}
void base_input_handler(menu * parent,menu * m){
    //variadic argument function to jump to specific menus for things like zeroize without having to using ints directly, but finding by string name
    if( ! inp_q_empty(&input_q) ){
        input_et in = inp_q_get(&input_q);
        if( in.key == '\n' ){
            m->current = child_idx_by_name(m, "MainMenu");
            assert(m->current >= 0);
            int_q_rpush(&menustack, m->current);
        }
        if( in.key == KEY_PLUS ){
            //vfoA += .0125;
        }
        if( in.key == KEY_MINUS ){
            //vfoA -= .0125;
        }
        if( in.key == '8' ){
            m->current = child_idx_by_name(m, "PGM");
            assert(m->current >= 0);
            int_q_rpush(&menustack, m->current);
        }
        if( in.key == '2' ){
            m->current = child_idx_by_name(m, "LT");
            assert(m->current >= 0);
            int_q_rpush(&menustack, m->current);
        }
        if( in.key == '5' ){
            m->current = child_idx_by_name(m, "Zeroize");
            assert(m->current >= 0);
            int_q_rpush(&menustack, m->current);
        }
        if( in.key == '3' ){
            m->current = child_idx_by_name(m, "FM");
            assert(m->current >= 0);
            int_q_rpush(&menustack, m->current);
        }
        if( in.key == '7' ){
            m->current = child_idx_by_name(m, "OPT");
            assert(m->current >= 0);
            int_q_rpush(&menustack, m->current);
        }
        if( in.key == KEY_CLR && m->current != 0 ){ 
            assert( int_q_empty(&menustack) );
            m->current = 0;
        }
    }
}
void menu_simple_draw(menu * parent, menu * m){
    //int row, col;
    //getmaxyx(stdscr,row,col); //not a function, but a macro
    //wmove(rightwin,1,1);
    //wprintw(rightwin, "current: %d\n", m->current);
    if( m->numchildren > 0 && m->children != NULL ){
        for( int i = 0; i < m->numchildren; i++ ){
            //wmove(display,1+i,1);
            //wprintw(display,"%s", m->current == i? ">":" ");
            //wprintw(display, "%d %s",i, m->children[i]->name);
        }
        //wprintw(display,"\n");
    }
}
void menu_viewonly_input(menu * parent,menu * m){
    if( ! inp_q_empty(&input_q) ){
        input_et in = inp_q_get(&input_q);
        if( in.key == KEY_CLR ){
            int_q_rpop(&menustack);
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
	if( in.key == '\n' ){
	    int_q_rpush(&menustack, m->current);
	}
	if( in.key == KEY_CLR ){
	    int_q_rpop(&menustack);
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
void menu_unimplemented_draw(menu * parent, menu * m){
        //wmove(display, 1, 1);
        //wprintw(display,"Unimplemented Menu Type");
}

menu mainmenu = {
    .name = "MainMenu",
    .type = VIEW_SIMPLEMENU,
    //.children = main_entries,
    .numchildren = 0,
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
	.numchildren = 1,
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
    //if( i != int_q_size(&menustack) ){
        //printf("ERR:Did not consume all of menustack\n");
    //}

    //render the selected menu structure
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
        menu_unimplemented_draw(parent,m);
    }
}

void view_init(){
    int_q_init(&menustack, _menustackstorage, 15); 
}
