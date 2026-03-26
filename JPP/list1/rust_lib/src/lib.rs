pub fn gcd(mut a: u64, mut b: u64) -> u64 {
    while b != 0 {
        let t = b;
        b = a % b;
        a = t;
    }
    return a;
}


pub fn smallest_prime_divisor(n: u64) -> u64 {
    assert!(n > 1, "n must be greater than 1");
    if n % 2 == 0 {
        return 2;
    }
    let mut i = 3u64;
    while i * i <= n {
        if n % i == 0 {
            return i;
        }
        i += 2;
    }
    return n;
}

pub fn euler_totient(n: u64) -> u64 {
    let mut result = n;
    let mut temp = n;
    let mut p = 2u64;
    while p * p <= temp {
        if temp % p == 0 {
            while temp % p == 0 {
                temp /= p;
            }
            result -= result / p;
        }
        p += 1;
    }
    if temp > 1 {
        result -= result / temp;
    }
    return result;
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct DiophantineSolution {
    pub has_solution: bool,
    pub x: i64,
    pub y: i64,
}

fn extended_gcd(a: i64, b: i64) -> (i64, i64, i64) {
    if b == 0 {
        return (a, 1, 0);
    } else {
        let (g, x1, y1) = extended_gcd(b, a % b);
        return (g, y1, x1 - (a / b) * y1);
    }
}

fn ceil_div(num: i64, den: i64) -> i64 {
    let q = num / den;
    if num % den != 0 && (num < 0) == (den < 0) {
        return q + 1;
    } else {
        return q;
    }
}

pub fn solve_diophantine(a: i64, b: i64, c: i64) -> DiophantineSolution {
    let no_sol = DiophantineSolution { has_solution: false, x: 0, y: 0 };
    if a <= 0 || b <= 0 {
        return no_sol;
    }
    let (g, mut x0, mut y0) = extended_gcd(a, b);
    if c % g != 0 {
        return no_sol;
    }
    let scale = c / g;
    x0 *= scale;
    y0 *= scale;
    let step_x = b / g;
    let step_y = a / g;

    let t_min_x = ceil_div(1 - x0, step_x);
    let t_min_y = ceil_div(1 + y0, step_y);
    let t = t_min_x.max(t_min_y);

    let x_sol = x0 + step_x * t;
    let y_sol = -y0 + step_y * t;

    if x_sol > 0 && y_sol > 0 {
        return DiophantineSolution { has_solution: true, x: x_sol, y: y_sol };
    } else {
        return no_sol;
    }
}

// -----------------------------------------------------------------
//                           C exports
// -----------------------------------------------------------------

#[repr(C)]
pub struct CDiophantineSolution {
    pub has_solution: i32,
    pub x: i64,
    pub y: i64,
}

#[no_mangle]
pub extern "C" fn rust_gcd(a: u64, b: u64) -> u64 {
    gcd(a, b)
}

#[no_mangle]
pub extern "C" fn rust_smallest_prime_divisor(n: u64) -> u64 {
    smallest_prime_divisor(n)
}

#[no_mangle]
pub extern "C" fn rust_euler_totient(n: u64) -> u64 {
    euler_totient(n)
}

#[no_mangle]
pub extern "C" fn rust_solve_diophantine(a: i64, b: i64, c: i64) -> CDiophantineSolution {
    let s = solve_diophantine(a, b, c);
    CDiophantineSolution {
        has_solution: s.has_solution as i32,
        x: s.x,
        y: s.y,
    }
}