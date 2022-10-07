// parser file
%code requires{
	#include "inc.hpp"
	#include <cassert>

	extern Node *g_root; // get AST out of parser

	//! This is to fix problems when generating C++
	// We are declaring the functions provided by Flex, so
	// that Bison generated code can call them.
	int yylex(void);
	void yyerror(const char *);
}

// Possible values with each AST node
%union{
	Node* node;
	int integer;
	float float_val;
	double double_val;
	std::string* string;
	char* character;
}

%define parse.error verbose

%token COMMENT

// keywords
%token BREAK CASE CHAR CONTINUE DEFAULT DO DOUBLE ELSE ENUM
%token FLOAT FOR IF INT LONG RETURN SHORT SIGNED SIZEOF
%token STRUCT SWITCH TYPEDEF UNSIGNED VOID WHILE

// tokens
%token T_INT T_FLOAT STRING_LITERAL CHAR_LITERAL T_DOUBLE
%token IDENTIFIER CUSTOM_TYPE

// arithmetic operators
%token ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN
%token AND_ASSIGN OR_ASSIGN XOR_ASSIGN SHL_ASSIGN SHR_ASSIGN
%token BIT_SHIFTL BIT_SHIFTR
%token INC_OP DEC_OP PTR_OP

// logical operators
%token OR AND LESSTHAN GREATERTHAN '!' EQ NOTEQ LESSTHANEQ GREATERTHANEQ


%type <integer> T_INT
%type <float_val> T_FLOAT
%type <double_val> T_DOUBLE
%type <character> CHAR_LITERAL
 
%type <string> IDENTIFIER INT FLOAT DOUBLE SHORT SIGNED UNSIGNED LONG CHAR VOID STRING_LITERAL CUSTOM_TYPE
%type <node> ROOT CONSTANT TRANSLATION_UNIT FUNCTION DECLARATION DECLARATION_SPECIFIER PRIMITIVE_TYPE 
%type <node> EXPRESSION STATEMENT COMPOUND_STATEMENT RETURN_STATEMENT ASSIGNMENT_STATEMENT BRANCH_STATEMENT STATEMENT_LIST JUMP_STATEMENT LABELLED_STATEMENT
%type <node> DECLARATION_STATEMENT ITERATION_STATEMENT INIT_DECLARATION_LIST DECLARATION_STATEMENT_LIST
%type <node> UNARY_OPERATOR ASSIGNMENT_OP PRIMARY_EXPRESSION ADDITIVE_EXPRESSION PREFIX_EXPRESSION
%type <node> ENUM_SPECIFIER ENUM_LIST ENUMERATOR STRUCT_SPECIFIER MULTIPLICATIVE_EXPRESSION
%type <node> LOGICAL_OR_EXPRESSION LOGICAL_AND_EXPRESSION INCLUSIVE_OR_EXPRESSION EXCLUSIVE_OR_EXPRESSION
%type <node> AND_EXPRESSION EQUALITY_EXPRESSION RELATIONAL_EXPRESSION SHIFT_EXPRESSION ARRAY_INITIALIZER_LIST
%type <node> POSTFIX_EXPRESSION EXPRESSION_LIST EXPRESSION_STATEMENT ASSIGNMENT_EXPRESSION TYPEDEF_STATEMENT
 

// declare start point
%start ROOT


%%


/* 
From wikipedia:
	A translation unit is the ultimate input to a C or C++ compiler from 
	which an object file is generated. 
*/

/**************************** FUNCTIONS ****************************/

ROOT	:	TRANSLATION_UNIT	{ g_root = $1; }

