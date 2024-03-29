/*
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <ugelis.h>
#include <tc_util.h>
#include <ztest.h>
#include <misc/rb.h>

#define CHECK(n) \
	zassert_true(!!(n), "Tree check failed: [ " #n " ] @%d", __LINE__)

#define MAX_NODES 256

static struct rbtree tree;

static struct rbnode nodes[MAX_NODES];

/* Bit is set if node is in the tree */
static unsigned int node_mask[(MAX_NODES + 31)/32];

/* Array of nodes dumed via rb_walk */
static struct rbnode *walked_nodes[MAX_NODES];

/* Node currently being inserted, for testing lessthan() argument order */
static struct rbnode *current_insertee;

void set_node_mask(int node, int val)
{
	unsigned int *p = &node_mask[node / 32];
	unsigned int bit = 1u << (node % 32);

	*p &= ~bit;
	*p |= val ? bit : 0;
}

int get_node_mask(int node)
{
	unsigned int *p = &node_mask[node / 32];
	unsigned int bit = 1u << (node % 32);

	return !!(*p & bit);
}

int node_index(struct rbnode *n)
{
	return (int)(n - &nodes[0]);
}

/* Our "lessthan" is just the location of the struct */
int node_lessthan(struct rbnode *a, struct rbnode *b)
{
	if (current_insertee) {
		CHECK(a == current_insertee);
		CHECK(b != current_insertee);
	}

	return a < b;
}

/* Simple LCRNG (modulus is 2^64!) cribbed from:
 * https://nuclear.llnl.gov/CNP/rng/rngman/node4.html
 *
 * Don't need much in the way of quality, do need repeatability across
 * platforms.
 */
static unsigned int next_rand_mod(unsigned int mod)
{
	static unsigned long long state = 123456789; /* seed */

	state = state * 2862933555777941757ul + 3037000493ul;

	return ((unsigned int)(state >> 32)) % mod;
}

void visit_node(struct rbnode *node, void *cookie)
{
	int *nwalked = cookie;

	CHECK(*nwalked < MAX_NODES);

	walked_nodes[*nwalked] = node;
	*nwalked += 1;
}

/* Stores the last-seen black height at a leaf during check_rb(), or
 * zero if no leaves have been seen yet
 */
static int last_black_height;

void check_rbnode(struct rbnode *node, int blacks_above)
{
	int side, bheight = blacks_above + _rb_is_black(node);

	for (side = 0; side < 2; side++) {
		struct rbnode *ch = _rb_child(node, side);

		if (ch) {
			/* Basic tree requirement */
			if (side == 0) {
				CHECK(node_lessthan(ch, node));
			} else {
				CHECK(node_lessthan(node, ch));
			}

			/* Can't have adjacent red nodes */
			CHECK(_rb_is_black(node) || _rb_is_black(ch));

			/* Recurse */
			check_rbnode(ch, bheight);
		} else {
			/* All leaf nodes must be at the same black height */
			if (last_black_height) {
				CHECK(last_black_height == bheight);
			}
			last_black_height = bheight;
		}
	}
}

void check_rb(void)
{
	last_black_height = 0;

	CHECK(tree.root);
	CHECK(_rb_is_black(tree.root));

	check_rbnode(tree.root, 0);
}

/* First validates the external API behavior via a walk, then checks
 * interior tree and red/black state via internal APIs.
 */
void _check_tree(int size, int use_foreach)
{
	int nwalked = 0, i, ni;
	struct rbnode *n, *last = NULL;

	memset(walked_nodes, 0, sizeof(walked_nodes));

	if (use_foreach) {
		RB_FOR_EACH(&tree, n) {
			visit_node(n, &nwalked);
		}
	} else {
		rb_walk(&tree, visit_node, &nwalked);
	}

	/* Make sure all found nodes are in-order and marked in the tree */
	for (i = 0; i < nwalked; i++) {
		n = walked_nodes[i];
		ni = node_index(n);

		if (last) {
			CHECK(node_lessthan(last, n));
		}

		CHECK(get_node_mask(ni));

		last = n;
	}

	/* Make sure all tree bits properly reflect the set of nodes we found */
	ni = 0;
	for (i = 0; i < MAX_NODES; i++) {
		CHECK(get_node_mask(i) == rb_contains(&tree, &nodes[i]));

		if (get_node_mask(i)) {
			CHECK(node_index(walked_nodes[ni]) == i);
			ni++;
		}
	}

	CHECK(ni == nwalked);

	if (tree.root) {
		check_rb();
	}
}

void check_tree(int size)
{
	/* Do it with both enumeration mechanisms */
	_check_tree(size, 0);
	_check_tree(size, 1);
}

void checked_insert(struct rbtree *tree, struct rbnode *node)
{
	current_insertee = node;
	rb_insert(tree, node);
	current_insertee = NULL;
}

void test_tree(int size)
{
	int i, j;

	/* Small trees get checked after every op, big trees less often */
	int small_tree = size <= 32;

	memset(&tree, 0, sizeof(tree));
	tree.lessthan_fn = node_lessthan;
	memset(nodes, 0, sizeof(nodes));
	memset(node_mask, 0, sizeof(node_mask));

	for (j = 0; j < 10; j++) {
		for (i = 0; i < size; i++) {
			int node = next_rand_mod(size);

			if (!get_node_mask(node)) {
				rb_insert(&tree, &nodes[node]);
				set_node_mask(node, 1);
			} else {
				rb_remove(&tree, &nodes[node]);
				set_node_mask(node, 0);
			}

			if (small_tree) {
				check_tree(size);
			}
		}

		if (!small_tree) {
			check_tree(size);
		}
	}
}

void test_rbtree_spam(void)
{
	int size = 1;

	do {
		size += next_rand_mod(size) + 1;

		if (size > MAX_NODES) {
			size = MAX_NODES;
		}

		TC_PRINT("Checking trees built from %d nodes...\n", size);

		test_tree(size);
	} while (size < MAX_NODES);
}

void test_main(void)
{
	ztest_test_suite(test_rbtree,
			 ztest_unit_test(test_rbtree_spam));
	ztest_run_test_suite(test_rbtree);
}
