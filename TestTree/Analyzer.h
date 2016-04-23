#pragma once

#include "Node.h"

#include <vector>
#include <string>
#include <stdexcept>

using std::vector;
using std::string;
using std::logic_error;

class EGLanalyzer
{
public:
    EGLanalyzer(): vec_pos_(), vec_end_() {}
    
    ~EGLanalyzer() 
    { 
        vec_pos_ = vector<EGLtoken>::iterator(); 
        vec_end_ = vector<EGLtoken>::iterator(); 
    }
    
    void operator()(const vector<EGLtoken> tok_vec, EGLnode& root_node);
    
private:
    EGLanalyzer          (const EGLanalyzer& set) {}
    EGLanalyzer operator=(const EGLanalyzer& set) { return EGLanalyzer(set); }

    void getNumeric (EGLnode& cur_node);
    void getBracket (EGLnode& cur_node);
    void getFunction(EGLnode& cur_node);
    void getValue   (EGLnode& cur_node);
    void getPow     (EGLnode& cur_node);
    void getMulDiv  (EGLnode& cur_node);
    void getSumSub  (EGLnode& cur_node);

    vector<EGLtoken>::const_iterator vec_pos_, vec_end_;
};

void EGLanalyzer::operator()(const vector<EGLtoken> tok_vec, EGLnode& root_node)
{
    vec_pos_ = tok_vec.cbegin();
    vec_end_ = tok_vec.cend();

    getSumSub(root_node);

    if (vec_pos_ < vec_end_) throw logic_error("Unknown token");
}

void EGLanalyzer::getNumeric(EGLnode& cur_node)
{
    if(vec_pos_->type != TOK_CONSTANT &&
       vec_pos_->type != TOK_VARIABLE) throw logic_error("Expected variable or constant");

    cur_node.move(EGLnode(*vec_pos_));  
    vec_pos_++;
}

void EGLanalyzer::getBracket(EGLnode& cur_node)
{
    if (vec_pos_->type               != TOK_OPERATOR || 
        vec_pos_->value.operator_val != OP_TOK_OPENED_BRACKET) throw logic_error("Expected '('");
    vec_pos_++;
    
    getSumSub(cur_node);

    if (vec_pos_->type               != TOK_OPERATOR || 
        vec_pos_->value.operator_val != OP_TOK_CLOSED_BRACKET) throw logic_error("Expected ')'");
    vec_pos_++; 
}

void EGLanalyzer::getFunction(EGLnode& cur_node)
{
    if (vec_pos_->type != TOK_FUNCTION) throw logic_error("Expected function");

    cur_node.move(EGLnode(*vec_pos_));
    vec_pos_++;

    EGLnode       param_node;
    getBracket   (param_node);
    cur_node.push(param_node);
}

void EGLanalyzer::getValue(EGLnode& cur_node)
{
    EGLtoken sign_tok = OperatorToken(OP_INVALID);
    
    if(vec_pos_->type == TOK_OPERATOR && (vec_pos_->value.operator_val == OP_TOK_SUM ||
                                          vec_pos_->value.operator_val == OP_TOK_SUB))
    {
        sign_tok = OperatorToken((vec_pos_->value.operator_val == OP_TOK_SUM) ? OP_UNARY_PLUS : OP_UNARY_MINUS);
        vec_pos_++;
    }

    switch (vec_pos_->type)
    {
        case TOK_CONSTANT: getNumeric (cur_node); break;
        case TOK_VARIABLE: getNumeric (cur_node); break;
        case TOK_FUNCTION: getFunction(cur_node); break;
        case TOK_OPERATOR: 
        {
            if (vec_pos_->value.operator_val != OP_TOK_OPENED_BRACKET) 
                throw logic_error("Expected '('...')', constant, variable or function call");
            
            getBracket (cur_node); 
        }
        break;

        default: 
            throw logic_error("Expected '('...')', constant, variable or function call"); 
        break;
    }

    if (sign_tok.value.operator_val != OP_INVALID) cur_node.set_as_parent(sign_tok);
}

void EGLanalyzer::getPow(EGLnode& cur_node)
{
    EGLnode nextPow_node;
    
    getValue(cur_node);
    if (vec_pos_ < vec_end_ && vec_pos_->type               == TOK_OPERATOR && 
                               vec_pos_->value.operator_val == OP_TOK_POW)
    {
        cur_node.set_as_parent(OperatorToken(OP_BINARY_POW)); vec_pos_++;
        getPow       (nextPow_node);
        cur_node.push(nextPow_node);
    }
}

void EGLanalyzer::getMulDiv(EGLnode& cur_node)
{
    EGLnode value_node;
    EGLopType cur_op_type = OP_INVALID;

    getPow(cur_node);
    cur_node.set_as_parent(OperatorToken(OP_GROUP_MUL));
    cur_node.set_as_parent(OperatorToken(OP_BINARY_DIV));
    cur_node.push         (OperatorToken(OP_GROUP_MUL));

    while (vec_pos_ < vec_end_ && vec_pos_->type               == TOK_OPERATOR && 
                                 (vec_pos_->value.operator_val == OP_TOK_MUL || 
                                  vec_pos_->value.operator_val == OP_TOK_DIV))
    {
        cur_op_type = vec_pos_->value.operator_val;
        vec_pos_++;

        getPow(value_node);

        if (cur_op_type == OP_TOK_MUL) cur_node. left().push(value_node);
        else                           cur_node.right().push(value_node);
    }

    egl_nodeSetOpt(cur_node);
}

void EGLanalyzer::getSumSub(EGLnode& cur_node)
{
    EGLnode value_node;
    EGLopType cur_op_type = OP_INVALID;

    getMulDiv(cur_node);
    cur_node.set_as_parent(OperatorToken(OP_GROUP_SUM));
    cur_node.set_as_parent(OperatorToken(OP_BINARY_SUB));
    cur_node.push         (OperatorToken(OP_GROUP_SUM));

    while (vec_pos_ < vec_end_ && vec_pos_->type               == TOK_OPERATOR && 
                                 (vec_pos_->value.operator_val == OP_TOK_SUM || 
                                  vec_pos_->value.operator_val == OP_TOK_SUB))
    {
        cur_op_type = vec_pos_->value.operator_val;
        vec_pos_++;

        getMulDiv(value_node);

        if (cur_op_type == OP_TOK_SUM) cur_node. left().push(value_node);
        else                           cur_node.right().push(value_node);
    }

    egl_nodeSetOpt(cur_node);
}

    