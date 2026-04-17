-- rsa.adb
with Ada.Numerics.Discrete_Random;
with Ada.Text_IO; use Ada.Text_IO;

package body RSA_Pkg is

   -- ── Internal key storage ──────────────────────────────────────────────────
   Public_Key  : Positive := 3;
   Private_Key : Positive := 3;

   -- ── Arithmetic helpers ────────────────────────────────────────────────────

   -- Extended GCD: returns gcd(A, B), sets X s.t. A*X ≡ gcd (mod B)
   procedure Extended_GCD (A, B :     Long_Long_Integer;
                            G    : out Long_Long_Integer;
                            X    : out Long_Long_Integer) is
      Old_R : Long_Long_Integer := A;
      R     : Long_Long_Integer := B;
      Old_X : Long_Long_Integer := 1;
      Xx    : Long_Long_Integer := 0;
      Q, T  : Long_Long_Integer;
   begin
      while R /= 0 loop
         Q     := Old_R / R;
         T     := R;   R   := Old_R - Q * R;   Old_R := T;
         T     := Xx;  Xx  := Old_X - Q * Xx;  Old_X := T;
      end loop;
      G := Old_R;
      X := Old_X;
   end Extended_GCD;

   function GCD (A, B : Long_Long_Integer) return Long_Long_Integer is
      G, X : Long_Long_Integer;
   begin
      Extended_GCD (A, B, G, X);
      return G;
   end GCD;

   function Normalize (V, Mod_Val : Long_Long_Integer) return Long_Long_Integer is
      R : Long_Long_Integer := V mod Mod_Val;
   begin
      if R < 0 then R := R + Mod_Val; end if;
      return R;
   end Normalize;

   -- Fast modular exponentiation: Base^Exp mod Modulus
   function Fast_Pow (Base, Exp, Modulus : Long_Long_Integer)
                      return Long_Long_Integer is
      B : Long_Long_Integer := Base mod Modulus;
      E : Long_Long_Integer := Exp;
      R : Long_Long_Integer := 1;
   begin
      while E > 0 loop
         if E mod 2 = 1 then
            R := (R * B) mod Modulus;
         end if;
         B := (B * B) mod Modulus;
         E := E / 2;
      end loop;
      return R;
   end Fast_Pow;

   -- ── Init ──────────────────────────────────────────────────────────────────

   procedure Init is
      subtype E_Range is Positive range 2 .. Phi - 1;
      package Rand_E is new Ada.Numerics.Discrete_Random (E_Range);
      Gen : Rand_E.Generator;
      E   : Long_Long_Integer;
      D   : Long_Long_Integer;
      G   : Long_Long_Integer;
      X   : Long_Long_Integer;
      Phi_LLI : constant Long_Long_Integer := Long_Long_Integer (Phi);
   begin
      Rand_E.Reset (Gen);
      loop
         E := Long_Long_Integer (Rand_E.Random (Gen));
         exit when GCD (E, Phi_LLI) = 1;
      end loop;
      Extended_GCD (E, Phi_LLI, G, X);
      D := Normalize (X, Phi_LLI);

      Public_Key  := Positive (E);
      Private_Key := Positive (D);
   end Init;

   -- ── Public interface ──────────────────────────────────────────────────────

   function Get_Modulo return Positive is
   begin return N; end Get_Modulo;

   function Get_Public_Key return Positive is
   begin return Public_Key; end Get_Public_Key;

   function Encrypt (M : Message) return Message is
      C : Long_Long_Integer;
   begin
      C := Fast_Pow (Long_Long_Integer (M),
                     Long_Long_Integer (Public_Key),
                     Long_Long_Integer (N));
      return Message (C);
   end Encrypt;

   function Decrypt (S : Message) return Message is
      M : Long_Long_Integer;
   begin
      M := Fast_Pow (Long_Long_Integer (S),
                     Long_Long_Integer (Private_Key),
                     Long_Long_Integer (N));
      return Message (M);
   end Decrypt;

end RSA_Pkg;