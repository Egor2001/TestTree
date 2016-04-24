#pragma once

#include "Node.h"
#include "OperatorsOptimizer.h"
#include "ConstantsOptimizer.h"

#include <string>
#include <stdexcept>

using std::string;
using std::logic_error;
using std::invalid_argument;

class EGLoptimizer : public EGLoperatorsOptimizer, 
                     public EGLconstantsOptimizer
{
public:
    EGLoptimizer(): EGLoperatorsOptimizer(), EGLconstantsOptimizer() {}
 
    ~EGLoptimizer() {}

    void operator()(EGLnode& root_node) { optNode(root_node); }

private:
    EGLoptimizer          (const EGLoptimizer& set) {}
    EGLoptimizer operator=(const EGLoptimizer& set) { return EGLoptimizer(set); }

    void optNode(EGLnode& root_node);
};

void EGLoptimizer::optNode(EGLnode& root_node)
{
    for (EGLnode* child_ptr : root_node.child_lst()) optNode(*child_ptr);

    optOperators(root_node);
    optConstants(root_node);
}
