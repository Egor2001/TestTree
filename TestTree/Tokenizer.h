#pragma once

#include "Token.h"

#include <string>
#include <vector>
#include <map>

using std::invalid_argument;

using std::vector;
using std::string;
using std::map;

class EGLtokenizer
{
public:
    EGLtokenizer(const map<string, double*>&   variable_map_set = map<string, double*>(), 
                 const map<string, EGLfncPtr>& function_map_set = map<string, EGLfncPtr>(),
                 const map<string, double>&    constant_map_set = map<string, double>()): 
        varconst_map_(), 
        function_map_(), 
        str_pos_(), 
        str_end_() 
    {
        for (std::pair<string, double*>   elem : variable_map_set) varconst_map_[elem.first] = VariableToken(elem.second);
        for (std::pair<string, double>    elem : constant_map_set) varconst_map_[elem.first] = ConstantToken(elem.second);
        for (std::pair<string, EGLfncPtr> elem : function_map_set) function_map_[elem.first] = FunctionToken(elem.second);
    }
    
    ~EGLtokenizer() { varconst_map_.clear(); function_map_.clear(); }

    void set_variable(const string& name_str,   double* value_ptr) { varconst_map_[name_str] = VariableToken(value_ptr); }
    void set_function(const string& name_str, EGLfncPtr value_ptr) { function_map_[name_str] = FunctionToken(value_ptr); }
    void set_constant(const string& name_str,    double value)     { varconst_map_[name_str] = ConstantToken(value);     }

    void operator()(const string& expr_str, vector<EGLtoken>& result_v);
    
private:
    EGLtokenizer          (const EGLtokenizer& set) {}
    EGLtokenizer operator=(const EGLtokenizer& set) { return EGLtokenizer(set); }
    
    EGLtoken getNumber  ();
    EGLtoken getNamed   ();
    EGLtoken getOperator();
    void     skipSpaces ();

    map<string, EGLtoken> varconst_map_;
	map<string, EGLtoken> function_map_;
    
    string::const_iterator str_pos_, str_end_;
};

void EGLtokenizer::operator()(const string& expr_str, vector<EGLtoken>& result_v) 
{
    str_pos_ = expr_str.cbegin();
    str_end_ = expr_str.cend();

    while (str_pos_ < str_end_)
	{
		skipSpaces();

			 if (isdigit(*str_pos_)) result_v.push_back(getNumber  ());
		else if (isalpha(*str_pos_)) result_v.push_back(getNamed   ());
		else                         result_v.push_back(getOperator());
	}
}
    
EGLtoken EGLtokenizer::getNumber() 
{
    size_t beforeDot = 0, 
            afterDot = 0;
	size_t fracDigNum = 0;

	while (str_pos_ < str_end_ && isdigit(*str_pos_))
	{
		beforeDot = beforeDot * 10 + (*str_pos_ - '0');
		str_pos_++;
	}

	if (str_pos_ == str_end_ || *str_pos_ != '.') return ConstantToken((double)beforeDot);
	else str_pos_++;

	while (str_pos_ < str_end_ && isdigit(*str_pos_))
	{
		afterDot = afterDot * 10 + (*str_pos_ - '0');
		fracDigNum++;
		str_pos_++;
	}

    return ConstantToken(beforeDot + (double)afterDot / pow(10.0, fracDigNum));
}

EGLtoken EGLtokenizer::getNamed() 
{
	string name;

	while (str_pos_ < str_end_ && isalnum(*str_pos_))
	{
		name.push_back(*str_pos_);
		str_pos_++;
	}
	
    skipSpaces();

	if (str_pos_ < str_end_ && *str_pos_ == '(') 
    {
        if   (!function_map_.count(name)) throw invalid_argument(string("function was not found: ") + name);
        return function_map_.find (name)->second;
	}
    else
    {
        if   (!varconst_map_.count(name)) throw invalid_argument(string("variable was not found") + name);
        return varconst_map_.find (name)->second;
	}
}

EGLtoken EGLtokenizer::getOperator() 
{
    EGLtoken result;

    switch(*str_pos_)
    {
        case '+': result = OperatorToken(OP_TOK_SUM);            break;
        case '-': result = OperatorToken(OP_TOK_SUB);            break;
        case '*': result = OperatorToken(OP_TOK_MUL);            break;
        case '/': result = OperatorToken(OP_TOK_DIV);            break;
        case '^': result = OperatorToken(OP_TOK_POW);            break;
        case '(': result = OperatorToken(OP_TOK_OPENED_BRACKET); break;
        case ')': result = OperatorToken(OP_TOK_CLOSED_BRACKET); break;
        default: throw invalid_argument(string("Unknown symbol: ") + *str_pos_); break;
    }

    str_pos_++;

    return result;
}
void EGLtokenizer::skipSpaces() 
{
    while (str_pos_ < str_end_ && isspace(*str_pos_)) str_pos_++;
}
