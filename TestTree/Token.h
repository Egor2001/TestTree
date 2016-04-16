#pragma once
#pragma once

enum   EGLopType;
enum   EGLtokenType;
union  EGLtokenValue;
struct EGLtoken;

enum EGLopType 
{
    OP_INVALID = 0,
    OP_GROUP_SUM, OP_GROUP_MUL, 
    OP_BINARY_SUB, OP_BINARY_DIV, OP_BINARY_POW,
    OP_UNARY_PLUS, OP_UNARY_MINUS,
    OP_TOK_SUM, OP_TOK_SUB, OP_TOK_MUL, OP_TOK_DIV, OP_TOK_POW, OP_TOK_OPENED_BRACKET, OP_TOK_CLOSED_BRACKET
};

enum EGLtokenType 
{ 
    TOK_INVALID = 0, 
    TOK_CONSTANT,
    TOK_VARIABLE,
    TOK_FUNCTION, 
    TOK_OPERATOR 
};

typedef double (*EGLfncPtr)(double);

union EGLtokenValue
{
    double    constant_val;
    double*   variable_ptr;
    EGLopType operator_val;
    EGLfncPtr function_ptr;
};

bool operator<(const EGLtoken& op1, const EGLtoken& op2);
bool operator>(const EGLtoken& op1, const EGLtoken& op2);

EGLtoken ConstantToken(double    val);
EGLtoken VariableToken(double*   ptr);
EGLtoken OperatorToken(EGLopType val);
EGLtoken FunctionToken(EGLfncPtr ptr);

struct EGLtoken
{
    EGLtokenType  type;
    EGLtokenValue value;

    EGLtoken                     (): type (TOK_INVALID), value() {}
    EGLtoken(EGLtokenType type_set): type(type_set), value() {}

    ~EGLtoken () { type = TOK_INVALID; value = EGLtokenValue(); }
};

bool operator<(const EGLtoken& op1, const EGLtoken& op2)
{
    if (op1.type >= op2.type) return false;
    
    switch(op1.type)
    {
        case TOK_CONSTANT: if(op1.value.constant_val >= op2.value.constant_val) return false;
        case TOK_VARIABLE: if(op1.value.variable_ptr >= op2.value.variable_ptr) return false;
        case TOK_FUNCTION: if(op1.value.function_ptr >= op2.value.function_ptr) return false;
        case TOK_OPERATOR: if(op1.value.operator_val >= op2.value.operator_val) return false;
        default: return false;
    }

    return true;
}

bool operator>(const EGLtoken& op1, const EGLtoken& op2)
{
    if (op1.type <= op2.type) return false;
    
    switch(op1.type)
    {
        case TOK_CONSTANT: if(op1.value.constant_val <= op2.value.constant_val) return false;
        case TOK_VARIABLE: if(op1.value.variable_ptr <= op2.value.variable_ptr) return false;
        case TOK_FUNCTION: if(op1.value.function_ptr <= op2.value.function_ptr) return false;
        case TOK_OPERATOR: if(op1.value.operator_val <= op2.value.operator_val) return false;
        default: return false;
    }

    return true;
}

EGLtoken ConstantToken(double value)
{
    EGLtoken result;

    result.type               = TOK_CONSTANT;
    result.value.constant_val = value;

    return result;
}

EGLtoken VariableToken(double* ptr)
{
    EGLtoken result;

    result.type               = TOK_VARIABLE;
    result.value.variable_ptr = ptr;

    return result;
}

EGLtoken OperatorToken(EGLopType value)
{
    EGLtoken result;

    result.type               = TOK_OPERATOR;
    result.value.operator_val = value;

    return result;
}

EGLtoken FunctionToken(EGLfncPtr ptr)
{
    EGLtoken result;

    result.type               = TOK_FUNCTION;
    result.value.function_ptr = ptr;

    return result;
}
