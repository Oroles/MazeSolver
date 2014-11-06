#include "Node.h"

struct node* create_empty_node() {
	struct node* empty_node = (struct node*)malloc( sizeof( struct node ) );
	empty_node->f_cost = 0;
	empty_node->g_cost = 127;
	empty_node->x = 0;
	empty_node->y = 0;
	empty_node->parent = NULL;
	return empty_node;
}

struct list_element* create_empty_list_element(){
	struct list_element* element = (struct list_element*)malloc( sizeof( struct list_element ) );
	element->data = NULL;
	element->next = NULL;
	return element;
}

void init_nod_position(struct node* root, S8 x, S8 y) {
	root->x = x;
	root->y = y;
}

void add_node_priority(struct list_element** root, struct list_element* new_node) {
	if ( *root == NULL ) {
		(*root) = new_node;
		return;
	}
	struct list_element* it_prev = NULL;
	struct list_element* it = *root;
	while( it != NULL ) { 
		if ( it->data->f_cost <= new_node->data->f_cost ) {
			it_prev = it;
			it = it->next;
			continue;
		}
		else {
			if ( it_prev != NULL ) {
				new_node->next = it;
				it_prev->next = new_node;
			}
			else {
				new_node->next = it;
				*root = new_node;
			}
			return;
		}
	}
	new_node->next = it;
	it_prev->next = new_node;
}

void add_node(struct list_element** root, struct list_element* new_node) {
	if ( *root == NULL ) {
		(*root) = new_node;
		return;
	}
	struct list_element* it = *root;
	while( it->next != NULL ) {
		it = it->next;
	}
	it->next = new_node;
}

struct list_element* remove_first_node(struct list_element** root) {
	if ( *root == NULL ) {
		return NULL;
	}
	struct list_element* aux = *root;
	*root = (*root)->next;
	aux->next = NULL;
	return aux;
}

int find_node(struct list_element** root, struct node* node_to_find) {
	if ( root == NULL ) {
		return FALSE;
	}
	struct list_element* it = *root;
	while ( it != NULL ) {
		if ( ( it->data->x ==  node_to_find->x ) && ( it->data->y == node_to_find->y ) ) {
			return TRUE;
		}
		it = it->next;
	}
	return FALSE;
}

void free_list(struct list_element** root) {
	if ( *root == NULL ) {
		return;
	}
	struct list_element* aux = *root;
	while ( aux != NULL ) {
		*root = aux->next;
		free( aux->data );
		free( aux );
		aux = *root;
	}
	free( aux );
}
