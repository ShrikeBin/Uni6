with Interfaces.C; use Interfaces.C;

package C_Binding is
   pragma Pure;

   -- Mirror of C DiophantineSolution
   type C_Dioph_Sol is record
      Has_Solution : int;
      X            : long;
      Y            : long;
   end record;
   pragma Convention (C, C_Dioph_Sol);

   function GCD (A, B : unsigned_long) return unsigned_long;
   pragma Import (C, GCD, "gcd");

   function Smallest_Prime_Divisor (N : unsigned_long) return unsigned_long;
   pragma Import (C, Smallest_Prime_Divisor, "smallest_prime_divisor");

   function Euler_Totient (N : unsigned_long) return unsigned_long;
   pragma Import (C, Euler_Totient, "euler_totient");

   function Solve_Diophantine (A, B, C : long) return C_Dioph_Sol;
   pragma Import (C, Solve_Diophantine, "solve_diophantine");

end C_Binding;