#pragma once
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ostream>
#include <utility>
#include <cassert>
#include <cstdarg>
#include <iostream>
#include <memory>


inline int sgn(const int number){
    if (number > 0){
        return 1;
    }else if ( number < 0){
        return -1;
    }else{
        return 0;
    }
}


template < typename T >
struct SNode{
    SNode(const T & val, std::weak_ptr<SNode> par){
        value = val;
        left = nullptr;
        right = nullptr;
        if( !par.expired() ){
            parent = par;
        }else{
            parent.reset();
        }
        balance = 0;
    }

    SNode(const T & val){
        value = val;
        left = nullptr;
        right = nullptr;
        parent.reset();
        balance = 0;
    }

    void set_to (const std::shared_ptr<SNode<T>> & other, const std::shared_ptr<SNode<T>> this_ptr){
        // sets to different node, setting parents needs to be handled separately
        this->value = other->value;
        this->balance = other->balance;
        this->left = other->left;
        this->right = other->right;
        this->parent = other->parent;
        if(this->left){
            this->left->parent = this_ptr;
        }
        if(this->right){
            this->right->parent = this_ptr;
        }
    }
    T value;
    std::shared_ptr<SNode> left;
    std::shared_ptr<SNode> right;
    std::weak_ptr<SNode> parent;
    int balance;
};

template < typename T >
class CAvlTree {
    public:
        CAvlTree(){
            m_root = nullptr;
            m_size = 0;
        }

        size_t size() const{
            return m_size;
        }

        const T* find(const T& value) const{
            auto element = rec_find( value, m_root );
            if ( element == nullptr ){
                return nullptr;
            }
            return &(element->value);
        }

        bool insert(const T& value){
            if (m_size == 0){
                m_size++;
                SNode<T> n (value);
                m_root = std::make_shared<SNode<T>>(n);
                return true;
            }
            if (rec_insert(value, m_root)){
                m_size++;
                return true;
            }
            return false;
        }

        bool erase(const T& value){
            auto element = rec_find( value, m_root );
            if ( element == nullptr ){
                return false;
            }
            m_size--;
            auto erased = rec_erase(m_root, value);
            m_root = erased.first;
            int * direction = &(erased.second);
            AVL_check_balance(element, false,direction);
            return true;
        }

        std::shared_ptr<SNode<T>> bst_min ( const std::shared_ptr<SNode<T>> starting_node ) const {
            if (starting_node == nullptr){
                return nullptr;
            }else if (starting_node->left == nullptr){
                return starting_node;
            }else{
                return bst_min(starting_node->left);
            }
        }

        void bst_show( const std::shared_ptr<SNode<T>> current_node, bool debug_mode = false ) const{
            if ( current_node == nullptr ){
                return;
            }
            bst_show(current_node->left, debug_mode);
            if(debug_mode){
                std::cout << "show: " << current_node->value << " with balance: " << current_node->balance << std::endl;
            }else{
                std::cout << current_node->value << ", ";
            }
            bst_show(current_node->right, debug_mode);
        }

        std::shared_ptr<SNode<T>> get_root()const{
            return m_root;
        }


    private:
        std::shared_ptr<SNode<T>>  m_root;
        size_t m_size;


        const std::shared_ptr<SNode<T>> rec_find( const T& value, const std::shared_ptr<SNode<T>>  current_node) const{
            if ( current_node->value > value ) {
                // go left
                if( current_node->left == nullptr ){
                    return nullptr;
                }
                return rec_find(value, current_node->left);

            }else if ( current_node->value < value ){
                // go right
                if( current_node->right == nullptr ){
                    return nullptr;
                }
                return rec_find(value, current_node->right);

            }else{
                // value found
                return current_node;
            }
        }

        std::shared_ptr<SNode<T>> set_node(std::shared_ptr<SNode<T>> new_node, const T& value, const std::shared_ptr<SNode<T>>  current_node )const{
            // creates a new node and sets its variables
            auto parent = std::weak_ptr<SNode<T>>(current_node);
            SNode<T> n (value, parent);
            new_node = std::make_shared<SNode<T>>(n);
            return new_node;
        }

