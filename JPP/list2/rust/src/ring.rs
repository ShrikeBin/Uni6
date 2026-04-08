#[derive(Debug, Clone, Copy)]
pub struct Ring<const N: u64> {
    pub value: u64,
}

impl<const N: u64> Ring<N> {
    pub fn new(val: i64) -> Self {
        if N == 0 {
            panic!("Modulus N cannot be zero.");
        }
        let m = N as i64;
        Ring { value: (((val % m) + m) % m) as u64 }
    }

    fn extended_gcd(a: i64, b: i64) -> (i64, i64, i64) {
        if a == 0 {
            (b, 0, 1)
        } else {
            let (gcd, x1, y1) = Self::extended_gcd(b % a, a);
            let x = y1 - (b / a) * x1;
            let y = x1;
            (gcd, x, y)
        }
    }
}

impl<const N: u64> std::cmp::PartialEq for Ring<N> {
    fn eq(&self, other: &Self) -> bool { self.value == other.value }
}

impl<const N: u64> std::ops::Neg for Ring<N> {
    type Output = Self;
    fn neg(self) -> Self {
        Ring { value: (N - self.value) % N }
    }
}

impl<const N: u64> std::ops::Add for Ring<N> {
    type Output = Self;
    fn add(self, rhs: Self) -> Self {
        Ring { value: (self.value + rhs.value) % N }
    }
}

impl<const N: u64> std::ops::Sub for Ring<N> {
    type Output = Self;
    fn sub(self, rhs: Self) -> Self {
        Ring { value: (self.value + N - rhs.value) % N }
    }
}

impl<const N: u64> std::ops::Mul for Ring<N> {
    type Output = Self;
    fn mul(self, rhs: Self) -> Self {
        Ring { value: (self.value * rhs.value) % N }
    }
}

impl<const N: u64> std::ops::Div for Ring<N> {
    type Output = Result<Self, String>;
    fn div(self, rhs: Self) -> Self::Output {
        if rhs.value == 0 {
            return Err(String::from("Division by zero"));
        }

        let (gcd, x, _) = Self::extended_gcd(rhs.value as i64, N as i64);

        if gcd != 1 {
            return Err(format!("No modular inverse: gcd({}, {}) != 1", rhs.value, N));
        }

        let inverse = Ring::<N>::new(x);
        Ok(self * inverse)
    }
}

impl<const N: u64> std::fmt::Display for Ring<N> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{} [mod{}]", self.value, N)
    }
}