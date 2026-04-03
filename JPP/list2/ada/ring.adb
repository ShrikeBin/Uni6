-- ring.adb
-- Generic Ring modulo N — package body

with Ada.Text_IO; use Ada.Text_IO;

package body Ring is

   -- Reduce any integer to [0, N-1]
   function Normalize (X : Integer) return Integer is
      R : Integer := X mod N;
   begin
      if R < 0 then R := R + N; end if;
      return R;
   end Normalize;

   -- Extended Euclidean: finds GCD and S such that A*S ≡ GCD (mod N)
   procedure Extended_GCD
     (A, B : in  Integer;
      GCD  : out Integer;
      S    : out Integer)
   is
      Old_R : Integer := A;
      R     : Integer := B;
      Old_S : Integer := 1;
      SS    : Integer := 0;
      Q, T  : Integer;
   begin
      while R /= 0 loop
         Q     := Old_R / R;
         T     := R;     R     := Old_R - Q * R;  Old_R := T;
         T     := SS;    SS    := Old_S - Q * SS; Old_S := T;
      end loop;
      GCD := Old_R;
      S   := Old_S;
   end Extended_GCD;

   -- -------------------------------------------------------------------------

   function To_Element (Value : Integer) return Element is
   begin
      return (Value => Normalize (Value));
   end To_Element;

   function To_Integer (E : Element) return Integer is
   begin
      return E.Value;
   end To_Integer;

   function "+" (Left, Right : Element) return Element is
   begin
      return (Value => Normalize (Left.Value + Right.Value));
   end "+";

   function "-" (Left, Right : Element) return Element is
   begin
      return (Value => Normalize (Left.Value - Right.Value));
   end "-";

   function "-" (E : Element) return Element is
   begin
      return (Value => Normalize (-E.Value));
   end "-";

   function "*" (Left, Right : Element) return Element is
   begin
      return (Value => Normalize (Left.Value * Right.Value));
   end "*";

   function Inverse (E : Element) return Element is
      G, S : Integer;
   begin
      if E.Value = 0 then
         raise Division_By_Zero with
           "Cannot compute inverse of zero in ring mod" & N'Image;
      end if;
      Extended_GCD (E.Value, N, G, S);
      if G /= 1 then
         raise No_Inverse with
           "Element" & E.Value'Image &
           " has no inverse in ring mod" & N'Image &
           " (gcd =" & G'Image & ")";
      end if;
      return (Value => Normalize (S));
   end Inverse;

   function Has_Inverse (E : Element) return Boolean is
      G, S : Integer;
   begin
      if E.Value = 0 then return False; end if;
      Extended_GCD (E.Value, N, G, S);
      return G = 1;
   end Has_Inverse;

   function "/" (Left, Right : Element) return Element is
   begin
      if Right.Value = 0 then
         raise Division_By_Zero with "Division by zero in ring mod" & N'Image;
      end if;
      return Left * Inverse (Right);
   end "/";

   function "=" (Left, Right : Element) return Boolean is
   begin
      return Left.Value = Right.Value;
   end "=";

   function "<" (Left, Right : Element) return Boolean is
   begin
      return Left.Value < Right.Value;
   end "<";

   function "<=" (Left, Right : Element) return Boolean is
   begin
      return Left.Value <= Right.Value;
   end "<=";

   function ">" (Left, Right : Element) return Boolean is
   begin
      return Left.Value > Right.Value;
   end ">";

   function ">=" (Left, Right : Element) return Boolean is
   begin
      return Left.Value >= Right.Value;
   end ">=";

   function Modulus return Positive is
   begin
      return N;
   end Modulus;

   procedure Put (E : Element) is
   begin
      Ada.Text_IO.Put (E.Value'Image & " (mod" & N'Image & ")");
   end Put;

   procedure Put_Line (E : Element) is
   begin
      Put (E);
      Ada.Text_IO.New_Line;
   end Put_Line;

end Ring;