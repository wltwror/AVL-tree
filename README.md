# Avl tree
Avl tree is self-balancing binary search tree. It is balanced if for each node the difference of the height of the left and right subtree is at most 1.
Long story short this means that operations like insert, delete and search have O(log n) time complexity.

##  Is this useful?
In a way you can think of this AVL implementation as slightly less performant version of std::set (usually implemented as RB tree) with fewer features. 
But it is useful for learning purposes and understanding how self-balancing trees work.
My implementation is clearly not optimal unlike the one in the standard library, but it is functional and can be used for educational purposes.

## Implementation
The implementation is mostly standard but there are some parts I want to go over. AVL tree usually has a height field
in each node. Height is used to determine the balance of a node. But in my implementation I have used a different approach 
(I am pretty sure it works correctly but if you can manage to find a counterexample let me know).
Instead of storing the height of each node I calculate balance after rotations from pre-rotation balance of certain nodes.
The calculation is done relatively to subtree, so I can get away with setting size of one subtree to 2 (the value itself is irrelevant)
in order to simplify the calculation.

First lets discuss the naming of subtrees and nodes. I have used the following naming convention:

### Rotate right
Before right rotation:
```
    x
   / \
  y   C
 / \
A   B
```
After right rotation:
```
    y
   / \
  A   x
     / \
    B   C
```

Balance after rotation is calculated as follows:
```
A = 2
// as mentioned above the value of A is not relevant, this value could be different and the result would be the same
B = A + balance of y
C = max(A,B) + 1 + balance of x
// using those we can get balance after rotation
balance of x after rotation = C - B
balance of y after rotation = max(B,C) + 1 - A
```
### Rotate left
Before left rotation:
```
    x
   / \
  A   y
     / \
    B   C
```
After left rotation:
```
    y
   / \
  x   C
 / \
A   B
```
Balance after rotation is calculated as follows:
```
C = 2
// as mentioned above the value of C is not relevant, this value could be different and the result would be the same
B = C - balance of y
A = max(B,C) + 1 - balance of x
// using those we can get balance after rotation
balance of x after rotation = B - A
balance of y after rotation = C - (max(A,B) + 1)
```
## LR rotation
LR rotation is a combination of left and right rotation. It is used when simple rotation is not enough to balance the tree.
```
    x
   / \
  y   D
 / \
A   z
   / \
  B   C
```
After the rotation:
```
    z
   / \
  y   x
 / \ / \
A  B C  D

```
## RL rotation
RL rotation is a combination of right and left rotation. It is used when simple rotation is not enough to balance the tree.
```
    x
   / \
  A   y
     / \
    z   D
   / \
  B   C
```
After the rotation:
```
        z
       / \
      x   y
     / \ / \
    A  B C  D
```

## Testing
I have tested the implementation with decent tests. Bugs are possible but not likely. Tests are not included
because they are not public (and not created by me).

