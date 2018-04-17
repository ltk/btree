//
// btree_test.cpp
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "btree.h"
#include "btree_unittest_help.h"
#include <iostream>
#include <vector>

using namespace std;

// Test cases - study these, and the helper functions in btree_unittest_help.cpp.

TEST_CASE("B-Tree: Sanity Check", "[sanity]") {
  // run this one with the following command line:
  // ./btree_test "[sanity]"
  btree* small = build_small();
  cout << "print_tree writes something like: \"graph btree{ ... }\" to stdout." << endl;
  cout << "use webgraphviz.com to turn that into diagrams of your tree." << endl << endl;
  print_tree(small);
  cout << endl;

  REQUIRE(check_tree(small));
  REQUIRE(private_contains(small, 13));
  REQUIRE_FALSE(private_contains(small, 14));

  btree* broken = build_broken(); // invariant should fail
  REQUIRE_FALSE(check_tree(broken)); // be sure we catch that
}


TEST_CASE("B-Tree: Report number of nodes", "[count nodes]") {
  btree* empty = build_empty();
  REQUIRE(count_nodes(empty) == 1); // not zero, since there's a root node

  btree* small = build_small();
  REQUIRE(count_nodes(small) == 4);

  btree* two_thin = build_two_tier();
  REQUIRE(count_nodes(two_thin) == 5);

  btree* two_full = build_full_two_tier();
  REQUIRE(count_nodes(two_full) == 6);

  btree* thrice = build_thin_three_tier();
  REQUIRE(count_nodes(thrice) == 9);
}

TEST_CASE("B-Tree: Report number of keys", "[count keys]") {
  btree* empty = build_empty();
  REQUIRE(count_keys(empty) == 0);

  btree* small = build_small();
  REQUIRE(count_keys(small) == 8);

  btree* two_thin = build_two_tier();
  REQUIRE(count_keys(two_thin) == 14);

  btree* two_full = build_full_two_tier();
  REQUIRE(count_keys(two_full) == 19);

  btree* thrice = build_thin_three_tier();
  REQUIRE(count_keys(thrice) == 17);
}

TEST_CASE("B-Tree: Find present key in leaf", "[find present leaf]") {
  btree* small = build_small();
  btree* node;
  
  node = find(small, 17);
  REQUIRE(node == small->children[1]);

  node = find(small, 2);
  REQUIRE(node == small->children[0]);

  node = find(small, 28);
  REQUIRE(node == small->children[2]);
}

TEST_CASE("B-Tree: Find present key in internal node", "[find present intnl]") {
  btree* thrice = build_thin_three_tier();
  btree* node;

  node = find(thrice, 7);
  REQUIRE(node == thrice->children[0]);

  node = find(thrice, 17);
  REQUIRE(node == thrice->children[1]);
}

TEST_CASE("B-Tree: Find present key in root", "[find present root]") {
  btree* small = build_small();
  btree* node;
  
  node = find(small, 10);
  REQUIRE(node == small);

  node = find(small, 20);
  REQUIRE(node == small);
}

TEST_CASE("B-Tree: Find not present key", "[find not present]") {
  btree* small = build_small();
  btree* node;
  
  node = find(small, 6);
  REQUIRE(node == small->children[0]);

  node = find(small, 15);
  REQUIRE(node == small->children[1]);

  node = find(small, 21);
  REQUIRE(node == small->children[2]);
  REQUIRE(check_tree(small));
}

TEST_CASE("B-Tree: Insert key into empty root", "[ins root empty]") {
  btree* empty = build_empty();
  insert(empty, 42);
  REQUIRE(check_tree(empty));
  REQUIRE(private_contains(empty, 42));
}

TEST_CASE("B-Tree: Insert key into semifull root", "[ins root semifull]") {
  btree* semi = build_semifull();
  insert(semi, 42);
  REQUIRE(check_tree(semi));
  REQUIRE(private_contains(semi, 10));
  REQUIRE(private_contains(semi, 30));
  REQUIRE(private_contains(semi, 42));

  insert(semi, 7);
  REQUIRE(check_tree(semi));
  REQUIRE(private_contains(semi, 7));
  REQUIRE(private_contains(semi, 10));
  REQUIRE(private_contains(semi, 30));
  REQUIRE(private_contains(semi, 42));
}

TEST_CASE("B-Tree: Insert key into full root", "[ins root full]") {
  btree* full = build_full_leaf_root();
  insert(full, 15);
  REQUIRE(check_tree(full));

  // root should have split and given another level of height
  int height = 0;
  bool leaves_ok = check_height(full, height);
  REQUIRE(leaves_ok);
  REQUIRE(height == 1);
}

TEST_CASE("B-Tree: Insert key into semifull leaf node", "[ins leaf semifull]") {
  // get a tree with semifull leaf
  btree* semi = build_two_tier();
  int height = 0;
  bool leaves_ok = check_height(semi, height);
  REQUIRE(height == 1);
  REQUIRE(leaves_ok);
  
  // insert a key that would put it in the right spot
  insert(semi, 4); // should add to child[0]
  REQUIRE(check_tree(semi));
  
  // check that height did not change
  leaves_ok = check_height(semi, height);
  REQUIRE(height == 1);
  REQUIRE(leaves_ok);

  // check that key is now present
  REQUIRE(private_contains(semi->children[0], 4));

  insert(semi, 24); // should add to child[2]
  REQUIRE(check_tree(semi));
  leaves_ok = check_height(semi, height);
  REQUIRE(height == 1);
  REQUIRE(leaves_ok);
  REQUIRE(private_contains(semi->children[2], 24));

  insert(semi, 40); // should add to child[3]
  REQUIRE(check_tree(semi));
  leaves_ok = check_height(semi, height);
  REQUIRE(height == 1);
  REQUIRE(leaves_ok);
  REQUIRE(private_contains(semi->children[3], 40));
}

