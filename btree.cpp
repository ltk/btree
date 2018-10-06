// btree.cpp

#include <iostream>
#include "btree.h"

using namespace std;

void insert_and_fix(int key, btree*& insertion_node, btree*& root) {
  // We’ll start by creating a new, empty array to hold our new key sequence.
  int new_keys[BTREE_ORDER];

  // We iterate through the existing node’s keys, adding each key to the new array
  // until we encounter a value that is larger than the key being inserted. We then
  // add the insertion key to the new array, and then add the remainder of the
  // existing keys. Then we increment the value of the node’s num_keys.
  bool key_inserted = false;
  for (int i = 0; i < insertion_node->num_keys; i++) {
    int existing_key = insertion_node->keys[i];
    if (!key_inserted && key < existing_key) {
      new_keys[i] = key;
      key_inserted = true;
    }

    int insertion_index = key_inserted ? i + 1 : i;
    new_keys[insertion_index] = existing_key;
  }

  // If the key being inserted is the largest key in the node,
  // or the node has no keys yet, we won't have added it yet.
  // Add it now to the end.
  if (!key_inserted) {
    new_keys[insertion_node->num_keys] = key;
  }

  // Increment the number of keys in this node.
  insertion_node->num_keys++;

  // Set the new keys.
  for (int j = 0; j < insertion_node->num_keys; j++) {
    insertion_node->keys[j] = new_keys[j];
  }

  // There is now a possibility of the node being overfull. We’ll check this by comparing
  // num_keys to the maximum allowed number of keys (BTREE_ORDER - 1). If we are not overfull, return.
  if (insertion_node->num_keys <= BTREE_ORDER - 1) {
    return;
  }

  // Otherwise, we are overfull, and need to fix the tree to satisfy the key count invariant.
  // Call `split_node` providing the btree root node reference and the insertion node reference.
  // TODO!
}

void insert(btree*& root, int key) {
  // The provided pointer could be null, which means there is no existing tree.
  // We can handle this by creating one! Just create a node with the provided value
  // as a key, update the provided pointer to point at the new node, and return.

  // if (root == NULL) {
  //   root = new btree;
  //   root->num_keys = 1;
  //   root->is_leaf = true;
  //   root->keys[0] = key;
  //   return;
  // }

  // Otherwise we’ll find the node that we need to insert the key into by calling the
  // `find` function as defined above. Once we have the insertion node, we’ll check to
  // see if it already contains the key we want to insert. If it does, just return (since
  // one of our invariants is that all keys are unique).
  btree* insertion_node = find(root, key);
  for (int i = 0; i < insertion_node->num_keys; i++) {
    if (insertion_node->keys[i] == key) {
      return;
    }
  }

  // Otherwise we’ll call a helper function `insert_and_fix`, providing a reference to the insertion
  // node. Potential invariant violations will be corrected by the `insert_and_fix` helper method.
  insert_and_fix(key, insertion_node, root);
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

