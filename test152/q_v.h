#ifndef MM_MENU_H
#define MM_MENU_H
#include <stddef.h>
#include <stdbool.h>
#ifdef __cplusplus      
extern "C" {  
#endif  

typedef enum menutype {
    VIEW_SIMPLEMENU = 0,
    VIEW_LISTING,
    VIEW_CUSTOM,
} viewtype;
extern char * viewtypestr[];//index into this array to get descriptive strings for the above enum



typedef enum eViewMode {
    MODE_NORMAL = 0,
    MODE_EDIT,
    MODE_EDITFIELD,
} qViewMode;
extern char * qViewModeStr[]; //index into here with the enum value to get corresponding descriptive string 

struct menu;
typedef void (*draw_fn)( struct menu * parent, struct menu * m); //draw and input handling functions still being worked on
typedef void (*handle_fn)(struct menu * parent, struct menu * m);
typedef struct menu {
    char * name; //required for all types
    viewtype type; //required 

    draw_fn draw; //unused if SIMPLEMENU
    handle_fn handle_input; //unused if SIMPLEMENU

    void * object; //only used for VIEW_LISTING

    qViewMode mode;
    char ** fields; //list of strings with the field names in the correct order in which to view/edit fields. 
    int numfields; //count of how many strings are in fields 
    int editidx; //which field is currently being edited
    char editbuf[32];  //temporary buffer for string contents of field being edited. Doesn't really have to be here.
    
    struct menu * * children; //an array of pointers to menus. if VIEW_LISTING then there's some special behavior (0th item is the detail view).
    int numchildren; //how many children are in the array
    int numchildrenslots; //total number of slots in the children array, both used and unused
    bool freeableslots; //is it safe to free the children pointer? 
    int current; //which child is currently selected and/or highlighted
} menu;
int child_idx_by_name(menu * m, char * name); 
void printallmenu(menu * root, int indentlevel );
int menu_size(menu * root);
int menu_node_count(menu * root);
menu * traverse_menu_via_names(menu * dst, ...); 
int add_to_menu_via_names(menu * dst, menu * src, ...);
void add_to_simplemenu_at_index(menu * dst, int idx, menu * src);
int add_to_simplemenu_before_name(menu * dst, char* name, menu * src);
int add_to_simplemenu_after_name(menu * dst, char* name, menu * src);
menu ** add_childrenslots(menu * dst, int numspaces);
void merge_menus_at(menu * dst, menu * src, ... );
void merge_menus(menu * dst, menu * src);
#ifdef __cplusplus      
}
#endif  
#endif

