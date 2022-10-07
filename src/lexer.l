%option noyywrap

%{
	// Avoid error "error: `fileno' was not declared in this scope"
	extern "C" int fileno(FILE *stream);

	#include "parser.tab.hpp"
	#include <string>

%}

D [0-9]
L [a-zA-Z_]
E [Ee][+-]?{D}+

%%

"/*".("/*")?		{ return COMMENT; }


"char"				{ yylval.string=new std::string(yytext); return CHAR; }
"signed"			{ yylval.string=new std::string(yytext); return SIGNED; }
"unsigned"			{ yylval.string=new std::string(yytext); return UNSIGNED; }
"short"				{ yylval.string=new std::string(yytext); return SHORT; }
"int"				{ yylval.string=new std::string(yytext); return INT; }
"float"				{ yylval.string=new std::string(yytext); return FLOAT; }
"double"			{ yylval.string=new std::string(yytext); return DOUBLE; }
"long"				{ yylval.string=new std::string(yytext); return LONG; }
"void"				{ yylval.string=new std::string(yytext); return VOID; }

"break"				{ return BREAK; }
"case"				{ return CASE; }
"continue"			{ return CONTINUE; }
"default"			{ return DEFAULT; }
"do"				{ return DO; }
"else"				{ return ELSE; }
"enum"				{ return ENUM; }
"for"				{ return FOR; }
"if"				{ return IF; }
"return"			{ return RETURN; }
"sizeof"			{ return SIZEOF; }
"struct"			{ return STRUCT; }
"switch"			{ return SWITCH; }
"typedef"			{ return TYPEDEF; }
"while"				{ return WHILE; }


{L}+({D}|{L})*		{ 
						yylval.string=new std::string(yytext); 
						if(Program::typedefTable.count(*yylval.string) != 0){
							return CUSTOM_TYPE;
					}

						return IDENTIFIER;
					}

'(\\.|[^\\'])+'     { yylval.character = yytext+1; return CHAR_LITERAL; }

\"(\\.|[^\ \"])*\"  { yylval.string = new std::string(yytext); return STRING_LITERAL; }

[0-9]+(l|L)?			{ // TODO: implement hex, octa, binary, exponential
							yylval.integer=std::stoi(yytext); return T_INT; 
						}
{D}+{E}(f|F)	{ yylval.float_val=std::stof(yytext); return T_FLOAT; }
{D}+{E}(l|L)?	{ yylval.double_val=std::stod(yytext); return T_DOUBLE; }
{D}*"."{D}+({E})?(f|F)		{ yylval.float_val=std::stof(yytext); return T_FLOAT; }
{D}+"."{D}*({E})?(f|F)		{ yylval.float_val=std::stof(yytext); return T_FLOAT; }
{D}*"."{D}+({E})?(l|L)?		{ yylval.double_val=std::stod(yytext); return T_DOUBLE; }
{D}+"."{D}*({E})?(l|L)?		{ yylval.double_val=std::stod(yytext); return T_DOUBLE; }

"+"             	{ return '+'; }
"-"             	{ return '-'; }
"*"					{ return '*'; }
"/"             	{ return '/'; }
"%"					{ return '%'; }

"="					{ return '='; }
"+="				{ return ADD_ASSIGN; }
"-="				{ return SUB_ASSIGN; }
"*="				{ return MUL_ASSIGN; }
"/="				{ return DIV_ASSIGN; }
"%="				{ return MOD_ASSIGN; }

"&="				{ return AND_ASSIGN; }
"|="				{ return OR_ASSIGN; }
"^="				{ return XOR_ASSIGN; }

"<<="				{ return SHL_ASSIGN; }
">>="				{ return SHR_ASSIGN; }


"||"				{ return OR; }
"&&"				{ return AND; }
"<"					{ return LESSTHAN; }
">"					{ return GREATERTHAN; }
"!"					{ return '!'; }
"!="				{ return NOTEQ; }
"=="				{ return EQ; }
"<="				{ return LESSTHANEQ; }
">="				{ return GREATERTHANEQ; }

"++"				{ return INC_OP; }
"--"				{ return DEC_OP; }
"->"				{ return PTR_OP; }

"&"					{ return '&'; }
"|"					{ return '|'; }
"~"					{ return '~'; }
"^"					{ return '^'; }
"<<"				{ return BIT_SHIFTL; }
">>"				{ return BIT_SHIFTR; }

"("             	{ return '('; }
")"             	{ return ')'; }
"["             	{ return '['; }
"]"             	{ return ']'; }
"{"             	{ return '{'; }
"}"             	{ return '}'; }
";"					{ return ';'; }
","					{ return ','; }
":"					{ return ':'; }
"?"					{ return '?'; }
"."					{ return '.'; }

[ \t\r\n]+			{ ; } // ignore all whitespace
.					{ fprintf(stderr, "Caught invalid token %s\n", yytext); exit(1); } // catch all

%%
/* [0-9]+(\.[0-9]*)?	{ yylval.single_prec=strtod(yytext, 0); return CONSTANT; } */

void yyerror (char const *s)
{
  fprintf (stderr, "Parse error : %s \n", s);
  exit(1);
}

