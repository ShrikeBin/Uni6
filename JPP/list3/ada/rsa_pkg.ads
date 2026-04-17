-- rsa.ads
-- Generic RSA over an instantiated Ring package.
--
-- Formal parameters:
--   N        : the ring modulus (p * q)
--   Phi      : Euler totient of N, i.e. (p-1)*(q-1)
--   Ring_Pkg : the already-instantiated Ring package
--
-- The caller must guarantee that N = p*q for primes p, q,
-- and that Phi = (p-1)*(q-1).

with Ring;

generic
   P   : Positive;          -- first  prime
   Q   : Positive;          -- second prime
   -- We need the instantiated Ring for Ring<P*Q>; Ada generics cannot
   -- nest Ring instantiation here, so we pass the element type and ops
   -- via an inner generic.  Instead we keep it simple: instantiate Ring
   -- inside the package body with N => P*Q.
package RSA_Pkg is

   -- Concrete ring modulus
   N   : constant Positive := P * Q;
   Phi : constant Positive := (P - 1) * (Q - 1);

   -- We use Long_Long_Integer arithmetic internally; the "message" type
   -- visible to users is just a non-negative integer in [0, N-1].
   subtype Message is Natural;

   -- Initialise RSA: pick random e, compute d.
   -- Must be called before Encrypt / Decrypt.
   procedure Init;

   function  Get_Modulo     return Positive;
   function  Get_Public_Key return Positive;

   function  Encrypt (M : Message) return Message;
   function  Decrypt (S : Message) return Message;

end RSA_Pkg;