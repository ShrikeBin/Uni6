-- ring.adb
-- Generic Ring modulo N — package body
with Ada.Text_IO;

package body Ring is

   -- ── helpers ────────────────────────────────────────────────────────────────

   function Normalize (V : Integer) return Integer is
      R : Integer := V mod N;
   begin
      if R < 0 then R := R + N; end if;
      return R;
   end Normalize;

   -- Extended Euclidean: returns gcd, sets S so that A*S ≡ gcd (mod N)
   procedure Extended_GCD (A, B  :     Integer;
                            G, S  : out Integer) is
      Old_R : Integer := A;
      R     : Integer := B;
      Old_S : Integer := 1;
      Ss    : Integer := 0;
      Q, Tmp : Integer;
   begin
      while R /= 0 loop
         Q     := Old_R / R;
         Tmp   := R;   R   := Old_R - Q * R;   Old_R := Tmp;
         Tmp   := Ss;  Ss  := Old_S - Q * Ss;  Old_S := Tmp;
      end loop;
      G := Old_R;
      S := Old_S;
   end Extended_GCD;

   -- ── Conversion ─────────────────────────────────────────────────────────────

   function To_Element (Value : Integer) return Element is
   begin
      return (Value => Normalize (Value));
   end To_Element;

   function To_Integer (E : Element) return Integer is
   begin
      return E.Value;
   end To_Integer;

   -- ── Arithmetic ─────────────────────────────────────────────────────────────

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

   function "/" (Left, Right : Element) return Element is
   begin
      if Right.Value = 0 then
         raise Division_By_Zero;
      end if;
      return Left * Inverse (Right);
   end "/";

   -- ── Modular inverse ────────────────────────────────────────────────────────

   function Inverse (E : Element) return Element is
      G, S : Integer;
   begin
      if E.Value = 0 then
         raise No_Inverse;
      end if;
      Extended_GCD (E.Value, N, G, S);
      if G /= 1 then
         raise No_Inverse;
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

   -- ── Comparisons ────────────────────────────────────────────────────────────

   function "="  (Left, Right : Element) return Boolean is
   begin return Left.Value =  Right.Value; end "=";

   function "<"  (Left, Right : Element) return Boolean is
   begin return Left.Value <  Right.Value; end "<";

   function "<=" (Left, Right : Element) return Boolean is
   begin return Left.Value <= Right.Value; end "<=";

   function ">"  (Left, Right : Element) return Boolean is
   begin return Left.Value >  Right.Value; end ">";

   function ">=" (Left, Right : Element) return Boolean is
   begin return Left.Value >= Right.Value; end ">=";

   -- ── Utility ────────────────────────────────────────────────────────────────

   function Modulus return Positive is
   begin return N; end Modulus;

   procedure Put (E : Element) is
   begin
      Ada.Text_IO.Put (Integer'Image (E.Value) & " [mod" & Integer'Image (N) & "]");
   end Put;

   procedure Put_Line (E : Element) is
   begin
      Put (E);
      Ada.Text_IO.New_Line;
   end Put_Line;

end Ring;