TEST_CASE("B-Tree: Insert key into full leaf node", "[ins leaf full]") {
  btree* semi = build_two_tier();
  int height = 0;
  bool leaves_ok = check_height(semi, height);
  REQUIRE(height == 1);
  REQUIRE(leaves_ok);
  
  // insert a key that should split child[1]
  insert(semi, 18); // should add to child[0]
  REQUIRE(check_tree(semi));

  // shouldn't have raised height; parent of insert site had room
  leaves_ok = check_height(semi, height);
  REQUIRE(height == 1);
  REQUIRE(leaves_ok);  

  // key 17 should have moved to the root
  REQUIRE(private_contains(semi, 17));

  // key 15 should be in child[1]
  REQUIRE(private_contains(semi->children[1], 15));

  // and key 19 shoudl be in child[2]
  REQUIRE(private_contains(semi->children[2], 19));

  // remember if you're having trouble with this, you can always hack
  // this test file and put some print_tree calls at the trouble
  // spots, then recompile and run to troubleshoot.
}

TEST_CASE("B-Tree: Remove not present key from empty tree", "[rm not present empty]") {
  btree* semi = build_empty();
  remove(semi, 28); // should have no effect
  REQUIRE(check_tree(semi));
  REQUIRE_FALSE(private_contains(semi, 28)); // no idea why this would be the case
}

TEST_CASE("B-Tree: Remove key from non-empty root", "[rm root not empty]") {
  btree* full = build_full_leaf_root();
  REQUIRE(private_contains(full, 30));
  remove(full, 30);
  REQUIRE(check_tree(full));
  REQUIRE_FALSE(private_contains(full, 30));
}

TEST_CASE("B-Tree: Remove not present key from leaf", "[rm not present leaf]") {
  btree* semi = build_two_tier();
  remove(semi, 28); // should have no effect
  REQUIRE(check_tree(semi));
  REQUIRE_FALSE(private_contains(semi, 28)); // no idea why this would be the case
}

TEST_CASE("B-Tree: Remove key from leaf with full siblings", "[rm leaf sibs full]") {
  btree* semi = build_two_tier();
  remove(semi, 27); // should cause a rotate right involving parent and sibling to left
  REQUIRE(check_tree(semi));

  // height should remain at 1
  int height = 0;
  bool leaves_ok = check_height(semi, height);
  REQUIRE(height == 1);
  REQUIRE(leaves_ok);

  // ensure no node has 27
  REQUIRE_FALSE(private_search_all(semi, 27));
}

TEST_CASE("B-Tree: Remove key from leaf with at-min-capactiy siblings", "[rm leaf sibs mincap]") {
  btree* thrice = build_thin_three_tier();
  int height = 0;
  bool leaves_ok = check_height(thrice, height);
  REQUIRE(height == 2); // just a sanity check
  REQUIRE(leaves_ok);

  remove(thrice, 1); // rm 1, smallest value. should result in shorter tree
  leaves_ok = check_height(thrice, height);
  REQUIRE(height == 1);
  REQUIRE(leaves_ok);
  REQUIRE(check_tree(thrice));
  REQUIRE_FALSE(private_search_all(thrice, 1));

  // reset, do it again but remove 16. this one is on the inside.
  thrice = build_thin_three_tier();
  remove(thrice, 16);
  leaves_ok = check_height(thrice, height);
  REQUIRE(height == 1);
  REQUIRE(leaves_ok);
  REQUIRE(check_tree(thrice));
  REQUIRE_FALSE(private_search_all(thrice, 16));

  // again, remove 26, this is the largest key in the tree
  thrice = build_thin_three_tier();
  remove(thrice, 26);
  leaves_ok = check_height(thrice, height);
  REQUIRE(height == 1);
  REQUIRE(leaves_ok);
  REQUIRE(check_tree(thrice));
  REQUIRE_FALSE(private_search_all(thrice, 26));
}

TEST_CASE("B-Tree: Remove key from internal node with at-min-capacity siblings", "[rm intnl sibs mincap]") {
  btree* thrice = build_thin_three_tier();
  int height = 0;
  bool leaves_ok = check_height(thrice, height);
  REQUIRE(height == 2); // just a sanity check
  REQUIRE(leaves_ok);

  remove(thrice, 4); // rm 4, a key in an internal node.
  leaves_ok = check_height(thrice, height);
  REQUIRE(height == 1); // should have shrunk tree by one
  REQUIRE(leaves_ok);
  REQUIRE(check_tree(thrice));
  REQUIRE_FALSE(private_search_all(thrice, 4));

  thrice = build_thin_three_tier();
  remove(thrice, 24); // rm 24, a key in an internal node.
  leaves_ok = check_height(thrice, height);
  REQUIRE(height == 1); // should have shrunk tree by one
  REQUIRE(leaves_ok);
  REQUIRE(check_tree(thrice));
  REQUIRE_FALSE(private_search_all(thrice, 24));  
}
