#pragma once

#include "Node.h"

#include <string>
#include <stdexcept>

using std::string;
using std::logic_error;

class Optimizer
{
public:
    Optimizer() {}
 
    ~Optimizer() {}

    void operator()(EGLnode& root_node);

private:
    Optimizer           (const Optimizer& set) {}
    Optimizer& operator=(const Optimizer& set) { return Optimizer(); }

    void optNode (EGLnode& root_node);
    void optOper (EGLnode& root_node);
    void optConst(EGLnode& root_node);
    void optFunct(EGLnode& root_node);
    void optEqual(EGLnode& root_node);
};

void Optimizer::optNode(EGLnode& root_node)
{
    switch(root_node.token().type)
    {
        case TOK_CONSTANT:                      break;
        case TOK_VARIABLE:                      break;
        case TOK_FUNCTION: optFunct(root_node); break;
        case TOK_OPERATOR: optOper (root_node); break;

        default: throw logic_error("Invalid node type"); break;
    }
}

void Optimizer::optOper(EGLnode& root_node)
{
    if (root_node.token().type != TOK_OPERATOR)
        throw logic_error("Expected operator");

    for (EGLnode* child_ptr : root_node.child_lst()) optNode(*child_ptr);

    switch (root_node.token().value.operator_val)
    {
        default: throw logic_error("Invalid operator type"); break;
    }
}

void Optimizer::optConst(EGLnode& root_node)
{
}

void Optimizer::optFunct(EGLnode& root_node)
{
}

void Optimizer::optEqual(EGLnode& root_node)
{
}

