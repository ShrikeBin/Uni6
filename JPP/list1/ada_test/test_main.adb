with Ada.Text_IO;        use Ada.Text_IO;
with Interfaces.C;       use Interfaces.C;
with C_Binding;
with Rust_Binding;
with Mathlib;

procedure Test_Main is

   procedure Print_Dioph
     (Lib : String; Has : Integer; X, Y : Long_Integer) is
   begin
      if Has /= 0 then
         Put_Line ("  [" & Lib & "] x=" & Long_Integer'Image (X) &
                   ", y=" & Long_Integer'Image (Y));
      else
         Put_Line ("  [" & Lib & "] No natural solution");
      end if;
   end Print_Dioph;

begin
   Put_Line ("=== GCD(48, 18) ===");
   Put_Line ("  Ada:  " & Mathlib.Natural_64'Image (Mathlib.GCD (48, 18)));
   Put_Line ("  C:    " & unsigned_long'Image (C_Binding.GCD (48, 18)));
   Put_Line ("  Rust: " & unsigned_long'Image (Rust_Binding.GCD (48, 18)));

   Put_Line ("");
   Put_Line ("=== Smallest prime divisor of 91 ===");
   Put_Line ("  Ada:  " & Mathlib.Natural_64'Image
               (Mathlib.Smallest_Prime_Divisor (91)));
   Put_Line ("  C:    " & unsigned_long'Image
               (C_Binding.Smallest_Prime_Divisor (91)));
   Put_Line ("  Rust: " & unsigned_long'Image
               (Rust_Binding.Smallest_Prime_Divisor (91)));

   Put_Line ("");
   Put_Line ("=== Euler totient of 36 ===");
   Put_Line ("  Ada:  " & Mathlib.Natural_64'Image (Mathlib.Euler_Totient (36)));
   Put_Line ("  C:    " & unsigned_long'Image (C_Binding.Euler_Totient (36)));
   Put_Line ("  Rust: " & unsigned_long'Image (Rust_Binding.Euler_Totient (36)));

   Put_Line ("");
   Put_Line ("=== Diophantine: 5x - 3y = 1 ===");
   declare
      AS : constant Mathlib.Diophantine_Solution :=
             Mathlib.Solve_Diophantine (5, 3, 1);
      CS : constant C_Binding.C_Dioph_Sol :=
             C_Binding.Solve_Diophantine (5, 3, 1);
      RS : constant Rust_Binding.Rust_Dioph_Sol :=
             Rust_Binding.Solve_Diophantine (5, 3, 1);
   begin
      Print_Dioph ("Ada",   Integer (AS.Has_Solution),
                   Long_Integer (AS.X), Long_Integer (AS.Y));
      Print_Dioph ("C",    Integer (CS.Has_Solution),
                   Long_Integer (CS.X), Long_Integer (CS.Y));
      Print_Dioph ("Rust", Integer (RS.Has_Solution),
                   Long_Integer (RS.X), Long_Integer (RS.Y));
   end;
end Test_Main;