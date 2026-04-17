-- diffie_hellman.ads
-- Generic Diffie-Hellman key exchange over a prime field of characteristic P.

generic
   P : Positive;     -- prime field characteristic
package DH_Pkg is

   subtype Element is Natural;   -- values in [0, P-1]

   -- ── DHSetup ────────────────────────────────────────────────────────────────

   procedure DH_Init;              -- pick random generator
   function  Get_Generator return Element;
   function  Field_Power (A : Element; B : Long_Long_Integer) return Element;

   -- ── User ──────────────────────────────────────────────────────────────────

   type User is private;

   function  Make_User          return User;
   function  Get_Public_Key     (U : User) return Element;
   procedure Set_Key            (U : in out User; A : Element);
   function  Encrypt            (U : User; M : Element) return Element;
   function  Decrypt            (U : User; C : Element) return Element;

   -- Exceptions
   Key_Not_Set     : exception;
   No_Inverse      : exception;

private
   type User is record
      Secret      : Long_Long_Integer := 0;
      Shared_Key  : Long_Long_Integer := 0;
      Key_Is_Set  : Boolean           := False;
   end record;

end DH_Pkg;