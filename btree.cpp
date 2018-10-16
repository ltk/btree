// btree.cpp

#include <iostream>
#include <math.h>
#include "btree.h"

using namespace std;

void print_tree(btree* &root);
void remove_from_node(btree* node, btree*& root, int key);

void print_node(btree* node, int level) {
  cout << "Level " << level << "(leaf:" << node->is_leaf << ", numkeys:" << node->num_keys << ")" << endl;
  for (int p = 0; p < node->num_keys; p++) {
    cout << node->keys[p] << ",";
  }

  cout << endl;
}

// void print_tree(btree* root, int level) {
//   if (!root->is_leaf) {
//     cout << "children" << endl;
//     for (int i = 0; i < root->num_keys + 1; i++) {
//       print_node(root->children[i], level +1);
//     }
//     for (int i = 0; i < root->num_keys + 1; i++) {
//       print_tree(root->children[i], level +1);
//     }
//   }
// }

void pt(btree* root) {
  // print_node(root, 0);
  print_tree(root);
}

btree* find_parent(btree* node, btree*& root) {
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

void split_node(btree* node, btree*& root) {
  // Find the median key (the key at index ceil(order / 2)).
  int median_key_index = ceil(node->num_keys / 2);
  int median_key = node->keys[median_key_index];

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

  // Set current node’s keys to the lower_keys, and set the current node’s num_keys to the
  // count of prior key values lower than the median key.
  node->num_keys = lower_count;
  for (int j = 0; j < node->num_keys; j++) {
    node->keys[j] = lower_keys[j];
  }

  btree* parent;
  if (node == root) {
    // If the target node is the root node, create a new btree node and update the root node
    // pointer to point to it. This new node is now our parent node.
    btree* new_root = new btree;
    root = new_root;
    root->is_leaf = false;
    root->num_keys = 0;
    for (int i=0; i <= BTREE_ORDER; i++) {
      root->children[i] = NULL;
    }
    parent = root;
  } else {
    // Otherwise, find the current node’s parent node using the `find_parent` function.
    parent = find_parent(node, root);
  }

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
        parent->keys[i] = median_key;
        key_inserted = true;
        key_insertion_index = i;
      }

      int insertion_index = key_inserted ? i + 1 : i;
      parent->keys[insertion_index] = key;
    }
  }

  // Now create a new children array in the same fashion, inserting a reference to the current
  // node at index (key insertion index). Then create a new btree node with keys set to
  // higher_keys, and insert a reference to it in the children array at index (key insertion index + 1).
  btree* parent_children[BTREE_ORDER + 1];
  bool children_inserted = false;

  btree* new_node = new btree;
  new_node->num_keys = higher_count;
  new_node->is_leaf = true;
  for (int i=0; i <= BTREE_ORDER; i++) {
    new_node->children[i] = NULL;
  }
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

    int insertion_index = children_inserted ? k + 2 : k;
    parent_children[insertion_index] = child;
  }

  // Check to see if the parent is now overfull (in the manner described previously). If it is,
  // call `split_node` for the parent node.
  if (parent->num_keys > BTREE_ORDER - 1) {
    split_node(parent, root);
  }
}