TRANSLATION_UNIT	: FUNCTION { $$ = new TranslationUnit($1); }
					| DECLARATION_STATEMENT { $$ = new TranslationUnit($1); }
					| ASSIGNMENT_STATEMENT { $$ = new TranslationUnit($1); }
					| TYPEDEF_STATEMENT { $$ = new TranslationUnit($1); }
					| TRANSLATION_UNIT FUNCTION { $1->listAppend($2); $$ = $1; }
					| TRANSLATION_UNIT DECLARATION_STATEMENT { $1->listAppend($2); $$ = $1; }
					| TRANSLATION_UNIT ASSIGNMENT_STATEMENT { $1->listAppend($2); $$ = $1; }
					| TRANSLATION_UNIT TYPEDEF_STATEMENT { $1->listAppend($2); $$ = $1; }
					;



// int f(int a, int b){}
FUNCTION	: DECLARATION '(' ')' ';' { $$ = new Function($1, NULL, NULL); }
			| DECLARATION '(' INIT_DECLARATION_LIST ')' ';' { $$ = new Function($1, $3, NULL); }
			| DECLARATION '(' ')' COMPOUND_STATEMENT { $$ = new Function($1, NULL, $4); }
			| DECLARATION '(' INIT_DECLARATION_LIST ')' COMPOUND_STATEMENT { $$ = new Function($1, $3, $5); }
			;

// Inside function calls only (int a, int b)
INIT_DECLARATION_LIST 	: DECLARATION { $$ = new InitDeclarationList($1); }
						| INIT_DECLARATION_LIST ',' DECLARATION { $1->listAppend($3); $$ = $1;}
						;

// for arrays, x[5] = {1,2,3,4,5};
ARRAY_INITIALIZER_LIST	: '{' EXPRESSION_LIST '}' { $$ = new ArrayInitializerList($2); }
						;

/**************************** STRUCT ****************************/

STRUCT_SPECIFIER	: STRUCT IDENTIFIER '{' DECLARATION_STATEMENT_LIST '}'	{ $$ = new StructSpecifier($2, $4); }
					| STRUCT '{' DECLARATION_STATEMENT_LIST '}'				{ $$ = new StructSpecifier(NULL, $3); }
					| STRUCT IDENTIFIER										{ $$ = new StructSpecifier($2, NULL); }
					;


DECLARATION_STATEMENT_LIST	: DECLARATION_STATEMENT { $$ = new DeclarationStatementList($1); }
							| DECLARATION_STATEMENT_LIST DECLARATION_STATEMENT { $1->listAppend($2); $$ = $1;}
							;


/**************************** ENUMS ****************************/

ENUM_SPECIFIER	: ENUM '{' ENUM_LIST '}'				{ $$ = new EnumSpecifier(NULL, $3); }
				| ENUM IDENTIFIER '{' ENUM_LIST '}'		{ $$ = new EnumSpecifier($2, $4); }
				| ENUM IDENTIFIER						{ $$ = new EnumSpecifier($2, NULL); }
				;

// list of ENUMERATORs
ENUM_LIST	: ENUMERATOR				{ $$ = new EnumList($1); }
			| ENUM_LIST ',' ENUMERATOR	{ $1->listAppend($3); $$ = $1; }
			;

// a or a = 3
ENUMERATOR	: IDENTIFIER				{ $$ = new Enumerator($1); }
			| IDENTIFIER '=' EXPRESSION	{ $$ = new Enumerator($1, $3); }
			;

// const int a
DECLARATION	: DECLARATION_SPECIFIER IDENTIFIER { $$ = new Declaration($1, $2, false); }
			| DECLARATION_SPECIFIER IDENTIFIER '[' EXPRESSION ']' { $$ = new Declaration($1, $2, $4); }
			| DECLARATION_SPECIFIER '*' IDENTIFIER { $$ = new Declaration($1, $3, true); }
			| DECLARATION_SPECIFIER // TODO: whats this for again? , maybe for f(int, int) decl?
			;
			
		
// const unsigned long long
DECLARATION_SPECIFIER 	: PRIMITIVE_TYPE { $$ = new DeclarationSpecifier($1); }
						| DECLARATION_SPECIFIER PRIMITIVE_TYPE  { $1->listAppend($2); $$ = $1; }
						;


