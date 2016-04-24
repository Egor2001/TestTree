#pragma once

#include "Node.h"

#include <string>
#include <stdexcept>

using std::string;
using std::logic_error;
using std::invalid_argument;

class EGLoperatorsOptimizer
{
public:
    EGLoperatorsOptimizer() {}
    
    void optOperators(EGLnode& root_node);

private:
    EGLoperatorsOptimizer    (const EGLoperatorsOptimizer& set) {}
    EGLoperatorsOptimizer operator=(EGLoperatorsOptimizer& set) { return EGLoperatorsOptimizer(set); }

    void optGroupBinary(EGLnode& root_node, EGLopType group_op_type);
    void optUnary      (EGLnode& root_node);
};

void EGLoperatorsOptimizer::optOperators(EGLnode& root_node)
{
    if (root_node.token().type != TOK_OPERATOR) return;

    switch (root_node.token().value.operator_val)
    {
        case OP_UNARY_PLUS:
        case OP_UNARY_MINUS: optUnary(root_node); break;
        case OP_GROUP_SUM:  
        case OP_BINARY_SUB: optGroupBinary(root_node, OP_GROUP_SUM); break;
        case OP_GROUP_MUL:  
        case OP_BINARY_DIV: optGroupBinary(root_node, OP_GROUP_MUL); break;
        case OP_BINARY_POW: break;
        
        default: throw logic_error("Invalid operator type"); break; 
    }
}

void EGLoperatorsOptimizer::optGroupBinary(EGLnode& root_node, EGLopType group_op_type)
{
    if (root_node.token().type != TOK_OPERATOR) 
        throw logic_error("Expected operator");

    if (group_op_type != OP_GROUP_SUM && 
        group_op_type != OP_GROUP_MUL) throw logic_error("Expected group operator");

    EGLopType bin_op_type = (group_op_type == OP_GROUP_SUM ? OP_BINARY_SUB : OP_BINARY_DIV);

    if (root_node.token().value.operator_val == group_op_type)
    {
        root_node.set_as_parent(OperatorToken  (bin_op_type));
        root_node.push         (OperatorToken(group_op_type));
    }
    else if (root_node.token().value.operator_val == bin_op_type)
    {
        egl_nodeSetFull(root_node. left(), group_op_type);
        egl_nodeSetFull(root_node.right(), group_op_type);
    }
    else throw logic_error("Expected binary or group operator type");
    
    for (auto it  = root_node.left().child_lst().begin();
              it != root_node.left().child_lst().  end();)
    {
        if ((*it)->token().type               != TOK_OPERATOR) { it++; continue; }
        if ((*it)->token().value.operator_val == group_op_type)
        {
                 root_node.left().merge_with(*(*it)); (*it)->clear();
            it = root_node.left().child_lst().erase(it);
        }
        else if ((*it)->token().value.operator_val == bin_op_type)
        {
            egl_nodeSetFull             ((*it)->right(), group_op_type);
            root_node.right().merge_with((*it)->right());
            egl_nodeSetOpt             (*(*it)); 
        }
        else it++;
    }
    
    for (auto it  = root_node.right().child_lst().begin();
              it != root_node.right().child_lst().  end();)
    {
        if ((*it)->token().type               != TOK_OPERATOR) { it++; continue; }
        if ((*it)->token().value.operator_val == group_op_type)
        {
                 root_node.right().merge_with    (*(*it)); (*it)->clear();
            it = root_node.right().child_lst().erase(it);
        }
        else if ((*it)->token().value.operator_val == bin_op_type)
        {
            egl_nodeSetFull            ((*it)->right(), group_op_type);
            root_node.left().merge_with((*it)->right());
            egl_nodeSetOpt            (*(*it));
        }
        else it++;
    }
    
    egl_nodeSetOpt(root_node);
}

void EGLoperatorsOptimizer::optUnary(EGLnode& root_node)
{
    if (root_node.token().type != TOK_OPERATOR) 
        throw logic_error("Expected operator");

    if (root_node.token().value.operator_val == OP_UNARY_PLUS)
    {
        root_node.move(root_node.next());
    } 
    else if (root_node.token().value.operator_val == OP_UNARY_MINUS)
    {
        if (root_node.next().token().type == TOK_CONSTANT)
        {
                           root_node.next().token().value.constant_val *= -1;
            root_node.move(root_node.next());
        }
    }
    else throw logic_error("Expected unary operator");
}