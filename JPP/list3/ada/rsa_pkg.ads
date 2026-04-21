with Ring;
generic
   P   : Positive;
   Q   : Positive;
package RSA_Pkg is

   N   : constant Positive := P * Q;
   Phi : constant Positive := (P - 1) * (Q - 1);

   subtype Message is Natural;

   procedure Init;

   function  Get_Modulo     return Positive;
   function  Get_Public_Key return Positive;

   function  Encrypt (M : Message) return Message;
   function  Decrypt (S : Message) return Message;

end RSA_Pkg;