void insert_and_fix(int key, btree* insertion_node, btree*& root) {
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

  if (root == NULL) {
    root = new btree;
    root->num_keys = 1;
    root->is_leaf = true;
    for (int i=0; i <= BTREE_ORDER; i++) {
      root->children[i] = NULL;
    }
    root->keys[0] = key;
    
    return;
  }

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

btree* prev_sibling(btree* node, btree*& root) {
  btree* parent = find_parent(node, root);

  if (!parent) {
    // We must be at a root node, which doesn't have a parent.
    return NULL;
  }

  // Scan through the children on the parent to find the target node's index.
  int child_index = -1;
  int i = 0;
  while (child_index < 0) {
    if (parent->children[i] == node) {
      child_index = i;
    }
    i++;
  }

  // If there is no previous child, there is no previous sibling.
  if (child_index == 0) {
    return NULL;
  }

  return parent->children[child_index - 1];
}

btree* next_sibling(btree* node, btree*& root) {
  btree* parent = find_parent(node, root);

  if (!parent) {
    // We must be at a root node, which doesn't have a parent.
    return NULL;
  }

  // Scan through the children on the parent to find the target node's index.
  int child_index = -1;
  int i = 0;
  while (child_index < 0) {
    if (parent->children[i] == node) {
      child_index = i;
    }
    i++;
  }

  // If there is no next child, there is no next sibling.
  if (child_index == parent->num_keys) {
    return NULL;
  }

  return parent->children[child_index + 1];
}

bool is_minimal(btree* node) {
  bool min = node->num_keys <= (BTREE_ORDER / 2);
  return min;
}

void merge(btree* sib_1, btree* sib_2, btree*& root) {
  btree* parent = find_parent(sib_1, root);
  bool is_leaf = sib_1->is_leaf;

  // Find the indexes of the two siblings so that we can find the
  // separating key.
  int sib_1_index = -1;
  int sib_2_index = -1;
  int i = 0;

  while (sib_1_index < 0 || sib_2_index < 0) {
    if (parent->children[i] == sib_1) {
      sib_1_index = i;
    }

    if (parent->children[i] == sib_2) {
      sib_2_index = i;
    }

    i++;
  }

  int separating_key_index = sib_1_index < sib_2_index ? sib_1_index : sib_2_index;
  int separating_key = parent->keys[separating_key_index];

  int new_keys[BTREE_ORDER];
  btree* new_children[BTREE_ORDER + 1];
  int num_new_keys = sib_1->num_keys + sib_2->num_keys + 1;

  if (sib_1_index > sib_2_index) {
    // Add sib 2 keys, then sep key, then sib 1 keys.
    for (int i = 0; i < sib_2->num_keys; i++) {
      new_keys[i] = sib_2->keys[i];
      if (!is_leaf) {
        new_children[i] = sib_2->children[i];
      }
    }
    if (!is_leaf) {
      new_children[sib_2->num_keys] = sib_2->children[sib_2->num_keys];
    }

    new_keys[sib_2->num_keys] = separating_key;

    for (int i = 0; i < sib_1->num_keys; i++) {
      new_keys[sib_2->num_keys + 1 + i] = sib_1->keys[i];

      if (!is_leaf) {
        new_children[sib_2->num_keys + 1 + i] = sib_1->children[i];
      }
    }

    if (!is_leaf) {
      new_children[num_new_keys] = sib_1->children[sib_1->num_keys];
    }
  } else {
    // Add keys and children from sib_1
    for (int i = 0; i < sib_1->num_keys; i++) {
      new_keys[i] = sib_1->keys[i];

      // Add children if we're not on a leaf.
      if (!is_leaf) {
        new_children[i] = sib_1->children[i];
      }
    }

    // Add final child from sib1
    if (!is_leaf) {
      new_children[sib_1->num_keys] = sib_1->children[sib_1->num_keys];
    }

    // Add the separating key after sib1 keys
    new_keys[sib_1->num_keys] = separating_key;

    // Add sib2's keys and children
    for (int i = 0; i < sib_2->num_keys; i++) {
      new_keys[sib_1->num_keys + 1 + i] = sib_2->keys[i];

      // Add children if we're not on a leaf.
      if (!is_leaf) {
        new_children[sib_1->num_keys + 1 + i] = sib_2->children[i];
      }
    }

    // Add final child from sib2
    if (!is_leaf) {
      new_children[num_new_keys] = sib_2->children[sib_2->num_keys];
    }
  }

  // Reset the number of keys for the node
  sib_1->num_keys = num_new_keys;

  // Add the new keys to the node
  for (int j = 0; j < num_new_keys; j++) {
    // cout << "Setting Key " << new_keys[j] << endl;
    sib_1->keys[j] = new_keys[j];
  }

  // Add the new children to the node
  if (!is_leaf) {
    for (int k = 0; k < num_new_keys; k++) {
      sib_1->children[k] = new_children[k];
    }

    // Add the last child to the node
    sib_1->children[num_new_keys] = new_children[num_new_keys];
  }

  // Shuffle keys and children in parent to remove separating key from parent
  if (parent == root && parent->num_keys < 2) {
    // Set root to sib 1.
    root = sib_1;
    // Delete parent.
    delete parent;
  } else {
    // Move keys and children in parent after separating key down one index.
    // for (int h = parent->num_keys - 1; h > separating_key_index; h--) {
    for (int h = separating_key_index + 1; h < parent->num_keys; h++) {
      parent->keys[h - 1] = parent->keys[h];
      parent->children[h - 1] = parent->children[h];
    }
    parent->num_keys--;
    parent->children[parent->num_keys] = parent->children[parent->num_keys + 1];
  }

  // Delete the useless sibling.
  delete sib_2;
}

void fix_for_removal(btree* node, btree*& root) {
  // If we're descending down into a node for a removal, and it's minimal
  // do some work to make it not minimal.
  btree* prev_sib = prev_sibling(node, root);
  btree* next_sib = next_sibling(node, root);
  bool prev_sib_nonminimal = prev_sib && !is_minimal(prev_sib);
  bool next_sib_nonminimal = next_sib && !is_minimal(next_sib);

  // Case 1: At least one sibling is non-minimal.. rotate!
  if (prev_sib_nonminimal || next_sib_nonminimal) {
    // TODO: Rotate
    return;
  }

  // Case 2: All siblings are minimal... merge!
  if ((!prev_sib || is_minimal(prev_sib)) && (!next_sib || is_minimal(next_sib))) {
    if (next_sib) {
      merge(node, next_sib, root);
    } else {
      merge(node, prev_sib, root);
    }
  }
}

btree* find_successor_node(btree* node, btree*& root, int key) {
  if (node->is_leaf) {
    return NULL;
  }

  int child_index = -1;
  int i = 0;
  while (child_index < 0) {
    if (node->keys[i] == key) {
      child_index = i + 1;
    }
    i++;
  }

  // TODO: fix first child node if minimal
  btree* successor_node = node->children[child_index];

  while (!successor_node->is_leaf) {
    successor_node = successor_node->children[0];
    if (is_minimal(successor_node)) {
      fix_for_removal(successor_node, root);
    }
  }

  if (is_minimal(successor_node)) {
    fix_for_removal(successor_node, root);
  }

  return successor_node;
}

btree* find_predecessor_node(btree* node, btree*& root, int key) {
  if (node->is_leaf) {
    return NULL;
  }

  int child_index = -1;
  int i = 0;
  while (child_index < 0) {
    if (node->keys[i] == key) {
      child_index = i;
    }
    i++;
  }

  // TODO: fix first child node if minimal
  btree* first_child = node->children[child_index];
  if (is_minimal(first_child)) {
    fix_for_removal(first_child, root);
  }

  btree* predecessor_node = first_child;

  while (!predecessor_node->is_leaf) {
    predecessor_node = predecessor_node->children[predecessor_node->num_keys];
    if (is_minimal(predecessor_node)) {
      fix_for_removal(predecessor_node, root);
    }
  }

  if (is_minimal(predecessor_node)) {
    fix_for_removal(predecessor_node, root);
  }

  return predecessor_node;
}

bool node_has_key(btree* node, int key) {
  for (int i = 0; i < node->num_keys; i++) {
    if (node->keys[i] == key) {
      return true;
    }
  }

  return false;
}

void swap_keys(btree* node, int key_to_remove, int key_to_add) {
  for (int i = 0; i < node->num_keys; i++) {
    if (node->keys[i] == key_to_remove) {
      node->keys[i] = key_to_add;
      return;
    }
  }
}

void remove_from_leaf_node(btree* node, int key) {
  bool key_found = false;
  for (int i = 0; i < node->num_keys; i++) {
    if (node->keys[i] == key) {
      key_found = true;
    } else if (key_found) {
      node->keys[i - 1] = node->keys[i];
    }
  }
  node->num_keys--;
}

void remove_from_inner_node(btree* node, btree*& root, int key) {
  // Swap the inner node key with successor/predecessor, then remove the
  // desired key from its new location.

  btree* successor = find_successor_node(node, root, key);
  if (successor) {
    
    if (!node_has_key(node, key)) {
      btree* new_containing_node = find(root, key);
      return remove_from_node(new_containing_node, root, key);
    }

    int successor_key = successor->keys[0];
    swap_keys(node, key, successor_key);
    remove_from_leaf_node(successor, successor_key);
    return;
  }

  btree* predecessor = find_predecessor_node(node, root, key);
  if (predecessor) {
    // Finding a success or pred can change the position of the target key in the tree.
    if (!node_has_key(node, key)) {
      btree* new_containing_node = find(root, key);
      return remove_from_node(new_containing_node, root, key);
    }

    int predecessor_key = predecessor->keys[predecessor->num_keys - 1];
    swap_keys(node, key, predecessor_key);
    remove_from_leaf_node(predecessor, predecessor_key);
  }
}

void remove_from_node(btree* node, btree*& root, int key) {
  if (node->is_leaf) {
    remove_from_leaf_node(node, key);
  } else {
    remove_from_inner_node(node, root, key);
  }
}

void remove(btree*& root, int key) {
  cout << endl << "Removing " << key << endl << endl;

  // If the root has the key, just remove from the root.
  // We don't need to fix root minimalism.
  if (node_has_key(root, key)) {
    remove_from_node(root, root, key);
    return;
  }

  // Gradually step to the node from which we should remove the value.
  btree* traversal_node = root;
  while (!node_has_key(traversal_node, key)) {
    // If we're in a leaf, but still haven't found the key,
    // it doesn't exist in the tree.
    if (traversal_node->is_leaf) {
      return;
    }

    // Find the index of the next child we should traverse to.
    int next_child_index = -1;
    bool next_child_found = false;
    
    for (int i = 0; i < traversal_node->num_keys; i++) {
      if (!next_child_found && traversal_node->keys[i] > key) {
        next_child_index = i;
        next_child_found = true;
      }
    }

    // If the key is bigger than all keys, traverse to the last child.
    if (!next_child_found) {
      next_child_index = traversal_node->num_keys;
    }

    // Repeat traversal for the next child
    traversal_node = traversal_node->children[next_child_index];
    if (is_minimal(traversal_node)) {
      fix_for_removal(traversal_node, root);
    }
  }

  // If we made it here, we know that traversal_node contains the key.
  remove_from_node(traversal_node, root, key);
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

