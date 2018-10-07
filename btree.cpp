// btree.cpp

#include <iostream>
#include <math.h>
#include "btree.h"

using namespace std;

btree* find_parent(btree*& node, btree*& root) {
  // If the root node is equal to the target node, return null, since there is no parent node.
  if (node == root) {
    return NULL;
  }

  int first_key_of_node = node->keys[0];

  // Scan the key values of the root node to see if the first key of the target node is present.
  // If it is, return null since there is no parent node.
  for (int i = 0; i < root->num_keys + 1; i++) {
    if (root->keys[i] == first_key_of_node) {
      return NULL;
    }
  }

  // Otherwise, for each key in the root node, if the target node first key is less than the key
  // we’re looking at, check if the child node linked from the left of the key we’re looking at is
  // equal to the target node.
  for (int j = 0; j < root->num_keys; j++) {
    int key = root->keys[j];
    if (first_key_of_node < key) {
      btree* child = root->children[j];
      if (child == node) {
        // If it is, return the root node reference.
        return root;
      } else {
        // Otherwise call `find_parent_node` again passing in the child node linked from the left of
        // the key we’re looking at as the root node.
        return find_parent(node, child);
      }
    }
  }
  
  // If we get to the end and still haven’t called `find_parent_node` again,
  // check if the child node linked from the right of the key we’re looking at is equal to the target
  // node. If it is, return the root node reference, otherwise call `find_parent_node` passing in the
  // last child as the root node.
  btree* last_child = root->children[root->num_keys];
  if (last_child == node) {
    return root;
  } else {
    return find_parent(node, last_child);
  }
}

void split_node(btree*& node, btree*& root) {
  // Find the median key (the key at index ceil(order / 2)).
  int median_key_index = ceil(node->num_keys / 2);
  int median_key = node->keys[median_key_index];


  // DEBUG
  cout << "Splitting" << endl;
  for (int p = 0; p < node->num_keys; p++) {
    cout << node->keys[p] << ",";
  }
  cout << endl;
  cout << "at" << endl;
  cout << median_key << endl;
  // END DEBUG

  // Create two new arrays (“lower_keys” and “higher_keys”) containing keys lower than and
  // greater than the median key respectively.
  int lower_keys[BTREE_ORDER];
  int higher_keys[BTREE_ORDER];

  int lower_count = 0;
  int higher_count = 0;
  for (int i = 0; i < node->num_keys; i++) {
    int key = node->keys[i];
    if (key < median_key) {
      lower_keys[lower_count] = key;
      lower_count++;
    } else if (key > median_key) {
      higher_keys[higher_count] = key;
      higher_count++;
    }
  }

  // DEBUG
  cout << "Lower" << endl;
  for (int p = 0; p < lower_count; p++) {
    cout << lower_keys[p] << ",";
  }
  cout << endl;

  cout << "Higher" << endl;
  for (int p = 0; p < higher_count; p++) {
    cout << higher_keys[p] << ",";
  }
  cout << endl;
  // END DEBUG

  // Set current node’s keys to the lower_keys, and set the current node’s num_keys to the
  // count of prior key values lower than the median key.
  node->num_keys = lower_count;
  for (int j = 0; j < node->num_keys; j++) {
    node->keys[j] = lower_keys[j];
  }

  // DEBUG
  cout << "Node's New Keys" << endl;
  for (int p = 0; p < node->num_keys; p++) {
    cout << node->keys[p] << ",";
  }
  cout << endl;
  // END DEBUG

  btree* parent;
  if (node == root) {
    // DEBUG
    cout << "Node is root" << endl;
    cout << "NODE IS " << node << endl;
    cout << "ROOT IS " << root << endl;
    // END DEBUG


    // If the target node is the root node, create a new btree node and update the root node
    // pointer to point to it. This new node is now our parent node.
    btree* new_root = new btree;
    root = new_root;
    root->is_leaf = false;
    root->num_keys = 0;
    parent = root;
  } else {
    // Otherwise, find the current node’s parent node using the `find_parent` function.
    parent = find_parent(node, root);
  }

  // DEBUG
  cout << "NODE IS " << node << endl;
  cout << "ROOT IS " << root << endl;
  cout << "PARENT IS " << parent << endl;
  cout << "Parent's Existing Keys" << endl;
  for (int p = 0; p < parent->num_keys; p++) {
    cout << parent->keys[p] << ",";
  }
  cout << endl;
  // END DEBUG

  // Creating a new, empty array to hold the new key sequence for the parent node.
  int parent_keys[BTREE_ORDER];

  // We iterate through the parent node’s keys, adding each key to the new array until we
  // encounter a value that is larger than the median key. We then add the median key to the
  // new array. Then add the remainder of the parent node’s keys. Then we increment the value
  // of the parent node’s num_keys.
  int key_insertion_index;

  if (parent->num_keys == 0) {
    parent->keys[0] = median_key;
    parent->num_keys = 1;
    key_insertion_index = 0;
  } else {
    bool key_inserted = false;
    for (int i = 0; i < parent->num_keys; i++) {
      int key = parent->keys[i];
      if (!key_inserted && median_key < key) {
        parent_keys[i] = key;
        key_inserted = true;
        key_insertion_index = i;
      }

      int insertion_index = key_inserted ? i + 1 : i;
      parent_keys[insertion_index] = key;
    }
  }

  // DEBUG
  cout << "Parent's New Keys" << endl;
  for (int p = 0; p < parent->num_keys; p++) {
    cout << parent->keys[p] << ",";
  }
  cout << endl;
  // END DEBUG

  // Now create a new children array in the same fashion, inserting a reference to the current
  // node at index (key insertion index). Then create a new btree node with keys set to
  // higher_keys, and insert a reference to it in the children array at index (key insertion index + 1).
  btree* parent_children[BTREE_ORDER + 1];
  bool children_inserted = false;

  btree* new_node = new btree;
  new_node->num_keys = higher_count;
  new_node->is_leaf = true;
  for (int l = 0; l < higher_count; l++) {
    new_node->keys[l] = higher_keys[l];
  }

  for (int k = 0; k < parent->num_keys + 1; k++) {
    if (!children_inserted && k == key_insertion_index) {
      parent->children[k] = node;
      parent->children[k+1] = new_node;
      children_inserted = true;
    }

    btree* child = parent->children[k];

    // DEBUG
    cout << "k is: " << k << ". II is " << key_insertion_index << endl;
    // END DEBUG

    int insertion_index = children_inserted ? k + 2 : k;
    parent_children[insertion_index] = child;
  }

  // DEBUG
  cout << "Parent's New Children" << endl;
  for (int p = 0; p < parent->num_keys + 1; p++) {
    cout << parent->children[p] << ", ";
  }
  cout << endl;
  // END DEBUG

  // Check to see if the parent is now overfull (in the manner described previously). If it is,
  // call `split_node` for the parent node.
  if (parent->num_keys > BTREE_ORDER - 1) {
    // DEBUG
    cout << "Splitting Again" << endl;
    // END DEBUG
    split_node(parent, root);
  }
}

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
   split_node(insertion_node, root);
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