        bool rec_insert ( const T& value, const std::shared_ptr<SNode<T>>  current_node ){
            if ( current_node->value > value ){
                // go left
                if (current_node->left == nullptr){
                    // insert left
                    auto new_node = set_node(current_node->left, value, current_node );
                    current_node->left = new_node;
                    AVL_check_balance(new_node,true);
                    return true;
                }
                return rec_insert(value, current_node->left);
            }
            else if ( current_node->value < value ){
                // go right
                if (current_node->right == nullptr){
                    // insert right
                    auto new_node = set_node(current_node->right, value, current_node );
                    current_node->right = new_node;
                    AVL_check_balance(new_node,true);
                    return true;
                }
                return rec_insert(value, current_node->right);
            }
            return false;
        }

        void swapWith(std::shared_ptr<SNode<T>> & current_node, std::shared_ptr<SNode<T>> & other_node){
            // swap contents of two nodes
            SNode<T> node(current_node->value,current_node->parent);
            std::shared_ptr<SNode<T>> temp = std::make_shared<SNode<T>>(node);
            temp->right = current_node->right;
            temp->left = current_node->left;
            temp->balance = current_node->balance;

            current_node->set_to(other_node, current_node);
            if ( other_node->parent.lock() != current_node ){
                if( other_node->parent.lock()->left == other_node){
                    other_node->parent.lock()->left = current_node;
                }else{
                    other_node->parent.lock()->right = current_node;
                }
            }
            other_node->set_to(temp,other_node);
            if(m_root == current_node){
                m_root = other_node;
                other_node->parent.reset();
            }else{
                if ( temp->parent.lock()->left == current_node ){
                    temp->parent.lock()->left = other_node;
                }else{
                    temp->parent.lock()->right = other_node;
                }
            }
            if ( current_node->parent.lock() == current_node){ // if other was right son of current
                current_node->parent = other_node;
                other_node->right = current_node;
            }

        }

        std::pair<std::shared_ptr<SNode<T>>,int> rec_erase (std::shared_ptr<SNode<T>> current_node, const T& value){
            // returns node and direction of erase
            int direction;
            if (current_node == nullptr){
                return std::make_pair(nullptr,0);
            }else if (value < current_node->value){
                auto newLeft = rec_erase(current_node->left, value);
                current_node->left = newLeft.first;
                if (newLeft.first){
                    newLeft.first->parent = current_node;
                }
                direction = newLeft.second;

            }else if (value > current_node->value){
                auto newRight = rec_erase(current_node->right, value);
                current_node->right = newRight.first;
                if (newRight.first){
                    newRight.first->parent = current_node;
                }
                direction = newRight.second;

            }else {
                if (current_node->parent.lock() == nullptr){
                    direction = 0;
                }
                else if ( current_node->parent.lock()->left == current_node ){
                    direction = -1;
                }else{
                    direction = 1;
                }
                if ( current_node->left == nullptr && current_node->right == nullptr ){
                    return std::make_pair(nullptr,direction);
                }else if (current_node->left == nullptr){
                    current_node->right->parent.lock() = current_node->parent.lock();
                    return std::make_pair(current_node->right,direction);

                }else if (current_node->right == nullptr){
                    current_node->left->parent.lock() = current_node->parent.lock();
                    return std::make_pair(current_node->left,direction);
                }
                auto w = bst_min(current_node->right);
                current_node->value = w->value;
                swapWith(current_node, w);
                auto newRight = rec_erase(w->right, current_node->value);
                w->right = newRight.first;
                if(newRight.first){
                    newRight.first->parent = w;
                }
                direction = newRight.second;
                return std::make_pair(w,direction);
            }
            return std::make_pair(current_node,direction);
        }

        // rotations
        std::shared_ptr<SNode<T>> rotateLeft(std::shared_ptr<SNode<T>> x){
            auto y = x->right;
            x->right = y->left;


            // precalculate balance
            int preBalanceX;
            int preBalanceY;
            int C = 2;
            int B = C - y->balance;
            int A = std::max(B,C) + 1 - x->balance;
            preBalanceX = B - A;
            preBalanceY = C - (std::max(A,B) + 1);

            y->left = x;
            if ( x->right != nullptr ) {
                x->right->parent = x;
            }
            y->parent = x->parent;
            if (x == m_root){
                m_root = y;
                y->parent.reset();

            }else{
                if (x->parent.lock()->right == x){
                    x->parent.lock()->right = y;
                }else{
                    x->parent.lock()->left= y;
                }
            }
            x->parent = y;
            x->balance = preBalanceX;
            y->balance = preBalanceY;
            return y;
        }

