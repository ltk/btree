// btree.cpp

#include <iostream>
#include "btree.h"

using namespace std;

void insert(btree*& root, int key) {
  // TODO
}

void remove(btree*& root, int key) {
  // TODO
}

btree* find(btree*& root, int key) {
  if (root == NULL) {
    return NULL;
  }

  if (root->is_leaf) {
    return root;
  }

  // scan the key values of the input node to see if the input key is present.
  // If it is, return this node.
  for (int i = 0; i < root->num_keys; i++) {
    if (root->keys[i] == key) {
      return root;
    }
  }

  // Otherwise, for each key in the input node, if the input key is less than the
  // key we’re looking at, call `find` again passing in the child node linked from
  // the left of the key we’re looking at. 
  for (int j = 0; j < root->num_keys; j++) {
    if (key < root->keys[j]) {
      return find(root->children[j], key);
    }
  }

  // If we get to the end and still haven’t called `find` again, call `find` passing in the last child.
  return find(root->children[root->num_keys], key);
}

int count_nodes(btree*& root) {
  if (root == NULL) {
    return 0;
  }

  int count = 1;

  if (!root->is_leaf) {
    for (int i = 0; i <= root->num_keys; i++) {
      count += count_nodes(root->children[i]);
    }
  }

  return count;
}

int count_keys(btree*& root) {
  if (root == NULL) {
    return 0;
  }

  int count = root->num_keys;

  if (!root->is_leaf) {
    for (int i = 0; i <= root->num_keys; i++) {
      count += count_keys(root->children[i]);
    }
  }

  return count;
}

