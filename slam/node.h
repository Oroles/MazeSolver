#ifndef _NODE_H
#define _NODE_H

#include <stdlib.h>
#include "kernel.h"
#include "ecrobot_interface.h"

#define NULL 0

struct node {
	S8 x;
	S8 y;
	S8 f_cost;
	S8 g_cost;
	struct node* parent;
};

struct list_element
{
	struct node* data;
	struct list_element* next;
};

struct node* create_empty_node();
struct list_element* create_empty_list_element();

void init_nod_position(struct node* root, S8 x, S8 y);

void add_node(struct list_element** root, struct list_element* new_node);
void add_node_priority(struct list_element** root, struct list_element* new_node);

struct list_element* remove_first_node(struct list_element** root);

int find_node(struct list_element** root, struct node* node_to_find);

void free_list(struct list_element** root);

#endif 