TYPEDEF_STATEMENT	: TYPEDEF DECLARATION_SPECIFIER IDENTIFIER ';'		{ 
							auto declSpec = dynamic_cast<DeclarationSpecifier*>($2);
							// struct
							if(declSpec->getTypename() == "struct"){
								auto prim = dynamic_cast<PrimitiveType*>(declSpec->branches[0]);
								auto structSpec = dynamic_cast<StructSpecifier*>(prim->branches[0]);
								if (structSpec->getType() == "anonymous"){
									structSpec->setIdentifier($3);
									std::cerr << "**** set struct ident to " << *$3 << std::endl;
									Program::insertTypedef($3, structSpec->getRawString(), false); 
								}

								// NOT IMPLEMENTING: double alias on struct
								
							} 
							// not struct
							else {
								//                 ident  declSpec  isPointer?
								Program::insertTypedef($3, declSpec->getTypename(), false); 
							}

							// return dummy AST node for debugging
							$$ = new TypedefStatement($2, $3, false);
						}

					| TYPEDEF DECLARATION_SPECIFIER '*' IDENTIFIER ';'	{
							auto declSpec = dynamic_cast<DeclarationSpecifier*>($2);
							// struct
							if(declSpec->getTypename() == "struct"){
								auto prim = dynamic_cast<PrimitiveType*>(declSpec->branches[0]);
								auto structSpec = dynamic_cast<StructSpecifier*>(prim->branches[0]);
								if (structSpec->getType() == "anonymous"){
									structSpec->setIdentifier($4);
									std::cerr << "**** set struct ident to " << *$4 << std::endl;
									Program::insertTypedef($4, structSpec->getRawString(), true); 
								}

								// NOT IMPLEMENTING: double alias on struct
								
							} 
							// not struct
							else {
								//                 ident  declSpec  isPointer?
								Program::insertTypedef($4, declSpec->getTypename(), true); 
							}

							// return dummy AST node for debugging
							$$ = new TypedefStatement($2, $4, true);
						}

PRIMITIVE_TYPE	: SHORT 			{ $$ = new PrimitiveType($1); }
				| SIGNED 			{ $$ = new PrimitiveType($1); }
				| UNSIGNED 			{ $$ = new PrimitiveType($1); }
				| INT 				{ $$ = new PrimitiveType($1); }
				| LONG 				{ $$ = new PrimitiveType($1); }
				| FLOAT 			{ $$ = new PrimitiveType($1); }
				| DOUBLE 			{ $$ = new PrimitiveType($1); }
				| ENUM_SPECIFIER	{ $$ = new PrimitiveType($1); }
				| STRUCT_SPECIFIER 	{ $$ = new PrimitiveType($1); }
				| CHAR				{ $$ = new PrimitiveType($1); }
				| VOID				{ $$ = new PrimitiveType($1); }
				| CUSTOM_TYPE		{ $$ = new PrimitiveType($1); }
				;


/**************************** Expressions ****************************/


EXPRESSION	: LOGICAL_OR_EXPRESSION { $$ = $1; }
			| LOGICAL_OR_EXPRESSION '?' EXPRESSION ':' EXPRESSION { $$ = new Expression($1, $3, $5); }
			;

LOGICAL_OR_EXPRESSION	: LOGICAL_AND_EXPRESSION { $$ = $1; }
						| LOGICAL_OR_EXPRESSION OR LOGICAL_AND_EXPRESSION { $$ = new Expression($1,"||",$3); }
						;

LOGICAL_AND_EXPRESSION	: INCLUSIVE_OR_EXPRESSION { $$ = $1; }
						| LOGICAL_AND_EXPRESSION AND INCLUSIVE_OR_EXPRESSION { $$ = new Expression($1,"&&",$3); }
						;

