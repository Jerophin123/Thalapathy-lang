import subprocess
import os

compiler_path = "./build-release-audit/thalapathy.exe"

test_cases = {
    "unsupported_extension": {
        "file": "temp_err.py",
        "code": "print('hello')",
        "expected_diag": "THALA-SOURCE-001",
        "phase": "File validation"
    },
    "unterminated_string": {
        "file": "temp_err.tvk",
        "code": 'thalapathy aarambam { nanba x = "hello; }',
        "expected_diag": "lexical error: unterminated string literal",
        "phase": "Lexing"
    },
    "invalid_character": {
        "file": "temp_err.tvk",
        "code": 'thalapathy aarambam { nanba x = @; }',
        "expected_diag": "lexical error: unexpected character",
        "phase": "Lexing"
    },
    "invalid_char_literal": {
        "file": "temp_err.tvk",
        "code": "thalapathy aarambam { char x = 'ab'; }",
        "expected_diag": "lexical error: unterminated or invalid character literal",
        "phase": "Lexing"
    },
    "missing_semicolon": {
        "file": "temp_err.tvk",
        "code": "thalapathy aarambam { sollu(1) }",
        "expected_diag": "parser error: expected ';'",
        "phase": "Parsing"
    },
    "malformed_expression": {
        "file": "temp_err.tvk",
        "code": "thalapathy aarambam { sollu(+); }",
        "expected_diag": "parser error: unexpected expression start",
        "phase": "Parsing"
    },
    "undefined_variable": {
        "file": "temp_err.tvk",
        "code": "thalapathy aarambam { sollu(xyz); }",
        "expected_diag": "THALA-NAME-002",
        "phase": "Semantic analysis"
    },
    "duplicate_same_scope_decl": {
        "file": "temp_err.tvk",
        "code": "thalapathy aarambam { makkal x = 10; makkal x = 20; }",
        "expected_diag": "THALA-NAME-001",
        "phase": "Semantic analysis"
    },
    "immutable_nanba_reassignment": {
        "file": "temp_err.tvk",
        "code": "thalapathy aarambam { nanba x = 10; x = 20; }",
        "expected_diag": "THALA-MUT-001",
        "phase": "Semantic analysis"
    },
    "uruthi_reassignment": {
        "file": "temp_err.tvk",
        "code": "thalapathy aarambam { uruthi x = 10; x = 20; }",
        "expected_diag": "THALA-MUT-002",
        "phase": "Semantic analysis"
    },
    "explicit_type_mismatch": {
        "file": "temp_err.tvk",
        "code": 'thalapathy aarambam { int x = "hello"; }',
        "expected_diag": "THALA-TYPE-001",
        "phase": "Semantic analysis"
    },
    "invalid_operator_operands": {
        "file": "temp_err.tvk",
        "code": 'thalapathy aarambam { sollu(1 + "hello"); }',
        "expected_diag": "THALA-TYPE-001",
        "phase": "Semantic analysis"
    },
    "wrong_function_arity": {
        "file": "temp_err.tvk",
        "code": "thalapathy aarambam { range(1); }",
        "expected_diag": "THALA-FUNC-004",
        "phase": "Semantic analysis"
    },
    "return_outside_function": {
        "file": "temp_err.tvk",
        "code": "thalapathy aarambam { thiruppi; }",
        "expected_diag": "THALA-FUNC-001",
        "phase": "Semantic analysis"
    },
    "return_type_mismatch": {
        "file": "temp_err.tvk",
        "code": 'mersal f() -> int { thiruppi "hello"; } thalapathy aarambam {}',
        "expected_diag": "THALA-TYPE-001",
        "phase": "Semantic analysis"
    },
    "break_outside_loop": {
        "file": "temp_err.tvk",
        "code": "thalapathy aarambam { break; }",
        "expected_diag": "THALA-LOOP-001",
        "phase": "Semantic analysis"
    },
    "continue_outside_loop": {
        "file": "temp_err.tvk",
        "code": "thalapathy aarambam { continue; }",
        "expected_diag": "THALA-LOOP-001",
        "phase": "Semantic analysis"
    },
    "division_by_zero": {
        "file": "temp_err.tvk",
        "code": "thalapathy aarambam { makkal x = 10 / 0; }",
        "expected_diag": "Runtime error: division by zero",
        "phase": "Runtime"
    },
    "array_index_out_of_bounds": {
        "file": "temp_err.tvk",
        "code": "thalapathy aarambam { makkal x = [1]; sollu(x[2]); }",
        "expected_diag": "Runtime error: array index out of bounds",
        "phase": "Runtime"
    },
    "invalid_property": {
        "file": "temp_err.tvk",
        "code": "master H {} thalapathy aarambam { makkal h = H(); sollu(h.age); }",
        "expected_diag": "Member 'age' not found",
        "phase": "Runtime"
    },
    "missing_method": {
        "file": "temp_err.tvk",
        "code": "master H {} thalapathy aarambam { makkal h = H(); h.test(); }",
        "expected_diag": "Member 'test' not found",
        "phase": "Runtime"
    },
    "super_without_inheritance": {
        "file": "temp_err.tvk",
        "code": "master H { mersal f() { super.test(); } } thalapathy aarambam {}",
        "expected_diag": "THALA-CLASS-004",
        "phase": "Semantic analysis"
    },
    "inheritance_cycle": {
        "file": "temp_err.tvk",
        "code": "master A varisu B {} master B varisu A {} thalapathy aarambam {}",
        "expected_diag": "THALA-CLASS-001",
        "phase": "Semantic analysis"
    },
    "duplicate_aarambam": {
        "file": "temp_err.tvk",
        "code": "thalapathy aarambam {} thalapathy aarambam {}",
        "expected_diag": "THALA-FUNC-002",
        "phase": "Semantic analysis"
    }
}

print("| Test Case | Phase | Command | Exit Code | Diagnostic Found? | Result |")
print("|---|---|---|---|---|---|")

for name, info in test_cases.items():
    filepath = info["file"]
    with open(filepath, "w") as f:
        f.write(info["code"])
    
    cmd = [compiler_path, "run", filepath, "--mode", "professional"]
    res = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    err_output = res.stderr or res.stdout
    diag_found = info["expected_diag"] in err_output
    
    # clean up file
    if os.path.exists(filepath):
        os.remove(filepath)
        
    result_str = "PASS" if diag_found else "FAIL"
    print(f"| {name} | {info['phase']} | `thalapathy run {filepath}` | {res.returncode} | {diag_found} (expected: '{info['expected_diag']}') | {result_str} |")
