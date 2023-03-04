#ifndef _MM_CHAN_H
#define _MM_CHAN_H


#include "editabletypes.h"


typedef struct cht {
    char name[32];
    float power;
    double txf;
    double rxf;
    int encode;
    int decode;
} channel_t;

static const struct_field_desc channel_t_fields[] = {
    {EDIT_CHAR32, offsetof(channel_t, name),   "name"},
    {EDIT_FLOAT,  offsetof(channel_t, power),  "power"},
    {EDIT_DOUBLE, offsetof(channel_t, txf),    "txf"},
    {EDIT_DOUBLE, offsetof(channel_t, rxf),    "rxf"},
    {EDIT_INT,    offsetof(channel_t, encode), "encode"},
    {EDIT_INT,    offsetof(channel_t, decode), "decode"},
};
static const struct_desc channel_t_desc = {
    .count = 6,
    .fields = channel_t_fields,
};

//tags to easily filter channels super nicely?

typedef struct chtl {
    uint32_t     sanity; //must always be == SAFE2SET
    editabletype type; //for this, must be set to EDIT_CHANNEL_T for each instance
    channel_t * channels;
    int count;
} channellist_t;


#endif
