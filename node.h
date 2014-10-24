#ifndef _NODE_H
#define _NODE_H

#include <stdlib.h>
#include "kernel.h"
#include "ecrobot_interface.h"

#define NULL 0

struct node {
	int x;
	int y;
	int f_cost;
	int g_cost;
	struct node* parent;
	struct node* next;
};

struct node* create_empty_node();
void init_nod_position(struct node* root, int x, int y);

void add_node(struct node** root, struct node* new_node);
void add_node_priority(struct node** root, struct node* new_node);

struct node* remove_first_node(struct node** root);

int find_node(struct node** root, struct node* node_to_find);

void free_list(struct node** root);
int count(struct node* root);

#endif
