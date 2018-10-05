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
  return NULL; // TODO
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

