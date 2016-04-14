#pragma once

#include "Node.h"

namespace TREE_CHECK {
    void PrintTree(FILE* in_file, const EGLnode* root_node);
    
    void PrintTree(FILE* in_file, const EGLnode* root_node)
    {
        switch (root_node->token().type)
        {
            case TOK_CONSTANT: fprintf(in_file, "[ {constant |%lg|} ", root_node->token().value.constant_val); break;
            case TOK_VARIABLE: fprintf(in_file, "[ {variable (%p)} ",  root_node->token().value.variable_ptr); break;
            case TOK_FUNCTION: fprintf(in_file, "[ {function (%p)} ",  root_node->token().value.function_ptr); break;
            case TOK_OPERATOR: fprintf(in_file, "[ {operator '%d'} ",  root_node->token().value.operator_val); break;
            default:           fprintf(in_file, "[ {invalid} "); break;
        }

        for (const EGLnode* child_ptr : root_node->child_lst()) PrintTree(in_file, child_ptr);

        fprintf(in_file, " ]");
    }
}