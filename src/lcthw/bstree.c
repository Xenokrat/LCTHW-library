#include <assert.h>
#include <lcthw/bstree.h>
#include <lcthw/bstrlib.h>
#include <lcthw/dbg.h>
#include <stdlib.h>
#include <string.h>

static int
default_compare(void *a, void *b)
{
	return bstrcmp((bstring)a, (bstring)b);
}

BSTree *
BSTree_create(BSTree_compare compare)
{
	BSTree *map = calloc(1, sizeof(BSTree));
	check_mem(map);

	map->compare = compare == NULL ? default_compare : compare;
	map->root = NULL;
	map->count = 0;

	return map;
error:
	if (map) {
		BSTree_destroy(map);
	}
	return NULL;
}

static int
BSTree_destroy_cb(BSTreeNode *node)
{
	free(node);
	return 0;
}

void
BSTree_destroy(BSTree *map)
{
	if (map) {
		BSTree_traverse(map, BSTree_destroy_cb);
		free(map);
	}
}

static inline BSTreeNode *
BStreeNode_create(BSTreeNode *parent, void *key, void *data)
{
	BSTreeNode *node = calloc(1, sizeof(BSTreeNode));
	check_mem(node);

	node->key = key;
	node->data = data;
	node->parent = parent;

	return node;
error:
	return NULL;
}

static inline void
BSTree_setnode(BSTree *map, BSTreeNode *node, void *key, void *data)
{
	assert(map != NULL && "Map is NULL");
	assert(node != NULL && "Node is NULL");

	int cmp = map->compare(node->key, key);

	if (cmp <= 0) {
		if (node->left) {
			BSTree_setnode(map, node->left, key, data);
		} else {
			node->left = BStreeNode_create(node, key, data);
		}
	} else {
		if (node->right) {
			BSTree_setnode(map, node->right, key, data);
		} else {
			node->right = BStreeNode_create(node, key, data);
		}
	}
}

int
BSTree_set(BSTree *map, void *key, void *data)
{
	check_mem(map);

	if (map->root == NULL) {
		// first so just make it and get out
		map->root = BStreeNode_create(NULL, key, data);
		check_mem(map->root);
	} else {
		BSTree_setnode(map, map->root, key, data);
	}

	return 0;

error:
	return -1;
}

static inline BSTreeNode *
BSTree_getnode(BSTree *map, BSTreeNode *node, void *key)
{
	assert(map != NULL && "Map is NULL");
	assert(node != NULL && "Node is NULL");

	int cmp = map->compare(node->key, key);

	if (cmp == 0) {
		return node;
	} else if (cmp < 0) {
		if (node->left) {
			return BSTree_getnode(map, node->left, key);
		} else {
			return NULL;
		}
	} else {
		if (node->right) {
			return BSTree_getnode(map, node->right, key);
		} else {
			return NULL;
		}
	}
}

void *
BSTree_get(BSTree *map, void *key)
{
	check_mem(map);

	if (map->root == NULL) {
		return NULL;
	} else {
		BSTreeNode *node = BSTree_getnode(map, map->root, key);
		return node == NULL ? NULL : node->data;
	}
error:
	return NULL;
}

static inline int
BSTree_traverse_nodes(BSTreeNode *node, BSTree_traverse_cb traverse_cb)
{
	int rc = 0;
	assert(node != NULL && "Node is NULL");

	if (node->left) {
		rc = BSTree_traverse_nodes(node->left, traverse_cb);
		if (rc != 0) {
			return rc;
		}
	}

	if (node->right) {
		rc = BSTree_traverse_nodes(node->right, traverse_cb);
		if (rc != 0) {
			return rc;
		}
	}

	return traverse_cb(node);
}

int
BSTree_traverse(BSTree *map, BSTree_traverse_cb traverse_cb)
{
	assert(map != NULL && "Map is NULL");
	if (map->root) {
		return BSTree_traverse_nodes(map->root, traverse_cb);
	}

	return 0;
}

static inline BSTreeNode *
BSTree_find_min(BSTreeNode *node)
{
	while (node->left) {
		node = node->left;
	}

	/* !!! It's natural to return NULL */
	return node;
}

static inline void
BSTree_replace_node_in_parent(BSTree *map, BSTreeNode *node,
                              BSTreeNode *new_value)
{
	assert(map != NULL && "Map is NULL!");
	assert(node != NULL && "Node is NULL!");
	/* it's OK if new_value is NULL */

	if (node->parent) {
		if (node == node->parent->left) {
			node->parent->left = new_value;
		} else {
			node->parent->right = new_value;
		}
	} else {
		// this is the root so gotta change it
		map->root = new_value;
	}

	if (new_value) {
		new_value->parent = node->parent;
	}
}

static inline void
BSTree_swap(BSTreeNode *a, BSTreeNode *b)
{
	assert(a != NULL && "Node is NULL during swap");
	assert(b != NULL && "Node is NULL during swap");

	void *temp = NULL;
	temp = b->key;
	b->key = a->key;
	a->key = temp;
	temp = b->data;
	b->data = a->data;
	a->data = temp;
}

static inline BSTreeNode *
BSTree_node_delete(BSTree *map, BSTreeNode *node, void *key)
{
	assert(node != NULL && "Node is NULL!");
	int cmp = map->compare(node->key, key);

	if (cmp < 0) {
		if (node->left) {
			return BSTree_node_delete(map, node->left, key);
		} else {
			// Not found
			return NULL;
		}
	} else if (cmp > 0) {
		if (node->right) {
			return BSTree_node_delete(map, node->right, key);
		} else {
			// Not found
			return NULL;
		}
	} else {
		if (node->left && node->right) {
			// swap this node for the smallest node that is bigger
			// than us
			BSTreeNode *successor = BSTree_find_min(node->right);
			/* Here bc of condition above, BStree_find_min should NEVER return NULL */
			assert(successor != NULL && "successor is NULL");
			BSTree_swap(successor, node);

			// this leaves the old successor with possibly a right
			// child so replace it with that right child
			BSTree_replace_node_in_parent(map, successor,
			                              successor->right);

			// finally it's swapped, so return successor instead of
			// node
			return successor;
		} else if (node->left) {
			BSTree_replace_node_in_parent(map, node, node->left);
		} else if (node->right) {
			BSTree_replace_node_in_parent(map, node, node->right);
		} else {
			BSTree_replace_node_in_parent(map, node, NULL);
		}

		return node;
	}
}

void *
BSTree_delete(BSTree *map, void *key)
{
	assert(map != NULL && "Map is NULL");
	void *data = NULL;

	if (map->root) {
		BSTreeNode *node = BSTree_node_delete(map, map->root, key);

		if (node) {
			data = node->data;
			free(node);
		}
	}

	return data;
}
