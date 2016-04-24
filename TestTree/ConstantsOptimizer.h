#pragma once

#include "Node.h"

#include <string>
#include <stdexcept>

using std::string;
using std::logic_error;
using std::invalid_argument;

class EGLconstantsOptimizer
{
public:
    EGLconstantsOptimizer() {}

    void optConstants(EGLnode& root_node);

private:
    EGLconstantsOptimizer    (const EGLconstantsOptimizer& set) {}
    EGLconstantsOptimizer operator=(EGLconstantsOptimizer& set) { return EGLconstantsOptimizer(set); }

    void optGroupBinary(EGLnode& root_node, EGLopType group_op_type);
    void optOperator   (EGLnode& root_node);
};

void EGLconstantsOptimizer::optConstants(EGLnode& root_node)
{
    switch (root_node.token().type)
    {
        case TOK_CONSTANT:
        case TOK_VARIABLE: return;

        case TOK_FUNCTION:
        {
            if (root_node.next().token().type != TOK_CONSTANT) return;

            root_node.move(EGLnode(ConstantToken((*root_node.       token().value.function_ptr)
                                                  (root_node.next().token().value.constant_val))));
        }
        break;

        case TOK_OPERATOR: optOperator(root_node); break;
        
        default: throw invalid_argument("invalid token type"); break;
    }
}

void EGLconstantsOptimizer::optGroupBinary(EGLnode& root_node, EGLopType group_op_type)
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
    
    double result_val = (group_op_type == OP_GROUP_SUM ? 0.0 : 1.0f);

    for (auto it  = root_node.left().child_lst().begin();
              it != root_node.left().child_lst().  end();)
    {
        if ((*it)->token().type != TOK_CONSTANT) { it++; continue; }

             if (group_op_type == OP_GROUP_SUM) result_val += (*it)->token().value.constant_val;
        else if (group_op_type == OP_GROUP_MUL) result_val *= (*it)->token().value.constant_val;
        else throw logic_error("expected group operator");

        (*it)->clear(); it = root_node.left().child_lst().erase(it);
    }
              
    for (auto it  = root_node.right().child_lst().begin();
              it != root_node.right().child_lst().  end();)
    {
        if ((*it)->token().type != TOK_CONSTANT) { it++; continue; }
    
             if (group_op_type == OP_GROUP_SUM) result_val -= (*it)->token().value.constant_val;
        else if (group_op_type == OP_GROUP_MUL) result_val /= (*it)->token().value.constant_val;
        else throw logic_error("expected group operator");

        (*it)->clear(); it = root_node.right().child_lst().erase(it);
    }
    
    if (result_val != (group_op_type == OP_GROUP_SUM ? 0.0 : 1.0f)) 
        root_node.left().push(ConstantToken(result_val));

    egl_nodeSetOpt(root_node);
}
    
void EGLconstantsOptimizer::optOperator(EGLnode& root_node)
{
    if (root_node.token().type != TOK_OPERATOR)
        throw invalid_argument("expected operator");

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
        
        case OP_BINARY_POW: 
        {
            if (root_node. left().token().type == TOK_CONSTANT &&
                root_node.right().token().type == TOK_CONSTANT)
            {
                root_node.move(EGLnode(ConstantToken(pow(root_node. left().token().value.constant_val,
                                                         root_node.right().token().value.constant_val))));
            }
        }
        break;
        
        case OP_GROUP_SUM:  
        case OP_BINARY_SUB: optGroupBinary(root_node, OP_GROUP_SUM); break;
        case OP_GROUP_MUL:  
        case OP_BINARY_DIV: optGroupBinary(root_node, OP_GROUP_MUL); break;
        
        default: throw logic_error("Invalid operator type"); break;
    }
}
