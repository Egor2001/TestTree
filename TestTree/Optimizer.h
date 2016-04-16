#pragma once

#include "Node.h"

#include <string>
#include <stdexcept>

using std::string;
using std::logic_error;

class EGLoptimizer
{
public:
    EGLoptimizer() {}
 
    ~EGLoptimizer() {}

    void operator()(EGLnode& root_node) { optNode(root_node); }

private:
    EGLoptimizer           (const EGLoptimizer& set) {}
    EGLoptimizer& operator=(const EGLoptimizer& set) { return EGLoptimizer(); }

    void optNode (EGLnode& root_node);
    void optOper (EGLnode& root_node);
    void optEqual(EGLnode& root_node);

    void optBinSub  (EGLnode& root_node);
    void optBinDiv  (EGLnode& root_node);
    void optBinPow  (EGLnode& root_node);
    void optGroupSum(EGLnode& root_node);
    void optGroupMul(EGLnode& root_node);
};

void EGLoptimizer::optNode(EGLnode& root_node)
{
    for (EGLnode* child_ptr : root_node.child_lst()) optNode(*child_ptr);

    if (root_node.token().type == TOK_OPERATOR) optOper(root_node);
}

void EGLoptimizer::optOper(EGLnode& root_node)
{                                                                     
    if (root_node.token().type != TOK_OPERATOR)
        throw logic_error("Expected operator");

    switch (root_node.token().value.operator_val)
    {
        case OP_UNARY_PLUS: root_node.move(*root_node.left()); break;
        case OP_UNARY_MINUS:
        {
            if (root_node.left()->token().type == TOK_CONSTANT)
            {
                root_node.left()->token().value.constant_val *= -1;
                root_node.move(*root_node.left());
            }
        }
        break;
        
        case OP_BINARY_SUB: optBinSub  (root_node); break;
        case OP_BINARY_DIV: optBinDiv  (root_node); break;
        case OP_BINARY_POW: optBinPow  (root_node); break;
        case OP_GROUP_SUM:  optGroupSum(root_node); break;
        case OP_GROUP_MUL:  optGroupMul(root_node); break;

        default: throw logic_error("Invalid operator type"); break;
    }
}

void EGLoptimizer::optEqual(EGLnode& root_node)
{
}

void EGLoptimizer::optBinSub(EGLnode& root_node)
{
    if (root_node.token().type               != TOK_OPERATOR || 
        root_node.token().value.operator_val != OP_BINARY_SUB)
        throw logic_error("Expected '-' operator");

    if (root_node. left()->token().type == TOK_CONSTANT &&
        root_node.right()->token().type == TOK_CONSTANT)
    {
        root_node.move(EGLnode(ConstantToken(root_node. left()->token().value.constant_val -
                                             root_node.right()->token().value.constant_val)));
        return;
    }

    if (root_node.left()->token().type               == TOK_OPERATOR &&
        root_node.left()->token().value.operator_val == OP_BINARY_SUB)
    {
        if (root_node.right()->token().type               != TOK_OPERATOR ||
            root_node.right()->token().value.operator_val != OP_GROUP_SUM)
            root_node.right()->set_as_parent(OperatorToken  (OP_GROUP_SUM));

        if (root_node.left()->right()->token().type               != TOK_OPERATOR ||
            root_node.left()->right()->token().value.operator_val != OP_GROUP_SUM) 
            root_node.left()->set_as_parent           (OperatorToken(OP_GROUP_SUM));

        root_node.right()->child_lst().merge(std::move(root_node.left()->right()->child_lst()));
        root_node. left()->                      move(*root_node.left()->left());
    }

    if (root_node.right()->token().type               == TOK_OPERATOR &&
        root_node.right()->token().value.operator_val == OP_BINARY_SUB)
    {
        if (root_node.left()->token().type               != TOK_OPERATOR ||
            root_node.left()->token().value.operator_val != OP_GROUP_SUM)
            root_node.left()->set_as_parent(OperatorToken  (OP_GROUP_SUM));

        if (root_node.right()->right()->token().type               != TOK_OPERATOR ||
            root_node.right()->right()->token().value.operator_val != OP_GROUP_SUM) 
            root_node.right()->set_as_parent           (OperatorToken(OP_GROUP_SUM));

        root_node. left()->child_lst().merge(std::move(root_node.right()->right()->child_lst()));
        root_node.right()->                      move(*root_node.right()-> left());
    }

    if      (root_node. left()->child_lst().size() == 1) root_node. left()->move(*root_node. left()->left());
    if      (root_node.right()->child_lst().size() == 1) root_node.right()->move(*root_node.right()->left());
    else if (root_node.right()->child_lst().empty())     root_node.         move(*root_node. left());
}

