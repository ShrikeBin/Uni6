package Mathlib is

   type Natural_64 is mod 2**64;
   type Integer_64 is range -2**63 .. 2**63 - 1;

   -- 1. Greatest Common Divisor
   function GCD (A, B : Natural_64) return Natural_64;

   -- 2. Smallest prime divisor (N > 1)
   function Smallest_Prime_Divisor (N : Natural_64) return Natural_64;

   -- 3. Euler totient function
   function Euler_Totient (N : Natural_64) return Natural_64;

   -- 4. Diophantine solution structure
   type Diophantine_Solution is record
      Has_Solution : Boolean;
      X            : Integer_64;
      Y            : Integer_64;
   end record;

   -- Solve ax - by = c over natural numbers
   function Solve_Diophantine
     (A, B, C : Integer_64) return Diophantine_Solution;

end Mathlib;