#include "node.h"

struct node* create_empty_node() {
	struct node* empty_node = (struct node*)malloc( sizeof( struct node ) );
	empty_node->f_cost = 0;
	empty_node->g_cost = 127;
	empty_node->x = 0;
	empty_node->y = 0;
	empty_node->next = NULL;
	empty_node->parent = NULL;
	return empty_node;
}

void init_nod_position(struct node* root, S8 x, S8 y) {
	root->x = x;
	root->y = y;
}

void add_node_priority(struct node** root, struct node* new_node) {
	if ( *root == NULL ) {
		*root = new_node;
		return;
	}
	struct node* it_prev = NULL;
	struct node* it = *root;
	while( it != NULL ) { 
		if ( it->f_cost <= new_node->f_cost ) {
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

void add_node(struct node** root, struct node* new_node) {
	if ( *root == NULL ) {
		*root = new_node;
		return;
	}
	struct node* it = *root;
	while( it->next != NULL ) {
		it = it->next;
	}
	it->next = new_node;
}

struct node* remove_first_node(struct node** root) {
	if ( *root == NULL ) {
		return NULL;
	}
	struct node* aux = *root;
	*root = (*root)->next;
	aux->next = NULL;
	return aux;
}

void remove_node(struct node** root, struct node* nod)
{
	if( *root == NULL ) {
		return;
	}
	struct node* it_prev = NULL;
	struct node* it = *root;
	while( ( it != NULL ) && ( !equal( it, nod ) ) ) {
		it_prev = it;
		it = it->next;
	}
	if ( it == NULL ) {
		return;
	}
	if ( it_prev == NULL ) {
		it = NULL;
		return;
	}
	it_prev->next = it->next;
	//In case the deleted nod is the last node
	if ( it->next != NULL ) {
		it->next->parent = it_prev;
	}
}

int equal(struct node* first_node, struct node* second_node) {
	if( ( first_node->x == second_node->x ) && ( first_node->y == second_node->y ) ) {
		return TRUE;
	}
	return FALSE;
}

int find_node(struct node** root, struct node* node_to_find) {
	if ( root == NULL ) {
		return FALSE;
	}
	struct node* it = *root;
	while ( it != NULL ) {
		if ( ( it->x ==  node_to_find->x ) && ( it->y == node_to_find->y ) ) {
			return TRUE;
		}
		it = it->next;
	}
	return FALSE;
}

void free_list(struct node** root) {
	if ( *root == NULL ) {
		return;
	}
	struct node* aux = *root;
	while ( aux != NULL ) {
		*root = aux->next;
		free( aux );
		aux = *root;
	}
}

int count(struct node* root) {
	int result = 0;
	while( root != NULL ) {
		root = root->next;
		++result;
	}
	return result;
}
