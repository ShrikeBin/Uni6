with Interfaces.C; use Interfaces.C;

package Rust_Binding is
   pragma Pure;

   type Rust_Dioph_Sol is record
      Has_Solution : int;
      X            : long;
      Y            : long;
   end record;
   pragma Convention (C, Rust_Dioph_Sol);

   function GCD (A, B : unsigned_long) return unsigned_long;
   pragma Import (C, GCD, "rust_gcd");

   function Smallest_Prime_Divisor (N : unsigned_long) return unsigned_long;
   pragma Import (C, Smallest_Prime_Divisor, "rust_smallest_prime_divisor");

   function Euler_Totient (N : unsigned_long) return unsigned_long;
   pragma Import (C, Euler_Totient, "rust_euler_totient");

   function Solve_Diophantine (A, B, C : long) return Rust_Dioph_Sol;
   pragma Import (C, Solve_Diophantine, "rust_solve_diophantine");

end Rust_Binding;