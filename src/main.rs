use std::ffi::CString;
use std::io::{self, Write};
use std::os::raw::c_char;

extern "C" {
    fn jit_compile_and_execute(expr: *const c_char) -> i64;
}

fn is_valid_arithmetic(expr: &str) -> bool {
    let mut paren_count = 0;
    let mut last_char = ' ';
    let mut has_digit = false;

    for c in expr.chars() {
        match c {
            '0'..='9' => has_digit = true,
            '+' | '-' | '*' | '/' => {
                if last_char != ')' && !last_char.is_ascii_digit() {
                    return false; // Operator must follow a digit or closing parenthesis
                }
            }
            '(' => paren_count += 1,
            ')' => {
                if paren_count == 0 || !has_digit || last_char == '(' {
                    return false; // Unmatched or empty parentheses
                }
                paren_count -= 1;
            }
            ' ' => {} // ignore whitespace
            _ => return false, // invalid character
        }
        last_char = c;
    }

    paren_count == 0 && has_digit && (last_char.is_ascii_digit() || last_char == ')')
}

fn main() {
    loop {
        print!("Enter an arithmetic expression (or type 'quit' to exit): ");
        io::stdout().flush().unwrap();

        let mut expr = String::new();
        io::stdin().read_line(&mut expr).unwrap();
        let expr = expr.trim();

        if expr.eq_ignore_ascii_case("quit") {
            break;
        }

        if is_valid_arithmetic(expr) {
            let c_expr = CString::new(expr).unwrap();
            let result = unsafe { jit_compile_and_execute(c_expr.as_ptr()) };
            println!("Result: {}", result);
        } else {
            println!("Invalid expression. Please try again.");
        }
    }
}
