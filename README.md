# README for B-Tree Homework

In this assignment you'll implement B-Trees. You will probably want to
re-watch the lectures, and get the slides as well because there are
lots of helpful diagrams. There are lots of resources out there on the
Internet about how to understand and implement B-Trees.

By all means, read all you can! But do be aware that there's not 100%
consensus on the terms, so if you read two sentences from two
different sources that seem to contradict one another, it might just
be that they're using different definitions for things.

There is an interactive B-Tree visualizer at this URL: 
https://www.cs.usfca.edu/~galles/visualization/BTree.html

## Points

Like all the other assignments in this course, this one is worth 20
points. However, given the difficulty level, you can score up to 40
points (so yay extra credit).

Getting a 20/20 is super respectable. Getting a 40/20 is possible, but
to be totally honest not many students get there.

## Difficult on purpose

This homework assignment is really hard. That's on purpose! If you
just dive in and start hacking, I promise you won't be able to get all
40 points.

The idea is to design a strategy first, with a careful plan of how
you're going to work the problem. That was the point of the previous
homework assignment.

You'll need helper functions. It is up to you to determine what those
should be, and how (or if) you test them individually. I strongly
recommend writing tests for your helper functions before you even
implement the functions themselves. That way you think through exactly
what they should do, what happens if they're given a NULL input, or a
leaf node, or an overly-full node, or whatever else might be
conceivable in the situation.

## Look at the unit tests

The unit tests that come along with the homework are meant to be
read. There are little hints strewn throughout them. In particular,
the `print_tree` function should be helpful. (Take a look at the
[sanity check] unit test for help with that.)

## Functions to implement

The `btree.h` header file contains all the instructions you'll need
for implementing the functions.

Some things to keep in mind:

`insert` and `remove` must handle cases where adding or removing keys
will grow or shrink the tree. In some cases you'll have to merge nodes
and re-allocate keys; other times you'll have to need to split nodes
and similarly shuffle keys around. Consider having logic to test the
various situations and handle them in their own functions, which are
testable.

Good luck! 

