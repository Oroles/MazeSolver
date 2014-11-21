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
	struct node* next;
};

struct node* create_empty_node();
void init_nod_position(struct node* root, S8 x, S8 y);

void add_node(struct node* root, struct node* new_node);
void add_node_priority(struct node* root, struct node* new_node);

struct node* remove_first_node(struct node* root);
void remove_node(struct node* root, struct node* nod );

int find_node(struct node* root, struct node* node_to_find);

int equal(struct node* first_node, struct node* second_node);

void free_list(struct node* root);
int count(struct node* root);

#endif
