#pragma once

#include "stdafx.h"


#define STATE_IDLE 0
#define STATE_COLLECTING_STRINGS 1
#define STATE_EXPECTING_ARRAY 2
#define STATE_EXPECTING_ID 3

#define copy_rstring(from, to) strcpy_s(to, RSTRING_PTR(from));

typedef struct script_message
{
	char id[256];
	
	char jp[256];
	char en[256];
	char fr[256];
	char it[256];
	char de[256];
	char sp[256];
	char kr[256];
	char cn[256];

	char ru[256];
} script_message;

typedef struct script_scene
{
	int num_ids;
	char(*ids)[255];
} script_scene;

typedef struct script_content
{
	int num_messages;
	script_message messages[100];
	
	int num_scenes;
	script_scene scenes[100];
} script_content;

script_content* script_extract(FILE* script);

void script_export(script_content* content, const char* filename);

void script_export_debug(FILE* file, const char* out_filename);

script_message* script_find_messsage(script_content* content, const char* id);
