#pragma once
#pragma once
#include "Token.h"
#include <stdexcept>
#include <vector>

using std::logic_error;
using std::vector;

class EGLnode;

bool operator==(const EGLnode& op1, const EGLnode& op2);
bool operator!=(const EGLnode& op1, const EGLnode& op2);
bool operator< (const EGLnode& op1, const EGLnode& op2);
bool operator> (const EGLnode& op1, const EGLnode& op2);

class EGLnode
{
public:
    EGLnode                         (): token_(EGLtoken()), child_vec_() {}
	EGLnode(const EGLtoken& token_set): token_ (token_set), child_vec_() {}

    ~EGLnode();
    
    void clear();
    
    void move           (EGLnode& set); 
    void copy_tree(const EGLnode& set);
    
    void push                (EGLnode& node_set);
    void push         (const EGLtoken&  tok_set);
    void set_as_parent(const EGLtoken&  tok_set);

          EGLtoken& token()       { return token_; }
    const EGLtoken& token() const { return token_; }
    
          vector<EGLnode*>& child_vec()       { return child_vec_; }
    const vector<EGLnode*>& child_vec() const { return child_vec_; }

          EGLnode&  next()       { return *child_vec_.front(); }
    const EGLnode&  next() const { return *child_vec_.front(); }
          EGLnode&  left()       { return *child_vec_.front(); }
    const EGLnode&  left() const { return *child_vec_.front(); }
          EGLnode& right()       { return *child_vec_.back(); }
    const EGLnode& right() const { return *child_vec_.back(); }

          EGLnode& operator[](size_t elm_index)       { return *child_vec_[elm_index]; }
    const EGLnode& operator[](size_t elm_index) const { return *child_vec_[elm_index]; }

    const size_t child_vec_sz() { return child_vec_.size(); }

private:
    EGLnode           (EGLnode& node_set): token_(node_set.token_), child_vec_(node_set.child_vec_) {}
    EGLnode& operator=(EGLnode& node_set) { return EGLnode(node_set); }
    
	EGLtoken token_;
    vector<EGLnode*> child_vec_; 
};

bool operator==(const EGLnode& op1, const EGLnode& op2)
{
    if (op1.token()            != op2.token())            return false;
    if (op1.child_vec().size() != op2.child_vec().size()) return false;
    
    for (auto it1 = op1.child_vec().cbegin(), it2 = op2.child_vec().cbegin(); 
              it1 < op1.child_vec().cend() && it2 < op2.child_vec().cend(); it1++, it2++)
    {
        if (*(*it1) != *(*it2)) return false;
    }

    return true;
}

bool operator!=(const EGLnode& op1, const EGLnode& op2)
{
    return !(op1 == op2);    
}

bool operator<(const EGLnode& op1, const EGLnode& op2)
{
    if (op1.token()            > op2.token())            return false;
    if (op1.child_vec().size() > op2.child_vec().size()) return false;
    
    for (auto it1 = op1.child_vec().cbegin(), it2 = op2.child_vec().cbegin(); 
              it1 < op1.child_vec().cend() && it2 < op2.child_vec().cend(); it1++, it2++)
    {
        if (*(*it1) > *(*it2)) return false;
    }

    return true;
}

bool operator>(const EGLnode& op1, const EGLnode& op2)
{
    if (op1.token()            < op2.token())            return false;
    if (op1.child_vec().size() < op2.child_vec().size()) return false;
    
    for (auto it1  = op1.child_vec().cbegin(), it2  = op2.child_vec().cbegin(); 
              it1 != op1.child_vec().cend() && it2 != op2.child_vec().cend(); it1++, it2++)
    {
        if (*(*it1) < *(*it2)) return false;
    }

    return true;
}

EGLnode::~EGLnode()
{
    clear();
}

void EGLnode::clear()
{ 
    token_ = EGLtoken();

    for (EGLnode* child_ptr : child_vec_) delete child_ptr;
}
    
void EGLnode::move(EGLnode& set) 
{ 
    EGLtoken         copy_tok =           set.token_; set.token_ = EGLtoken();
    vector<EGLnode*> copy_vec = std::move(set.child_vec_);
    
    for (EGLnode* child_ptr : child_vec_) delete child_ptr;
    
    token_     =           copy_tok;
    child_vec_ = std::move(copy_vec);
}

void EGLnode::copy_tree(const EGLnode& set)
{
    token_ = set.token_;
        
    for (EGLnode* child_ptr : child_vec_) delete child_ptr;

    child_vec_.clear();
    
    for (EGLnode* child_ptr : set.child_vec_)
    {
        child_vec_.push_back(new EGLnode);
        child_vec_.back()->copy_tree(*child_ptr);
    }
}

void EGLnode::push(EGLnode& node_set)
{
    child_vec_.push_back(new EGLnode);
    child_vec_.back()->token_     =           node_set.token_; node_set.token_ = EGLtoken();
    child_vec_.back()->child_vec_ = std::move(node_set.child_vec_);
}

void EGLnode::push(const EGLtoken& tok_set)
{
    child_vec_.push_back(new EGLnode(tok_set));
}

void EGLnode::set_as_parent(const EGLtoken& tok_set)
{
    EGLnode copy_node;
    copy_node.move(*this);
    this->move(EGLnode(tok_set));
    this->push(copy_node);
}