void EGLoptimizer::optBinDiv(EGLnode& root_node)
{
    if (root_node.token().type               != TOK_OPERATOR || 
        root_node.token().value.operator_val != OP_BINARY_DIV)
        throw logic_error("Expected '/' operator");
    
    if (root_node. left()->token().type == TOK_CONSTANT &&
        root_node.right()->token().type == TOK_CONSTANT)
    {
        root_node.move(EGLnode(ConstantToken(root_node. left()->token().value.constant_val /
                                             root_node.right()->token().value.constant_val)));
        return;
    }
                                                                                                     
    if (root_node.left()->token().type               == TOK_OPERATOR &&
        root_node.left()->token().value.operator_val == OP_BINARY_DIV)
    {
        if (root_node.right()->token().type               != TOK_OPERATOR ||
            root_node.right()->token().value.operator_val != OP_GROUP_MUL)
            root_node.right()->set_as_parent(OperatorToken  (OP_GROUP_MUL));

        if (root_node.left()->right()->token().type               != TOK_OPERATOR ||
            root_node.left()->right()->token().value.operator_val != OP_GROUP_MUL) 
            root_node.left()->set_as_parent           (OperatorToken(OP_GROUP_MUL));

        root_node.right()->child_lst().merge(std::move(root_node.left()->right()->child_lst()));
        root_node. left()->                      move(*root_node.left()-> left());
    }

    if (root_node.right()->token().type               == TOK_OPERATOR &&
        root_node.right()->token().value.operator_val == OP_BINARY_DIV)
    {
        if (root_node.left()->token().type               != TOK_OPERATOR ||
            root_node.left()->token().value.operator_val != OP_GROUP_MUL)
            root_node.left()->set_as_parent(OperatorToken  (OP_GROUP_MUL));

        if (root_node.right()->right()->token().type               != TOK_OPERATOR ||
            root_node.right()->right()->token().value.operator_val != OP_GROUP_MUL) 
            root_node.right()->set_as_parent           (OperatorToken(OP_GROUP_MUL));

        root_node. left()->child_lst().merge(std::move(root_node.right()->right()->child_lst()));
        root_node.right()->                      move(*root_node.right()-> left());
    }

    if      (root_node. left()->child_lst().size() == 1) root_node. left()->move(*root_node. left()->left());
    if      (root_node.right()->child_lst().size() == 1) root_node.right()->move(*root_node.right()->left());
    else if (root_node.right()->child_lst().empty())     root_node.         move(*root_node. left());
}

void EGLoptimizer::optBinPow(EGLnode& root_node)
{
    if (root_node.token().type               != TOK_OPERATOR || 
        root_node.token().value.operator_val != OP_BINARY_POW)
        throw logic_error("Expected '^' operator");
    
    if (root_node. left()->token().type == TOK_CONSTANT &&
        root_node.right()->token().type == TOK_CONSTANT)
    {
        root_node.move(EGLnode(ConstantToken(pow(root_node. left()->token().value.constant_val, 
                                                 root_node.right()->token().value.constant_val))));
        return;
    }
}

