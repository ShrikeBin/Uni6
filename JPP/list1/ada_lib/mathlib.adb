package body Mathlib is

   -- 1. GCD (iterative — for loop unsuitable, count unknown)
   function GCD (A, B : Natural_64) return Natural_64 is
      X : Natural_64 := A;
      Y : Natural_64 := B;
      T : Natural_64;
   begin
      while Y /= 0 loop
         T := Y;
         Y := X mod Y;
         X := T;
      end loop;
      return X;
   end GCD;

   -- 2. Smallest prime divisor
   function Smallest_Prime_Divisor (N : Natural_64) return Natural_64 is
      I : Natural_64 := 3;
   begin
      if N mod 2 = 0 then
         return 2;
      end if;
      -- while loop: bound not fixed at start
      while I * I <= N loop
         if N mod I = 0 then
            return I;
         end if;
         I := I + 2;
      end loop;
      return N;
   end Smallest_Prime_Divisor;

   -- 3. Euler totient
   function Euler_Totient (N : Natural_64) return Natural_64 is
      Result : Natural_64 := N;
      Temp   : Natural_64 := N;
      P      : Natural_64 := 2;
   begin
      -- while loop: prime factors found dynamically
      while P * P <= Temp loop
         if Temp mod P = 0 then
            while Temp mod P = 0 loop
               Temp := Temp / P;
            end loop;
            Result := Result - Result / P;
         end if;
         P := P + 1;
      end loop;
      if Temp > 1 then
         Result := Result - Result / Temp;
      end if;
      return Result;
   end Euler_Totient;

   -- Extended GCD helper
   procedure Extended_GCD
     (A, B   :     Integer_64;
      X, Y   : out Integer_64;
      Result : out Integer_64)
   is
      X1, Y1, G : Integer_64;
   begin
      if B = 0 then
         X      := 1;
         Y      := 0;
         Result := A;
      else
         Extended_GCD (B, A mod B, X1, Y1, G);
         X      := Y1;
         Y      := X1 - (A / B) * Y1;
         Result := G;
      end if;
   end Extended_GCD;

   -- 4. Solve ax - by = c
   function Solve_Diophantine
     (A, B, C : Integer_64) return Diophantine_Solution
   is
      No_Sol : constant Diophantine_Solution := (False, 0, 0);
      X0, Y0, G : Integer_64;
      Scale     : Integer_64;
      Step_X    : Integer_64;
      Step_Y    : Integer_64;
      T         : Integer_64;
      X_Sol     : Integer_64;
      Y_Sol     : Integer_64;

      function Ceil_Div (Num, Den : Integer_64) return Integer_64 is
         Q : Integer_64 := Num / Den;
      begin
         if (Num mod Den /= 0) and then ((Num < 0) = (Den < 0)) then
            Q := Q + 1;
         end if;
         return Q;
      end Ceil_Div;

   begin
      if A <= 0 or B <= 0 then
         return No_Sol;
      end if;
      Extended_GCD (A, B, X0, Y0, G);
      if C mod G /= 0 then
         return No_Sol;
      end if;
      Scale  := C / G;
      X0     := X0 * Scale;
      Y0     := Y0 * Scale;
      Step_X := B / G;
      Step_Y := A / G;

      declare
         T_Min_X : constant Integer_64 := Ceil_Div (1 - X0, Step_X);
         T_Min_Y : constant Integer_64 := Ceil_Div (1 + Y0, Step_Y);
      begin
         if T_Min_X > T_Min_Y then
            T := T_Min_X;
         else
            T := T_Min_Y;
         end if;
      end;

      X_Sol := X0 + Step_X * T;
      Y_Sol := -Y0 + Step_Y * T;

      if X_Sol > 0 and then Y_Sol > 0 then
         return (True, X_Sol, Y_Sol);
      else
         return No_Sol;
      end if;
   end Solve_Diophantine;

end Mathlib;