INCLUSIVE_OR_EXPRESSION	: EXCLUSIVE_OR_EXPRESSION { $$ = $1; }
						| INCLUSIVE_OR_EXPRESSION '|' EXCLUSIVE_OR_EXPRESSION { $$ = new Expression($1,"|",$3); }
						;

EXCLUSIVE_OR_EXPRESSION	: AND_EXPRESSION { $$ = $1; }
						| EXCLUSIVE_OR_EXPRESSION '^' AND_EXPRESSION { $$ = new Expression($1,"^",$3); }
						;

AND_EXPRESSION	: EQUALITY_EXPRESSION { $$ = $1; }
				| AND_EXPRESSION '&' EQUALITY_EXPRESSION { $$ = new Expression($1,"&",$3); }
				;

EQUALITY_EXPRESSION	: RELATIONAL_EXPRESSION { $$ = $1; }
					| EQUALITY_EXPRESSION EQ RELATIONAL_EXPRESSION { $$ = new Expression($1,"==",$3); }
					| EQUALITY_EXPRESSION NOTEQ RELATIONAL_EXPRESSION { $$ = new Expression($1,"!=",$3); }
					;

RELATIONAL_EXPRESSION	: SHIFT_EXPRESSION { $$ = $1; }
						| RELATIONAL_EXPRESSION LESSTHAN SHIFT_EXPRESSION { $$ = new Expression($1,"<",$3); }
						| RELATIONAL_EXPRESSION GREATERTHAN SHIFT_EXPRESSION { $$ = new Expression($1,">",$3); }
						| RELATIONAL_EXPRESSION LESSTHANEQ SHIFT_EXPRESSION { $$ = new Expression($1,"<=",$3); }
						| RELATIONAL_EXPRESSION GREATERTHANEQ SHIFT_EXPRESSION { $$ = new Expression($1,">=",$3); }
						;

SHIFT_EXPRESSION	: ADDITIVE_EXPRESSION { $$ = $1; }
					| SHIFT_EXPRESSION BIT_SHIFTL ADDITIVE_EXPRESSION { $$ = new Expression($1,"<<",$3); }
					| SHIFT_EXPRESSION BIT_SHIFTR ADDITIVE_EXPRESSION { $$ = new Expression($1,">>",$3); }
					;

ADDITIVE_EXPRESSION	: MULTIPLICATIVE_EXPRESSION { $$ = $1; }
					| ADDITIVE_EXPRESSION '+' MULTIPLICATIVE_EXPRESSION { $$ = new Expression($1,"+",$3); }
					| ADDITIVE_EXPRESSION '-' MULTIPLICATIVE_EXPRESSION { $$ = new Expression($1,"-",$3); }
					;

MULTIPLICATIVE_EXPRESSION	: PREFIX_EXPRESSION { $$ = $1; }
							| MULTIPLICATIVE_EXPRESSION '*' PREFIX_EXPRESSION { $$ = new Expression($1,"*",$3); }
							| MULTIPLICATIVE_EXPRESSION '/' PREFIX_EXPRESSION { $$ = new Expression($1,"/",$3); }
							| MULTIPLICATIVE_EXPRESSION '%' PREFIX_EXPRESSION { $$ = new Expression($1,"%",$3); }
							;

PREFIX_EXPRESSION	: POSTFIX_EXPRESSION 					{ $$ = $1; }
					| INC_OP PREFIX_EXPRESSION				{ $$ = new Expression("++", $2); }
					| DEC_OP PREFIX_EXPRESSION				{ $$ = new Expression("--", $2); }
					| UNARY_OPERATOR PREFIX_EXPRESSION 		{ $$ = new Expression($1, $2); }
					| SIZEOF PREFIX_EXPRESSION				{ $$ = new Expression("sizeof", $2); }
					| SIZEOF '(' DECLARATION_SPECIFIER ')' 	{ $$ = new Expression("sizeof", $3); } // size of long long
					;