        std::shared_ptr<SNode<T>> rotateRight(std::shared_ptr<SNode<T>> x){
            auto y = x->left;
            x->left = y->right;

            // precalculate balance
            int preBalanceX;
            int preBalanceY;
            int A = 2;
            int B = A + y->balance;
            int C = std::max(A,B) + 1 + x->balance;
            preBalanceX = C - B;
            preBalanceY = (std::max(B,C) + 1) - A;

            y->right = x;
            if ( x->left != nullptr ){
                x->left->parent = x;
            }
            y->parent = x->parent;
            if( x == m_root ){
                m_root = y;
                y->parent.reset();
            }else{
                if (x->parent.lock()->right == x){
                    x->parent.lock()->right = y;
                }else{
                    x->parent.lock()->left= y;
                }
            }
            x->parent = y;
            x->balance = preBalanceX;
            y->balance = preBalanceY;
            return y;
        }

        std::shared_ptr<SNode<T>> rotateLR (std::shared_ptr<SNode<T>> x){
            auto y = x->left;
            auto z = y->right;
            rotateLeft(y);
            rotateRight(x);
            return z;
        }


        std::shared_ptr<SNode<T>> rotateRL (std::shared_ptr<SNode<T>> x){
            auto y = x->right;
            auto z = y->left;
            rotateRight(y);
            rotateLeft(x);
            return z;
        }


        void AVL_check_balance(std::shared_ptr<SNode<T>> current_node, bool insert = true, int * direction = nullptr){
            auto parent = current_node->parent.lock();
            // is root
            if ( current_node == m_root ){
                return;
            }
            auto original_current = current_node;
            int operation_multipilier;
            int side_multiplier;
            if (insert){
                operation_multipilier = 1;
            }else{
                operation_multipilier = -1;
            }

            // is left son
            if ( direction != nullptr){
                side_multiplier = *direction;
            }
            else if ( parent->left == current_node){
                side_multiplier = -1;
            }
                // is right son
            else{
                side_multiplier = 1;
            }
            parent->balance += 1 * operation_multipilier * side_multiplier;
            if ( insert ){
                if (parent->balance == 0){
                    return;
                }else if ( std::abs(parent->balance) < 2 ){
                    AVL_check_balance(parent,insert);
                }else {
                    // rotate
                    if (sgn(parent->balance) == sgn(current_node->balance)){
                        // simple rotation
                        if(side_multiplier == 1){
                            // rotate left
                            current_node = rotateLeft(parent);
                        }else{
                            // rotate right
                            current_node = rotateRight(parent);
                        }

                    }else{
                        // double rotation
                        if(side_multiplier == 1){
                            // rotate RL
                            current_node = rotateRL(parent);
                        }else{
                            // rotate LR
                            current_node = rotateLR(parent);
                        }
                    }
                }
                return;
            }
            else{
                if (parent->balance == 0){
                    // continue
                }else if ( abs(parent->balance) == 1 ){
                    return;
                }else{
                    // rotate

                    // sets current to correct node for comparison
                    if ( side_multiplier == 1  ){
                        current_node = parent->left;
                    }else if ( side_multiplier == -1 ){
                        current_node = parent->right;
                    }

                    if ( sgn(parent->balance) == sgn(current_node->balance) ){
                        if( side_multiplier == 1){
                            // rotate right
                            current_node = rotateRight(parent);
                        }else{
                            // rotate left
                            current_node = rotateLeft(parent);
                        }
                    }else if ( current_node->balance == 0 ){
                        if( side_multiplier == 1){
                            // rotate right
                            current_node = rotateRight(parent);
                        }else{
                            // rotate left
                            current_node = rotateLeft(parent);
                        }
                        return;
                    }else{
                        if( side_multiplier == 1){
                            // rotate LR
                            current_node = rotateLR(parent);
                        }else{
                            // rotate RL
                            current_node = rotateRL(parent);
                        }
                    }
                }

                if (original_current == current_node){
                    parent = current_node->parent.lock();
                }else{
                    parent = current_node;
                }
                AVL_check_balance(parent,false);
            }
        }
};