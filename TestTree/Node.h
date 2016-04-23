#pragma once
#pragma once
#include "Token.h"
#include <stdexcept>
#include <list>

using std::logic_error;
using std::list;

class EGLnode;

bool operator==(const EGLnode& op1, const EGLnode& op2);
bool operator!=(const EGLnode& op1, const EGLnode& op2);
bool operator< (const EGLnode& op1, const EGLnode& op2);
bool operator> (const EGLnode& op1, const EGLnode& op2);

void egl_nodeSetFull(EGLnode& root_node, EGLopType opType);
void egl_nodeSetOpt (EGLnode& root_node);

class EGLnode
{
public:
    EGLnode                         (): token_(EGLtoken()), child_lst_() {}
	EGLnode(const EGLtoken& token_set): token_ (token_set), child_lst_() {}

    ~EGLnode();
    
    void clear();
    
    void move           (EGLnode& set); 
    void copy_tree(const EGLnode& set);
    
    void merge_with          (EGLnode& node_set);
    void push                (EGLnode& node_set);
    void push         (const EGLtoken&  tok_set);
    void set_as_parent(const EGLtoken&  tok_set);
    
          EGLtoken& token()       { return token_; }
    const EGLtoken& token() const { return token_; }
    
          list<EGLnode*>& child_lst()       { return child_lst_; }
    const list<EGLnode*>& child_lst() const { return child_lst_; }

          EGLnode&  next()       { return *child_lst_.front(); }
    const EGLnode&  next() const { return *child_lst_.front(); }
          EGLnode&  left()       { return *child_lst_.front(); }
    const EGLnode&  left() const { return *child_lst_.front(); }
          EGLnode& right()       { return *child_lst_.back(); }
    const EGLnode& right() const { return *child_lst_.back(); }

private:
    EGLnode          (EGLnode& node_set): token_(node_set.token_), child_lst_(node_set.child_lst_) {}
    EGLnode operator=(EGLnode& node_set) { return EGLnode(node_set); }
    
	EGLtoken token_;
    list<EGLnode*> child_lst_; 
};

bool operator==(const EGLnode& op1, const EGLnode& op2)
{
    if (op1.token()            != op2.token())            return false;
    if (op1.child_lst().size() != op2.child_lst().size()) return false;
    
    for (auto it1  = op1.child_lst().cbegin(), it2  = op2.child_lst().cbegin(); 
              it1 != op1.child_lst().cend() && it2 != op2.child_lst().cend(); it1++, it2++)
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
    if (op1.child_lst().size() > op2.child_lst().size()) return false;
    
    for (auto it1  = op1.child_lst().cbegin(), it2  = op2.child_lst().cbegin(); 
              it1 != op1.child_lst().cend() && it2 != op2.child_lst().cend(); it1++, it2++)
    {
        if (*(*it1) > *(*it2)) return false;
    }

    return true;
}

bool operator>(const EGLnode& op1, const EGLnode& op2)
{
    if (op1.token()            < op2.token())            return false;
    if (op1.child_lst().size() < op2.child_lst().size()) return false;
    
    for (auto it1  = op1.child_lst().cbegin(), it2  = op2.child_lst().cbegin(); 
              it1 != op1.child_lst().cend() && it2 != op2.child_lst().cend(); it1++, it2++)
    {
        if (*(*it1) < *(*it2)) return false;
    }

    return true;
}

void egl_nodeSetFull(EGLnode& root_node, EGLopType opType)
{
    if (root_node.token().type               != TOK_OPERATOR ||
        root_node.token().value.operator_val != opType)
        root_node.set_as_parent(OperatorToken  (opType));
}

void egl_nodeSetOpt(EGLnode& root_node)
{
    if (root_node.token().type               != TOK_OPERATOR ||
       (root_node.token().value.operator_val != OP_BINARY_SUB &&
        root_node.token().value.operator_val != OP_BINARY_DIV)) return;
        
    if      (root_node. left().child_lst().size() == 1) root_node. left().move(root_node. left().left());
    if      (root_node.right().child_lst().size() == 1) root_node.right().move(root_node.right().left());
    else if (root_node.right().child_lst().empty())     root_node.        move(root_node. left());
}

EGLnode::~EGLnode()
{
    clear();
}

void EGLnode::clear()
{ 
    token_ = EGLtoken();

    for (EGLnode* child_ptr : child_lst_) delete child_ptr;
}
    
void EGLnode::move(EGLnode& set) 
{ 
    EGLtoken       copy_tok =           set.token_; set.token_ = EGLtoken();
    list<EGLnode*> copy_lst = std::move(set.child_lst_);
    
    for (EGLnode* child_ptr : child_lst_) delete child_ptr;
    
    token_     =           copy_tok;
    child_lst_ = std::move(copy_lst);
}

void EGLnode::copy_tree(const EGLnode& set)
{
    token_ = set.token_;
        
    for (EGLnode* child_ptr : child_lst_) delete child_ptr;

    child_lst_.clear();
    
    for (EGLnode* child_ptr : set.child_lst_)
    {
        child_lst_.push_back(new EGLnode);
        child_lst_.back()->copy_tree(*child_ptr);
    }
}

void EGLnode::merge_with(EGLnode& node_set)
{
    child_lst_.splice(child_lst_.cend(), std::move(node_set.child_lst()));
}

void EGLnode::push(EGLnode& node_set)
{
    child_lst_.push_back(new EGLnode);
    child_lst_.back()->token_     =           node_set.token_; node_set.token_ = EGLtoken();
    child_lst_.back()->child_lst_ = std::move(node_set.child_lst_);
}

void EGLnode::push(const EGLtoken& tok_set)
{
    child_lst_.push_back(new EGLnode(tok_set));
}

void EGLnode::set_as_parent(const EGLtoken& tok_set)
{
    EGLnode copy_node;
    copy_node.move(*this);
    this->move(EGLnode(tok_set));
    this->push(copy_node);
}