void EGLoptimizer::optGroupSum(EGLnode& root_node)
{
    if (root_node.token().type               != TOK_OPERATOR || 
        root_node.token().value.operator_val != OP_GROUP_SUM)
        throw logic_error("Expected '+' operator");

    root_node.set_as_parent(OperatorToken(OP_BINARY_SUB));
    root_node.push         (OperatorToken(OP_GROUP_SUM));

    for (auto it  = root_node.left()->child_lst().begin(); 
              it != root_node.left()->child_lst().end(); it++)
    {
        if ((*it)->token().type != TOK_OPERATOR) continue;

        if ((*it)->token().value.operator_val == OP_GROUP_SUM)
        {
            root_node.left()->child_lst().merge(std::move((*it)->child_lst()));    
            
            (*it)->clear();
            root_node.left()->child_lst().erase(it);
        }
        else if ((*it)->token().value.operator_val == OP_BINARY_SUB)
        {
            if ((*it)->left()->token().type               != TOK_OPERATOR ||
                (*it)->left()->token().value.operator_val != OP_GROUP_SUM) 
                (*it)->set_as_parent          (OperatorToken(OP_GROUP_SUM));

            if ((*it)->right()->token().type               != TOK_OPERATOR ||
                (*it)->right()->token().value.operator_val != OP_GROUP_SUM) 
                (*it)->set_as_parent           (OperatorToken(OP_GROUP_SUM));

            root_node. left()->child_lst().merge(std::move((*it)-> left()->child_lst()));
            root_node.right()->child_lst().merge(std::move((*it)->right()->child_lst()));

            (*it)->clear();
            root_node.left()->child_lst().erase(it);
        }
        else if ((*it)->token().value.operator_val == OP_UNARY_PLUS)
        {
            root_node.left()->child_lst().merge(std::move((*it)->child_lst()));

            (*it)->clear();
            root_node.left()->child_lst().erase(it);
        }
        else if ((*it)->token().value.operator_val == OP_UNARY_MINUS)
        {
            root_node.right()->child_lst().merge(std::move((*it)->child_lst()));

            (*it)->clear();
            root_node.left()->child_lst().erase(it);
        }
    }

    if      (root_node. left()->child_lst().size() == 1) root_node. left()->move(*root_node. left()->left());
    if      (root_node.right()->child_lst().size() == 1) root_node.right()->move(*root_node.right()->left());
    else if (root_node.right()->child_lst().empty())     root_node.         move(*root_node. left());
}

void EGLoptimizer::optGroupMul(EGLnode& root_node)
{
    if (root_node.token().type               != TOK_OPERATOR || 
        root_node.token().value.operator_val != OP_GROUP_MUL)
        throw logic_error("Expected '*' operator");

    root_node.set_as_parent(OperatorToken(OP_BINARY_DIV));
    root_node.push         (OperatorToken(OP_GROUP_MUL));

    for (auto it  = root_node.left()->child_lst().begin();  
              it != root_node.left()->child_lst().end(); it++)
    {
        if ((*it)->token().type != TOK_OPERATOR) continue;

        if ((*it)->token().value.operator_val == OP_GROUP_MUL)
        {
            root_node.left()->child_lst().merge(std::move((*it)->child_lst()));    
            
            (*it)->clear();
            root_node.left()->child_lst().erase(it);
        }
        else if ((*it)->token().value.operator_val == OP_BINARY_DIV)
        {
            if ((*it)->left()->token().type               != TOK_OPERATOR ||
                (*it)->left()->token().value.operator_val != OP_GROUP_MUL) 
                (*it)->set_as_parent          (OperatorToken(OP_GROUP_MUL));

            if ((*it)->right()->token().type               != TOK_OPERATOR ||
                (*it)->right()->token().value.operator_val != OP_GROUP_MUL) 
                (*it)->set_as_parent           (OperatorToken(OP_GROUP_MUL));

            root_node. left()->child_lst().merge(std::move((*it)-> left()->child_lst()));
            root_node.right()->child_lst().merge(std::move((*it)->right()->child_lst()));

            (*it)->clear();
            root_node.left()->child_lst().erase(it);
        }
    }

    if      (root_node. left()->child_lst().size() == 1) root_node. left()->move(*root_node. left()->left());
    if      (root_node.right()->child_lst().size() == 1) root_node.right()->move(*root_node.right()->left());
    else if (root_node.right()->child_lst().empty())     root_node.         move(*root_node. left());
}
