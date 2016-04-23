#pragma once

#include "Node.h"
#include "OperatorsOptimizer.h"
#include "ConstantsOptimizer.h"

#include <string>
#include <stdexcept>

using std::string;
using std::logic_error;
using std::invalid_argument;

class EGLoptimizer
{
public:
    EGLoptimizer() {}
 
    ~EGLoptimizer() {}

    void operator()(EGLnode& root_node) { optNode(root_node); }

private:
    EGLoptimizer          (const EGLoptimizer& set) {}
    EGLoptimizer operator=(const EGLoptimizer& set) { return EGLoptimizer(set); }

    void optNode(EGLnode& root_node);
    void optOper(EGLnode& root_node);

    void toFullFormat(EGLnode& node, EGLopType opType);
    void toOptFormat (EGLnode& node);

    void optGroupBinOp(EGLnode& root_node, EGLopType group_op_type);
    void optConst     (EGLnode& root_node);
    void optEqual     (EGLnode& root_node);
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
        case OP_UNARY_PLUS: 
        {
            root_node.move(root_node.left()); 
        }
        break;

        case OP_UNARY_MINUS:
        {
            if (root_node.left().token().type == TOK_CONSTANT)
            {
                root_node.left().token().value.constant_val *= -1;
                root_node.move(root_node.left());
            }
        }
        break;
        
        case OP_GROUP_SUM:  
        case OP_BINARY_SUB: optGroupBinOp(root_node, OP_GROUP_SUM); break;
        case OP_GROUP_MUL:  
        case OP_BINARY_DIV: optGroupBinOp(root_node, OP_GROUP_MUL); break;
        case OP_BINARY_POW: break;
        
        default: throw logic_error("Invalid operator type"); break;
    }
}

void EGLoptimizer::toFullFormat(EGLnode& node, EGLopType opType)
{
    if (node.token().type               != TOK_OPERATOR ||
        node.token().value.operator_val != opType)
        node.set_as_parent(OperatorToken  (opType));
}

void EGLoptimizer::toOptFormat(EGLnode& node)
{
    if (node.token().type               != TOK_OPERATOR ||
       (node.token().value.operator_val != OP_BINARY_SUB &&
        node.token().value.operator_val != OP_BINARY_DIV)) 
        throw invalid_argument("expected binary operator");

    if      (node. left().child_lst().size() == 1) node. left().move(node. left().left());
    if      (node.right().child_lst().size() == 1) node.right().move(node.right().left());
    else if (node.right().child_lst().empty())     node.        move(node. left());
}

void EGLoptimizer::optGroupBinOp(EGLnode& root_node, EGLopType group_op_type)
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
        toFullFormat(root_node. left(), group_op_type);
        toFullFormat(root_node.right(), group_op_type);
    }
    else throw logic_error("Expected '+' or '-'");
    
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
            toFullFormat                ((*it)->right(), group_op_type);
            root_node.right().merge_with((*it)->right());
            toOptFormat                (*(*it)); 
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
            toFullFormat               ((*it)->right(), group_op_type);
            root_node.left().merge_with((*it)->right());
            toOptFormat               (*(*it));
        }
        else it++;
    }
    
    toOptFormat(root_node);
}

void EGLoptimizer::optConst(EGLnode& root_node)
{
}

void EGLoptimizer::optEqual(EGLnode& root_node)
{
}