POSTFIX_EXPRESSION	: PRIMARY_EXPRESSION 							{ $$ = $1; }
					| POSTFIX_EXPRESSION '[' EXPRESSION ']' 		{ $$ = new Expression($1, $3, "[]"); } // array indexing
					| POSTFIX_EXPRESSION '(' ')'					{ $$ = new Expression($1, "()"); }
					| POSTFIX_EXPRESSION '(' EXPRESSION_LIST ')' 	{ $$ = new Expression($1, $3, "()"); }
					| POSTFIX_EXPRESSION '.' IDENTIFIER				{ $$ = new Expression($1, $3, "."); }
					| POSTFIX_EXPRESSION PTR_OP IDENTIFIER			{ $$ = new Expression($1, $3, "->"); }
					| POSTFIX_EXPRESSION INC_OP						{ $$ = new Expression($1, "++"); }
					| POSTFIX_EXPRESSION DEC_OP						{ $$ = new Expression($1, "--"); }
					;

EXPRESSION_LIST 	: EXPRESSION { $$ = new ExpressionList($1); }
					| EXPRESSION_LIST ',' EXPRESSION { $1->listAppend($3); $$ = $1; }
					;

PRIMARY_EXPRESSION	: IDENTIFIER   			{ $$ = new Expression($1); }
					| CONSTANT     			{ $$ = new Expression($1); }
					| STRING_LITERAL		{ $$ = new Expression(true, $1); }
					| CHAR_LITERAL			{ $$ = new Expression($1); }
					| '(' EXPRESSION ')' 	{ $$ = $2; }
					;
					
UNARY_OPERATOR 	: '-' 		{ $$ = new UnaryOperator("-"); }
				| '!'		{ $$ = new UnaryOperator("!"); }
				| '~'		{ $$ = new UnaryOperator("~"); }
				| '&'		{ $$ = new UnaryOperator("&"); }
				| '+'		{ $$ = new UnaryOperator("+"); } 
				| '*'		{ $$ = new UnaryOperator("*"); }
				;


CONSTANT		: T_INT			{ $$ = new Constant($1); }
				| T_FLOAT		{ $$ = new Constant($1); }
				| T_DOUBLE		{ $$ = new Constant($1); }
				;



/**************************** STATEMENTS & ASSIGNMENTS ****************************/

COMPOUND_STATEMENT 	: '{' '}' { $$ = new CompoundStatement(NULL); }
					| '{' STATEMENT_LIST '}' { $$ = new CompoundStatement($2); }
					;

STATEMENT_LIST		: STATEMENT { $$ = new StatementList($1); }
					| STATEMENT_LIST STATEMENT { $1->listAppend($2); $$ = $1; } // TO TEST
					;

STATEMENT 	: COMPOUND_STATEMENT 	{ $$ = new Statement($1); }
			| ITERATION_STATEMENT 	{ $$ = new Statement($1); }
			| BRANCH_STATEMENT 		{ $$ = new Statement($1); }
			| ASSIGNMENT_STATEMENT 	{ $$ = new Statement($1); }
			| DECLARATION_STATEMENT { $$ = new Statement($1); }
			| RETURN_STATEMENT 		{ $$ = new Statement($1); }
			| JUMP_STATEMENT		{ $$ = new Statement($1); }
			| LABELLED_STATEMENT	{ $$ = new Statement($1); }
			;

// const int a;
DECLARATION_STATEMENT	: DECLARATION ';'	{ $$ = new DeclarationStatement($1); }
						| ';'				{ $$ = new DeclarationStatement(NULL); }
						;


// const int a = 0; | a = 0;
ASSIGNMENT_STATEMENT	: PREFIX_EXPRESSION ASSIGNMENT_OP EXPRESSION ';' { $$ = new AssignmentStatement($1, $2, $3); }
						| DECLARATION ASSIGNMENT_OP EXPRESSION ';' { $$ = new AssignmentStatement($1, $2, $3); }
						| DECLARATION '=' ARRAY_INITIALIZER_LIST ';' { $$ = new AssignmentStatement($1, $3); }
						| PREFIX_EXPRESSION ';'
						;

