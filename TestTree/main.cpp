#define _USE_MATH_DEFINES

#include <cmath>

#include "Node.h"
#include "Tokenizer.h"
#include "Analyzer.h"
#include "Calculator.h"
#include "Optimizer.h"
#include "TreeCheck.h"

int main()
{
    double x = 100.0;
    
    EGLtokenizer tokenizer;

    tokenizer.set_variable("x", &x);
    tokenizer.set_function("sin", &sin);
    tokenizer.set_function("cos", &cos);
    tokenizer.set_constant("PI", M_PI);
    tokenizer.set_constant("E", M_E);

    EGLanalyzer analyzer;
    EGLoptimizer optimizer;

    vector<EGLtoken> tok_vec;
    EGLnode root_node;

    tokenizer("1+(2-3)-(4+5-6)-(-7)", tok_vec);
    analyzer(tok_vec, root_node);
    optimizer(root_node);

    TREE_CHECK::PrintTree(stdout, &root_node);

    printf("\n %lg", eglCalculate(root_node));

    getchar();

    return 0;
}