#ifndef _MM_EDITABLETYPES_H
#define _MM_EDITABLETYPES_H


#define SAFE2SET 0x5afe25e7 //"safe 2 set"
			    //must be present in the 'sanity' field of a safe2set_t or any compatible struct that looks like that.
			    //e.g. first uint32_t of the struct must equal that. Then must have a editabletype from the enum
typedef enum {
	UPDATE_NO_ERROR = 0,
	UPDATE_FAILURE,
	UPDATE_FAILBADTYPE,
	UPDATE_FAILBADPTR,
	//...
} update_res;
typedef enum editabletypes {
    EDIT_NO_TYPE_SELECTED=0,
    EDIT_CHAR8,
    EDIT_CHAR16,
    EDIT_CHAR32,

    EDIT_CHAR256, //not implemented yet
    EDIT_VARCHAR, //not implemented yet, not sure how it would work

    EDIT_INT,
    EDIT_FLOAT,
    EDIT_DOUBLE,


    EDIT_SELECT,  //a select/options style choice
    EDIT_BOOL,  //a special version of EDIT_SELECT
    EDIT_RANGE,  //a min/max number range


    EDIT_CHANNEL_T,
    EDIT_CHANNELLIST_T,
    EDIT_ENCRYPTKEY,
    EDIT_ENCRYPTKEYLIST,

} editabletype;

typedef struct editablestructbase {
    uint32_t     sanity;
    editabletype type;
} safe2set_t; //only really intended for casting a pointer and checking values for casting to the right value

typedef struct struct_field_desc {
    editabletype type;
    size_t offset;
    const char *name;
} struct_field_desc;

typedef struct struct_desc {
    int count;
    const struct_field_desc * fields;
} struct_desc;

#endif
