-- ring.ads
-- Generic Ring modulo N — package specification

generic
   N : Positive;
package Ring is

   No_Inverse    : exception;
   Division_By_Zero : exception;

   type Element is private;

   -- Conversion
   function To_Element (Value : Integer) return Element;
   function To_Integer (E : Element) return Integer;

   -- Arithmetic
   function "+" (Left, Right : Element) return Element;
   function "-" (Left, Right : Element) return Element;
   function "-" (E : Element) return Element;          -- unary minus
   function "*" (Left, Right : Element) return Element;
   function "/" (Left, Right : Element) return Element; -- raises No_Inverse / Division_By_Zero

   -- Modular inverse
   function Inverse     (E : Element) return Element;  -- raises No_Inverse
   function Has_Inverse (E : Element) return Boolean;

   -- Comparisons (Ada derives /= from = automatically)
   function "="  (Left, Right : Element) return Boolean;
   function "<"  (Left, Right : Element) return Boolean;
   function "<=" (Left, Right : Element) return Boolean;
   function ">"  (Left, Right : Element) return Boolean;
   function ">=" (Left, Right : Element) return Boolean;

   -- Utility
   function Modulus return Positive;
   procedure Put      (E : Element);
   procedure Put_Line (E : Element);

private

   type Element is record
      Value : Integer range 0 .. N - 1;
   end record;

end Ring;