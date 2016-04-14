#include "Node.h"
#include "Tokenizer.h"
#include "Analyzer.h"
#include "Calculator.h"
#include "TreeCheck.h"

int main()
{
    /*EGLnode root_node;
    root_node.set_as_parent(OperatorToken(OP_GROUP_SUM));
    root_node.set_as_parent(OperatorToken(OP_BINARY_SUB));
    root_node.push         (OperatorToken(OP_GROUP_SUM));
*/
    double x = 100.0;
    const double PI = 3.14159265;

    EGLtokenizer tokenizer ({{"x", &x}}, {{"sin", &sin}, {"cos", &cos}}, {{"PI", PI}});
    EGLanalyzer analyzer;

    vector<EGLtoken> tok_vec;
    EGLnode root_node;

    tokenizer("x*sin(cos(PI/3)*PI/3)^(1/2)", tok_vec);
    analyzer(tok_vec, root_node);

    TREE_CHECK::PrintTree(stdout, &root_node);

    printf("\n %lg", eglCalculate(root_node));

    getchar();

    return 0;
}