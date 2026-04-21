with Ada.Numerics.Discrete_Random;

package body DH_Pkg is

   Generator_Val : Long_Long_Integer := 2;

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

   function Normalize (V, Mod_Val : Long_Long_Integer) return Long_Long_Integer is
      R : Long_Long_Integer := V mod Mod_Val;
   begin
      if R < 0 then R := R + Mod_Val; end if;
      return R;
   end Normalize;

   function Mod_Inverse (A, M : Long_Long_Integer) return Long_Long_Integer is
      G, X : Long_Long_Integer;
   begin
      Extended_GCD (A, M, G, X);
      if G /= 1 then raise No_Inverse; end if;
      return Normalize (X, M);
   end Mod_Inverse;

   function Fast_Pow (Base, Exp, Modulus : Long_Long_Integer)
                      return Long_Long_Integer is
      B : Long_Long_Integer := Base mod Modulus;
      E : Long_Long_Integer := Exp;
      R : Long_Long_Integer := 1;
   begin
      while E > 0 loop
         if E mod 2 = 1 then R := (R * B) mod Modulus; end if;
         B := (B * B) mod Modulus;
         E := E / 2;
      end loop;
      return R;
   end Fast_Pow;

   procedure DH_Init is
      subtype G_Range is Positive range 2 .. P - 2;
      package Rand_G is new Ada.Numerics.Discrete_Random (G_Range);
      Gen : Rand_G.Generator;
   begin
      Rand_G.Reset (Gen);
      Generator_Val := Long_Long_Integer (Rand_G.Random (Gen));
   end DH_Init;

   function Get_Generator return Element is
   begin return Element (Generator_Val); end Get_Generator;

   function Field_Power (A : Element; B : Long_Long_Integer) return Element is
   begin
      return Element (Fast_Pow (Long_Long_Integer (A), B,
                                Long_Long_Integer (P)));
   end Field_Power;

   function Make_User return User is
      subtype S_Range is Positive range 2 .. P - 2;
      package Rand_S is new Ada.Numerics.Discrete_Random (S_Range);
      Gen : Rand_S.Generator;
      U   : User;
   begin
      Rand_S.Reset (Gen);
      U.Secret     := Long_Long_Integer (Rand_S.Random (Gen));
      U.Key_Is_Set := False;
      return U;
   end Make_User;

   function Get_Public_Key (U : User) return Element is
   begin
      return Element (Fast_Pow (Generator_Val, U.Secret,
                                Long_Long_Integer (P)));
   end Get_Public_Key;

   procedure Set_Key (U : in out User; A : Element) is
   begin
      U.Shared_Key := Fast_Pow (Long_Long_Integer (A), U.Secret,
                                Long_Long_Integer (P));
      U.Key_Is_Set := True;
   end Set_Key;

   function Encrypt (U : User; M : Element) return Element is
   begin
      if not U.Key_Is_Set then raise Key_Not_Set; end if;
      return Element ((Long_Long_Integer (M) * U.Shared_Key)
                      mod Long_Long_Integer (P));
   end Encrypt;

   function Decrypt (U : User; C : Element) return Element is
      Inv : Long_Long_Integer;
   begin
      if not U.Key_Is_Set then raise Key_Not_Set; end if;
      Inv := Mod_Inverse (U.Shared_Key, Long_Long_Integer (P));
      return Element ((Long_Long_Integer (C) * Inv)
                      mod Long_Long_Integer (P));
   end Decrypt;

end DH_Pkg;