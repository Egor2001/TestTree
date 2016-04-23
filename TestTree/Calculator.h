#pragma once

#include "Node.h"

#include <string>

using std::string;

double eglCalculate(const EGLnode& root_node);

double eglCalculate(const EGLnode& root_node)
{
    switch(root_node.token().type)
    {
        case TOK_CONSTANT: return    root_node.token().value.constant_val;                                   break;
        case TOK_VARIABLE: return  *(root_node.token().value.variable_ptr);                                  break;
        case TOK_FUNCTION: return (*(root_node.token().value.function_ptr))(eglCalculate(root_node.left())); break;
        case TOK_OPERATOR:
        {
            switch (root_node.token().value.operator_val)
            {
                case OP_UNARY_PLUS:  return +eglCalculate(root_node.left()); break;
                case OP_UNARY_MINUS: return -eglCalculate(root_node.left()); break;

                case OP_BINARY_SUB: return     eglCalculate(root_node.left()) - eglCalculate(root_node.right());  break;
                case OP_BINARY_DIV: return     eglCalculate(root_node.left()) / eglCalculate(root_node.right());  break;
                case OP_BINARY_POW: return pow(eglCalculate(root_node.left()),  eglCalculate(root_node.right())); break;
                
                case OP_GROUP_SUM:
                {
                    double result = 0.0;

                    for (EGLnode* child_ptr : root_node.child_lst()) result += eglCalculate(*child_ptr); 
                    
                    return result;
                }
                break;

                case OP_GROUP_MUL:
                {
                    double result = 1.0;

                    for (EGLnode* child_ptr : root_node.child_lst()) result *= eglCalculate(*child_ptr); 
                    
                    return result;
                }
                break;

                default: throw logic_error("Unknown symbol");
            }
        }
        break;

        default: throw logic_error("Unknown token type");
    }
}