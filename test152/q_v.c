#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include "q_v.h"

#ifdef __cplusplus      
extern "C" {  
#endif  

char * viewtypestr[] = {
    "simplemenu",
    "listing",
    "custom"
};
char * qViewModeStr[] = {
    "NORMAL",
    "EDIT",
    "EDITFIELD",
};
int child_idx_by_name(menu * m, char * name){
    int idx = -1;
    assert( m != NULL );
    assert( name != NULL );
    assert( m->numchildren > 0 && m->children != NULL );
    for( int i = 0; i < m->numchildren; i++ ){
        if( strcmp(m->children[i]->name, name) == 0){
            return i;
        }
    }
    return idx;
}
int menu_size(menu * root){
    return sizeof(menu) * menu_node_count(root);
}
int menu_node_count(menu * root){
    int count = 0;
    if( root->children != NULL ){
        for( int i = 0; i < root->numchildren; i++ ){
            count += menu_node_count(root->children[i]); //each child
        }
    }
    return count+1;
}
void printallmenu(menu * root, int indentlevel ){
    for(int j = 0; j < indentlevel; j++){printf("\t");}
    printf("%s [%s] \n", root->name, viewtypestr[root->type]);
    if( root->numchildren > 0 && root->children != NULL ){
        for( int i = 0; i < root->numchildren; i++ ){
            printallmenu(root->children[i], indentlevel+1);
        }
    }
}


menu * traverse_menu_via_names(menu * dst, ...){
    //variadic function.
    //call like down_menu_via_names(dst, "MainMenu", "Settings", "Security", NULL); //last argument MUST be NULL to indicate end of arguments
    va_list ap;
    va_start(ap, dst);
    while( 1 ){
        char * entryname = va_arg(ap, char*);
        if( entryname == NULL ){
            break;
        }
        int j = child_idx_by_name(dst, entryname);
        if( j >= 0 ){
            dst = dst->children[j];
            /*printf("%s %d \n", entryname, j);*/
        } else {
            return NULL;
        }
    }
    return dst;
}
int add_to_menu_via_names(menu * dst, menu * src, ...){
    //variadic function.
    //call like add_to_menu_via_names(dst, src, "MainMenu", "Settings", "Security", NULL)
    va_list ap;
    va_start(ap, src);
    while( 1 ){
        char * entryname = va_arg(ap, char*);
        if( entryname == NULL ){
            break;
        }
        int j = child_idx_by_name(dst, entryname);
        if( j >= 0 ){
            dst = dst->children[j];
            /*printf("%s %d \n", entryname, j);*/
        } else {
            return j;
        }
    }
    add_to_simplemenu_at_index(dst, -1, src);
    va_end(ap);
    return 0;
}

menu ** add_childrenslots(menu * dst, int numspaces){
    assert(numspaces > 0);
    int totalslots = dst->numchildrenslots + numspaces;
    int newsize = sizeof(menu*) * totalslots;
    menu ** cousins = malloc(newsize);
    memset(cousins, 0, newsize);
    memcpy(cousins, dst->children, sizeof(menu*) * dst->numchildren);
    //TODO: free old children here?
    if( dst->freeableslots ){
        free(dst->children);
    }
    if( cousins != NULL ){
        dst->children = cousins;
        dst->numchildrenslots += numspaces;
        dst->freeableslots = true;
    }
    return cousins;
}
void add_to_simplemenu_at_index(menu * dst, int idx, menu * src){
    //requires malloc
    //inserts at idx
    //0 puts at top, etc
    //-1 appends to end
    //dst->children is a pointer to an array of multiple (menu *)
    //on every addition to dst->children we should 
    //1. allocate enough space for the current entries plus what we're adding
    //2. copy out all the current entries into the new space and add the new entry
    //3. replace our children pointer in dst
    //4. TODO: somehow need a way to safely free the old array, since we're potentially going to have some of those. 
    //That'll probably require a bit to be set indicating that it's dynamically allocated.
    //could also pre-allocate spaces for the main menu and have a numchildren and numchildrenslots or something like that to cut down on executions of malloc
    bool enoughspace = dst->numchildren + 1 <= dst->numchildrenslots;

    if( enoughspace ){
        //there's enough space, so transform idx as appropriate to handle negative values
        if( idx < 0 ){
            idx = dst->numchildren + idx + 1; //so -1 is the last entry, -2 is the second to last entry, ...
        }
        if( idx < 0 ){  //you're seeing double on purpose
            //still can't add even after idx transform, idx would be nonsensical value because it's -n for a list of length < abs(n) . Just append it instead.
            idx = dst->numchildren;
        }
        //idx is now a reasonable >=0 value suitable as an index into the array
        assert(idx >= 0 );
        for( int i = dst->numchildren; i > idx; i-- ){
            //shift all the displaced entries, starting from the last to keep it simple
            dst->children[i] = dst->children[i-1];
            //at the end of this loop, idx == i 
        }
        //so we add src to children
        dst->children[idx] = src;
        //and remember that the count has changed now.
        dst->numchildren++;
    } else {
        if( add_childrenslots(dst, 1) == NULL ){
            //TODO: ERR
            return;
        }
        enoughspace = dst->numchildren + 1 <= dst->numchildrenslots;
        assert(enoughspace);//make sure it worked, because we need this recursive call to hit the base case to be safe.
        return add_to_simplemenu_at_index(dst, idx, src); 
    }
}
int add_to_simplemenu_after_name(menu * dst, char* name, menu * src){
    //it's an error to give a name that's not in the menu
    int j = child_idx_by_name(dst, name);
    if( j < 0 ){
        return j;
    }
    add_to_simplemenu_at_index(dst, j+1, src);
    return j+1;
}
int add_to_simplemenu_before_name(menu * dst, char* name, menu * src){
    //it's an error to give a name that's not in the menu
    int j = child_idx_by_name(dst, name);
    if( j < 0 ){
        return j;
    }
    add_to_simplemenu_at_index(dst, j, src);
    return j;
}
void merge_menus_at(menu * dst, menu * src, ... ){
    va_list ap;
    va_start(ap, src);
    while( 1 ){
        char * entryname = va_arg(ap, char*);
        if( entryname == NULL ){
            break;
        }
        int j = child_idx_by_name(dst, entryname);
        if( j >= 0 ){
            dst = dst->children[j];
            /*printf("%s %d \n", entryname, j);*/
        } else {
            return;
        }
    }
    merge_menus(dst,src);
}
void merge_menus(menu * dst, menu * src){
    //UNTESTED, and won't work right now. Requires implementation for the base case.
    //
    //warning: malloc()s memory. free()s memory. Modifies dst but does not modify src in any way, to include free()s
    //requires all dst menu components to be on the heap
    //Will graft the src tree onto the dst tree - adding entries as needed, and recursing down the tree for matching subtrees to avoid overwriting.
    //uses the name field for a particular entry at a particular level as a unique identifier, so having two 'settings' menu entries is fine as long as they are not siblings.
    //called 'merge' because it cannot remove items. The idea here is to support adding new entries for when, as an example, a plugin is loaded 
   

    //or wait - does it need to malloc and free anything other than the children[] arrays? and update those pointers? hmm.
    //alternatively we could have a 'dynamic' vs 'static' option for the creation to indicate whether to free something or not.

    //recurse down dst and check sizes.

    for( int i = 0; i < src->numchildren ; i++ ){
            //for each entry in src 
            //find the matching entry at this level if possible
            int j = child_idx_by_name(dst, src->children[i]->name);
            if( j >= 0 ){
                //if it's found, recurse down the subtree where possible
                if( dst->children[j]->numchildren >= 0 && src->children[i]->numchildren >= 0 ){
                    printf("Mutating %s\n", dst->children[j]->name);
                    merge_menus( dst->children[j], src->children[i]);
                } else {
                    printf("Error! Name collision but can't graft menu trees\n");
                }
            } else {
                //whereas if we never found a match for our entry in src, we want to add it to dst
                //And if by chance we're in the MainMenu, we add it before Settings.
                int settings_idx = child_idx_by_name(dst, "Settings");
                //but mutate_menu gets called recursively - in those instances, settings_idx is -1 and add_to_menu_after interprets -1 as "append to end" of menu.
                //so it works out :)
                add_to_simplemenu_at_index(dst, settings_idx, src);
            }
        }

}

#ifdef __cplusplus      
}
#endif  
