with Interfaces.C; use Interfaces.C;
package Mathlib is
   type Natural_64  is mod 2**64;
   type Integer_64  is range -2**63 .. 2**63 - 1;

   function GCD(A, B : Natural_64) return Natural_64
     with Convention => C, Export => True,
          External_Name => "mathlib__gcd";

   function Smallest_Prime_Divisor(N : Natural_64) return Natural_64
     with Convention => C, Export => True,
          External_Name => "mathlib__smallest_prime_divisor";

   function Euler_Totient(N : Natural_64) return Natural_64
     with Convention => C, Export => True,
          External_Name => "mathlib__euler_totient";

   type Diophantine_Solution is record
      Has_Solution : int;
      X, Y         : long_long;
   end record
     with Convention => C;

   function Solve_Diophantine(A, B, C : Integer_64) return Diophantine_Solution
     with Convention => C, Export => True,
          External_Name => "mathlib__solve_diophantine";
end Mathlib;