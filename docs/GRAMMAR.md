# THALAPATHY Grammar Spec (EBNF)

```ebnf
program             = { declaration } ;

declaration         = entry_block
                    | class_decl
                    | func_decl
                    | var_decl
                    | statement ;

entry_block         = "thalapathy" "aarambam" block ;

class_decl          = "master" IDENTIFIER [ "varisu" IDENTIFIER ] "{" { class_member } "}" ;
class_member        = func_decl | typed_field_decl ;
typed_field_decl    = primitive_type IDENTIFIER ";" ;

func_decl           = "mersal" IDENTIFIER "(" [ param_list ] ")" [ "->" primitive_type ] block ;
param_list          = param { "," param } ;
param               = primitive_type IDENTIFIER ;

var_decl            = ( "nanba" | "makkal" | "uruthi" | primitive_type ) IDENTIFIER [ "=" expression ] ";" ;

primitive_type      = "int" | "float" | "bool" | "string" | "char" | "void" | "null" ;

statement           = block
                    | if_stmt
                    | for_stmt
                    | range_loop
                    | return_stmt
                    | throw_stmt
                    | try_catch
                    | break_stmt
                    | continue_stmt
                    | expr_stmt ;

block               = "{" { declaration } "}" ;
if_stmt             = "ghilli" "(" expression ")" statement [ "illana" statement ] ;
for_stmt            = "bigil" "(" [ var_decl | expr_stmt | ";" ] [ expression ] ";" [ expression ] ")" statement ;
range_loop          = "vaathi" IDENTIFIER "in" "range" "(" expression "," expression ")" statement ;
return_stmt         = "thiruppi" [ expression ] ";" ;
throw_stmt          = "theri" expression ";" ;
try_catch           = "pokkiri" block "kaavalan" "(" IDENTIFIER ")" block ;
break_stmt          = "break" ";" ;
continue_stmt       = "continue" ";" ;
expr_stmt           = expression ";" ;
```