RETURN_STATEMENT	: RETURN EXPRESSION ';' { $$ = new ReturnStatement($2); }

ASSIGNMENT_OP	: '='			{ $$ = new AssignmentOperator("="); }
				| ADD_ASSIGN	{ $$ = new AssignmentOperator("+="); }
				| SUB_ASSIGN	{ $$ = new AssignmentOperator("-="); }
				| MUL_ASSIGN 	{ $$ = new AssignmentOperator("*="); }
				| DIV_ASSIGN	{ $$ = new AssignmentOperator("/="); }
				| MOD_ASSIGN	{ $$ = new AssignmentOperator("%="); }
				| AND_ASSIGN	{ $$ = new AssignmentOperator("&="); }
				| OR_ASSIGN		{ $$ = new AssignmentOperator("|="); }
				| XOR_ASSIGN 	{ $$ = new AssignmentOperator("^="); }
				| SHL_ASSIGN	{ $$ = new AssignmentOperator("<<="); }
				| SHR_ASSIGN	{ $$ = new AssignmentOperator(">>="); }
				;

// used for FOR loops only
EXPRESSION_STATEMENT	: ';'					{ $$ = new ExpressionStatement(NULL); }
						| EXPRESSION ';'		{ $$ = new ExpressionStatement($1); }
						| ASSIGNMENT_STATEMENT	{ $$ = new ExpressionStatement($1); }
						;

// used for FOR loops only
ASSIGNMENT_EXPRESSION	: EXPRESSION { $$ = new AssignmentExpression($1); }
						| PREFIX_EXPRESSION ASSIGNMENT_OP EXPRESSION { $$ = new AssignmentExpression($1, $2, $3); }
						;

// ITERATION STATMENT AST CONSTRUCTOR: "type of loop", <condition>, <body>, <optional start statement?> <optional increment?>
ITERATION_STATEMENT : WHILE '(' EXPRESSION ')' STATEMENT { $$ = new IterationStatement("while", $3, $5, NULL, NULL); }
					| DO STATEMENT WHILE '(' EXPRESSION ')' ';' { $$ = new IterationStatement("dowhile", $5, $2, NULL, NULL); }
					| FOR '(' EXPRESSION_STATEMENT EXPRESSION_STATEMENT ')' STATEMENT { $$ = new IterationStatement("for", $4, $6, $3, NULL); }
					| FOR '(' EXPRESSION_STATEMENT EXPRESSION_STATEMENT ASSIGNMENT_EXPRESSION ')' STATEMENT { $$ = new IterationStatement("for", $4, $7, $3, $5); }
					;

					
BRANCH_STATEMENT 	: IF  '(' EXPRESSION ')' STATEMENT ELSE STATEMENT 	{ $$ = new BranchStatement($3, $5, $7); }
					| IF '(' EXPRESSION ')' STATEMENT 					{ $$ = new BranchStatement($3, $5); }
					| SWITCH '(' EXPRESSION ')' STATEMENT				{ $$ = new BranchStatement("switch", $3, $5); }
					;

LABELLED_STATEMENT	: CASE EXPRESSION ':' STATEMENT	{ $$ = new LabelledStatement($2, $4); }
					| DEFAULT ':' STATEMENT			{ $$ = new LabelledStatement(NULL, $3); }
					;

JUMP_STATEMENT	: CONTINUE ';'	{ $$ = new JumpStatement("continue"); }
				| BREAK ';'		{ $$ = new JumpStatement("break"); }
				;


%%


Node *g_root; // Definition of IDENTIFIER (to match declaration earlier)

Node *parseAST()
{
  g_root=0;
  yyparse();
  return g_root;
}
